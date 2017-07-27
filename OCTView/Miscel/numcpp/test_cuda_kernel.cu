__global__ void vecAdd_kernel(const int *A, const int *B, int *C)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	C[idx] = A[idx] + B[idx];
}

void vecAdd(const int *A, const int *B, int *C, int N)
{
	vecAdd_kernel <<< 1, N >>> (A, B, C);
}