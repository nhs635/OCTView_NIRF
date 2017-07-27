#include <catch.hpp>
#include <numcpp/base_array.h>

namespace {

using namespace np;

TEST_CASE("DeclareEmptyBaseArray", "BaseArray")
{
	BaseArray a0;

	CHECK(a0.itemSize() == 1);
	CHECK(a0.length() == 0);
	CHECK(a0.ndims() == 0);
	CHECK(a0.size() == BaseArray::size_type());
	CHECK(a0.strides() == BaseArray::stride_type());
	CHECK(a0.raw_ptr() == nullptr);

	CHECK(empty(a0));
	CHECK(byteSize(a0) == 0);
}

TEST_CASE("DeclareBaseArrayWithSize", "BaseArray")
{
	BaseArray a1(sizeof(int), make_vector(5));

	CHECK(a1.itemSize() == sizeof(int));
	CHECK(a1.length() == 5);
	CHECK(a1.ndims() == 1);
	CHECK(a1.size(0) == 5);
	CHECK(a1.stride(0) == 1);
	CHECK(a1.raw_ptr() != nullptr);

	CHECK_FALSE(empty(a1));
	CHECK(byteSize(a1) == 5 * sizeof(int));

	BaseArray a2(sizeof(float), make_vector(2, 3));

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

	BaseArray a3(sizeof(double), make_vector(2, 3, 4));

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
}

} // anonymous namespace