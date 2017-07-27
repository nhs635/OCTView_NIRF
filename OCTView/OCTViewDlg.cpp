
// OCTViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OCTView.h"
#include "OCTViewDlg.h"
#include "afxdialogex.h"

#include <chrono>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COCTViewDlg 대화 상자

COCTViewDlg::COCTViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OCTVIEW_DIALOG, pParent),
	m_SliderCurFrame(0), m_nFrame(1), m_nScansFFT(0), m_CheckCircularize(false), m_CheckShowGuideline(false),
	m_CircCenterPen(PS_SOLID, 1, RGB(0, 255, 0)), m_SheathThicknessPen(PS_SOLID, 1, RGB(255, 255, 0)), m_CircRadiusPen(PS_SOLID, 1, RGB(255, 0, 0)),
	m_ToggleMeasureDistance(FALSE), m_CheckSimulWriting(TRUE), m_CheckSingleFrame(FALSE), m_CheckFilipHorizontal(TRUE),
	m_pBitmapOCT(nullptr), m_pBitmapCircOCT(nullptr),
	m_pBitmapProjection(nullptr), m_pColorbarProjection(nullptr),
	m_pBitmapNirfMap(nullptr), m_pColorbarNirfMap(nullptr)
	, m_EditNirfOffset(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_OCTVIEW);
}


void COCTViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDER_CUR_FRAME, m_SliderCurFrame);

	DDX_Text(pDX, IDC_EDIT_PIXEL_SIZE, m_PixelSize);
	DDX_Text(pDX, IDC_EDIT_DISCOM_VALUE, m_DiscomValue);
	DDX_Text(pDX, IDC_EDIT_BUFFER_SIZE, m_BufferSize);

	DDX_Text(pDX, IDC_EDIT_RES_ALINE_MAX_DB, m_EditResMaxDb);
	DDX_Text(pDX, IDC_EDIT_RES_ALINE_MIN_DB, m_EditResMinDb);
	DDX_Text(pDX, IDC_EDIT_RES_CIRC_CENTER, m_EditCircCenter);
	DDX_Text(pDX, IDC_EDIT_RES_SHEATH_THICKNESS, m_EditSheathThickness);
	DDX_Text(pDX, IDC_EDIT_CIRC_RADIUS_RATIO, m_EditCircRadiusRatio);

	DDX_Text(pDX, IDC_EDIT_RES_NIRF_MAP_MAX, m_EditResMaxNirfMap);
	DDX_Text(pDX, IDC_EDIT_RES_NIRF_MAP_MIN, m_EditResMinNirfMap);

	DDX_Check(pDX, IDC_CHECK_RES_CIRCULARIZE, m_CheckCircularize);
	DDX_Check(pDX, IDC_CHECK_RES_GUIDELINE, m_CheckShowGuideline);
	DDX_CBIndex(pDX, IDC_COMBO_RES_LUT, m_ComboResLut);

	DDX_Control(pDX, IDC_STATIC_RES_IMAGE, m_StaticResImage);
	DDX_Control(pDX, IDC_STATIC_RES_PROJECTION, m_StaticResProjection);
	DDX_Control(pDX, IDC_STATIC_RES_COLORBAR_PROJECTION, m_StaticResColorbarProjection);
	DDX_Control(pDX, IDC_STATIC_RES_NIRF_MAP, m_StaticResNirfMap);
	DDX_Control(pDX, IDC_STATIC_RES_COLORBAR_NIRF_MAP, m_StaticResColorbarNirfMap);

	DDX_Control(pDX, IDC_PROGRESS_RES, m_ProgressRes);
	DDX_Check(pDX, IDC_TOGGLE_MEASURE_DISTANCE, m_ToggleMeasureDistance);
	DDX_Check(pDX, IDC_CHECK_SIMUL_WRITING, m_CheckSimulWriting);
	DDX_Check(pDX, IDC_CHECK_SINGLE_FRAME, m_CheckSingleFrame);
	DDX_Check(pDX, IDC_CHECK_HORIZON_FLIP, m_CheckFilipHorizontal);
	DDX_Text(pDX, IDC_EDIT_NIRF_OFFSET, m_EditNirfOffset);
}

BEGIN_MESSAGE_MAP(COCTViewDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_RES_CIRCULARIZE, &COCTViewDlg::OnBnClickedCheckCircularize)
	ON_CBN_SELCHANGE(IDC_COMBO_RES_LUT, &COCTViewDlg::OnCbnSelchangeComboResLut)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PROCESSING, &COCTViewDlg::OnBnClickedButtonImageProcessing)
	ON_EN_CHANGE(IDC_EDIT_RES_ALINE_MAX_DB, &COCTViewDlg::OnEnChangeEditResAlineMaxDb)
	ON_EN_CHANGE(IDC_EDIT_RES_ALINE_MIN_DB, &COCTViewDlg::OnEnChangeEditResAlineMinDb)
	ON_EN_CHANGE(IDC_EDIT_RES_MAX_NIRF, &COCTViewDlg::OnEnChangeEditResNirfMapMax)
	ON_EN_CHANGE(IDC_EDIT_RES_MIN_NIRF, &COCTViewDlg::OnEnChangeEditResNirfMapMin)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_EN_CHANGE(IDC_EDIT_RES_CIRC_CENTER, &COCTViewDlg::OnEnChangeEditResCircCenter)
	ON_EN_CHANGE(IDC_EDIT_RES_SHEATH_THICKNESS, &COCTViewDlg::OnEnChangeEditResSheathThickness)
	ON_BN_CLICKED(IDC_CHECK_RES_GUIDELINE, &COCTViewDlg::OnBnClickedCheckResGuideline)
	ON_BN_CLICKED(IDC_TOGGLE_MEASURE_DISTANCE, &COCTViewDlg::OnBnClickedToggleMeasureDistance)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_BITMAP, &COCTViewDlg::OnBnClickedButtonSaveBitmap)
	ON_BN_CLICKED(IDC_CHECK_SIMUL_WRITING, &COCTViewDlg::OnBnClickedCheckSimulWriting)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PROJECTION, &COCTViewDlg::OnBnClickedButtonSaveProjection)
	ON_BN_CLICKED(IDC_CHECK_HORIZON_FLIP, &COCTViewDlg::OnBnClickedCheckHorizonFlip)
	ON_EN_CHANGE(IDC_EDIT_CIRC_RADIUS_RATIO, &COCTViewDlg::OnEnChangeEditCircRadiusRatio)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NIRF_OFFSET, &COCTViewDlg::OnDeltaposSpinNirfOffset)
	ON_EN_CHANGE(IDC_EDIT_NIRF_OFFSET, &COCTViewDlg::OnEnChangeEditNirfOffset)
	ON_EN_CHANGE(IDC_EDIT_RES_NIRF_MAP_MIN, &COCTViewDlg::OnEnChangeEditResNirfMapMin)
	ON_EN_CHANGE(IDC_EDIT_RES_NIRF_MAP_MAX, &COCTViewDlg::OnEnChangeEditResNirfMapMax)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// COCTViewDlg 메시지 처리기입니다.

BOOL COCTViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Status bar initialization
	m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);
	int strPartDim[3] = { 600, 900, 1500 }; // , 300, 450 - 1};
	m_StatusBar.SetParts(3, strPartDim);
	m_StatusBar.SetText(_T("OCT Image Processing Software (OCT NIRF)"), 0, 0);
	m_StatusBar.SetText(_T("(000, 000)"), 1, 0); // 000 (00.0 dB)"), 1, 0);
	m_StatusBar.SetText(_T("Radius: 0"), 2, 0);

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	SetWindowTextW(_T("OCTView_NIRF v1.1.0."));

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// Image View 부분 초기화 ////////////////////////////////////////////////////////////////////////////
	CWnd *pWnd = this->GetDlgItem(IDC_STATIC_RES_IMAGE);
	pWnd->GetWindowRect(&m_ResImageRect);
	this->ScreenToClient(&m_ResImageRect);

	pWnd = this->GetDlgItem(IDC_STATIC_RES_PROJECTION);
	pWnd->GetWindowRect(&m_ResProjectionRect);
	this->ScreenToClient(&m_ResProjectionRect);
		
	pWnd = this->GetDlgItem(IDC_STATIC_RES_COLORBAR_PROJECTION);
	pWnd->GetWindowRect(&m_CBarRegionRect1);
	this->ScreenToClient(&m_CBarRegionRect1);

	pWnd = this->GetDlgItem(IDC_STATIC_RES_NIRF_MAP);
	pWnd->GetWindowRect(&m_ResNirfMapRect);
	this->ScreenToClient(&m_ResNirfMapRect);

	pWnd = this->GetDlgItem(IDC_STATIC_RES_COLORBAR_NIRF_MAP);
	pWnd->GetWindowRect(&m_CBarRegionRect2);
	this->ScreenToClient(&m_CBarRegionRect2);
	
	m_VisRegionRectWnd = CRect(m_ResImageRect.left, m_ResImageRect.top,
		m_CBarRegionRect2.right, m_ResImageRect.bottom);

	pt = { -m_ResImageRect.left, -m_ResImageRect.top };
	m_ResImageRect.OffsetRect(pt);
	m_ResProjectionRect.OffsetRect(pt);
	m_CBarRegionRect1.OffsetRect(pt);
	m_ResNirfMapRect.OffsetRect(pt);
	m_CBarRegionRect2.OffsetRect(pt);

	// Control Initialization
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetRange(0, m_nFrame - 1);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetLineSize(1);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetPageSize(20);

	// Get ini file
	TCHAR path[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, path);
	CString strPath = path;
	m_strIniPath = strPath + (CString)_T("\\OCTView.ini");
	GetIniFile();

	// Set Size	
	SetSize(GetDlgItemInt(IDC_EDIT_NSCANS), GetDlgItemInt(IDC_EDIT_NALINES)); // ini value
	OnEnChangeEditCircRadiusRatio();

	// Visualization Bitmap File
	SetBitmaps(m_nFrame);

	// Buffers Setting
	SetBuffers(m_nFrame);		
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COCTViewDlg::SetSize(int scans, int alines)
{
	m_nScans = scans;
	m_nScansFFT = GET_NEAR_2_POWER(scans);

	UpdateData(TRUE);
	m_radius = int(_wtof(m_EditCircRadiusRatio) * (double)m_nScansFFT / 2.0);
	m_radius = (m_radius >> 2) << 2;

	CString str;
	str.Format(_T("Radius: %d"), m_radius);
	m_StatusBar.SetText(str, 2, 0);

	m_nAlines = alines;
	m_nAlines4 = (m_nAlines + 3) & ~3;

	m_nSizeFrame = m_nScans * m_nAlines;
	m_nSizeImageOCT = m_nScansFFT / 2 * m_nAlines;

	circ_OCT = circularize(m_radius, m_nAlines, false);
	median_filter = medfilt_8u(m_nScansFFT / 2, m_nAlines, 3);
}


void COCTViewDlg::SetBitmaps(int nFrame)
{
	if (m_pBitmapOCT) {	delete m_pBitmapOCT; m_pBitmapOCT = nullptr; }
	if (m_pBitmapCircOCT) {	delete m_pBitmapCircOCT; m_pBitmapCircOCT = nullptr; }	
	if (m_pBitmapProjection) { delete m_pBitmapProjection; m_pBitmapProjection = nullptr; }
	if (m_pColorbarProjection) { delete m_pColorbarProjection; m_pColorbarProjection = nullptr; }
	if (m_pBitmapNirfMap) { delete m_pBitmapNirfMap; m_pBitmapNirfMap = nullptr; }
	if (m_pColorbarNirfMap) { delete m_pColorbarNirfMap; m_pColorbarNirfMap = nullptr; }

	m_pBitmapOCT = new MyBitmap;
	m_pBitmapOCT->SetBitmap(m_nScansFFT / 2, m_nAlines, 3, m_ComboResLut);
	m_pBitmapCircOCT = new MyBitmap;
	m_pBitmapCircOCT->SetBitmap(2 * m_radius, 2 * m_radius, 3, m_ComboResLut);

	m_pBitmapProjection = new MyBitmap;
	m_pBitmapProjection->SetBitmap(nFrame, m_nAlines, 1, m_ComboResLut);
	m_pColorbarProjection = new MyBitmap;
	m_pColorbarProjection->SetBitmap(256, 4, 1, m_ComboResLut); 
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 4; j++)
			m_pColorbarProjection->SetPtr((BYTE)i, i, j);	

	m_pBitmapNirfMap = new MyBitmap;
	m_pBitmapNirfMap->SetBitmap(nFrame, m_nAlines, 1, LUT_HOT);
	m_pColorbarNirfMap = new MyBitmap;
	m_pColorbarNirfMap->SetBitmap(256, 4, 1, LUT_HOT);
	for (int i = 0; i < 256; i++)
		for (int j = 0; j < 4; j++)
			m_pColorbarNirfMap->SetPtr((BYTE)i, i, j);
}


void COCTViewDlg::SetBuffers(int nFrame)
{
	// Clear Previous Buffers
	std::vector<np::Array<float, 2>>().swap(m_OCT_Image32f);

	// Data Buffers
	for (int i = 0; i < nFrame; i++)
	{
		np::Array<float, 2> buffer = np::Array<float, 2>(m_nScansFFT / 2, m_nAlines);
		m_OCT_Image32f.push_back(buffer);
	}
	m_Projection32f = np::Array<float, 2>(m_nAlines4, nFrame);
	m_NirfMap32f = np::Array<float, 2>(m_nAlines4, nFrame);
	memset(m_NirfMap32f.raw_ptr(), 0, sizeof(float) * m_NirfMap32f.length());

	// Visualization Buffers
	m_TempBitmapOCT = np::Array<BYTE, 2>(m_nAlines, m_nScansFFT / 2);
	m_TempBitmapCircOCT = np::Array<BYTE, 2>(2 * m_radius, 2 * m_radius);

	m_OCT_Image8u = np::Array<BYTE, 2>(m_nScansFFT / 2, m_nAlines4);
}


BOOL COCTViewDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SetIniFile();

	return CDialogEx::DestroyWindow();
}


void COCTViewDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	std::vector<np::Array<float, 2>>().swap(m_OCT_Image32f);

	delete m_pBitmapOCT;
	delete m_pBitmapCircOCT;
	delete m_pBitmapProjection;
	delete m_pColorbarProjection;
	delete m_pBitmapNirfMap;
	delete m_pColorbarNirfMap;

	CDialogEx::PostNcDestroy();
}


void COCTViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void COCTViewDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt1 = point;
	pt1.x = pt1.x + pt.x;
	pt1.y = pt1.y + pt.y;

	if (m_ResImageRect.PtInRect(pt1) && m_ToggleMeasureDistance)
	{
		pt_dist1[n_click] = { point.x, point.y }; 
		pt1 = { pt1.x - m_ResImageRect.left, pt1.y - m_ResImageRect.top };

		pt_dist2[n_click].x = (LONG)floor(double(2 * m_radius * pt1.x) / double(m_ResImageRect.Width()));
		pt_dist2[n_click].y = (LONG)floor(double(2 * m_radius * pt1.y) / double(m_ResImageRect.Width()));

		if (pt_dist2[n_click].y < 2 * m_radius)
		{		
			CClientDC dc(this);
			CBrush brush; brush.CreateSolidBrush(RGB(255, 0, 255));
			dc.SelectStockObject(NULL_PEN);
			dc.SelectObject(&brush);
			dc.Ellipse(pt_dist1[n_click].x - 2, pt_dist1[n_click].y - 2, pt_dist1[n_click].x + 2, pt_dist1[n_click].y + 2);

			n_click++;

			if (n_click == 2)
			{
				CPen pen(PS_SOLID, 1, RGB(255, 0, 255));
				dc.SelectObject(&pen);
				dc.MoveTo(pt_dist1[0].x, pt_dist1[0].y);
				dc.LineTo(pt_dist1[1].x, pt_dist1[1].y);

				UpdateData(TRUE);
				double dist = _wtof(m_PixelSize) / 1000.0 * sqrt( (pt_dist2[0].x - pt_dist2[1].x) * (pt_dist2[0].x - pt_dist2[1].x) + (pt_dist2[0].y - pt_dist2[1].y) * (pt_dist2[0].y - pt_dist2[1].y) );
				CString str;
				str.Format(_T("%.3f mm"), dist);

				dc.SetTextColor(RGB(255, 0, 255));
				dc.TextOutW((pt_dist1[0].x + pt_dist1[1].x) / 2, (pt_dist1[0].y + pt_dist1[1].y) / 2, str);

				m_ToggleMeasureDistance = FALSE;
				UpdateData(FALSE);
			}
		}
	}
	else
	{
		if (!m_ToggleMeasureDistance)
			InvalidateRect(m_VisRegionRectWnd, FALSE);
	}

	if (m_ResProjectionRect.PtInRect(pt1) || m_ResNirfMapRect.PtInRect(pt1)) 
	{
		if (m_ResProjectionRect.PtInRect(pt1))
			pt1 = { pt1.x - m_ResProjectionRect.left, pt1.y - m_ResProjectionRect.top };
		else if (m_ResNirfMapRect.PtInRect(pt1))
			pt1 = { pt1.x - m_ResNirfMapRect.left, pt1.y - m_ResNirfMapRect.top };

		int frame_pos = (int)floor(double(m_nFrame * (m_ResProjectionRect.Height() - pt1.y)) / double(m_ResProjectionRect.Height()));

		m_SliderCurFrame = frame_pos;
		UpdateData(FALSE);

		CString str;
		str.Format(_T("Current Frame : %4d / %d"), m_SliderCurFrame + 1, m_nFrame);
		SetDlgItemText(IDC_STATIC_CUR_FRAME, str);

		InvalidateRect(m_VisRegionRectWnd, FALSE);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void COCTViewDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pt1 = point;
	pt1.x = pt1.x + pt.x;
	pt1.y = pt1.y + pt.y;

	if (m_ResNirfMapRect.PtInRect(pt1))
	{
		if (m_NirfSignal32f.raw_ptr())
		{
			UpdateData(TRUE);

			CScope scope;
			scope(m_NirfSignal32f.raw_ptr(), m_NirfSignal32f.length(), (float)_wtof(m_EditResMinNirfMap), (float)_wtof(m_EditResMaxNirfMap));
		}
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void COCTViewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.	
	
	CPoint pt1 = point, pt2;
	pt1.x = pt1.x + pt.x;
	pt1.y = pt1.y + pt.y;

	if (m_ResImageRect.PtInRect(pt1))
	{
		pt2 = { pt1.x - m_ResImageRect.left, pt1.y - m_ResImageRect.top };

		if (!m_CheckCircularize)
		{		
			pt2.x = (LONG)floor(double(m_nAlines * pt2.x) / double(m_ResImageRect.Width()));
			pt2.y = (LONG)floor(double(m_nScansFFT / 2 * pt2.y) / double(m_ResImageRect.Height()));

			CString str;
			str.Format(_T("(%4d, %4d)"), // %3d (%.1f dB)"),
				pt2.x, pt2.y);// , *m_pBitmapOCT->GetPtr(m_nScansFFT / 2 - 1 - pt2.y, pt2.x), m_OCT_Image32f.at(m_SliderCurFrame)(pt2.y, pt2.x));
			m_StatusBar.SetText(str, 1, 0);			
		}
		else
		{
			pt2.x = (LONG)floor(double(2 * m_radius * pt2.x) / double(m_ResImageRect.Width()));
			pt2.y = (LONG)floor(double(2 * m_radius * pt2.y) / double(m_ResImageRect.Width()));

			if (pt2.y < 2 * m_radius)
			{
				CString str;
				str.Format(_T("(%4d, %4d)"), // %3d (???? dB)"),
					pt2.x, pt2.y); // , *m_pBitmapCircOCT->GetPtr(2 * m_radius - 1 - pt2.y, pt2.x)); // , m_OCT_Image32f.at(m_SliderCurFrame)(pt2.y, pt2.x));
				m_StatusBar.SetText(str, 1, 0);
			}
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL COCTViewDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void COCTViewDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

}


void COCTViewDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	DestroyWindow();
}


void COCTViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	CPaintDC dcResImage(&m_StaticResImage);
	CPaintDC dcProjection(&m_StaticResProjection);
	CPaintDC dcCBar1(&m_StaticResColorbarProjection);
	CPaintDC dcNirfmap(&m_StaticResNirfMap);
	CPaintDC dcCBar2(&m_StaticResColorbarNirfMap);
	CDC dcMem;
	
	// Assigning memory DC
	dcMem.CreateCompatibleDC(&dc);
	CBitmap btmp;
	btmp.CreateCompatibleBitmap(&dc, m_VisRegionRectWnd.Width(), m_VisRegionRectWnd.Height());
	dcMem.SelectObject(&btmp);
	SetStretchBltMode(dcMem.m_hDC, COLORONCOLOR);

	// Scaling & Drawing OCT Projection en face map
	float* OCT_projection = m_Projection32f.raw_ptr();
	IppiSize roi_proj = { m_nAlines4, m_nFrame };
	ippiScale_32f8u_C1R(OCT_projection, roi_proj.width * sizeof(float),
		m_pBitmapProjection->GetPtr(), roi_proj.width * sizeof(uint8_t), roi_proj, (float)m_EditResMinDb, (float)m_EditResMaxDb);

	StretchDIBits(dcMem.m_hDC,
		m_ResProjectionRect.left, m_ResProjectionRect.top, m_ResProjectionRect.Width(), m_ResProjectionRect.Height(),
		0, 0, m_pBitmapProjection->GetLpBmpInfo()->bmiHeader.biWidth, m_pBitmapProjection->GetLpBmpInfo()->bmiHeader.biHeight,
		m_pBitmapProjection->GetPtr(), m_pBitmapProjection->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

	int cur_frame = int(double((m_nFrame - m_SliderCurFrame) * m_ResProjectionRect.Height()) / (double)(m_nFrame));
	dcMem.SelectObject(&m_CircCenterPen);
	dcMem.MoveTo(m_ResProjectionRect.left, m_ResProjectionRect.top + cur_frame); dcMem.LineTo(m_ResProjectionRect.right, m_ResProjectionRect.top + cur_frame);

	// Scaling & Drawing NIRF map
	float* NIRF_map = m_NirfMap32f.raw_ptr();
	IppiSize roi_nirf = { m_nAlines4, m_nFrame };
	int nirf_offset = 0;

	if (m_NirfSignal32f.raw_ptr())
	{
		float* NIRF_signal = m_NirfSignal32f.raw_ptr();

		int sig_len = m_NirfSignal32f.length();
		int map_len = m_NirfMap32f.length();
		int diff, case0;

		memset(m_NirfMap32f.raw_ptr(), 0, sizeof(float) * m_NirfMap32f.length());

		if (sig_len > map_len)
		{
			case0 = 0;
			diff = sig_len - map_len;
			memcpy(&m_NirfMap32f(m_EditNirfOffset, 0), NIRF_signal + diff, sizeof(float) * (map_len - m_EditNirfOffset));
		}
		else if (sig_len == map_len)
		{
			case0 = 1;
			memcpy(&m_NirfMap32f(m_EditNirfOffset, 0), NIRF_signal, sizeof(float) * (map_len - m_EditNirfOffset));
		}
		else
		{
			case0 = 2;
			diff = map_len - sig_len;
			memcpy(&m_NirfMap32f(diff + m_EditNirfOffset, 0), NIRF_signal, sizeof(float) * (sig_len - m_EditNirfOffset));
		}

		printf("[case %d] sig : %d / map : %d\n", case0, sig_len, map_len);
	}

	ippiScale_32f8u_C1R(NIRF_map, roi_nirf.width * sizeof(float),
		m_pBitmapNirfMap->GetPtr() + nirf_offset, roi_nirf.width * sizeof(uint8_t), roi_nirf, (float)_wtof(m_EditResMinNirfMap), (float)_wtof(m_EditResMaxNirfMap));

	StretchDIBits(dcMem.m_hDC,
		m_ResNirfMapRect.left, m_ResNirfMapRect.top, m_ResNirfMapRect.Width(), m_ResNirfMapRect.Height(),
		0, 0, m_pBitmapNirfMap->GetLpBmpInfo()->bmiHeader.biWidth, m_pBitmapNirfMap->GetLpBmpInfo()->bmiHeader.biHeight,
		m_pBitmapNirfMap->GetPtr(), m_pBitmapNirfMap->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

	dcMem.SelectObject(&m_CircCenterPen);
	dcMem.MoveTo(m_ResNirfMapRect.left, m_ResNirfMapRect.top + cur_frame); dcMem.LineTo(m_ResNirfMapRect.right, m_ResNirfMapRect.top + cur_frame);

	// Scaling & Transposing & Drawing OCT image
	ScaleRect(m_OCT_Image32f.at(m_SliderCurFrame), m_OCT_Image8u.raw_ptr());
	TransposeRect(m_OCT_Image8u.raw_ptr(), m_TempBitmapOCT.raw_ptr());
	m_pBitmapOCT->IndToRGB(m_TempBitmapOCT.raw_ptr(), m_ComboResLut);

	if (!m_CheckCircularize)
	{				
		// Draw NIRF Rings
		if (m_NirfSignal32f.raw_ptr())
		{
			int ring_thickness = 60;
			for (int i = 0; i < ring_thickness; i++)
				m_pBitmapOCT->IndToRGB_line(m_pBitmapNirfMap->GetPtr(m_SliderCurFrame, 0), i, LUT_HOT);
		}

		StretchDIBits(dcMem.m_hDC,
			m_ResImageRect.left, m_ResImageRect.top, m_ResImageRect.Width(), m_ResImageRect.Height(),
			0, 0, m_pBitmapOCT->GetLpBmpInfo()->bmiHeader.biWidth, m_pBitmapOCT->GetLpBmpInfo()->bmiHeader.biHeight,
			m_pBitmapOCT->GetPtr(), m_pBitmapOCT->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

		if (m_CheckShowGuideline)
		{
			int circ_center = int(double(m_EditCircCenter * m_ResImageRect.Height()) / (double)(m_nScansFFT / 2));
			int sheath_thickness = circ_center + int(double(m_EditSheathThickness * m_ResImageRect.Height()) / (double)(m_nScansFFT / 2));
			int circ_radius = circ_center + int(double(m_radius * m_ResImageRect.Height()) / (double)(m_nScansFFT / 2));
			dcMem.SelectObject(&m_CircCenterPen);
			dcMem.MoveTo(m_ResImageRect.left, circ_center); dcMem.LineTo(m_ResImageRect.right, circ_center);
			dcMem.SelectObject(&m_SheathThicknessPen);
			dcMem.MoveTo(m_ResImageRect.left, sheath_thickness); dcMem.LineTo(m_ResImageRect.right, sheath_thickness);
			dcMem.SelectObject(&m_CircRadiusPen);
			dcMem.MoveTo(m_ResImageRect.left, circ_radius); dcMem.LineTo(m_ResImageRect.right, circ_radius);
		}

		dcResImage.BitBlt(0, 0, m_ResImageRect.Width(), m_ResImageRect.Height(), &dcMem, m_ResImageRect.left, m_ResImageRect.top, SRCCOPY);
	}
	else
	{
		// Draw FLIM Rings
		int ring_thickness = 60;
		for (int i = 0; i < ring_thickness; i++)
		{
			int circ_offset = m_nScansFFT / 2 - 1 - m_EditCircCenter - m_radius + i;
			m_pBitmapOCT->IndToRGB_line(m_pBitmapNirfMap->GetPtr(m_SliderCurFrame, 0), circ_offset, LUT_HOT);
		}

		// Circularize OCT Image
		IppiSize roi_oct = { m_nScansFFT / 2, m_nAlines };
		IppiSize roi_oct1 = { m_nAlines, m_nScansFFT / 2 };
		for (int i = 0; i < 3; i++)
		{
			np::Array<BYTE, 2> temp_rect(m_pBitmapOCT->GetChannelImg(i), m_nAlines, m_nScansFFT / 2);
			np::Array<BYTE, 2> temp_rect1(m_nScansFFT / 2, m_nAlines);

			ippiTranspose_8u_C1R(temp_rect.raw_ptr(), roi_oct1.width * sizeof(uint8_t), temp_rect1.raw_ptr(), roi_oct1.height * sizeof(uint8_t), roi_oct1);
			ippiMirror_8u_C1IR(temp_rect1.raw_ptr(), roi_oct.width * sizeof(uint8_t), roi_oct, ippAxsVertical);

			circ_OCT(temp_rect1, m_TempBitmapCircOCT, m_EditCircCenter);
			m_pBitmapCircOCT->PutChannelImg(m_TempBitmapCircOCT, i);
		}
				
		StretchDIBits(dcMem.m_hDC,
			m_ResImageRect.left, m_ResImageRect.top, m_ResImageRect.Width(), m_ResImageRect.Width(),
			0, 0, m_pBitmapCircOCT->GetLpBmpInfo()->bmiHeader.biWidth, m_pBitmapCircOCT->GetLpBmpInfo()->bmiHeader.biHeight,
			m_pBitmapCircOCT->GetPtr(), m_pBitmapCircOCT->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);		

		if (m_CheckShowGuideline)
		{
			int sheath_thickness = int(double(m_EditSheathThickness * m_ResImageRect.Width()) / (double)(2 * m_radius));
			dcMem.SelectObject(&m_SheathThicknessPen);
			dcMem.SelectStockObject(NULL_BRUSH);
			dcMem.Ellipse(m_ResImageRect.Width() / 2 - sheath_thickness, m_ResImageRect.Width() / 2 - sheath_thickness,
				m_ResImageRect.Width() / 2 + sheath_thickness, m_ResImageRect.Width() / 2 + sheath_thickness);
		}

		dcResImage.BitBlt(0, 0, m_ResImageRect.Width(), m_ResImageRect.Width(), &dcMem, m_ResImageRect.left, m_ResImageRect.top, SRCCOPY);
	}

	// Drawing Colorbars (for OCT)
	StretchDIBits(dcMem.m_hDC,
		m_CBarRegionRect1.left, m_CBarRegionRect1.top, m_CBarRegionRect1.Width(), m_CBarRegionRect1.Height(),
		0, 0, m_pColorbarProjection->GetLpBmpInfo()->bmiHeader.biWidth, m_pColorbarProjection->GetLpBmpInfo()->bmiHeader.biHeight,
		m_pColorbarProjection->GetPtr(), m_pColorbarProjection->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

	// Drawing Colorbars (for NIRF)
	StretchDIBits(dcMem.m_hDC,
		m_CBarRegionRect2.left, m_CBarRegionRect2.top, m_CBarRegionRect2.Width(), m_CBarRegionRect2.Height(),
		0, 0, m_pColorbarNirfMap->GetLpBmpInfo()->bmiHeader.biWidth, m_pColorbarNirfMap->GetLpBmpInfo()->bmiHeader.biHeight,
		m_pColorbarNirfMap->GetPtr(), m_pColorbarNirfMap->GetLpBmpInfo(), DIB_RGB_COLORS, SRCCOPY);

	// Memory DC transfer	
	dcProjection.BitBlt(0, 0, m_ResProjectionRect.Width(), m_ResProjectionRect.Height(), &dcMem, m_ResProjectionRect.left, m_ResProjectionRect.top, SRCCOPY);
	dcNirfmap.BitBlt(0, 0, m_ResNirfMapRect.Width(), m_ResNirfMapRect.Height(), &dcMem, m_ResNirfMapRect.left, m_ResNirfMapRect.top, SRCCOPY);
	dcCBar1.BitBlt(0, 0, m_CBarRegionRect1.Width(), m_CBarRegionRect1.Height(), &dcMem, m_CBarRegionRect1.left, m_CBarRegionRect1.top, SRCCOPY);
	dcCBar2.BitBlt(0, 0, m_CBarRegionRect2.Width(), m_CBarRegionRect2.Height(), &dcMem, m_CBarRegionRect2.left, m_CBarRegionRect2.top, SRCCOPY);
}


void COCTViewDlg::ScaleRect(const float* pSrc, uint8_t* pDst)
{
	IppiSize roi_oct = { m_nScansFFT / 2, m_nAlines };
	IppiSize roi_oct4 = { m_nScansFFT / 2, m_nAlines4 };

	ippiScale_32f8u_C1R(pSrc, roi_oct.width * sizeof(float),
		pDst, roi_oct.width * sizeof(uint8_t), roi_oct, (float)m_EditResMinDb, (float)m_EditResMaxDb);
	median_filter(pDst);
	if (m_CheckFilipHorizontal) ippiMirror_8u_C1IR(pDst, roi_oct4.width * sizeof(uint8_t), roi_oct4, ippAxsVertical);
}


void COCTViewDlg::TransposeRect(const uint8_t* pSrc, uint8_t* pDst)
{
	IppiSize roi_oct4 = { m_nScansFFT / 2, m_nAlines4 };
	IppiSize roi_oct4t = { m_nAlines4, m_nScansFFT / 2 };

	ippiTranspose_8u_C1R(pSrc, roi_oct4.width * sizeof(uint8_t), pDst, roi_oct4.height * sizeof(uint8_t), roi_oct4);
	ippiMirror_8u_C1IR(pDst, roi_oct4t.width * sizeof(uint8_t), roi_oct4t, ippAxsHorizontal);
}


void COCTViewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME);

	if (pScrollBar == (CScrollBar*)pSlider)
	{
		UpdateData(TRUE);

		CString str;
		str.Format(_T("Current Frame : %d / %d"), m_SliderCurFrame + 1, m_nFrame);
		SetDlgItemText(IDC_STATIC_CUR_FRAME, str);
		InvalidateRect(m_VisRegionRectWnd, FALSE);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void COCTViewDlg::OnCbnSelchangeComboResLut()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_pBitmapOCT->SetColormap(m_ComboResLut);
	m_pBitmapCircOCT->SetColormap(m_ComboResLut);
	m_pBitmapProjection->SetColormap(m_ComboResLut);
	m_pColorbarProjection->SetColormap(m_ComboResLut);

	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnBnClickedCheckSimulWriting()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
}


void COCTViewDlg::OnBnClickedButtonImageProcessing()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	if (m_CheckSingleFrame)
		ImageProcessingSingle();
	else if (!m_CheckSimulWriting)
		ImageProcessingExternal();
	else
		ImageProcessingSimulWriting();
}


void COCTViewDlg::OnBnClickedButtonSaveBitmap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	std::thread im_writing([&]() {

		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

		// Create directory to save images //////////////////////////////////////////////////////////////////
		CString rect_path = m_path + (CString)_T("rect_image");
		CString circ_path = m_path + (CString)_T("circ_image");
		CString rect_name, circ_name;

		CreateDirectory(rect_path, NULL);
		CreateDirectory(circ_path, NULL);

		// Image Processing	Buffers Allocation ////////////////////////////////////////////////////////////////			
		std::queue<uint8_t*> rect, circ;

		UpdateData(TRUE);
		for (int i = 0; i < m_BufferSize; i++)
		{
			uint8_t* buffer4 = new uint8_t[m_nScansFFT / 2 * m_nAlines4];
			uint8_t* buffer5 = new uint8_t[4 * m_radius * m_radius];

			memset(buffer4, 0, m_nScansFFT / 2 * m_nAlines4 * sizeof(uint8_t));
			memset(buffer5, 0, 4 * m_radius * m_radius * sizeof(uint8_t));

			rect.push(buffer4);
			circ.push(buffer5);
		}

		// Synchronization Objects //////////////////////////////////////////////////////////////////////////
		Queue<float*> Queue_image; Queue<uint8_t*> Queue_rect, Queue_circ;
		std::mutex mutex_image, mutex_rect, mutex_circ;

		// Circ Image Writing Processing ///////////////////////////////////////////////////////////////////////
		SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Writing images..."));
		std::thread circ_writing_proc([&]() {
			int frame_count = 0;
			while (frame_count < m_nFrame)
			{
				// Get data from synch Queue
				uint8_t* circ_data = Queue_circ.pop();

				// Writing images
				circ_name.Format(_T("\\circ_1pullback_%03d.bmp"), ++frame_count);
				memcpy(m_pBitmapCircOCT->GetPtr(), circ_data, sizeof(uint8_t) * 4 * m_radius * m_radius);
				m_pBitmapCircOCT->WriteImage(circ_path + circ_name);

				// Status update
				m_ProgressRes.SetPos(frame_count);

				// Return circ buffer to original queue
				{
					std::unique_lock<std::mutex> lock(mutex_circ);
					circ.push(circ_data);
				}
			}
		});

		// Circ Image Processing //////////////////////////////////////////////////////////////////////////////
		std::thread circ_proc([&]() {
			int frame_count = 0;
			while (frame_count < m_nFrame)
			{
				// Get data from synch Queue
				uint8_t* circ_data = nullptr;
				{
					std::unique_lock<std::mutex> lock(mutex_circ);
					if (!circ.empty())
					{
						circ_data = circ.front();
						circ.pop();
					}
				}

				if (circ_data)
				{
					// Get data from synch Queue
					uint8_t* rect_data = Queue_rect.pop();

					// Circularizing rect images
					np::Array<BYTE, 2> temp_rect(rect_data, m_nAlines, m_nScansFFT / 2);
					circ_OCT(temp_rect, circ_data, m_EditCircCenter);

					// Return buffer to original queue
					{
						std::unique_lock<std::mutex> lock(mutex_rect);
						rect.push(rect_data);
					}

					// Push to sync Queue
					Queue_circ.push(circ_data);

					frame_count++;
				}
			}
		});

		// Rect Image Processing //////////////////////////////////////////////////////////////////////////////
		std::thread rect_proc([&]() {
			int frame_count = 0;
			while (frame_count < m_nFrame)
			{
				// Get data from synch Queue
				uint8_t* rect_data = nullptr;
				{
					std::unique_lock<std::mutex> lock(mutex_rect);
					if (!rect.empty())
					{
						rect_data = rect.front();
						rect.pop();
					}
				}

				if (rect_data)
				{
					// Image conversion (32f -> 8u)
					ScaleRect(m_OCT_Image32f.at(frame_count++), rect_data);

					// Transpose Image & Writing images
					TransposeRect(rect_data, m_pBitmapOCT->GetPtr());
					rect_name.Format(_T("\\rect_1pullback_%03d.bmp"), frame_count);
					m_pBitmapOCT->WriteImage(rect_path + rect_name);
					
					// Push to sync Queue
					Queue_rect.push(rect_data);
				}
			}
		});

		// Wait for threads end ////////////////////////////////////////////////////////////////////////////			
		rect_proc.join();
		circ_proc.join();
		circ_writing_proc.join();

		// Delete buffer queues /////////////////////////////////////////////////////////////////////////			
		for (int i = 0; i < m_BufferSize; i++)
		{
			uint8_t*  buffer4 = rect.front(); rect.pop(); delete[] buffer4;
			uint8_t*  buffer5 = circ.front(); circ.pop(); delete[] buffer5;
		}

		// Status Update /////////////////////////////////////////////////////////////////////////////
		SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Ready"));
		printf("Writing images is successfully completed!\n");

		std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start;

		printf("elapsed time : %.2f sec\n", elapsed.count());
		
	});

	im_writing.detach();
}


void COCTViewDlg::OnBnClickedButtonSaveProjection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HANDLE hFile = INVALID_HANDLE_VALUE;
	CString filename; filename.Format(_T("\\MaxProjectionMap_%d.bin"), m_nFrame);

	hFile = CreateFile(m_path + filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	DWORD dwWritten;
	WriteFile(hFile, m_Projection32f, sizeof(float) * m_Projection32f.length(), &dwWritten, NULL);
	
	CloseHandle(hFile);
}


void COCTViewDlg::OnEnChangeEditResAlineMaxDb()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	SetDlgItemInt(IDC_STATIC_RES_PROJECTION_MAX, m_EditResMaxDb);
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditResAlineMinDb()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	SetDlgItemInt(IDC_STATIC_RES_PROJECTION_MIN, m_EditResMinDb);
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditResNirfMapMax()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATIC_RES_NIRF_MAP_MAX, m_EditResMaxNirfMap);
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditResNirfMapMin()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATIC_RES_NIRF_MAP_MIN, m_EditResMinNirfMap);
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditResCircCenter()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_nScansFFT > 0)
	{
		if (m_EditCircCenter > (m_nScansFFT / 2 - m_radius))
		{
			m_EditCircCenter = m_nScansFFT / 2 - m_radius - 1;
			UpdateData(FALSE);
		}
		if (m_nFrame != 1)
		{
			m_Projection32f = np::Array<float, 2>(m_nAlines4, m_nFrame);
			GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);
		}
	}

	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditResSheathThickness()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_nFrame != 1)
	{
		m_Projection32f = np::Array<float, 2>(m_nAlines4, m_nFrame);
		GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);
	}
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnEnChangeEditCircRadiusRatio()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int temp_radius = int(_wtof(m_EditCircRadiusRatio) * (double)m_nScansFFT / 2.0);
	temp_radius = (temp_radius >> 2) << 2;

	CString str;
	str.Format(_T("Radius: %d"), temp_radius);
	m_StatusBar.SetText(str, 2, 0);

	//InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnBnClickedCheckCircularize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	GetDlgItem(IDC_TOGGLE_MEASURE_DISTANCE)->EnableWindow(m_CheckCircularize);
	InvalidateRect(m_VisRegionRectWnd, TRUE);
}


void COCTViewDlg::OnBnClickedCheckHorizonFlip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_nFrame != 1)
	{
		m_Projection32f = np::Array<float, 2>(m_nAlines4, m_nFrame);
		GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);
	}
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnBnClickedCheckResGuideline()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::OnBnClickedToggleMeasureDistance()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_ToggleMeasureDistance)
	{
		n_click = 0;
		InvalidateRect(m_VisRegionRectWnd, FALSE);
	}
}


void COCTViewDlg::OnDeltaposSpinNirfOffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMUpDown->iDelta < 0)
	{
		if (m_EditNirfOffset >= m_nAlines)
			return;
		m_EditNirfOffset++;
	}
	else
	{
		if (m_EditNirfOffset <= 0)
			return;
		m_EditNirfOffset--;
	}
	UpdateData(FALSE);
	InvalidateRect(m_VisRegionRectWnd, FALSE);

	*pResult = 0;
}


void COCTViewDlg::OnEnChangeEditNirfOffset()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_EditNirfOffset > m_nAlines)
	{
		m_EditNirfOffset = m_nAlines;
		UpdateData(FALSE);
	}

	InvalidateRect(m_VisRegionRectWnd, FALSE);
}


void COCTViewDlg::ImageProcessingSingle()
{
	std::thread im_proc([&]() {

		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

		CString str = _T("OCT raw data (*.data) | *.data; | All Files (*.*) | *.*||");
		CFileDialog dlg(TRUE, _T("data"), NULL, OFN_HIDEREADONLY, str);

		if (IDOK == dlg.DoModal())
		{
			CString fullpath = dlg.GetPathName();
			CString name = dlg.GetFileName();
			CString title = dlg.GetFileTitle();
			CString path = fullpath.Left(fullpath.GetLength() - name.GetLength()); m_path = path;
			CString bg_path = path + title + (CString)_T(".background");
			CString calib_path = path + title + (CString)_T(".calibration");

			HANDLE hFile = INVALID_HANDLE_VALUE;
			hFile = CreateFile(fullpath, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				printf("Invalid external data!\n");
			else
			{
				// Size Initialization //////////////////////////////////////////////////////////////////////////////
				SetSize(GetDlgItemInt(IDC_EDIT_NSCANS), GetDlgItemInt(IDC_EDIT_NALINES));

				// Set Range (다시 확인 필요) /////////////////////////////////////////////////////////////////////
				DWORD FileSize[2];
				FileSize[0] = GetFileSize(hFile, &FileSize[1]);
				QWORD TotalFileSize = (QWORD)FileSize[0] + ((QWORD)(-1 * FileSize[1])) + (QWORD)FileSize[1];
				m_nFrame = 1;
				int nFrame = int(TotalFileSize / sizeof(uint16_t) / m_nScans / m_nAlines / 2);

				printf("Start external image processing... (Total nFrame: %d)\n", nFrame);
				m_ProgressRes.SetRange(0, m_nFrame);
				m_ProgressRes.SetPos(0);

				// Set Buffers ////////////////////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Setting buffers..."));
				SetBitmaps(m_nFrame);
				SetBuffers(m_nFrame);

				// Set OCT-FLIM Object ///////////////////////////////////////////////////////////////////////////	
				OCTProcess* pOCT = new OCTProcess(this); // new OCT object..
				UpdateData(TRUE);
				pOCT->LoadCalibration(bg_path, calib_path, m_DiscomValue);
				
				// Image Processing	Buffers Allocation ////////////////////////////////////////////////////////////////	
				np::Array<uint16_t, 2> frame(2 * m_nScans, m_nAlines); 
				np::Array<uint16_t, 2> fringe(m_nScans, m_nAlines), foo(m_nScans, m_nAlines);		
				
				// Get external data /////////////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Image processing..."));
				DWORD dwRead;
				LARGE_INTEGER SelPtr; SelPtr.QuadPart = ( nFrame / 10 ) * 2 * m_nScans * m_nAlines * sizeof(uint16_t);
				SetFilePointerEx(hFile, SelPtr, NULL, FILE_BEGIN);
				ReadFile(hFile, frame, sizeof(uint16_t) * 2 * m_nScans * m_nAlines, &dwRead, NULL);
				
				// Data DeInterleaving /////////////////////////////////////////////////////////////////////////////
				ippsCplxToReal_16sc((Ipp16sc *)frame.raw_ptr(), (Ipp16s *)fringe.raw_ptr(), (Ipp16s *)foo.raw_ptr(), m_nSizeFrame);
				
				// OCT Process /////////////////////////////////////////////////////////////////////////////////////
				(*pOCT)(m_OCT_Image32f.at(0), fringe.raw_ptr());

				// Delete OCT Process object /////////////////////////////////////////////////////////////////////
				delete pOCT;

				// Get OCT Projection /////////////////////////////////////////////////////////////////////////////					
				GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);

				// Status Update /////////////////////////////////////////////////////////////////////////////				
				str.Format(_T("Current Frame : %d / %d"), 1, m_nFrame);
				SetDlgItemText(IDC_STATIC_CUR_FRAME, str);

				GetDlgItem(IDC_BUTTON_SAVE_BITMAP)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_SAVE_PROJECTION)->EnableWindow(TRUE);

				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetRange(0, m_nFrame - 1);
				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetPos(0);
				m_SliderCurFrame = 0;
				UpdateData(FALSE);

				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Ready"));
				printf("External image processing is successfully completed!\n");

				InvalidateRect(m_VisRegionRectWnd, FALSE);
			}
			CloseHandle(hFile);

			std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start;

			printf("elapsed time : %.2f sec\n", elapsed.count());
		}
	});

	im_proc.detach();
}


void COCTViewDlg::ImageProcessingExternal()
{	
	std::thread im_proc([&]() {

		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

		CString str = _T("OCT raw data (*.data) | *.data; | All Files (*.*) | *.*||");
		CFileDialog dlg(TRUE, _T("data"), NULL, OFN_HIDEREADONLY, str);

		if (IDOK == dlg.DoModal())
		{
			CString fullpath = dlg.GetPathName();
			CString name = dlg.GetFileName();
			CString title = dlg.GetFileTitle();
			CString path = fullpath.Left(fullpath.GetLength() - name.GetLength()); m_path = path;
			CString bg_path = path + title + (CString)_T(".background");
			CString calib_path = path + title + (CString)_T(".calibration");
			CString nirf_path = path + (CString)_T("NIRF.txt");

			HANDLE hFile = INVALID_HANDLE_VALUE;
			hFile = CreateFile(fullpath, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				printf("Invalid external data!\n");
			else
			{
				// Size Initialization //////////////////////////////////////////////////////////////////////////////
				SetSize(GetDlgItemInt(IDC_EDIT_NSCANS), GetDlgItemInt(IDC_EDIT_NALINES));

				// Set Range (다시 확인 필요) /////////////////////////////////////////////////////////////////////
				DWORD FileSize[2];
				FileSize[0] = GetFileSize(hFile, &FileSize[1]);
				QWORD TotalFileSize = (QWORD)FileSize[0] + ((QWORD)(-1 * FileSize[1])) + (QWORD)FileSize[1];
				m_nFrame = 1;
				int nFrame = int(TotalFileSize / sizeof(uint16_t) / m_nScans / m_nAlines / 2);

				printf("Start external image processing... (Total nFrame: %d)\n", nFrame);
				m_ProgressRes.SetRange(0, nFrame);
				m_ProgressRes.SetPos(0);

				// Set Buffers ////////////////////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Setting buffers..."));
				SetBitmaps(nFrame);
				SetBuffers(nFrame);

				// Set OCT-FLIM Object ///////////////////////////////////////////////////////////////////////////	
				OCTProcess* pOCT = new OCTProcess(this); // new OCT object..
				UpdateData(TRUE);
				pOCT->LoadCalibration(bg_path, calib_path, m_DiscomValue);

				// Image Processing	Buffers Allocation ////////////////////////////////////////////////////////////////	
				std::queue<uint16_t*> frame, fringe;
				np::Array<uint16_t, 2> foo(m_nScans, m_nAlines);

				for (int i = 0; i < m_BufferSize; i++)
				{
					uint16_t* buffer1 = new uint16_t[2 * m_nScans * m_nAlines];
					uint16_t* buffer2 = new uint16_t[m_nScans * m_nAlines];

					memset(buffer1, 0, 2 * m_nScans * m_nAlines * sizeof(uint16_t));
					memset(buffer2, 0, m_nScans * m_nAlines * sizeof(uint16_t));

					frame.push(buffer1);
					fringe.push(buffer2);
				}
				
				// Synchronization Objects //////////////////////////////////////////////////////////////////////////
				Queue<uint16_t*> Queue_frame, Queue_fringe;
				std::mutex mutex_frame, mutex_fringe;
				
				// OCT Process /////////////////////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Image processing..."));
				std::thread oct_proc([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint16_t* fringe_data = Queue_fringe.pop();

						// OCT Process
						(*pOCT)(m_OCT_Image32f.at(frame_count++), fringe_data);
							
						// Return fringe buffer to original queue
						{
							std::unique_lock<std::mutex> lock(mutex_fringe);
							fringe.push(fringe_data);
						}

						// Status Update
						m_ProgressRes.SetPos(frame_count);
					}					
				});
							
				// Data DeInterleaving /////////////////////////////////////////////////////////////////////////////				
				std::thread deinterleave([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint16_t* fringe_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_fringe);							
							if (!fringe.empty())
							{
								fringe_data = fringe.front();
								fringe.pop();
							}
						}
						
						if (fringe_data)
						{
							// Get data from synch Queue
							uint16_t* frame_data = Queue_frame.pop();

							// Data deinterleaving
							ippsCplxToReal_16sc((Ipp16sc *)frame_data, (Ipp16s *)fringe_data, (Ipp16s *)foo.raw_ptr(), m_nSizeFrame);

							// Return frame buffer to original queue
							{
								std::unique_lock<std::mutex> lock(mutex_frame);
								frame.push(frame_data);
							}

							// Push to sync Queue
							Queue_fringe.push(fringe_data);

							frame_count++;
						}
					}
				});

				// Get external data /////////////////////////////////////////////////////////////////////////////
				DWORD dwRead;
				std::thread load_data([&]() {
					int frame_count = 0;
					while(frame_count < nFrame)
					{
						// Get a frame buffer from raw data queue
						uint16_t* frame_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_frame);							
							if (!frame.empty())
							{
								frame_data = frame.front();
								frame.pop();									
							}							
						}
						
						if (frame_data)
						{
							// Read data from the external data 
							ReadFile(hFile, frame_data, sizeof(uint16_t) * 2 * m_nScans * m_nAlines, &dwRead, NULL);

							// Push to sync Queue
							Queue_frame.push(frame_data);

							frame_count++;
						}
					}
				});		

				// Wait for threads end ////////////////////////////////////////////////////////////////////////////
				load_data.join();
				deinterleave.join();
				oct_proc.join();

				// Delete OCT Process object & fringe queue //////////////////////////////////////////////////////////
				delete pOCT;
				for (int i = 0; i < m_BufferSize; i++)
				{
					uint16_t* buffer1 = frame. front(); frame. pop(); delete[] buffer1;
					uint16_t* buffer2 = fringe.front(); fringe.pop(); delete[] buffer2;
				}

				// Get OCT Projection /////////////////////////////////////////////////////////////////////////////					
				GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);

				// Get NIRF Map //////////////////////////////////////////////////////////////////////////////
				FILE* pFile = nullptr;
				fopen_s(&pFile, (CStringA)nirf_path, "rt");

				if (pFile != nullptr)
				{
					char temp[256], temp1[256], temp2[256];

					int nLines = 0;
					while (!feof(pFile))
					{
						fgets(temp, sizeof(temp), pFile);
						nLines++;
					}
					fseek(pFile, 0, SEEK_SET);
					m_NirfSignal32f = np::Array<float>((nLines - 1) / 2);

					for (int i = 0; i < (nLines - 1) / 2; i++)
					{
						fgets(temp1, sizeof(temp1), pFile);
						fgets(temp2, sizeof(temp2), pFile);
						m_NirfSignal32f(i) = ((float)atof(temp1) + (float)atof(temp2)) / 2.0f;
					}

					fclose(pFile);
				}
				else
				{
					m_NirfSignal32f = np::Array<float>();
				}

				// Status Update /////////////////////////////////////////////////////////////////////////////
				m_nFrame = nFrame;				

				str.Format(_T("Current Frame : %d / %d"), 1, m_nFrame);
				SetDlgItemText(IDC_STATIC_CUR_FRAME, str);

				GetDlgItem(IDC_BUTTON_SAVE_BITMAP)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_SAVE_PROJECTION)->EnableWindow(TRUE);

				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetRange(0, m_nFrame - 1);
				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetPos(0);
				m_SliderCurFrame = 0;
				UpdateData(FALSE);
					
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Ready"));
				printf("External image processing is successfully completed!\n");

				InvalidateRect(m_VisRegionRectWnd, FALSE);				
			}
			CloseHandle(hFile);
			
			std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start;

			printf("elapsed time : %.2f sec\n", elapsed.count());
		}
	});

	im_proc.detach();
}
		

void COCTViewDlg::ImageProcessingSimulWriting()
{	
	std::thread im_proc([&]() {

		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

		CString str = _T("OCT raw data (*.data) | *.data; | All Files (*.*) | *.*||");
		CFileDialog dlg(TRUE, _T("data"), NULL, OFN_HIDEREADONLY, str);

		if (IDOK == dlg.DoModal())
		{
			CString fullpath = dlg.GetPathName();
			CString name = dlg.GetFileName();
			CString title = dlg.GetFileTitle();
			CString path = fullpath.Left(fullpath.GetLength() - name.GetLength()); m_path = path;
			CString bg_path = path + title + (CString)_T(".background");
			CString calib_path = path + title + (CString)_T(".calibration");
			CString rect_path = path + (CString)_T("rect_image");
			CString circ_path = path + (CString)_T("circ_image");
			CString rect_name, circ_name;

			HANDLE hFile = INVALID_HANDLE_VALUE;
			hFile = CreateFile(fullpath, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				printf("Invalid external data!\n");
			else
			{
				// Initialization ////////////////////////////////////////////////////////////////////////////////////////	
				SetSize(GetDlgItemInt(IDC_EDIT_NSCANS), GetDlgItemInt(IDC_EDIT_NALINES));

				// Set Range (다시 확인 필요) /////////////////////////////////////////////////////////////////////
				DWORD FileSize[2];
				FileSize[0] = GetFileSize(hFile, &FileSize[1]);
				QWORD TotalFileSize = (QWORD)FileSize[0] + ((QWORD)(-1 * FileSize[1])) + (QWORD)FileSize[1];
				m_nFrame = 1;
				int nFrame = int(TotalFileSize / sizeof(uint16_t) / m_nScans / m_nAlines / 2);

				printf("Start external image processing... (Total nFrame: %d)\n", nFrame);
				m_ProgressRes.SetRange(0, nFrame);
				m_ProgressRes.SetPos(0);

				// Set Buffers ////////////////////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Setting buffers..."));
				SetBitmaps(nFrame);
				SetBuffers(nFrame);

				// Set OCT-FLIM Object ///////////////////////////////////////////////////////////////////////////	
				OCTProcess* pOCT = new OCTProcess(this); // new OCT object..
				UpdateData(TRUE);
				pOCT->LoadCalibration(bg_path, calib_path, m_DiscomValue);

				// Image Processing	Buffers Allocation ////////////////////////////////////////////////////////////////	
				std::queue<uint16_t*> frame, fringe;
				std::queue<float*> image;
				std::queue<uint8_t*> rect, circ;
				np::Array<uint16_t, 2> foo(m_nScans, m_nAlines);

				for (int i = 0; i < m_BufferSize; i++)
				{
					uint16_t* buffer1 = new uint16_t[2 * m_nScans * m_nAlines];
					uint16_t* buffer2 = new uint16_t[m_nScans * m_nAlines];
					float* buffer3 = new float[m_nScansFFT / 2 * m_nAlines];
					uint8_t* buffer4 = new uint8_t[m_nScansFFT / 2 * m_nAlines4];
					uint8_t* buffer5 = new uint8_t[4 * m_radius * m_radius];

					memset(buffer1, 0, 2 * m_nScans * m_nAlines * sizeof(uint16_t));
					memset(buffer2, 0, m_nScans * m_nAlines * sizeof(uint16_t));
					memset(buffer3, 0, m_nScansFFT / 2 * m_nAlines * sizeof(float));
					memset(buffer4, 0, m_nScansFFT / 2 * m_nAlines4 * sizeof(uint8_t));
					memset(buffer5, 0, 4 * m_radius * m_radius * sizeof(uint8_t));

					frame.push(buffer1);
					fringe.push(buffer2);
					image.push(buffer3);
					rect.push(buffer4);
					circ.push(buffer5);
				}

				// Create directory to save images //////////////////////////////////////////////////////////////////
				CreateDirectory(rect_path, NULL);
				CreateDirectory(circ_path, NULL);

				// Synchronization Objects //////////////////////////////////////////////////////////////////////////
				Queue<uint16_t*> Queue_frame, Queue_fringe;
				Queue<float*> Queue_image; Queue<uint8_t*> Queue_rect, Queue_circ;
				std::mutex mutex_frame, mutex_fringe, mutex_image, mutex_rect, mutex_circ;

				// Circ Image Writing Processing ///////////////////////////////////////////////////////////////////////
				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Image processing..."));
				std::thread circ_writing_proc([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint8_t* circ_data = Queue_circ.pop();

						// Writing images
						circ_name.Format(_T("\\circ_1pullback_%03d.bmp"), frame_count + 1);
						memcpy(m_pBitmapCircOCT->GetPtr(), circ_data, sizeof(uint8_t) * 4 * m_radius * m_radius);
						m_pBitmapCircOCT->WriteImage(circ_path + circ_name);

						// Status update
						m_ProgressRes.SetPos(frame_count++ + 1);

						// Return circ buffer to original queue
						{
							std::unique_lock<std::mutex> lock(mutex_circ);
							circ.push(circ_data);
						}
					}
				});

				// Circ Image Processing //////////////////////////////////////////////////////////////////////////////
				std::thread circ_proc([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint8_t* circ_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_circ);
							if (!circ.empty())
							{
								circ_data = circ.front();
								circ.pop();
							}
						}

						if (circ_data)
						{
							// Get data from synch Queue
							uint8_t* rect_data = Queue_rect.pop();

							// Circularizing rect images
							np::Array<BYTE, 2> temp_rect(rect_data, m_nAlines, m_nScansFFT / 2);
							circ_OCT(temp_rect, circ_data, m_EditCircCenter);

							// Return buffer to original queue
							{
								std::unique_lock<std::mutex> lock(mutex_rect);
								rect.push(rect_data);
							}

							// Push to sync Queue
							Queue_circ.push(circ_data);

							frame_count++;
						}
					}
				});

				// Rect Image Processing //////////////////////////////////////////////////////////////////////////////
				std::thread rect_proc([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint8_t* rect_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_rect);
							if (!rect.empty())
							{
								rect_data = rect.front();
								rect.pop();
							}
						}

						if (rect_data)
						{
							// Get data from synch Queue
							float* image_data = Queue_image.pop();

							// Image conversion (32f -> 8u)
							ScaleRect(image_data, rect_data);

							// Transpose Image & Writing images
							TransposeRect(rect_data, m_pBitmapOCT->GetPtr());
							rect_name.Format(_T("\\rect_1pullback_%03d.bmp"), frame_count++ + 1);
							m_pBitmapOCT->WriteImage(rect_path + rect_name);

							// Return image buffer to original queue
							{
								std::unique_lock<std::mutex> lock(mutex_image);
								image.push(image_data);
							}

							// Push to sync Queue
							Queue_rect.push(rect_data);
						}
					}
				});

				// OCT Process /////////////////////////////////////////////////////////////////////////////////////
				std::thread oct_proc([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue				
						float* image_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_image);
							if (!image.empty())
							{
								image_data = image.front();
								image.pop();
							}
						}

						if (image_data)
						{
							// Get data from synch Queue
							uint16_t* fringe_data = Queue_fringe.pop();

							// OCT Process
							(*pOCT)(image_data, fringe_data);
							memcpy(m_OCT_Image32f.at(frame_count++), image_data, sizeof(float) * m_nSizeImageOCT);

							// Return fringe buffer to original queue
							{
								std::unique_lock<std::mutex> lock(mutex_fringe);
								fringe.push(fringe_data);
							}

							// Push to sync Queue
							Queue_image.push(image_data);
						}
					}
				});

				// Data DeInterleaving /////////////////////////////////////////////////////////////////////////////				
				std::thread deinterleave([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get data from synch Queue
						uint16_t* fringe_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_fringe);
							if (!fringe.empty())
							{
								fringe_data = fringe.front();
								fringe.pop();
							}
						}

						if (fringe_data)
						{
							// Get data from synch Queue
							uint16_t* frame_data = Queue_frame.pop();

							// Data deinterleaving
							ippsCplxToReal_16sc((Ipp16sc *)frame_data, (Ipp16s *)fringe_data, (Ipp16s *)foo.raw_ptr(), m_nSizeFrame);

							// Return frame buffer to original queue
							{
								std::unique_lock<std::mutex> lock(mutex_frame);
								frame.push(frame_data);
							}

							// Push to sync Queue
							Queue_fringe.push(fringe_data);

							frame_count++;
						}
					}
				});

				// Get external data /////////////////////////////////////////////////////////////////////////////
				DWORD dwRead;
				std::thread load_data([&]() {
					int frame_count = 0;
					while (frame_count < nFrame)
					{
						// Get a frame buffer from raw data queue
						uint16_t* frame_data = nullptr;
						{
							std::unique_lock<std::mutex> lock(mutex_frame);
							if (!frame.empty())
							{
								frame_data = frame.front();
								frame.pop();
							}
						}

						if (frame_data)
						{
							// Read data from the external data 
							ReadFile(hFile, frame_data, sizeof(uint16_t) * 2 * m_nScans * m_nAlines, &dwRead, NULL);

							// Push to sync Queue
							Queue_frame.push(frame_data);

							frame_count++;
						}
					}
				});

				// Wait for threads end ////////////////////////////////////////////////////////////////////////////
				load_data.join();
				deinterleave.join();
				oct_proc.join();
				rect_proc.join();
				circ_proc.join();
				circ_writing_proc.join();

				// Delete OCT Process object & fringe queue //////////////////////////////////////////////////////////
				delete pOCT;
				for (int i = 0; i < m_BufferSize; i++)
				{
					uint16_t* buffer1 = frame.front(); frame.pop(); delete[] buffer1;
					uint16_t* buffer2 = fringe.front(); fringe.pop(); delete[] buffer2;
					float*    buffer3 = image.front(); image.pop(); delete[] buffer3;
					uint8_t*  buffer4 = rect.front(); rect.pop(); delete[] buffer4;
					uint8_t*  buffer5 = circ.front(); circ.pop(); delete[] buffer5;
				}

				// Get OCT Projection /////////////////////////////////////////////////////////////////////////////					
				GetProjection(m_OCT_Image32f, m_Projection32f, m_EditCircCenter);

				// Status Update /////////////////////////////////////////////////////////////////////////////
				m_nFrame = nFrame;

				str.Format(_T("Current Frame : %d / %d"), 1, m_nFrame);
				SetDlgItemText(IDC_STATIC_CUR_FRAME, str);

				GetDlgItem(IDC_BUTTON_SAVE_BITMAP)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_SAVE_PROJECTION)->EnableWindow(TRUE);

				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetRange(0, m_nFrame - 1);
				((CSliderCtrl*)GetDlgItem(IDC_SLIDER_CUR_FRAME))->SetPos(0);
				m_SliderCurFrame = 0;
				UpdateData(FALSE);

				SetDlgItemText(IDC_STATIC_PROG_MSG, _T("Ready"));
				printf("External image processing is successfully completed!\n");

				InvalidateRect(m_VisRegionRectWnd, FALSE);

			}
			CloseHandle(hFile);

			std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start;

			printf("elapsed time : %.2f sec\n", elapsed.count());
		}
	});

	im_proc.detach();
}


//#include <chrono>
void COCTViewDlg::GetProjection(const std::vector<np::Array<float, 2>>& OCT_vector, np::Array<float, 2>& Proj, int offset)
{
	//std::chrono::system_clock::time_point start1 = std::chrono::system_clock::now();
	UpdateData(TRUE);

	int len = m_radius - m_EditSheathThickness;
	for (int i = 0; i < (int)OCT_vector.size(); i++)
	{
		float max_val;
		Ipp32f* aline = ippsMalloc_32f(m_nScansFFT / 2);
		for (int j = 0; j < m_nAlines; j++)
		{
			memcpy(aline, &OCT_vector.at(i)(0, j), sizeof(Ipp32f) * m_nScansFFT / 2);
			if (m_CheckFilipHorizontal)
				ippsFlip_32f_I(aline, m_nScansFFT / 2);

			ippsMax_32f(&aline[offset + m_EditSheathThickness], len, &max_val);
			Proj(j, i) = max_val;
		}
	}
	//std::chrono::system_clock::time_point end1 = std::chrono::system_clock::now();
	
	//std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
	//printf("elapsed time : %d msec\n", (int)elapsed.count());
}


void COCTViewDlg::SetIniFile()
{
	CString strSection, strKey, strValue;

	strSection = _T("configuration");

	strKey = _T("nScans");
	strValue.Format(_T("%d"), m_nScans);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("nAlines");
	strValue.Format(_T("%d"), m_nAlines);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	UpdateData(TRUE);

	strKey = _T("WriteMode");
	strValue.Format(_T("%d"), m_CheckSimulWriting);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("BufferSize");
	strValue.Format(_T("%d"), m_BufferSize);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("DiscomValue");
	strValue.Format(_T("%d"), m_DiscomValue);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("PixelSize");
	strValue.Format(_T("%s"), m_PixelSize);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("Max_dB");
	strValue.Format(_T("%d"), m_EditResMaxDb);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("Min_dB");
	strValue.Format(_T("%d"), m_EditResMinDb);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("Max_NIRF");
	strValue.Format(_T("%s"), m_EditResMaxNirfMap);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("Min_NIRF");
	strValue.Format(_T("%s"), m_EditResMinNirfMap);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("CircCenter");
	strValue.Format(_T("%d"), m_EditCircCenter);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("SheathThickness");
	strValue.Format(_T("%d"), m_EditSheathThickness);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("CircRadiusRatio");
	strValue.Format(_T("%s"), m_EditCircRadiusRatio);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);

	strKey = _T("LookUpTable");
	strValue.Format(_T("%d"), m_ComboResLut);
	WritePrivateProfileString(strSection, strKey, strValue, m_strIniPath);
}


void COCTViewDlg::GetIniFile()
{
	TCHAR szBuf[MAX_PATH] = { 0, };
	CString strSection, strKey, strValue;

	strSection = _T("configuration");

	strKey = _T("nScans");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_NSCANS, _wtoi(strValue));

	strKey = _T("nAlines");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_NALINES, _wtoi(strValue));

	strKey = _T("WriteMode");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	((CButton *)GetDlgItem(IDC_CHECK_SIMUL_WRITING))->SetCheck(_wtoi(strValue) == 1);

	strKey = _T("BufferSize");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_BUFFER_SIZE, _wtoi(strValue));

	strKey = _T("DiscomValue");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_DISCOM_VALUE, _wtoi(strValue));

	strKey = _T("PixelSize");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	CString str; str.Format(_T("%.3f"), _wtof(strValue));
	SetDlgItemText(IDC_EDIT_PIXEL_SIZE, str);

	strKey = _T("Max_dB");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_RES_ALINE_MAX_DB, _wtoi(strValue));

	strKey = _T("Min_dB");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_RES_ALINE_MIN_DB, _wtoi(strValue));

	strKey = _T("Max_NIRF");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	str.Format(_T("%.1f"), _wtof(strValue));
	SetDlgItemText(IDC_EDIT_RES_NIRF_MAP_MAX, str);

	strKey = _T("Min_NIRF");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	str.Format(_T("%.1f"), _wtof(strValue));
	SetDlgItemText(IDC_EDIT_RES_NIRF_MAP_MIN, str);
	
	strKey = _T("CircCenter");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_RES_CIRC_CENTER, _wtoi(strValue));

	strKey = _T("SheathThickness");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemInt(IDC_EDIT_RES_SHEATH_THICKNESS, _wtoi(strValue));

	strKey = _T("CircRadiusRatio");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	SetDlgItemText(IDC_EDIT_CIRC_RADIUS_RATIO, strValue);

	strKey = _T("LookUpTable");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_PATH, m_strIniPath);
	strValue.Format(_T("%s"), szBuf);
	((CComboBox *)GetDlgItem(IDC_COMBO_RES_LUT))->SetCurSel(_wtoi(strValue));	

	UpdateData(TRUE);
}