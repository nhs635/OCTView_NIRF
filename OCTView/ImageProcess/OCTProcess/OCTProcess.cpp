#include "stdafx.h"
#include "OCTProcess.h"

#include <OCTViewDlg.h>

#include "tbb\parallel_for.h"
#include "tbb\blocked_range.h"
//#include <chrono>


OCTProcess::OCTProcess(const COCTViewDlg* pMainDlg) :
	pMainDlg(pMainDlg), _running(false),

	raw_size({ pMainDlg->m_nScans, pMainDlg->m_nAlines }), raw2_size({ pMainDlg->m_nScans / 2, pMainDlg->m_nAlines }),
	fft_size({ pMainDlg->m_nScansFFT, pMainDlg->m_nAlines }), fft2_size({ GET_NEAR_2_POWER(raw2_size.width), pMainDlg->m_nAlines }),
	
	signal(fft_size.width, fft_size.height),
	complex_signal(fft_size.width, fft_size.height), complex_resamp(fft2_size.width, fft2_size.height),
	fft_complex(fft_size.width, fft_size.height), fft2_complex(fft2_size.width, fft2_size.height),
	fft2_linear(fft2_size.width, fft2_size.height), 	

	bg(raw_size.width), fringe(fft_size.width, 2),
	calib_index(raw2_size.width), calib_weight(raw2_size.width), dispersion(raw2_size.width), discom(raw2_size.width), dispersion1(raw2_size.width), win(raw_size.width)
{
	fft1.initialize(raw_size.width, raw_size.height);
	fft2.initialize(fft_size.width);
	fft3.initialize(raw2_size.width);

	memset(fringe.raw_ptr(), 0, sizeof(float) * fft_size.width * 2);
	memset(signal.raw_ptr(), 0, sizeof(float) * fft_size.width * fft_size.height);
	memset(fft_complex.raw_ptr(), 0, sizeof(float) * 2 * fft_size.width * fft_size.height);
	memset(complex_resamp.raw_ptr(), 0, sizeof(float) * 2 * fft2_size.width * fft2_size.height);

	for (int i = 0; i < raw_size.width; i++)
	{
		bg(i) = POWER_2_15;
		win(i) = (float)(1 - cos(IPP_2PI * i / (raw_size.width - 1))) / 2; // Hann Window
	}

	for (int i = 0; i < raw2_size.width; i++)
	{
		calib_index(i) = (float)i * 2.0f;
		calib_weight(i) = 1;
		dispersion(i) = { 1, 0 };
		discom(i) = { 1, 0 };
		dispersion1(i) = { 1, 0 };
	}		
}


OCTProcess::~OCTProcess()
{
	if (_thread.joinable())
	{
		_running = false;
		_thread.join();
	}
}


/* OCT Image */
void OCTProcess::operator() (float* img, uint16_t* fringe) //(np::Array<float, 2>& img, const np::Array<uint16_t, 2>& fringe) // (2600 x 1024) => (4096 x 1024)
{	
	tbb::parallel_for(tbb::blocked_range<size_t>(0, (size_t)raw_size.height),
		[&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i != r.end(); ++i)
		{
			int r1 = raw_size.width * i;
			int f1 = fft_size.width * i;
			int f2 = fft2_size.width * i;

			// 1. Single Precision Conversion & Zero Padding (3 msec)
			ippsConvert_16u32f(fringe + r1, signal.raw_ptr() + f1, raw_size.width);
			
			// 2. BG Subtraction & Hanning Windowing (10 msec)
			ippsSub_32f_I(bg, signal.raw_ptr() + f1, raw_size.width);
			ippsMul_32f_I(win, signal.raw_ptr() + f1, raw_size.width);
			
			// 3. Fourier transform (21 msec)
			fft1((Ipp32fc*)(fft_complex.raw_ptr() + f1), signal.raw_ptr() + f1, i);

			// 4. Circshift by nfft/4 & Inverse Fourier transform (5+19 sec)
			std::rotate(fft_complex.raw_ptr() + f1, fft_complex.raw_ptr() + f1 + 3 * fft_size.width / 4, fft_complex.raw_ptr() + f1 + fft_size.width);
			ippsSet_32f(0.0f, (Ipp32f*)(fft_complex.raw_ptr() + f1 + fft_size.width / 4), fft_size.width);

			fft2.inverse((Ipp32fc*)(complex_signal.raw_ptr() + f1), (const Ipp32fc*)(fft_complex.raw_ptr() + f1));

			// 5. k linear resampling (5 msec)		
			LinearInterp_32fc((const Ipp32fc*)(complex_signal.raw_ptr() + f1), (Ipp32fc*)(complex_resamp.raw_ptr() + f2),
				raw_size.width, raw2_size.width, calib_index.raw_ptr(), calib_weight.raw_ptr());

			// 6. Dispersion compensation (4 msec)
			ippsMul_32fc_I((const Ipp32fc*)dispersion1.raw_ptr(), (Ipp32fc*)(complex_resamp.raw_ptr() + f2), raw2_size.width);
						
			// 7. Fourier transform (9 msec)
			fft3.forward((Ipp32fc*)(fft2_complex.raw_ptr() + f2), (const Ipp32fc*)(complex_resamp.raw_ptr() + f2));

			// 8. dB Scaling (5 msec)
			ippsPowerSpectr_32fc((const Ipp32fc*)(fft2_complex.raw_ptr() + f2), fft2_linear.raw_ptr() + f2, fft2_size.width);
			ippsLog10_32f_A11(fft2_linear.raw_ptr() + f2, img + f2, fft2_size.width);
			ippsMulC_32f_I(10.0f, img + f2, fft2_size.width);
			
			// 9. Circshift by -nfft/2 (1 msec)
			std::rotate(img + f2, img + f2 + fft2_size.width / 2, img + f2 + fft2_size.width);
		}
	});	
}


void OCTProcess::run()
{
	unsigned int frameIndex = 0;

	_running = true;
	while (_running)
		DidAcquireData(frameIndex++);	
}


bool OCTProcess::startOCTprocess()
{
	if (_thread.joinable())
	{
		dumpErrorSystem(::GetLastError(), "ERROR: OCT processing thread is already running: ");
		return false;
	}

	_thread = std::thread(&OCTProcess::run, this);

	printf("OCT image processing thread is started.\n");

	return true;
}


void OCTProcess::stopOCTprocess()
{
	if (_thread.joinable())
	{
		DidStopData(); //_running = false;
		_thread.join();
	}

	printf("OCT image processing thread is finished normally.\n");
}


/* OCT Calibration */
void OCTProcess::SetBg(const np::Array<uint16_t, 2> &frame)
{
	std::thread set_bg([&]() {
		int N = 20;

		for (int i = 0; i < frame.size(0); i++)
		{
			bg(i) = 0;
			for (int j = 0; j < N; j++)
				bg(i) += (float)frame(i, j);
			bg(i) /= N;
		}

		HANDLE hFile = INVALID_HANDLE_VALUE;
		CString filename(_T("bg.bin"));
		DWORD dwWrote;
		hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hFile, frame.raw_ptr(), sizeof(uint16_t) * frame.length(), &dwWrote, NULL);
		CloseHandle(hFile);

		printf("Captured background fringe.\n");
	});

	set_bg.detach();
}

void OCTProcess::SetFringe(const np::Array<uint16_t, 2> &frame, int ch)
{	
	std::thread set_fringe([&]() {
		for (int i = 0; i < frame.size(0); i++)
			fringe(i, ch) = (float)frame(i, 0);

		HANDLE hFile = INVALID_HANDLE_VALUE;
		CString filename; filename.Format(_T("d%d.bin"), ch + 1);
		DWORD dwWrote;
		hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hFile, frame.raw_ptr(), sizeof(uint16_t) * frame.length(), &dwWrote, NULL);
		CloseHandle(hFile);

		printf("Captured d%d fringe.\n", ch+1);
	});

	set_fringe.detach();
}

void OCTProcess::GenerateCalibration(int discom_val, bool res_show)
{	
	int dsc_val = discom_val;
	bool show = res_show;

	std::thread calib([&]() {
		// buffer
		np::Array<std::complex<float>, 2> res(fft_size.width, 2);
		np::Array<float, 2> res1(fft_size.width, 2);
		np::Array<float, 2> res2(fft_size.width, 2);
		np::Array<std::complex<float>, 2> res3(fft_size.width, 2);
		np::Array<float, 2> phase(raw_size.width, 2);

		Ipp32f mxVal; int mxInd;
		Ipp32f th = 20.0f;

		for (int ch = 0; ch < 2; ch++)
		{
			// 0. BG removal & windowing
			for (int i = 0; i < raw_size.width; i++)
				fringe(i, ch) = (fringe(i, ch) - bg(i)) * win(i);

			if (show)
			{
				CScope scope;
				scope(&fringe(0, ch), fft_size.width, -32768.0f, 32768.0f);
			}

			// 1. FFT of raw signal	
			fft1((Ipp32fc*)&res(0, ch), &fringe(0, ch));
			ippsPowerSpectr_32fc((const Ipp32fc*)&res(0, ch), (Ipp32f*)&res1(0, ch), fft_size.width);
			ippsLog10_32f_A11((const Ipp32f*)&res1(0, ch), (Ipp32f*)&res2(0, ch), fft_size.width);
			ippsMulC_32f_I(10.0f, (Ipp32f*)&res2(0, ch), fft_size.width);

			if (show)
			{
				CScope scope;
				scope(&res2(0, ch), fft_size.width, 0.0f, 160.0f);
			}

			// 2. Maximum detection & Non-peak signal elimination		
			//DC Removal &
			for (int i = fft_size.width - 1; i >= fft_size.width - 5; i--)
				res2(i, ch) = 0;
			ippsMaxIndx_32f((const Ipp32f*)&res2(fft2_size.width, ch), fft2_size.width, &mxVal, &mxInd);
			mxInd += fft2_size.width;

			np::Array<float, 2> mask(fft_size.width, 2);
			ippsZero_32f(mask, fft_size.width * 2);

			for (int i = mxInd; i >= fft2_size.width; i--)
			{
				if (res2(i, ch) > mxVal - th)
				{
					for (int j = mxInd; j >= i; j--)
						mask(j, ch) = 1;
						//res(j, ch) = { 0, 0 };
					//break;
				}
			}
			for (int i = mxInd; i < fft_size.width; i++)
			{
				if (res2(i, ch) > mxVal - th)
				{
					for (int j = mxInd; j < i; j++)
						mask(j, ch) = 1;
						//res(j, ch) = { 0, 0 };
					//break;
				}
			}

			if (show)
			{
				CScope scope;
				scope(&mask(0, ch), fft_size.width, 0.0f, 2.0f);
			}


			if (show)
			{
				CScope scope;
				ippsPowerSpectr_32fc((const Ipp32fc*)&res(0, ch), (Ipp32f*)&res1(0, ch), fft_size.width);
				ippsLog10_32f_A11((const Ipp32f*)&res1(0, ch), (Ipp32f*)&res2(0, ch), fft_size.width);
				ippsMulC_32f_I(10.0f, (Ipp32f*)&res2(0, ch), fft_size.width);
				scope(&res2(0, ch), fft_size.width, 0.0f, 160.0f);
			}

			// 3. Frequency shifting effect removal
			std::rotate(&res(0, ch), &res(3 * fft_size.width / 4, ch), &res(fft_size.width, ch));
			ippsSet_32f(0.0f, (Ipp32f*)&res(fft_size.width / 4, ch), fft_size.width);
			
			// 4. IFFT of the signal & Phase extraction
			fft2.inverse((Ipp32fc*)&res3(0, ch), (Ipp32fc*)&res(0, ch));

			ippsPhase_32fc((const Ipp32fc*)&res3(0, ch), &phase(0, ch), raw_size.width);
			UnwrapPhase_32f(&phase(0, ch), raw_size.width);

			if (show)
			{
				CScope scope;
				scope(&phase(0, ch), raw_size.width, -3000.0f, 3000.0f);
			}
		}

		// 5. calib_index & calib_weight generation
		np::Array<float> index(raw_size.width); // 0 ~ nScans
		np::Array<float> lin_phase(raw2_size.width);
		np::Array<float> new_index(raw2_size.width);

		ippsSub_32f_I(&phase(0, 0), &phase(0, 1), raw_size.width); // phase(:,1) - phase(:,0)
		ippsAbs_32f_I(&phase(0, 1), raw_size.width); // absolute value

		LineSpace_32f(0.0f, (float)(raw_size.width - 1), raw_size.width, index.raw_ptr());
		LineSpace_32f(phase(0, 1), phase(raw_size.width - 1, 1), raw2_size.width, lin_phase.raw_ptr());
		Interpolation_32f(&phase(0, 1), index.raw_ptr(), lin_phase.raw_ptr(), raw_size.width, raw2_size.width, new_index.raw_ptr());

		float temp;
		for (int i = 0; i < raw2_size.width; i++)
		{
			temp = floor(new_index(i));
			calib_weight(i) = 1.0f - (new_index(i) - temp);
			calib_index(i) = temp;
		}

		// 6. Dispersion compensation
		np::Array<float> index1(raw2_size.width); // 0 ~ nScans/2
		np::Array<float> lin_phase1(raw2_size.width);
		np::Array<float> lin_phase2(raw2_size.width);
		LineSpace_32f(0.0f, (float)(raw2_size.width - 1), raw2_size.width, index1.raw_ptr());

		Ipp32f offset, slope;
		Interpolation_32f(index.raw_ptr(), &phase(0, 0), new_index.raw_ptr(), raw_size.width, raw2_size.width, lin_phase1.raw_ptr()); // phase with disp : lin_phase1
		LinearRegression_32f(index1.raw_ptr(), lin_phase1.raw_ptr(), raw2_size.width, offset, slope); // poly fit
		ippsVectorSlope_32f(lin_phase2.raw_ptr(), raw2_size.width, offset, slope); // poly val (phase without disp : lin_phase2)

		for (int i = 0; i < raw2_size.width; i++)
		{
			temp = lin_phase2(i) - lin_phase1(i);
			dispersion(i) = { cos(temp), sin(temp) };
		}		
		ChangeDiscomValue(dsc_val);
				
		SaveCalibration();

		printf("Successfully calibrated.\n");
	});

	calib.detach();
}


void OCTProcess::ChangeDiscomValue(int discom_val)
{
	float temp;
	for (int i = 0; i < raw2_size.width; i++)
	{
		temp = (((float)i - (float)raw2_size.width / 2.0f) / (float)raw2_size.width); // *((i - raw2_size.width / 2) / raw2_size.width);
		discom(i) = { cos((float)discom_val*temp*temp), sin((float)discom_val*temp*temp) };
	}
	ippsMul_32fc((Ipp32fc*)dispersion.raw_ptr(), (Ipp32fc*)discom.raw_ptr(), (Ipp32fc*)dispersion1.raw_ptr(), raw2_size.width);
}


void OCTProcess::SaveCalibration()
{
	DWORD dwWritten, dwTotalWritten = 0;

	// create file
	HANDLE hCalibFile = INVALID_HANDLE_VALUE;
	CString strPathName(_T("calibration.dat"));

	hCalibFile = CreateFile(strPathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	
	// calib_index
	WriteFile(hCalibFile, calib_index.raw_ptr(), sizeof(float) * raw2_size.width, &dwWritten, NULL);
	dwTotalWritten += dwWritten;

	// calib_weight
	WriteFile(hCalibFile, calib_weight.raw_ptr(), sizeof(float) * raw2_size.width, &dwWritten, NULL);
	dwTotalWritten += dwWritten;

	// dispersion compensation real
	Ipp32f* real = ippsMalloc_32f(raw2_size.width);
	ippsReal_32fc((const Ipp32fc*)dispersion.raw_ptr(), real, raw2_size.width);
	WriteFile(hCalibFile, real, sizeof(float) * raw2_size.width, &dwWritten, NULL);
	ippsFree(real);
	dwTotalWritten += dwWritten;

	// dispersion compensation imag
	Ipp32f* imag = ippsMalloc_32f(raw2_size.width);
	ippsImag_32fc((const Ipp32fc*)dispersion.raw_ptr(), imag, raw2_size.width);
	WriteFile(hCalibFile, imag, sizeof(float) * raw2_size.width, &dwWritten, NULL);
	ippsFree(imag);
	dwTotalWritten += dwWritten;

	CloseHandle(hCalibFile); 
}


void OCTProcess::LoadCalibration(CString bg_path, CString calib_path, int discom)
{
	DWORD dwRead, dwTotalRead = 0;
	
	// create file (background)
	HANDLE hBgFile = INVALID_HANDLE_VALUE;
	np::Array<uint16_t, 2> frame(2 * pMainDlg->m_nScans, pMainDlg->m_nAlines);
	np::Array<uint16_t, 2> frame1(pMainDlg->m_nScans, pMainDlg->m_nAlines);
	np::Array<uint16_t, 2> frame2(pMainDlg->m_nScans, pMainDlg->m_nAlines);

	hBgFile = CreateFile(bg_path, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hBgFile != INVALID_HANDLE_VALUE)
	{
		// background
		BOOL res = ReadFile(hBgFile, frame.raw_ptr(), sizeof(uint16_t) * frame.length(), &dwRead, NULL);
		if (res) 
		{
			ippsCplxToReal_16sc((Ipp16sc *)frame.raw_ptr(), (Ipp16s *)frame1.raw_ptr(), (Ipp16s *)frame2.raw_ptr(), pMainDlg->m_nScans * pMainDlg->m_nAlines);
			int N = 20;
			for (int i = 0; i < frame1.size(0); i++)
			{
				bg(i) = 0;
				for (int j = 0; j < N; j++)
					bg(i) += (float)frame1(i, j);
				bg(i) /= N;
			}
		} 
		CloseHandle(hBgFile);		
	}
		
	// create file (calibration)
	HANDLE hCalibFile = INVALID_HANDLE_VALUE;
	hCalibFile = CreateFile(calib_path, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hCalibFile != INVALID_HANDLE_VALUE)
	{
		// calib_index
		Ipp32s* ind = ippsMalloc_32s(raw2_size.width);
		ReadFile(hCalibFile, ind, sizeof(int) * raw2_size.width, &dwRead, NULL);
		ippsConvert_32s32f(ind, calib_index.raw_ptr(), raw2_size.width);
		ippsFree(ind);
		ippsFlip_32f_I(calib_index.raw_ptr(), raw2_size.width);
		dwTotalRead += dwRead;

		// calib_weight
		ReadFile(hCalibFile, calib_weight.raw_ptr(), sizeof(float) * raw2_size.width, &dwRead, NULL);
		ippsFlip_32f_I(calib_weight.raw_ptr(), raw2_size.width);
		dwTotalRead += dwRead;

		// dispersion compensation real
		Ipp32f* real = ippsMalloc_32f(raw2_size.width);
		ReadFile(hCalibFile, real, sizeof(float) * raw2_size.width, &dwRead, NULL);		
		ippsFlip_32f_I(real, raw2_size.width);
		dwTotalRead += dwRead;
		
		// dispersion compensation imag
		Ipp32f* imag = ippsMalloc_32f(raw2_size.width);
		ReadFile(hCalibFile, imag, sizeof(float) * raw2_size.width, &dwRead, NULL);
		ippsFlip_32f_I(imag, raw2_size.width);
		dwTotalRead += dwRead;

		// dispersion compensation
		ippsRealToCplx_32f(real, imag, (Ipp32fc*)dispersion.raw_ptr(), raw2_size.width);			
		ippsFree(real); ippsFree(imag);	
		ChangeDiscomValue(discom);

		CloseHandle(hCalibFile);
	}

	// 2-dimension
	//for (int i = 0; i < fft_size.height; i++)
	//{
	//	memcpy(&BG(fft_size.width * i), bg, sizeof(float) * raw_size.width); // BG 2D
	//	memcpy(&DISP(fft2_size.width * i), dispersion, sizeof(float) * raw2_size.width * 2); // DISP 2D
	//}
}





//// Operation ////
void OCTProcess::UnwrapPhase_32f(Ipp32f* p, int length)
{
	Ipp32f* dp = ippsMalloc_32f(length - 1);
	Ipp32f* dps = ippsMalloc_32f(length - 1);
	Ipp32f* dp_corr = ippsMalloc_32f(length - 1);
	Ipp32f* cumsum = ippsMalloc_32f(length - 1);

	Ipp32f _pi = (Ipp32f)IPP_PI;
	Ipp32f cutoff = (Ipp32f)IPP_PI;

	// incremental phase variation 
	// MATLAB: dp = diff(p,1,1);  
	for (int i = 0; i < length - 1; i++)
		dp[i] = p[i + 1] - p[i];

	// equivalent phase variation in [-pi, pi]
	// MATLAB: dps = mod(dp+pi,2*pi) - pi;
	for (int i = 0; i < length - 1; i++)
		dps[i] = (dp[i] + _pi) - floor((dp[i] + _pi) / (2 * _pi)) * (2 * _pi) - _pi;

	// preserve variation sign for +pi vs. -pi
	// MATLAB: dps(dps==-pi & dp>0,:) = pi;
	for (int i = 0; i < length - 1; i++)
		if ((dps[i] == -_pi) && (dp[i] > 0))
			dps[i] = _pi;

	// incremental phase correction
	// MATLAB: dp_corr = dps - dp;
	for (int i = 0; i < length - 1; i++)
		dp_corr[i] = dps[i] - dp[i];

	// Ignore correction when incremental variation is smaller than cutoff
	// MATLAB: dp_corr(abs(dp)<cutoff,:) = 0;
	for (int i = 0; i < length - 1; i++)
		if ((fabs(dp[i]) < cutoff))
			dp_corr[i] = 0;

	// Find cumulative sum of deltas
	// MATLAB: cumsum = cumsum(dp_corr, 1);
	cumsum[0] = dp_corr[0];
	for (int i = 1; i < length - 1; i++)
		cumsum[i] = cumsum[i - 1] + dp_corr[i];

	// Integrate corrections and add to P to produce smoothed phase values
	// MATLAB: p(2:m,:) = p(2:m,:) + cumsum(dp_corr,1);
	for (int i = 1; i < length; i++)
		p[i] += cumsum[i - 1];

	ippsFree(dp);
	ippsFree(dps);
	ippsFree(dp_corr);
	ippsFree(cumsum);

}


void OCTProcess::LineSpace_32f(Ipp32f x0, Ipp32f x1, int length, Ipp32f* dst)
{
	Ipp32f incre = (x1 - x0) / Ipp32f(length - 1);

	for (int i = 0; i < length; i++)
		dst[i] = x0 + i * incre;
}


void OCTProcess::Interpolation_32f(const Ipp32f* srcX, const Ipp32f* srcY, Ipp32f* dstX, int srcLen, int dstLen, Ipp32f* dstY)
{
	// This function is only valid when diff(srcX) is always larger than 0...
	// Also diff(dstX) should be either >0 or <0.

	Ipp32f srcX_0 = 0, srcX_1 = 0;
	Ipp32f srcY_0 = 0, srcY_1 = 0;
	int ii;
	Ipp32f w;

	BOOL isIncre = (dstX[1] > dstX[0]) ? TRUE : FALSE;
	BOOL isExist;

	for (int i = 0; i < dstLen; i++)
	{
		for (int j = 0; j < srcLen - 1; j++)
		{
			isExist = isIncre ? ((dstX[i] >= srcX[j]) && (dstX[i] <= srcX[j + 1])) : ((dstX[i] <= srcX[j]) && (dstX[i] >= srcX[j + 1]));
			if (isExist)
			{
				srcX_0 = srcX[j]; srcX_1 = srcX[j + 1];
				srcY_0 = srcY[j]; srcY_1 = srcY[j + 1];
				ii = j;
				break;
			}
		}

		w = (dstX[i] - srcX_0) / (srcX_1 - srcX_0);
		dstY[i] = (1 - w) * srcY_0 + w * srcY_1;
	}
}


void OCTProcess::LinearInterp_32fc(const Ipp32fc* src, Ipp32fc* dst, int src_len, int dst_len, Ipp32f* cal_ind, Ipp32f* cal_weight)
{
	for (int i = 0; i < dst_len; i++)
	{
		dst[i].re = cal_weight[i] * src[(int)cal_ind[i]].re + (1 - cal_weight[i]) * src[(int)cal_ind[i] + 1].re;
 		dst[i].im = cal_weight[i] * src[(int)cal_ind[i]].im + (1 - cal_weight[i]) * src[(int)cal_ind[i] + 1].im;
	}
}


void OCTProcess::LinearRegression_32f(const Ipp32f* pSrcX, const Ipp32f* pSrcY, int length, Ipp32f& offset, Ipp32f& slope)
{
	Ipp32f cov_xy, var_x;
	Ipp32f mean_x, mean_y;

	ippsMean_32f(pSrcX, length, &mean_x, ippAlgHintFast);
	ippsMean_32f(pSrcY, length, &mean_y, ippAlgHintFast);
	ippsDotProd_32f(pSrcX, pSrcY, length, &cov_xy);

	cov_xy = (cov_xy / (Ipp32f)length) - mean_x * mean_y;

	ippsDotProd_32f(pSrcX, pSrcX, length, &var_x);

	var_x = (var_x / (Ipp32f)length) - mean_x * mean_x;

	slope = cov_xy / var_x;
	offset = mean_y - slope * mean_x;

}


void OCTProcess::dumpErrorSystem(int res, LPCSTR pPreamble)
{
	char *pErr = nullptr;

	CString str; str.Format(_T("Error code (%d)"), res);
	AfxMessageBox(str);

	//EndBufferedPciAcquisitionPX14(_board);
	//if (dma_bufp) FreeDmaBufferPX14(_board, dma_bufp);
	//DisconnectFromDevicePX14(_board);
}