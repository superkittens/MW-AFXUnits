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


#include "MW_AFXUnit_MiscUtils.h"


/*
 *  Optimized sin and cos calculations
 *  (https://www.musicdsp.org/en/latest/Synthesis/10-fast-sine-and-cosine-calculation.html)
 * 
 *  First, initialize an intance of MW_AFXUnit_FastSine using MW_AFXUnit_Utils_FastSine_init
 *  To get the next sin/cos value, call MW_AFXUnit_Utils_FastSine_update
 *  The sin value can be found in s[1] while cos can be found in s[0]
 *
 *  Inputs:
 *    fastSine:   Instace of MW_AFXUnit_FastSine struct
 *    A:          Amplitude
 *    f:          Frequency
 *    fs:         Sampling frequency
 *
 *  Returns:
 *    0 if initialization unsuccessful
 *    1 otherwise
 */
int32_t MW_AFXUnit_Utils_FastSine_init(MW_AFXUnit_FastSine *fastSine, float32_t A, float32_t f, float32_t fs)
{
  if (fastSine == NULL)
    return 0;

  fastSine->fs = fs;
  fastSine->a = 2.f * arm_sin_f32(PI * f / fs);
  fastSine->s[0] = A;
  fastSine->s[1] = 0.f;
  fastSine->A = A;
}


/*
 *  Calculate the next sin/cos values
 *  To get the next sin/cos value, call MW_AFXUnit_Utils_FastSine_update
 *  The sin value can be found in s[1] while cos can be found in s[0]
 *
 *  Inputs:
 *    fastSine:   Instace of MW_AFXUnit_FastSine struct
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_Utils_FastSine_update(MW_AFXUnit_FastSine *fastSine)
{
  #ifdef NO_OPTIMIZE
  if (fastSine == NULL) while(1);
  #endif

  fastSine->s[0] = fastSine->s[0] - (fastSine->a * fastSine->s[1]);
  fastSine->s[1] = fastSine->s[1] + (fastSine->a * fastSine->s[0]);
}


/*
 *  Set the frequency of an existing fastSine struct
 *
 *  Inputs:
 *    fastSine:   Instace of MW_AFXUnit_FastSine struct
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_Utils_FastSine_setFrequency(MW_AFXUnit_FastSine *fastSine, float32_t f)
{
  #ifdef NO_OPTIMIZE
  if (fastSine == NULL)
  #endif

  fastSine->a = 2.f * arm_sin_f32(PI * f / fastSine->fs);
}


/*
 *  Write fade in/out envelope values to the array passed into the function
 *  NOTE:  This function WILL NOT allocate memory automatically for you.  You must pass in a preallocated array
 *
 *  Inputs:
 *    dest:     Pointer to preallocated array that will hold the envelope values
 *    size:     Number of points in the envelope (cannot be 0)
 *    fadeType: 0 to create a fade in envelope, 1 to create a fade out envelope
 *
 *  Returns:
 *    0 if initialization unsuccessful
 *    1 otherwise
 */
int32_t MW_AFXUnit_Utils_createFadeEnvelope(float32_t *dest, size_t size, uint32_t fadeType)
{
  if (dest == NULL || fadeType > 1 || size == 0)
    return 0;

  float32_t M = PI / (2.f * (float32_t)size);

  switch (fadeType)
  {
    case 0:
    {
      for (uint32_t i = 0; i < size; ++i)
        dest[i] = arm_sin_f32(i * M) * arm_sin_f32(i * M);
      break;
    }

    case 1:
    {
      for (uint32_t i = 0; i < size; ++i)
        dest[i] = arm_cos_f32(i * M) * arm_cos_f32(i * M);
      break;
    }

    default:
      return 0;
  }

  return 1;
}


/*
 *  Reset sinusoid back to 0 and A (sin, cos respectively)
 *
 *  Inputs:
 *    fastSine: Pointer to instance of MW_AFXUnit_FastSine
 * 
 *  Returns:
 *    None
 */
void MW_AFXUnit_Utils_FastSine_reset(MW_AFXUnit_FastSine *fastSine)
{
  if (fastSine == NULL) return;

  fastSine->s[0] = fastSine->A;
  fastSine->s[1] = 0.f;
}


/*
 *  Crossfade two blocks of data
 *  NOTE:  fade in/out envelopes must already be pre-allocated and pre-calculated using MW_AFXUnit_Utils_createFadeEnvelope()
 *
 *  Inputs:
 *    fadeInEnvelope:     Pre-allocated and pre-calculated fade in envelope
 *    fadeOutEnvelope:    Pre-allocated and pre-calculated fade out envelope
 *    fadeInData:         Data to fade in
 *    fadeOutData:        Data to fade out
 *    dest:               Array to write crossfaded result to
 *    numSamples:         Number of samples to crossfade (must match the size of the fade in/out envelopes and fade in/out data)
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_Utils_crossFade(float32_t *fadeInEnvelope, float32_t *fadeOutEnvelope, float32_t *fadeInData, float32_t *fadeOutData, float32_t *dest, uint32_t numSamples)
{
#ifdef NO_OPTIMIZE
  assert(fadeInEnvelope != NULL);
  assert(fadeOutEnvelope != NULL);
  assert(fadeInData != NULL);
  assert(fadeOutData != NULL);
#endif

  arm_mult_f32(fadeInEnvelope, fadeInData, fadeInData, numSamples);
  arm_mult_f32(fadeOutEnvelope, fadeOutData, fadeOutData, numSamples);
  arm_add_f32(fadeInData, fadeOutData, dest, numSamples);
}


float32_t MW_AFXUnit_Utils_mapToRange(float32_t value, float32_t low1, float32_t high1, float32_t low2, float32_t high2)
{
  return low2 + ((value - low1) * (high2 - low2) / (high1 - low1));
}








