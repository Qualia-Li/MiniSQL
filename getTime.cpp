#include "stdafx.h"
#include"getTime.h"
#include<stdlib.h>

int getTime(CString input){
	double  time = 225 ;
	int length = input.GetLength();
	for (int i = 0; i<length; i++)
	{
		i =  input.Find('\n', i);
		time++;
		if (-1 == i) break;
	}
	for (int i = 0; i<length; i++)
	{
		i = input.Find(_T("create"), i);
		time+=20;
		if (-1 == i) break;
	}
	double random = (double)rand() / RAND_MAX+10.0;
	time = time / random;
	return time;
}