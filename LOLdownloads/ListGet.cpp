#include "stdafx.h"
#include "GameUpdateDlg.h"

#define port 443

char DNFsdat[] = {
	0x00, 0x00, 0x00, 0x42, 0x43, 0x55, 0x00, 0x07,
	0x00, 0x02, 0x00, 0x20, 0x0b, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x0d, 0x00, 0x00,
	0x00, 0x1f, 0x00, 0x06, 0x00, 0x1a, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0a, 0x31, 0x33, 0x2e, 0x30, 0x2e, 0x33,
	0x31, 0x2e, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x02 };

char LOLsdat[] = {
	0x00, 0x00, 0x00, 0x40, 0x43, 0x55, 0x00, 0x07,
	0x00, 0x02, 0x00, 0x20, 0x17, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01,
	0x00, 0x05, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x33, 0x2e, 0x31, 0x2e, 0x35, 0x2e,
	0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };

int CGameUpdateDlg::LOLGetLine(char *buf)
{
	char url[MAX_DOWNLINE][MAX_STRING];
	char buf2[MAX_BUFFER_SIZE];
	char *p = buf, *q = buf2, *n;
	int i, j;

	memset(buf2, 0, MAX_BUFFER_SIZE);
	for (i = 0; i < MAX_BUFFER_SIZE; i++, p++){
		if (*p != '\0'){
			*q = *p;
			q++;
		}
	}
	q = NULL;

	//printf("获取地址\n");
	memset(buf, 0, MAX_BUFFER_SIZE);
	memcpy(buf, buf2, MAX_BUFFER_SIZE);
	p = buf;
	for (i = 0;; i++){
		q = NULL;
		q = strstr(p, "http");
		if (q != NULL){
			sscanf(q, "%[0-9,a-z,A-Z,,:,/,.,-]", url[i]);
			p = q + strlen(url[i]) + 1;
			//printf("%s\n", url[i]);
		}
		else
			break;
	}

	//printf("获取文件名\n");
	//memset(buf, 0, MAX_STRING);
	//memcpy(buf, buf2, MAX_STRING);
	p = buf;
	for (i = 0; p != NULL; i++){
		q = NULL;
		n = strstr(p, "_version.list");
		if (n != NULL){
			n += strlen("_version.list");
			sscanf(n, "%*[^0-9]%[0-9,.]", LOLVersion[i]);
		}
		q = strstr(p, "_version.list");
		if (q != NULL){
			p = q + strlen("_version.list") + 1;
			q -= 32;
			*(q + 32 + strlen("_version.list")) = '\0';
			//printf("%s\n", q);
			strncat(url[i], q, MAX_STRING);
		}
		else
			break;
	}
	for (j = 0; j < i; j++){
		strncpy(ListUrl[j], url[j], MAX_STRING);
		//printf("%s\n", ListUrl[j]);
	}
	if (i > 0)
		strcpy(m_SVersion, LOLVersion[i - 1]);
	printf("LOL The News Version:%s\n", m_SVersion);
	if (strlen(m_SVersion) != 0){
		CompareVersion(m_SVersion, NULL, &m_SVersionInt, NULL);
		return 1;
	}
	/*
	if (j != 0){
		p = strstr(ListUrl[j - 1], "autopatch/");
		p += strlen("autopatch/");
		if (p != NULL)
			q = strchr(p, '/');
		else
			return 0;
		memcpy(Ver, p, q - p);
		m_SVersionInt = atoi(Ver);
		

		sprintf(m_SVersion, "%c.%c.%c.%c", Ver[0], Ver[1], Ver[2], Ver[3]);
		printf("LOL The News Version:%s\n", m_SVersion);
		return 1;
	}
	*/

	return 0;
}
int CGameUpdateDlg::DNFGetLine(char *buf){
	char url[MAX_DOWNLINE][MAX_STRING];
	char buf2[MAX_BUFFER_SIZE];
	
	char *p = buf, *q = buf2, *m,*n;
	int i,j;

	memset(buf2, 0, MAX_BUFFER_SIZE);
	for (i = 0; i < MAX_BUFFER_SIZE; i++, p++){
		if (*p != '\0'){
			*q = *p;
			q++;
		}
	}
	q = NULL;

	//printf("获取地址\n");
	memset(buf, 0, MAX_BUFFER_SIZE);
	memcpy(buf, buf2, MAX_BUFFER_SIZE);
	p = buf;
	for (i = 0;; i++){
		q = NULL;
		n = strstr(p, "_version.list");
		if (n != NULL){
			n += strlen("_version.list");
			sscanf(n, "%*[^0-9]%[0-9,.]", DNFVersion[i]);
		}
		q = strstr(p, "http");
		if (q != NULL){
			m = strstr(q, "_version.list");
			if (m != NULL)memcpy(url[i], q, m + strlen("_version.list") - q); else break;
			p = m + strlen("_version.list");
			//printf("%s\n", url[i]);
		}
		else
			break;
	}
	for (j = 0; j < i; j++){
		strncpy_s(ListUrl[j], url[j], MAX_STRING);
		//printf("%s\n", ListUrl[j]);
	}
	if (i > 0)
		strcpy(m_SVersion, DNFVersion[i - 1]);
	printf("DNF The News Version:%s\n", m_SVersion);
	if (strlen(m_SVersion) != 0){
		CompareVersion(m_SVersion, NULL, &m_SVersionInt, NULL);
		return 1;
	}
	return 0;
}
int CGameUpdateDlg::ListGet(char *server){
	int sockfd, ret = -1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0){
		printf("socket");
		return NULL;
	}

	struct hostent *hptr;
	hptr = (struct hostent *)gethostbyname(server);
	if(hptr == NULL){
		printf("Err gethostbyname");
		return NULL;
    }

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.s_addr = inet_addr(server);
 	addr.sin_addr.s_addr = *(unsigned long *)hptr->h_addr;
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		printf("Err connect");
		return NULL;
	}

	char buf[MAX_BUFFER_SIZE];
	if (strcmp(server, DNFServer) == 0)
		ret = send(sockfd, DNFsdat, sizeof(DNFsdat), 0);
	if (strcmp(server, LOLServer) == 0)
		ret = send(sockfd, LOLsdat, sizeof(LOLsdat), 0);
	if (ret < 0){
		printf("Err send");
		return NULL;
	}
	memset(buf, 0, MAX_BUFFER_SIZE);
	ret = recv(sockfd, buf, MAX_BUFFER_SIZE, 0);
	if (strcmp(server, LOLServer) == 0)
		LOLGetLine(buf);
	if (strcmp(server, DNFServer) == 0)
		DNFGetLine(buf);

	GetDlgItem(IDC_STATIC_VerSer)->SetWindowText(m_SVersion);

	return 0;
}
