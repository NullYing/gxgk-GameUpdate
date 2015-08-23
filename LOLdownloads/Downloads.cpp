#include "stdafx.h"
#include "GameUpdateDlg.h"
#include "Downloads.h"
static double progress;

static size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}
size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	Buffer *buf = (Buffer*)buffer;
	if (buf->buffer == NULL) return size;
	if (buf->offset + size >= buf->size) return 0;
	memcpy(buf->buffer + buf->offset, ptr, size * nmemb);
	buf->offset += size * nmemb;
	return size * nmemb;
}

int wxcurldav_dl_progress_func(void *ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow)
{
	if (ptr != NULL){
		long localLen = *(long*)ptr;
		if (rDlTotal > 0){
			progress = 100.0 * ((rDlNow + (double)localLen) / (rDlTotal + (double)localLen));
		}
	}
	else
		if (rDlTotal > 0){
			progress = 100.0 * (rDlNow / rDlTotal);
		}
	//sprintf(ProogressShow, "%3.1f", progress);
	//printf("%4.1f\n", 100.0 * (rDlNow / rDlTotal));
	return 0;
}

long GetLocalFileLenth(const char* fileName)
{
	    char strTemp[256] = { 0 };
	    strcpy_s(strTemp, fileName);
	    FILE* fp = fopen(strTemp, "rb");
	    if (fp != NULL)
		    {
		        long localLen = _filelength(_fileno(fp));
		        fclose(fp);
		        return localLen;
		    }
	    return 0;
}


int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout, char *cookiefile, const char *cookiejar)
{
	FILE *file = NULL;
	bool flag=FALSE;
	long localLen = 0;
	FileInfo *remote_info = GetHttpModifyTime(url);
	if (remote_info == NULL||remote_info->filesize == NULL)
			return -4;
	if ((_access(savename, 0)) == 0){
		localLen = GetLocalFileLenth(savename);
		if (localLen != NULL&&remote_info != NULL){
			if (localLen >= 100 && localLen < remote_info->filesize){
				flag = TRUE;
			}				
		}
	}
	
	if (flag)
		file = fopen(savename, "ab+");
	else
		file = fopen(savename, "wb+");
	if (file == NULL) return 1;
	
	CURL *curl;
	CURLcode res;
	long retcode = 0;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0L);    //只需要header头  
		curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);    //不需要body  
		//回调进度条函数
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, wxcurldav_dl_progress_func);
		//设置进度条名称
		
		if (ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, FALSE);			//打印调试信息
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		if (cookiefile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefile);//保存cookies
		else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if (cookiejar)curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);//读取cookies
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
		
		if (flag){
			printf("从%d开始断点续传\n", localLen);
			curl_easy_setopt(curl, CURLOPT_RESUME_FROM, localLen); //断点续传
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &localLen);
		}
		
		res = curl_easy_perform(curl);
		if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				fclose(file);
				file = NULL;
				curl_easy_cleanup(curl);
				return -1;
			}
			fclose(file);
			file = NULL;
			curl_easy_cleanup(curl);
			printf("res=%d\n", res);
			return -2;
		}

		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (res != CURLE_OK || (retcode != 200 && retcode!=206)) {
			fclose(file);
			file = NULL;
			curl_easy_cleanup(curl);
			printf("res=%d  retcode=%d\n", res, retcode);
			return -3;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	fclose(file);
	file = NULL;
	return 0;
}
int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiesfile)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	Buffer buf;

	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_REFERER, url);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);			//打印调试信息
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		if (cookiesfile)curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiesfile);
		else curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if (cookies)  curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);
		if (ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				curl_easy_cleanup(curl);
				return -1;
			}
			curl_easy_cleanup(curl);
			return -2;
		}

		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (res != CURLE_OK || retcode != 200) {
			curl_easy_cleanup(curl);
			return -3;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return buf.offset;
}
int GetHTTPMsg(const char  *http, const int size, const int num, char sign1[], const char sign2[], const char  sign3[], char *data1, char *data2, char *data3, const char begainsign[], const char endsign[])
{
	char *x = (char *)malloc(size), *q, *p, *w;
	p = x;
	char Sign[MAX_STRING];
	int SeccessSign = 1;
	strncpy(p, http, size);
	for (int i = 1; i <= num; i++)
	{
		p = x;
		strcpy(p, http);//strcpy会移动指针，必须重新赋值
		if (i == 1)strncpy(Sign, sign1, MAX_STRING);
		if (i == 2)strncpy(Sign, sign2, MAX_STRING);
		if (i == 3)strncpy(Sign, sign3, MAX_STRING);
		p = strstr(p, Sign);
		if (p == NULL)
		{
			printf("失败1");
			SeccessSign = 0;
			continue;
		}
		p += strlen(Sign);
		if (begainsign != NULL){
			p = strstr(p, begainsign);
			if (p == NULL)
			{
				printf("失败2");
				SeccessSign = 0;
				continue;
			}
			p += strlen(begainsign);
		}
		if (*p == '\r'){
			p += 2;
			for (; *p == '\t'; p++);
		}

		q = strstr(p, endsign);
		if (q == NULL)
		{
			printf("失败3");
			SeccessSign = 0;
			continue;
		}
		*q = '\0';
		w = strstr(p, "\r\n");
		if (w != NULL)*w = '\0';
		if (i == 1)strcpy(data1, p);
		if (i == 2)strcpy(data2, p);
		if (i == 3)strcpy(data3, p);
	}
	free(x);
	return SeccessSign;
}

void GetFullPathToFile(char  *pszFullPath, char * pszFilename)
{
	GetModuleFileName(GetModuleHandle(NULL), pszFullPath, MAX_STRING);
	strcpy(strrchr(pszFullPath, '\\') + 1, pszFilename);
}
//取得路径
void GetDocumentsPath(char  *pszFullPath, char * pszFilename)
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	char szDir[200];

	if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl))) {
			// 如果成功返回true
			SHGetPathFromIDList(pidl, szDir);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	sprintf(pszFullPath, "%s\\%s", szDir, pszFilename);
}

int CompareVersion(char *localver, char *remotever, int *LocalVerInt, int *RomoteVerInt){
	if (strlen(localver) == 0)
		return -1;
	char LocalVer[20], RomoteVer[20], TempVar[20] = {0}; int a = 0, c = 0;
	if (localver != NULL)
		if (strlen(localver) != 0)
			c = 1;
	if (remotever != NULL)
		if (strlen(remotever)!=0)
			c = 2;
	for (int b = 1; b <= c; b++)
	{
		if (b == 1)strncpy(TempVar, localver, strlen(localver));
		if (b == 2)strncpy(TempVar, remotever, strlen(remotever));

		for (int i = 0; TempVar[i] != '\0'; i++)
		{
			if (TempVar[i] == '.'){
				a = i;
				for (; TempVar[i] != '\0'; i++){
					TempVar[i] = TempVar[i + 1];
				}
				i = a;
			}
		}
		if (b == 1)strncpy(LocalVer, TempVar, strlen(localver));
		if (b == 2)strncpy(RomoteVer, TempVar, strlen(remotever));
	}
	if (LocalVerInt != NULL )
		*LocalVerInt = atoi(LocalVer);
	if (RomoteVerInt != NULL)
		*RomoteVerInt = atoi(RomoteVer);
	if (c == 2){
		if (atoi(LocalVer) < atoi(RomoteVer))
			return 1;
		else
			return 0;
	}
	else
		return -1;
}
FileInfo *GetFileModifyTime(const char* filepath)
{
	if (filepath == NULL)
		return NULL;
	static FileInfo info;
	info.filesize = 0; info.modifytime = 0;
	LARGE_INTEGER FileSize;
	HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile == NULL) return NULL;

	FILETIME LastModifyTime;

	LARGE_INTEGER largeInt;
	if (GetFileTime(hFile, NULL, NULL, &LastModifyTime)) {
		largeInt.QuadPart = ((DWORD64)LastModifyTime.dwHighDateTime << 32) + LastModifyTime.dwLowDateTime;
		info.modifytime = (long)((largeInt.QuadPart - 116444736000000000) / 10000000);
	}

	GetFileSizeEx(hFile, &FileSize);
	info.filesize = FileSize.QuadPart;
	CloseHandle(hFile);

	return &info;
}
FileInfo *GetHttpModifyTime(const char* url)
{

	static FileInfo fileinfo;
	double size;
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	fileinfo.filesize = 0; fileinfo.modifytime = 0;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FILETIME, 1);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);
		res = curl_easy_perform(curl);
		if (res == CURLE_OK) {
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
			if (res == CURLE_OK && retcode == 200) {
				res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &fileinfo.modifytime);
				if (res != CURLE_OK) fileinfo.modifytime = 0;
				res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
				if (res != CURLE_OK) fileinfo.filesize = 0;
				else fileinfo.filesize = (int)size;
				curl_easy_cleanup(curl);
				fileinfo.filesize = fileinfo.filesize;
				return &fileinfo;
			}
		}
		/* always cleanup */
		curl_easy_cleanup(curl);

	}

	return NULL;
}

void CGameUpdateDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nPos, nMaxRange;
	CProgressCtrl  *pro;
	switch (nIDEvent)
	{
	case PROGRESS1_ID:
		pro = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
		break;
	default:
		return;
	}
	//两种设置进度条位置的方法，其中注释掉的也行
	/*pro->GetRange( nPos, nMaxRange );
	nPos = pro->GetPos()+PROGRESS_INCREASE;
	if( nPos>nMaxRange ) nPos = 0;
	pro->SetPos( nPos );*/
	//pro->OffsetPos(1);//PROGRESS_INCREASE宏定义设置每次递增量
	pro->GetRange(nPos, nMaxRange);
	//nPos = pro->GetPos();
	//printf("%2.1f\n", progress);
	//if (nPos >= nMaxRange) 
	if (progress<=100.0 &&progress>=0.0){
		pro->SetPos((int)progress);
		static char Temp[MAX_STRING];
		sprintf(Temp, "%3.1lf%%", progress);
		GetDlgItem(IDC_STATIC_Progress)->SetWindowText(Temp);
	}
	CDialog::OnTimer(nIDEvent);
}