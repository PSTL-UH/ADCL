package datastructures;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class WinnerTable 
{
	class Winner
	{
		int _requestId;
		int _fnctsetId;
		String _fnctsetName;
		String _problemSize;
		int _winnerFunctionId;
		
		public Winner(int requestId, int fnctsetId, String fnctsetName, int winnerFunctionId)
		{
			_requestId = requestId;
			_fnctsetId = fnctsetId;
			_fnctsetName = fnctsetName;
			_winnerFunctionId = winnerFunctionId;
		}
	}
	
	class FunctionSetComparator implements Comparator<Winner>
	{

		@Override
		public int compare(Winner o1, Winner o2) 
		{
			if (o1._fnctsetId > o2._fnctsetId)
			{
	            return +1;
	        }
			else if (o1._fnctsetId < o2._fnctsetId)
			{
	            return -1;
	        }
	        else
	        {
	            return 0;
	        }
		}
		
	}
	
	ArrayList<Winner> winners;
	
	public WinnerTable()
	{
		winners = new ArrayList<Winner>();
	}
	
	public void addInfoToTable(int requestId, int fnctsetId, String fnctsetName, int winnerFunctionId)
	{
		winners.add(new Winner( requestId, fnctsetId, fnctsetName, winnerFunctionId));
	}
	
	//analysis doesnt change unless the application is restarted. 
	//so for the current run it has to be stored for it to be available when moving from process to process.
	public String analysisOfWinnerFunctions(FunctionNameTable funcNameTable)
	{
		String analysisString = "";
		
		String dominantWinnerFnctSets = getDominantWinnerFnctSets(funcNameTable);
		
		analysisString += dominantWinnerFnctSets;
	
		return analysisString;
	}

	private String getDominantWinnerFnctSets(FunctionNameTable funcNameTable) 
	{
		String dominantWinners = "";
		FunctionSetComparator fnctsetComp = new FunctionSetComparator();
		Collections.sort(winners, fnctsetComp);
		int fromIndex = 0;
		for(int i = 0; i < winners.size(); i++)
		{
			if( (i == winners.size() - 1) || winners.get(i)._fnctsetId != winners.get(i+1)._fnctsetId)
			{
				ArrayList<Integer> winnerFunctionIds = getDominantWinner(winners.subList(fromIndex, i+1));
				
				if(winnerFunctionIds != null)
				{				
					Winner winner = winners.get(fromIndex);
					if(winnerFunctionIds.size() == 1)
					{
						dominantWinners = dominantWinners+"\nDominant winner for function set "+winner._fnctsetId+" "+
						winner._fnctsetName +" is function "+winnerFunctionIds.get(0)+" "+
					    funcNameTable.getFunctionName(winner._requestId, winnerFunctionIds.get(0));
					}
					else
					{
						dominantWinners = dominantWinners+"\nWinner functions ";
						for(Integer winnerFunctionId : winnerFunctionIds)
						{
							dominantWinners = dominantWinners+winnerFunctionId+" "+
							funcNameTable.getFunctionName(winner._requestId, winnerFunctionId)+" ";
						}
						dominantWinners = dominantWinners+"are equally dominant in function set"+winner._fnctsetId+" "+winner._fnctsetName; 
					}
				}
				fromIndex = i+1;
			}
		}		
		return dominantWinners;
	}

	private ArrayList<Integer> getDominantWinner(List<Winner> subList) 
	{
		int maxCount = -1, totalCount = 0;
		ArrayList<Integer> maxFunctionIds = null;
		HashMap<Integer, Integer> winnerFunctionToCount = new HashMap<Integer, Integer>();
		for(Winner winner : subList)
		{
			if(winnerFunctionToCount.containsKey(winner._winnerFunctionId))
			{
				winnerFunctionToCount.put(winner._winnerFunctionId, winnerFunctionToCount.get(winner._winnerFunctionId)+1);
			}
			else
			{
				winnerFunctionToCount.put(winner._winnerFunctionId, 1);
			}
		}
		
		Iterator<Map.Entry<Integer, Integer>> iterator = winnerFunctionToCount.entrySet().iterator();
		while(iterator.hasNext())
		{
			Map.Entry<Integer,Integer> pairs = iterator.next();
			
			if(pairs.getValue() > maxCount)
			{
				if(maxFunctionIds == null)
				{
					maxFunctionIds = new ArrayList<Integer>();
				}
				else
				{
					maxFunctionIds.clear();
				}
				maxCount = pairs.getValue();
				maxFunctionIds.add(pairs.getKey());
				System.out.println("adding "+pairs.getKey());
				totalCount = 1;
			}
			else if(pairs.getValue() == maxCount)
			{
				totalCount++;
				maxFunctionIds.add(pairs.getKey());
			}
		}
		
		/*if( totalCount > 1 && maxCount <= 1)
		{
			return null;
		}*/
		
		return maxFunctionIds;
	}

	public void clear() 
	{
		winners.clear();		
	}
}
