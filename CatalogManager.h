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
	void InitialTalbeCatalog()     //��ʼ�����Catalog����ֵ��ر���
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

	void InitialIndexCatalog()     //��ʼ������Catalog����ֵ��ر���
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

	void StoreTalbeCatalog()    //�����Catalog�е���Ϣ����ļ�
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

	void StoreIndexCatalog()    //������Catalog�е���Ϣ����
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

	~CatalogManager()    //�������������ı��˵���Ϣ�洢������
	{
		StoreTalbeCatalog();
		StoreIndexCatalog();
	}
	void createTable(Table& table)  //����tableʱ��CatalogManager�еı����ı�
	{
		tableNum++;
		for (int i = 0; i < table.attriNum; i++){
			table.totalLength += table.attr[i].length;   //����table������ĳ��ȣ��Ա�洢
		}
		tables.push_back(table);  //push��table��������
	}

	void createIndex(Index index)   //����index
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

	void dropTable(string tablename)    //ɾ����ʱ��ɾ��CatalogManager�е�table��Ϣ��Index��Ϣ��ͬʱ�� tableNum-1
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

	void dropIndex(string index_name)     //ɾ������
	{
		for (int i = indexNum - 1; i >= 0; i--){
			if (indexes[i].index_name == index_name){
				indexes.erase(indexes.begin() + i);
				indexNum--;
			}
		}
	}

	void update(Table& tableinfo)   //����table��Ϣ
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

	void update(Index& index)    //����Index
	{
		for (int i = 0; i< indexNum; i++){
			if (indexes[i].index_name == index.index_name){
				indexes[i].table_name = index.table_name;
				indexes[i].column = index.column;
				indexes[i].blockNum = index.blockNum;
			}
		}
	}
	  
	bool ExistTable(string tablename)   //�ж�ĳһ��table�Ƿ����
	{
		int i;
		for (i = 0; i < tables.size(); i++){
			if (tables[i].name == tablename)
				return true;
		}
		return false;
	}

	bool ExistIndex(string tablename, int column)    //�ж�ĳһ�������Ƿ����
	{
		int i;
		for (i = 0; i < indexes.size(); i++){
			if (indexes[i].table_name == tablename && indexes[i].column == column)
				break;
		}
		if (i >= indexes.size()) return 0;
		else return 1;
	}

	bool ExistIndex(string indexname)          //�������������ж�
	{
		int i;
		for (i = 0; i <indexes.size(); i++){
			if (indexes[i].index_name == indexname)
				break;
		}
		if (i >= indexes.size()) return 0;
		else return 1;
	}

	Table gettableinformation(string tablename)   //ͨ��������ֵõ�������Ϣ
	{
		int i;
		Table temp;
		for (i = 0; i<tableNum; i++){
			if ((tables[i].name) == tablename){

				return tables[i];
			}
		}
		return temp;    //���û�����table������һ���յ�table
	}

	Index getIndexInformation(string tablename, int column)     //ͨ��������ֺ�column�õ�index����Ϣ
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

	Index getIndexInformation(string indexName)   //ͨ��Index�����ֵõ�Index��Ϣ
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

	int GetColumnNumber(Table& tableinfo, string columnname)   //ͨ��������ֺ�column�����ֵõ�column��Ӧ������
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
