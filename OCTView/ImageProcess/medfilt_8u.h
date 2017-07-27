#ifndef _MEDFILT_H_
#define _MEDFILT_H_

class medfilt_8u
{
public:
	medfilt_8u()
	{
	};

	medfilt_8u(int _width, int _height, int _kernel) :
		width(_width), height(_height), kernel(_kernel)
	{ 
		RoiSize = { width, height };
		MaskSize = { kernel, kernel };
		ippiFilterMedianBorderGetBufferSize(RoiSize, MaskSize, ipp32f, 1, &BufferSize);
		MemBuffer = np::Array<uint8_t>(BufferSize);
		DstBuffer = np::Array<uint8_t, 2>(width, height);
	};

	~medfilt_8u()
	{	
	};

	void operator() (Ipp8u* pSrcDst)
	{
		ippiFilterMedianBorder_8u_C1R(pSrcDst, sizeof(Ipp8u) * RoiSize.width, DstBuffer, sizeof(Ipp8u) * RoiSize.width, RoiSize, MaskSize, ippBorderRepl, 0, MemBuffer);
		memcpy(pSrcDst, DstBuffer, sizeof(Ipp8u) * width * height);
	};


private:
	int width, height, kernel;
	IppiSize RoiSize, MaskSize;
	Ipp32s BufferSize;
	np::Array<uint8_t> MemBuffer;
	np::Array<uint8_t, 2> DstBuffer;

};

#endif
