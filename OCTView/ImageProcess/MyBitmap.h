#ifndef MY_BITMAP_H_
#define MY_BITMAP_H_

#include "afxwin.h"
#include "afxcmn.h"
//#include <windows.h>
#include <wingdi.h>

#include <iostream>
#include <Miscel\numcpp\array.h>

#define CLIP(x) (x < 0)? 0 : x > 255 ? 255 : x

typedef np::Array<BYTE, 2> CMAP;

class MyBitmap
{
public:
	MyBitmap();
	virtual ~MyBitmap();
	
	BOOL WriteImage(CString filename);	
	void SetBitmap(int nHeight, int nWidth, int nChannel = 1, int cmap_num = 0);
	void SetColormap(int cmap_num);
	void ScaleImage(float* pSrc, float cMin, float cMax);
	
	void PutChannelImg(const BYTE* img, int ch);
	BYTE* GetChannelImg(int ch);
	void SwapBlueRed();
	void IndToRGB(const MyBitmap* gray_bmp, int cmap_num = 0);
	void IndToRGB(const BYTE* gray_bmp, int cmap_num = 0);
	void IndToRGB_line(const BYTE* gray_bmp, int height_offset = 0, int cmap_num = 0);

	int GetChannel()	 const { return m_nChannels; }
	int GetHeight()		 const { return m_nHeight; }
	int GetWidth()		 const { return m_nWidth; }
	int GetLength()      const { return m_nHeight * m_nWidth; }
	int GetTotalLength() const { return m_nHeight * m_nWStep; }
	int GetWStep()		 const { return m_nWStep; }

	void SetPtr(BYTE val, int r = 0, int c = 0, int ch = 0) { *(m_pImageData + r*m_nWStep + m_nChannels * c + ch) = val; }
	BYTE* GetPtr(int r = 0, int c = 0, int ch = 0)	const { return m_pImageData + r*m_nWStep + m_nChannels *c + ch; }
	operator BYTE* () { return m_pImageData; };
	

	LPBITMAPINFO GetLpBmpInfo() const { return m_lpBmInfo; }

protected:
	int	m_nChannels;	// 채널 수
	int	m_nHeight;		// 세로 픽셀 수
	int	m_nWidth;		// 가로 픽셀 수
	int	m_nWStep;		// 행당 데이터 원소 수
	int m_nImSize;   
	BYTE* m_pImageData;	// 픽셀 배열 포인터
	BITMAPFILEHEADER m_hBmFile;
	LPBITMAPINFO m_lpBmInfo;
	
	CString cmapName[11] = { _T("inv_gray"), _T("gray"), _T("sepia"), 
								_T("jet"), _T("parula"), _T("hot"), _T("fire"), _T("hsv"),
								_T("ch1"), _T("ch2"), _T("ch3") };
	CMAP map[11];
};

#endif