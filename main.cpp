#include "table.h"
#include "macro.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "IndexManager.h"
#include "commandcut.h"
#include "Interpret.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include "api.h"

using namespace std;

BufferManager bufferMa;
RecordManager record;
CatalogManager catalog;
Interpret parsetree;
IndexManager index;


void Welcome()
{
	cout << "Welcome to MyMiniSQL!!";
}

int main()
{
	api A;
	Welcome();
	cout << endl;
	while (1)
	{
		clock_t start, finish;
		double Total_time;

		cout << "MyMiniSQL>>";
		string s = "";

		s = A.InputAndProcess();
		start = clock();
		parsetree.parse(s);
		string ans = A.ExeExpr();
		finish = clock();
		Total_time = (double)(finish - start) / CLOCKS_PER_SEC;

		cout << ans << endl;
		cout << "This operation takes "<<Total_time * 1000 << " ms." << endl;
		cout << endl;
	}
	getchar();
	return 0;
}