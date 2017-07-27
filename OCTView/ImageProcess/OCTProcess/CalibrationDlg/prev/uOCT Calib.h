#pragma once

#include "microOCTDlg.h"
#include "BasicOps.h"
#include "afxwin.h"

class CmicroOCTDlg;
// CCalibration 대화 상자입니다.

class CCalibration : public CDialogEx
{
	DECLARE_DYNAMIC(CCalibration)

public:
	CCalibration(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCalibration();

	CmicroOCTDlg *m_pMainDlg;
	
	CRect m_RectBG;
	CRect m_RectSig1;
	CRect m_RectSig2;
	CRect m_RectPhase;
	CRect m_RectDisp;
	CRect m_RectCalibRes;

	CRect m_RectBGWnd;
	CRect m_RectSig1Wnd;
	CRect m_RectSig2Wnd;
	CRect m_RectPhaseWnd;
	CRect m_RectDispWnd;
	CRect m_RectCalibResWnd;

	CPen m_GridPen;
	CPen m_DataPen[3];

	DWORD m_dwWidth;
	DWORD m_dwNfft;

	double* m_dBG;	 BOOL m_bGotBG;
	double* m_dSig1; BOOL m_bGotSig1;
	double* m_dSig2; BOOL m_bGotSig2;

	double* m_dPhase;
	double* m_dDisp;
	double* m_dCalibRes;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALIBRATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBg();
	afx_msg void OnBnClickedButtonSig1();
	afx_msg void OnBnClickedButtonSig2();
	afx_msg void OnBnClickedButtonCalibrate();
	afx_msg void OnBnClickedButtonSaveCalib();
	afx_msg void OnBnClickedButtonExit();
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnPaint();

	void OnPaintPict(CStatic* Picture, CRect Rect, double* Data, DWORD chN, DWORD N, double rMax, double rMin);

	CStatic m_PictureBG;
	CStatic m_PictureSig1;
	CStatic m_PictureSig2;
	CStatic m_PicturePhase;
	CStatic m_PictureDisp;
	CStatic m_PictureCalibRes;
	UINT m_Edit_dB_Drop;
};
