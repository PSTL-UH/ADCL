package Main;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JFrame;

import parser.OfflineParser;

import chartplot.Plot;

import communicationmanager.StartupServer;

public class ChartPlotController implements MouseListener, ActionListener
{
	StartupServer _server;
	Plot _plot;
	OfflineParser _parser;
	Dimension _windowSize;
	boolean restart = false;
	boolean begin = false;
	
   public enum modes { online, offline};
	
	modes onOffMode;
	int procNum = 0;
	
	public int getNumberOfProcs()
	{
		return _parser.getNumberOfProcs();
	}
	
	public modes getOnOffMode() {
		return onOffMode;
	}

	public void toggleOnOffMode() 
	{
		if(this.onOffMode == modes.online)
		{
			this.onOffMode = modes.offline;
			//restart = true;
		}
		else
		{
			this.onOffMode = modes.online;
			//restart = true;
		}
	}
	
	public boolean isRestart() {
		return restart;
	}

	public void setRestart(boolean restart) {
		this.restart = restart;
	}

	public ChartPlotController(Dimension windowSize) 
	{

		_plot = new Plot(windowSize);
		_server = new StartupServer();
		_parser = new OfflineParser(procNum);
		
		_windowSize = windowSize;
		onOffMode = modes.online;
		
	}	
	
	public int getProcNum()
	{
		return procNum;
	}
	
	public void setProcNum(int procNumber)
	{
		procNum = procNumber;
	}
	
	@Override
	public void mouseClicked(MouseEvent event)
	{		
		if(event.getComponent().getName() == "Disconnect")
		{
			_server.disconnect();
			System.out.println("disconnect");
		}
		else if(event.getComponent().getName() == "Start Over")
		{
			System.out.println("Start over");

		    restart = true;
		    begin = false;
			onOffMode = modes.online;
		}
	}

	@Override
	public void mouseEntered(MouseEvent arg0) 
	{		
	}

	@Override
	public void mouseExited(MouseEvent arg0) 
	{
	}

	@Override
	public void mousePressed(MouseEvent arg0) 
	{
	}

	@Override
	public void mouseReleased(MouseEvent arg0) 
	{		
	}

	public void actionPerformed(ActionEvent e) 
	{
		System.out.println(e.getActionCommand());
		if(e.getActionCommand() == "On/Off")
		{
			this.toggleOnOffMode();
		}
		else if(e.getActionCommand() == "Start")
		{
			restart = true;
			begin = true;
		}
	}
	

	private void restart()
	{
		System.out.println("restarting");
		restart = false;
	    _server.removeRecvListener(_plot);
	    _parser.removeRecvListener(_plot);
		_plot.startOver();
		_plot.setVisible(false);
		_plot.dispose();
		_plot = null;
	    _server.disconnect();
	    _parser = null;
		
		System.gc();      
		
		_plot = new Plot(_windowSize);
		_parser = new OfflineParser(procNum);
		setup();
	}

	private void setup() 
	{
		_plot.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);	   
		_plot.setController(this);
		_plot.setVisible(true);
		_plot.paint(_plot.getGraphics());
		
		if(begin == true)
		{
			start();
		}		
	}
	
	public boolean getBegin() {
		return begin;
	}

	private void start()
	{
		if(getOnOffMode() == modes.online)
		{
			String serverArgs[] = {"-n","1","-p","20000"};
		    _server.addRecvListener(_plot); //define addrecvlistener in startupserver. put the generator code in it.
		    _server.start(serverArgs);
		}
		else
		{
			_parser.addRecvListener(_plot);
			_parser.startRead();
		}
	}

	public void init() 
	{
        setup();
	    
		while(true)
		{		
			if(isRestart())
			{			
				restart();
			}				
		}		
		
	}

}
