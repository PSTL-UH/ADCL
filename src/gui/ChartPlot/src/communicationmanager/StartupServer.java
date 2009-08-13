package communicationmanager;


import java.io.*;

import java.net.*;

import chartplot.Plot;

import eventpackage.EventGenerator;

public class StartupServer
{	
    static int numclients = 0;
    static int port       = 31500;

	EventGenerator generator = new EventGenerator();
	StartupConnection[] hosts;

    public EventGenerator getGenerator() 
    {
		return generator;
	}

	/**
     * The associated model
     */
    private ConfigurationModel _model;

    /**
     *   Constructor for StartupServer class.
     *  
     *   @param  model  current model
     */
    public StartupServer(ConfigurationModel model)
    {
        _model = model;
    }

    /**
     * Starts the startup-server.
     */
    public void start(String[] args) 
    {
        checkargs(args);
        _model.messageOccured("Waiting for " + numclients + " clients to connect\n");

        int                 i;
        hosts = new StartupConnection[numclients];

        for (i = 0; i < numclients; i++) {
            hosts[i] = new StartupConnection(_model);
        }

        try {
            ServerSocket ssocket = new ServerSocket(port, numclients);

            for (i = 0; i < numclients; i++) {
                hosts[i].init(ssocket);
                _model.messageOccured("Client " + i + " connected\n");
            }

            int totallength = 0;
            int length;

            for (i = 0; i < numclients; i++) {
                length = hosts[i].readmsglength();
                _model.messageOccured("Host " + i + " sends " + length + " bytes of data\n");
                totallength += length;
            }
           
            byte msgbuf[] = null;
            
            infinite:
            while(true)
            {
            	Header[] head = new Header[numclients];
                for (i = 0; i < numclients; i++) 
                {
                	head[i] = hosts[i].readHeader();      

                	if(head[i].get_magic() == StartupConnection.BNR_MAGIC)
                    {                		
                		if(head[i].get_type() == 2)
                    	{
                         	 break infinite;
                    	}
                		else if(head[i].get_type() == 1)
                		{                                			
                			msgbuf = new byte[head[i].get_len()];
                			int  offset   = 0;
                			int  ret;
                			ret    = hosts[i].readmsg(msgbuf,head[i].get_len(), offset);
                			offset += ret;
                		}
                     	raiseEvent(msgbuf, head[i]);
                     }                	
                }           
                msgbuf = null;               
            }
            
            msgbuf = new byte[32];

            for (i = 0; i < numclients; i++) 
            {
                hosts[i].sendmsg(msgbuf, 32);
            }

            ssocket.close();
        } catch (IOException e) {
            System.err.println(e);
            System.exit(-1);
        }
    }
    

    private void raiseEvent(byte[] msgbuf, Header head) 
    {
    	generator.dataReceived(msgbuf,head);
	}

	public void checkargs(String[] args) {
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

                    if (ret == 0) {
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

    public static int determinefirstrank(StartupConnection[] hosts) 
    {
        int i, j;
        int totalnodes = 0;

        for (i = 0; i < numclients; i++) 
        {
            for (j = 0; j < numclients; j++) 
            {
                if (hosts[j].rank == i) 
                {
                    hosts[j].firstnode = totalnodes;
                    totalnodes         += hosts[j].size;
                }
            }
        }

        return (totalnodes);
    }

	public void addRecvListener(Plot gui) 
	{
		generator.addRecvListener(gui);
	}
	
	public void disconnect()
	{
		for (int i = 0; i < numclients; i++) 
		{
            hosts[i].disconnect();
        }
	}

}
