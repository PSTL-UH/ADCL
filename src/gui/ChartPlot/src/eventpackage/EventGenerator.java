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
	    
	    public synchronized void _firePointsEvent(Object source, int id, double yval ) 
	    {
	    	EventObject event = createPointsEvent( source, id, yval);
	        callHandler(event);
	    }

		public EventObject createPointsEvent(Object source, int id, double yval) 
		{
			EventObject event = new PointsEvent( source, id, yval);
			return event;
		}
	    
	    public synchronized void _fireFunctionChangeEvent(Object source, int msgId, int funcId, String message ) 
	    {
	    	EventObject event = createFunctionChangeEvent(source, msgId, funcId, message);
	        callHandler(event);
	    }

		public EventObject createFunctionChangeEvent(Object source, int msgId,
				int funcId, String message) {
			EventObject event = new FunctionChangeEvent( this, msgId, funcId, message);
			return event;
		}
	    
	    public synchronized void _fireWinnerDecidedEvent(Object source, int messageId, int fnctsetId, String fnctsetName, 
	    		int functionId, String objectName) 
	    {
	    	EventObject event = createWinnerDecidedEvent(source, messageId,
					fnctsetId, fnctsetName, functionId, objectName);
	        callHandler(event);
	    }

		public EventObject createWinnerDecidedEvent(Object source, int messageId, int fnctsetId, String fnctsetName, 
				int functionId, String objectName)
		{
			EventObject event = new WinnerDecidedEvent( this, messageId, fnctsetId, fnctsetName, 
					functionId, objectName);
			return event;
		}
	    
	    public synchronized void _fireMessageReceivedEvent(Object source, int msgId, String message ) 
	    {
	    	EventObject event = createMessageReceivedEvent(source, msgId, message);
	        callHandler(event);
	    }

	    public EventObject createMessageReceivedEvent(Object source, int msgId, String message) 
	    {
	    	EventObject event = new MessageReceivedEvent( this, msgId, message);
			return event;
		}
	    
		public synchronized void callHandler(EventObject event) 
		{
			Iterator<CustomEventListener> listeners = _listeners.iterator();
	        while( listeners.hasNext() ) 
	        {
	            listeners.next().recvReceived( event );
	        }
		}

		public void _fireSensitivityEvent(Object source, Integer key, double sensitivity) 
		{
			EventObject event = createSensitivityEvent(source, key, sensitivity);
			callHandler(event);
		}

		public EventObject createSensitivityEvent(Object source, Integer key,	double sensitivity) 
		{
			EventObject event = new SensitivityEvent(source, key, sensitivity);
			return event;
		}

		public void _fireAnalysisEvent(Object source, String analysisOfWinnerFunctions) 
		{
			EventObject event = createAnalysisEvent(source, analysisOfWinnerFunctions);
			callHandler(event);
		}

		public EventObject createAnalysisEvent(Object source, String analysisOfWinnerFunctions) 
		{
			EventObject event = new AnalysisEvent(source,analysisOfWinnerFunctions);
			return event;
		}
}
