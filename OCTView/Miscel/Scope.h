#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "afxwin.h"


// CScope 대화 상자입니다.

class CScope : public CDialogEx
{
	DECLARE_DYNAMIC(CScope)

public:
	CScope(CWnd* pParent = NULL);   // 표준 생성자입니다.
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

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCOPE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CStatic m_ScopeRegion;
};

#endif