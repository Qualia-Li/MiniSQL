#include "stdafx.h"
#ifndef __API__H__
#define __API__H__

extern BufferManager bufferMa;
extern RecordManager record;
extern CatalogManager catalog;
extern Interpret parsetree;
extern IndexManager index;
class api
{

public:
	api(){}
	~api(){}
	string ExeExpr( );
	string ShowResult(Data data, Table tableinfor, vector<Attribute> column);
	string InputAndProcess();
	bool isExprEnd(char *);
	string AttrData[2][10000];
	
	string NumToStr(int i)
	{
		char number[12];
		sprintf_s(number, "%d", i);
		string str(number);
		return str;
	}
};

#endif