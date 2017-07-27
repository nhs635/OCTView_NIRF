// CalibrationDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "OCT-FLIM.h"
#include "OCT-FLIMDlg.h"
#include "CalibrationDlg.h"
#include "afxdialogex.h"

#include <Tab View\VisStream.h>
#include <ImageProcess\OCTProcess\OCTProcess.h>

// CalibrationDlg ��ȭ �����Դϴ�.

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


// CalibrationDlg �޽��� ó�����Դϴ�.
void CalibrationDlg::SetMainDlg(COCTFLIMDlg * pMainDlg)
{
	m_pMainDlg = pMainDlg;
	m_pVisStream = &(m_pMainDlg->m_VisStream);
	m_pOCT = m_pMainDlg->m_pOCT;
}


BOOL CalibrationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CalibrationDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_pVisStream->m_pCalibrationDlg = nullptr;
	delete this;
}


void CalibrationDlg::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	DestroyWindow();
}


void CalibrationDlg::OnBnClickedButtonBackground()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	m_pOCT->GenerateCalibration(m_Edit_DiscomVal, m_CheckResult);
	SetDlgItemTextW(IDC_STATIC_GENERATE_CALIB, _T("Calibrated"));
}


void CalibrationDlg::OnEnChangeEdit1()
{
	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	m_pOCT->ChangeDiscomValue(m_Edit_DiscomVal);
}
