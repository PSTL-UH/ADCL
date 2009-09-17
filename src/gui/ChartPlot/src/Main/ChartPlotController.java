package Main;

import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import chartplot.Plot;

import communicationmanager.StartupServer;

public class ChartPlotController implements MouseListener
{
	StartupServer _server;
	Plot _plot;
	Dimension _windowSize;
	boolean restart = false;
	
	public boolean isRestart() {
		return restart;
	}

	public void setRestart(boolean restart) {
		this.restart = restart;
	}

	public ChartPlotController(Plot plot, StartupServer server, Dimension windowSize) 
	{
		_plot = plot;
		_server = server;
		_windowSize = windowSize;
		
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

}
