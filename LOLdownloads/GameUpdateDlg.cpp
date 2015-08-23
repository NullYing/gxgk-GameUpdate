
// GameUpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameUpdate.h"
#include "GameUpdateDlg.h"
#include "afxdialogex.h"
#include "md5.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CGameUpdateDlg 对话框



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


// CGameUpdateDlg 消息处理程序

BOOL CGameUpdateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//初始化线程
	GetVerThread = NULL;
	SelfUpdateThread = NULL;
	UpdateThread = NULL;

	//日志保存
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING] = "GameUpdate.log";
	GetDocumentsPath(pszFullPath, pszFilename);

	freopen(pszFullPath, "w", stdout);

	//更新
	extern int __argc;
	extern char**  __argv;
	if (__argc == 3){
		if (strcmp(__argv[1], "-updated") == 0){
			AfxMessageBox("更新完毕!", MB_TOPMOST);
		}
	}
	SelfUpdateThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdate, this, 0, 0);
	// TODO:  在此添加额外的初始化代码
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

	m_pro.SetRange(0, MAX_RANGE);//设置矩阵大小
	m_pro.SetPos(0);//设置进度条的初始位置
	SetTimer(PROGRESS1_ID, 20, NULL);
	GetDlgItem(IDC_Version)->SetWindowText(_T(STR_VERSION));

	PCsign(DiskSign);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGameUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGameUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGameUpdateDlg::OnBnClickedButton_Choose()
{
	CString str;//声明一个字符串
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "请选择游戏所在目录：";
	//bi.ulFlags = BIF_USENEWUI;
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	SHGetPathFromIDList(idl, str.GetBuffer(MAX_PATH));
	str.ReleaseBuffer();
	m_strPath = str;//为对话框中与一编辑框对应的CString型变量，保存并显示选中的路径。
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
		AfxMessageBox(_T("正在更新中，请稍后..."), MB_TOPMOST);
	// TODO:  在此添加控件通知处理程序代码
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
		AfxMessageBox(_T("正在获取服务器版本号中！"), MB_TOPMOST);
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
			AfxMessageBox(_T("LOL安装目录选择错误！"), MB_TOPMOST);
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
			AfxMessageBox(_T("DNF安装目录选择错误！"), MB_TOPMOST);
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
	Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("正在获取版本信息...");
	Dlg->GetDlgItem(IDC_STATIC_VerRom)->SetWindowText("无");
	Dlg->GetDlgItem(IDC_STATIC_VerLoc)->SetWindowText("无");
	Dlg->GetDlgItem(IDC_STATIC_VerSer)->SetWindowText("无");
	Dlg->GetDlgItem(IDC_GetVersion)->EnableWindow(FALSE);
	if (Dlg->GameChoose == LOL)
		ret = Dlg->GetLOLVer();
	if (Dlg->GameChoose == DNF)
		ret = Dlg->GetDNFVer();

	if (ret == 1){
		CompareVersion(Dlg->m_Version, Dlg->m_RVersion, &Dlg->m_VersionInt, &Dlg->m_RVersionInt);
		Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("版本信息获取成功！");
	}
	else{
		Dlg->GetDlgItem(IDC_STATIC_Status)->SetWindowText("版本信息获取失败,请检查网络！");
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
			AfxMessageBox(_T("正在获取版本号中..."), MB_TOPMOST);
	}
	if (ret == 0) AfxMessageBox(_T("已是最新版本，无须更新！"), MB_TOPMOST);
	if (ret == -1) AfxMessageBox(_T("网络连接失败,请检测网络是否正常！"), MB_TOPMOST);

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
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("无须再次下载，请启动游戏完成更新！");
		if (IsDlgButtonChecked(IDC_CHECK_StartGame))
			OnBnClickedStartgame();
	}
	else if (ret == 1)
	{
		if (IsDlgButtonChecked(IDC_CHECK_StartGame))
			OnBnClickedStartgame();
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("下载更新包成功，请启动游戏完成！");
		Feedback(1);
	}
	else if (ret == 0){
		sprintf(szTemp, "下载失败的更新包：%s", ErrorName);
		GetDlgItem(IDC_STATIC_Status)->SetWindowText(szTemp);
		Feedback(0);
	}
	else if (ret == -1){
		GetDlgItem(IDC_STATIC_Status)->SetWindowText("更新失败，网络连接失败！");
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
	// TODO:  在此添加控件通知处理程序代码
}


void CGameUpdateDlg::OnBnClickedRadioLol()
{
	GameChoose = LOL;
	PathGet();
	GetVerThread = (HANDLE)_beginthreadex(NULL, 0, GetVersion, this, 0, 0);
	// TODO:  在此添加控件通知处理程序代码
}

void CGameUpdateDlg::OnBnClickedCheckStartgame()
{
	SaveConfig();
	// TODO:  在此添加控件通知处理程序代码
}
