#include "circularize.h"
#include <iostream>

int main(int argc, char *argv[])
{
	using namespace std;
	using namespace np;

	auto image = imread("Lena.bmp");
	if (image.empty())
	{
		cout << "Cannot read image file!" << endl;
		return -1;
	}

	const int DIAMETER = 1024;
	Array<uint8_t> result_image;

	Circularize circularize;

	// Run CPU version
	cout << "Running CPU version..." << endl;
	circularize(result_image, image, DIAMETER);
	cout << "Finished!" << endl;

	imshow(result_image);

/*
#ifdef USE_CUDA

	// Run GPU version
	Circularize_d circularize_d;

	cout << "Running GPU version..." << endl;
	GpuArray<uint8_t> image_d(image), result_image_d;
	circularize_d(result_image_d, image_d, DIAMETER);

	Array<uint8_t> result_image_h(DIAMETER, DIAMETER);
	to_host(result_image_h, result_image_d);
	cout << "Finished!" << endl;

	imshow(result_image_h); 

#endif // USE_CUDA
*/
	
	return 0;
}