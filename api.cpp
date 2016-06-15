#include "stdafx.h"
#include "table.h"
#include "macro.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "IndexManager.h"
#include "commandcut.h"
#include "Interpret.h"
#include <iostream>
#include <fstream>
#include "api.h"

using namespace std;

extern BufferManager bufferMa;
extern RecordManager record;
extern CatalogManager catalog;
extern Interpret parsetree;
extern IndexManager index;


string api::ExeExpr()
{
	string answer = "";
	int i;
	int j;
	int k;

	Table tableinfor;
	Index indexinfor;

	string tempKeyValue;
	int tempPrimaryPosition = -1;
	int rowCount = 0;

	Data data;

	

	if (parsetree.m_nOperation == EXE_FILE)
	{
		fstream fin;
		int i = 0;
		int j = 0;
		string temp;
		string command = "";
		fin.open(parsetree.file_name, ios::in|ios::out);
		while (fin.peek()!= EOF )
		{	
			command = "";
			while (1)
			{
				fin >> temp;
				char * tchar;
				tchar = new char[temp.length() + 2];
				for (int j = 0; j < temp.length(); j++)
					tchar[j] = temp[j];

				tchar[temp.length()] = '\0';
				i++;
				
				if (isExprEnd(tchar))
				{
					string s(tchar);
					command = command + s;
					break;
				}
				command += temp;
				command += " ";
				delete tchar;
			}
		
			parsetree.parse(command);
			ExeExpr();
			cout << j << endl;
			j++;
			
		}
		cout << endl;
		answer = "File operation is done!";
		fin.close();
	}
	else
	{
		switch (parsetree.m_nOperation)
		{
		case FILE_NOT_FOUND:
			answer = parsetree.file_name + " cannot be found!";
			break;
		case CREATE_TABLE:
			parsetree.table_info.attriNum = parsetree.table_info.attriNum;
			catalog.createTable(parsetree.table_info);
			record.createTable(parsetree.table_info);
			tableinfor = parsetree.table_info;
			indexinfor = parsetree.index_info;
			
			if (parsetree.PrimaryKeyPos != -1)
				if (!tableinfor.attr[indexinfor.column].isPrimaryKey&&!tableinfor.attr[indexinfor.column].isUnique){
				answer = "Column " + tableinfor.attr[indexinfor.column].name + "  is a not unique key.";
				break;
				}
			catalog.createIndex(indexinfor);
			index.createIndex(tableinfor, indexinfor);
			catalog.update(indexinfor);
			answer = "Table " + parsetree.table_info.name + " has been created successfully!";
			break;
		case TABLE_EXIST:
			answer = "The table has been created.";
			break;
		case DROP_TABLE:
			record.dropTable(parsetree.table_info);
			for (int i = 0; i < parsetree.table_info.attriNum; i++){//把这各表所有的index都删掉
				indexinfor = catalog.getIndexInformation(parsetree.table_info.name, i);
				if (indexinfor.index_name != "")
					index.dropIndex(indexinfor);
			}
			catalog.dropTable(parsetree.table_info);
			answer = "Table " + parsetree.table_info.name + " has been dropped successfully!";
			break;
		case INSERT:
			tableinfor = parsetree.table_info;
			record.insertValue(tableinfor, parsetree.data);
			tableinfor.recordNum++;
			catalog.update(tableinfor);
			answer = "One record has been inserted successfully";
			break;
		case PRIMARY_RAID:			
			answer = "Primary Key Redundancy occurs, thus insertion failed";
			break;
	
		case UNIQUE_RAID:
			answer = "Unique Value Redundancy occurs, thus insertion failed";
			break;
		case INSERT_ERR:
			answer = "Incorrect usage of \"insert\" query! Please check your input!";
			break;
		case COMMIT:
			tableinfor = parsetree.table_info;
			for (int i = 0; i < parsetree.table_info.attriNum; i++)
			{
				if ((parsetree.table_info.attr[i].isPrimaryKey == true || parsetree.table_info.attr[i].isUnique == true) )
				{
					tempPrimaryPosition = i;
					indexinfor = catalog.getIndexInformation(tableinfor.name, i);
					index.updateIndex(tableinfor, indexinfor);
					break;
				}
			}
			answer = "Commit operation is successful!";
			break;
		case SELECT_NOWHERE:
			tableinfor = parsetree.table_info;
			data = record.select(tableinfor);
			if (data.rows.size() != 0)
				answer = ShowResult(data, tableinfor, parsetree.column);
			else{
				answer = "No data is found!!!";
			}
			break;
		case SELECT_WHERE:
			
			tableinfor = parsetree.table_info;
			
			if (parsetree.condition.size() == 1){
				for (int i = 0; i < parsetree.table_info.attriNum; i++){
					if ((parsetree.table_info.attr[i].isPrimaryKey == true || parsetree.table_info.attr[i].isUnique == true) && parsetree.m_colname == parsetree.table_info.attr[i].name){
						tempPrimaryPosition = i;
						indexinfor = catalog.getIndexInformation(tableinfor.name, i);
						break;
					}
				}
				if (tempPrimaryPosition == parsetree.condition[0].columnNum&&parsetree.condition[0].op == Eq&&indexinfor.table_name != ""){

					tempKeyValue = parsetree.condition[0].value;
					data = index.selectEqual(tableinfor, indexinfor, tempKeyValue);
				}
				else{

					data = record.select(tableinfor, parsetree.condition);
				}
			}
			else{
				data = record.select(tableinfor, parsetree.condition);
			}
			if (data.rows.size() != 0)
				answer = ShowResult(data, tableinfor, parsetree.column);
			else{
				answer = "No data is found!!!";
			}
			break;
		case DELETE_R:
			rowCount = record.deleteValue(parsetree.table_info, parsetree.condition);
			char * temp;
			answer = NumToStr(rowCount);
			answer += " rows have been deleted.";
			break;
		case DELETE_WHERE:
			rowCount = record.deleteValue(parsetree.table_info, parsetree.condition);
			answer = NumToStr(rowCount) + " rows have been deleted.";
			break;
		case CREATE_INDEX:
			tableinfor = parsetree.table_info;
			indexinfor = parsetree.index_info;
			if (!tableinfor.attr[indexinfor.column].isPrimaryKey&&!tableinfor.attr[indexinfor.column].isUnique){//不是primary key，不可以建index
				answer = "Column " + tableinfor.attr[indexinfor.column].name + "  is not unique.";
				break;
			}
			catalog.createIndex(indexinfor);
			index.createIndex(tableinfor, indexinfor);
			catalog.update(indexinfor);
			answer = "The index " + indexinfor.index_name + "has been created successfully";
			break;
		case INDEX_ERR:
			answer = "The index on primary key of table has been existed";
			break;
		case DROP_INDEX:
			indexinfor = catalog.getIndexInformation(parsetree.m_indexname);
			if (indexinfor.index_name == ""){
				answer = "Index " + parsetree.m_indexname + " does not exist!";
			}
			index.dropIndex(indexinfor);
			catalog.dropIndex(parsetree.m_indexname);
			answer = "The index has been dropped successfully";
			break;
		case CREATE_INDEX_ERR:
			answer = "Incorrect usage of \"create index\" query! Please check your input!";
			break;
		case QUIT:
			answer = "Have a good day! Press any key to close this window.";
			getchar();
			exit(0);
			break;
		case EMPTY_QUERY:
			answer = "Empty query! Please enter your command!";
			break;
		case UNKNOWN:
			answer = "Unknown query! Please check your input!";
			break;
		case SELECT_ERR:
			answer = "Incorrect usage of \"select\" query! Please check your input!";
			break;
		case CREATE_TABLE_ERR:
			answer = "Incorrect usage of \"create table\" query! Please check your input!";
			break;
		case DELETE_ERR:
			answer = "Incorrect usage of \"delete from\" query! Please check your input!";
			break;
		case DROP_TABLE_ERR:
			answer = "Incorrect usage of \"drop table\" query! Please check your input!";
			break;
		case DROP_INDEX_ERR:
			answer = "Incorrect usage of \"drop index\" query! Please check your input!";
			break;
		case VOID_PRIMARY:
			answer = "Error: invalid primary key! Please check your input!";
			break;
		case VOID_UNI:
			answer = "Error: invalid unique key! Please check your input!";
			break;
		case CHAR_BOUND:
			answer = "Error: only 1~255 charactors is allowed! Please check your input!";
			break;
		case NO_PRIMARY_KEY:
			answer = "No primary key is defined! Please check your input!";
			break;
		case TABLE_ERR:
			answer = "Table is not existed,please check the database";
			break;
		case INDEX_NOT_EXIST:
			answer = "Index is not existed,please check the database";
			break;
		case COLUMN_ERR:
			answer = "One column is not existed";
			break;
		case INSERT_NUM_ERR:
			answer = "The column number is not according to the columns in our database";
			break;
		}
	}

	return answer;
}

string api::InputAndProcess()
{
	char command[COMLEN] = "";
	char input[INPUTLEN] = "";
	char word[WORDLEN] = "";
	short int ComEnd = 0;
	int len;

	while (ComEnd == 0)
	{
		//get(input);
		gets_s(input);
		if (isExprEnd(input))
			ComEnd = 1;
		//strcat(command, input);
		strcat_s(command, input);
		len = strlen(command);
		command[len] = ' ';
		command[len + 1] = '\0';
	}
	string in(command);
	return in;
}

bool api::isExprEnd(char * input)
{
	unsigned int next = strlen(input) - 1;
	char prev = ' ';
	while (next >= 0 && (prev == '\t' || prev == ' '))
	{
		prev = input[next];
		next--;
	}
	if (prev == ';')
	{
		input[next + 1] = ' ';
		input[next + 1] = '\0';
		return 1;
	}
	return 0;
}

string api::ShowResult(Data data, Table tableinfor, vector<Attribute> column)
{
	string ans = "";
	if (column[0].name == "*"){
		ans +=  "\r\n";
		ans += "+";
		for (int i = 0; i < tableinfor.attriNum; i++){
			for (int j = 0; j < tableinfor.attr[i].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\r\n";
		ans += "| ";
		for (int i = 0; i < tableinfor.attriNum; i++){
			ans += tableinfor.attr[i].name;
			int lengthLeft = tableinfor.attr[i].length - tableinfor.attr[i].name.length();
			for (int j = 0; j < lengthLeft; j++){
				ans +=  ' ';
			}
			ans +=  "| ";
		}
		ans += "\r\n";
		ans += "+";
		for (int i = 0; i < tableinfor.attriNum; i++){
			for (int j = 0; j < tableinfor.attr[i].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\r\n";

		//内容
		for (int i = 0; i < data.rows.size(); i++){
			ans += "| ";
			for (int j = 0; j < tableinfor.attriNum; j++){
				int lengthLeft = tableinfor.attr[j].length;
				for (int k = 0; k < data.rows[i].columns[j].length(); k++){
					if (data.rows[i].columns[j].c_str()[k] == EMPTY) break;
					else{
						ans += data.rows[i].columns[j].c_str()[k];
						lengthLeft--;
					}
				}
				for (int k = 0; k < lengthLeft; k++) ans += " ";
				ans += "| ";
			}
			ans += "\r\n";
		}

		ans += "+";
		for (int i = 0; i < tableinfor.attriNum; i++){
			for (int j = 0; j < tableinfor.attr[i].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\r\n";
	}
	else{
		ans += "\r\n" ;
		ans += "+";
		for (int i = 0; i < column.size(); i++){
			int col;
			for (col = 0; col < tableinfor.attriNum; col++){
				if (tableinfor.attr[col].name == column[i].name) break;
			}
			for (int j = 0; j < tableinfor.attr[col].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\r\n";
		ans += "| ";
		for (int i = 0; i < column.size(); i++){
			int col;
			for (col = 0; col < tableinfor.attriNum; col++){
				if (tableinfor.attr[col].name == column[i].name) break;
			}
			ans += tableinfor.attr[col].name;
			int lengthLeft = tableinfor.attr[col].length - tableinfor.attr[col].name.length();
			for (int j = 0; j < lengthLeft; j++){
				ans += ' ';
			}
			ans += "| ";
		}
		ans += "\r\n";
		ans += "+";
		for (int i = 0; i < column.size(); i++){
			int col;
			for (col = 0; col < tableinfor.attriNum; col++){
				if (tableinfor.attr[col].name == column[i].name) break;
			}
			for (int j = 0; j < tableinfor.attr[col].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\n";

		//内容
		for (int i = 0; i < data.rows.size(); i++){
			ans += "| ";
			for (int j = 0; j < column.size(); j++){
				int col;
				for (col = 0; col < tableinfor.attriNum; col++){
					if (tableinfor.attr[col].name == column[j].name) break;
				}
				int lengthLeft = tableinfor.attr[col].length;
				for (int k = 0; k < data.rows[i].columns[col].length(); k++){
					if (data.rows[i].columns[col].c_str()[k] == EMPTY) break;
					else{
						ans += data.rows[i].columns[col].c_str()[k];
						lengthLeft--;
					}
				}
				for (int k = 0; k < lengthLeft; k++) ans +=  " ";
					ans += "| ";
			}
			ans += "\r\n";
		}

		ans += "+";
		for (int i = 0; i < column.size(); i++){
			int col;
			for (col = 0; col < tableinfor.attriNum; col++){
				if (tableinfor.attr[col].name == column[i].name) break;
			}
			for (int j = 0; j < tableinfor.attr[col].length + 1; j++){
				ans += "-";
			}
			ans += "+";
		}
		ans += "\r\n";
	}
	ans += NumToStr(data.rows.size()) + " rows have been found.";
	ans += "\r\n";
	return ans;
}