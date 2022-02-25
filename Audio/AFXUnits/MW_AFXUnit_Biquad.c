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


#include "MW_AFXUnit_Biquad.h"


/*
 *  Calculate biquad filter coefficients for a given type of filter and parameters
 *  These calculations are taken from the Will Prikle book, Designing Audio Effect Plugins in C++
 */
void MW_AFXUnit_Biquad_calculateCoefficients(MW_AFXUnit_BiquadType filterType, float32_t *coefficientsOut, float32_t fs, float32_t fc, float32_t Q, float32_t gain)
{
#ifdef NO_OPTIMIZE
    if (filterType >= MW_BIQUAD_NUM_TYPES && filterType < 0) while(1);
    if (coefficientsOut == NULL) while(1);
    if (fs <= 0.f) while(1);
    if (fc >= fs * 0.5f) while(1);
    if (Q < 0.f) while(1);
#endif

    switch(filterType)
    {
        case MW_BIQUAD_PARAM_EQ_NCQ:
        {
            float32_t mu = pow(10, gain / 20.f);
            float32_t theta = 2.f * PI * fc / fs;

            float32_t zeta = 4.f / (1.f + mu);
            float32_t k = zeta * arm_sin_f32(theta / (2.f * Q)) / arm_cos_f32(theta / (2.f * Q));
            float32_t beta = 0.5f * (1.f - k) / (1.f + k);
            float32_t gamma = (0.5f + beta) * arm_cos_f32(theta);

            coefficientsOut[0] = 0.5f - beta;
            coefficientsOut[1] = 0.f;
            coefficientsOut[2] = beta - 0.5f;
            coefficientsOut[3] = 2.f * gamma;
            coefficientsOut[4] = -2.f * beta;
            coefficientsOut[5] = mu - 1.f;

            break;
        }

        case MW_BIQUAD_PARAM_EQ_CQ:
        {
            float32_t phase = PI * fc / fs;
            float32_t K = arm_sin_f32(phase) / arm_cos_f32(phase);
            float32_t V0 = pow(10, gain / 20.f);
            float32_t QInv = 1.f / Q;
            float32_t KSq = K * K;
            
            float32_t d0Inv = 1.f / (1.f + QInv * K + KSq);
            float32_t alpha = 1.f + QInv * K * V0 + KSq;
            float32_t beta = 2 * (KSq - 1.f);
            float32_t gamma = 1.f - V0 * QInv * K + KSq;
            float32_t delta = 1.f - QInv * K + KSq;

            if (gain >= 0)
            {
                coefficientsOut[0] = alpha * d0Inv;
                coefficientsOut[1] = beta * d0Inv;
                coefficientsOut[2] = gamma * d0Inv;
                coefficientsOut[3] = -beta * d0Inv;
                coefficientsOut[4] = -delta * d0Inv;
            }

            if (gain < 0)
            {
                float32_t V0QInv = QInv / V0;
                float32_t e0Inv = 1.f / (1.f + V0QInv * K + KSq);
                float32_t eta = 1.f - V0QInv * K + KSq;

                coefficientsOut[0] = (1.f / d0Inv) * e0Inv;
                coefficientsOut[1] = beta * e0Inv;
                coefficientsOut[2] = delta * e0Inv;
                coefficientsOut[3] = -beta * e0Inv;
                coefficientsOut[4] = -eta * e0Inv;
            }

            break;
        }

        case MW_BIQUAD_LPF:
        {
            float32_t theta = 2.f * PI * fc / fs;
            float32_t d = 1.f / Q;
            float32_t k = 0.5f * d * arm_sin_f32(theta);
            float32_t beta = 0.5f * (1.f - k) / (1.f + k);
            float32_t gamma = (0.5f + beta) * arm_cos_f32(theta);
            float32_t a = 0.5f + beta - gamma;

            coefficientsOut[0] = 0.5f * a;
            coefficientsOut[1] = a;
            coefficientsOut[2] = 0.5f * a;
            coefficientsOut[3] = 2.f * gamma;
            coefficientsOut[4] = -2.f * beta;

            break;
        }

        case MW_BIQUAD_HPF:
        {
            float32_t theta = 2.f * PI * fc / fs;
            float32_t d = 1.f / Q;
            float32_t k = 0.5f * d * arm_sin_f32(theta);
            float32_t beta = 0.5f * (1 - k) / (1 + k);
            float32_t gamma = (0.5f + beta) * arm_cos_f32(theta);
            float32_t a = 0.5f + beta + gamma;

            coefficientsOut[0] = 0.5f * a;
            coefficientsOut[1] = -a;
            coefficientsOut[2] = 0.5f * a;
            coefficientsOut[3] = 2.f * gamma;
            coefficientsOut[4] = -2.f * beta;

            break;
        }

        case MW_BIQUAD_BPF:
        {
            float32_t theta = PI * fc / fs;
            float32_t k = arm_sin_f32(theta) / arm_cos_f32(theta);
            float32_t delta = (k * k * Q) + k + Q;
            float32_t a0 = k / delta;

            coefficientsOut[0] = a0;
            coefficientsOut[1] = 0.f;
            coefficientsOut[2] = -a0;
            coefficientsOut[3] = 2.f * Q * ((k * k) - 1.f) / delta;
            coefficientsOut[4] = ((k * k * Q) - k + Q) / delta;

            break;
        }

        default:
            break;
    }
}


/*
 *  Initialize an instance of MW_AFXUnit_Biquad
 *  This implementation will leverage the biquad functions already available in the ARM CMSIS DSP library.
 * 
 *  Depending on the type of filter, valid inputs for copyBuffer and copyBufferSize may need to be passed
 *  This typically applies to tone control filters (shelving and parametric EQ filters) as they have a feed-forward component
 *  that is summed with the biquad output.
 *  
 *  The following filter types must have valid copyBuffer and copyBufferSize inputs (copyBuffer must hold the same number of samples in your audio buffer)
 *      - MW_BIQUAD_LOW_SHELF
 *      - MW_BIQUAD_HIGH_SHELF
 *      - MW_BIQUAD_PARAM_EQ_NCQ
 * 
 *  All other filter types can pass in NULL.
 * 
 *  Some filter types do not need Q defined, in which case, it is simply ignored
 * 
 *  NOTE:   This implementation does not have input parameter smoothing so any rapid changes in parameters will likely result in
 *          clicking noises.  For a variable parameter filter, the MW_AFXUnit_SVFilter is preferable
 * 
 *  Inputs:
 *      biquad:         Pointer to MW_AFXUnit_Biquad instance
 *      filterType:     Filter type
 *      fs:             Sampling frequency
 *      fc:             Filter cutoff/centre frequency
 *      Q:              Filter Q
 *      gain:           Filter gain (in dB)
 *      copyBuffer:     Pointer to pre-allocated array that holds the same number of samples as your audio buffer (If using Low, High Shelf or Param EQ filters)
 *      copyBufferSize: Size of copyBuffer (must be the same size as your audio buffer)
 * 
 *  Returns:
 *      0: if initialization unsuccessful
 *      1: otherwise
 */ 
int32_t MW_AFXUnit_Biquad_init(MW_AFXUnit_Biquad *biquad, MW_AFXUnit_BiquadType filterType, float32_t fs, float32_t fc, float32_t Q, float32_t gain, float32_t *copyBuffer, size_t copyBufferSize)
{
    if (biquad == NULL)
        return 0;
    
    if (fc >= 0.5f * fs)
        return 0;

    if (fs <= 0)
        return 0;
    
    if (Q <= 0.f)
        return 0;

    if (filterType < 0 || filterType >= MW_BIQUAD_NUM_TYPES)
        return 0;

    //  These filter types need to have a valid copyBuffer and copyBufferSize input
    if ((filterType == MW_BIQUAD_LOW_SHELF && copyBuffer == NULL) || (filterType == MW_BIQUAD_LOW_SHELF && copyBufferSize == 0))
        return 0;

    if ((filterType == MW_BIQUAD_HIGH_SHELF && copyBuffer == NULL) || (filterType == MW_BIQUAD_HIGH_SHELF && copyBufferSize == 0))
        return 0;

    if ((filterType == MW_BIQUAD_PARAM_EQ_NCQ && copyBuffer == NULL) || (filterType == MW_BIQUAD_PARAM_EQ_NCQ && copyBufferSize == 0))
        return 0;

    biquad->copyBuffer = copyBuffer;
    biquad->bufferSize = copyBufferSize;
    biquad->fs = fs;
    biquad->filterType = filterType;

    MW_AFXUnit_Biquad_calculateCoefficients(filterType, biquad->coefficients, fs, fc, Q, gain);

    arm_biquad_cascade_df2T_init_f32(&biquad->biquadInstance, 1, biquad->coefficients, biquad->stateVariables);

    return 1;
}


void MW_AFXUnit_Biquad_changeParameters(MW_AFXUnit_Biquad *biquad, MW_AFXUnit_BiquadType filterType, float32_t fc, float32_t Q, float32_t gain)
{
#ifdef NO_OPTIMIZE
    if (biquad == NULL) while(1);)
    if (!biquad.isInitialized) while(1);
    if (filterType >= MW_BIQUAD_NUM_TYPES || filterType < 0) while(1);
    if (fc >= biquad->fs * 0.5f) while(1);
#endif

    MW_AFXUnit_Biquad_calculateCoefficients(filterType, biquad->coefficients, biquad->fs, fc, Q, gain);

    return;
}


void MW_AFXUnit_Biquad_process(MW_AFXUnit_Biquad *biquad, float32_t *buffer, size_t numSamples)
{
#ifdef NO_OPTIMIZE
    if (biquad == NULL) while(1);
    if (buffer == NULL) while(1);
#endif

    switch (biquad->filterType)
    {
        case MW_BIQUAD_LOW_SHELF:
        case MW_BIQUAD_HIGH_SHELF:
        case MW_BIQUAD_PARAM_EQ_NCQ:
        {
            #ifdef NO_OPTIMIZE
            if (biquad->bufferSize != numSamples) while(1);
            #endif
            arm_copy_f32(buffer, biquad->copyBuffer, numSamples);
            arm_biquad_cascade_df2T_f32(&biquad->biquadInstance, buffer, buffer, numSamples);
            arm_scale_f32(buffer, biquad->coefficients[5], buffer, numSamples);
            arm_add_f32(buffer, biquad->copyBuffer, buffer, numSamples);

            break;
        }

        default:
            arm_biquad_cascade_df2T_f32(&biquad->biquadInstance, buffer, buffer, numSamples);
            break;
    }
}


void MW_AFXUnit_Biquad_reset(MW_AFXUnit_Biquad *biquad)
{
    #ifdef NO_OPTIMIZE
    if (biquad == NULL) while(1);
    #endif

    if (biquad == NULL) return;

    biquad->stateVariables[0] = 0;
    biquad->stateVariables[1] = 0;
}
