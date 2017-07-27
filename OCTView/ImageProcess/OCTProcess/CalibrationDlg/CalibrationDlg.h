#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "afxwin.h"
#include "afxcmn.h"

class COCTFLIMDlg;
class CVisStream;
class OCTProcess;

// Calibration ��ȭ �����Դϴ�.
class CalibrationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CalibrationDlg)

public:
	CalibrationDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CalibrationDlg();

private:
	COCTFLIMDlg* m_pMainDlg;
	CVisStream* m_pVisStream;
	OCTProcess* m_pOCT;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALIBRATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:

	void SetMainDlg(COCTFLIMDlg* pMainDlg);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonBackground();
	afx_msg void OnBnClickedButtonD1();
	afx_msg void OnBnClickedButtonD2();
	afx_msg void OnBnClickedButtonGenerateCalib();
	int m_Edit_DiscomVal;
	afx_msg void OnEnChangeEdit1();

	BOOL m_CheckResult;
};

#endif
