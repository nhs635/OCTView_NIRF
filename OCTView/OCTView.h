
// OCTView.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// COCTViewApp:
// �� Ŭ������ ������ ���ؼ��� OCTView.cpp�� �����Ͻʽÿ�.
//

class COCTViewApp : public CWinApp
{
public:
	COCTViewApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern COCTViewApp theApp;