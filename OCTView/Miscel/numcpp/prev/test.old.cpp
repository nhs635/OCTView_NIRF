#include <gtest/gtest.h>

TEST(Main, HelloGoogleTest)
{
	EXPECT_EQ(2, 1 + 1);
}

#include "config.h"
//#include "numcpp/base_array.test.inl"
#include "numcpp/array.test.inl"

#ifdef USE_OPENCV
#include "numcpp/opencv.test.inl"
#endif

#ifdef USE_CUDA
#include "numcpp/gpu_array.test.inl"
#endif

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}