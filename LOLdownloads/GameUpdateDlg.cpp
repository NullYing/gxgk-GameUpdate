
// GameUpdateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GameUpdate.h"
#include "GameUpdateDlg.h"
#include "afxdialogex.h"
#include "md5.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CGameUpdateDlg �Ի���



CGameUpdateDlg::CGameUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGameUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGameUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_pro);
}

BEGIN_MESSAGE_MAP(CGameUpdateDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Choose, &CGameUpdateDlg::OnBnClickedButton_Choose)
	ON_BN_CLICKED(ID_Update, &CGameUpdateDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_StartGame, &CGameUpdateDlg::OnBnClickedStartgame)
	ON_BN_CLICKED(IDC_GetVersion, &CGameUpdateDlg::OnBnClickedGetversion)
	ON_BN_CLICKED(IDC_BugReport, &CGameUpdateDlg::OnBnClickedBugReport)
	ON_BN_CLICKED(IDC_RADIO_DNF, &CGameUpdateDlg::OnBnClickedRadioDnf)
	ON_BN_CLICKED(IDC_RADIO_LOL, &CGameUpdateDlg::OnBnClickedRadioLol)
	ON_BN_CLICKED(IDC_CHECK_StartGame, &CGameUpdateDlg::OnBnClickedCheckStartgame)
END_MESSAGE_MAP()


// CGameUpdateDlg ��Ϣ�������

BOOL CGameUpdateDlg::OnInitDialog()
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
	//��ʼ���߳�
	GetVerThread = NULL;
	SelfUpdateThread = NULL;
	UpdateThread = NULL;

	//��־����
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING] = "GameUpdate.log";
	GetDocumentsPath(pszFullPath, pszFilename);

	freopen(pszFullPath, "w", stdout);

	//����
	extern int __argc;
	extern char**  __argv;
	if (__argc == 3){
		if (strcmp(__argv[1], "-updated") == 0){
			AfxMessageBox("�������!", MB_TOPMOST);
		}
	}
	SelfUpdateThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdate, this, 0, 0);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	LoadConfig();
	
	if (GameChoose==LOL)
		((CButton *)GetDlgItem(IDC_RADIO_LOL))->SetCheck(TRUE);
	if (GameChoose==DNF)
		((CButton *)GetDlgItem(IDC_RADIO_DNF))->SetCheck(TRUE);
	if (m_strPath.Compare("") == 0){
		PathGet();
	}
	SetDlgItemText(IDC_EDIT_GamePath, m_strPath);
	
	m_VersionInt = 0; m_RVersionInt = 0;

	//if (CheckGameExist()){
		//GetVerThread = (HANDLE)_beginthreadex(NULL, 0, GetVersion, this, 0, 0);
	//}

	m_pro.SetRange(0, MAX_RANGE);//���þ����С
	m_pro.SetPos(0);//���ý������ĳ�ʼλ��
	SetTimer(PROGRESS1_ID, 20, NULL);
	GetDlgItem(IDC_Version)->SetWindowText(_T(STR_VERSION));

	PCsign(DiskSign);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGameUpdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGameUpdateDlg::OnPaint()
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
HCURSOR CGameUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGameUpdateDlg::OnBnClickedButton_Choose()
{
	CString str;//����һ���ַ���
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "��ѡ����Ϸ����Ŀ¼��";
	//bi.ulFlags = BIF_USENEWUI;
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH));
	str.ReleaseBuffer();
	m_strPath = str;//Ϊ�Ի�������һ�༭���Ӧ��CString�ͱ��������沢��ʾѡ�е�·����
	if (str.GetAt(str.GetLength() - 1) != '\\')
		m_strPath += "";
	UpdateData(FALSE);

	SetDlgItemText(IDC_EDIT_GamePath, m_strPath);
	if (GameChoose == LOL)
		m_LOLPath = m_strPath;
	if (GameChoose == DNF)
		m_DNFPath = m_strPath;
	CheckGameExist();
}


void CGameUpdateDlg::OnBnClickedUpdate()
{
	if (!CheckGameExist())
		return ;
	if (UpdateThread == NULL)
		UpdateThread = (HANDLE)_beginthreadex(NULL, 0, StartUpdate, this, 0, 0);
	else
		AfxMessageBox(_T("���ڸ����У����Ժ�..."), MB_TOPMOST);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CGameUpdateDlg::OnBnClickedStartgame()
{
	if (!CheckGameExist())
		return;
	CString Path;
	Path = m_strPath + "\\TCLS\\Client.exe";

	::ShellExecute(this->GetSafeHwnd(), _T("open"), Path, NULL, NULL, SW_SHOWNORMAL);
}


void CGameUpdateDlg::OnBnClickedGetversion()
{
	if (!CheckGameExist())
		return ;
	if (GetVerThread == NULL){
		GetVerThread = (HANDLE)_beginthreadex(NULL, 0, GetVersion, this, 0, 0);
	}
	else{
		AfxMessageBox(_T("���ڻ�ȡ�������汾���У�"), MB_TOPMOST);
	}
}

void CGameUpdateDlg::UpdateStatus(bool Updating)
{
	GetDlgItem(IDC_RADIO_LOL)->EnableWindow(!Updating);
	GetDlgItem(IDC_RADIO_DNF)->EnableWindow(!Updating);
	GetDlgItem(ID_Update)->EnableWindow(!Updating);
	GetDlgItem(IDC_EDIT_GamePath)->EnableWindow(!Updating);
	GetDlgItem(IDC_BUTTON_Choose)->EnableWindow(!Updating);
	GetDlgItem(IDC_StartGame)->EnableWindow(!Updating);
	GetDlgItem(IDC_GetVersion)->EnableWindow(!Updating);
	//GetDlgItem(IDC_BugReport)->EnableWindow(!Updating);
}
int CGameUpdateDlg::PathGet()
{
	if (GameChoose == LOL){
		LOLGetPath();
		m_strPath = m_LOLPath;
	}
	if (GameChoose == DNF){
		DNFGetPath();
		m_strPath = m_DNFPath;
	}
	SetDlgItemText(IDC_EDIT_GamePath, m_strPath);
	return 1;
}
int CGameUpdateDlg::CheckGameExist()
{
	if (GameChoose == LOL){
		if (!CheckLOL()){
			AfxMessageBox(_T("LOL��װĿ¼ѡ�����"), MB_TOPMOST);
			return 0;
		}
		else{
			GetDlgItem(ID_Update)->EnableWindow(TRUE);
			GetDlgItem(IDC_StartGame)->EnableWindow(TRUE);
			return 1;
		}
	}
	if (GameChoose == DNF){
		if (!CheckDNF()){
			AfxMessageBox(_T("DNF��װĿ¼ѡ�����"), MB_TOPMOST);
			return 0;
		}
		else{
			GetDlgItem(ID_Update)->EnableWindow(TRUE);
			GetDlgItem(IDC_StartGame)->EnableWindow(TRUE);
			return 1;
		}
	}
	return 0;
}
unsigned WINAPI __stdcall CGameUpdateDlg::GetVersion(void *para)
{
	CGameUpdateDlg *Dlg = (CGameUpdateDlg*)para;
	int ret = 0;
	Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("���ڻ�ȡ�汾��Ϣ...");
	Dlg->GetDlgItem(IDC_STATIC_VerRom)->SetWindowText("��");
	Dlg->GetDlgItem(IDC_STATIC_VerLoc)->SetWindowText("��");
	Dlg->GetDlgItem(IDC_STATIC_VerSer)->SetWindowText("��");
	Dlg->GetDlgItem(IDC_GetVersion)->EnableWindow(FALSE);
	if (Dlg->GameChoose == LOL)
		ret = Dlg->GetLOLVer();
	if (Dlg->GameChoose == DNF)
		ret = Dlg->GetDNFVer();

	if (ret == 1){
		CompareVersion(Dlg->m_Version, Dlg->m_RVersion, &Dlg->m_VersionInt, &Dlg->m_RVersionInt);
		Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("�汾��Ϣ��ȡ�ɹ���");
	}
	else{
		Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("�汾��Ϣ��ȡʧ��,�������磡");
	}
	Dlg->GetVerThread = NULL;
	Dlg->GetDlgItem(IDC_GetVersion)->EnableWindow(TRUE);
	return ret;
}

unsigned WINAPI __stdcall CGameUpdateDlg::StartUpdate(void *para)
{
	CGameUpdateDlg *Dlg = (CGameUpdateDlg*)para;
	Dlg->SaveConfig();

	int ret = -1;
	for (int i = 0; i < 3; i++){
		printf("%d %d %d\n", Dlg->m_VersionInt, Dlg->m_RVersionInt, Dlg->m_SVersionInt);
		if (Dlg->m_VersionInt != 0 && (Dlg->m_RVersionInt != 0 || Dlg->m_SVersionInt != 0)){
			if (Dlg->m_RVersionInt != 0)
				if (Dlg->m_VersionInt <Dlg->m_RVersionInt || Dlg->m_VersionInt < Dlg->m_SVersionInt){
					ret = 1;
				}
				else{
					ret = 0;
				}
			else
				if (Dlg->m_VersionInt < Dlg->m_SVersionInt){
					ret = 1;
				}
				else
					ret = 0;
			break;
		}
		else
		{
			GetVersion(Dlg);
		}
	}
	if (ret == 1){
		if (Dlg->GetVerThread == NULL){
			Dlg->Update();
		}
		else
			AfxMessageBox(_T("���ڻ�ȡ�汾����..."), MB_TOPMOST);
	}
	if (ret == 0) AfxMessageBox(_T("�������°汾��������£�"), MB_TOPMOST);
	if (ret == -1) AfxMessageBox(_T("��������ʧ��,���������Ƿ�������"), MB_TOPMOST);

	Dlg->UpdateThread = NULL;
	return ret;
}
int CGameUpdateDlg::Update()
{
	UpdateStatus(TRUE);
	int ret = -1; char ErrorName[MAX_STRING], szTemp[MAX_STRING];
	if (GameChoose == LOL)
		ret = LOLUpdate(ErrorName, LOLRUrl, LOLVersion);
	if (GameChoose == DNF)
		ret = LOLUpdate(ErrorName, DNFRUrl, DNFVersion);

	if (ret == 2){
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("�����ٴ����أ���������Ϸ��ɸ��£�");
		if (IsDlgButtonChecked(IDC_CHECK_StartGame))
			OnBnClickedStartgame();
	}
	else if (ret == 1)
	{
		if (IsDlgButtonChecked(IDC_CHECK_StartGame))
			OnBnClickedStartgame();
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("���ظ��°��ɹ�����������Ϸ��ɣ�");
		Feedback(1);
	}
	else if (ret == 0){
		sprintf(szTemp, "����ʧ�ܵĸ��°���%s", ErrorName);
		GetDlgItem(IDC_STATIC_Status)->SetWindowText(szTemp);
		Feedback(0);
	}
	else if (ret == -1){
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("����ʧ�ܣ���������ʧ�ܣ�");
	}

	UpdateStatus(FALSE);
	UpdateThread = NULL;
	return NULL;
}

void CGameUpdateDlg::OnBnClickedRadioDnf()
{
	GameChoose = DNF;
	PathGet();
	GetVerThread = (HANDLE)_beginthreadex(NULL, 0, GetVersion, this, 0, 0);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CGameUpdateDlg::OnBnClickedRadioLol()
{
	GameChoose = LOL;
	PathGet();
	GetVerThread = (HANDLE)_beginthreadex(NULL, 0, GetVersion, this, 0, 0);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CGameUpdateDlg::OnBnClickedCheckStartgame()
{
	SaveConfig();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
