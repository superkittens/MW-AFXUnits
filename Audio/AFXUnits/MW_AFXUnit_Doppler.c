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


#include "MW_AFXUnit_Doppler.h"

#define TRANSITION_SIGNAL_AMPLITUDE 0.1f
#define TRANSITION_SIGNAL_FREQ 20.f
#define TRANSITION_EXIT_DELTA 0.005f


/*
 *  Initialize a MW_AFXUnit_Doppler structure
 *  Doppler initialization routine WILL NOT allocate memory for the internal delay line for you
 *  You MUST manually create a section of memory for the delay line
 *  Use MW_AFXUnit_Doppler_init_memalloc() if you want the delay line to be dynamically allocated
 *
 *  Inputs:
 *    dopplerUnit:          Pointer to a MW_AFXUnit_Doppler structure
 *    delayLineBuffer:      Pointer to an array that will hold the audio samples.  Memory must be already allocated
 *    delayLineBufferSize:  Size of delayLineBuffer
 *    fs:                   Sampling Frequency
 * 
 *  Returns:
 *    0 if unsuccessful, 1 otherwise    
 */
int32_t MW_AFXUnit_Doppler_init(MW_AFXUnit_Doppler *dopplerUnit, float32_t *delayLineBuffer, int32_t delayLineBufferSize, float32_t fs)
{

  if (dopplerUnit == NULL || delayLineBuffer == NULL || delayLineBufferSize <= 0)
    return 0;

  arm_fill_f32(0.f, delayLineBuffer, delayLineBufferSize);

  dopplerUnit->buffer = delayLineBuffer;
  dopplerUnit->N = delayLineBufferSize;
  dopplerUnit->writePtr = 1;
  dopplerUnit->readPtr = 0;
  dopplerUnit->g = 0.f;
  dopplerUnit->inTransition = 0;
  dopplerUnit->readWritePtrCross = 0;
  dopplerUnit->prevOutput = 0;
  dopplerUnit->transitionPhaseIncrement = 6.283185 * TRANSITION_SIGNAL_FREQ / fs;
  dopplerUnit->transitionSignalPhase = 0;

  return 1;
}


/*
 *  Change delay line growth factor
 *  This is the key parameter in modulating the length of the delay line
 *  For a lower pitch effect, set g to range (0, 1)
 *  For a higher pitch effect, set g to range (-1, 0)
 *
 *  Inputs:
 *    dopplerUnit:  Pointer to MW_AFXUnit_Doppler structure (must be previously initialized)
 *    g:            Growth factor
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_Doppler_changeParameters(MW_AFXUnit_Doppler *dopplerUnit, float32_t g)
{
  dopplerUnit->g = g;
}


/*
 *  Apply doppler effect to a buffer of audio samples
 *  Linear interpolation is used to calculate the fractional delay sample
 *
 *  Inputs:
 *    dopplerUnit:  Pointer to MW_AFXUnit_Doppler structure (must be previously initialized)
 *    buffer:       Pointer to buffer holding audio samples.  Processed audio will be stored in the same buffer
 *    bufferSize:   Number of audio samples
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_Doppler_process(MW_AFXUnit_Doppler *dopplerUnit, float32_t *buffer, size_t bufferSize)
{
  #ifdef NO_OPTIMIZE
  if (dopplerUnit == NULL || dopplerUnit->buffer == NULL) while(1);
  #endif

  if (dopplerUnit == NULL || dopplerUnit->buffer == NULL) return;

  if (dopplerUnit->g == 1.f)
  {
    arm_fill_f32(0.f, buffer, bufferSize);
    return;
  }

  for (size_t i = 0; i < bufferSize; ++i)
  {
    //  Calculate interpolated sample from fractional delay
    int32_t index1 = (int32_t)dopplerUnit->readPtr;
    int32_t index2 = index1 + 1;

    if (index2 >= dopplerUnit->N)
      index2 = 0;

    //  Determine if the read/write pointer overlap each other
    if ((dopplerUnit->g > 1) || (dopplerUnit->g < 0))
    {
      int32_t nextWritePtr = (dopplerUnit->writePtr + 1) % dopplerUnit->N;
      if (nextWritePtr == index1)
        dopplerUnit->readWritePtrCross = 1;
    }

    if (index1 == dopplerUnit->writePtr)
      dopplerUnit->readWritePtrCross = 1;

    //  Continue with output calculation
    float32_t value1 = dopplerUnit->buffer[index1];
    float32_t value2 = dopplerUnit->buffer[index2];
    float32_t a = dopplerUnit->readPtr - index1;

    float32_t y = ((1.f - a) * value1) + (a * value2);

    //  If in transition period, write the sinusoid transitional signal to avoid audio clicks
    if (dopplerUnit->inTransition)
    {
      float32_t transitionValue = dopplerUnit->transitionSignalBias + (dopplerUnit->transitionSignalSign * TRANSITION_SIGNAL_AMPLITUDE * arm_sin_f32(dopplerUnit->transitionSignalPhase));
      dopplerUnit->transitionSignalPhase += dopplerUnit->transitionPhaseIncrement;

      if ((y >= transitionValue - TRANSITION_EXIT_DELTA) && (y <= transitionValue + TRANSITION_EXIT_DELTA))
      {
        dopplerUnit->inTransition = 0;
        dopplerUnit->transitionSignalPhase = 0.f;
      }

      y = transitionValue;
    }

    dopplerUnit->readPtr += (1.f - dopplerUnit->g);

    dopplerUnit->buffer[dopplerUnit->writePtr++] = buffer[i];

    //  read/writePtr bounds check
    //  ------------------------------------------------- //
    if (dopplerUnit->writePtr >= dopplerUnit->N)
      dopplerUnit->writePtr = 0;

    if (dopplerUnit->readPtr >= (float32_t)dopplerUnit->N)
      dopplerUnit->readPtr = 0;

    if (dopplerUnit->readPtr < 0.f)
      dopplerUnit->readPtr = (float32_t)(dopplerUnit->N - 1);

    //  ------------------------------------------------- //

    //  Check to see if the writePtr crossed over readPtr and go into transition if so
    if ((dopplerUnit->writePtr != (int32_t)dopplerUnit->readWritePtrCross) && dopplerUnit->readWritePtrCross)
    {
      dopplerUnit->transitionSignalBias = dopplerUnit->prevOutput;

      //  Determine the sign of the transition signal
      if (y < dopplerUnit->prevOutput)
        dopplerUnit->transitionSignalSign = -1.f;
      else
        dopplerUnit->transitionSignalSign = 1.f;

      dopplerUnit->readWritePtrCross = 0;
      dopplerUnit->inTransition = 1;

      y = dopplerUnit->prevOutput;
    }

    dopplerUnit->prevOutput = y;

    buffer[i] = y;
  }
}


void MW_AFXUnit_Doppler_reset(MW_AFXUnit_Doppler *dopplerUnit)
{
  #ifdef NO_OPTIMIZE
  if (dopplerUnit == NULL) while(1);
  if (dopplerUnit->buffer == NULL) while(1);
  #endif

  if (dopplerUnit == NULL) return;
  if (dopplerUnit->buffer == NULL) return;

  arm_fill_f32(0, dopplerUnit->buffer, dopplerUnit->N);

  dopplerUnit->g = 0;
  dopplerUnit->readPtr = 0;
  dopplerUnit->writePtr =0;
}

