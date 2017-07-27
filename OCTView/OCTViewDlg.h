#ifndef _OCT_VIEW_DLG_H_
#define _OCT_VIEW_DLG_H_

#include "afxwin.h"
#include "afxcmn.h"

#include "ImageProcess\OCTProcess\OCTProcess.h"
#include "ImageProcess\circularize.h"
#include "ImageProcess\medfilt_8u.h"
#include "ImageProcess\MyBitmap.h"
#include "Configuration.h"

#include "Miscel\objcpp\Queue.h"
#include <iostream>
#include <complex>
#include <thread>
#include <mutex>
#include <queue>


// COCTViewDlg 대화 상자입니다.
class COCTViewDlg : public CDialogEx
{

public:
	COCTViewDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OCTVIEW_DIALOG };
#endif

private:
	CStatic m_StaticResImage;
	CStatic m_StaticResProjection;
	CStatic m_StaticResColorbarProjection;
	CStatic m_StaticResNirfMap;
	CStatic m_StaticResColorbarNirfMap;

public:
	MyBitmap* m_pBitmapOCT;
	MyBitmap* m_pBitmapCircOCT;
	MyBitmap* m_pBitmapProjection;
	MyBitmap* m_pColorbarProjection;
	MyBitmap* m_pBitmapNirfMap;
	MyBitmap* m_pColorbarNirfMap;
	
	CRect m_ResImageRect;
	CRect m_ResProjectionRect, m_CBarRegionRect1;
	CRect m_ResNirfMapRect, m_CBarRegionRect2;
	CRect m_VisRegionRectWnd;
	POINT pt;

public: // Buffers
	std::vector<np::Array<float, 2>> m_OCT_Image32f; // (2048 x 1024) x N
	np::Array<float, 2> m_Projection32f; // (1024 x N)
	np::Array<float, 2> m_NirfMap32f;
	np::Array<float> m_NirfSignal32f;
	np::Array<BYTE, 2> m_TempBitmapOCT; // (1024 x 2048)
	np::Array<BYTE, 2> m_TempBitmapCircOCT;
	np::Array<BYTE, 2> m_OCT_Image8u; 

	circularize circ_OCT;
	medfilt_8u median_filter;

private: // Pens
	CPen m_CircCenterPen;
	CPen m_SheathThicknessPen;
	CPen m_CircRadiusPen;

	// 구현입니다.
protected:
	HICON m_hIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void SetSize(int scans, int alines);
	void SetBitmaps(int nFrame);
	void SetBuffers(int nFrame);
	virtual BOOL DestroyWindow();
	virtual void PostNcDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	void ScaleRect(const float* pSrc, uint8_t* pDst);
	void TransposeRect(const uint8_t* pSrc, uint8_t* pDst);

public: // Control Data
	CStatusBarCtrl m_StatusBar;
	CProgressCtrl m_ProgressRes;
	int m_BufferSize;
	int m_DiscomValue;
	int m_SliderCurFrame;
	CString m_PixelSize;
	int m_EditResMaxDb, m_EditResMinDb;	
	int m_EditCircCenter;
	int m_EditSheathThickness;
	CString m_EditCircRadiusRatio;
	int m_ComboResLut;
	CString m_EditResMaxNirfMap, m_EditResMinNirfMap;
	int m_EditNirfOffset;

	BOOL m_CheckSimulWriting;
	BOOL m_CheckSingleFrame;
	BOOL m_CheckFilipHorizontal;
	BOOL m_CheckCircularize;
	BOOL m_CheckShowGuideline;
	BOOL m_ToggleMeasureDistance;

public:
	int m_nScans, m_nScansFFT, m_nAlines, m_nAlines4, m_radius, m_nFrame;
	int m_nSizeFrame, m_nSizeImageOCT;

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeComboResLut();
	afx_msg void OnBnClickedCheckSimulWriting();
	afx_msg void OnBnClickedButtonImageProcessing();
	afx_msg void OnBnClickedButtonSaveBitmap();
	afx_msg void OnBnClickedButtonSaveProjection();
	afx_msg void OnEnChangeEditResAlineMaxDb();
	afx_msg void OnEnChangeEditResAlineMinDb();
	afx_msg void OnEnChangeEditResNirfMapMax();
	afx_msg void OnEnChangeEditResNirfMapMin();
	afx_msg void OnEnChangeEditResCircCenter();
	afx_msg void OnEnChangeEditResSheathThickness();
	afx_msg void OnBnClickedCheckCircularize();
	afx_msg void OnBnClickedCheckResGuideline();
	afx_msg void OnBnClickedToggleMeasureDistance();
	afx_msg void OnBnClickedCheckHorizonFlip();
	afx_msg void OnEnChangeEditCircRadiusRatio();
	afx_msg void OnDeltaposSpinNirfOffset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditNirfOffset();

public:
	void ImageProcessingSingle();
	void ImageProcessingExternal();
	void ImageProcessingSimulWriting();
	void GetProjection(const std::vector<np::Array<float, 2>>& OCT_vector, np::Array<float, 2>& Proj, int offset = 0);

private: // About Ini File
	CString m_strIniPath;
	void SetIniFile();
	void GetIniFile();

public:
	CPoint pt_dist1[2], pt_dist2[2];
	int n_click;

public:
	CString m_path;
};

#endif