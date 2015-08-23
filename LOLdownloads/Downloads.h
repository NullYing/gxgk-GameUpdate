#ifndef UPDATE_H
#define UPDATE_H


#define MAX_BUFFER_SIZE 102040
#define MAX_STRING 2048

typedef struct _buffer{
	char *buffer;
	size_t size;
	int offset;

} Buffer;
typedef struct _FileInfo{
	time_t modifytime;
	size_t filesize;
} FileInfo;

FileInfo *GetHttpModifyTime(const char* url);
FileInfo *GetFileModifyTime(const char* url);
int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout, char *cookiefile, const char *cookiejar);
int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *cookiesfile);
int CompareVersion(char *localver, char *remotever, int *LocalVerInt, int *RomoteVerInt);
void GetFullPathToFile(char  *pszFullPath, char * pszFilename);
void GetDocumentsPath(char  *pszFullPath, char * pszFilename);
int GetHTTPMsg(const char  *http, const int size, const int num, char sign1[], const char sign2[], const char  sign3[], char *data1, char *data2, char *data3, const char begainsign[], const char endsign[]);
#endif