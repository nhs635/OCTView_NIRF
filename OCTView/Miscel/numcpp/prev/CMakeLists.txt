cmake_minimum_required(VERSION 3.0)

project(numcpp)

# Conditional build with external dependencies
set(USE_OPENCV ON CACHE BOOL "Use OpenCV")
set(USE_CUDA ON CACHE BOOL "Use CUDA")
set(BUILD_TEST ON CACHE BOOL "Build unit tests")
set(BUILD_EXAMPLE OFF CACHE BOOL "Build examples")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

# Project header directory
include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")

# Compiler option for gcc
if (${CMAKE_COMPILER_IS_GNUCXX})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
endif (${CMAKE_COMPILER_IS_GNUCXX})

# Compiler options for VC++
if (${MSVC})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS /wd4819")
endif (${MSVC})

# numcpp library
add_library(numcpp
	numcpp.cpp

	numcpp/allocator.h
	numcpp/base_array.h
	numcpp/array.h
	numcpp/array_functions.h
	numcpp/gpu_array.h
	numcpp/opencv.h)

# External library: OpenCV
if (USE_OPENCV)
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/opencv")
	set(OpenCV_STATIC OFF)

	find_package(OpenCV REQUIRED)
	target_link_libraries(numcpp ${OpenCV_LIBS})
endif (USE_OPENCV)

# External library: NVIDIA CUDA
if (USE_CUDA)
	find_package(CUDA REQUIRED)
	include_directories(${CUDA_INCLUDE_DIRS})
	target_link_libraries(numcpp ${CUDA_LIBRARIES})

	if (${UNIX})
		# Use C++03 compiler, not C++11
		set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-Xcompiler -std=c++03)
	endif (${UNIX})
endif (USE_CUDA)

if (BUILD_TEST)
    if (USE_OPENCV)
        set(ADDITIONAL_TESTS "numcpp/opencv.test.cpp ${ADDITIONAL_TESTS}")
    endif (USE_OPENCV)

	if (USE_CUDA)
		cuda_compile(TEST_CUDA_KERNEL_O numcpp/test_cuda_kernel.cu)
        set(ADDTIONAL_TESTS "numcpp/cuda.test.cpp ${TEST_CUDA_KERNEL_O} ${ADDITIONAL_TESTS}")
	endif (USE_CUDA)

	# test executable
	add_executable(numcpp.test 
		test.cpp

		numcpp/base_array.test.cpp
		numcpp/array.test.cpp
        ${ADDITIONAL_TESTS})

	target_link_libraries(numcpp.test numcpp)

	# Copy files for test
	file(COPY images/Lena.bmp DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
endif (BUILD_TEST)

if (BUILD_EXAMPLE)
	# Example: circularize
	if (USE_OPENCV)
		add_executable(circularize example/circularize.h example/circularize.cpp)
		target_link_libraries(circularize numcpp)
	endif (USE_OPENCV)
endif (BUILD_EXAMPLE)