package datastructures;

import java.util.ArrayList;
import java.util.Set;

public class SpinList extends ArrayList<Integer>
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public SpinList()
	{
		super();
	}
	
	public SpinList(Set<Integer> keySet) 
	{
		super(keySet);
	}

	public int indexOf(Object o) 
	{
        if (o instanceof String) {
           try 
           {
              o = new Integer(Integer.parseInt(((String) o).trim()));
           } 
           catch (NumberFormatException ex) 
           {
              ex.printStackTrace();
              return -1;
           }
        }
        return super.indexOf(o);
     }	
}
