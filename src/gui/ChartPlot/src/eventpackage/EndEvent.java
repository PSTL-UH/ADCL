package eventpackage;

import java.util.EventObject;

public class EndEvent extends EventObject 
{
    /**
	 * 
	 */
	private static final long serialVersionUID = -5160777933487588279L;

	public EndEvent( Object source) 
    {
        super( source );
    }
}