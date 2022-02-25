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


#include "MW_AFXUnit_SVFilterTests.h"

static int32_t MW_AFXUnit_SVFilter_initializationTests()
{
  MW_AFXUnit_SVFilter filter;
  MW_AFXUnit_SVFilterType filterType = MW_AFXUNIT_SVFILTER_LPF;
  float32_t fs = 44100.f;
  float32_t fc = 500.f;
  float32_t Q = 0.707f;

  //  Start with invalid inputs for initialization
  //  SVFilter instance NULL
  int32_t success = MW_AFXUnit_SVFilter_init(NULL, filterType, fs, fc, Q);
  if (success)
    return 0;

  //  Invalid filter type
  success = MW_AFXUnit_SVFilter_init(&filter, -1, fs, fc, Q);
  if (success)
    return 0;

  //  Invalid fs
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, -fs, fc, Q);
  if (success)
    return 0;

  //  Invalid fc
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, -fc, Q);
  if (success)
    return 0;

  //  Invalid Q
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, -Q);
  if (success)
    return 0;

  //  Now properly initialize the filter instance
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

  if (filter.filterType != MW_AFXUNIT_SVFILTER_LPF)
    return 0;

  if (filter.internalStates[0] != 0.f && filter.internalStates[1] != 0.f)
    return 0;


  return 1;
}


static int32_t MW_AFXUnit_SVFilter_standardOperationTests()
{
  MW_AFXUnit_SVFilter filter;
  MW_AFXUnit_SVFilterType filterType = MW_AFXUNIT_SVFILTER_LPF;
  float32_t fs = 44100.f;
  float32_t fc = 500.f;
  float32_t Q = 0.707f;

  int32_t success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

#ifdef NO_OPTIMIZE
  /*  Attempt to change the filter parameters using invalid inputs
   *  This should cause the changeParameters() function to invalidate the filter instance
   *
   *  ie. Filter coefficients are set to -9999999.0 and filterType is set to MW_AFXUNIT_SVFILTER_INVALID_TYPE
   *
   *  Note that invalid input checks are only done if NO_OPTIMIZE is enabled!
   */

  //  Enter invalid filter type
  MW_AFXUnit_SVFilter_changeParameters(&filter, -1, fs, fc, Q);
  if (filterType != MW_AFXUNIT_SVFILTER_INVALID_TYPE)
    return 0;

  //  Reset the filter
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

  //  Enter invalid sampling frequency
  MW_AFXUnit_SVFilter_changeParameters(&filter, filterType, -fs, fc, Q);
  if (filterType != MW_AFXUNIT_SVFILTER_INVALID_TYPE)
    return 0;

  //  Reset the filter
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

  //  Enter invalid fc
  MW_AFXUnit_SVFilter_changeParameters(&filter, filterType, fs, -fc, Q);
  if (filterType != MW_AFXUNIT_SVFILTER_INVALID_TYPE)
    return 0;

  //  Reset the filter
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

  //  Enter invalid Q
  MW_AFXUnit_SVFilter_changeParameters(&filter, filterType, fs, fc, -Q);
  if (filterType != MW_AFXUNIT_SVFILTER_INVALID_TYPE)
    return 0;

  //  Reset the filter
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

  //  Attempt to process a buffer of samples with NULL as input
  MW_AFXUnit_SVFilter_process(&filter, NULL, 10);
  if (filterType != MW_AFXUNIT_SVFILTER_INVALID_TYPE)
    return 0;

  //  Reset the filter
  success = MW_AFXUnit_SVFilter_init(&filter, filterType, fs, fc, Q);
  if (!success)
    return 0;

#endif

  //  Swap the filter type from LPF to HPF
  MW_AFXUnit_SVFilter_changeParameters(&filter, MW_AFXUNIT_SVFILTER_HPF, fs, fc, Q);

  if (filter.filterType != MW_AFXUNIT_SVFILTER_HPF)
    return 0;

  return 1;
}



int32_t MW_AFXUnit_SVFilter_runUnitTests()
{

  if (!MW_AFXUnit_SVFilter_initializationTests())
    return 0;

  if (!MW_AFXUnit_SVFilter_standardOperationTests())
    return 0;

  return 1;
}
