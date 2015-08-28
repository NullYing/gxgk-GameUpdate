#include "stdafx.h"
#include "GameUpdateDlg.h"
#include "Downloads.h"
#include "md5.h"

int CGameUpdateDlg::DNFGetPath()
{
	HKEY PathHKEY;
	char PathDNF[100] = "SOFTWARE\\Tencent\\DNF";
	DWORD keyType = REG_SZ;
	char DATE[MAX_STRING];
	DWORD DATELen;

	LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		PathDNF,
		NULL, KEY_ALL_ACCESS, &PathHKEY);
	if (errCode == ERROR_SUCCESS)
	{
		DATELen = sizeof(DATE);
		errCode = ::RegQueryValueEx(PathHKEY, _T("InstallPath"), NULL, &keyType, (unsigned char*)&DATE, &DATELen);
		if (errCode == ERROR_SUCCESS)
		{
			m_DNFPath = DATE;
			RegCloseKey(PathHKEY);
			SetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
			//备用默认地址
			if (!CheckDNF()){
				m_DNFPath = "C:\\Program Files\\腾讯游戏\\地下城与勇士";
				SetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
				if (!CheckDNF()){
					m_DNFPath = "C:\\Program Files (x86)\\腾讯游戏\\地下城与勇士";
					SetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
				}
			}
			return 1;
		}
	}
	RegCloseKey(PathHKEY);

	if (!CheckDNF()){
		m_DNFPath = "C:\\Program Files\\腾讯游戏\\地下城与勇士";
		SetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
		if (!CheckDNF()){
			m_DNFPath = "C:\\Program Files (x86)\\腾讯游戏\\地下城与勇士";
			SetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
			if (CheckDNF())
				return 1;
		}
		else
			return 1;
	}
	return 0;
}
int CGameUpdateDlg::CheckDNF()
{
	GetDlgItemText(IDC_EDIT_GamePath, m_DNFPath);
	char TempUrl[MAX_STRING];
	sprintf(TempUrl, "%s\\TCLS\\Client.exe", m_DNFPath);
	if ((_access(TempUrl, 0)) != -1){
		return 1;
	}
	GetDlgItem(ID_Update)->EnableWindow(FALSE);
	GetDlgItem(IDC_StartGame)->EnableWindow(FALSE);
	return 0;
}
int CGameUpdateDlg::GetDNFVer()
{
	int ret, ret2;
	ret = LOLLocalVer();
	ret2 = DNFRomoteVer();

	if (ret != 0 && (ret2 != 0 || m_SVersionInt != 0))
		return 1;
	return 0;
}
int CGameUpdateDlg::DNFRomoteVer()
{
	char http[MAX_STRING];
	ListGet(DNFServer);
	int ret = DownLoadFileToBuffer(http, MAX_STRING, DNFRUrl DNFVer, NULL, TIMEOUT, NULL, NULL);
	if (ret < 0)
		return 0;
	ret = GetHTTPMsg(http, MAX_STRING, 1, "", NULL, NULL, m_RVersion, NULL, NULL, NULL, "\n");
	if (ret)GetDlgItem(IDC_STATIC_VerRom)->SetWindowText(m_RVersion);
	else
	{
		return 0;
	}
	return 1;
}