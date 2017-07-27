// CalibrationDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OCT-FLIM.h"
#include "OCT-FLIMDlg.h"
#include "CalibrationDlg.h"
#include "afxdialogex.h"

#include <Tab View\VisStream.h>
#include <ImageProcess\OCTProcess\OCTProcess.h>

// CalibrationDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CalibrationDlg, CDialogEx)

CalibrationDlg::CalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CALIBRATION, pParent)
	, m_Edit_DiscomVal(0)
	, m_CheckResult(FALSE)
{

}


CalibrationDlg::~CalibrationDlg()
{
}


void CalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_DiscomVal);
	DDX_Check(pDX, IDC_CHECK_RESULT, m_CheckResult);
}


BEGIN_MESSAGE_MAP(CalibrationDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BACKGROUND, &CalibrationDlg::OnBnClickedButtonBackground)
	ON_BN_CLICKED(IDC_BUTTON_D1, &CalibrationDlg::OnBnClickedButtonD1)
	ON_BN_CLICKED(IDC_BUTTON_D2, &CalibrationDlg::OnBnClickedButtonD2)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_CALIB, &CalibrationDlg::OnBnClickedButtonGenerateCalib)
	ON_EN_CHANGE(IDC_EDIT1, &CalibrationDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CalibrationDlg 메시지 처리기입니다.
void CalibrationDlg::SetMainDlg(COCTFLIMDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
	m_pVisStream = &(m_pMainDlg->m_VisStream);
	m_pOCT = m_pMainDlg->m_pOCT;
}


BOOL CalibrationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CalibrationDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_pVisStream->m_pCalibrationDlg = nullptr;
	delete this;
}


void CalibrationDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	DestroyWindow();
}


void CalibrationDlg::OnBnClickedButtonBackground()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pOCT->SetBg(m_pMainDlg->m_FrameFringe);
	SetDlgItemTextW(IDC_STATIC_BACKGROUND, _T("Captured"));

	UpdateData(TRUE);
	if (m_CheckResult)
	{
		CScope scope;
		scope(m_pOCT->GetBg(), 2600, 0, 65535);
	}
}


void CalibrationDlg::OnBnClickedButtonD1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pOCT->SetFringe(m_pMainDlg->m_FrameFringe, 0);
	SetDlgItemTextW(IDC_STATIC_D1, _T("Captured"));
	
	UpdateData(TRUE);
	if (m_CheckResult)
	{
		CScope scope;
		scope(m_pOCT->GetFringe(0), 2600, 0, 65535);
	}
}


void CalibrationDlg::OnBnClickedButtonD2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pOCT->SetFringe(m_pMainDlg->m_FrameFringe, 1);
	SetDlgItemTextW(IDC_STATIC_D2, _T("Captured"));

	UpdateData(TRUE);
	if (m_CheckResult)
	{
		CScope scope;
		scope(m_pOCT->GetFringe(1), 2600, 0, 65535);
	}
}


void CalibrationDlg::OnBnClickedButtonGenerateCalib()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_pOCT->GenerateCalibration(m_Edit_DiscomVal, m_CheckResult);
	SetDlgItemTextW(IDC_STATIC_GENERATE_CALIB, _T("Calibrated"));
}


void CalibrationDlg::OnEnChangeEdit1()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_pOCT->ChangeDiscomValue(m_Edit_DiscomVal);
}
