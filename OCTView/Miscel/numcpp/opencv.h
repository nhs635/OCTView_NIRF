#ifndef NUMCPP_OPENCV_H_
#define NUMCPP_OPENCV_H_

#include "array.h"

#include <opencv2/opencv.hpp>

namespace np {

template <typename T>
cv::Mat to_cv_mat(Array<T, 2> &array)
{
	return cv::Mat(
		array.size(1), array.size(0), 
		cv::DataType<T>::type, 
		array.raw_ptr(), 
		array.stride(1) * array.itemSize());
}

template <typename T>
const cv::Mat to_cv_mat(const Array<T, 2> &array)
{
	return cv::Mat(
		array.size(1), array.size(0), 
		cv::DataType<T>::type, 
		const_cast<T *>(array.raw_ptr()), 
		array.stride(1) * array.itemSize());
}

template <typename T>
void from_cv_mat(Array<T, 2> &dst, const cv::Mat &src)
{
	// check type of array
	if (cv::DataType<T>::type != src.type())
	{
		// TODO: exception
		std::cout << "[from_cv_mat] Error: invalid array type" << std::endl;
		return;
	}

	if (dst.size() != make_array(src.cols, src.rows))
		dst = Array<T, 2>(src.cols, src.rows);

	memcpy(dst, src.data, byteSize(dst));
}

inline bool imread(Array<uint8_t, 2> &dst, const std::string &filename)
{
	cv::Mat cv_image = cv::imread(filename);
	if (cv_image.empty()) 
		return false;

	cv::Mat cv_grayscale;
	cv::cvtColor(cv_image, cv_grayscale, CV_BGR2GRAY);

	from_cv_mat(dst, cv_grayscale);
	return true;
}

template <typename T>
inline bool imwrite(const Array<T, 2> &image, const std::string &filename)
{
    // return cv::imwrite(filename, to_cv_mat(image));
    return cvSaveImage(filename.c_str(), &(IplImage(to_cv_mat(image))));
}

} // namespace np

#if 0

/*
#ifdef USE_CUDA
#include "cuda.h"
#include <opencv2/gpu/gpu.hpp>
#endif
*/

namespace np {

template <>
inline cv::Mat to_cv_mat(Array<cv::Vec3b> &array)
{
	return cv::Mat(
		array.size(1), array.size(0), 
		CV_8UC3, 
		array.raw_ptr(), 
		array.stride(1) * array.itemSize());
}

template <>
inline const cv::Mat to_cv_mat(const Array<cv::Vec3b> &array)
{
	return cv::Mat(
		array.size(1), array.size(0), 
		CV_8UC3, 
		const_cast<cv::Vec3b *>(array.raw_ptr()), 
		array.stride(1) * array.itemSize());
}

inline bool imwrite(const Array<uint8_t> &image, const std::string &filename)
{
	return cv::imwrite(filename, to_cv_mat(image));
}

inline bool imwrite(const Array<cv::Vec3b> &image, const std::string &filename)
{
	return cv::imwrite(filename, to_cv_mat(image));
}

// TODO: imwrite for other types?
inline bool imwrite(const TArray<uint8_t, 2> &image, const std::string &filename)
{
	return cv::imwrite(filename, to_cv_mat(image));
}

inline void imshow(const Array<uint8_t> &image)
{
	cv::imshow("image", to_cv_mat(image));
	cv::waitKey(0);
}

inline void imshow(const Array<cv::Vec3b> &image)
{
	cv::imshow("image", to_cv_mat(image));
	cv::waitKey(0);
}

} // namespace np

#endif // if 0

#endif // NUMCPP_OPENCV_H_