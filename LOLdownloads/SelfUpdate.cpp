#include "stdafx.h"
#include "GameUpdateDlg.h"

void CGameUpdateDlg::OnBnClickedBugReport()
{
	::ShellExecute(this->GetSafeHwnd(), _T("open"), "http://bbs.gxgk.cc/thread-7314-1-1.html", NULL, NULL, SW_SHOWNORMAL);
}

unsigned WINAPI __stdcall CGameUpdateDlg::CheckUpdate(void *para)
{
	CGameUpdateDlg *Dlg = (CGameUpdateDlg*)para;
	int ret = Dlg->SelfUpdate();
	if (ret == 1) {
		Dlg->PostMessage(WM_QUIT);
		//	Sleep(1000);
		TerminateProcess(GetCurrentProcess(), 0);
	}
	return 1;
}

int CGameUpdateDlg::SelfUpdate()
{
	char http[MAX_STRING]; char Version[MAX_STRING];
	int ret = DownLoadFileToBuffer(http, MAX_STRING, UpdataUrl SoftVer, NULL, TIMEOUT, NULL, NULL);
	if (ret < 0){
		SelfUpdateThread = NULL;
		return -3;
	}
	ret = GetHTTPMsg(http, MAX_STRING, 1, "SoftVer:\"", NULL, NULL, Version, NULL, NULL, NULL, "\"");
	if (!ret){
		SelfUpdateThread = NULL;
		return -2;
	}
	if (CompareVersion(STR_VERSION, Version, NULL, NULL) == 1){
		char szTempPath[MAX_PATH], szTempName[MAX_PATH],TmpName[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		GetTempFileName(szTempPath, _T("GameUp"), 0, szTempName);
		strcpy(TmpName, szTempName);
		strcat(szTempName, ".exe");

		GetDlgItem(IDC_STATIC_Status)->SetWindowText("正在更新下载器...");
		UpdateStatus(TRUE);
		//GetDlgItem(IDC_SelfUpdate)->EnableWindow(FALSE);
		if (DownLoadFile(szTempName, UpdataUrl SoftName, NULL, 30, NULL, NULL) != 0)
		{
			UpdateStatus(FALSE);
			AfxMessageBox(_T("新版程序下载失败，请手动更新！"), MB_TOPMOST);
			::ShellExecute(this->GetSafeHwnd(), _T("open"), UpdataUrl, NULL, NULL, SW_SHOWNORMAL);
			SelfUpdateThread = NULL;
			return -1;
		}
		DeleteFile(TmpName);
		char filepath[MAX_PATH] = { 0 };
		char parameters[MAX_PATH] = "-update \"";
		if (!GetModuleFileName(NULL, filepath, MAX_PATH)) return -1;
		strcat(parameters, filepath);

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile = szTempName;
		ShExecInfo.lpParameters = parameters;
		ShExecInfo.nShow = SW_HIDE;
		if (!ShellExecuteEx(&ShExecInfo)) {
			DeleteFile(szTempName);
			AfxMessageBox(_T("启动更新程序失败！"), MB_TOPMOST);
			SelfUpdateThread = NULL;
			return 0;
		}
	}
	else
	{
		SelfUpdateThread = NULL;
		return 2;
	}
	SelfUpdateThread = NULL;
	return 1;
}