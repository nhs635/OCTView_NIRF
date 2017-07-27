// Calibration.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "microOCT.h"
#include "Calibration.h"
#include "afxdialogex.h"


// CCalibration 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCalibration, CDialogEx)

CCalibration::CCalibration(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CALIBRATION_DIALOG, pParent)
	, m_dBG(NULL)
	, m_dSig1(NULL)
	, m_dSig2(NULL)
	, m_dPhase(NULL)
	, m_dDisp(NULL)
	, m_dCalibRes(NULL)
	, m_bGotBG(FALSE)
	, m_bGotSig1(FALSE)
	, m_bGotSig2(FALSE)
	, m_Edit_dB_Drop(30)
{

}

CCalibration::~CCalibration()
{
}

void CCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_BG, m_PictureBG);
	DDX_Control(pDX, IDC_STATIC_SIG1, m_PictureSig1);
	DDX_Control(pDX, IDC_STATIC_SIG2, m_PictureSig2);
	DDX_Control(pDX, IDC_STATIC_PHASE, m_PicturePhase);
	DDX_Control(pDX, IDC_STATIC_DISP, m_PictureDisp);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_PictureCalibRes);
	DDX_Text(pDX, IDC_EDIT_DB_DROP, m_Edit_dB_Drop);
}


BEGIN_MESSAGE_MAP(CCalibration, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BG, &CCalibration::OnBnClickedButtonBg)
	ON_BN_CLICKED(IDC_BUTTON_SIG1, &CCalibration::OnBnClickedButtonSig1)
	ON_BN_CLICKED(IDC_BUTTON_SIG2, &CCalibration::OnBnClickedButtonSig2)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATE, &CCalibration::OnBnClickedButtonCalibrate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CALIB, &CCalibration::OnBnClickedButtonSaveCalib)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CCalibration::OnBnClickedButtonExit)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CCalibration 메시지 처리기입니다.
BOOL CCalibration::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
	CMenu *pMenu = this->GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

	m_PictureBG.GetClientRect(&m_RectBG); 
	m_PictureSig1.GetClientRect(&m_RectSig1); 
	m_PictureSig2.GetClientRect(&m_RectSig2);
	m_PicturePhase.GetClientRect(&m_RectPhase);
	m_PictureDisp.GetClientRect(&m_RectDisp);
	m_PictureCalibRes.GetClientRect(&m_RectCalibRes);

	m_RectBGWnd = m_RectBG;
	m_RectSig1Wnd = m_RectSig1;
	m_RectSig2Wnd = m_RectSig2;
	m_RectPhaseWnd = m_RectPhase;
	m_RectDispWnd = m_RectDisp;
	m_RectCalibResWnd = m_RectCalibRes;

	CWnd* pWnd;
	pWnd = this->GetDlgItem(IDC_STATIC_BG);
	pWnd->GetWindowRect(&m_RectBGWnd);
	this->ScreenToClient(&m_RectBGWnd);
	pWnd = this->GetDlgItem(IDC_STATIC_SIG1);
	pWnd->GetWindowRect(&m_RectSig1Wnd);
	this->ScreenToClient(&m_RectSig1Wnd);
	pWnd = this->GetDlgItem(IDC_STATIC_SIG2);
	pWnd->GetWindowRect(&m_RectSig2Wnd);
	this->ScreenToClient(&m_RectSig2Wnd);
	pWnd = this->GetDlgItem(IDC_STATIC_PHASE);
	pWnd->GetWindowRect(&m_RectPhaseWnd);
	this->ScreenToClient(&m_RectPhaseWnd);
	pWnd = this->GetDlgItem(IDC_STATIC_DISP);
	pWnd->GetWindowRect(&m_RectDispWnd);
	this->ScreenToClient(&m_RectDispWnd);
	pWnd = this->GetDlgItem(IDC_STATIC_RESULT);
	pWnd->GetWindowRect(&m_RectCalibResWnd);
	this->ScreenToClient(&m_RectCalibResWnd);

	m_GridPen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	m_DataPen[0].CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_DataPen[1].CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_DataPen[2].CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	m_dwWidth = m_pMainDlg->m_dwScopeDataNum;
	m_dwNfft = m_pMainDlg->m_dwScopeDataNumFFT;
	DWORD dwLeft = m_pMainDlg->m_RoiRect.left;

	SetDlgItemInt(IDC_STATIC_BG_X0, dwLeft);
	SetDlgItemInt(IDC_STATIC_BG_X1, dwLeft + m_dwWidth - 1);
	SetDlgItemInt(IDC_STATIC_SIG1_X0, dwLeft);
	SetDlgItemInt(IDC_STATIC_SIG1_X1, dwLeft + m_dwWidth - 1);
	SetDlgItemInt(IDC_STATIC_SIG2_X0, dwLeft);
	SetDlgItemInt(IDC_STATIC_SIG2_X1, dwLeft + m_dwWidth - 1);
	SetDlgItemInt(IDC_STATIC_PHASE_X0, dwLeft);
	SetDlgItemInt(IDC_STATIC_PHASE_X1, dwLeft + m_dwWidth - 1);
	SetDlgItemInt(IDC_STATIC_DISP_X0, dwLeft);
	SetDlgItemInt(IDC_STATIC_DISP_X1, dwLeft + m_dwWidth - 1);
	SetDlgItemInt(IDC_STATIC_CALIB_RES_X0, 0);

	m_dBG = new double[m_dwWidth]; memset(m_dBG, 0, sizeof(double) * m_dwWidth);
	m_dSig1 = new double[m_dwWidth]; memset(m_dSig1, 0, sizeof(double) * m_dwWidth);
	m_dSig2 = new double[m_dwWidth]; memset(m_dSig2, 0, sizeof(double) * m_dwWidth);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CCalibration::OnBnClickedButtonBg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (DWORD i = 0; i < m_dwWidth; i++)
		m_dBG[i] = m_pMainDlg->m_pScopeData[i];

	m_bGotBG = TRUE;
	
	if (m_bGotBG && m_bGotSig1 && m_bGotSig2)
		GetDlgItem(IDC_BUTTON_CALIBRATE)->EnableWindow(TRUE);

	InvalidateRect(m_RectBGWnd, FALSE); // Rect(rect, FALSE);
}


void CCalibration::OnBnClickedButtonSig1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (DWORD i = 0; i < m_dwWidth; i++)
		m_dSig1[i] = m_pMainDlg->m_pScopeData[i];

	m_bGotSig1 = TRUE;

	if (m_bGotBG && m_bGotSig1 && m_bGotSig2)
		GetDlgItem(IDC_BUTTON_CALIBRATE)->EnableWindow(TRUE);

	InvalidateRect(m_RectSig1Wnd, FALSE);
}


void CCalibration::OnBnClickedButtonSig2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (DWORD i = 0; i < m_dwWidth; i++)
		m_dSig2[i] = m_pMainDlg->m_pScopeData[i];

	m_bGotSig2 = TRUE;

	if (m_bGotBG && m_bGotSig1 && m_bGotSig2)
		GetDlgItem(IDC_BUTTON_CALIBRATE)->EnableWindow(TRUE);

	InvalidateRect(m_RectSig2Wnd, FALSE);
}


void CCalibration::OnBnClickedButtonCalibrate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double* dSig1 = new double[m_dwWidth];
	double* dSig2 = new double[m_dwWidth];
	double* dSig1_1 = new double[m_dwWidth];
	double* dSig2_1 = new double[m_dwWidth];

	double* WvRange = new double[m_dwWidth];
	double* kRange = new double[m_dwWidth];
	double* kRangeLin = new double[m_dwWidth];

	DWORD dwNfft = _getNear2Power(m_dwWidth);
	double* dSig1Re = new double[dwNfft];
	double* dSig1Im = new double[dwNfft];
	double* dSig2Re = new double[dwNfft];
	double* dSig2Im = new double[dwNfft];
	double* dSig1ReTemp = new double[dwNfft];
	double* dSig2ReTemp = new double[dwNfft];
	
	double* dPhase1 = new double[m_dwWidth];
	double* dPhase2 = new double[m_dwWidth];
	double* dPhaseD = new double[m_dwWidth];
	double* dPhaseLin = new double[m_dwWidth];
	double* dIndex = new double[m_dwWidth];
	double* dIndexLin = new double[m_dwWidth];
	double* dPhaseReal = new double[m_dwWidth];
	double* dPhaseIdeal = new double[m_dwWidth];

	double* dDisp = new double[m_dwWidth];
	double* dDispFit = new double[m_dwWidth];
	double* dDispComRe = new double[m_dwWidth];
	double* dDispComIm = new double[m_dwWidth];

	double* dCalibIndex1 = new double[m_dwWidth];
	double* dCalibIndex2 = new double[m_dwWidth];
	DWORD* dwCalibInd = new DWORD[m_dwWidth];
	double* dCalibWeight = new double[m_dwWidth];

	double* dTempRe = new double[m_dwNfft];
	double* dTempRe1 = new double[m_dwNfft];
	double* dTempIm = new double[m_dwNfft];
	
	m_dPhase = new double[m_dwWidth * 2];
	m_dDisp = new double[m_dwWidth * 2];
	m_dCalibRes = new double[m_dwNfft];

	do
	{
		// BG subtraction
		_arraySubtraction(m_dSig1, m_dBG, m_dwWidth, dSig1);
		_arraySubtraction(m_dSig2, m_dBG, m_dwWidth, dSig2);

		memset(dTempRe, 0, sizeof(double) * m_dwNfft);
		memset(dTempRe1, 0, sizeof(double) * m_dwNfft);
		memset(dTempIm, 0, sizeof(double) * m_dwNfft);
		memcpy(dTempRe, dSig1, sizeof(double) * m_dwNfft);

		// Find linear k-domain
		memcpy(WvRange, m_pMainDlg->m_pSpectralRange, sizeof(double) * m_dwWidth);
		_reciprocal(WvRange, kRange, m_dwWidth, 2 * PI);
		_lineSpace(kRange[0], kRange[m_dwWidth - 1], m_dwWidth, kRangeLin);

		// k-domain linearization
		_interp1d(kRange, dSig1, kRangeLin, m_dwWidth, m_dwWidth, dSig1_1);
		_interp1d(kRange, dSig2, kRangeLin, m_dwWidth, m_dwWidth, dSig2_1);

		// FFT
		memset(dSig1Re, 0, sizeof(double) * dwNfft);
		memset(dSig1Im, 0, sizeof(double) * dwNfft);
		memset(dSig2Re, 0, sizeof(double) * dwNfft);
		memset(dSig2Im, 0, sizeof(double) * dwNfft);

		memcpy(dSig1Re, dSig1_1, sizeof(double) * m_dwWidth);
		memcpy(dSig2Re, dSig2_1, sizeof(double) * m_dwWidth);

		_FFT1d(dSig1Re, dSig1Im, dwNfft, 1);
		_FFT1d(dSig2Re, dSig2Im, dwNfft, 1);

		memcpy(dSig1ReTemp, dSig1Re, sizeof(double) * dwNfft);
		memcpy(dSig2ReTemp, dSig2Re, sizeof(double) * dwNfft);

		_abs2(dSig1Re, dSig1Im, dwNfft);
		_abs2(dSig2Re, dSig2Im, dwNfft);

		_dBscale(dSig1Re, dwNfft, 0);
		_dBscale(dSig2Re, dwNfft, 0);

		memcpy(m_dCalibRes, dSig1Re, sizeof(double) * m_dwNfft / 2);

		// Leave peak only
		memset(dSig1Re + dwNfft / 2, 0, sizeof(double) * dwNfft / 2);
		memset(dSig2Re + dwNfft / 2, 0, sizeof(double) * dwNfft / 2);

		DWORD dwMaxPos1 = _findMaxPos(dSig1Re, dwNfft / 2, dwNfft / 50);
		DWORD dwMaxPos2 = _findMaxPos(dSig2Re, dwNfft / 2, dwNfft / 50);
		DWORD dwLeftPos1 = 0, dwLeftPos2 = 0;
		DWORD dwRightPos1 = 0, dwRightPos2 = 0;

		DWORD dB_drop = GetDlgItemInt(IDC_EDIT_DB_DROP);
		_findWidth(dSig1Re, dwMaxPos1, dB_drop, dwNfft / 2, &dwLeftPos1, &dwRightPos1);
		_findWidth(dSig2Re, dwMaxPos2, dB_drop, dwNfft / 2, &dwLeftPos2, &dwRightPos2);
		
	//	if (dwLeftPos1 == 0 || dwRightPos1 == 0 || dwLeftPos2 == 0 || dwRightPos2 == 0)
	//	{
		//	AfxMessageBox(_T("Calibration error!"), MB_ICONERROR);
		//	break;
	//	}		

		memcpy(dSig1Re, dSig1ReTemp, sizeof(double) * dwNfft);
		memcpy(dSig2Re, dSig2ReTemp, sizeof(double) * dwNfft);

		memset(dSig1Re, 0, sizeof(double) * dwLeftPos1);
		memset(dSig2Re, 0, sizeof(double) * dwLeftPos2);
		memset(dSig1Re + dwRightPos1 + 1, 0, sizeof(double) * (dwNfft - dwRightPos1 - 1));
		memset(dSig2Re + dwRightPos2 + 1, 0, sizeof(double) * (dwNfft - dwRightPos2 - 1));
		memset(dSig1Im, 0, sizeof(double) * dwLeftPos1);
		memset(dSig2Im, 0, sizeof(double) * dwLeftPos2);
		memset(dSig1Im + dwRightPos1 + 1, 0, sizeof(double) * (dwNfft - dwRightPos1 - 1));
		memset(dSig2Im + dwRightPos2 + 1, 0, sizeof(double) * (dwNfft - dwRightPos2 - 1));

		// IFFT
		_FFT1d(dSig1Re, dSig1Im, dwNfft, -1);
		_FFT1d(dSig2Re, dSig2Im, dwNfft, -1);

		// phase extraction
		_getPhase(dSig1Re, dSig1Im, m_dwWidth, dPhase1);
		_getPhase(dSig2Re, dSig2Im, m_dwWidth, dPhase2);
		_unwrapPhase(dPhase1, m_dwWidth);
		_unwrapPhase(dPhase2, m_dwWidth);
		_arraySubtraction(dPhase2, dPhase1, m_dwWidth, dPhaseD);
		_lineSpace(0, m_dwWidth - 1, m_dwWidth, dIndex);
		_lineSpace(dPhaseD[0], dPhaseD[m_dwWidth - 1], m_dwWidth, dPhaseLin);

		// nonlinear laser fitting
		_interp1d(dPhaseD, dIndex, dPhaseLin, m_dwWidth, m_dwWidth, dIndexLin);

		// dipsersion compensation
		_interp1d(dIndex, dPhase1, dIndexLin, m_dwWidth, m_dwWidth, dPhaseReal);
		_polyFit(dIndex, dPhaseReal, 1, m_dwWidth, dPhaseIdeal);
		_arraySubtraction(dPhaseReal, dPhaseIdeal, m_dwWidth, dDisp);
		_polyFit(dIndex, dDisp, 4, m_dwWidth, dDispFit);

		// dispersion compensation index
		//double hann;
		for (DWORD i = 0; i < m_dwWidth; i++)
		{
			//hann = 0.5 * (1 - cos(2 * PI * (double(i) / double(m_dwWidth / 2 - 1))));

			dDispComRe[i] = cos(-dDispFit[i]);// *hann;
			dDispComIm[i] = sin(-dDispFit[i]);// *hann;
		}

		// calibration index	
		_interp1d(kRange, dIndex, kRangeLin, m_dwWidth, m_dwWidth, dCalibIndex1);
		_interp1d(dPhaseD, dCalibIndex1, dPhaseLin, m_dwWidth, m_dwWidth, dCalibIndex2);
		_interp_generation(dIndex, dCalibIndex2, m_dwWidth, m_dwWidth, dwCalibInd, dCalibWeight);


		// calibration result visualization
		memcpy(m_dPhase, dPhase1, sizeof(double) * m_dwWidth);
		memcpy(m_dPhase + m_dwWidth, dPhase2, sizeof(double) * m_dwWidth);
		memcpy(m_dDisp, dDisp, sizeof(double) * m_dwWidth);
		memcpy(m_dDisp + m_dwWidth, dDispFit, sizeof(double) * m_dwWidth);
		memcpy(m_dCalibRes + m_dwNfft / 2, dSig1Re, sizeof(double) * m_dwNfft / 2);

		//	_interp1d(dIndex, dTempRe, dCalibIndex2, m_dwWidth, m_dwWidth, dTempRe1);
		_interp1d(dTempRe1, dTempRe, m_dwWidth, m_dwWidth, dwCalibInd, dCalibWeight);
		_complexProduct(dTempRe1, dTempIm, dDispComRe, dDispComIm, m_dwWidth);
		_FFT1d(dTempRe1, dTempIm, m_dwNfft, 1);
		_abs2(dTempRe1, dTempIm, dwNfft);
		_dBscale(dTempRe1, dwNfft, 0);
		memcpy(m_dCalibRes + m_dwNfft / 2, dTempRe1, sizeof(double) * m_dwNfft / 2);

		// save calibration data	
		m_pMainDlg->m_bIsCalibrated = TRUE;

		m_pMainDlg->m_dwCalibLeft = m_pMainDlg->m_RoiRect.left; // DWORD
		m_pMainDlg->m_dwCalibWidth = m_pMainDlg->m_RoiRect.width; // DWORD

		if (m_pMainDlg->m_wCalibBG == NULL)
			m_pMainDlg->m_wCalibBG = new UINT16[m_pMainDlg->m_dwCalibWidth];
		if (m_pMainDlg->m_dwCalibIndex == NULL)
			m_pMainDlg->m_dwCalibIndex = new DWORD[m_pMainDlg->m_dwCalibWidth];
		if (m_pMainDlg->m_dCalibWeight == NULL)
			m_pMainDlg->m_dCalibWeight = new double[m_pMainDlg->m_dwCalibWidth];
		if (m_pMainDlg->m_dCalibDispComRe == NULL)
			m_pMainDlg->m_dCalibDispComRe = new double[m_pMainDlg->m_dwCalibWidth];
		if (m_pMainDlg->m_dCalibDispComIm == NULL)
			m_pMainDlg->m_dCalibDispComIm = new double[m_pMainDlg->m_dwCalibWidth];

		for (DWORD i = 0; i < m_pMainDlg->m_dwCalibWidth; i++)
			m_pMainDlg->m_wCalibBG[i] = (UINT16)m_dBG[i];
		memcpy(m_pMainDlg->m_dwCalibIndex, dwCalibInd, sizeof(DWORD) * m_pMainDlg->m_dwCalibWidth);
		memcpy(m_pMainDlg->m_dCalibWeight, dCalibWeight, sizeof(double) * m_pMainDlg->m_dwCalibWidth);
		memcpy(m_pMainDlg->m_dCalibDispComRe, dDispComRe, sizeof(double) * m_pMainDlg->m_dwCalibWidth);
		memcpy(m_pMainDlg->m_dCalibDispComIm, dDispComIm, sizeof(double) * m_pMainDlg->m_dwCalibWidth);

		m_pMainDlg->SetDlgItemTextW(IDC_BUTTON_LOAD_CALIBRATION, _T("Delete Calibration Data"));
		GetDlgItem(IDC_BUTTON_SAVE_CALIB)->EnableWindow(TRUE);
		//m_pMainDlg->GetDlgItem(IDC_RADIO_VIEW_IMAGE)->EnableWindow(TRUE);

		// [DWORD] m_pMainDlg->m_RoiRect.left;
		// [DWORD] m_pMainDlg->m_RoiRect.width;
		// [UINT16] m_wBG;
		// [double] dCalibIndex2
		// [double] dDispComRe * Hann Window
		// [double] dDispComIm * Hann Window

		InvalidateRect(m_RectPhaseWnd, FALSE);
		InvalidateRect(m_RectDispWnd, FALSE);
		InvalidateRect(m_RectCalibResWnd, FALSE);

	} while (0);
	
	delete[] dSig1;	delete[] dSig2; delete[] dSig1_1; delete[] dSig2_1;
	delete[] WvRange; delete[] kRange; delete[] kRangeLin;
	delete[] dSig1Re; delete[] dSig1Im;	delete[] dSig1ReTemp;
	delete[] dSig2Re; delete[] dSig2Im; delete[] dSig2ReTemp;

	delete[] dPhase1; delete[] dPhase2;
	delete[] dPhaseD; delete[] dPhaseLin;
	delete[] dIndex; delete[] dIndexLin;
	delete[] dPhaseReal; delete[] dPhaseIdeal;
	delete[] dDisp; delete[] dDispFit;
	delete[] dDispComRe; delete[] dDispComIm;
	delete[] dCalibIndex1; delete[] dCalibIndex2;
	delete[] dwCalibInd; delete[] dCalibWeight;
	delete[] dTempRe; delete[] dTempIm; delete[] dTempRe1;	
}


void CCalibration::OnBnClickedButtonSaveCalib()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strMsg;
	int res;

	CString szFilter = _T("μOCT Calibration File (*.calib) | *.calib; | ALL FIles (*.*)|*.*||");
	CFileDialog dlg(FALSE, _T("calib"), _T("microOCT"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		HANDLE hCalibFile = INVALID_HANDLE_VALUE;

		try
		{
			CString strPathName = dlg.GetPathName();
			DWORD dwWritten;
			DWORD dwTotalWritten = 0;

			// create file
			if ((hCalibFile = CreateFile(strPathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
				FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, NULL)) == INVALID_HANDLE_VALUE)
			{
				res = GetLastError();
				throw res;
			}

			// m_dwCalibLeft & m_dwCalibWidth
			DWORD* dwTemp = new DWORD[m_pMainDlg->m_dwScopeDataNumFFT];
			memset(dwTemp, 0, sizeof(DWORD) * m_pMainDlg->m_dwScopeDataNumFFT);
			dwTemp[0] = m_pMainDlg->m_dwCalibLeft;
			dwTemp[1] = m_pMainDlg->m_dwCalibWidth;

			if (!WriteFile(hCalibFile, dwTemp, sizeof(DWORD) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;
						
			// m_wCalibBG
			UINT16* wTemp = new UINT16[m_pMainDlg->m_dwScopeDataNumFFT];
			memset(wTemp, 0, sizeof(UINT16) * m_pMainDlg->m_dwScopeDataNumFFT);
			memcpy(wTemp, m_pMainDlg->m_wCalibBG, sizeof(UINT16) * m_pMainDlg->m_dwCalibWidth);
			if (!WriteFile(hCalibFile, wTemp, sizeof(UINT16) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;
			delete[] wTemp;
			
			// m_dwCalibIndex
			memcpy(dwTemp, m_pMainDlg->m_dwCalibIndex, sizeof(DWORD) * m_pMainDlg->m_dwCalibWidth);
			if (!WriteFile(hCalibFile, dwTemp, sizeof(DWORD) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;
			delete[] dwTemp;

			// m_dCalibWeight
			double* dTemp = new double[m_pMainDlg->m_dwScopeDataNumFFT];
			memset(dTemp, 0, sizeof(double) * m_pMainDlg->m_dwScopeDataNumFFT);
			memcpy(dTemp, m_pMainDlg->m_dCalibWeight, sizeof(double) * m_pMainDlg->m_dwCalibWidth);
			if (!WriteFile(hCalibFile, dTemp, sizeof(double) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;

			// m_dCalibDispComRe
			memcpy(dTemp, m_pMainDlg->m_dCalibDispComRe, sizeof(double) * m_pMainDlg->m_dwCalibWidth);
			if (!WriteFile(hCalibFile, dTemp, sizeof(double) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;

			// m_dCalibDispComIm
			memcpy(dTemp, m_pMainDlg->m_dCalibDispComIm, sizeof(double) * m_pMainDlg->m_dwCalibWidth);
			if (!WriteFile(hCalibFile, dTemp, sizeof(double) * m_pMainDlg->m_dwScopeDataNumFFT, &dwWritten, NULL))
			{
				res = GetLastError();
				throw res;
			}
			dwTotalWritten += dwWritten;
			delete[] dTemp;


			CloseHandle(hCalibFile);

			m_pMainDlg->m_bIsCalibrated = TRUE;

			SetDlgItemTextW(IDC_BUTTON_LOAD_CALIBRATION, _T("Delete Calibration Data"));

			strMsg.Format(_T("Success to save calibration data! (Size: %d Bytes)"), dwTotalWritten);
			AfxMessageBox(strMsg);
			m_pMainDlg->m_ListStatusMsg.AddString(strMsg);
			m_pMainDlg->m_ListStatusMsg.SetCurSel(m_pMainDlg->m_ListStatusMsg.GetCount() - 1);
		}
		catch (int res)
		{						
			strMsg.Format(_T("Fail to load calibration data (%d)"), res);
			m_pMainDlg->m_ListStatusMsg.AddString(strMsg);
			m_pMainDlg->m_ListStatusMsg.SetCurSel(m_pMainDlg->m_ListStatusMsg.GetCount() - 1);
			AfxMessageBox(strMsg);
		}
	}
	
}


void CCalibration::OnBnClickedButtonExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	DestroyWindow();
}


void CCalibration::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete[] m_dBG;
	delete[] m_dSig1;
	delete[] m_dSig2;
	delete[] m_dPhase;
	delete[] m_dDisp;
	delete[] m_dCalibRes;

	m_pMainDlg->GetDlgItem(IDC_RADIO_VIEW_IMAGE)->EnableWindow(TRUE);
	m_pMainDlg->m_pCalibrationDlg = NULL;

	m_pMainDlg->m_p1DProfileDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	delete this;
}


void CCalibration::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	double rMax = m_pMainDlg->GetDlgItemInt(IDC_EDIT_CONTRAST_MAX);
	double rMin = m_pMainDlg->GetDlgItemInt(IDC_EDIT_CONTRAST_MIN);

	OnPaintPict(&m_PictureBG, &m_RectBG, m_dBG, 1, m_dwWidth, rMax, rMin);
	OnPaintPict(&m_PictureSig1, &m_RectSig1, m_dSig1, 1, m_dwWidth, rMax, rMin);
	OnPaintPict(&m_PictureSig2, &m_RectSig2, m_dSig2, 1, m_dwWidth, rMax, rMin);
	OnPaintPict(&m_PicturePhase, &m_RectPhase, m_dPhase, 2, m_dwWidth, 100*PI, 0);
	OnPaintPict(&m_PictureDisp, &m_RectDisp, m_dDisp, 2, m_dwWidth, 5, -5);
	OnPaintPict(&m_PictureCalibRes, &m_RectCalibRes, m_dCalibRes, 2, m_dwNfft/2, 100, 30);	

	SetDlgItemInt(IDC_STATIC_SIG_Y0, (int)rMin);
	SetDlgItemInt(IDC_STATIC_SIG_Y1, (int)rMax);

	SetDlgItemInt(IDC_STATIC_PHASE_Y0, 0);
	CString strPi = _T("100pi");
	SetDlgItemTextW(IDC_STATIC_PHASE_Y1, strPi);
	SetDlgItemInt(IDC_STATIC_DISP_Y0, -5);
	SetDlgItemInt(IDC_STATIC_DISP_Y1, 5);
	SetDlgItemInt(IDC_STATIC_CALIB_RES_Y0, 30);
	SetDlgItemInt(IDC_STATIC_CALIB_RES_Y1, 100);
	SetDlgItemInt(IDC_STATIC_CALIB_RES_X1, m_dwNfft / 2);
}


void CCalibration::OnPaintPict(CStatic* Picture, CRect Rect, double* Data, DWORD chN, DWORD N, double rMax, double rMin)
{
	CPaintDC dcPic(Picture);

	dcPic.FillSolidRect(Rect, RGB(255, 255, 255));
	
	double xInc = double(Rect.right) / double(N - 1);
	double yInc = double(Rect.bottom) / (rMax - rMin);

	dcPic.SelectObject(&m_GridPen);
	for (int i = 0; i < 6; i++)
	{
		dcPic.MoveTo(i * Rect.right / 6, 0);
		dcPic.LineTo(i * Rect.right / 6, Rect.bottom);
	}

	if (Data != NULL)
	{
		for (DWORD i = 0; i < chN; i++)
		{
			dcPic.SelectObject(&m_DataPen[i]);
			for (DWORD j = 0; j < N - 1; j++)
			{
				dcPic.MoveTo(int(xInc * (j)), Rect.bottom - int(yInc * (Data[j + i * N] - rMin)));
				dcPic.LineTo(int(xInc * (j + 1)), Rect.bottom - int(yInc * (Data[j + 1 + i * N] - rMin)));
			}
		}
	}
}