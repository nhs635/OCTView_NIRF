#ifndef NUMCPP_GPU_ARRAY_H_
#define NUMCPP_GPU_ARRAY_H_

#include "array.h"
#include <cuda_runtime.h>

#define CUDA_CALL(func) \
	{ \
		cudaError_t error = (func); \
		if (error != cudaSuccess) std::cerr << "CUDA Error: " << cudaGetErrorString(error) << std::endl; \
	}

namespace np {

struct cuda_allocator
{
	static std::shared_ptr<void> allocate(int size)
	{
		void *ptr = nullptr;
		CUDA_CALL(cudaMalloc(&ptr, size));

		return std::shared_ptr<void>(ptr, free);
	}

	static void free(void *ptr)
	{
		CUDA_CALL(cudaFree(ptr));
	}
};

template <typename T, int Dim = 1>
using GpuArray = Array<T, Dim, cuda_allocator>;

//template <typename T, size_t Dim = 1>
//struct GpuArray : public Array<T, Dim, cuda_allocator<T>>
//{
//public:
//	typedef T value_type;
//	typedef std::array<int, Dim> size_type;
//	typedef std::array<int, Dim> stride_type;
//
//public:
//	// inherited constructors
//	GpuArray() : Array() { }
//	GpuArray(const size_type &size) : Array(size) { }
//	GpuArray(int size0) : Array(size0) { }
//	GpuArray(int size0, int size1) : Array(size0, size1) { }
//
//	explicit GpuArray(const GpuArray &other) : Array(other) { }
//
//	const GpuArray &operator=(const GpuArray &other)
//	{
//		Array::operator=(other);
//		return *this;
//	}
//
//	GpuArray(Array &&other) : Array(other) { }
//
//	const GpuArray &operator=(GpuArray &&other)
//	{
//		Array::operator=(other);
//		return *this;
//	}
//
//	// TODO: constructor from host array
//};

template <typename T, int Dim>
void to_device(GpuArray<T, Dim> &dst_d, const Array<T, Dim> &src)
{
	// FIXME: dst_d.setSize()
	CUDA_CALL(cudaMemcpy(dst_d.raw_ptr(), src.raw_ptr(), byteSize(src), cudaMemcpyHostToDevice));
}

template <typename T, int Dim>
GpuArray<T, Dim> to_device(const Array<T, Dim> &host_array)
{
	GpuArray<T, Dim> result(host_array.size());
	to_device(result, host_array);

	return std::move(result);
}

template <typename T, int Dim>
void to_host(Array<T, Dim> &dst, const GpuArray<T, Dim> &src_d)
{
	// FIXME: dst.setSize()
	CUDA_CALL(cudaMemcpy(dst.raw_ptr(), src_d.raw_ptr(), byteSize(src_d), cudaMemcpyDeviceToHost));
}

} // namespace np

#endif // NUMCPP_GPU_ARRAY_H_