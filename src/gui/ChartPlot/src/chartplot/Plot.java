
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

import eventpackage.*;
import net.quies.math.plot.*;

@SuppressWarnings("serial")
public final class Plot extends JFrame implements RecvListener
{
	
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
		disconnect.setBounds(440,10,100,25);
		disconnect.addActionListener(_controller);
		Graph graphToAdd = idToTabMap.get(Integer.valueOf(i));
		graphToAdd.add(disconnect,BorderLayout.AFTER_LAST_LINE);

		Panel textPanel = new Panel();
		textPanel.setBounds(0,520, 640, 100);
		TextArea messageBox = new TextArea("", 5, 80, 1);
		
		textPanel.add(messageBox);
		graphToAdd.add(textPanel,BorderLayout.AFTER_LAST_LINE);		

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
		Insets padding = new Insets(50, 50, 150, 50);
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
    	for(Object func : functions)
    	{
    		Function graphFunc = (Function)func;
    		graphFunc.addPoint(xPoint, yPoint);
    	}
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
		if( head.get_type() == 0)
		{    	
			Integer iteration = idToIteration.get(head.get_id());
			addPointtoGraph(head.get_id(), iteration.doubleValue(), head.get_yval());
			idToIteration.remove(head.get_id());
			iteration = Integer.valueOf(iteration.intValue() + 1);	
			idToIteration.put(head.get_id(), iteration);
		}
		else if(head.get_type() == 1)
		{
			showInMessageBox(head.get_id(),new String(msg));
		}
		
		for(Graph eachGraph : graph)
		{
			eachGraph.render();
			eachGraph.repaint();
		}
    	
    }

	private void showInMessageBox(int id, String str) 
	{
		System.out.println(str);
			Panel panel = (Panel)idToTabMap.get(id).getComponent(2);
			TextArea textbox = (TextArea)panel.getComponent(0);
			textbox.append(str+"\n");
	}
}


