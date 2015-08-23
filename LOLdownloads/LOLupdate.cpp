#include "stdafx.h"
#include "GameUpdateDlg.h"
#include "Downloads.h"
#include "md5.h"
int CGameUpdateDlg::LOLGetPath()
{
	HKEY PathHKEY;
	char PathLOL[100] = "SOFTWARE\\Tencent\\LOL";
	DWORD keyType = REG_SZ;
	char DATE[MAX_STRING];
	DWORD DATELen;

	LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		PathLOL,
		NULL, KEY_ALL_ACCESS, &PathHKEY);
	if (errCode == ERROR_SUCCESS)
	{
		DATELen = sizeof(DATE);
		errCode = ::RegQueryValueEx(PathHKEY, _T("InstallPath"), NULL, &keyType, (unsigned char*)&DATE, &DATELen);
		if (errCode == ERROR_SUCCESS)
		{
			m_LOLPath = DATE;
			RegCloseKey(PathHKEY);
			SetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
			//备用默认地址
			if (!CheckLOL()){
				SetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
				m_LOLPath = "C:\\Program Files\\腾讯游戏\\英雄联盟";
				if (!CheckLOL()){
					SetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
					m_LOLPath = "C:\\Program Files (x86)\\腾讯游戏\\英雄联盟";
				}
			}
			return 1;
		}
	}
	RegCloseKey(PathHKEY);

	if (!CheckLOL()){
		SetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
		m_LOLPath = "C:\\Program Files\\腾讯游戏\\英雄联盟";
		if (!CheckLOL()){
			SetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
			m_LOLPath = "C:\\Program Files (x86)\\腾讯游戏\\英雄联盟";
			if (CheckLOL())
				return 1;
		}
		else
			return 1;
	}
	return 0;
}
int CGameUpdateDlg::CheckLOL()
{
	GetDlgItemText(IDC_EDIT_GamePath, m_LOLPath);
	char TempUrl[MAX_STRING];
	sprintf(TempUrl, "%s\\TCLS\\Client.exe", m_LOLPath);
	if ((_access(TempUrl, 0)) != -1){
		return 1;
	}
	GetDlgItem(ID_Update)->EnableWindow(FALSE);
	GetDlgItem(IDC_StartGame)->EnableWindow(FALSE);
	return 0;
}
int CGameUpdateDlg::GetLOLVer()
{
	int ret, ret2;
	ret = LOLLocalVer();
	ret2 = LOLRomoteVer();

	if (ret != 0 && (ret2 != 0 || m_SVersionInt != 0))
		return 1;
	return 0;
}
int CGameUpdateDlg::LOLLocalVer()
{
	//char sign[MAX_STRING] = "<Version>";
	//char sign2[MAX_STRING] = "</Version>";
	char szTemp[MAX_STRING];
	//char *p, *q;
	sprintf(szTemp, "%s/TCLS/mmog_data.xml", m_strPath);

	if ((_access(szTemp, 0)) == -1){
		return NULL;
	}
	FILE *fp; char buf[MAX_BUFFER_SIZE];
	fp = fopen(szTemp, "r");
	if (fp == NULL){
		printf("打开文件失败");
		return 0;
	}
	fread(buf, MAX_BUFFER_SIZE, 1, fp);

	int ret = GetHTTPMsg(buf, MAX_BUFFER_SIZE, 1, "<Version>", NULL, NULL, m_Version, NULL, NULL, NULL, "</Version>");

	/*
	p = strstr(buf, sign);
	if (p == NULL){
	printf("获取版本号失败");
	return NULL;
	}
	p += strlen(sign);
	q = strstr(buf, sign2);
	if (q == NULL){
	printf("获取版本号失败");
	return NULL;
	}
	*q = '\0';
	strcpy(m_Version, p);
	*/
	fclose(fp);

	if (ret){
		GetDlgItem(IDC_STATIC_VerLoc)->SetWindowText(m_Version);
		return 1;
	}
	return 0;
}
int CGameUpdateDlg::LOLRomoteVer()
{
	char http[MAX_STRING];
	ListGet(LOLServer);
	memset(http, 0, MAX_STRING);
	int ret = DownLoadFileToBuffer(http, MAX_STRING, LOLRUrl LOLVer, NULL, TIMEOUT, NULL, NULL);
	if (ret < 0)
		return 0;
	else
		strncpy(LOLLanVer, http, MAX_STRING);
	ret = GetHTTPMsg(http, MAX_STRING, 1, "", NULL, NULL, m_RVersion, NULL, NULL, NULL, "\n");
	if (ret)GetDlgItem(IDC_STATIC_VerRom)->SetWindowText(m_RVersion);
	else
	{
		return 0;
	}
	return 1;
}


int CGameUpdateDlg::LOLUpdate(char *ErrPackets, char *Server,char Version[][MAX_STRING])
{
	int m_PackageCount;
	CString m_PackageName;
	char pszFullPath[MAX_STRING];
	char pszFilename[MAX_STRING];
	GetTempPath(MAX_PATH, pszFullPath);

	char szTemp[MAX_STRING];
	char szTemp2[MAX_STRING];
	char TempUrl[MAX_STRING];
	char RUrl[MAX_STRING];
	
	//int p,q;
	bool ErrorNum = FALSE;
	int HaveDown = 0;

	int md5_len = 32;
	char *RMd5 = (char *)malloc((md5_len + 1) * sizeof(char)), *LMd5 = (char *)malloc((md5_len + 1) * sizeof(char));
	
	int UP = 0, Ver = 0;
	int LVersionInt,RVersionInt;
	for (Ver = 0;; Ver++){
		CompareVersion(m_Version, &Version[Ver][MAX_STRING], &LVersionInt, &RVersionInt);
		if (RVersionInt == 0){
			break;
		}
		if (LVersionInt >= RVersionInt){
			continue;
		}
		sprintf(pszFilename, "%sV%s"VerisonFile, pszFullPath, Version[Ver + 1]);
		//GetFullPathToFile(pszFullPath, pszFilename);

		sprintf(TempUrl, "%sV%s"VerisonFile, Server, Version[Ver + 1]);
		int ret = DownLoadFile(pszFilename, TempUrl, NULL, TIMEOUT, NULL, NULL);
		if (ret < 0)
		{
			strncpy(TempUrl, ListUrl[Ver+1], MAX_STRING);
			ret = DownLoadFile(pszFilename, TempUrl, NULL, TIMEOUT*10, NULL, NULL);
			if (ret < 0){ 
				free(RMd5); free(LMd5); 
				return -1; 
			}
		}
#define LOAD_CONFIG_INT_NAME(n, v, d) v=(GetPrivateProfileInt("Header",n,d,pszFilename));
		LOAD_CONFIG_INT_NAME("PackageCount", m_PackageCount, 0);
		sprintf(szTemp, "%s版本共%d个更新包", Version[Ver + 1], m_PackageCount);
		GetDlgItem(IDC_TOTAL)->SetWindowText(szTemp);
		printf("总数：%d\n", m_PackageCount);
		for (int i = 0; i < m_PackageCount; i++){
			sprintf(szTemp, "Package%d", i);
#define LOAD_CONFIG_STRING_NAME(n, v, d) {char t[MAX_STRING];GetPrivateProfileString("PackageInfo",n,d,t,MAX_STRING,pszFilename);v=t;}
			LOAD_CONFIG_STRING_NAME(szTemp, m_PackageName, NULL);
			//list文件第三个
				ret=sscanf(m_PackageName, "%*[^?]?%*[^?]?%*[^?]?%*[^?]?%[^?]?%[^?]?%[^?]?%*", szTemp, RUrl, RMd5);
				//m_PackageName = szTemp;
				/*
				p = m_PackageName.ReverseFind('/');
				q = m_PackageName.ReverseFind('?');

				m_PackageName.Delete(q, MAX_STRING);
				q = m_PackageName.ReverseFind('?');
				//printf("p:%d\n", p); printf("q:%d\n", q);
				strncpy(RMd5, m_PackageName.GetBuffer() + (m_PackageName.GetLength() - 32), (32 + 1) * sizeof(char));

				m_PackageName = m_PackageName.Mid(p+1, q - (p+1));
				*/
			//list文件第一个
				printf("sscanf=%d\n",ret);
			if (ret != 3){
				ret=sscanf(m_PackageName, "%[^?]?%[^?]?%[^?]?%*", szTemp, RUrl, RMd5);
				
				/*
				p = m_PackageName.FindOneOf("?");
				q = m_PackageName.ReverseFind('?');
				m_PackageName.Delete(q, MAX_STRING);
				strncpy(RMd5, m_PackageName.GetBuffer() + (m_PackageName.GetLength() - 32), (32 + 1) * sizeof(char));
				m_PackageName.Delete(p, MAX_STRING);
				*/
				printf("ONE RMd5=%s\n", RMd5);
			}
			m_PackageName = szTemp;
			printf("包%d：%s\n", i, m_PackageName);

			sprintf(szTemp, "%spatchs/%s", Server, m_PackageName);

			sprintf(szTemp2, "正在下载第%d个更新包：%s", i + 1, m_PackageName);
			GetDlgItem(IDC_STATIC_Status)->SetWindowText(szTemp2);

			sprintf(TempUrl, "%s\\TCLS\\patchs\\%s", m_strPath, m_PackageName);

			if ((_access(TempUrl, 0)) != -1){
				Md5Check(LMd5, TempUrl, md5_len);
				printf("RMd5=%s\n", RMd5);
				printf("LMd5=%s\n", LMd5);
				if (strcmp(RMd5, LMd5) == 0){
					HaveDown++;
					printf("文件存在且Md5相同，跳过\n");
					continue;
				}

				/*
				FileInfo *local_info = GetFileModifyTime(TempUrl);
				if (local_info != NULL&&remote_info != NULL){
				printf("本地包大小：%d\n", local_info->filesize);
				if (local_info->filesize == remote_info->filesize)
				continue;
				}
				*/
			}
			int trynum;
			for (trynum = 0; trynum < 4; trynum++){
				if (trynum == 2){
					strncpy(szTemp, RUrl, MAX_STRING);
				}
				printf("从%s下载\n", szTemp);

				ret = DownLoadFile(TempUrl, szTemp, NULL, FileTIMEOUT, NULL, NULL);
				if (ret < 0)
				{
					printf("%s下载失败,ret=%d\n", m_PackageName,ret);
					continue;
				}
				else{
					Md5Check(LMd5, TempUrl, md5_len);
					printf("RMd5=%s\n", RMd5);
					printf("LMd5=%s\n", LMd5);
					if (strcmp(RMd5, LMd5) == 0){
						printf("%s下载成功\n", m_PackageName);
						break;
					}
					else{
						printf("%s下载文件不完整\n", m_PackageName);
						continue;
					}
				}
			}
			if (trynum == 4)
			{
				DeleteFile(TempUrl);
				ErrorNum = TRUE;
				strcat(ErrPackets, m_PackageName);
				strcat(ErrPackets, "  ");
			}
		}
		DeleteFile(pszFilename);
	}
	free(RMd5); free(LMd5);

	if (!ErrorNum)
	{
		if ((HaveDown == m_PackageCount) && HaveDown != 0)
			return 2;
		return 1;
	}
	return 0;
}

