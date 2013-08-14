package parser;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.StringTokenizer;

import datastructures.FunctionNameTable;
import datastructures.Sensitivity;
import datastructures.SpinList;
import datastructures.WinnerTable;
import eventpackage.CustomEventListener;
import eventpackage.EventGenerator;

public class OfflineParser
{
	public class OnlyOutputFiles implements FilenameFilter 
	{ 
		String ext; 
		public OnlyOutputFiles() { 
			this.ext = ".out"; 
		} 
		public boolean accept(File dir, String name) 
		{ 
			return name.endsWith(ext); 
		} 
	}

	private ArrayList<String> reqFuncPair;
	private String[] files;
	private Integer _processNumber;

	HashMap< Integer, Sensitivity > idToFunctionAvgTime;

	private EventGenerator generator;

	private String filePath;
	private String folderPath;

	private Boolean _stopReading;
	private FilenameFilter filter;
	
	private WinnerTable winnerTable;
	private FunctionNameTable functionNameTable;

	public void stopRead() 
	{
		this._stopReading = true;
	}

	public void addRecvListener(CustomEventListener gui) 
	{
		generator.addRecvListener(gui);
	}

	public void removeRecvListener(CustomEventListener gui)
	{
		generator.removeRecvListener(gui);
	}

	public OfflineParser() 
	{
		generator = new EventGenerator();
		idToFunctionAvgTime = new HashMap<Integer, Sensitivity>();
		reqFuncPair = new ArrayList<String>();
		_stopReading = false;
	    filter = new OnlyOutputFiles();
	    winnerTable = new WinnerTable();
	    functionNameTable = new FunctionNameTable();
	}

	public boolean setFolderPath(String path)
	{
		if(path.length() > 0)
		{
			folderPath = path;
		}
		listFilesFromPath(folderPath);
		if(files.length > 0)
			return true;
		else
			return false;		
	}

	public void resetParser() 
	{
		reqFuncPair.clear();
		idToFunctionAvgTime.clear();
		functionNameTable.clear();
		winnerTable.clear();
	}

	private void listFilesFromPath(String path)
	{
		File directory = new File(path);
		files = directory.list(filter);
	}

	public ArrayList<Integer> getProcessNumList()
	{
		ArrayList<Integer> processNumList = new SpinList();
		for(String file : files)
		{
			processNumList.add(extractProcessNumber(file));
		}
		Collections.sort(processNumList);
		return processNumList;
	}

	private int extractProcessNumber(String file)
	{
		return Integer.parseInt(file.substring(0, file.indexOf(".")));
	}

	public void startRead(int procNum)
	{
		Thread.currentThread().setPriority(Thread.MIN_PRIORITY); // this piece of code is so that the wait time at the start
		//before the user sees some output is less. application works without it too.
		
		synchronized (_stopReading) 
		{
			_stopReading = false;
		}		

		set_processNumber(procNum);

		filePath = folderPath + files[_processNumber];

		try 
		{
			FileInputStream fstream;
			DataInputStream in;
			BufferedReader br;
			fstream = new FileInputStream(filePath);
			in = new DataInputStream(fstream);
			br = new BufferedReader(new InputStreamReader(in));

			String strLine = null;

			synchronized (_stopReading) 
			{
				while (!_stopReading && (strLine = br.readLine()) != null)
				{
					parseAndFireEvent(strLine);            //launch event to be caught by recvReceived.
				}

				if(!_stopReading && strLine == null)
				{
					System.out.println("read the whole file");
					for(Integer key : idToFunctionAvgTime.keySet())
					{
						generator._fireSensitivityEvent(this, key, idToFunctionAvgTime.get(key).getSensitivity());
					}
					
					String dominantWinnerMessage =  winnerTable.analysisOfWinnerFunctions(functionNameTable);
					generator._fireAnalysisEvent(this, dominantWinnerMessage);
				}					
				
				stopRead(fstream, in , br);
			}

		} 
		catch (Exception e) 
		{
			e.printStackTrace();
		}
	}

	private void set_processNumber(int procNum) 
	{
		if(procNum == -1)
		{
			_processNumber = extractProcessNumber(files[0]);
		}
		else
		{
			_processNumber = procNum;
		}
	}

	private void stopRead(FileInputStream fstream, DataInputStream in, BufferedReader br) throws IOException 
	{
		br.close();
		in.close();
		fstream.close();
		br = null;
		in = null;
		fstream = null;
		resetParser();
	}

	private void parseAndFireEvent(String strLine)  // instead of making the parsing absolute using numbers 
	//use indexOf(String) function to find the position of the string and then relative to it parse the line
	{		
		//using string tokenizer to remove the extra spaces in between and replace them with single space.
		StringTokenizer stringTokens = new StringTokenizer(strLine," ",false);
		String trimmedString="";
		while (stringTokens.hasMoreElements()) 
		{
			trimmedString += stringTokens.nextElement() + " ";
		}
		trimmedString.trim();

		if(trimmedString.charAt(0) != '#')
		{
			boolean exists = false;

			exists = verifyFunctionChange(new StringTokenizer(strLine," ",false), exists);
			//checking if the function has changed. if so fire a function change event.
			if(!exists)
			{
				fireFunctionChange(new StringTokenizer(strLine," ",false));
			}

			// points event needs to be fired.
			firePointsEvent(new StringTokenizer(strLine," ",false));

		}	
		else
		{
			// this is the part when winner is decided.
			if(trimmedString.contains("winner") && trimmedString.contains("req"))
			{
				fireWinnerDecided(new StringTokenizer(strLine," ",false));			
			}			
		}
	}

	private boolean verifyFunctionChange(StringTokenizer tokens, boolean exists) 
	{
		int id = 0, funcId = 0;
		while(tokens.hasMoreTokens())
		{
			String token = tokens.nextToken();
			if( token.equalsIgnoreCase("request"))
			{
				id = Integer.parseInt(tokens.nextToken());
			}			
			if(token.equalsIgnoreCase("method"))
			{
				funcId = Integer.parseInt(tokens.nextToken());
			}
		}
		for(String val : reqFuncPair)
		{
			if(val == id+" "+funcId)
			{
				exists = true;
			}				
		}
		return exists;
	}

	private void fireWinnerDecided(StringTokenizer tokens) {
		int id = 0;
		int funcId = 0;
		int fnctsetId = 0;
		String objName = null;
		String fnctsetName = null;
		boolean fnctsetNameEnds = false;
		while(tokens.hasMoreTokens())
		{
			String token = tokens.nextToken();
			if(token.equalsIgnoreCase("req") || token.equalsIgnoreCase("emethod"))
			{
				objName = token;
				id = Integer.parseInt(tokens.nextToken());
			}			
			else if(token.equalsIgnoreCase("function") && tokens.nextToken().equalsIgnoreCase("set"))
			{
				fnctsetId = Integer.parseInt(tokens.nextToken());
				fnctsetName = tokens.nextToken();
			}
			else if( token.equalsIgnoreCase("winner"))
			{
				fnctsetNameEnds = true;
				tokens.nextToken();
				funcId = Integer.parseInt(tokens.nextToken());
			}
			else if(fnctsetName != null && !fnctsetNameEnds)
			{
				fnctsetName = fnctsetName+" "+token;
			}			
		}
		
		for(int i=objName.length();i<8;i++)
		{
			objName = objName + " ";
		}

		generator._fireWinnerDecidedEvent(this, id, fnctsetId, fnctsetName, funcId, objName);
		winnerTable.addInfoToTable(id, fnctsetId, fnctsetName, funcId);
	}

	private void firePointsEvent(StringTokenizer tokens) {
		int id = 0;
		double yval = 0.0;
		String lastStr = null;
		while(tokens.hasMoreTokens())
		{
			lastStr = tokens.nextToken();
			if(lastStr.equalsIgnoreCase("request"))
			{
				id = Integer.parseInt(tokens.nextToken());
			}
		}
		yval = Double.parseDouble(lastStr);
		
		generator._firePointsEvent(this, id, yval);
		ifNewFunction(id);
		idToFunctionAvgTime.get(id).addTime(yval);
	}

	private void fireFunctionChange(StringTokenizer tokens) 
	{
		int id = 0;
		int funcId = 0;
		String funcName = "";
		while(tokens.hasMoreTokens())
		{
			String token = tokens.nextToken();
			if(token.equalsIgnoreCase("request"))
			{
				id = Integer.parseInt(tokens.nextToken());
			}
			if(token.equalsIgnoreCase("method"))
			{
				funcId = Integer.parseInt(tokens.nextToken());
				funcName = tokens.nextToken();
			}
		}
		String reqFunc = id+" "+funcId; 
		reqFuncPair.add(reqFunc);
		
		funcName = funcName.substring(1,funcName.length() - 1);
		functionNameTable.addInfoToTable(id, funcId, funcName);
		generator._fireFunctionChangeEvent(this, id, funcId, funcName);
		ifNewFunction(id);
		idToFunctionAvgTime.get(id).newFunctionTime();
	}

	private void ifNewFunction(int id)
	{
		if(!idToFunctionAvgTime.containsKey(id))
		{
			idToFunctionAvgTime.put(id, new Sensitivity());
		}	
	}

	public Integer getCurrentProcess() {
		return _processNumber;
	}

	public void setCurrentProcess(int procNum) 
	{
		_processNumber = procNum;
	}

	public Boolean getStopRead()
	{
		return _stopReading;
	}
}
