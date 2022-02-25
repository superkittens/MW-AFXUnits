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


#include "MW_AFXUnit_MiscUtilsTests.h"


static int32_t MW_AFXUnit_MiscUtils_crossFadeInitializationTests()
{
  float32_t fadeInEnvelope[16];
  float32_t fadeOutEnvelope[16];
  size_t N = 16;

  //  Run invalid input checks (nullptr, size 0 and invalid fadeType)
  int32_t success = MW_AFXUnit_Utils_createFadeEnvelope(NULL, N, 0);
  if (success)
    return 0;

  success = MW_AFXUnit_Utils_createFadeEnvelope(fadeInEnvelope, 0, 0);
  if (success)
    return 0;

  success = MW_AFXUnit_Utils_createFadeEnvelope(fadeInEnvelope, N, 3);
  if (success)
    return 0;

  //  Now perform a valid call
  success = MW_AFXUnit_Utils_createFadeEnvelope(fadeInEnvelope, N, 0);
  if (!success)
    return 0;

  success = MW_AFXUnit_Utils_createFadeEnvelope(fadeOutEnvelope, N, 1);
  if (!success)
    return 0;

  return 1;
}


static int32_t MW_AFXUnit_MiscUtils_crossFadeStandardOperationTests()
{
  float32_t fadeInEnvelope[16];
  float32_t fadeOutEnvelope[16];
  float32_t fadeInData[16];
  float32_t fadeOutData[16];
  size_t N = 16;
  float32_t epsilon = 0.01f;

  arm_fill_f32(1.f, fadeInData, N);
  arm_fill_f32(1.f, fadeOutData, N);

  int32_t success = MW_AFXUnit_Utils_createFadeEnvelope(fadeInEnvelope, N, 0);
  if (!success)
    return 0;

  success = MW_AFXUnit_Utils_createFadeEnvelope(fadeOutEnvelope, N, 1);
  if (!success)
    return 0;

  MW_AFXUnit_Utils_crossFade(fadeInEnvelope, fadeOutEnvelope, fadeInData, fadeOutData, fadeOutData, N);

  //  Crossfading all ones should result in a crossfaded result of ones
  //  Since we're dealing with floating point numbers, we might not get precisely 1.0 which is why we compare between (1 - epsilon, 1 + epsilon)
  for (uint32_t i = 0; i < N; ++i)
  {
    float32_t data = fadeOutData[i];
    if (data < (1.f - epsilon) || data > (1.f + epsilon))
      return 0;
  }

  return 1;
}


int32_t MW_AFXUnit_MiscUtils_runUnitTests()
{
  if (!MW_AFXUnit_MiscUtils_crossFadeInitializationTests())
    return 0;

  if (!MW_AFXUnit_MiscUtils_crossFadeStandardOperationTests())
    return 0;

  return 1;
}
