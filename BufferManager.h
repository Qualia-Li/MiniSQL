#ifndef _buffer_h_
#define _buffer_h_



#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "table.h"
#include "macro.h"

using namespace std;


class BufferManager
{
private:
public:
	friend class RecordManager;
	friend class IndexManager;
	friend class CatalogManager;
	//保证RecordManager、IndexManager和CatalogManager能读取buffer的值
	
	BufferManager()    //构造函数要将所有的buffer初始化
	{
		for(int i=0; i<MAXBLOCKNUMBER; i++) BufferBlock[i].initialize();
	}
	~BufferManager()    //析构函数，要将全部的buffer写到存储器
	{
		for(int i=0; i<MAXBLOCKNUMBER; i++) flashback(i);
	}
	buffer BufferBlock[MAXBLOCKNUMBER];

	void flashback(int bufferNum)     //用二进制形式将buffer中的内容保存到文件中
	{
		if(!BufferBlock[bufferNum].isWritten) return;
		string filename=BufferBlock[bufferNum].filename;

		fstream fout(filename.c_str(), ios::in|ios::out);

		fout.seekp(BLOCKSIZE*BufferBlock[bufferNum].blockOffset, fout.beg);
		fout.write(BufferBlock[bufferNum].values, BLOCKSIZE);
		BufferBlock[bufferNum].initialize();
		fout.close();
	}

	int getbufferNum(string filename, int blockOffset)    //通过文件、偏移量得到buffer的编号
	{
		int bufferNum=getIfIsInBuffer(filename, blockOffset);
		if(bufferNum==-1){
			bufferNum=getEmptyBufferExcept(filename);
			readBlock(filename, blockOffset, bufferNum);
		}
		return bufferNum;
	}

	int getIfIsInBuffer(string filename, int blockOffset)    //判断是否在已经在的buffer当中
	{
		for(int bufferNum=0; bufferNum<MAXBLOCKNUMBER; bufferNum++)
			if(BufferBlock[bufferNum].filename == filename && BufferBlock[bufferNum].blockOffset == blockOffset)
				return bufferNum;
		return -1;
	}
	int getEmptyBufferExcept(string filename)    //得到一个filename之外的buffer
	{
		int bufferNum=-1;
		int highestLRUvalue = BufferBlock[0].LRUvalue;
		for(int i=0; i < MAXBLOCKNUMBER; i++)
		{
			if(!BufferBlock[i].isValid)
			{
				BufferBlock[i].isValid = 1;
				return i;
			}
			else if(highestLRUvalue < BufferBlock[i].LRUvalue && BufferBlock[i].filename != filename)
			{
				highestLRUvalue = BufferBlock[i].LRUvalue;
				bufferNum=i;
			}
		}
		if(bufferNum==-1)
		{
			cout << "All the buffers in the database system are used up. Sorry about that!" << endl;
			exit(0);
		}
		flashback(bufferNum);
		BufferBlock[bufferNum].isValid = 1;
		return bufferNum;
	}
	void readBlock(string filename, int blockOffset, int bufferNum)   //读取文件到buffer
	{
		BufferBlock[bufferNum].isValid =1;
		BufferBlock[bufferNum].isWritten=0;
		BufferBlock[bufferNum].filename=filename;
		BufferBlock[bufferNum].blockOffset=blockOffset;

		fstream fin(filename.c_str(), ios::in | ios::binary);

		fin.seekp(BLOCKSIZE*blockOffset, fin.beg);
		fin.read(BufferBlock[bufferNum].values, BLOCKSIZE);
		fin.close();
	}
	
	void writeBlock(int bufferNum)    //将buffer的一些变量改变
	{
		BufferBlock[bufferNum].isWritten =1;
		useBlock(bufferNum);
	}
	void useBlock(int bufferNum)
	{
		for	(int i=1; i<MAXBLOCKNUMBER; i++){
			if(i == bufferNum){
				BufferBlock[bufferNum].LRUvalue=0;
				BufferBlock[i].isValid=1;
			}
			else BufferBlock[bufferNum].LRUvalue++;
		}
	}

	int getEmptyBuffer()    //找到合适的一个buffer
	{
		int bufferNum=0;
		int highestLRUvalue = BufferBlock[0].LRUvalue;
		for(int i=0; i<MAXBLOCKNUMBER; i++)
		{
			if(!BufferBlock[i].isValid)
			{
				BufferBlock[i].initialize();
				BufferBlock[i].isValid=1;
				return i;
			}
			else if(highestLRUvalue < BufferBlock[i].LRUvalue)
			{
				highestLRUvalue = BufferBlock[i].LRUvalue;
				bufferNum=i;
			}
		}
		flashback(bufferNum);
		BufferBlock[bufferNum].isValid=1;
		return bufferNum;
	}

	insertPos getInsertPosition(Table& tableinfor)   //找到插入记录的位置
	{
		insertPos ipos;
		if(tableinfor.blockNum==0)
		{
			ipos.bufferNum = addBlockInFile(tableinfor);
			ipos.position =0;
			return ipos;
		}
		string filename=tableinfor.name+".table";
		int length = tableinfor.totalLength+1;
		int blockOffset = tableinfor.blockNum -1;//get the block offset in file of the last block
		int bufferNum=getIfIsInBuffer(filename,	blockOffset);
		if(bufferNum==-1){//indicate that the data is not read in buffer yet
			bufferNum=getEmptyBuffer();
			readBlock(filename, blockOffset, bufferNum);
		}
		const int recordNum=BLOCKSIZE / length;//加多一位来判断这条记录是否被删除了
		for(int offset =0; offset < recordNum; offset++)
		{
			int position =offset*length;
			char isEmpty=BufferBlock[bufferNum].values[position];
			if(isEmpty==EMPTY){
				ipos.bufferNum=bufferNum;
				ipos.position=position;
				return ipos;
			}
		}
		//if the program run till here, the last block is full, therefor one more block is added
		ipos.bufferNum=addBlockInFile(tableinfor);
		ipos.position=0;
		return ipos;
	}
	int addBlockInFile(Table& tableinfor)    //block不够，向表格文件增加一个block
	{
		int bufferNum = getEmptyBuffer();
		BufferBlock[bufferNum].initialize();
		BufferBlock[bufferNum].isValid=1;
		BufferBlock[bufferNum].isWritten=1;
		BufferBlock[bufferNum].filename=tableinfor.name + ".table";
		BufferBlock[bufferNum].blockOffset=tableinfor.blockNum++;
		return bufferNum;
	}
	int addBlockInFile(Index& indexinfor)    //block不够，向Index文件中增加一个block
	{
		string filename = indexinfor.index_name + ".index";
		int bufferNum = getEmptyBufferExcept(filename);
		BufferBlock[bufferNum].initialize();
		BufferBlock[bufferNum].isValid = 1;
		BufferBlock[bufferNum].isWritten = 1;
		BufferBlock[bufferNum].filename = filename;
		BufferBlock[bufferNum].blockOffset = indexinfor.blockNum++;
		return bufferNum;
	}

	void scanIn(Table tableinfo)    //将表格读到buffer
	{
		string filename = tableinfo.name + ".table";
		
		fstream fin(filename.c_str(), ios::in| ios::binary);
		for(int blockOffset=0; blockOffset < tableinfo.blockNum; blockOffset++)
		{
			if(getIfIsInBuffer(filename, blockOffset) == -1)
			{
				int bufferNum = getEmptyBufferExcept(filename);
				readBlock(filename, blockOffset, bufferNum);
			}
		}
		fin.close();
	}

	void setInvalid(string filename)  //设置某个文件对应的buffer是无效的
	{
		for(int i = 0; i < MAXBLOCKNUMBER; i++)
		{
			if(BufferBlock[i].filename == filename)
			{
					BufferBlock[i].isValid = 0;
					BufferBlock[i].isWritten = 0;
			}
		}
	}

};

#endif