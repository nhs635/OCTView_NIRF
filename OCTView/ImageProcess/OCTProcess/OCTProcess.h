#ifndef _OCT_PROCESS_H_
#define _OCT_PROCESS_H_

#include <complex>
#include <thread>
#include <Miscel\objcpp\signal.h>

struct FFT_R2C
{
public:
	FFT_R2C() : pFFTSpec(nullptr), pMemSpec(nullptr), pMemInit(nullptr), pMemBuffer(nullptr)
	{
	}

	~FFT_R2C()
	{
		if (pMemSpec) { ippsFree(pMemSpec); pMemSpec = nullptr; }
		if (pMemInit) { ippsFree(pMemInit); pMemInit = nullptr; }
		if (pMemBuffer) { ippsFree(pMemBuffer); pMemBuffer = nullptr; }
	}

	void operator() (Ipp32fc* dst, const Ipp32f* src, int line = 0)
	{
		ippsFFTFwd_RToPerm_32f(src, &temp(0, line), pFFTSpec, pMemBuffer);
		ippsConjPerm_32fc(&temp(0, line), dst, temp.size(0));
	}

	void initialize(int len, int line)
	{
		// init FFT spec
		const int ORDER = (int)(ceil(log2(len)));
		temp = np::Array<float, 2>(1 << ORDER, line);

		int sizeSpec, sizeInit, sizeBuffer;
		ippsFFTGetSize_R_32f(ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuffer);

		pMemSpec = ippsMalloc_8u(sizeSpec);
		pMemInit = ippsMalloc_8u(sizeInit);
		pMemBuffer = ippsMalloc_8u(sizeBuffer);

		ippsFFTInit_R_32f(&pFFTSpec, ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, pMemSpec, pMemInit);
	}

private:
	IppsFFTSpec_R_32f *pFFTSpec;
	Ipp8u *pMemSpec, *pMemInit, *pMemBuffer;

	np::Array<float, 2> temp;
};

struct FFT_C2C
{
	FFT_C2C() : pFFTSpec(nullptr),
		pMemSpec(nullptr), pMemInit(nullptr), pMemBuffer(nullptr), length(-1)
	{
	}

	~FFT_C2C()
	{
		if (pMemSpec) { ippsFree(pMemSpec); pMemSpec = nullptr; }
		if (pMemInit) { ippsFree(pMemInit); pMemInit = nullptr; }
		if (pMemBuffer) { ippsFree(pMemBuffer); pMemBuffer = nullptr; }
	}

	void forward(Ipp32fc* dst, const Ipp32fc* src) //(np::Array<std::complex<float>> &dst, const np::Array<std::complex<float>> &src)
	{
		ippsFFTFwd_CToC_32fc(src, dst, pFFTSpec, pMemBuffer);
	}

	void inverse(Ipp32fc* dst, const Ipp32fc* src) //(np::Array<std::complex<float>> &dst, const np::Array<std::complex<float>> &src)
	{
		ippsFFTInv_CToC_32fc(src, dst, pFFTSpec, pMemBuffer);
	}

	void initialize(int length)
	{
		this->length = length;
		const int ORDER = (int)(ceil(log2(length)));

		int sizeSpec, sizeInit, sizeBuffer;
		ippsFFTGetSize_C_32fc(ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuffer);

		pMemSpec = ippsMalloc_8u(sizeSpec);
		pMemInit = ippsMalloc_8u(sizeInit);
		pMemBuffer = ippsMalloc_8u(sizeBuffer);

		ippsFFTInit_C_32fc(&pFFTSpec, ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintNone, pMemSpec, pMemInit);
	}

private:
	IppsFFTSpec_C_32fc* pFFTSpec;
	Ipp8u *pMemSpec, *pMemInit, *pMemBuffer;

	int length;
};

class COCTViewDlg;

class OCTProcess
{
public:
	OCTProcess(const COCTViewDlg* pMainDlg);
	~OCTProcess();
	
	// Generate OCT image
	void operator() (float* img, uint16_t* fringe); // (np::Array<float, 2>& img, const np::Array<uint16_t, 2>& fringe);
	
	// For Calibration
	void SetBg(const np::Array<uint16_t, 2> &frame);
	void SetFringe(const np::Array<uint16_t, 2> &frame, int ch);
	void GenerateCalibration(int discom_val = 0, bool res_show = false);
	void ChangeDiscomValue(int discom_val = 0);
	void SaveCalibration();
	void LoadCalibration(CString bg_path = _T("bg.bin"), CString calib_path = _T("calibration.dat"), int discom = 0);
	
	// Get Calibration Data
	auto GetBg() { return bg.raw_ptr(); };
	auto GetFringe(int ch) { return &fringe(0, ch); };

private:
	// operational function
	void UnwrapPhase_32f(Ipp32f* p, int length);
	void LineSpace_32f(Ipp32f x0, Ipp32f x1, int length, Ipp32f* dst);
	void Interpolation_32f(const Ipp32f* srcX, const Ipp32f* srcY, Ipp32f* dstX, int srcLen, int dstLen, Ipp32f* dstY);
	void LinearInterp_32fc(const Ipp32fc* src, Ipp32fc* dst, int src_len, int dst_len, Ipp32f* cal_ind, Ipp32f* cal_weight);
	void LinearRegression_32f(const Ipp32f* pSrcX, const Ipp32f* pSrcY, int length, Ipp32f& offset, Ipp32f& slope);	

public: 
	// thread operation	
	signal<int> DidAcquireData; // signal	
	signal<void> DidStopData;

private:
	void run(); // run OCT Process (thread)	
	std::thread _thread; // thread object

public:
	bool _running;
	bool startOCTprocess();
	void stopOCTprocess();

public:
	FFT_R2C fft1;
	FFT_C2C fft2, fft3; // ifft, fft

public:
	// Main dialog pointer
	const COCTViewDlg* pMainDlg;

	// Size variables
	IppiSize raw_size, raw2_size;
	IppiSize fft_size, fft2_size;
	//IppiSize circ_size;

	// Calibration varialbes
	np::Array<float> bg;
	np::Array<float, 2> fringe; // d1, d2	
	np::Array<float> calib_index, calib_weight;
	np::Array<std::complex<float>> dispersion, discom, dispersion1;
	np::Array<float> win;

	//np::Array<float, 2> BG, WIN, SHIFT_MASK;
	//np::Array<std::complex<float>, 2> DISP;

	// OCT image processing buffer
	np::Array<float, 2> signal;
	np::Array<std::complex<float>, 2> complex_signal, complex_resamp;

	np::Array<std::complex<float>, 2> fft_complex, fft2_complex;
	np::Array<float, 2> fft2_linear;// , circ_temp;

private:
	void dumpErrorSystem(int res, LPCSTR pPreamble);
};

#endif