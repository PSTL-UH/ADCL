package net.quies.math.plot;

/*
Copyright (c) 2006, 2007 Pascal S. de Kloe. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

import java.awt.*;
import java.util.*;
import java.util.Timer;
import java.math.*;
import javax.swing.*;


/**
 * Demonstration with random coordinates.
 @author Pascal S. de Kloe
 */
public final class Demo extends JFrame {

private
Demo() {
	initGraph();
	initFunctions();
	initTimer();
	getContentPane().add(graph);
}


private void
initGraph() {
	graph.getXAxis().setZigZaginess(BigDecimal.valueOf(7L, 1));
	graph.getYAxis().setZigZaginess(BigDecimal.valueOf(7L, 1));
	graph.setBackground(Color.WHITE);
}


private void
initFunctions() {
	ChartStyle style1 = new ChartStyle();
	style1.setUpperLimitEnabled(true);
	style1.setLowerLimitEnabled(true);
	style1.setPaint(Color.RED);
	graph.addFunction(function1, style1);

	ChartStyle style2 = new ChartStyle();
	style2.setPaint(Color.GREEN);
	graph.addFunction(function2, style2);
}


private void
initTimer() {
	timer.schedule(new RandomFeed(function1), 100L, 800L);
	timer.schedule(new RandomFeed(function2), 100L, 900L);
	timer.schedule(new TimerTask() {

		public void run() {
			graph.render();
			graph.repaint();
		}

	}, 200L, 1000L);
}


public static void
main(String[] args) {
	JFrame demo = new Demo();
	demo.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	demo.setSize(640, 480);
	demo.validate();
	demo.setVisible(true);
}


private final Timer timer = new Timer("QN Plot demo");
private final Graph graph = new InteractiveGraph();
private final Function function1 = new Function("function 1");
private final Function function2 = new Function("function 2");

}



class RandomFeed extends TimerTask {

RandomFeed(Function f) {
	function = f;
}


public void
run() {
	double xFeed = (Math.random() - horizontalDirection) * 100;
	double yFeed = Math.pow((Math.random() - 0.5) * 2.5, 3.0);
	x = x.add(new BigDecimal(xFeed));
	y = y.add(new BigDecimal(yFeed));
	function.addPoint(x, y);
}


final Function function;
final double horizontalDirection = Math.random();
BigDecimal x = new BigDecimal((Math.random() - 0.5) * 200);
BigDecimal y = new BigDecimal((Math.random() - 0.5) * 200);

};
