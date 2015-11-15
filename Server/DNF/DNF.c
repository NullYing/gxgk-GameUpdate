#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_STRING 20480
#define server "tcls.qq.com"
//#define server "113.105.154.159"
#define port 443
#define MAX_DOWNLINE 50
#define file "s.dat"
#define file1 "r.dat"
#define savafile "line.txt"
 
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
	
int main(int argc, char *argv){
	int sockfd, ret;FILE *fp;
	char url[MAX_DOWNLINE][MAX_STRING];
	char DNFVersion[MAX_DOWNLINE][MAX_STRING];
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0){
		perror("socket");
		return -1;
	}

	struct hostent *hptr;
	hptr = (struct hostent *)gethostbyname(server);
	if(hptr == NULL){
		perror("gethostbyname");
        return -1; 
    }

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.s_addr = inet_addr(server);
 	addr.sin_addr.s_addr = *(unsigned long *)hptr->h_addr;
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("connect");
		return 0;
	}

	char buf[MAX_STRING], buf2[MAX_STRING];

	if(send(sockfd, DNFsdat, sizeof(DNFsdat), 0) < 0){
		perror("send");
		return 0;
	}
	bzero(buf, MAX_STRING);
	ret = recv(sockfd, buf, MAX_STRING,0);
	if(ret < 0){
		perror("recv");
		return 0;
	}
	fp = fopen(file1,"wb");
	fwrite(buf, 1, ret, fp);
	fclose(fp);

	//printf("读取接收文件\n");

	char *p = buf, *q = buf2;
	int i;
	memset(buf2, 0, MAX_STRING);
	for (i = 0; i < MAX_STRING; i++, p++){
		if (*p != '\0'){
			*q = *p;
			q++;
		}
	}
	q = NULL;

	//printf("获取地址\n");
	memset(buf, 0, MAX_STRING);
	memcpy(buf, buf2, MAX_STRING);
	p = buf;
	char *m,*n;
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
			if (m != NULL)
				memcpy(url[i], q, m + strlen("_version.list") - q);
			else 
				break;
			p = m + strlen("_version.list");
			//printf("%s\n", url[i]);
		}
		else
			break;
	}
	//printf("写入"savafile"\n");
	int j;
	fp = fopen(savafile, "wb");
	printf("%s=%s\n",DNFVersion[i-1] ,url[i-1]);
	char Temp[MAX_DOWNLINE][MAX_STRING];
	for (j = 0; j < i; j++){
		sprintf(Temp[j],"%s=%s",DNFVersion[j],url[j]);
		if (fwrite(Temp[j], strlen(Temp[j]), 1, fp) != 1){
			printf("地址文件写入错误。!\n");
			fclose(fp);
			return 0;
		}
		fputs("\n", fp);
	}
	fclose(fp);
	return 0;
}
