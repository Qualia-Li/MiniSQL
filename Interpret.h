#ifndef __interpret__h__
#define __interpret__h__
#include "stdafx.h"

class Condition
{                     //whereÓï¾äµÄÌõ¼þ
public:
	Comparison op;
	int columnNum;
	string value;
};

extern CatalogManager catalog;

class Interpret
{
public:
	
	Command cmd;
	OPERATION m_nOperation;
	string m_tablename;
	string m_indexname;
	string m_colname;
	string file_name;
	vector<Attribute> column;
	vector<Condition> condition;
	
	Row data;
	Table table_info;
	Index index_info;
	int PrimaryKeyPos;
	int UniquePos;
	vector<Condition> UniqueCondition;
	string PrimaryKeyValue[100000];
	string UniqueKeyValue[100000];
public:
	Interpret()
	{
		m_nOperation = UNKNOWN;
		m_tablename = "";
		m_indexname = "";
		m_colname = "";
		file_name = "";
		PrimaryKeyPos = -1;
		UniquePos = -1;
		cmd.n = 0;
	}
	~Interpret(){}
	void getCommand(string & src)
	{
		cmd.strcut(src);
	}
	
	void parse(string & command);
	void InitColumn();
	void InitCondition();
	void InitData();
	void InitTableinfo();
	void InitIndexinfo();

	void Initialize();
	void CreateParse(Command & cmd);
	void CreateTableParse(Command & cmd);
	void CreateIndexParse(Command & cmd);
	void DeleteParse(Command & cmd);
	void DropParse(Command & cmd);
	void InsertParse(Command & cmd);
	void SelectParse(Command & cmd);
	void ExecFileParse(Command & cmd);

	Comparison JudgeSymbol(string &s);
	int JudgeDataType(int &indexStr);
	int GetCmdType(int & indexStr);
	bool isInt(string & input);
	bool isFloat(string & input);
	int CheckPrimary(int &indexStr);
	int CheckUnique(int & indexStr);
};


#endif
