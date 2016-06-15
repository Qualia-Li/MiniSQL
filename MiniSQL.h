#pragma once
#include "stdafx.h"
#include "afxcmn.h"

// MiniSQL 对话框

class MiniSQL : public CDialogEx
{
	DECLARE_DYNAMIC(MiniSQL)

public:
	MiniSQL(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~MiniSQL();

// 对话框数据
	enum { IDD = IDD_MYMINISQL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

};
