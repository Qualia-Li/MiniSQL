#include "stdafx.h"
#include <iostream>
#include "table.h"
#include "macro.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "commandcut.h"
#include "Interpret.h"


using namespace std;

extern RecordManager record;

void Interpret::Initialize()
{
	m_nOperation = UNKNOWN;
	m_tablename = "";
	
	PrimaryKeyPos = -1;
	UniquePos = -1;
	InitColumn();
	InitCondition();
	InitData();
	InitTableinfo();
	InitIndexinfo();
	cmd.n = 0;
}

void Interpret::InitColumn()
{
	if (column.size() > 0)
		column.clear();
}

void Interpret::InitCondition()
{
	if (condition.size() > 0)
		condition.clear();
	if (UniqueCondition.size() > 0)
		UniqueCondition.clear();
}

void Interpret::InitData()
{
	if (data.columns.size() > 0)
		data.columns.clear();
}

void Interpret::InitTableinfo()
{
	table_info.attriNum = 0;
	table_info.blockNum = 0;
	table_info.name = "";
	table_info.recordNum = 0;
	table_info.totalLength = 0;
}

void Interpret::InitIndexinfo()
{
	index_info.index_name = "";
	index_info.table_name = "";
	index_info.column = -1;
	index_info.columnLength = 0;
	index_info.blockNum = 0;
}



void Interpret::parse(string & command)
{
	Initialize();
	getCommand(command);
	for (int i = 0; i < cmd.n; i++){
		if (cmd.str[i] == "\'"){
			i++;
			while (cmd.str[i] != "\'" && cmd.str[i+1] != "\'")
			{
				cmd.str[i] += " ";
				cmd.str[i] += cmd.str[i + 1];
				for (int j = i + 1; j < cmd.n; j++)
					cmd.str[j] = cmd.str[j + 1];
				cmd.n--;
			}
			if (cmd.str[i] != "\'")
				i++;
		}
	}
	if (cmd.n == 0)
	{
		m_nOperation = EMPTY_QUERY;
		return;
	}
	else if (cmd.str[0] == "create")
		CreateParse(cmd);
	else if (cmd.str[0] == "delete")
	{
		if (cmd.n == 1)
			return;
		if (cmd.str[1] != "from")
			return;
		DeleteParse(cmd);
	}
	else if (cmd.str[0] == "drop")
		DropParse(cmd);
	else if (cmd.str[0] == "insert")
	{
		if (cmd.n == 1)
			return;
		if (cmd.str[1] != "into")
			return;
		InsertParse(cmd);
	}
	else if (cmd.str[0] == "select")
		SelectParse(cmd);
	else if (cmd.str[0] == "quit")
	{
		if (cmd.n==1)
			m_nOperation = QUIT;
		return;
	}
	else if (cmd.str[0] == "commit")
	{
		if (cmd.n == 1)
			return;
		m_tablename = cmd.str[1];
		if (!catalog.ExistTable(m_tablename))
		{
			m_nOperation = TABLE_ERR;
			return;
		}
		table_info = catalog.gettableinformation(m_tablename);
		if (cmd.n == 2)
			m_nOperation = COMMIT;
		return;
	}
	else if (cmd.str[0] == "execfile")
		ExecFileParse(cmd);
	else
		return;
}

void Interpret::CreateParse(Command & cmd)
{
	int indexStr = 1;

	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] == "table")
		CreateTableParse(cmd);
	if (cmd.str[indexStr] == "index")
		CreateIndexParse(cmd);
	return;
}

void Interpret::CreateTableParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	int indexStr = 2;

	m_nOperation = CREATE_TABLE_ERR;

	if (cmd.n == indexStr)
		return;
	m_tablename = cmd.str[indexStr];
	if (catalog.ExistTable(m_tablename))
	{
		m_nOperation = TABLE_EXIST;
		return;
	}
	table_info.name = m_tablename;
	
	indexStr++;

	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != "(")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] == "unique" || cmd.str[indexStr] == "primary")
		return;
	while (cmd.n > indexStr)
	{
		tempAttr.isUnique = tempAttr.isPrimaryKey = 0;
		if(cmd.str[indexStr] == "unique")   //如果是unique语句
		{
			int temp = CheckUnique(indexStr);
			if (temp == -2)
			{
				m_nOperation = VOID_UNI;
				return;
			}
			if (temp == -1)
				return;
			indexStr++;
		}
		else if(cmd.str[indexStr] == "primary")  //如果是primary key语句
		{
			int temp = CheckPrimary(indexStr);
			if (temp == -2)
			{
				m_nOperation = VOID_PRIMARY;
				return;
			}
			if (temp == -1)
				return;
			indexStr++;
		}
		else
		{
			tempAttr.name = cmd.str[indexStr];
			indexStr++;
			if (cmd.n == indexStr)
				return;
			int temp = GetCmdType(indexStr);
			if (temp == -1)
				return;
			if (temp == INT)
			{
				tempAttr.type = INT;
				tempAttr.length = 11;
				indexStr++;
				if (cmd.n == indexStr)
					return;
				if (cmd.str[indexStr] == "unique")
				{
					tempAttr.isUnique = 1;
					indexStr++;
				}
			}
			else if (temp == FLOAT)
			{
				tempAttr.type = FLOAT;
				tempAttr.length = 10;
				indexStr++;
				if (cmd.n == indexStr)
					return;
				if (cmd.str[indexStr] == "unique")
				{
					tempAttr.isUnique = 1;
					indexStr++;
				}
			}
			else if (temp == CHARN)
			{
				tempAttr.type = CHARN;
				tempAttr.length = atoi(cmd.str[indexStr - 1].c_str()) + 1;
				if (tempAttr.length > 256 || tempAttr.length < 2)
				{
					m_nOperation = CHAR_BOUND;
					return;
				}
				indexStr++;
				if (cmd.n == indexStr)
					return;
				if (cmd.str[indexStr] == "unique")
				{
					tempAttr.isUnique = 1;
					indexStr++;
				}
			}
			table_info.attr[table_info.attriNum++] = tempAttr;
		}
		if (cmd.n == indexStr)
			return;
		if (cmd.str[indexStr] == ")")
		{
			if (cmd.n == indexStr + 1)
				m_nOperation = CREATE_TABLE;
			return;
		}
		if (cmd.str[indexStr] != ",")
			return;
		indexStr++;
	}
}

void Interpret::CreateIndexParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	short int FindPriKey = 0, FindUniKey = 0;
	int indexStr = 2;
	m_indexname = "";

	m_nOperation = CREATE_INDEX_ERR;
	if (cmd.n == indexStr)
		return;
	m_indexname = cmd.str[indexStr];
	if (catalog.ExistIndex(m_indexname))
	{
		m_nOperation = INDEX_ERR;
		return;
	}
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != "on")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	m_tablename = cmd.str[indexStr];
	if (!catalog.ExistTable(m_tablename))
	{
		m_nOperation = TABLE_ERR;
		return;
	}
	table_info = catalog.gettableinformation(m_tablename);
	index_info.table_name = m_tablename;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != "(")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	int tempint;
	tempint = catalog.GetColumnNumber(table_info, cmd.str[indexStr]);
	if (tempint == -1)
	{
		m_nOperation = COLUMN_ERR;
		return;
	}
	if (catalog.ExistIndex(m_tablename, tempint))
	{
		m_nOperation = INDEX_ERR;
		return;
	}
	index_info.column = tempint;
	index_info.blockNum = 0;
	index_info.columnLength = table_info.attr[tempint].length;
	index_info.index_name = cmd.str[indexStr];
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != ")")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		m_nOperation = CREATE_INDEX;
	return;
}

void Interpret::DeleteParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	string tempRow;
	short int FindPriKey = 0, FindUniKey = 0;
	int indexStr=2;

	m_nOperation = DELETE_ERR;
	if (cmd.n == indexStr)
		return;
	m_tablename = cmd.str[indexStr];
	if (!catalog.ExistTable(m_tablename))
	{
		m_nOperation = TABLE_ERR;
		return;
	}
	table_info = catalog.gettableinformation(m_tablename);
	indexStr++;
	if (cmd.n == indexStr)
	{
		m_nOperation = DELETE_R;
		return;
	}
	if (cmd.str[indexStr] != "where")
		return;
	indexStr++;
	while (cmd.n>indexStr)
	{
		m_colname = cmd.str[indexStr];
		tempCon.columnNum = catalog.GetColumnNumber(table_info, m_colname);
		if (tempCon.columnNum == -1)
		{
			m_nOperation = COLUMN_ERR;
			return;
		}
		indexStr++;
		if (cmd.n == indexStr)
			return;
		tempCon.op = JudgeSymbol(cmd.str[indexStr]);
		if (tempCon.op == NOTANY)
			return;
		indexStr++;
		if (cmd.n == indexStr)
			return;
		int temptype;
		temptype = JudgeDataType(indexStr);
		if (table_info.attr[tempCon.columnNum].type == FLOAT && temptype == INT)
			temptype = FLOAT;
		if (temptype != table_info.attr[tempCon.columnNum].type)
			return;
		if (temptype == CHARN)
			tempCon.value = cmd.str[indexStr - 1];
		else
			tempCon.value = cmd.str[indexStr];

		condition.push_back(tempCon);
		indexStr++;
		if (cmd.n == indexStr)
		{
			m_nOperation = DELETE_WHERE;
			return;
		}
		if (cmd.str[indexStr] != "and")
			return;
		indexStr++;
	}
	return;
}

void Interpret::DropParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	string tempRow;
	short int FindPriKey = 0, FindUniKey = 0;
	int indexStr=1;
	m_indexname = "";

	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] == "table")
	{
		m_nOperation = DROP_TABLE_ERR;
		indexStr++;
		if (cmd.n == indexStr)
			return;
		m_tablename = cmd.str[indexStr];
		if (!catalog.ExistTable(m_tablename))
		{
			m_nOperation = TABLE_ERR;
			return;
		}
		table_info = catalog.gettableinformation(m_tablename);
		indexStr++;
		if (cmd.n == indexStr)
			m_nOperation = DROP_TABLE;
		return;
	}
	else if (cmd.str[indexStr] == "index")
	{
		m_nOperation = DROP_INDEX_ERR;
		indexStr++;
		if (cmd.n == indexStr)
			return;
		m_indexname = cmd.str[indexStr];
		if (!catalog.ExistIndex(m_indexname))
		{
			m_nOperation = INDEX_NOT_EXIST;
			return;
		}
		indexStr++;
		if (cmd.n == indexStr)
			m_nOperation = DROP_INDEX;
		return;
	}
}

void Interpret::InsertParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	string tempRow;
	int indexStr = 2;

	m_nOperation = INSERT_ERR;
	if (cmd.n == indexStr)
		return;
	m_tablename = cmd.str[indexStr];
	if (!catalog.ExistTable(m_tablename))
	{
		m_nOperation = TABLE_ERR;
		return;
	}
	table_info = catalog.gettableinformation(m_tablename);
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != "values")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	if (cmd.str[indexStr] != "(")
		return;
	indexStr++;
	int i = 0;
	while (cmd.n > indexStr)
	{
		int datatype = JudgeDataType(indexStr);
		if (table_info.attr[i].type == FLOAT && datatype == INT)
			datatype = FLOAT;

		if (datatype != table_info.attr[i].type)
			return;
		if (datatype == CHARN)
		{
			tempRow = cmd.str[indexStr - 1];
			if (tempRow.length() > table_info.attr[i].length - 1)
				return;
		}
		else
			tempRow = cmd.str[indexStr];
		data.columns.push_back(tempRow);
		indexStr++;
		if (cmd.n == indexStr)
			return;
		if (cmd.str[indexStr] == ")")
			break;
		if (cmd.str[indexStr] != ",")
			return;
		indexStr++;

		i++;
	}
	if (data.columns.size() != table_info.attriNum)
	{
		m_nOperation = INSERT_NUM_ERR;
		return;
	}

	
	for (int i = 0; i<table_info.attriNum; i++)
	{
		if (table_info.attr[i].isUnique)
		{
			if (table_info.attr[i].isPrimaryKey)
			{
				if (PrimaryKeyValue[0] != table_info.name)
				{
					Data tableValue = record.select(table_info);
					PrimaryKeyValue[0] = table_info.name;
					for (int j = 0; j < tableValue.rows.size(); j++)
						PrimaryKeyValue[j + 1] = tableValue.rows[j].columns[i];
				}
				for (int j = 0; j < table_info.recordNum;j++)
					if (PrimaryKeyValue[j + 1] == data.columns[i])
					{
					m_nOperation = PRIMARY_RAID;
					return;
					}
				PrimaryKeyValue[table_info.recordNum+1] = data.columns[i];
			}
			if (table_info.attr[i].isPrimaryKey != 1 && table_info.attr[i].isUnique)
			{
				if (UniqueKeyValue[0] != table_info.name)
				{
					Data tableValue = record.select(table_info);
					table_info.recordNum = tableValue.rows.size();
					UniqueKeyValue[0] = table_info.name;
					for (int j = 0; j < tableValue.rows.size(); j++)
						UniqueKeyValue[j + 1] = tableValue.rows[j].columns[i];
				}
				for (int j = 0; j < table_info.recordNum;j++)
					if (UniqueKeyValue[j + 1] == data.columns[i])
					{
					m_nOperation = UNIQUE_RAID;
					return;
					}
				
				UniqueKeyValue[table_info.recordNum+1] = data.columns[i];
			}
		}
	}
	indexStr++;
	if (cmd.n == indexStr)
		m_nOperation = INSERT;
	return;
}

void Interpret::SelectParse(Command & cmd)
{
	Attribute tempAttr;
	Condition tempCon;
	string tempRow;
	short int FindPriKey = 0, FindUniKey = 0;
	int indexStr = 1;
	vector<Attribute>::iterator it;

	m_nOperation = SELECT_ERR;
	while (cmd.n > indexStr)
	{
		tempAttr.name = cmd.str[indexStr];
		column.push_back(tempAttr);
		indexStr++;
		if (cmd.n == indexStr)
			return;
		if (cmd.str[indexStr] != ",")
			break;
		indexStr++;
	}

	if ( cmd.str[indexStr] != "from")
		return;
	indexStr++;
	if (cmd.n == indexStr)
		return;
	m_tablename = cmd.str[indexStr];
	if (!catalog.ExistTable(m_tablename))
	{
		m_nOperation = TABLE_ERR;
		return;
	}
	table_info = catalog.gettableinformation(m_tablename);
	for(it = column.begin(); it != column.end(); it++)
	{
			m_nOperation = COLUMN_ERR;
		if (column.size() == 1 && (*it).name == "*")
			break;
		else if (catalog.GetColumnNumber(table_info, it->name) == -1)
			return;
	}

	indexStr++;
	if (cmd.n == indexStr)
	{
		m_nOperation = SELECT_NOWHERE;
		return;
	}

	if (cmd.str[indexStr] != "where")
		return;
	indexStr++;
	while (cmd.n>indexStr)
	{
		m_colname = cmd.str[indexStr];
		tempCon.columnNum = catalog.GetColumnNumber(table_info, m_colname);
		if (tempCon.columnNum == -1)
		{
			return;
		}
		indexStr++;
		if (cmd.n == indexStr)
			return;
		tempCon.op = JudgeSymbol(cmd.str[indexStr]);
		if (tempCon.op == NOTANY)
			return;
		indexStr++;
		if (cmd.n == indexStr)
			return;
		int temptype;
		temptype = JudgeDataType(indexStr);
		if (table_info.attr[tempCon.columnNum].type == FLOAT && temptype == INT)
			temptype = FLOAT;
		if (temptype != table_info.attr[tempCon.columnNum].type)
			return;
		if (temptype == CHARN)
			tempCon.value = cmd.str[indexStr - 1];
		else
			tempCon.value = cmd.str[indexStr];

		condition.push_back(tempCon);
		indexStr++;
		if (cmd.n == indexStr)
		{
			m_nOperation = SELECT_WHERE;
			return;
		}
		if (cmd.str[indexStr] != "and")
			return;
		indexStr++;
	}
	return;
}

void Interpret::ExecFileParse(Command & cmd)
{
	file_name = "";
	int indexStr = 1;
	m_nOperation = FILE_NOT_FOUND;
	if (cmd.n == indexStr)
		return;
	file_name = cmd.str[indexStr];
	indexStr++;
	fstream fin(file_name.c_str(), ios::in|ios::_Nocreate);
	if (!fin)
		return;
	fin.close();
	if (cmd.n == indexStr)
	{
		m_nOperation = EXE_FILE;
		return;
	}
}

Comparison Interpret::JudgeSymbol(string &s)
{
	if (s == "=")
		return Eq;
	else if (s == "<")
		return Lt;
	else if (s == ">")
		return Gt;
	else if (s == ">=")
		return Ge;
	else if (s == "<=")
		return Le;
	else if (s == "<>")
		return Ne;
	return NOTANY;
}

int Interpret::JudgeDataType(int & indexStr)
{
	if (cmd.str[indexStr] == "\'")
	{
		indexStr++;
		if (cmd.n == indexStr)
			return -1;
		indexStr++;
		if (cmd.n == indexStr)
			return -1;
		if (cmd.str[indexStr] != "\'")
			return -1;
		return CHARN;
	}
	else
	{	
		if (isInt(cmd.str[indexStr]))
			return INT;
		if (isFloat(cmd.str[indexStr]))
			return FLOAT;
	}
	return -1;
}

bool Interpret::isInt(string & input)
{
	int i;
	int length = input.length();
	if (!isdigit(input[0]) && input[0] != '-')
		return false;
	for (i = 1; i < length; i++)
		if (!isdigit(input[i]))
			return false;

	return true;
}

bool Interpret::isFloat(string & input)
{
	int dot = 0;
	int i;
	int length = input.length();
	if (!isdigit(input[0]) && !(input[0] == '-'))
		return false;
	for (i = 1; i < length; i++)
	{
		if (!isdigit(input[i]) && input[i] != '.')
			return false;
		else if (input[i] == '.')
			switch (dot)
		{
			case 0:
				dot++;
				break;
			default:
				return false;
		}
	}
	return true;
}

int Interpret::GetCmdType(int & indexStr)   //获取数据类型
{
	if (cmd.str[indexStr] == "int")
		return INT;
	if (cmd.str[indexStr] == "float")
		return FLOAT;
	if(cmd.str[indexStr] == "char")
	{
		indexStr++;
		if (cmd.n == indexStr)
			return -1;
		if (cmd.str[indexStr] != "(")
			return -1;
		indexStr++;
		if (cmd.n == indexStr)
			return -1;
		if (! isInt(cmd.str[indexStr]))
			return -1;
		indexStr++;
		if (cmd.n == indexStr)
			return -1;
		if (cmd.str[indexStr] != ")")
			return -1;

		return CHARN;
	}
	return -1;
}

int Interpret::CheckPrimary(int & indexStr)  //查看primary key语句是否正确
{
	int i;

	if (cmd.str[indexStr] != "primary")
		return -1;
	indexStr++;
	if (cmd.n == indexStr)
		return -1;
	if (cmd.str[indexStr] != "key")
		return -1;
	indexStr++;
	if (cmd.n == indexStr)
		return -1;
	if (cmd.str[indexStr] != "(")
		return -1;
	indexStr++;
	if (cmd.n == indexStr)
		return -1;
	for (i = 0; i < table_info.attriNum;i++)
		if (table_info.attr[i].name == cmd.str[indexStr])
		{
		table_info.attr[i].isPrimaryKey = 1;
		table_info.attr[i].isUnique = 1;
		index_info.column = i;
		index_info.index_name = table_info.attr[i].name;
		index_info.columnLength = table_info.attr[i].length;
		index_info.table_name = table_info.name;
		break;
		}
	if (i == table_info.attriNum)
		return -2;
	indexStr++;

	if (cmd.n == indexStr)
		return -1;
	if (cmd.str[indexStr] != ")")
		return -1;
	return i;
}

int Interpret::CheckUnique(int & indexStr)
{
	int i;

	if (cmd.str[indexStr] != "unique")
		return -1;
	indexStr++;
	if (cmd.n == indexStr)
		return -1;
	if (cmd.str[indexStr] != "(")
		return -1;
	indexStr++;
	if (cmd.n == indexStr)
		return -1;
	for (i = 0; i < table_info.attriNum; i++)
		if (table_info.attr[i].name == cmd.str[indexStr])
		{
		table_info.attr[i].isUnique = 1;
		UniquePos = i;
		break;
		}
	if (i == table_info.attriNum)
		return -2;
	indexStr++;

	if (cmd.n == indexStr)
		return -1;
	if (cmd.str[indexStr] != ")")
		return -1;
	return i;
}

