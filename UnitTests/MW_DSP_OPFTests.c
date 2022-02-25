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


#include "MW_DSP_OPFTests.h"


static int32_t MW_DSP_OPF_initializationTests()
{
  MW_DSP_OPF filter;
  float32_t b0 = 1.f;
  float32_t a1 = -0.8f;

  //  Try to pass in invalid inputs into initialization
  //  Pass in nullptr for filter instance input
  int32_t success = MW_DSP_OPF_init(NULL, b0, a1);
  if (success)
    return 0;

  //  Initialize filter with valid parameters
  success = MW_DSP_OPF_init(&filter, b0, a1);
  if (!success)
    return 0;

  return 1;
}


static int32_t MW_DSP_OPF_standardOperationTests()
{
  MW_DSP_OPF filter;
  float32_t b0 = 1.f;
  float32_t a1 = 0.f;

  int32_t success = MW_DSP_OPF_init(&filter, b0, a1);
  if (!success)
    return 0;

#ifdef NO_OPTIMIZE

  //  Pass in invalid parameters to the tick() and peek() functions
  float32_t sample = MW_DSP_OPF_tick(NULL, 0);
  if (sample != 0)
    return 0;

  //  Attempt to pass in nullptr to buffer pointer in process()
  MW_DSP_OPF_process(&filter, NULL, 0);
  if (filter.b0 != -9999999.f)
    return 0;

  if (filter.a1 != -9999999.f)
    return 0;

#endif

  return 1;
}


int32_t MW_DSP_OPF_runUnitTests()
{
  if (!MW_DSP_OPF_initializationTests())
    return 0;

  if (!MW_DSP_OPF_standardOperationTests())
    return 0;

  return 1;
}
