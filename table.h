#ifndef __table__h__
#define __table__h__
#include <string>
#include "macro.h"
#include <vector>
using namespace std;


class Attribute
{
public:
	string name;    
	int type;   
	int length;    
	bool isPrimaryKey;  
	bool isUnique;      
	Attribute()       
	{
		isPrimaryKey = false;  
		isUnique = false;
	}
	Attribute(string n, int t, int l, bool isP, bool isU)
		:name(n), type(t), length(l), isPrimaryKey(isP), isUnique(isU){}  
};

class Table
{
public:
	string name;   
	int blockNum;	
	int recordNum;
	int attriNum;
	int totalLength;
	Attribute attr[32];

	Table() : blockNum(0), recordNum(0),attriNum(0), totalLength(0){}
};

class Index
{
public:
	string index_name;
	string table_name;
	int blockNum;
	int column;
	int columnLength;
	Index() : column(0), blockNum(0){}
};

class Row
{
public:
	vector<string> columns;
};
class Data
{
public:
	vector<Row> rows;
};
class buffer{
public:
	bool isWritten;
	bool isValid;
	string filename;
	int blockOffset;
	int LRUvalue;
	char values[BLOCKSIZE + 1];
	buffer()
	{
		initialize();
	}
	void initialize()    //初始化buffer变量
	{
		isWritten = 0;
		isValid = 0;
		filename = "NULL";
		blockOffset = 0;
		LRUvalue = 0;
		for (int i = 0; i<BLOCKSIZE; i++) values[i] = EMPTY;
		values[BLOCKSIZE] = '\0';
	}
	string getvalues(int startpos, int endpos)   //将startpos和endpos之间的字符串取出，不包括endpos
	{
		string tmpt = "";
		if (startpos >= 0 && startpos <= endpos && endpos <= BLOCKSIZE)
			for (int i = startpos; i<endpos; i++)
				tmpt += values[i];
		return tmpt;
	}
	char getvalues(int pos)          //得到pos处的一个字符
	{
		if (pos >= 0 && pos <= BLOCKSIZE)
			return values[pos];
		return '\0';
	}

};

class insertPos{
public:
	int bufferNum;       //对应的buffer
	int position;        //插入的位置
};
#endif