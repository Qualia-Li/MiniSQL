// MiniSQL.cpp : 实现文件
//

#include "stdafx.h"
#include "My MiniSQL.h"
#include "MiniSQL.h"
#include "afxdialogex.h"


// MiniSQL 对话框

IMPLEMENT_DYNAMIC(MiniSQL, CDialogEx)

MiniSQL::MiniSQL(CWnd* pParent /*=NULL*/)
	: CDialogEx(MiniSQL::IDD, pParent)
{

}

MiniSQL::~MiniSQL()
{
}

void MiniSQL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(MiniSQL, CDialogEx)
END_MESSAGE_MAP()


// MiniSQL 消息处理程序


