package eventpackage;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import communicationmanager.Header;


	
public class EventGenerator 
{
		byte[] recvdata;
		Header _head;
	    private List _listeners = new ArrayList();
	    
	    public synchronized void dataReceived(byte[] data, Header head) 
	    {
	       recvdata = data;
	       _head = head;
	       _fireRecvEvent();
	    }
	    
	    public synchronized void addRecvListener( RecvListener l ) {
	        _listeners.add( l );
	    }
	    
	    public synchronized void removeRecvListener( RecvListener l ) {
	        _listeners.remove( l );
	    }
	     
	    private synchronized void _fireRecvEvent() 
	    {
	        RecvEvent event = new RecvEvent( this, recvdata, _head);
	        Iterator listeners = _listeners.iterator();
	        while( listeners.hasNext() ) 
	        {
	            ( (RecvListener) listeners.next() ).recvReceived( event );
	        }
	    }
}
