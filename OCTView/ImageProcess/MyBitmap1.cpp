#include "stdafx.h"
#include "MyBitmap.h"

MyBitmap::MyBitmap()
	: m_nChannels(0), m_nHeight(0), m_nWidth(0), m_nWStep(0)
	, m_nImSize(0), m_lpBmInfo(NULL), m_pImageData(NULL)
{
	CString strPath = _T("ImageProcess\\Colormap\\");
	CString ext = _T(".colormap");
	DWORD dwRead;

	for (int i = 0; i < 10; i++)
	{
		HANDLE hColor = INVALID_HANDLE_VALUE;
		hColor = CreateFile(strPath + cmapName[i] + ext, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		
		map[i] = CMAP(256, 3); // gray, inv_gray, sepia, jet, parula, hot, fire

		ReadFile(hColor, map[i].raw_ptr(), sizeof(BYTE) * map[i].length(), &dwRead, NULL);
		CloseHandle(hColor);
	}
}


MyBitmap::~MyBitmap()
{
	if (m_pImageData) delete[] m_pImageData;
}


BOOL MyBitmap::WriteImage(CString filename)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwWrote;

	// 파일 열기
	if ((hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return FALSE;

	// Bitmap File Header
	if (WriteFile(hFile, &m_hBmFile, sizeof(BITMAPFILEHEADER), &dwWrote, NULL) == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	// Bitmap Info Header
	if (WriteFile(hFile, &(m_lpBmInfo->bmiHeader), sizeof(BITMAPINFOHEADER), &dwWrote, NULL) == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	// Bitmap Palette		
	if (WriteFile(hFile, &(m_lpBmInfo->bmiColors), 256 * sizeof(RGBQUAD), &dwWrote, NULL) == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	// 비트맵 데이터 writing		
	if (WriteFile(hFile, m_pImageData, m_nImSize, &dwWrote, NULL) == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);

	return TRUE;
}


void MyBitmap::SetBitmap(int nHeight, int nWidth, int cmap_num)
{
	/* Bitmap Info Header & Pallete */
	m_lpBmInfo = (LPBITMAPINFO)new BYTE[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	BITMAPINFOHEADER* pBmih = &(m_lpBmInfo->bmiHeader);

	m_nChannels = 1; // Gray Scale
	m_nHeight = nHeight;
	m_nWidth = nWidth;
	m_nWStep = ((m_nWidth*m_nChannels*sizeof(BYTE) + 3)&~3) / sizeof(BYTE);
	m_nImSize = nHeight * m_nWStep;

	pBmih->biSize = sizeof(BITMAPINFOHEADER);
	pBmih->biWidth = m_nWidth;
	pBmih->biHeight = m_nHeight;
	pBmih->biPlanes = 1;
	pBmih->biBitCount = m_nChannels * 8;
	pBmih->biCompression = BI_RGB;
	pBmih->biSizeImage = m_nImSize;
	pBmih->biClrImportant = 0;
	pBmih->biClrUsed = 0;
	pBmih->biXPelsPerMeter = 0;
	pBmih->biYPelsPerMeter = 0;
	
	/* Colormap */
	SetColormap(cmap_num);
	
	/* Bitmap File Header */
	m_hBmFile.bfType = 0x4D42; // 'BM'
	m_hBmFile.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + m_nImSize;
	m_hBmFile.bfReserved1 = 0;
	m_hBmFile.bfReserved2 = 0;
	m_hBmFile.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	/* Bitmap Data */
	m_pImageData = new BYTE[m_nHeight * m_nWStep];
	memset(m_pImageData, 0, m_nHeight * m_nWStep * sizeof(BYTE));
}


void MyBitmap::SetColormap(int cmap_num)
{
	// Colormap
	for (int i = 0; i < 256; i++) {
		m_lpBmInfo->bmiColors[i].rgbBlue = map[cmap_num](i, 2);
		m_lpBmInfo->bmiColors[i].rgbGreen = map[cmap_num](i, 1);
		m_lpBmInfo->bmiColors[i].rgbRed = map[cmap_num](i, 0);
		m_lpBmInfo->bmiColors[i].rgbReserved = 0;
	}
}


void MyBitmap::ScaleImage(float* pSrc, float cMin, float cMax)
{
	DWORD dwOffset[2];// = nHeight * nWStep;

	for (int i = 0; i < m_nHeight; i++)
	{
		dwOffset[0] = m_nWStep * (m_nHeight - 1 - i);
		dwOffset[1] = m_nWStep * i;
		for (int j = 0; j < m_nWStep; j++)
			m_pImageData[dwOffset[0] + j] = (BYTE)(CLIP(255 * (pSrc[dwOffset[1] + j] - cMin) / (cMax - cMin)));
	}
}