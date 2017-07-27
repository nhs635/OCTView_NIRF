#ifndef _IMSHOW_H_
#define _IMSHOW_H_

#include "afxwin.h"
#include "MyBitmap.h"

// Imshow 대화 상자입니다.

class Imshow : public CDialogEx
{
	DECLARE_DYNAMIC(Imshow)

public:
	Imshow(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~Imshow();

	CStatic m_StaticImshow;

	MyBitmap* m_pBitmap;
	float* m_pImageData;

	float m_cMax, m_cMin;
	int m_nImageSize;

	void operator() (float* pImageData, int width, int height, float cMin, float cMax);
	void operator() (uint8_t* pImageData, int width, int height); // , float cMin, float cMax);
	//void operator() (unsigned short* pImageData, int width, int height, float cMin, float cMax);
	//void operator() (unsigned char* pImageData, int width, int height, float cMin, float cMax);

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IMSHOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

};

#endif