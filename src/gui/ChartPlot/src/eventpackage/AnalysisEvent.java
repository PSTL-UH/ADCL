package eventpackage;

import java.util.EventObject;

public class AnalysisEvent extends EventObject 
{
	private static final long serialVersionUID = -8747048316665097955L;

	/**
	 * 
	 */
	String _analysis;

	public String getAnalysis() 
	{
		return _analysis;
	}

	public AnalysisEvent( Object source, String analysis) 
    {
        super( source );
        _analysis = analysis;
    }
	
	
}