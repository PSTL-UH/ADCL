package eventpackage;

import java.util.EventObject;

public class WinnerDecidedEvent extends EventObject {
    /**
	 * 
	 */
	private static final long serialVersionUID = 4738169978476939518L;
	int _messageId;
    int _fnctsetId;
    int _functionId; 
    String _objName;
    String _fnctsetName;
    
    public WinnerDecidedEvent( Object source, int msgId, int fnctsetId, String fnctsetName, int funcId, String objName) 
    {
        super( source );
        _messageId = msgId;
        _fnctsetId = fnctsetId;
        _functionId = funcId;
        _objName = objName;
        _fnctsetName = fnctsetName;
    }
    
    public int get_messageId() 
    {
		return _messageId;
	}

	public int get_functionId(){return _functionId;}
	
	public String get_objName(){return _objName;} 
	
	public int get_fnctsetId(){return _fnctsetId;}
	
	public String get_fnctsetName(){return _fnctsetName;}
}