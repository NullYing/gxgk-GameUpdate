
// GameUpdate.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGameUpdateApp: 
// �йش����ʵ�֣������ GameUpdate.cpp
//

class CGameUpdateApp : public CWinApp
{
public:
	CGameUpdateApp();

// ��д
public:
	BOOL CheckCommandLine();
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGameUpdateApp theApp;
