
// My MiniSQLDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include"table.h"

// CMyMiniSQLDlg �Ի���
class CMyMiniSQLDlg : public CDialogEx
{
public:
	CRect m_rect;
	CTime time;
	void Resize(int nID, int cx, int cy);
// ����
public:
	CMyMiniSQLDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYMINISQL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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