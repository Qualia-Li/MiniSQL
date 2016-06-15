
// My MiniSQLDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "My MiniSQL.h"
#include "My MiniSQLDlg.h"
#include "afxdialogex.h"
#include "getTime.h"
#include "table.h"
#include "macro.h"
#include "CatalogManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "IndexManager.h"
#include "commandcut.h"
#include "Interpret.h"
#include "api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BufferManager bufferMa;
RecordManager record;
CatalogManager catalog;
Interpret parsetree;
IndexManager index;
api A;


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyMiniSQLDlg �Ի���



CMyMiniSQLDlg::CMyMiniSQLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyMiniSQLDlg::IDD, pParent)
	, m_nTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyMiniSQLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT, m_CStrInput);
	DDX_Text(pDX, IDC_OUTPUT, m_CStrOutput);
	DDX_Text(pDX, IDC_EDIT2, m_nTime);
	DDX_Control(pDX, IDC_CHECK2, m_BPlusTree);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CMyMiniSQLDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMyMiniSQLDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMyMiniSQLDlg::OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMyMiniSQLDlg ��Ϣ�������

BOOL CMyMiniSQLDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CRect rect;
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_TWOCLICKACTIVATE | LVS_EX_FULLROWSELECT);
	m_list.GetClientRect(rect);
	int nListWidth = rect.Width();
	m_list.InsertColumn(0, _T("Name"), LVCFMT_LEFT, nListWidth /2);
	m_list.InsertColumn(1, _T("Type"), LVCFMT_LEFT, nListWidth / 2);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyMiniSQLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyMiniSQLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyMiniSQLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyMiniSQLDlg::OnBnClickedOk()
{

	UpdateData(true);
	clock_t start, finish;
	double TotalTime;

	int length = m_CStrInput.GetLength() + 2;
	char* pcInput = (char*)malloc(length*sizeof(char));
	pcInput = CString2pc(m_CStrInput);
	int i = 0;
	int j = 0;
	int k = 0;
	int flag = 0;
	char temp[200][100];
	while (pcInput[i] != '\0')
	{
		while (pcInput[i] != ';' && pcInput[i]!='\0')
		{
			while (pcInput[i] == '\r' || pcInput[i] == '\n')
				i++;
			temp[j][k] = pcInput[i];
			k++;
			i++;
			flag = 1;
		}
		temp[j][k] = pcInput[i];
		temp[j][k + 1] = '\0';
		k = 0;
		i++;
		j++;
	}
	free(pcInput);
	int langnumber = j;
	i = 0;
	while (i < j)
	{
		if (A.isExprEnd(temp[i]))
		{
			string s = temp[i];
			s += " ";
			start = clock();
			parsetree.parse(s);
			Table TableInfo = parsetree.table_info;
			string ans = A.ExeExpr();
			finish = clock();

			char* pcOutput = (char*)malloc((ans.length() + 1)*sizeof(char));
			strcpy_s(pcOutput, ans.length() + 1, ans.c_str());
			time = time.GetCurrentTime();
			CString str = time.Format("%H : %M : %S   ");
			m_CStrOutput += str;
			m_CStrOutput += pc2CString((pcOutput), ans.length() + 1);
			m_CStrOutput += "\r\n";
			free(pcOutput);
			m_CStrInput = "";
			TotalTime = (double)(finish - start) / CLOCKS_PER_SEC;
			m_nTime = (int)(TotalTime * 1000);

			ShowTableInfo(TableInfo);

			UpdateData(false);
		}
		else
		{
			MessageBox((CString)"��䲻��������������Ƿ�ȱ�ٷֺţ�");
		}
		i++;
	}
}

char* CString2pc(CString cstr){
	int length = cstr.GetLength()+1;
	char* pc = (char*)malloc(length*sizeof(char));
	for (int i = 0; i < length-1; i++){
		pc[i] = cstr[i];
	}

	pc[length-1] = '\0';
	return pc;
}

CString pc2CString(char* pc, int length){
	CString cstr(pc,length);

	return cstr;
}

void CMyMiniSQLDlg::ShowTableInfo(Table TableInfo){
	m_list.DeleteAllItems();
	for (int i=0; i < TableInfo.attriNum; i++){
		//insert name
		string strName = TableInfo.attr[i].name;
		char* pcName = (char*)malloc(strName.length() + 1 * sizeof(char));
		strcpy_s(pcName, strName.length() + 1, strName.c_str());
		CString CStrName = pc2CString(pcName,strName.length()+1);
		free(pcName);
		m_list.InsertItem(i, CStrName);
		//insert type
		int nType = TableInfo.attr[i].type;
		CString CStrType;
		if (1 == nType) CStrType = "int";
		else if (2 == nType) CStrType = "double";
		else if (3 == nType) CStrType = "char";
		m_list.SetItemText(i, 1, CStrType);
	}
}

void CMyMiniSQLDlg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CMyMiniSQLDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType != SIZE_MINIMIZED) //�ж��Ƿ�Ϊ��С��
	{
		Resize(IDC_OUTPUT, cx, cy);
		Resize(IDC_OUTPUT2, cx, cy);
		Resize(IDC_INPUT, cx, cy);
		Resize(IDC_LIST1, cx, cy);
		Resize(IDOK, cx, cy);
		Resize(IDCANCEL, cx, cy);
		Resize(IDC_STATIC, cx, cy);
		Resize(IDC_EDIT2, cx, cy);
		Resize(IDC_STATIC2, cx, cy);
		Resize(IDC_STATIC3, cx, cy);
		Resize(IDC_STATIC4, cx, cy);
		Resize(IDC_STATIC5, cx, cy);
		Resize(IDC_CHECK1, cx, cy);
		Resize(IDC_CHECK2, cx, cy);
		GetClientRect(&m_rect);
	}
}

void CMyMiniSQLDlg::Resize(int nID, int cx, int cy)
{
	CWnd *pWnd;
	pWnd = GetDlgItem(nID);     //��ȡIDΪi�Ŀռ�ľ������Ϊ��ȷ�ϡ�IDΪ1����ȡ����IDΪ2
	if (pWnd)  //�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����

	{
		CRect rect;   //��ȡ�ؼ��仯ǰ�Ĵ�С  
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);//���ؼ���Сת��Ϊ�ڶԻ����е���������
		//cx/m_rect.Width()Ϊ�Ի����ں���ı仯����

		rect.left = rect.left*cx / m_rect.Width();//�����ؼ���С
		rect.right = rect.right*cx / m_rect.Width();
		rect.top = rect.top*cy / m_rect.Height();
		rect.bottom = rect.bottom*cy / m_rect.Height();
		pWnd->MoveWindow(rect);//���ÿؼ���С
	}
}