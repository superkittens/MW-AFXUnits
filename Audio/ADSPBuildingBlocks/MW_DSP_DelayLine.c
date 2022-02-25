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


#include "MW_DSP_DelayLine.h"



/*
 *  MW_DSP_DelayLine_init will not allocate memory for you!
 *  You must have a place in memory set aside to contain the delay line samples
 *
 *  Use MW_DSP_DelayLine_memalloc() if you want the delay line buffer to be allocated for you
 *
 *  Inputs:
 *    delayLine:    Pointer to a MS_DSP_DelayLine structure
 *    bufferMemory: Pointer to array that will hold the audio samples.  Must be already allocated to hold N samples
 *    N:            Delay line length
 *
 *  Returns:
 *    0 if initialization unsuccessful
 *    1 if initialization successful
 */
int32_t MW_DSP_DelayLine_init(MW_DSP_DelayLine *delayLine, float32_t *bufferMemory, size_t N)
{
  if (delayLine == NULL || bufferMemory == NULL || N == 0)
    return 0;

  delayLine->buffer = bufferMemory;
  delayLine->N = N;
  delayLine->currentPtr = 0;

  arm_fill_f32(0.0, delayLine->buffer, delayLine->N);

  delayLine->memoryDynamicallyAllocated = 0;

  return 1;
}


/*
 *  MW_DSP_DelayLine_init_memalloc is the same as MW_DSP_DelayLine_init() except that it will
 *  dynamically allocate memory for the delay line buffer for you
 *
 *  Inputs:
 *    delayLine:    Pointer to a MS_DSP_DelayLine structure
 *    bufferMemory: Pointer to array that will hold the audio samples.  Must be already allocated
 *    N:            Delay length
 *
 *  Returns:
 *    0 if initialization unsuccessful
 *    1 if initialization successful
 */
int32_t MW_DSP_DelayLine_init_memalloc(MW_DSP_DelayLine **delayLine, size_t N)
{
  if (delayLine == NULL || N == 0)
    return 0;

  MW_DSP_DelayLine *d = (MW_DSP_DelayLine*)malloc(sizeof(MW_DSP_DelayLine));
  if (d == NULL)
    return 0;

  *delayLine = d;

  (*delayLine)->buffer = (float32_t*)malloc(sizeof(float32_t) * N);
  if ((*delayLine)->buffer == NULL)
  {
    free(*delayLine);
    return 0;
  }

  arm_fill_f32(0.f, (*delayLine)->buffer, N);

  (*delayLine)->N = N;
  (*delayLine)->currentPtr = 0;
  (*delayLine)->memoryDynamicallyAllocated = 1;

  return 1;
}


/*
 *  Delete the DelayLine object IF the DelayLine object was created using MW_DSP_DelayLine_init_memalloc()
 *  Attempting to delete a DelayLine object created using MW_DSP_DelayLine_init() will cause the function to return an error
 *
 *  Inputs:
 *    delayLine:  Pointer to DelayLine object to delete
 *
 *  Returns:
 *    1 if delete successful.  delayLine will be set to NULL
 *    0 otherwise
 */
int32_t MW_DSP_DelayLine_delete(MW_DSP_DelayLine **delayLine)
{
  if ((*delayLine)->memoryDynamicallyAllocated == 0)
    return 0;

  free((*delayLine)->buffer);
  (*delayLine)->buffer = NULL;
  free(*delayLine);

  *delayLine = NULL;

  return 1;
}


/*
 *  tick() receives an input and returns the next output sample
 *  A nullptr check is omitted to avoid slowdowns due to branches so beware!
 *
 *  Inputs:
 *    delayLine:  Pointer to MW_DSP_DelayLine structure (must be previously initialized)
 *    x:          Sample to feed into the delay line
 *
 *  Returns:
 *    Sample delayed by N cycles
 *    (NO_OPTIMIZE) NAN if delayLine is NULL
 *
 */
float32_t MW_DSP_DelayLine_tick(MW_DSP_DelayLine *delayLine, float32_t x)
{
#ifdef NO_OPTIMIZE
  if (delayLine == NULL)
    return NAN;

  if (delayLine->N == 0 || delayLine->buffer == NULL)
    return NAN;
#endif

  float32_t y = delayLine->buffer[delayLine->currentPtr];
  delayLine->buffer[delayLine->currentPtr] = x;

  delayLine->currentPtr = (delayLine->currentPtr + 1) % delayLine->N;

  return y;
}


/*
 *  peek() will return the next output sample in the delay line but will NOT pop it off the delay line!
 *
 *  Inputs:
 *    delayLine:  Pointer to MW_DSP_DelayLine structure (must be previously initialized)
 *
 *  Returns:
 *    Sample delayed by N cycles
 *    (NO_OPTIMIZE) NAN if delayLine is NULL
 */
float32_t MW_DSP_DelayLine_peek(MW_DSP_DelayLine *delayLine)
{
#ifdef NO_OPTIMIZE
  if (delayLine == NULL)
    return NAN;

  if (delayLine->N == 0 || delayLine->buffer == NULL)
    return NAN;
#endif
  return delayLine->buffer[delayLine->currentPtr];
}



//  Fractional Delay line Functions
// ------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------------------------------------------------------ //

/*
 *  Initialize an instance of MW_DSP_FractionalDelayLine
 *  NOTE:  The initialization function will NOT allocate memory for its internal delay line buffer
 *  You must pass in a pre-allocated array.
 * 
 *  Total delay size (N) is limited to int16_t to accomodate SIMD processing calls.  That means the total delay length
 *  is 32768.  For a resource limited system, this should be okay.  Hopefully.
 * 
 *  Inputs:
 *    delayLine:    Pointer to MW_DSP_FractionalDelayLine instance
 *    buffer:       Pointer to array that will hold delayed samples (must be already allocated in memory)
 *    N:            Total delay line length (ie. the size of buffer)
 *    M:            Desired delay line length (may be fractional)
*/
int32_t MW_DSP_FractionalDelayLine_init(MW_DSP_FractionalDelayLine *delayLine, float32_t *buffer, int16_t N, float32_t M)
{
  if (delayLine == NULL || buffer == NULL || M < 0 || N <= 0 || M >= MW_DSP_FRAC_DELAY_MAX_DELAY_LENGTH)
    return 0;

  if ((int16_t)M > N)
    return 0;

  arm_fill_f32(0.f, buffer, N);

  delayLine->buffer = buffer;
  delayLine->N = N;
  delayLine->writePtr = N - 1;

  delayLine->MInt = (int16_t)M;
  delayLine->MFrac = M - delayLine->MInt;
  delayLine->readPtr = delayLine->MInt + delayLine->writePtr - N;

  return 1;
}


/*
 *  Set the delay length.  Fractional lengths are allowed
 *
 *  Inputs:
 *    delayLine:    Pointer to MW_DSP_FractionalDelayLine instance
 *    M:            Delay length
 *
 *  Returns:
 *    None
 */
void MW_DSP_FractionalDelayLine_setDelayLength(MW_DSP_FractionalDelayLine *delayLine, float32_t M)
{
#ifdef NO_OPTIMIZE
  assert(delayLine != NULL);
  assert(M > 0);
  assert(delayLine->delay.N != 0);
#endif

  delayLine->MInt = (int16_t)M;
  delayLine->MFrac = M - floorf(M);

  delayLine->readPtr = (delayLine->writePtr - delayLine->MInt + delayLine->N) % delayLine->N;
}


/*
 *  Execute one cycle of delay line operation
 *  Note when writing to the delay line, instead of starting from 0 and moving up, this implementation starts from the end
 *  of the delay line and moves down.  This is done to check when the write pointer needs to wrap around while using SIMD functions
 * 
 *  Inputs:
 *    delayLine:    Pointer to MW_DSP_FractionalDelayLine instance
 *    x:            Value to shift into the delay line
 * 
 *  Returns:
 *    Linearly interpolated sample
 */
float32_t MW_DSP_FractionalDelayLine_tick(MW_DSP_FractionalDelayLine *delayLine, float32_t x)
{
#ifdef NO_OPTIMIZE
  assert(delayLine != NULL);
  assert(&delayLine->delay != NULL);
#endif

  delayLine->buffer[delayLine->writePtr--] = x;
  if (delayLine->writePtr < 0)
    delayLine->writePtr = delayLine->N - 1;
  
  int32_t firstInterpolatingIndex = delayLine->readPtr--;
  int32_t secondInterpolatingIndex = firstInterpolatingIndex - 1;
  if (secondInterpolatingIndex < 0)
    secondInterpolatingIndex = delayLine->N - 1;

  float32_t y = ((1.f - delayLine->MFrac) * delayLine->buffer[firstInterpolatingIndex]) + (delayLine->MFrac * delayLine->buffer[secondInterpolatingIndex]);

  if (delayLine->readPtr < 0)
    delayLine->readPtr = delayLine->N - 1;

  return y;
}


void MW_DSP_FractionalDelayLine_process(MW_DSP_FractionalDelayLine *delayLine, float32_t *buffer, size_t numSamples)
{
  #ifdef NO_OPTIMIZE
    assert(delayLine != NULL);
  #endif

  for (int32_t i = 0; i < numSamples; ++i)
  {
    delayLine->buffer[delayLine->writePtr--] = buffer[i];

    if (delayLine->writePtr < 0)
      delayLine->writePtr = delayLine->N - 1;

    //  We are going to linearly interpolate between two values in the delay line
    int32_t firstInterpolatingIndex = delayLine->readPtr--;
    int32_t secondInterpolatingIndex = firstInterpolatingIndex + 1;
    if (secondInterpolatingIndex >= delayLine->N)
      secondInterpolatingIndex = 0;

    buffer[i] = ((1.f - delayLine->MFrac) * delayLine->buffer[firstInterpolatingIndex]) + (delayLine->MFrac * delayLine->buffer[secondInterpolatingIndex]);

    if (delayLine->readPtr < 0)
      delayLine->readPtr = delayLine->N - 1;
  }

}


void MW_DSP_FractionalDelayLine_reset(MW_DSP_FractionalDelayLine *delayLine)
{
  #ifdef NO_OPTIMIZE
  if (delayLine == NULL) while(1);
  if (delayLine->buffer == NULL) while(1);
  #endif

  if (delayLine == NULL) return;
  if (delayLine->buffer == NULL) return;

  arm_fill_f32(0, delayLine->buffer, delayLine->N);

  delayLine->readPtr = 0;
  delayLine->writePtr = 0;
}




















