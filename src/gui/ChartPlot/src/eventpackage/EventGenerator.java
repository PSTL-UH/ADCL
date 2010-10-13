package eventpackage;

import java.util.ArrayList;
import java.util.EventObject;
import java.util.Iterator;
import java.util.List;

import datastructures.Header;
	
public class EventGenerator 
{
		byte[] recvdata;
		Header _head;
	    private List<CustomEventListener> _listeners = new ArrayList<CustomEventListener>();

	    public synchronized void addRecvListener( CustomEventListener l ) 
	    {
	        _listeners.add( l );
	    }
	    
	    public synchronized void removeRecvListener( CustomEventListener l )
	    {
	        _listeners.remove( l );
	    }
	    
	    public synchronized void _firePointsEvent(int id, double yval ) 
	    {
	        PointsEvent event = new PointsEvent( this, id, yval);
	        callHandler(event);
	    }
	    
	    public synchronized void _fireFunctionChangeEvent(int msgId, int funcId, String message ) 
	    {
	        FunctionChangeEvent event = new FunctionChangeEvent( this, msgId, funcId, message);
	        callHandler(event);
	    }
	    
	    public synchronized void _fireWinnerDecidedEvent(int messageId, int fnctsetId, String fnctsetName, int functionId, String objectName) 
	    {
	        WinnerDecidedEvent event = new WinnerDecidedEvent( this, messageId, fnctsetId, fnctsetName, functionId, objectName);
	        callHandler(event);
	    }
	    
	    public synchronized void _fireMessageReceivedEvent(int msgId, String message ) 
	    {
	        MessageReceivedEvent event = new MessageReceivedEvent( this, msgId, message);
	        callHandler(event);
	    }
	    
		public synchronized void callHandler(EventObject event) 
		{
			Iterator<CustomEventListener> listeners = _listeners.iterator();
	        while( listeners.hasNext() ) 
	        {
	            listeners.next().recvReceived( event );
	        }
		}

		public void _fireSensitivityEvent(Integer key, double sensitivity) 
		{
			SensitivityEvent event = new SensitivityEvent(this, key, sensitivity);
			callHandler(event);
		}

		public void _fireAnalysisEvent(String analysisOfWinnerFunctions) 
		{
			AnalysisEvent event = new AnalysisEvent(this,analysisOfWinnerFunctions);
			callHandler(event);
		}
}
