
// My MiniSQLDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include"table.h"

// CMyMiniSQLDlg 对话框
class CMyMiniSQLDlg : public CDialogEx
{
public:
	CRect m_rect;
	CTime time;
	void Resize(int nID, int cx, int cy);
// 构造
public:
	CMyMiniSQLDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYMINISQL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_CStrInput;
	CString m_CStrOutput;
	afx_msg void OnBnClickedOk();
	int m_nTime;
	CButton m_Binary;
	CButton m_BPlusTree;
	CListCtrl m_list;
	afx_msg void OnBnClickedCancel();
	void ShowTableInfo(Table TableInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


char* CString2pc(CString cstr);
CString pc2CString(char* pc, int length);