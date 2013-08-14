package eventpackage;

import java.util.EventObject;

public class PointsEvent extends EventObject 
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 4927937253215984931L;
	int _id;
    double _yval;    
    
    public PointsEvent( Object source, int id, double yval) 
    {
        super( source );
        _id = id;
        _yval = yval;
    }
    
    public int get_id() {
		return _id;
	}

	public double get_yval(){return _yval;}
}