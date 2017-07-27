// Imshow.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Imshow.h"
#include "afxdialogex.h"

#include <resource.h>


// Imshow ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Imshow, CDialogEx)

Imshow::Imshow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_IMSHOW, pParent),
	m_pBitmap(nullptr), m_pImageData(nullptr)
{
}

Imshow::~Imshow()
{
	if (m_pBitmap) delete[] m_pBitmap;
	if (m_pImageData) delete[] m_pImageData;
}

void Imshow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMSHOW, m_StaticImshow);
}


BEGIN_MESSAGE_MAP(Imshow, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Imshow �޽��� ó�����Դϴ�.


BOOL Imshow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void Imshow::operator() (float* pImageData, int width, int height, float cMin, float cMax)
{
	m_nImageSize = width * height;
	m_pImageData = new float[m_nImageSize];

	memcpy(m_pImageData, pImageData, sizeof(float) * m_nImageSize);
	m_cMax = cMax; m_cMin = cMin;

	m_pBitmap = new MyBitmap;
	m_pBitmap->SetBitmap(height, width, 0);
	m_pBitmap->ScaleImage(m_pImageData, m_cMin, m_cMax);

	this->DoModal();
}

void Imshow::operator() (uint8_t* pImageData, int width, int height) //, float cMin, float cMax)
{
	m_nImageSize = width * height;
	//m_pImageData = new float[m_nImageSize];

	//memcpy(m_pImageData, pImageData, sizeof(float) * m_nImageSize);
	//m_cMax = cMax; m_cMin = cMin;

	m_pBitmap = new MyBitmap;
	m_pBitmap->SetBitmap(height, width, 3);
	memcpy(m_pBitmap->GetPtr(), pImageData, sizeof(uint8_t) * m_nImageSize);
	//m_pBitmap->ScaleImage(m_pImageData, m_cMin, m_cMax);

	this->DoModal();
}

/*
void Imshow::operator() (unsigned short* pScopeData, int N, float rMin, float rMax)
{
m_pScopeData = new float[N];
for (int i = 0; i < N; i++)
m_pScopeData[i] = (float)pScopeData[i];

m_nDataNum = N;
m_rMax = (float)rMax;
m_rMin = (float)rMin;

this->DoModal();
}

void Imshow::operator() (unsigned char* pScopeData, int N, float rMin, float rMax)
{
m_pScopeData = new float[N];
for (int i = 0; i < N; i++)
m_pScopeData[i] = (float)pScopeData[i];

m_nDataNum = N;
m_rMax = (float)rMax;
m_rMin = (float)rMin;

this->DoModal();
}
*/

void Imshow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.	
	CPaintDC dImshow(&m_StaticImshow);
	CDC dcMem;

	// Assigning memory DC
	CRect RectWnd;
	m_StaticImshow.GetClientRect(&RectWnd);

	dcMem.CreateCompatibleDC(&dc);
	CBitmap btmp;
	btmp.CreateCompatibleBitmap(&dc, RectWnd.Width(), RectWnd.Height());
	dcMem.SelectObject(&btmp);
	
	SetStretchBltMode(dcMem.m_hDC, COLORONCOLOR);
	StretchDIBits(dcMem.m_hDC,
		0, 0, RectWnd.Width(), RectWnd.Height(),
		0, 0, m_pBitmap->GetLpBmpInfo()->bmiHeader.biWidth, m_pBitmap->GetLpBmpInfo()->bmiHeader.biHeight,
		m_pBitmap->GetPtr(), m_pBitmap->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

	dImshow.StretchBlt(RectWnd.left, RectWnd.top, RectWnd.Width(), RectWnd.Height(),
		&dcMem, RectWnd.left, RectWnd.top, RectWnd.Width(), RectWnd.Height(), SRCCOPY);
	//dImshow.TransparentBlt(RectWnd.left, RectWnd.top, RectWnd.Width(), RectWnd.Height(),
	//	&dcMem, 0, 0, RectWnd.Width(), RectWnd.Height(), 
	//	RGB(m_pBitmap->GetLpBmpInfo()->bmiColors[0].rgbRed, 
	//		m_pBitmap->GetLpBmpInfo()->bmiColors[0].rgbGreen, 
	//		m_pBitmap->GetLpBmpInfo()->bmiColors[0].rgbBlue));


}
