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


#include "MW_DSP_OPF.h"



static void MW_DSP_OPF_invalidateOPF(MW_DSP_OPF *filter)
{
  filter->b0 = -9999999.f;
  filter->a1 = -9999999.f;
}



/*
 *  Initialize MW_DSP_OPF filter instance
 *  NOTE:   a1 (filter feedback gain) is not subtracted from the filter input by default.  Be careful that you don't pass in
 *          a positive value while expecting it to be negative in the filter operation lest you encounter saturation.
 *
 *  Inputs:
 *    filter:   MW_DSP_OPF filter instance
 *    b0:       OPF filter input gain
 *    a1:       OPF filter feedback gain
 *
 *  Returns:
 *    0 if initialization unsuccessful
 *    1 if initialization successful
 */
int32_t MW_DSP_OPF_init(MW_DSP_OPF *filter, float32_t b0, float32_t a1)
{
  if (filter == NULL)
    return 0;

  filter->b0 = b0;
  filter->a1 = a1;

  return 1;
}


/*
 *  Execute one cycle of filter operation
 *
 *  Inputs:
 *    filter:   MW_DSP_OPF filter instance
 *    x:        Sample to feed into the filter
 *
 *  Returns:
 *    Filtered sample value
 *    If NO_OPTIMIZE is enabled and a nullptr is passed into the filter parameter, the function will return 0
 */
float32_t MW_DSP_OPF_tick(MW_DSP_OPF *filter, float32_t x)
{
#ifdef NO_OPTIMIZE
  if (filter == NULL)
    return 0;
#endif

  filter->stateVariable = (filter->stateVariable * filter->a1) + (x * filter->b0);
  return filter->stateVariable;
}


/*
 *  Process a buffer of samples
 *
 *  Inputs:
 *    filter:     MW_DSP_OPF filter instance
 *    buffer:     Pointer to a buffer of sample data
 *    bufferSize: Number of samples to process
 *
 *  Returns:
 *    Buffer of processed data (written to the input buffer)
 *    If NO_OPTIMIZE is enabled and a nullptr is passed into the buffer parameter, the function will invalidate the filter coefficients
 *    ie.  Set the filter coefficients to -9999999
 */
void MW_DSP_OPF_process(MW_DSP_OPF *filter, float32_t *buffer, size_t bufferSize)
{
#ifdef NO_OPTIMIZE

  if (filter == NULL)
    return;

  if (buffer == NULL)
  {
    MW_DSP_OPF_invalidateOPF(filter);
    return;
  }

#endif


  for (uint32_t i = 0; i < bufferSize; ++i)
  {
    buffer[i] = (filter->stateVariable * filter->a1) + (buffer[i] * filter->b0);
    filter->stateVariable = buffer[i];
  }
}



