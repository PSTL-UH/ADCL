package datastructures;

import java.util.HashMap;

public class FunctionNameTable 
{
	class FunctionUniqueID
	{
		int requestId;
		int functionId;
		
		public FunctionUniqueID(int reqId, int funcId)
		{
			requestId = reqId;
			functionId = funcId;
		}
		
		@Override
		public int hashCode() 
		{
		    final int PRIME = 31;
		    int result = 1;
		    result = PRIME * result + requestId+functionId;
		    return result;
		}
		
		@Override
		public boolean equals(Object obj)
		{
			FunctionUniqueID funcUniqueObj = (FunctionUniqueID)obj;
			if(this.requestId == funcUniqueObj.requestId && this.functionId == funcUniqueObj.functionId )
			{
				return true;
			}
			return false;
		}
	}
	
	HashMap<FunctionUniqueID, String> idToFunctionName;
	
	public FunctionNameTable()
	{
		idToFunctionName = new HashMap<FunctionUniqueID, String>();
	}
	
	public void addInfoToTable(int requestId, int functionId, String functionName)
	{
		idToFunctionName.put(new FunctionUniqueID(requestId, functionId), functionName);
	}
	
	public String getFunctionName(int requestId, int functionId)
	{
		return idToFunctionName.get(new FunctionUniqueID(requestId, functionId));
	}
}
