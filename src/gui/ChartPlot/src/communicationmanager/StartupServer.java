package communicationmanager;


import java.io.*;

import java.net.*;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.channels.CancelledKeyException;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EventObject;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import utility.Utility;
import datastructures.FunctionNameTable;
import datastructures.Header;
import datastructures.ProcessDataStore;
import datastructures.Sensitivity;
import datastructures.SpinList;
import datastructures.WinnerTable;
import datastructures.ProcessDataStore.ProcessCommStatus;

import eventpackage.*;


public class StartupServer
{	
	class CustomThread extends Thread
	{
		private volatile boolean stop = false;
		
		public void requestStop()
		{
			stop = true;
		}
		
		public void run()
		{
			System.out.println ("event loop thread created");
			while(!stop)
			{
				synchronized (getCurrentProcess())
				{
					try
					{
						EventObject event = getNextEvent();
						if(event != null)
						{
							if(event instanceof EndEvent)
							{
								System.out.println("Finished reading event information");
								//break; // we should not be breaking out as it would stop working 
								//after the completion of displaying a single process's information
							}
							generator.callHandler(event);
						}
						else
						{
							Thread.yield();
						}
					}
					catch(NullPointerException ne)
					{
						Thread.yield();
					}
				}
			}
			System.out.println("ending a thread");
		}		
	}
	
	private final static int BNR_MAGIC = 918273;

	public enum message_types {Points, Message, FunctionChange, WinnerDecided, EndOfComm};

	static int numclients = 0;
	static int port       = 31500;    

	Utility util = Utility.getInstance();

	private ConfigurationModel _model;
	
	//generator not needed if the while loop in the separate thread is in controller
	EventGenerator generator;

	private int numberOfConnectedProcs;
	private ServerSocketChannel ssc;
	private Selector selector;	
	private final ByteBuffer channelBuffer;

	private boolean serverStarted = false;

	private SpinList processNumList;
	private Integer currentProcess;	
	
	CustomThread thread = null;
	
	private WinnerTable winnerTable;
	private FunctionNameTable functionNameTable;
	HashMap< Integer, ProcessDataStore > rankToProcessData;	

	public HashMap<Integer, ProcessDataStore> getRankToProcessData() 
	{
		return rankToProcessData; 
	}

	HashMap< SocketAddress, ProcessDataStore > idToProcessData;

	public StartupServer()
	{
		_model = new ConfigurationModel();
		channelBuffer = ByteBuffer.allocate( 262144 );
		generator = new EventGenerator();
		
		currentProcess = 0;
		numberOfConnectedProcs = 0;
		processNumList = new SpinList();
		rankToProcessData = new HashMap<Integer, ProcessDataStore>(); // this is for use by displaying code
		idToProcessData = new HashMap<SocketAddress, ProcessDataStore>(); // this is to store the incoming data
		winnerTable = new WinnerTable();
		functionNameTable = new FunctionNameTable();
	}

	public EventGenerator getGenerator() 
	{
		return generator;
	}

	public void startRead(String[] args) 
	{
		serverStarted = true;
		checkargs(args);
		//_model.messageOccured("Waiting for " + numclients + " clients to connect\n");

		try 
		{
			ServerSocket ss = socketChannelSetup();
			eventSupply();
			socketPolling(ss);
		} 
		catch (IOException e) 
		{
			_model.messageOccured(e + "\n");
			System.exit(-1);
		}
	}

	private void eventSupply() 
	{
		if(thread != null && thread.isAlive())
		{
			thread.requestStop();
		}
				
		thread = new CustomThread();
		thread.start();
	}

	private ServerSocket socketChannelSetup() throws IOException,
	ClosedChannelException {
		ssc = ServerSocketChannel.open();

		ssc.configureBlocking( false );
		ServerSocket ss = ssc.socket();
		InetSocketAddress isa = new InetSocketAddress( port );
		ss.bind( isa );

		selector = Selector.open();
		ssc.register( selector, SelectionKey.OP_ACCEPT );
		_model.messageOccured( "Listening on port "+port );
		return ss;
	}    

	private void socketPolling(ServerSocket ss) throws IOException 
	{
		do 
		{
			int num = selector.select();

			if (num == 0) 
			{
				continue;
			}

			Set<SelectionKey> keys = selector.selectedKeys();
			Iterator<SelectionKey> it = keys.iterator();
			try
			{
				while (it.hasNext())
				{
					synchronized (selector)
					{
						SelectionKey key = (SelectionKey)it.next();

						if ((key.readyOps() & SelectionKey.OP_ACCEPT) == SelectionKey.OP_ACCEPT) 
						{
							_model.messageOccured( "accepting connection" );

							Socket s = ss.accept();
							_model.messageOccured( "Got connection from "+s.getRemoteSocketAddress() );

							SocketChannel sc = s.getChannel();
							sc.configureBlocking( false );
							sc.register( selector, SelectionKey.OP_READ );
							numberOfConnectedProcs++;
						} 
						else if ((key.readyOps() & SelectionKey.OP_READ) == SelectionKey.OP_READ) 
						{	
							SocketChannel socketChannel = null;

							try 
							{	
								socketChannel = (SocketChannel)key.channel();
								boolean dataRead = parseDataAndStore(socketChannel);

								if (!dataRead)
								{
									key.cancel();
									socketClose(socketChannel);
									_model.messageOccured("Closed "+socketChannel);
								}
							}
							catch( IOException ie ) 
							{	
								// On exception, remove this channel from the selector
								key.cancel();
								socketClose(socketChannel);
								_model.messageOccured( "Closed "+socketChannel );
							}
						}
					}
				}
			}
			catch(CancelledKeyException cke)
			{
				break;
			}
			keys.clear();
		}while(numberOfConnectedProcs > 0 && serverStarted);
	}

	private boolean parseDataAndStore(SocketChannel socketChannel) throws IOException
	{
		channelBuffer.clear();
		socketChannel.read( channelBuffer );
		channelBuffer.flip();
		Socket socket = socketChannel.socket();

		if (channelBuffer.limit()==0)
		{
			return false;
		}

		SocketAddress procID = socket.getRemoteSocketAddress();

		if( !idToProcessData.containsKey(procID))
		{
			newProcessData(procID);
		}
		else
		{
			int rest = channelBuffer.limit();
			ProcessDataStore procData = idToProcessData.get(procID);
			rest = rest + procData.getPosition();
			while(rest > 0)
			{				
				HashMap<Integer, Sensitivity> idToFuncTime = procData.getIdToFunctionAvgTime();
				
				if(procData.getCommStatus() == ProcessCommStatus.Head && rest >= Header.HEADER_MESSAGE_SIZE)
				{
					byte[] headerBuff = new byte[Header.HEADER_MESSAGE_SIZE];
					try
					{
						readBytesFromBuffer(procData, headerBuff);
					}
					catch(BufferUnderflowException be)
					{
						be.printStackTrace();
						break;
					}

					Header head = new Header(headerBuff);    
					
					if(!idToFuncTime.containsKey(head.get_id()))
					{
						idToFuncTime.put(head.get_id(), new Sensitivity());
					}							

					if(message_types.values()[head.get_type()] == message_types.Message ||
							message_types.values()[head.get_type()] == message_types.FunctionChange || 
							message_types.values()[head.get_type()] == message_types.WinnerDecided)
					{
						procData.setCommStatus(ProcessCommStatus.Msg);
						procData.setLastHeader(head);
					}        		
					else if(message_types.values()[head.get_type()] == message_types.EndOfComm)
					{
						_model.messageOccured("end of comm message received");

						channelBuffer.clear();
						byte[] msgbuf = new byte[32];
						channelBuffer.put(msgbuf);
						socketChannel.write(channelBuffer);
						numberOfConnectedProcs--;
						//compute the sentivity for each request and add the events for that to the event list
						//before end event
						for(Integer key : idToFuncTime.keySet())
						{
							procData.addEvent(new SensitivityEvent(this, key, idToFuncTime.get(key).getSensitivity()));
						}
						procData.addEvent(new AnalysisEvent(this, winnerTable.analysisOfWinnerFunctions(functionNameTable)));
						procData.addEvent(new EndEvent(this));
						return false;
					}
					else
					{
						PointsEvent ptEvent = new PointsEvent(this, head.get_id(), head.get_yval());
						procData.addEvent(ptEvent);				
						idToFuncTime.get(head.get_id()).addTime(head.get_yval());
					}
					rest = rest - Header.HEADER_MESSAGE_SIZE;
				}
				else if(procData.getCommStatus() == ProcessCommStatus.Msg && rest >= procData.getLastHeader().get_len())
				{
					Header head = procData.getLastHeader();

					if(head.get_magic() == BNR_MAGIC)
					{ 
						byte[] msgbuf = new byte[head.get_len()];	

						try
						{
							readBytesFromBuffer(procData, msgbuf);
						}
						catch(BufferUnderflowException be)
						{
							be.printStackTrace();
							break;
						}

						if(message_types.values()[head.get_type()] == message_types.FunctionChange)
						{
							parseAndStoreFunctionChange(procData, head, msgbuf);
							
							idToFuncTime.get(head.get_id()).newFunctionTime();
						}
						else if(message_types.values()[head.get_type()] == message_types.WinnerDecided)
						{
							parseAndStoreWinnerDecided(procData, head, msgbuf);
						}
						else
						{
							parseAndStoreMessageReceived(procData, head, msgbuf);
						}			
						rest = rest - head.get_len();
						procData.setCommStatus(ProcessCommStatus.Head);		
					}
				}
				else
				{
					byte[] restBuff = new byte[rest];
					channelBuffer.get(restBuff, 0,restBuff.length);
					procData.setUnparsedBytes(restBuff);
					break;
				}
			} 
		}

		return true;
	}

	private void parseAndStoreMessageReceived(ProcessDataStore procData,
			Header head, byte[] msgbuf) {
		MessageReceivedEvent messageReceived = new MessageReceivedEvent(this, head.get_id(), new String(msgbuf));
		procData.addEvent(messageReceived);
	}

	private void parseAndStoreWinnerDecided(ProcessDataStore procData,
			Header head, byte[] msgbuf) {
		int fnctsetId = util.byteArrayToInt(msgbuf, 40);
		int functionId = util.byteArrayToInt(msgbuf, 44);
		String entire = new String(msgbuf);
		String objectName = entire.substring(0, 7);
		String fnctsetName = entire.substring(8,39);
		fnctsetName = fnctsetName.replaceAll("[^A-Za-z\\_\\s]", " ");
		fnctsetName = fnctsetName.trim();
		WinnerDecidedEvent winnerDecided = new WinnerDecidedEvent(this, head.get_id(), fnctsetId, fnctsetName, functionId, objectName);
		procData.addEvent(winnerDecided);
		winnerTable.addInfoToTable(head.get_id(), fnctsetId, fnctsetName, functionId);
	}

	private void parseAndStoreFunctionChange(ProcessDataStore procData,
			Header head, byte[] msgbuf) {
		int functionId = util.byteArrayToInt(msgbuf, 0);
		String message = new String(msgbuf);
		message = message.substring(4);
		message = message.replaceAll("[^A-Za-z\\_\\s]", " ");
		if( message.indexOf(" ") > 0 )
		{
			message = message.substring(0, message.indexOf(" "));
		}
		FunctionChangeEvent funcChange = new FunctionChangeEvent(this, head.get_id(), functionId, message);
		functionNameTable.addInfoToTable(head.get_id(), functionId, message);
		procData.addEvent(funcChange);
	}

	private void newProcessData(SocketAddress procID) 
	{
		ProcessDataStore procDataStore = new ProcessDataStore();
		idToProcessData.put(procID, procDataStore);
		int rank = channelBuffer.getInt();
		rankToProcessData.put( rank, procDataStore );
		processNumList.add(rank);
		if(idToProcessData.size() == 1)
		{
			currentProcess = rank;
		}
		Collections.sort(processNumList);
	}

	private void readBytesFromBuffer(ProcessDataStore procData, byte[] buff) 
	{
		if(procData.getPosition() > 0)
		{
			procData.getUnparsedBytes(buff);
			int position = procData.getPosition();
			channelBuffer.get(buff, position,buff.length - position);
			procData.resetPosition();
		}
		else
		{
			channelBuffer.get(buff, 0,buff.length);
		}
	}

	public void checkargs(String[] args) 
	{
		String option;
		int    i       = 0;
		int    numargs = args.length;
		int    ret;

		if (args.length <= 0) 
		{
			_model.messageOccured("USAGE: StartupServer -n <NumberOfClients> -p <port>");
			System.exit(-1);
		}
		else 
		{
			try {
				while (numargs > 0) {
					option = args[i];
					numargs--;
					i++;
					ret = option.compareTo("-n");

					if (ret == 0) {
						numclients = Integer.parseInt(args[i]);
						numargs--;
						i++;
					}

					ret = option.compareTo("-p");

					if (ret == 0) {
						port = Integer.parseInt(args[i]);
						numargs--;
						i++;
					}

					ret = option.compareTo("-h");

					if (ret == 0) 
					{
						_model.messageOccured("USAGE: StartupServer -n <NumberOfClients>\n");
						_model.messageOccured(" -p <port>");
						System.exit(-1);
					}
				}
			} 
			catch (NumberFormatException e) 
			{
				_model.messageOccured(e + "\n");
			}
		}

		if (numclients == 0)
		{
			_model.messageOccured("USAGE: StartupServer -n <NumberOfClients> -p <port>\n");
			System.exit(-1);
		}
	}

	public void addRecvListener(CustomEventListener gui)
	{
		generator.addRecvListener(gui);
	}

	public void removeRecvListener(CustomEventListener gui)
	{
		generator.removeRecvListener(gui);
	}

	private void socketClose(SocketChannel socketChannel) throws IOException 
	{
		socketChannel.socket().close();
	}

	public void stopRead() throws IOException
	{
		if(serverStarted)
		{
			//fix disconnect
			Set<SelectionKey> keys = selector.keys();
			Iterator<SelectionKey> it = keys.iterator();
			synchronized (selector) 
			{
				while(it.hasNext())
				{
					SelectionKey key = (SelectionKey)it.next();
					if(key.isValid())
					{
						if((key.readyOps() & SelectionKey.OP_READ) == SelectionKey.OP_READ)
						{
							SocketChannel socketChannel = (SocketChannel) key.channel();
							socketClose(socketChannel);
						}
						key.cancel();
						numberOfConnectedProcs--; // find out what numberOfConnectedProcs finally becomes. 
												  //whether it goes to 0 or not
					}
				}
				ssc.close();			
				serverStarted = false;
			}

			resetStartupServerData();
		}
	}

	private void resetStartupServerData() 
	{
		synchronized (currentProcess) 
		{	
			//need to test this out. added the for loop so that there is no memory leak.
			for( ProcessDataStore dataStoreObject : idToProcessData.values())
			{
				dataStoreObject.clearData();
			}
			idToProcessData.clear();
			rankToProcessData.clear();
			processNumList.clear();
			currentProcess = 0;
			numberOfConnectedProcs = 0;
		}
	}

	public ArrayList<Integer> getProcessNumList()
	{
		return processNumList;
	}

	public Integer getCurrentProcess()
	{
		return currentProcess;
	}

	public void setCurrentProcess(int processNum)
	{
		currentProcess = processNum;	
		rankToProcessData.get(currentProcess).resetPreviousLast();
	}

	public EventObject getNextEvent() 
	{
		ProcessDataStore processData;
		EventObject event = null;
		if(rankToProcessData.containsKey(currentProcess))
		{
			processData = rankToProcessData.get(currentProcess);
			event = processData.getEvent();
		}
		return event;		
	}
}
