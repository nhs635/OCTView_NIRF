// Scope.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Scope.h"
#include "afxdialogex.h"

#include <resource.h>


// CScope ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CScope, CDialogEx)

CScope::CScope(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SCOPE, pParent),
	m_pScopeData(nullptr),
	m_GridPen(PS_SOLID, 1, RGB(0, 160, 0)),
	m_DataPen(PS_SOLID, 1, RGB(255, 255, 255))
{	
}

CScope::~CScope()
{
	if (m_pScopeData) delete[] m_pScopeData;
}

void CScope::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCOPE_REGION, m_ScopeRegion);
}


BEGIN_MESSAGE_MAP(CScope, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CScope �޽��� ó�����Դϴ�.


BOOL CScope::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.	
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CScope::operator() (float* pScopeData, int N, float rMin, float rMax)
{		
	m_pScopeData = new float[N];
	for (int i = 0; i < N; i++)
		m_pScopeData[i] = (float)pScopeData[i];
	
	m_nDataNum = N;
	m_rMax = (float)rMax;
	m_rMin = (float)rMin;

	this->DoModal();
}

void CScope::operator() (unsigned short* pScopeData, int N, float rMin, float rMax)
{
	m_pScopeData = new float[N];
	for (int i = 0; i < N; i++)
		m_pScopeData[i] = (float)pScopeData[i];

	m_nDataNum = N;
	m_rMax = (float)rMax;
	m_rMin = (float)rMin;

	this->DoModal();
}

void CScope::operator() (unsigned char* pScopeData, int N, float rMin, float rMax)
{
	m_pScopeData = new float[N];
	for (int i = 0; i < N; i++)
		m_pScopeData[i] = (float)pScopeData[i];

	m_nDataNum = N;
	m_rMax = (float)rMax;
	m_rMin = (float)rMin;

	this->DoModal();
}


void CScope::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.	
	CPaintDC dcScope(&m_ScopeRegion);

	m_ScopeRegion.GetClientRect(m_ScopeRect);
	dcScope.FillSolidRect(m_ScopeRect, RGB(0, 0, 0));

	float xInc = float(m_ScopeRect.right) / float(m_nDataNum - 1);
	float yInc = float(m_ScopeRect.bottom) / float(m_rMax - m_rMin);
	
	dcScope.SelectObject(&m_GridPen);
	dcScope.MoveTo(0, int(m_ScopeRect.bottom * m_rMax / (m_rMax - m_rMin)));
	dcScope.LineTo(m_ScopeRect.right, int(m_ScopeRect.bottom * m_rMax / (m_rMax - m_rMin)));

	for (int i = 0; i < 20; i++)
	{
		dcScope.MoveTo(m_ScopeRect.right * i / 20, 0);
		dcScope.LineTo(m_ScopeRect.right * i / 20, m_ScopeRect.bottom);
	}

	dcScope.SelectObject(&m_DataPen);
	for (int i = 0; i < m_nDataNum - 1; i++) 
	{
		dcScope.MoveTo(int(xInc * (i    )), m_ScopeRect.bottom - int(yInc * (m_pScopeData[i    ] - m_rMin)));
		dcScope.LineTo(int(xInc * (i + 1)), m_ScopeRect.bottom - int(yInc * (m_pScopeData[i + 1] - m_rMin)));
	}

	CString str;
	str.Format(_T("%d"), int(m_rMax));
	GetDlgItem(IDC_SCOPE_MAX)->SetWindowTextW(str);
	str.Format(_T("%d"), int(m_rMin)); 
	GetDlgItem(IDC_SCOPE_MIN)->SetWindowTextW(str);
	str.Format(_T("%d"), m_nDataNum); 
	GetDlgItem(IDC_SCOPE_N)->SetWindowTextW(str);

}
