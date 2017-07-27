#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "afxwin.h"
#include "afxcmn.h"

class COCTFLIMDlg;
class CVisStream;
class OCTProcess;

// Calibration 대화 상자입니다.
class CalibrationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CalibrationDlg)

public:
	CalibrationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CalibrationDlg();

private:
	COCTFLIMDlg* m_pMainDlg;
	CVisStream* m_pVisStream;
	OCTProcess* m_pOCT;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALIBRATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
