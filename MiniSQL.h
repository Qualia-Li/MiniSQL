#pragma once
#include "stdafx.h"
#include "afxcmn.h"

// MiniSQL �Ի���

class MiniSQL : public CDialogEx
{
	DECLARE_DYNAMIC(MiniSQL)

public:
	MiniSQL(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~MiniSQL();

// �Ի�������
	enum { IDD = IDD_MYMINISQL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

};
