//  Copyright 2021 Allen Lee
//
//  Author:  Allen Lee (alee@meoworkshop.org)
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  For more information, please refer to https://opensource.org/licenses/mit-license.php
//
//  ------------------------------------------------------------------------------------------------  //

#ifndef MW_AFXUNIT_SVFILTER_H_
#define MW_AFXUNIT_SVFILTER_H_

#include "arm_math.h"

#define MW_AFXUNIT_SVFILTER_NUM_STATES 2
#define MW_AFXUNIT_SVFILTER_NUM_COEFFS 3


typedef enum
{
  MW_AFXUNIT_SVFILTER_LPF = 0,
  MW_AFXUNIT_SVFILTER_HPF,
  MW_AFXUNIT_SVFILTER_BPF,
  MW_AFXUNIT_SVFILTER_BSF,
  MW_AFXUNIT_SVFILTER_NUM_TYPES,
  MW_AFXUNIT_SVFILTER_INVALID_TYPE
}MW_AFXUnit_SVFilterType;


typedef struct
{
  MW_AFXUnit_SVFilterType   filterType;
  float32_t                 internalStates[MW_AFXUNIT_SVFILTER_NUM_STATES];
  float32_t                 filterCoefficients[MW_AFXUNIT_SVFILTER_NUM_COEFFS];
}MW_AFXUnit_SVFilter;


int32_t   MW_AFXUnit_SVFilter_init(MW_AFXUnit_SVFilter *filter, MW_AFXUnit_SVFilterType filterType, float32_t fs, float32_t fc, float32_t Q);
void      MW_AFXUnit_SVFilter_changeParameters(MW_AFXUnit_SVFilter *filter, MW_AFXUnit_SVFilterType filterType, float32_t fs, float32_t fc, float32_t Q);
void      MW_AFXUnit_SVFilter_process(MW_AFXUnit_SVFilter *filter, float32_t *buffer, size_t bufferSize);
void      MW_AFXUnit_SVFilter_reset(MW_AFXUnit_SVFilter *filter);

#endif /* MW_AFXUNIT_SVFILTER_H_ */
