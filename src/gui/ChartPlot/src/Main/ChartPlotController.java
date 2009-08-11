package Main;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import chartplot.Plot;

import communicationmanager.StartupServer;

public class ChartPlotController implements ActionListener
{
	StartupServer _server;
	Plot _plot;
	
	public ChartPlotController(Plot plot, StartupServer server) 
	{
		_plot = plot;
		_server = server;
	}

	@Override
	public void actionPerformed(ActionEvent e) 
	{
		_server.disconnect();
	}

}
