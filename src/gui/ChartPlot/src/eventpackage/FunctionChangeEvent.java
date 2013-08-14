package eventpackage;

import java.util.EventObject;

public class FunctionChangeEvent extends EventObject {
    /**
	 * 
	 */
	private static final long serialVersionUID = 1996468126653327812L;
	int _messageId;
    int _functionId; 
    String _objectName;
    
    public FunctionChangeEvent( Object source, int msgId, int funcId, String objectName) 
    {
        super( source );
        _messageId = msgId;
        _functionId = funcId;
        _objectName = objectName;
    }

	public int get_functionId(){return _functionId;}
	
	public String get_objectName(){return _objectName;}

	public int get_messageId() 
	{
		return _messageId;
	}    
}