package eventpackage;

import java.util.EventObject;

public class MessageReceivedEvent extends EventObject {
    /**
	 * 
	 */
	private static final long serialVersionUID = -2568182530156972115L;
	int _messageId;
    String _message;
    
    public MessageReceivedEvent( Object source, int msgId, String message) 
    {
        super( source );
        _messageId = msgId;
        _message = message;
    }
	
	public String get_message(){return _message;}

	public int get_messageId() 
	{
		return _messageId;
	}    
}