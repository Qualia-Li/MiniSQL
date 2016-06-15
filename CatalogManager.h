#ifndef __CatalogManager_h__
#define __CatalogManager_h__
#include "stdafx.h"
#include "macro.h"
#include "table.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>



class CatalogManager
{ 
private:
	vector<Table> tables;
	int tableNum;
	vector<Index> indexes; 
	int indexNum;
private:
	void InitialTalbeCatalog()     //初始化表格Catalog，赋值相关变量
	{
		const string filename = "table.catlog";
		fstream  fin(filename.c_str(), ios::in); 
		fin >> tableNum;
		for (int i = 0; i < tableNum; i++)
		{
			Table temp_table;
			fin >> temp_table.name;
			fin >> temp_table.attriNum; 
			fin >> temp_table.blockNum; 
			fin >> temp_table.recordNum;
			int j;
			for (j = 0; j < temp_table.attriNum; j++)
			{
				Attribute temp_attri;  
				fin >> temp_attri.name;
				fin >> temp_attri.type; 
				fin >> temp_attri.length; 
				fin >> temp_attri.isPrimaryKey;
				fin >> temp_attri.isUnique;
				temp_table.attr[j] = temp_attri;
				temp_table.totalLength += temp_attri.length;
			}
			tables.push_back(temp_table);
		}
		fin.close();
	}

	void InitialIndexCatalog()     //初始化索引Catalog，赋值相关变量
	{
		const string filename = "index.catlog";
		fstream  fin(filename.c_str(), ios::in);
		fin >> indexNum;
		for (int i = 0; i < indexNum; i++)
		{
			Index temp_index;
			fin >> temp_index.index_name;
			fin >> temp_index.table_name;
			fin >> temp_index.column;
			fin >> temp_index.columnLength;
			fin >> temp_index.blockNum;
			indexes.push_back(temp_index);
		}
		fin.close();
	}

	void StoreTalbeCatalog()    //将表格Catalog中的信息存回文件
	{
		string filename = "table.catlog";
		fstream  fout(filename.c_str(), ios::out);

		fout << tableNum << endl;
		for (int i = 0; i < tableNum; i++)
		{
			fout << tables[i].name << " ";
			fout << tables[i].attriNum << " "; 
			fout << tables[i].blockNum << " ";
			fout << tables[i].recordNum << endl;
			for (int j = 0; j < tables[i].attriNum; j++)
			{
				fout << tables[i].attr[j].name << " ";
				fout << tables[i].attr[j].type << " ";
				fout << tables[i].attr[j].length << " ";
				fout << tables[i].attr[j].isUnique << " ";
				fout << tables[i].attr[j].isPrimaryKey << endl;
			}
		}
		fout.close();
	}

	void StoreIndexCatalog()    //将索引Catalog中的信息储存
	{
		string filename = "index.catlog";
		fstream  fout(filename.c_str(), ios::out);
		fout << indexNum << endl;
		for (int i = 0; i < indexNum; i++)
		{
			fout << indexes[i].index_name << " ";
			fout << indexes[i].table_name << " ";
			fout << indexes[i].column << " ";
			fout << indexes[i].columnLength << " ";
			fout << indexes[i].blockNum << endl;
		}
		fout.close();
	}
public:
	CatalogManager(){
		InitialTalbeCatalog();
		InitialIndexCatalog();
	}

	~CatalogManager()    //析构函数，将改变了的信息存储进磁盘
	{
		StoreTalbeCatalog();
		StoreIndexCatalog();
	}
	void createTable(Table& table)  //建立table时，CatalogManager中的变量改变
	{
		tableNum++;
		for (int i = 0; i < table.attriNum; i++){
			table.totalLength += table.attr[i].length;   //计算table的整体的长度，以便存储
		}
		tables.push_back(table);  //push到table的向量中
	}

	void createIndex(Index index)   //建立index
	{
		indexNum++;
		indexes.push_back(index);
	}

	void dropTable(Table table)
	{
		dropTable(table.name);
	}

	void dropIndex(Index index)
	{
		dropIndex(index.index_name);
	}

	void dropTable(string tablename)    //删除表时，删除CatalogManager中的table信息和Index信息，同时把 tableNum-1
	{
		for (int i = tableNum - 1; i >= 0; i--)
		{
			if (tables[i].name == tablename){
				tables.erase(tables.begin() + i);
				tableNum--;
			}
		}
		for (int i = indexNum - 1; i >= 0; i--)
		{
			if (indexes[i].table_name == tablename){
				indexes.erase(indexes.begin() + i);
				indexNum--;
			}
		}
	}

	void dropIndex(string index_name)     //删除索引
	{
		for (int i = indexNum - 1; i >= 0; i--){
			if (indexes[i].index_name == index_name){
				indexes.erase(indexes.begin() + i);
				indexNum--;
			}
		}
	}

	void update(Table& tableinfo)   //更新table信息
	{
		for (int i = 0; i < tableNum; i++){
			if (tables[i].name == tableinfo.name){
				tables[i].attriNum = tableinfo.attriNum;
				tables[i].blockNum = tableinfo.blockNum;
				tables[i].recordNum = tableinfo.recordNum;
				tables[i].totalLength = tableinfo.totalLength;
				for (int j = 0; j < tableinfo.attriNum; j++)
					tables[i].attr[j] = tableinfo.attr[j];
			}
		}
	}

	void update(Index& index)    //更新Index
	{
		for (int i = 0; i< indexNum; i++){
			if (indexes[i].index_name == index.index_name){
				indexes[i].table_name = index.table_name;
				indexes[i].column = index.column;
				indexes[i].blockNum = index.blockNum;
			}
		}
	}
	  
	bool ExistTable(string tablename)   //判断某一个table是否存在
	{
		int i;
		for (i = 0; i < tables.size(); i++){
			if (tables[i].name == tablename)
				return true;
		}
		return false;
	}

	bool ExistIndex(string tablename, int column)    //判断某一个索引是否存在
	{
		int i;
		for (i = 0; i < indexes.size(); i++){
			if (indexes[i].table_name == tablename && indexes[i].column == column)
				break;
		}
		if (i >= indexes.size()) return 0;
		else return 1;
	}

	bool ExistIndex(string indexname)          //根据索引名字判断
	{
		int i;
		for (i = 0; i <indexes.size(); i++){
			if (indexes[i].index_name == indexname)
				break;
		}
		if (i >= indexes.size()) return 0;
		else return 1;
	}

	Table gettableinformation(string tablename)   //通过表格名字得到表格的信息
	{
		int i;
		Table temp;
		for (i = 0; i<tableNum; i++){
			if ((tables[i].name) == tablename){

				return tables[i];
			}
		}
		return temp;    //如果没有这个table，返回一个空的table
	}

	Index getIndexInformation(string tablename, int column)     //通过表格名字和column得到index的信息
	{
		int i;
		for (i = 0; i < indexes.size(); i++)
		{
			if (indexes[i].table_name == tablename && indexes[i].column == column)
				break;
		}
		if (i >= indexNum)
		{
			Index tmpt;
			return tmpt;
		}
		return indexes[i];
	}

	Index getIndexInformation(string indexName)   //通过Index的名字得到Index信息
	{
		int i;
		for (i = 0; i < tableNum; i++)
		{
			if (indexes[i].index_name == indexName)
				break;
		}
		if (i >= indexNum)
		{
			Index tmpt;
			return tmpt;
		}
		return indexes[i];
	}

	int GetColumnNumber(Table& tableinfo, string columnname)   //通过表格名字和column的名字得到column对应的数字
	{
		for (int i = 0; i<tableinfo.attriNum; i++){
			if (tableinfo.attr[i].name == columnname){
				return i;
			}
		}
		return -1;
	}
	int GetColumnAmount(Table& tableinfo){
		return tableinfo.attriNum;
	}

	vector<Table> getTables()
	{
		return tables;
	}

	vector<Index> getIndexes()
	{
		return indexes;
	}
};
#endif
