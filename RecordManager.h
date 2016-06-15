#ifndef _RECORD_H_
#define _RECORD_H_

#define CHAR 3
#define INTLENGTH 11
#define FLOATLENGTH 10
#define INTLEN		11
#define FLOATLEN	10

#define NOTEMPTY '1'

#include "BufferManager.h"
#include "table.h"
#include "macro.h"
#include <stdio.h>
#include <vector>
#include <sstream>
#include <string>
#include "commandcut.h"
#include "Interpret.h"

extern BufferManager bufferMa;

class RecordManager
{
public:
		
	bool Comparator(Table tableinfor, Row row, vector<Condition> conditions)	//比较函数：输入一个Table的类、 row和条件（向量），输出是否满足条件0/1
	{
		for(int i = 0; i<conditions.size(); i++){
			int colNum = conditions[i].columnNum;
			string value1 = "";
			string value2 = conditions[i].value;
			int length1 =0;
			int length2 = value2.length();
			for(int k =0; k < row.columns[colNum].length(); k++){
				if(row.columns[colNum].c_str()[k] == '$'){
					length1 = k;
					break;
				}
				value1 += row.columns[colNum].c_str()[k];
			}
			
			switch(tableinfor.attr[colNum].type)
			{
				case CHAR:	//字符串比较大小
					switch(conditions[i].op)
					{
						case Lt:	
							if(value1 >= value2) return 0;
							break;
						case Le:
							if(value1 > value2) return 0;
							break;
						case Gt:
							if(value1 <= value2) return 0;
							break;
						case Ge:
							if(value1 < value2) return 0;
							break;
						case Eq:
							if(value1 != value2) return 0;
							break;
						case Ne:
							if(value1  == value2) return 0;
							break;
					}
					break;
				case INT: //整型比较，先比较长度，比如说123肯定就比99大，长度相同345与543的比较同字符串
					{
					int ivalue1=atoi(value1.c_str());
					int ivalue2=atoi(value2.c_str());
					switch(conditions[i].op)
					{
						case Lt:
							if(ivalue1 >= ivalue2) return 0;
							break;
						case Le:
							if(ivalue1 > ivalue2) return 0;
							break;
						case Gt:
							if(ivalue1 <= ivalue2) return 0;
							break;
						case Ge:
							if(ivalue1 < ivalue2) return 0;
							break;
						case Eq:
							if(ivalue1 != ivalue2) return 0;
							break;
						case Ne:
							if(ivalue1  == ivalue2) return 0;
							break;
					}
					break;}
				case FLOAT:// return 0;//浮点数比较大小，不知道怎么方便 
					{
					float fvalue1 = atof(value1.c_str());
					float fvalue2 = atof(value2.c_str());
					switch(conditions[i].op)
					{
						case Lt:	
							if(fvalue1 >= fvalue2) return 0;
							break;
						case Le:
							if(fvalue1 > fvalue2) return 0;
							break;
						case Gt:
							if(fvalue1 <= fvalue2) return 0;
							break;
						case Ge:
							if(fvalue1 < fvalue2) return 0;
							break;
						case Eq:
							if(fvalue1 != fvalue2) return 0;
							break;
						case Ne:
							if(fvalue1  == fvalue2) return 0;
							break;
					}}
			}
		}
		return 1;
	}

		//简单选择
	Data select(Table& tableinfor){
		string filename = tableinfor.name + ".table";
		string stringrow;
		Row splitedRow;
		Data datas;
		int length = tableinfor.totalLength + 1;//加多一位来判断这条记录是否被删除了
		const int recordNum = BLOCKSIZE / length;
	
		for(int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++){
			int bufferNum = bufferMa.getIfIsInBuffer(filename, blockOffset);
			if(bufferNum == -1){
				bufferNum = bufferMa.getEmptyBuffer();
				bufferMa.readBlock(filename, blockOffset, bufferNum);
			}
			for(int offset = 0; offset < recordNum; offset ++){
					int position  = offset * length;
					stringrow = bufferMa.BufferBlock[bufferNum].getvalues(position, position + length);
					if(stringrow.c_str()[0] == '$') continue;//inticate that this row of record have been deleted
					if (stringrow.c_str()[0] == '\0') break;
					stringrow.erase(stringrow.begin());//把第一位去掉
					splitedRow = splitRow(tableinfor, stringrow);
					datas.rows.push_back(splitedRow);
			}
		}
		return datas;
	}
	
	

	//根据条件选择
	Data select(Table tableinfor, vector<Condition> conditions){
		Data datas;
		if(conditions.size() == 0){
			datas = select(tableinfor);
			return datas;
		}
		string filename = tableinfor.name + ".table";
		//buf.scanIn(tableinfor);
		string stringrow;
		Row splitedRow;
		int length = tableinfor.totalLength + 1;//加多一位来判断这条记录是否被删除了
		const int recordNum = BLOCKSIZE / length;//加多一位来判断这条记录是否被删除了
		
		for(int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++){
			int bufferNum = bufferMa.getIfIsInBuffer(filename, blockOffset);
			if(bufferNum == -1){
				bufferNum = bufferMa.getEmptyBuffer();
				bufferMa.readBlock(filename, blockOffset, bufferNum);
			}
			for(int offset = 0; offset < recordNum; offset ++){
				int position  = offset * length;
				stringrow = bufferMa.BufferBlock[bufferNum].getvalues(position, position + length);
				if (stringrow.c_str()[0] == '\0')	break;
				if(stringrow.c_str()[0] != '$'){
					stringrow.erase(stringrow.begin());//把第一位去掉
					splitedRow = splitRow(tableinfor, stringrow);
					if(Comparator(tableinfor, splitedRow, conditions)){//如果满足条件，就把结果push到datas里面去
						datas.rows.push_back(splitedRow);
					}
				}
			}
		}
		return datas;
	}

	//插入一条记录
	void insertValue(Table& tableinfor, Row& splitedRow){
		string stringrow = connectRow(tableinfor, splitedRow);
		insertPos iPos = bufferMa.getInsertPosition(tableinfor);
		bufferMa.BufferBlock[iPos.bufferNum].values[iPos.position] = NOTEMPTY;
		for(int i = 0; i < tableinfor.totalLength; i++){
			bufferMa.BufferBlock[iPos.bufferNum].values[iPos.position + i + 1] = stringrow.c_str()[i];
		}
		bufferMa.BufferBlock[iPos.bufferNum].isWritten = 1;
	}
	//删除一条记录
	int deleteValue(Table tableinfor){
		string filename = tableinfor.name + ".table";
		int count = 0;
		const int recordNum = BLOCKSIZE / (tableinfor.totalLength + 1);	//加多一位来判断这条记录是否被删除了
		for(int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++){
			int bufferNum = bufferMa.getIfIsInBuffer(filename, blockOffset);
			if(bufferNum == -1){
				bufferNum = bufferMa.getEmptyBuffer();
				bufferMa.readBlock(filename, blockOffset, bufferNum);
			}
			for(int offset = 0; offset < recordNum; offset ++){
				int position  = offset * (tableinfor.totalLength + 1);
				if (bufferMa.BufferBlock[bufferNum].values[position] == '\0')
					break;
				if(bufferMa.BufferBlock[bufferNum].values[position] != '$'){
					bufferMa.BufferBlock[bufferNum].values[position] = '$';
					count++;
				}
			}
		}
		return count;
	}
	//删除满足要求的记录
	int deleteValue(Table tableinfor, vector<Condition> conditions){
		string filename = tableinfor.name + ".table";
		string stringrow;
		Row splitedRow;
		int count = 0;
		const int recordNum = BLOCKSIZE / (tableinfor.totalLength + 1);
		for(int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++){
			int bufferNum = bufferMa.getIfIsInBuffer(filename, blockOffset);
			if(bufferNum == -1){
				bufferNum = bufferMa.getEmptyBuffer();
				bufferMa.readBlock(filename, blockOffset, bufferNum);
			}
			for(int offset = 0; offset < recordNum; offset ++){
				int position  = offset * (tableinfor.totalLength + 1);
				stringrow = bufferMa.BufferBlock[bufferNum].getvalues(position, position + tableinfor.totalLength + 1);
				if (stringrow.c_str()[0] == '\0')
					break;
				if(stringrow.c_str()[0] != '$'){
					stringrow.erase(stringrow.begin());//把第一位去掉
					splitedRow = splitRow(tableinfor, stringrow);
					if(Comparator(tableinfor, splitedRow, conditions)){//如果满足条件，就把记录delete掉 
						bufferMa.BufferBlock[bufferNum].values[position] = '$';
						count++;
					} 
				}
			}
			bufferMa.BufferBlock[bufferNum].isWritten = 1;
		}
		return count;
	}
	//删除表
	void dropTable(Table tableinfo){
		string filename = tableinfo.name + ".table";
		if( remove(filename.c_str()) != 0 )
			perror( "Error deleting file" );
		else
			bufferMa.setInvalid(filename);//when a file is deleted, a table or an index, all the value in buffer should be set invalid
	}
	//新建表
	void createTable(Table tableinfo){
		string filename = tableinfo.name + ".table";
		fstream  fout(filename.c_str(), ios::out);
		fout.close();
	}
	//显示数据
	void showDatas(const Data& datas) const{
		if(datas.rows.size() == 0) 
		{
			cout <<  "No Datas" << endl;
			return;
		}
		for(int i = 0; i< datas.rows.size(); i++){
			cout << (i+1) << " ";//序号 
			for(int j = 0; j<datas.rows[i].columns.size(); j++){
				for(int k =0; k < datas.rows[i].columns[j].length(); k++)
					if(datas.rows[i].columns[j].c_str()[k] == EMPTY) break;
					else cout << datas.rows[i].columns[j].c_str()[k];
				cout << '\t';
			}
			cout << endl;
		}
	}

private:
	//根据table类里面的元素的信息，将一段字符串（buffer中的）截取成几段，转存到一个字符串向量row中。
	Row splitRow(Table tableinfor, string row){
		Row splitedRow;
		int s_pos = 0, f_pos = 0;//start position & finish position
		for(int i= 0; i < tableinfor.attriNum; i++){
			s_pos = f_pos;
			f_pos += tableinfor.attr[i].length;
			string col;
			for(int j = s_pos; j < f_pos; j++){
				if (row[j] != '$')
					col += row[j];
					}
			
			splitedRow.columns.push_back(col);
		}
		return splitedRow;
	}
	//根据table类中的元素的信息，将一个字符串向量row整合起来，转存到一个字符串中，长度不够的补'\0'
	string connectRow(Table tableinfor, Row splitedRow){
		string tmptRow;
		string stringrow;
		for(int i = 0; i < splitedRow.columns.size(); i++){

			tmptRow = splitedRow.columns[i];
			for(;tmptRow.length() < tableinfor.attr[i].length; tmptRow += "$");//补'\0'以达到要求的长度

			stringrow += tmptRow;
		}
		return stringrow;
	}

};

#endif