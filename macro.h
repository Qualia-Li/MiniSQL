#ifndef _MACRO_H
#define _MACRO_H

#define EMPTY '$'
#define INT 1
#define FLOAT 2
#define CHARN 3
#define ISPRIMARYKEY 1
#define NOTPRIMARYKEY 0
#define ISUNIQUE 1
#define NOTUNIQUE 0


#define MAXBLOCKNUMBER 1000	//should be 10000
#define BLOCKSIZE 4096	//should be 4096


#define MAXPRIMARYKEYLENGTH  25    //should change sometime



#define COMLEN		400 
#define INPUTLEN	200 
#define WORDLEN		100
#define VALLEN		300
#define NAMELEN		100


enum OPERATION{
	UNKNOWN, SELECT_ERR, CREATE_TABLE_ERR,CREATE_INDEX_ERR, DELETE_ERR, INSERT_ERR, DROP_TABLE_ERR,
	DROP_INDEX_ERR, EXE_FILE_ERR, NO_PRIMARY_KEY, VOID_PRIMARY, VOID_UNI, CHAR_BOUND,
	TABLE_ERR, COLUMN_ERR, INSERT_NUM_ERR, INDEX_ERR, TABLE_EXIST, EMPTY_QUERY, FILE_NOT_FOUND,  //���Ͼ��������������
	INDEX_NOT_EXIST, PRIMARY_RAID,UNIQUE_RAID,
	SELECT_NOWHERE, SELECT_WHERE,DELETE_WHERE, COMMIT,                      //selectָ���Ϊ��where����û��where���
	SELECT, CREATE_TABLE, CREATE_INDEX, DROP_TABLE, DROP_INDEX, DELETE_R, INSERT,  //SQL���ָ��
	QUIT, EXE_FILE
};
enum Comparison{ Lt, Le, Gt, Ge, Eq, Ne, NOTANY };  //Lt��<��Le��<=��Gt��>�� Ge��>=�� Eq��==�� Ne��!=,NOTANY��ʾ������������

#endif