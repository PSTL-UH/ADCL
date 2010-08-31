package datastructures;

public class Sensitivity 
{
	private double max = -9999999.0;
	private double min = 9999999.0;
	private double sumTime;
	private int numIters;

	public Sensitivity() 
	{
		sumTime = 0.0;
		numIters = 0;
	}

	public void newFunctionTime()
	{
		double avg = sumTime/numIters;
		if( avg > max)
		{
			max = avg;
		}
		if( avg < min )
		{
			min = avg;
		}
	}
	
	public void addTime(double d)
	{
		sumTime += d;
		numIters++;
	}
	
	public double getSensitivity()
	{
		double sensitivity;
		if( max <= 0 || min <= 0)
		{
			sensitivity = 0;
		}
		else
		{
			sensitivity = (max - min)/min;
		}
		return sensitivity;
	}
}
