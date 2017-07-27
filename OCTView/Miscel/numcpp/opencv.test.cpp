#include <catch.hpp>
#include <numcpp/opencv.h>

namespace {

using namespace np;

TEST_CASE("HelloOpenCV", "OpenCV")
{
	cv::Mat image = cv::imread("Lena.bmp");
	CHECK(512 == image.cols);
	CHECK(512 == image.rows);

	// cv::imshow("Hello OpenCV", image);
	// cv::waitKey(0);
}

TEST_CASE("ToCvMat", "OpenCV")
{
	// uint_t
	Array<uint8_t, 2> mat(5, 8);
	for (int i = 0; i < mat.length(); i++)
		mat(i) = 128;
	mat(2, 3) = 255;
	mat(4, 1) = 0;

	cv::Mat cv_mat = to_cv_mat(mat);
	CHECK(mat.size(0) == cv_mat.cols);
	CHECK(mat.size(1) == cv_mat.rows);
	CHECK(cv_mat.at<uint8_t>(3, 2) == 255);
	CHECK(cv_mat.at<uint8_t>(1, 4) == 0);
	for (int y = 0; y < mat.size(0); y++)
		for (int x = 0; x < mat.size(1); x++)
			CHECK(mat(y, x) == cv_mat.at<uint8_t>(x, y));

	// float
	Array<float, 2> mat2(5, 8);
	for (int i = 0; i < mat2.length(); i++)
		mat2(i) = 0.5f;
	//fill(mat2, 0.5f);
	mat2(2, 3) = 1.0f;
	mat2(4, 1) = 0.0f;

	cv::Mat cv_mat2 = to_cv_mat(mat2);
	CHECK(mat2.size(0) == cv_mat2.cols);
	CHECK(mat2.size(1) == cv_mat2.rows);
	CHECK(cv_mat2.at<float>(3, 2) == 1.0f);
	CHECK(cv_mat2.at<float>(1, 4) == 0.0f);
	for (int y = 0; y < mat2.size(0); y++)
		for (int x = 0; x < mat2.size(1); x++)
			CHECK(mat2(y, x) == cv_mat2.at<float>(x, y));
}

TEST_CASE("FromCvMatVoid", "OpenCV")
{
	cv::Mat cv_mat(5, 8, CV_8U, cvScalar(128));
	cv_mat.at<uint8_t>(3, 2) = 123;
	cv_mat.at<uint8_t>(1, 4) = 7;

	// uint8_t
	Array<uint8_t, 2> mat;
	from_cv_mat(mat, cv_mat);
	CHECK(mat.size(0) == cv_mat.cols);
	CHECK(mat.size(1) == cv_mat.rows);
	for (int y = 0; y < mat.size(1); y++)
		for (int x = 0; x < mat.size(0); x++)
			CHECK(mat(x, y) == cv_mat.at<uint8_t>(y, x));
	CHECK(mat(2, 3) == 123);
	CHECK(mat(4, 1) == 7);

	// wrong type of array
	Array<double, 2> mat_error;
	from_cv_mat(mat_error, cv_mat);
	CHECK(empty(mat_error));

	// float
	cv::Mat cv_mat2(5, 8, CV_32F, cvScalar(0.5f));
	cv_mat2.at<float>(3, 2) = 1.0f;
	cv_mat2.at<float>(1, 4) = 0.0f;

	Array<float, 2> mat2;
	from_cv_mat(mat2, cv_mat2);
	CHECK(mat2.size(0) == cv_mat2.cols);
	CHECK(mat2.size(1) == cv_mat2.rows);
	for (int y = 0; y < mat2.size(1); y++)
		for (int x = 0; x < mat2.size(0); x++)
			CHECK(mat2(x, y) == cv_mat2.at<float>(y, x));
}

TEST_CASE("ImRead", "OpenCV")
{
	cv::Mat cv_image = cv::imread("Lena.bmp");
	CHECK(512 == cv_image.rows);
	CHECK(512 == cv_image.cols);

	cv::Mat cv_grayscale;
	cv::cvtColor(cv_image, cv_grayscale, CV_BGR2GRAY);

	Array<uint8_t, 2> image;
	bool result = imread(image, "Lena.bmp");
	CHECK(result);
	CHECK(image.size(0) == cv_image.rows);
	CHECK(image.size(1) == cv_image.cols);

	CHECK(image(412, 43) == cv_grayscale.at<uint8_t>(43, 412));
	CHECK(image(240, 360) == cv_grayscale.at<uint8_t>(360, 240));
	CHECK(image(0, 0) == cv_grayscale.at<uint8_t>(0, 0));
	CHECK(image(511, 511) == cv_grayscale.at<uint8_t>(511, 511));
}

TEST_CASE("ImWrite", "OpenCV")
{
	Array<uint8_t, 2> image;
	bool result = imread(image, "Lena.bmp");
	CHECK(result);
	// imshow(image);
	result = imwrite(image, "result_imwrite.bmp");
	CHECK(result);
}

} // anonymous namespace