#include <catch.hpp>
#include <numcpp/array.h>

namespace {

using namespace np;

TEST_CASE("DeclareEmptyArray", "Array")
{
	Array<int> a0;

    CHECK(a0.itemSize() == sizeof(int));
	CHECK(a0.length() == 0);
	CHECK(a0.ndims() == 1);
	CHECK(a0.size() == make_array(0));
	CHECK(a0.strides() == make_array(1));
	CHECK(a0.raw_ptr() == nullptr);

	CHECK(empty(a0));
	CHECK(byteSize(a0) == 0);
}

TEST_CASE("DeclareArrayWithSize", "Array")
{
	Array<int> a1(5);

	CHECK(a1.itemSize() == sizeof(int));
	CHECK(a1.length() == 5);
	CHECK(a1.ndims() == 1);
	CHECK(a1.size(0) == 5);
	CHECK(a1.stride(0) == 1);
    CHECK(a1.raw_ptr() != nullptr); 

	CHECK_FALSE(empty(a1));
    CHECK(byteSize(a1) == 5 * sizeof(int));

	Array<float, 2> a2(2, 3);

	CHECK(a2.itemSize() == sizeof(float));
	CHECK(a2.length() == 2 * 3);
	CHECK(a2.ndims() == 2);
	CHECK(a2.size(0) == 2);
	CHECK(a2.size(1) == 3);
	CHECK(a2.stride(0) == 1);
	CHECK(a2.stride(1) == 2);
    CHECK(a2.raw_ptr() != nullptr); 

	CHECK_FALSE(empty(a2));
    CHECK(byteSize(a2) == 2 * 3 * sizeof(int));

	Array<double, 3> a3(make_array(2, 3, 4));

	CHECK(a3.itemSize() == sizeof(double));
	CHECK(a3.length() == 2 * 3 * 4);
	CHECK(a3.ndims() == 3);
	CHECK(a3.size(0) == 2);
	CHECK(a3.size(1) == 3);
	CHECK(a3.size(2) == 4);
	CHECK(a3.stride(0) == 1);
	CHECK(a3.stride(1) == 2);
	CHECK(a3.stride(2) == 6);
    CHECK(a3.raw_ptr() != nullptr); 

	CHECK_FALSE(empty(a3));
	CHECK(byteSize(a3) == 2 * 3 * 4 * sizeof(double));

	// COMPILE ERROR: This function is only for Array<T, 2>
	// Array<int, 3> a3_error(3, 4);
}

TEST_CASE("AccessElements", "Array")
{
	// 1d array
	np::Array<int> a1(5);

	const int data1[5] = 
	{ 
		2, 3, 5, 1, 7 
	};

	memcpy(a1.raw_ptr(), data1, 5 * sizeof(int));

	CHECK(a1.at(0) == 2);
	CHECK(a1.at(1) == 3);
	CHECK(a1.at(2) == 5);
	CHECK(a1(3) == 1);
	CHECK(a1[4] == 7);

	int *ptr = a1.raw_ptr();
	ptr[2] = 8;

	CHECK(a1(2) == 8);

	// COMPILE ERROR: Array dimension bounds error
	// a1(1, 1);

	// 2d array
	np::Array<int, 2> a2(3, 2);

	const int data2[6] = 
	{ 
		7, 2, 3, 
		4, 1, 8 
	};

	memcpy(a2.raw_ptr(), data2, 6 * sizeof(int));

	CHECK(a2(0) == 7);
	CHECK(a2(2) == 3);
	CHECK(a2(5) == 8);

	CHECK(a2.at(0, 0) == 7);
	CHECK(a2(1, 1) == 1);
	CHECK(a2.at(2, 1) == 8);

	a2.at(2, 0) = 5;
	CHECK(a2.at(2, 0) == 5);
}

TEST_CASE("SetSize", "Array")
{
	Array<int> a1;
	a1 = Array<int>(5);

	CHECK_FALSE(empty(a1));
	CHECK(a1.itemSize() == sizeof(int));
	CHECK(a1.ndims() == 1);
	CHECK(a1.size(0) == 5);
    CHECK(a1.raw_ptr() != nullptr); 

	Array<int, 2> a2;
	a2 = Array<int, 2>(3, 2);

	CHECK_FALSE(empty(a2));
	CHECK(a2.itemSize() == sizeof(float));
	CHECK(a2.ndims() == 2);
	CHECK(a2.size(0) == 3);
	CHECK(a2.size(1) == 2);
	CHECK(a2.raw_ptr() != nullptr); 

	Array<double, 3> a3;
	a3 = Array<double, 3>(make_array(4, 3, 2));

	CHECK_FALSE(empty(a3));
	CHECK(a3.itemSize() == sizeof(double));
	CHECK(a3.ndims() == 3);
	CHECK(a3.size(0) == 4);
	CHECK(a3.size(1) == 3);
	CHECK(a3.size(2) == 2);
	CHECK(a3.raw_ptr() != nullptr); 
}

TEST_CASE("MoveSemantics", "Array")
{
	// move constructor
	Array<int> a1(5);
	auto moved = std::move(a1);

	CHECK(empty(a1));
	CHECK(a1.raw_ptr() == nullptr);

	CHECK_FALSE(empty(moved));
	CHECK(moved.ndims() == 1);
	CHECK(moved.size(0) == 5);
    CHECK(moved.raw_ptr() != nullptr); 

	// move assign
	Array<int, 2> a2(3, 2);
	auto moved2 = std::move(a2);

	CHECK(empty(a2));
	CHECK(a2.raw_ptr() == nullptr);

	CHECK_FALSE(empty(moved2));
	CHECK(moved2.ndims() == 2);
	CHECK(moved2.size(0) == 3);
	CHECK(moved2.size(1) == 2);
	CHECK(moved2.raw_ptr() != nullptr); 
}

TEST_CASE("DerivedFunctions", "Array")
{
	Array<int> a0;

	CHECK(empty(a0));
	CHECK(a0.length() == 0);
	CHECK(byteSize(a0) == 0 * sizeof(int));

	Array<int> a1(5);

	CHECK_FALSE(empty(a1));
	CHECK(a1.length() == 5);
    CHECK(byteSize(a1) == 5 * sizeof(int));
	CHECK(a1.stride(0) == 1);

	Array<float, 2> a2(3, 2);

	CHECK_FALSE(empty(a2));
    CHECK(a2.length() == 3 * 2); 
	CHECK(byteSize(a2) == 3 * 2 * sizeof(float)); 
	CHECK(a2.stride(0) == 1);
	CHECK(a2.stride(1) == 3);

	Array<char, 3> a3(make_array(4, 3, 2));

	CHECK_FALSE(empty(a3));
    CHECK(a3.length() == 4 * 3 * 2);
	CHECK(byteSize(a3) == 4 * 3 * 2 * sizeof(char));
	CHECK(a3.stride(0) == 1);
	CHECK(a3.stride(1) == 4);
	CHECK(a3.stride(2) == 4 * 3);
}

TEST_CASE("Slice", "Array")
{
	// 1d array
	np::Array<int> a1(5);

	const int data1[5] = 
	{ 
		2, 3, 5, 1, 7 
	};

	memcpy(a1.raw_ptr(), data1, 5 * sizeof(int));

	auto slice1 = slice(a1, _colon(1, 4));

	CHECK(slice1.length() == 3);
	CHECK(slice1.ndims() == 1);
	CHECK(slice1.size(0) == 3);
	CHECK(slice1.stride(0) == 1);
	CHECK(slice1.raw_ptr() == &a1(1));

	// 2d array
	np::Array<int, 2> a2(3, 2);

	const int data2[6] = 
	{ 
		7, 2, 3, 
		4, 1, 8 
	};

	memcpy(a2.raw_ptr(), data2, 6 * sizeof(int));

	auto slice2 = slice(a2, _colon(), 1);
	CHECK(slice2.length() == 3);
	CHECK(slice2.ndims() == 1);
	CHECK(slice2.size(0) == 3);
	CHECK(slice2.stride(0) == 1);
    CHECK(slice2.raw_ptr() == &a2(0, 1));
}

} // anonymous namespace