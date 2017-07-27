#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "afxwin.h"


// CScope ��ȭ �����Դϴ�.

class CScope : public CDialogEx
{
	DECLARE_DYNAMIC(CScope)

public:
	CScope(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CScope();

	CRect m_ScopeRect;
	CPen m_GridPen;
	CPen m_DataPen;
	
	float* m_pScopeData;
	float m_rMax, m_rMin;

	int m_nDataNum;

	void operator() (float* pScopeData, int N, float rMin, float rMax);
	void operator() (unsigned short* pScopeData, int N, float rMin, float rMax);
	void operator() (unsigned char* pScopeData, int N, float rMin, float rMax);

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCOPE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CStatic m_ScopeRegion;
};

#endif