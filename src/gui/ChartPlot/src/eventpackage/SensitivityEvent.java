package eventpackage;

import java.util.EventObject;

public class SensitivityEvent extends EventObject
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 8638832808686684741L;
	int _id;
    double _sensitivity;    
    
    public SensitivityEvent( Object source, int id, double sensitivity) 
    {
        super( source );
        _id = id;
        _sensitivity = sensitivity;
    }
    
    public int get_id() 
    {
		return _id;
	}

	public double get_sensitivity(){return _sensitivity;}
}
