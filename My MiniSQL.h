
// My MiniSQL.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyMiniSQLApp: 
// �йش����ʵ�֣������ My MiniSQL.cpp
//

class CMyMiniSQLApp : public CWinApp
{
public:
	CMyMiniSQLApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyMiniSQLApp theApp;