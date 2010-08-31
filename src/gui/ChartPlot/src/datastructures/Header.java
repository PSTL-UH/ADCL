package datastructures;

import utility.Utility;


public class Header 
{
	public static final int HEADER_MESSAGE_SIZE = 24;
	private int _magic,_type,_len,_id;
	private double _yval;
	public int get_magic() {
		return _magic;
	}

	public int get_type() {
		return _type;
	}

	public int get_len() {
		return _len;
	}

	public int get_id() {
		return _id;
	}

	public Header(byte[] msg)
	{
		Utility util = Utility.getInstance();
		_magic = util.byteArrayToInt(msg, 0);
		_type = util.byteArrayToInt(msg, 4);
		_len = util.byteArrayToInt(msg, 8);
		_id = util.byteArrayToInt(msg, 12);
		_yval = util.byteArrayToDouble(msg, 16);

	}

	public double get_yval() {
		return _yval;
	}		

}
