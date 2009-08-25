
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
import communicationmanager.StartupServer;
import communicationmanager.StartupServer.message_types;

import eventpackage.*;
import plot.*;
import utility.Utility;

@SuppressWarnings("serial")
public final class Plot extends JFrame implements RecvListener
{

	Color color[]={Color.GREEN,Color.BLACK,Color.RED,Color.BLUE,Color.CYAN,Color.YELLOW,Color.GRAY,Color.PINK,Color.ORANGE,Color.WHITE,Color.MAGENTA,Color.LIGHT_GRAY};
	private int noOfGraphs = 0;
    XAxis xAxis = new XAxis("No. of Iterations");
    YAxis yAxis = new YAxis("Time Elapsed");// make changes in axisinstance in the paint function
    private final ArrayList<Graph> graph = new ArrayList<Graph>();
    HashMap<Integer, Graph> idToTabMap;
    HashMap<Integer, Integer> idToIteration;
    
    
    JTabbedPane tab = null;
    ChartPlotController _controller;

	public Plot(StartupServer server) 
	{
		setResizable(false);
		ChartPlotController controller = new ChartPlotController(this,server);
		_controller = controller;
		idToTabMap = new HashMap<Integer, Graph>();
		idToIteration = new HashMap<Integer, Integer>();
		
		initGraph();
		initFunctions();
		initComponent();			
	}

    private void initComponent()
    {    	
        tab = new JTabbedPane();
        getContentPane().add(tab);
        
        for(int i=0;i<graph.size();i++)
        {
        	addComponentsToTab(i);
        }
    }

	private void addComponentsToTab(int i) 
	{
		JButton disconnect = new JButton("Disconnect");
		disconnect.setBounds(400,10,100,25);
		disconnect.addActionListener(_controller);
		Graph graphToAdd = idToTabMap.get(Integer.valueOf(i));
		JComponent toolbar = ((InteractiveGraph)graphToAdd).getToolBar();
		toolbar.add(disconnect);
		
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


    private void initGraph() 
    {    
    	for(int i=0;i<noOfGraphs ;i++)
    	{
    		addNewGraphToList();
    	}
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


    private void initFunctions()
    {
		for(int i=0;i<graph.size();i++)
		{
			addFunctionToGraph(i);
		}
    }

	private void addFunctionToGraph(int i) {
		ChartStyle style2 = new ChartStyle();
		style2.setPaint(Color.GREEN);
		idToTabMap.get(Integer.valueOf(i)).addFunction(new Function("Function "+i), style2);
	}

    public void addPointtoGraph(int index,double x,double y)
    {
    	BigDecimal xPoint = new BigDecimal(x);
    	BigDecimal yPoint = new BigDecimal(y);
    	Graph updateGraph = idToTabMap.get(Integer.valueOf(index));
    	Object[] functions = updateGraph.getGraphFunctions();
    	
    		Function graphFunc = (Function)functions[functions.length-1];
    		graphFunc.addPoint(xPoint, yPoint);
    	
    }

    
    @Override
    public void recvReceived(RecvEvent event) 
    {
		byte[] msg = event.getRecvData();
		Header head = event.get_header();
				
		if(!idToTabMap.containsKey(Integer.valueOf(head.get_id())))
		{
			addNewGraphToList();
			idToTabMap.put(Integer.valueOf(head.get_id()), graph.get(graph.size() - 1));
			idToIteration.put(Integer.valueOf(head.get_id()), Integer.valueOf(1));
			addFunctionToGraph(head.get_id());
			addComponentsToTab(head.get_id());					
		}
		
		if( message_types.values()[head.get_type()] == message_types.Points)
		{    	
			Integer iteration = idToIteration.get(head.get_id());
			addPointtoGraph(head.get_id(), iteration.doubleValue(), head.get_yval());
			idToIteration.remove(head.get_id());
			iteration = Integer.valueOf(iteration.intValue() + 1);	
			idToIteration.put(head.get_id(), iteration);
		}
		else if(message_types.values()[head.get_type()] == message_types.Message)
		{			
			showInMessageBox(head.get_id(),new String(msg));			
		}
		else if(message_types.values()[head.get_type()] == message_types.FunctionChange)
		{
			Utility util = Utility.getInstance();
			int functionId = util.byteArrayToInt(msg, 0);
			String message = new String(msg);
			message = message.replaceAll("[^A-Za-z0-9\\_]", " ");
			message.substring(0, message.indexOf(" "));
			
			functionChange(message, head, functionId);
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
		Graph currGraph = idToTabMap.get(Integer.valueOf(head.get_id()));
		currGraph.addFunction(new Function("Function "+head.get_id()), style2);			
		((Legend)currGraph.getComponent(currGraph.getComponentCount()-1)).addToLegendList(color[functionId],functionName);
	}

	private void showInMessageBox(int id, String str) 
	{
			Panel panel = (Panel)idToTabMap.get(id).getComponent(1);
			TextArea textbox = (TextArea)panel.getComponent(0);
			textbox.append(str+"\n");
	}
}


