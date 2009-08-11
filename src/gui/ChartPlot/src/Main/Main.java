package Main;
import javax.swing.JFrame;

import communicationmanager.*;


import chartplot.Plot;


public class Main
{
	public static void main(String[] args) 
	{	
	    	
	    ConfigurationModel config = new ConfigurationModel();
	        
	    StartupServer server = new StartupServer(config);

		Plot gui = new Plot(server);
	    gui.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    gui.setSize(640, 680);
	    gui.validate();
	    gui.setVisible(true);
	    
	    String serverArgs[] = {"-n","1","-p","20000"};
	    server.addRecvListener(gui); //define addrecvlistener in startupserver. put the generator code in it.
	    server.start(serverArgs);	                
	}
}
