package datastructures;

import java.util.ArrayList;
import java.util.EventObject;
import java.util.HashMap;


public class ProcessDataStore 
{
	public enum ProcessCommStatus {Head, Msg};
	
	private int rank;

	private ProcessCommStatus commStatus;
	private Integer previousLast;

	private ArrayList<EventObject> eventsList;
	
	HashMap< Integer, Sensitivity > idToFunctionAvgTime;

	public HashMap<Integer, Sensitivity> getIdToFunctionAvgTime() {
		return idToFunctionAvgTime;
	}

	public EventObject getEvent() 
	{
		EventObject event = null;

		if(previousLast < eventsList.size())
		{
			event = eventsList.get(previousLast);
			previousLast++;
		}
		return event;
	}

	private byte[] unparsedBytes;
	int unparsedBytesStartPosition;

	private Header lastHeader;

	public ProcessDataStore()
	{
		previousLast = 0;
		commStatus = ProcessCommStatus.Head;
		eventsList = new ArrayList<EventObject>();
		unparsedBytes = new byte[1024];
		unparsedBytesStartPosition = 0;
		idToFunctionAvgTime = new HashMap<Integer, Sensitivity>();
	}

	public void setUnparsedBytes(byte[] src)
	{
		System.arraycopy(src, 0, unparsedBytes, 0, src.length);
		unparsedBytesStartPosition = src.length;
	}

	public void getUnparsedBytes(byte[] dst)
	{
		System.arraycopy(unparsedBytes, 0, dst, 0, unparsedBytesStartPosition);
	}

	public void resetPosition()
	{
		unparsedBytesStartPosition = 0;
	}

	public int getPosition()
	{
		return unparsedBytesStartPosition;
	}

	public ProcessCommStatus getCommStatus() 
	{
		return commStatus;
	}

	public void setCommStatus(ProcessCommStatus commStatus) {
		this.commStatus = commStatus;
	}

	public int getRank() 
	{
		return rank;
	}

	public void addEvent(EventObject event)
	{
		eventsList.add(event);
	}

	public void setLastHeader(Header head)
	{
		lastHeader = head;
	}

	public Header getLastHeader() 
	{
		return lastHeader;
	}

	public void resetPreviousLast() 
	{
		//access to previousLast already synchronized using currentProcess variable 
		//in startup server so no need to do it here.
		previousLast = 0;	
	}

	public int getPreviousLast() 
	{
		return previousLast;
	}

	public void clearData() 
	{
		eventsList.clear();
		idToFunctionAvgTime.clear();		
	}	
}
