
// GameUpdateDlg.h : 头文件
//

#pragma once
#include "Resource.h"
#include "Downloads.h"
#include "afxcmn.h"
#include "Define.h"

#define PROGRESS1_ID 100
#define MAX_RANGE 100
// CGameUpdateDlg 对话框
class CGameUpdateDlg : public CDialogEx
{
// 构造
public:
	CGameUpdateDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GameUpdate_DIALOG };

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
	
	//存储配置
	bool m_ifStartGame;
	void SaveConfig();
	void LoadConfig();

	static unsigned WINAPI __stdcall  GetVersion(void *para);
	static unsigned WINAPI __stdcall  StartUpdate(void *para);
	static unsigned WINAPI __stdcall  CheckUpdate(void *para);
	void UpdateStatus(bool Updating);

	int SelfUpdate();
	int PathGet();
	int CheckGameExist();
	int Feedback(int Status);
	int ListGet(char *);
	int Update();
	unsigned int GameChoose;
	enum Game{
		LOL,
		DNF
	};

	//LOL更新
	CString m_LOLPath;						//游戏路径
	int LOLGetPath();
	int CheckLOL();
	int GetLOLVer();
	int LOLLocalVer();
	int LOLRomoteVer();
	int LOLUpdate(char *ErrPackets, char *Server, char Version[][MAX_STRING]);
	int LOLGetLine(char *buf);
	char LOLVersion[20][MAX_STRING];
	char LOLLanVer[MAX_STRING];
	//DNF更新
	CString m_DNFPath;						//游戏路径
	int DNFGetLine(char *buf);
	int DNFGetPath();
	int CheckDNF();
	int GetDNFVer();
	int DNFRomoteVer();
	int DNFUpdate(char *ErrPackets, char *Server);
	char DNFVersion[MAX_DOWNLINE][MAX_STRING];

	HANDLE UpdateThread;
	HANDLE GetVerThread;
	HANDLE SelfUpdateThread;
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton_Choose();

	CString m_strPath;						//游戏路径
	char m_Version[MAX_STRING];				//本地版本
	int m_VersionInt;
	char m_RVersion[MAX_STRING];			//内网服务器版本
	int m_RVersionInt;
	char m_SVersion[MAX_STRING];					//外网服务器版本
	int m_SVersionInt;					
	char DiskSign[17];
	char ListUrl[MAX_DOWNLINE][MAX_STRING];

	int PCsign(char *DiskSign);
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedStartgame();
	afx_msg void OnBnClickedGetversion();
	CProgressCtrl m_pro;
	afx_msg void OnBnClickedBugReport();
	afx_msg void OnBnClickedRadioDnf();
	afx_msg void OnBnClickedRadioLol();
	afx_msg void OnBnClickedCheckStartgame();
};
