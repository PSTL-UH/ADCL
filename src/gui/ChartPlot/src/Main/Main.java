package Main;
import java.awt.Dimension;

import chartplot.ChartPlotController;

public class Main
{
	public static void main(String[] args) 
	{	
	    Dimension windowSize = new Dimension(960,720); //820, 720
	    initialize(windowSize);	             
	}

	private static void initialize(Dimension windowSize) 
	{    		
		ChartPlotController controller = new ChartPlotController(windowSize);
		controller.init(); 
	}
}
