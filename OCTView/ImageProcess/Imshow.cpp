// Imshow.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Imshow.h"
#include "afxdialogex.h"

#include <resource.h>


// Imshow 대화 상자입니다.

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


// Imshow 메시지 처리기입니다.


BOOL Imshow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.	
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
