#include "stdafx.h"
#include "GameUpdateDlg.h"

void CGameUpdateDlg::SaveConfig()
{
	//保存配置到文件中去
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING] = CONFIGNAME;
	char szTemp[MAX_STRING];
	GetDocumentsPath(pszFullPath, pszFilename);

#define WRITE_CONFIG_INT_NAME_VAR(n, v)  {sprintf(szTemp,"%d",v);WritePrivateProfileString("Config", n, szTemp, pszFullPath);}

	WRITE_CONFIG_INT_NAME_VAR("GameChoose", GameChoose);

#define WRITE_CONFIG_BOOL_NAME_VAR(n, v)  WritePrivateProfileString("Config", n, v?"1":"0", pszFullPath);

#define BIND_BOOL_VAR_CONTROL(v, c) v=(IsDlgButtonChecked(c)==0)?false:true;

	BIND_BOOL_VAR_CONTROL(m_ifStartGame, IDC_CHECK_StartGame);

	WRITE_CONFIG_BOOL_NAME_VAR("StartGame", m_ifStartGame);

#define WRITE_CONFIG_STRING_NAME_VAR(n, v)  WritePrivateProfileString("GamePath", n, v, pszFullPath);
	//游戏路径
	WRITE_CONFIG_STRING_NAME_VAR("LOLPath", m_LOLPath);
	WRITE_CONFIG_STRING_NAME_VAR("DNFPath", m_DNFPath);
}
void CGameUpdateDlg::LoadConfig()
{
	//从配置文件中读取设置
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING] = CONFIGNAME;
	GetDocumentsPath(pszFullPath, pszFilename);

#define LOAD_CONFIG_INT_NAME_VAR(n, v, d) v=(GetPrivateProfileInt("Config",n,d,pszFullPath));

	LOAD_CONFIG_INT_NAME_VAR("GameChoose", GameChoose, 0);

#define LOAD_CONFIG_BOOL_NAME_VAR(n, v, d) v=(GetPrivateProfileInt("Config",n,d,pszFullPath)==1);

	LOAD_CONFIG_BOOL_NAME_VAR("StartGame", m_ifStartGame, 1);

	CheckDlgButton(IDC_CHECK_StartGame, m_ifStartGame ? BST_CHECKED : BST_UNCHECKED);

#define LOAD_CONFIG_STRING_NAME_VAR(n, v, d) {char t[MAX_STRING];GetPrivateProfileString("GamePath",n,d,t,MAX_STRING,pszFullPath);v=t;}

	//游戏路径
	LOAD_CONFIG_STRING_NAME_VAR("LOLPath", m_LOLPath, "");
	LOAD_CONFIG_STRING_NAME_VAR("DNFPath", m_DNFPath, "");
}