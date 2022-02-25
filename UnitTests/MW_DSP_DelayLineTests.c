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


#include "MW_DSP_DelayLineTests.h"


#define NAN_VALUE 0x7FC00000


static int32_t MW_DSP_DelayLine_StandardOperation()
{
  //  Create a buffer to store delayed samples
  size_t delayLineSize = 5;
  float32_t buffer[5];

  MW_DSP_DelayLine delayLine;
  int32_t error = MW_DSP_DelayLine_init(&delayLine, buffer, delayLineSize);

  if (!error)
    return 0;

  //  Delay Line should be initialized to all zeros.  Check for this while pushing in samples
  for (int32_t i = 0; i < delayLineSize; ++i)
  {
    float32_t y = MW_DSP_DelayLine_tick(&delayLine, i + 1);
    if (y != 0.f)
      return 0;
  }

  //  Check to see that we can peek at the next output sample
  if (MW_DSP_DelayLine_peek(&delayLine) != 1)
    return 0;

  //  Shuffle out all the stored samples
  for (int32_t i = 0; i < delayLineSize; ++i)
    if (MW_DSP_DelayLine_tick(&delayLine, 0) != (i + 1))
      return 0;

  //  Attempt to initialize a DelayLine object set to NULL and a buffer set to NULL
  error = MW_DSP_DelayLine_init(NULL, buffer, delayLineSize);
  if (error)
    return 0;

  error = MW_DSP_DelayLine_init(&delayLine, NULL, delayLineSize);
  if (error)
    return 0;


  return 1;
}


static int32_t MW_DSP_DelayLine_MemoryAllocTest()
{
  size_t delayLineSize = 5;
  MW_DSP_DelayLine *delayLine;

  int32_t error = MW_DSP_DelayLine_init_memalloc(&delayLine, delayLineSize);
  if (!error)
    return 0;

  //  Delay Line should be initialized to all zeros.  Check for this while pushing in samples
  for (int32_t i = 0; i < delayLineSize; ++i)
  {
    float32_t y = MW_DSP_DelayLine_tick(delayLine, i + 1);
    if (y != 0.f)
      return 0;
  }

  //  Check to see that we can peek at the next output sample
  if (MW_DSP_DelayLine_peek(delayLine) != 1)
    return 0;

  //  Shuffle out all the stored samples
  for (int32_t i = 0; i < delayLineSize; ++i)
    if (MW_DSP_DelayLine_tick(delayLine, 0) != (i + 1))
      return 0;

  //  Now, free the memory used for the delay line
  //  This means that the entire delay line will be destroyed
  MW_DSP_DelayLine_delete(&delayLine);

  if (delayLine != NULL)
    return 0;

  return 1;
}


#ifdef NO_OPTIMIZE
static int32_t MW_DSP_DelayLine_DelayLineNonInitializedTest()
{
  MW_DSP_DelayLine delayLine;

  //  Attempt to run tick() and peek() on an uninitialized delay line
  float32_t y = MW_DSP_DelayLine_tick(&delayLine, 0);
  uint32_t *val = &y;
  if (*val != NAN_VALUE)
    return 0;

  y = MW_DSP_DelayLine_peek(&delayLine);
  if (*val != NAN_VALUE)
    return 0;


  //  Attempt to run tick() and peek() on a nullptr DelayLine object
  MW_DSP_DelayLine *nullDelayLine = NULL;

  y = MW_DSP_DelayLine_tick(nullDelayLine, 0);
  if (*val != NAN_VALUE)
    return 0;

  y = MW_DSP_DelayLine_peek(nullDelayLine);
  if (*val != NAN_VALUE)
    return 0;

  return 1;
}
#endif


int32_t MW_DSP_FractionalDelayLine_initializationTests()
{
  MW_DSP_FractionalDelayLine delay;
  float32_t delayBuffer[7];
  float32_t M = 3.1;
  float32_t epsilon = 0.01;
  int32_t N = 7;

  //  Attempt to initialize a delay line instance with invalid inputs
  //  Starting with nullpointer passed in as a delay line instance
  int32_t success = MW_DSP_FractionalDelayLine_init(NULL, delayBuffer, M, N);
  if (success)
    return 0;

  //  Attempt to pass in a null buffer
  success = MW_DSP_FractionalDelayLine_init(&delay, NULL, N, M);
  if (success)
    return 0;
  
  //  Attempt to pass in a negative N value
  success = MW_DSP_FractionalDelayLine_init(&delay, delayBuffer, -N, M);
  if (success)
    return 0;

  //  Attempt to pass in 0 as a total delay line size
  success = MW_DSP_FractionalDelayLine_init(&delay, delayBuffer, 0, M);
  if (success)
    return 0;

  //  Attempt to pass in a negative delay value
  success = MW_DSP_FractionalDelayLine_init(&delay, delayBuffer, N, -M);
  if (success)
    return 0;

  //  Now initialize with valid parameters
  success = MW_DSP_FractionalDelayLine_init(&delay, delayBuffer, N, M);
  if (!success)
    return 0;

  if (delay.buffer != delayBuffer)
    return 0;

  if (delay.N != N)
    return 0;

    if (delay.MInt != (int32_t)M)
    return 0;

  if (delay.writePtr != N - 1)
    return 0;

  int32_t EXPECTED_READ_PTR = (delay.writePtr - (N - (int32_t)M)) % N;
  if (delay.readPtr != EXPECTED_READ_PTR)
    return 0;
  
  int32_t   M_INTEGER = (int32_t)M;
  float32_t EXPECTED_FRACTIONAL_PART = M - (float32_t)M_INTEGER;
  if (delay.MFrac < EXPECTED_FRACTIONAL_PART - epsilon || delay.MFrac > EXPECTED_FRACTIONAL_PART + epsilon)
    return 0;

  return 1;
}


int32_t MW_DSP_FractionalDelayLine_standardOperationTests()
{
  MW_DSP_FractionalDelayLine delay;
  float32_t delayBuffer[7];
  float32_t M = 3.5;
  int32_t N = 7;

  int32_t success = MW_DSP_FractionalDelayLine_init(&delay, delayBuffer, N, M);
  if (!success)
    return 0;

  for (int32_t i = 1; i < 10; ++i)
  {
    MW_DSP_FractionalDelayLine_tick(&delay, 0);

    //  After one tick cycle, for the given initial conditions, the write pointer should now be at 5
    int32_t EXPECTED_WRITE_INDEX = N - 1 - i;
    if (EXPECTED_WRITE_INDEX < 0)
      EXPECTED_WRITE_INDEX += N;

    if (delay.writePtr != EXPECTED_WRITE_INDEX)
      return 0;

    int32_t EXPECTED_READ_INDEX = (int32_t)M - 1 - i;
    if (EXPECTED_READ_INDEX < 0)
      EXPECTED_READ_INDEX += N;

    if (delay.readPtr != EXPECTED_READ_INDEX)
      return 0;
  }

  return 1;
}


int32_t MW_DSP_DelayLine_runUnitTests()
{
  if (!MW_DSP_DelayLine_StandardOperation())
    return 0;

  if (!MW_DSP_DelayLine_MemoryAllocTest())
    return 0;

#ifdef NO_OPTIMIZE
  if (!MW_DSP_DelayLine_DelayLineNonInitializedTest())
    return 0;
#endif

if (!MW_DSP_FractionalDelayLine_initializationTests())
  return 0;

if (!MW_DSP_FractionalDelayLine_standardOperationTests())
  return 0;

  return 1;
}
