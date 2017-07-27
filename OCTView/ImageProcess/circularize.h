#ifndef _CIRCULARIZE_H_
#define _CIRCULARIZE_H_


#include <chrono>
#include "tbb\parallel_for.h"
#include "tbb\blocked_range.h"

class circularize
{
public:
	circularize()
	{
	};

	circularize(int _radius, int _alines, bool half = true)
	{
		radius = _radius;
		alines = _alines;

		np::Array<float, 2> x_map, y_map;

		if (half)
		{
			// Generate Cicularize Map
			diameter = radius;
			x_map = np::Array<float, 2>(diameter, diameter);
			y_map = np::Array<float, 2>(diameter, diameter);

			Ipp32f* horizontal_line = ippsMalloc_32f(diameter);
			Ipp32f* vertical_line = ippsMalloc_32f(diameter);
			ippsVectorSlope_32f(horizontal_line, diameter, (Ipp32f)+radius, -2.0f);

			for (int i = 0; i < diameter; i++)
			{
				ippsSet_32f((float)(2 * i - radius), vertical_line, diameter);
				memcpy(x_map.raw_ptr() + i * diameter, horizontal_line, sizeof(float) * diameter);
				memcpy(y_map.raw_ptr() + i * diameter, vertical_line, sizeof(float) * diameter);
			}
			ippFree(horizontal_line);
			ippFree(vertical_line);
		}
		else
		{
			// Generate Cicularize Map
			diameter = 2 * radius;
			x_map = np::Array<float, 2>(diameter, diameter);
			y_map = np::Array<float, 2>(diameter, diameter);

			Ipp32f* horizontal_line = ippsMalloc_32f(diameter);
			Ipp32f* vertical_line = ippsMalloc_32f(diameter);
			ippsVectorSlope_32f(horizontal_line, diameter, (Ipp32f)+radius, -1.0f);

			for (int i = 0; i < diameter; i++)
			{
				ippsSet_32f((float)(i - radius), vertical_line, diameter);
				memcpy(x_map.raw_ptr() + i * diameter, horizontal_line, sizeof(float) * diameter);
				memcpy(y_map.raw_ptr() + i * diameter, vertical_line, sizeof(float) * diameter);
			}
			ippFree(horizontal_line);
			ippFree(vertical_line);
		}

		// Rho : Interpolation Map
		rho = np::Array<float, 2>(diameter, diameter);
		ippsMagnitude_32f(x_map, y_map, rho, diameter * diameter);
		ippsMulC_32f_I(((Ipp32f)radius - 1.0f) / radius, rho, diameter * diameter);

		// Theta : Interpolation Map
		theta = np::Array<float, 2>(diameter, diameter);
		ippsPhase_32f(x_map, y_map, theta, diameter * diameter);
		ippsMulC_32f_I(-1.0f, theta, diameter * diameter);
		ippsAddC_32f_I((Ipp32f)IPP_PI, theta, diameter * diameter);
		ippsMulC_32f_I(((Ipp32f)alines - 1.0f) / (Ipp32f)IPP_2PI, theta, diameter * diameter);


		//Imshow imshow;
		//imshow(rho.raw_ptr(), rho.size(0), rho.size(1), -1, 0);
		//imshow(theta.raw_ptr(), theta.size(0), theta.size(1), -1, 0);
	};

	~circularize()
	{
	};

public:
	void operator() (np::Array<float, 2>& rect_im, np::Array<float, 2>& circ_im, int offset = 0)
	{
		//std::chrono::system_clock::time_point start1 = std::chrono::system_clock::now();

		IppiSize srcSize = { radius, alines }; // width * height
		IppiRect srcRoi = { 0, 0, radius, alines };
		IppiSize dstRoiSize = { diameter, diameter };

		ippiRemap_32f_C1R(&rect_im(offset, 0), srcSize, sizeof(Ipp32f) * rect_im.size(0), srcRoi,
			rho, sizeof(Ipp32f) * dstRoiSize.width, theta, sizeof(Ipp32f) * dstRoiSize.width,
			circ_im.raw_ptr(), sizeof(Ipp32f) * dstRoiSize.width, dstRoiSize, IPPI_INTER_LINEAR);

		//std::chrono::system_clock::time_point end1 = std::chrono::system_clock::now();
		//std::chrono::milliseconds mil1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
		//printf("elapsed time : %d msec\n", (int)mil1.count());
	};

	void operator() (np::Array<uint8_t, 2>& rect_im, uint8_t* circ_im, int offset = 0)
	{
		//std::chrono::system_clock::time_point start1 = std::chrono::system_clock::now();

		IppiSize srcSize = { radius, alines }; // width * height
		IppiRect srcRoi = { 0, 0, radius, alines };
		IppiSize dstRoiSize = { diameter, diameter };

		ippiRemap_8u_C1R(&rect_im(offset, 0), srcSize, sizeof(Ipp8u) * rect_im.size(0), srcRoi,
			rho, sizeof(Ipp32f) * dstRoiSize.width, theta, sizeof(Ipp32f) * dstRoiSize.width,
			circ_im, sizeof(Ipp8u) * dstRoiSize.width, dstRoiSize, IPPI_INTER_LINEAR);

		//std::chrono::system_clock::time_point end1 = std::chrono::system_clock::now();
		//std::chrono::milliseconds mil1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
		//printf("elapsed time : %d msec\n", (int)mil1.count());
	};

private:
	int alines, radius, diameter;
	np::Array<float, 2> rho;
	np::Array<float, 2> theta;
};

#endif


