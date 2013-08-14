package chartplot;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JSpinner;
import javax.swing.JToggleButton;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import communicationmanager.StartupServer;

import parser.OfflineParser;

public class ChartPlotController implements MouseListener, ActionListener, ChangeListener
{
	public enum modes { online, offline	};

	private StartupServer _server;
	private Plot _plot;
	private OfflineParser _parser;
	
	private boolean begin = false;
	modes onOffMode;
	private int procNum = -1;
	JFileChooser chooser;
	String _args[];

	public boolean getBegin() 
	{
		return begin;
	}	

	public modes getOnOffMode() 
	{
		return onOffMode;
	}

	private void toggleOnOffMode(JToggleButton onoffButton) 
	{
		if(this.onOffMode == modes.online)
		{
			this.onOffMode = modes.offline;
			onoffButton.setText("Offline");
		}
		else
		{
			this.onOffMode = modes.online;
			onoffButton.setText("Online");
		}
	}

	public int getProcNum()
	{
		if(onOffMode == modes.online)
			return _server.getCurrentProcess();
		else
			return _parser.getCurrentProcess();
	}

	public void setProcNum(int procNumber)
	{
		procNum = procNumber;
	}

	public ChartPlotController(Dimension windowSize, String[] args) 
	{
		_args = args;
		_plot = new Plot(windowSize);
		_server = new StartupServer();
		_parser = new OfflineParser();
		initDirectoryChooser();
		onOffMode = modes.online;
	}		

	//invoked when user changes the current process to display
	public void stateChanged(ChangeEvent arg) 
	{
		JSpinner spinner = (JSpinner)arg.getSource();
		spinner.setEnabled(false);
		Integer spinValue = (Integer)spinner.getValue();
		resetToNewProc(spinValue);
	}

	//clear out the view and reload with the data for the new process.
	public void resetToNewProc(int spinValue)
	{
		setProcNum(spinValue);
		_plot.hideTab();
		//using thread hear so that the program doesn't freeze for an extended period
		new Thread() 
		{
			public void run() 
			{
				if(getOnOffMode() == modes.offline)
				{
					_parser.removeRecvListener(_plot);
					stopRead();
					_plot.startOver();
					synchronized (_parser.getStopRead()) 
					{
						_parser.addRecvListener(_plot);			
						_parser.startRead(procNum);
					}
				}
				else
				{
					synchronized (_server.getCurrentProcess()) //if application is not working comment this statement.
					{
						_plot.startOver();
						_server.setCurrentProcess(procNum);
					}
				}
			}
		}.start();
	}

	public void mouseClicked(MouseEvent event)
	{		
		if(event.getComponent().getName() == "Stop")
		{
			stopRead();
		}
		else if(event.getComponent().getName() == "Start Over")
		{
			System.out.println("Start over");

			new Thread() 
			{
				public void run() 
				{
					begin = false;
					restart();

				}
			}.start();
		}
	}

	private void stopRead() 
	{
		try
		{
			if(onOffMode == modes.online)
				_server.stopRead();
			else
				_parser.stopRead();			
		}
		catch (IOException e) 
		{
			e.printStackTrace();
		}
	}

	public void mouseEntered(MouseEvent arg0) 
	{		
	}

	public void mouseExited(MouseEvent arg0) 
	{
	}

	public void mousePressed(MouseEvent arg0) 
	{
	}

	public void mouseReleased(MouseEvent arg0) 
	{		
	}

	public void actionPerformed(ActionEvent e) 
	{
		System.out.println(e.getActionCommand());
		String action = e.getActionCommand();
		
		if( action == "Start")
		{
			new Thread() 
			{
				public void run() 
				{
					_plot.hideButtons();
					begin = true;
					setup();
				}
			}.start();
		}
		else if( action == "On/Off" )
		{
			this.toggleOnOffMode((JToggleButton)e.getSource());
		}
	}


	public void restart()
	{
		System.out.println("restarting");
		_server.removeRecvListener(_plot);
		_parser.removeRecvListener(_plot);
		_plot.startOver();
		_plot.showButtons();
		stopRead();

		System.gc();      
		setup();
	}

	private void setup() 
	{		
		if(begin == true)
		{
			if(getOnOffMode() == modes.online)
			{
				onlineSetup();
			}
			else
			{
				offlineSetup();
			}
		}		
		else
		{
			_plot.setOnOffButton(onOffMode);
			_plot.setVisible(true);
			_plot.paint(_plot.getGraphics());
		}
	}

	private void offlineSetup() 
	{
		String directory = showDirectoryChooser();
		boolean success = false;
		if(directory.length() > 0)
			success = _parser.setFolderPath(directory+"\\");
		if(success)
		{
			_parser.addRecvListener(_plot);
			_parser.startRead(procNum);
		}
		else
		{
			JOptionPane.showMessageDialog(_plot, "No output files to read from.");
			_plot.showButtons();
			begin = false; // case where a wrong directory is entered and then a right one
			// double values are being sent to the plot which means two threads are running. - seems to be fixed
		}
	}

	private void onlineSetup() 
	{
		//String serverArgs[] = {"-n","1","-p","20000"};
		_server.addRecvListener(_plot);
		//the eventsupply function can also be called here. 
		//the response time didnt really change with the location of the code.
		_server.startRead(_args);
	}

	public void init() 
	{
		_plot.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);	   
		_plot.setController(this);

		setup();
	}

	public ArrayList<Integer> getProcessNumList() 
	{
		if(onOffMode == modes.offline)
			return _parser.getProcessNumList(); 
		else
			return _server.getProcessNumList(); 
	}
	
	private void initDirectoryChooser()
	{
		chooser = new JFileChooser(); 
	    chooser.setCurrentDirectory(new java.io.File("."));
	    chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	    chooser.setAcceptAllFileFilterUsed(false);
	}
	
	public String showDirectoryChooser() 
	{				
	    int returnValue = chooser.showOpenDialog(_plot);
	    if(returnValue == JFileChooser.APPROVE_OPTION)
	    {
		    return chooser.getSelectedFile().toString();
	    }	    
	    else
	    	return "";
	}
}
