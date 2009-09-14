
package chartplot;

/**
 *
 * @author sarat
 */

import java.awt.*;
import java.util.*;
import java.math.*;
import javax.swing.*;

import Main.ChartPlotController;

import communicationmanager.Header;
import communicationmanager.StartupServer.message_types;

import eventpackage.*;
import plot.*;
import utility.Utility;

@SuppressWarnings("serial")
public final class Plot extends JFrame implements RecvListener
{
	class TabStatus
	{
		public TabStatus(Graph tab, int iteration) 
		{			
			super();
			this.tab = tab;
			this.iteration = iteration;
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
		HashMap<Integer, Integer> funcNumToFuncId = new HashMap<Integer, Integer>();
		public HashMap<Integer, Integer> getFuncNumToFuncId() {
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
			funcNumToFuncId.put(Integer.valueOf(functionNumber), Integer.valueOf(functionId));
			functionNumber++;
		}
		
		public boolean functionIdExists(int functionId)
		{
			return funcNumToFuncId.containsValue(functionId);
		}
		public void incrementIterator() 
		{
			iteration++;
		}
	}

	Color color[]={Color.GREEN,Color.BLACK,Color.RED,Color.BLUE,Color.CYAN,Color.YELLOW,Color.GRAY,Color.PINK,Color.ORANGE,Color.WHITE,Color.MAGENTA,Color.LIGHT_GRAY};
    XAxis xAxis = new XAxis("No. of Iterations");
    YAxis yAxis = new YAxis("Time Elapsed");// make changes in axisinstance in the paint function
    private final ArrayList<Graph> graph = new ArrayList<Graph>();
    boolean init = false;
    private static final Cursor ACTION_CURSOR = new Cursor(Cursor.HAND_CURSOR);
    
    Utility util = Utility.getInstance();
    
    HashMap<Integer, TabStatus> idToTabStatus = new  HashMap<Integer, TabStatus>();;
        
    JTabbedPane tab = null;
    ChartPlotController _controller;
    WelcomeScreen ws;
    Dimension _windowSize;

	public Plot(Dimension windowSize) 
	{
		_windowSize = windowSize;
		setSize(_windowSize);
		setResizable(false);		
				
		addWelcomeScreen();
	}
	
	public void setController(ChartPlotController controller)
	{
		_controller = controller;
	}
	
    private void addWelcomeScreen()
    {
		ws = new WelcomeScreen(_windowSize);
		getContentPane().add(ws);		
	}

	private void initComponent()
    {    	
        tab = new JTabbedPane();
        getContentPane().add(tab);
        
        for(int i=0;i<graph.size();i++)
        {
        	addComponentsToTab(i);
        }

		tab.setVisible(true);
    }

	private void addComponentsToTab(int i) 
	{
		Graph graphToAdd = idToTabStatus.get(Integer.valueOf(i)).getTab();
		
		JLabel disconnect = new JLabel();
		disconnect.setName("Disconnect");
		ImageIcon disconnectIcon = new ImageIcon(util.getImage("/icons/disconnect1.png").getScaledInstance(40, 40, Image.SCALE_SMOOTH));
		disconnect.setBounds(400,10,100,25);
		disconnect.setToolTipText("Disconnect");
		disconnect.setCursor(ACTION_CURSOR);
		disconnect.setIcon(disconnectIcon);
		disconnect.addMouseListener(_controller);
		
		JLabel startOver = new JLabel();
		startOver.setName("Start Over");
		ImageIcon startOverIcon = new ImageIcon(util.getImage("/icons/restart2.png").getScaledInstance(40, 40, Image.SCALE_SMOOTH));
		startOver.setToolTipText("Start Over");
		startOver.setCursor(ACTION_CURSOR);
		startOver.setIcon(startOverIcon);
		startOver.addMouseListener(_controller);
		startOver.setBounds(400,10,100,25);
		
		JComponent toolbar = ((InteractiveGraph)graphToAdd).getToolBar();
		toolbar.add(disconnect);
		toolbar.add(startOver);
		
		JToggleButton yLimit = new JToggleButton("Set Y");
		yLimit.addItemListener((InteractiveGraph)graphToAdd);
		toolbar.add(yLimit);
		
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.ipady = 100;
		c.weightx = 0.5;
		c.gridwidth = 3;
		c.gridx = 2;
		c.gridy = 0;
		
		graphToAdd.add(toolbar, c);

		Panel textPanel = new Panel();
		textPanel.setBounds(0,getHeight() - 120, getWidth(), 100);
		TextArea messageBox = new TextArea("", textPanel.getHeight()/16, getWidth()/8, 1);
		
		textPanel.add(messageBox);
		c.ipady = 0;
		c.gridwidth = 2;
		c.gridx = 1;
		c.gridy = 3;
		graphToAdd.add(textPanel,c);
		
		Legend legend = new Legend();
		c.ipady = 350;
		c.insets = new Insets(0,getWidth() - 200,0,0);
		c.gridwidth = 2;
		c.gridx = 2;
		c.gridy = 1;
		graphToAdd.add(legend,c);

		tab.addTab("Tab "+i, graphToAdd);
	}

	private void addNewGraphToList() 
	{
		Insets padding = new Insets(50, 70, 200, 200);
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

    public void addPointtoGraph(int index,double x,double y)
    {
    	BigDecimal xPoint = new BigDecimal(x);
    	BigDecimal yPoint = new BigDecimal(y);
    	Graph updateGraph = idToTabStatus.get(Integer.valueOf(index)).getTab();
    	
    	Object[] functions = updateGraph.getGraphFunctions();
    	Function graphFunc = (Function)functions[functions.length-1];
    	graphFunc.addPoint(xPoint, yPoint);    		   		
    }
    
    @Override
    public void recvReceived(RecvEvent event) 
    {
		byte[] msg = event.getRecvData();
		Header head = event.get_header();
				
		if(!idToTabStatus.containsKey(Integer.valueOf(head.get_id())))
		{
			if(!init)
			{
				ws.setVisible(false);
				initComponent();
				init = true;
			}
			addNewGraphToList();
			idToTabStatus.put(Integer.valueOf(head.get_id()), new TabStatus(graph.get(graph.size() - 1),Integer.valueOf(1)));
			
			addFunctionToGraph(head.get_id());
			addComponentsToTab(head.get_id());					
		}
		
		if( message_types.values()[head.get_type()] == message_types.Points)
		{    				
			Integer iteration = idToTabStatus.get(head.get_id()).getIteration();			
			addPointtoGraph(head.get_id(), iteration.doubleValue(), head.get_yval());
			idToTabStatus.get(head.get_id()).incrementIterator();			
		}
		else if(message_types.values()[head.get_type()] == message_types.Message)
		{			
			showInMessageBox(head.get_id(),new String(msg));			
		}
		else if(message_types.values()[head.get_type()] == message_types.FunctionChange)
		{
			int functionId = util.byteArrayToInt(msg, 0);
			String message = new String(msg);
			message = message.replaceAll("[^A-Za-z\\_\\s]", " ");
			
			if(!idToTabStatus.get(head.get_id()).functionIdExists(functionId))
			{
				functionChange(message, head, idToTabStatus.get(head.get_id()).getFunctionNumber());
				idToTabStatus.get(head.get_id()).addFunctionId(functionId);
			}
			
		}
		
		for(Graph eachGraph : graph)
		{
			eachGraph.render();
			eachGraph.repaint();
		}
    	
    }

	private void functionChange(String functionName, Header head, int functionId) 
	{		
		functionName = functionName.substring(4);
		System.out.println("function id is "+functionId);
		ChartStyle style2 = new ChartStyle();
		style2.setPaint(color[functionId]);
		Graph currGraph = idToTabStatus.get(Integer.valueOf(head.get_id())).getTab();
		
		currGraph.addFunction(new Function("Function "+head.get_id()), style2);			
		((Legend)currGraph.getComponent(currGraph.getComponentCount()-1)).addToLegendList(color[functionId],functionName);
		
	}

	private void showInMessageBox(int id, String str) 
	{
		Panel panel = (Panel)idToTabStatus.get(id).getTab().getComponent(1);		
		TextArea textbox = (TextArea)panel.getComponent(0);
		textbox.append(str+"\n");
	}
	
	public void startOver()
	{
		tab.setVisible(false);
		ws.setVisible(true);
		tab.removeAll();
		getContentPane().remove(tab);		
		tab = null;
		Collection<TabStatus> list = idToTabStatus.values();
		for(TabStatus eachStatus : list )
		{
			eachStatus.getFuncNumToFuncId().clear();
			eachStatus = null;
		}
		idToTabStatus.clear();
		graph.clear();
		_controller = null;
		init = false;
	}
}


