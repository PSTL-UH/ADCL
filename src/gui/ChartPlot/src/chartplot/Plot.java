
package chartplot;

/**
 *
 * @author sarat
 */

import java.awt.*;
import java.util.*;
import java.math.*;
import javax.swing.*;

import layout.TableLayout;

import chartplot.ChartPlotController.modes;

import eventpackage.*;
import plot.*;
import utility.Utility;

@SuppressWarnings("serial")
public final class Plot extends JFrame implements CustomEventListener
{
	class TabStatus
	{
		HashMap<Integer, Integer> funcNumToFuncId;
		public TabStatus(Graph tab, int iteration) 
		{			
			super();
			this.tab = tab;
			this.iteration = iteration;
			funcNumToFuncId = new HashMap<Integer, Integer>();
		}

		Graph tab;
		public void setTab(Graph tab) {
			this.tab = tab;
		}
		public Graph getTab() {
			return tab;
		}
		int iteration;
		public int getIteration() {
			return iteration;
		}
		public void setIteration(int iteration) {
			this.iteration = iteration;
		}

		public HashMap<Integer, Integer> getFuncIdToFuncNum() {
			return funcNumToFuncId;
		}		

		int functionNumber = 0;
		public int getFunctionNumber()
		{
			return functionNumber;
		}
		public void setFunctionNumber(int functionNumber) {
			this.functionNumber = functionNumber;
		}
		public void addFunctionId(int functionId) 
		{
			funcNumToFuncId.put(Integer.valueOf(functionId), Integer.valueOf(functionNumber));
			functionNumber++;
		}

		public boolean functionIdExists(int functionId)
		{
			return funcNumToFuncId.containsKey(functionId);
		}

		public void incrementIterator() 
		{
			iteration++;
		}

		public int getFuncNumFromId(int functionId) 
		{
			return funcNumToFuncId.get(functionId);
		}
		
		TextArea _messageBox = null;
		
		public void setMessageBox(TextArea messageBox)
		{
			_messageBox = messageBox;
		}
		
		public TextArea getMessageBox()
		{
			return _messageBox;
		}
		
		Legend _legend = null;
		
		public void setLegend(Legend legend) 
		{
			_legend = legend;
		}
		
		public Legend getLegend()
		{
			return _legend;
		}
	}

	private Color color[]={Color.GREEN,Color.BLACK,Color.RED,Color.BLUE,Color.CYAN,Color.YELLOW,Color.GRAY,Color.PINK,Color.ORANGE,Color.WHITE,Color.MAGENTA,Color.LIGHT_GRAY};
	private XAxis xAxis;
	private YAxis yAxis;// make changes in axisinstance in the paint function
	private final ArrayList<Graph> graph;
	private boolean init;
	private static final Cursor ACTION_CURSOR = new Cursor(Cursor.HAND_CURSOR);

	Utility util = Utility.getInstance();

	private HashMap<Integer, TabStatus> idToTabStatus;

	JTabbedPane tab = null;
	ChartPlotController _controller;
	WelcomeScreen ws;

	Dimension _windowSize;

	JFileChooser chooser;
	
	TextArea feed;

	public Plot(Dimension windowSize) 
	{
		xAxis = new XAxis("No. of Iterations");
		yAxis = new YAxis("Time Elapsed");
		graph = new ArrayList<Graph>();
		init = false;

		_windowSize = windowSize;
		setSize(_windowSize);
		setResizable(false);		
		idToTabStatus = new  HashMap<Integer, TabStatus>();
	}

	public void setController(ChartPlotController controller)
	{
		_controller = controller;
		addWelcomeScreen();
	}

	private void addWelcomeScreen()
	{
		ws = new WelcomeScreen(_windowSize, _controller);
		add(ws);	
	}

	public void setOnOffButton(modes onOff)
	{
		if(onOff == modes.online)
			ws.setModeToggle(true);
		if(onOff == modes.offline)
			ws.setModeToggle(false);
	}

	private void initComponent()
	{    			
		double tabFractionSize = 0.75;
		double size[][] = {{tabFractionSize,TableLayout.FILL},{TableLayout.FILL}};
		setLayout(new TableLayout(size));

		tab = new JTabbedPane();
		tab.setSize((int) (getWidth()*tabFractionSize), getHeight());
		add(tab, "0,0,1,1"); //removed getcontentpane()	
		
		feed = new TextArea("",120,40,TextArea.SCROLLBARS_VERTICAL_ONLY);
		feed.setBounds((int)(getWidth()*tabFractionSize), 0, (int) (getWidth()*(1-tabFractionSize)), getHeight());
		add(feed, "1,0,1,1");		

		for(int i=0;i<graph.size();i++)
		{
			addComponentsToTab(i);
		}

		tab.setVisible(true);
	}

	private void addComponentsToTab(int i) 
	{
		TabStatus currTabStatus = idToTabStatus.get(Integer.valueOf(i));
		Graph graphToAdd = currTabStatus.getTab();
		constructToolbar(graphToAdd);

		addMessageBox(currTabStatus);
		addLegend(currTabStatus);

		tab.addTab("Tab "+i, graphToAdd);
	}

	private void constructToolbar(Graph graphToAdd) 
	{
		JComponent toolbar = ((InteractiveGraph)graphToAdd).getToolBar();

		addStopLabel(toolbar);
		addStartOverLabel(toolbar);

		addSetYButton(graphToAdd, toolbar);
		addProcessSpinner(toolbar);			

		graphToAdd.add(toolbar, BorderLayout.NORTH);
	}

	private void addLegend(TabStatus tabStatus) 
	{
		Legend legend = new Legend();
		tabStatus.setLegend(legend);
		tabStatus.getTab().add(legend,BorderLayout.EAST);
	}

	private void addMessageBox(TabStatus tabStatus) 
	{		
		JPanel textPanel = new JPanel();
		textPanel.setBounds(0,getHeight() - 120, tab.getWidth(), 120);
		textPanel.setLayout(new GridBagLayout());
		TextArea messageBox = new TextArea("", textPanel.getHeight()/16, textPanel.getWidth()/8, TextArea.SCROLLBARS_VERTICAL_ONLY);
		tabStatus.setMessageBox(messageBox);

		Insets padding = new Insets(0,0,20,0);
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.insets = padding;
		textPanel.add(messageBox, gbc);

		tabStatus.getTab().add(textPanel,BorderLayout.SOUTH);
	}

	private void addSetYButton(Graph graphToAdd, JComponent toolbar)
	{
		JToggleButton yLimit = new JToggleButton("Set Y");
		yLimit.addItemListener((InteractiveGraph)graphToAdd);
		toolbar.add(yLimit);
	}

	private void addProcessSpinner(JComponent toolbar) 
	{
		SpinnerListModel model;
		ArrayList<Integer> procNumsList = _controller.getProcessNumList();
		model = new SpinnerListModel(procNumsList);

		JSpinner spin = new JSpinner(model);
		spin.setValue(_controller.getProcNum());
		spin.addChangeListener(_controller);
		toolbar.add(spin);
	}

	private void addStartOverLabel(JComponent toolbar) 
	{
		JLabel startOver = new JLabel();
		startOver.setName("Start Over");
		ImageIcon startOverIcon = new ImageIcon(util.getImage("/icons/restart2.png").getScaledInstance(40, 40, Image.SCALE_SMOOTH));
		startOver.setToolTipText("Start Over");
		startOver.setCursor(ACTION_CURSOR);
		startOver.setIcon(startOverIcon);
		startOver.addMouseListener(_controller);
		startOver.setBounds(400,10,100,25);
		toolbar.add(startOver);
	}

	private void addStopLabel(JComponent toolbar) 
	{
		JLabel stop = new JLabel();
		stop.setName("Stop");
		ImageIcon stopIcon = new ImageIcon(util.getImage("/icons/disconnect1.png").getScaledInstance(40, 40, Image.SCALE_SMOOTH));
		stop.setBounds(400,10,100,25);
		stop.setToolTipText("Stop");
		stop.setCursor(ACTION_CURSOR);
		stop.setIcon(stopIcon);
		stop.addMouseListener(_controller);
		toolbar.add(stop);
	}

	private void addNewGraphToList() 
	{
		Insets padding = new Insets(100, 70, 240, 200);
		Graph newGraph = new InteractiveGraph(xAxis, yAxis);    		
		newGraph.getXAxis().setZigZaginess(BigDecimal.valueOf(7L, 1));
		newGraph.getYAxis().setZigZaginess(BigDecimal.valueOf(7L, 1));
		newGraph.setBackground(Color.WHITE);
		newGraph.setPadding(padding);
		graph.add(newGraph);	
	}

	private void addFunctionToGraph(int i) 
	{
		ChartStyle style2 = new ChartStyle();
		style2.setPaint(color[0]);
		idToTabStatus.get(Integer.valueOf(i)).getTab().addFunction(new Function("Function "+i), style2);
	}

	public void addPointtoGraph(int messageId, double y)
	{
		Integer iteration = idToTabStatus.get(messageId).getIteration();
		BigDecimal xPoint = new BigDecimal(iteration.doubleValue());
		BigDecimal yPoint = new BigDecimal(y);
		Graph updateGraph = idToTabStatus.get(Integer.valueOf(messageId)).getTab();

		Object[] functions = updateGraph.getGraphFunctions();
		Function graphFunc = (Function)functions[functions.length-1];
		graphFunc.addPoint(xPoint, yPoint);    		   		

		idToTabStatus.get(messageId).incrementIterator();	
	}

	public void recvReceived(EventObject event) 
	{
		if(event.getClass().getSimpleName().contentEquals("PointsEvent"))  
		{
			PointsEvent ptEvent = (PointsEvent)event;
			int messageId = ptEvent.get_id();
			double yval = ptEvent.get_yval();
			newMessage(messageId);    	
			addPointtoGraph(messageId, yval);
		}
		else if(event.getClass().getSimpleName().contentEquals("MessageReceivedEvent"))
		{
			MessageReceivedEvent mrEvent = (MessageReceivedEvent)event;
			int messageId = mrEvent.get_messageId();
			String message = mrEvent.get_message();
			newMessage(messageId);
			showInMessageBox(messageId, message);
		}
		else if(event.getClass().getSimpleName().contentEquals("FunctionChangeEvent"))
		{
			FunctionChangeEvent fcEvent = (FunctionChangeEvent)event;
			int messageId = fcEvent.get_messageId();
			int funcId = fcEvent.get_functionId();
			String objName = fcEvent.get_objectName();
			newMessage(messageId);

			functionChange(messageId, funcId, objName);    		
		}
		else if(event.getClass().getSimpleName().contentEquals("WinnerDecidedEvent"))
		{
			WinnerDecidedEvent wdEvent = (WinnerDecidedEvent)event;
			int messageId = wdEvent.get_messageId();
			int functionId = wdEvent.get_functionId();
			int fnctsetId = wdEvent.get_fnctsetId();
			String fnctsetName = wdEvent.get_fnctsetName();
			String objName = wdEvent.get_objName();
			objName = objName.replaceAll("[^A-Za-z\\_\\s]", " ");
			newMessage(messageId);
			winnerDecided(messageId, fnctsetId, fnctsetName, functionId, objName);    
		}
		else if(event.getClass().getSimpleName().contentEquals("SensitivityEvent"))
		{
			SensitivityEvent sEvent = (SensitivityEvent)event;
			int messageId = sEvent.get_id();
			String message = "Sensitivity = " + sEvent.get_sensitivity();
			newMessage(messageId);
			showInMessageBox(messageId, message);
		}
		else if(event.getClass().getSimpleName().contentEquals("AnalysisEvent"))
		{
			AnalysisEvent aEvent = (AnalysisEvent)event;
			String analysis = aEvent.getAnalysis();
			feed.setText(analysis);
		}
		
		repaintApplication();
	}

	private void newMessage(int messageId) 
	{
		if(!idToTabStatus.containsKey(Integer.valueOf(messageId)))
		{
			if(!init)
			{				
				initComponent();
				init = true;
				ws.setVisible(false);
			}
			addNewGraphToList();
			idToTabStatus.put(Integer.valueOf(messageId), new TabStatus(graph.get(graph.size() - 1),Integer.valueOf(1)));

			addFunctionToGraph(messageId);
			addComponentsToTab(messageId);	
			this.paint(this.getGraphics());
		}
	}

	private void repaintApplication() 
	{
		for(Graph eachGraph : graph)
		{
			eachGraph.render();
			eachGraph.repaint();
		}
	}

	private void winnerDecided(int messageId, int fnctsetId, String fnctsetName, int functionId, String objectName) 
	{		
		ChartStyle style2 = new ChartStyle();		
		TabStatus tabStatus = idToTabStatus.get(Integer.valueOf(messageId));
		style2.setPaint(color[tabStatus.getFuncNumFromId(functionId)]);

		Graph currGraph = tabStatus.getTab();		
		currGraph.addFunction(new Function("Function "+messageId), style2);	
		
		String winnerFunctionName = currGraph.getGraphFunctions()[tabStatus.getFuncNumFromId(functionId)+1].toString();
		
		tabStatus.getLegend().highlightWinner(winnerFunctionName);

		String winnerMsg = objectName+" "+messageId+" function set "+fnctsetId+" "+fnctsetName
		+" winner is "+functionId+" "+winnerFunctionName;
		
		showInMessageBox(messageId, winnerMsg);
	}

	private void functionChange(int messageId, int functionId, String functionName) 
	{		
		if(!idToTabStatus.get(messageId).functionIdExists(functionId))
		{
			idToTabStatus.get(messageId).addFunctionId(functionId);
			int functionNum = idToTabStatus.get(messageId).getFunctionNumber();
			functionName = functionName.trim();
			ChartStyle style2 = new ChartStyle();
			Color funcColor = color[functionNum-1];
			style2.setPaint(funcColor);
			TabStatus tabStatus = idToTabStatus.get(Integer.valueOf(messageId));
			Graph currGraph = tabStatus.getTab();

			currGraph.addFunction(new Function(functionName), style2);			
			tabStatus.getLegend().addToLegendList(funcColor,functionName);
		}
	}

	private void showInMessageBox(int id, String str) 
	{
		TextArea textbox = (TextArea)idToTabStatus.get(id).getMessageBox();
		textbox.append(str+"\n");
	}

	public void hideTab()
	{
		tab.setVisible(false);
	}
	
	public void startOver()
	{
		Collection<TabStatus> list = idToTabStatus.values();

		for(TabStatus eachStatus : list )
		{			
			((InteractiveGraph)eachStatus.getTab()).resetToggle();
		}

		if(tab != null)
		{
			tab.setVisible(false);
			if(!_controller.getBegin())
			{
				ws.setVisible(true);
			}
			tab.removeAll();
			remove(tab);		
			tab = null;
		}

		for(TabStatus eachStatus : list )
		{			
			eachStatus.getFuncIdToFuncNum().clear();
			eachStatus = null;
		}
		if(idToTabStatus != null)
			idToTabStatus.clear();
		
		for(Graph graphToRemove : graph)
		{
			JComponent toolbar = ((InteractiveGraph)graphToRemove).getToolBar();
			toolbar.removeAll();			
			graphToRemove.removeAll();
		}

		if(graph != null)
		{
			graph.clear();	
		}
		
		init = false;
	}

	public void hideButtons() 
	{
		ws.hideButtons();		
	}
	
	public void showButtons() 
	{
		ws.showButtons();		
	}
}