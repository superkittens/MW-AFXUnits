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


#include "MW_AFXUnit_SVFilter.h"



static void MW_AFXUnit_SVFilter_processLPF(MW_AFXUnit_SVFilter *filter, float32_t *buffer, size_t bufferSize)
{
  for (int32_t i = 0; i < bufferSize; ++i)
  {
    float32_t yHP = filter->filterCoefficients[2] * (buffer[i] - filter->internalStates[0] * (filter->filterCoefficients[0] + filter->filterCoefficients[1]) - filter->internalStates[1]);
    float32_t yBP = yHP * filter->filterCoefficients[0] + filter->internalStates[0];
    float32_t yLP = yBP * filter->filterCoefficients[0] + filter->internalStates[1];

    filter->internalStates[0] = yHP * filter->filterCoefficients[0] + yBP;
    filter->internalStates[1] = yBP * filter->filterCoefficients[0] + yLP;

    buffer[i] = yLP;
  }
}


static void MW_AFXUnit_SVFilter_processHPF(MW_AFXUnit_SVFilter *filter, float32_t *buffer, size_t bufferSize)
{
  for (int32_t i = 0; i < bufferSize; ++i)
  {
    float32_t yHP = filter->filterCoefficients[2] * (buffer[i] - filter->internalStates[0] * (filter->filterCoefficients[0] + filter->filterCoefficients[1]) - filter->internalStates[1]);
    float32_t yBP = yHP * filter->filterCoefficients[0] + filter->internalStates[0];
    float32_t yLP = yBP * filter->filterCoefficients[0] + filter->internalStates[1];

    filter->internalStates[0] = yHP * filter->filterCoefficients[0] + yBP;
    filter->internalStates[1] = yBP * filter->filterCoefficients[0] + yLP;

    buffer[i] = yHP;
  }
}


static void MW_AFXUnit_SVFilter_processBPF(MW_AFXUnit_SVFilter *filter, float32_t *buffer, size_t bufferSize)
{
  for (int32_t i = 0; i < bufferSize; ++i)
  {
    float32_t yHP = filter->filterCoefficients[2] * (buffer[i] - filter->internalStates[0] * (filter->filterCoefficients[0] + filter->filterCoefficients[1]) - filter->internalStates[1]);
    float32_t yBP = yHP * filter->filterCoefficients[0] + filter->internalStates[0];
    float32_t yLP = yBP * filter->filterCoefficients[0] + filter->internalStates[1];

    filter->internalStates[0] = yHP * filter->filterCoefficients[0] + yBP;
    filter->internalStates[1] = yBP * filter->filterCoefficients[0] + yLP;

    buffer[i] = yBP;
  }
}


/*
 *  Coefficients are stored in a single array and stored in the following order:
 *  [alpha, 2R, alpha0]
 *  (The coefficient naming convention is taken from the Will Pirkle book, "Designing Audio Effect Plugins in C++"
 *  By the way, if you are using that book as reference, I think the SVFilter design equations are incorrect?
 *  So be careful
 *
 *  For performance reasons, calculateCoefficients() will not perform a nullptr check
 *  so be absolutely careful about what you're putting into the function!
 */
static void MW_AFXUnit_SVFilter_calculateCoefficients(MW_AFXUnit_SVFilter *filter, float32_t fs, float32_t fc, float32_t Q)
{
  #ifndef NO_OPTIMIZE
    if (filter == NULL) while(1);
    if (Q <= 0) while(1);
    if (fc >= 0.5 * fs) while(1);
  #endif

  if (filter == NULL) return;
  if (Q <= 0) return;
  if (fc >= 0.5 * fs) return;


  float32_t theta = PI * fc / fs;
  filter->filterCoefficients[0] = arm_sin_f32(theta) / arm_cos_f32(theta);
  filter->filterCoefficients[1] = 1.f / Q;
  filter->filterCoefficients[2] = 1.f / (1.f + (filter->filterCoefficients[1] * filter->filterCoefficients[0]) + (filter->filterCoefficients[0] * filter->filterCoefficients[0]));
}


/*
 *  Initialize an instance of MW_AFXUnit_SVFilter
 *  Further information about the State Variable (SV) filter implementation can be found in
 *  "The Art of VA Filter Design" [Zavalishin]
 *
 *  Inputs:
 *    filter:     Pointer to MW_AFXUnit_SVFilter instance
 *    filterType: Type of filter (LPF, HPF, BPF etc)
 *    fs:         System sampling frequency [Hz]
 *    fc:         Cutoff (LPF, HPF) or centre (BPF) frequency [Hz]
 *    Q:          Quality factor
 *
 *  Returns:
 *    0 if initialization not successful
 *    1 if successful
 */
int32_t MW_AFXUnit_SVFilter_init(MW_AFXUnit_SVFilter *filter, MW_AFXUnit_SVFilterType filterType, float32_t fs, float32_t fc, float32_t Q)
{
  if (filter == NULL || fs < 0 || fc <0 || Q < 0)
    return 0;

  if (filterType < 0 || filterType >= MW_AFXUNIT_SVFILTER_NUM_TYPES)
    return 0;

  filter->internalStates[0] = 0.f;
  filter->internalStates[1] = 0.f;
  filter->filterType = filterType;

  MW_AFXUnit_SVFilter_calculateCoefficients(filter, fs, fc, Q);

  return 1;
}


/*
 * Filter parameters are changed via one function
 * If you only wish to change one parameter, you must still input the current values of all the other parameters
 * This function will allow you to change the filter type (eg: switch from LPF to HPF)
 *
 * Inputs:
 *  filter:     Pointer to MW_AFXUnit_SVFilter instance
 *  fc:         Filter cutoff frequency/centre frequency [Hz]
 *  Q:          Quality factor
 *  filterType: Type of filter to change into (or input current filter type to leave the type unchanged)
 *
 *  Returns:
 *    None
 */
void MW_AFXUnit_SVFilter_changeParameters(MW_AFXUnit_SVFilter *filter, MW_AFXUnit_SVFilterType filterType, float32_t fs, float32_t fc, float32_t Q)
{
#ifdef NO_OPTIMIZE
  if (filter == NULL) while(1);
#endif

  if (filter == NULL)
    return;

  MW_AFXUnit_SVFilter_calculateCoefficients(filter, fs, fc, Q);
  filter->filterType = filterType;
}


/*
 *  Apply the filter to a buffer of samples
 *
 *  Inputs:
 *    filter:     Pointer to MW_AFXUnit_SVFilter instance
 *    buffer:     Buffer of input samples
 *    bufferSize: Number of input samples
 *
 *   Returns:
 *    None
 */
void MW_AFXUnit_SVFilter_process(MW_AFXUnit_SVFilter *filter, float32_t *buffer, size_t bufferSize)
{
#ifdef NO_OPTIMIZE
  if (filter == NULL || buffer == NULL) while(1);
#endif

  if (filter == NULL || buffer == NULL)
    return;

  switch (filter->filterType)
  {
    case MW_AFXUNIT_SVFILTER_LPF:
    {
      MW_AFXUnit_SVFilter_processLPF(filter, buffer, bufferSize);
      break;
    }

    case MW_AFXUNIT_SVFILTER_HPF:
    {
      MW_AFXUnit_SVFilter_processHPF(filter, buffer, bufferSize);
      break;
    }

    case MW_AFXUNIT_SVFILTER_BPF:
    {
      MW_AFXUnit_SVFilter_processBPF(filter, buffer, bufferSize);
      break;
    }

    default:
    {
      MW_AFXUnit_SVFilter_processLPF(filter, buffer, bufferSize);
      break;
    }
  }
}


void MW_AFXUnit_SVFilter_reset(MW_AFXUnit_SVFilter *filter)
{
  if (filter == NULL)
    return;

  for (int32_t i = 0; i < MW_AFXUNIT_SVFILTER_NUM_STATES; ++i)
    filter->internalStates[i] = 0.f;  
}