#include "stdafx.h"
#include "GameUpdateDlg.h"
#include <winioctl.h>
#include "md5.h"

//滤除字符串起始位置的空格
void TrimStart(LPTSTR pBuf)
{
	if (*pBuf != 0x20)
		return;

	LPTSTR pDest = pBuf;
	LPTSTR pSrc = pBuf + 1;
	while (*pSrc == 0x20)
		++pSrc;

	while (*pSrc)
	{
		*pDest = *pSrc;
		++pDest;
		++pSrc;
	}
	*pDest = 0;
}

//把WORD数组调整字节序为little-endian，并滤除字符串结尾的空格。
void ToLittleEndian(PUSHORT pWords, int nFirstIndex, int nLastIndex, LPTSTR pBuf)
{
	int index;
	LPTSTR pDest = pBuf;
	for (index = nFirstIndex; index <= nLastIndex; ++index)
	{
		pDest[0] = pWords[index] >> 8;
		pDest[1] = pWords[index] & 0xFF;
		pDest += 2;
	}
	*pDest = 0;

	//trim space at the endof string; 0x20: _T(' ')
	--pDest;
	while (*pDest == 0x20)
	{
		*pDest = 0;
		--pDest;
	}
}
//
// Model Number: 40 ASCII Chars
// SerialNumber: 20 ASCII Chars
//
BOOL GetPhyDriveSerial(LPTSTR pModelNo, LPTSTR pSerialNo)
{
	//-1是因为 SENDCMDOUTPARAMS 的结尾是 BYTE bBuffer[1];
	BYTE IdentifyResult[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	DWORD dwBytesReturned;
	GETVERSIONINPARAMS get_version;
	SENDCMDINPARAMS send_cmd = { 0 };

	HANDLE hFile = CreateFile(_T("\\\\.\\PHYSICALDRIVE0"), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//get version
	DeviceIoControl(hFile, SMART_GET_VERSION, NULL, 0,
		&get_version, sizeof(get_version), &dwBytesReturned, NULL);

	//identify device
	send_cmd.irDriveRegs.bCommandReg = (get_version.bIDEDeviceMap & 0x10) ? ATAPI_ID_CMD : ID_CMD;
	DeviceIoControl(hFile, SMART_RCV_DRIVE_DATA, &send_cmd, sizeof(SENDCMDINPARAMS) - 1,
		IdentifyResult, sizeof(IdentifyResult), &dwBytesReturned, NULL);
	CloseHandle(hFile);

	//adjust the byte order
	PUSHORT pWords = (USHORT*)(((SENDCMDOUTPARAMS*)IdentifyResult)->bBuffer);
	ToLittleEndian(pWords, 27, 46, pModelNo);
	ToLittleEndian(pWords, 10, 19, pSerialNo);
	return TRUE;
}

int CGameUpdateDlg::PCsign(char *DiskSign) {
	TCHAR szModelNo[48], szSerialNo[24];
	if (GetPhyDriveSerial(szModelNo, szSerialNo))
	{
		_tprintf(_T("Model No: %s\n"), szModelNo);
		TrimStart(szSerialNo);
		_tprintf(_T("Serial No: %s\n"), szSerialNo);
	}

	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	char md5[33];
	int i;

	int md5_len = 16;
	strcpy((char *)data, szSerialNo);
	bytes = strlen((char *)data);
	MD5Init(&mdContext);
	if (strcmp((char *)data, "") != 0)
	{
		MD5Update(&mdContext, data, bytes);
	}
	MD5Final(&mdContext);


	memset(md5, 0, (md5_len + 1));

	if (md5_len == 16)
	{
		for (i = 4; i<12; i++)
		{
			sprintf(&md5[(i - 4) * 2], "%02X", mdContext.digest[i]);
		}
	}
	if (strcmp((char *)md5, "") == 0)
	{
		fprintf(stderr, "failed.\n");
		return NULL;
	}
	_tprintf(_T("SN Hash: %s\n"), md5);
	strcpy(DiskSign, md5);
	return 1;
}
int CGameUpdateDlg::Feedback(int Status)
{
	char http[MAX_STRING] = {0}, url[MAX_STRING];

	sprintf(url, UpdataUrl "log.php?Sign=%s&Status=%d", DiskSign, Status);
	int ret=DownLoadFileToBuffer(http, MAX_STRING, url, NULL, TIMEOUT, NULL, NULL);
	if (ret >= 0)
		return 1;
	return 0;
}