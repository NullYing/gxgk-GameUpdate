#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_STRING 20480
#define server "tversion2.lol.qq.com"
//#define server "113.105.154.159"
#define port 443
#define MAX_DOWNLINE 50

#define file "s.dat"
#define file1 "r.dat"
#define savafile "line.txt"

char LOLsdat[] = {
	0x00, 0x00, 0x00, 0x40, 0x43, 0x55, 0x00, 0x07,
	0x00, 0x02, 0x00, 0x20, 0x17, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01,
	0x00, 0x05, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x33, 0x2e, 0x31, 0x2e, 0x35, 0x2e,
	0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
	
int main(int argc, char *argv){
	int sockfd, ret;
	char url[MAX_DOWNLINE][MAX_STRING];
	char LOLVersion[MAX_DOWNLINE][MAX_STRING];
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
	FILE *fp;

	char buf[MAX_STRING], buf2[MAX_STRING];

	if(send(sockfd, LOLsdat, sizeof(LOLsdat), 0) < 0){
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
	memcpy(buf, buf2, MAX_STRING);
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
	//memcpy(buf, buf2, MAX_STRING);
	p = buf;char *n;
	for (i = 0;p!=NULL; i++){
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

	//printf("写入"savafile"\n");
	int j;
	fp = fopen(savafile, "wb");
	printf("%s=%s\n",LOLVersion[i-1] ,url[i-1]);
		char Temp[MAX_DOWNLINE][MAX_STRING];
	for (j = 0; j < i; j++){
		sprintf(Temp[j],"%s=%s",LOLVersion[j],url[j]);
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
