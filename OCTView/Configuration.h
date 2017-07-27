#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define GET_NEAR_2_POWER(x)   1 << (int)ceil(log2(x))

#define POWER_2_15			  32768.0f
#define POWER_2_16		      65536.0f

//#define DSP_COMP_DC           20
//#define PIXEL_SIZE		      5.3715 / 1000 // mm/pixel
//#define NUM_BUFFER_FRAME	  50

#define LUT_GRAY			  0
#define LUT_INV_GRAY		  1
#define LUT_SEPIA			  2
#define LUT_JET			      3
#define LUT_PARULA			  4
#define LUT_HOT			      5
#define LUT_FIRE			  6
#define LUT_PURPLE			  7
#define LUT_BLUE			  8
#define LUT_GREEN			  9

#endif