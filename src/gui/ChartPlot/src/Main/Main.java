package Main;
import java.awt.Dimension;

import javax.swing.JFrame;

import communicationmanager.*;


import chartplot.Plot;


public class Main
{
	public static void main(String[] args) 
	{	
	    Dimension windowSize = new Dimension(820,720);
	    
	    initialize(windowSize);	                
	}

	private static void initialize(Dimension windowSize) 
	{    		
		Plot plot = new Plot(windowSize);
		StartupServer server = new StartupServer();
		ChartPlotController controller = new ChartPlotController(plot,server,windowSize);
		
		setup(plot, server, controller);
	    
		while(true)
		{		
			if(controller.isRestart())
			{			
				System.out.println("restarting");
				controller.setRestart(false);
			    plot.startOver();
			    plot.setVisible(false);
			    plot.dispose();
			    plot = null;
			    server.disconnect();
			    
			    System.gc();      
			    
			    plot = new Plot(windowSize);
			    setup(plot, server, controller);
			}				
		}		    
	}

	private static void setup(Plot plot, StartupServer server,ChartPlotController controller) 
	{
		plot.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);	   
		plot.setVisible(true);
		plot.setController(controller);
    
		String serverArgs[] = {"-n","1","-p","20000"};
	    server.addRecvListener(plot); //define addrecvlistener in startupserver. put the generator code in it.
	    server.start(serverArgs);
		
	}
}
