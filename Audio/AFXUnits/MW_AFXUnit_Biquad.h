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

#ifndef MW_AFXUNIT_BIQUAD_H_
#define MW_AFXUNIT_BIQUAD_H_

#include "arm_math.h"
#include "math.h"


typedef enum
{
    MW_BIQUAD_LOW_SHELF = 0,
    MW_BIQUAD_HIGH_SHELF,
    MW_BIQUAD_PARAM_EQ_NCQ,
    MW_BIQUAD_PARAM_EQ_CQ,
    MW_BIQUAD_LPF,
    MW_BIQUAD_HPF,
    MW_BIQUAD_BPF,
    MW_BIQUAD_NUM_TYPES
}MW_AFXUnit_BiquadType;

/*
 *  Single biquad instance
 *  A single biquad stage will contain 5 + 1 coefficients (+1 for filter output gain)
 *  and 2 state variables (single sample delay lines)
 */ 
typedef struct
{
    MW_AFXUnit_BiquadType                   filterType;
    arm_biquad_cascade_df2T_instance_f32    biquadInstance;
    int32_t                                 isInitializated;
    float32_t                               coefficients[6];
    float32_t                               stateVariables[2];
    float32_t                               *copyBuffer;
    size_t                                  bufferSize;
    float32_t                               fs;
}MW_AFXUnit_Biquad;



int32_t MW_AFXUnit_Biquad_init(MW_AFXUnit_Biquad *biquad, MW_AFXUnit_BiquadType filterType, float32_t fs, float32_t fc, float32_t Q, float32_t gain, float32_t *copyBuffer, size_t copyBufferSize);
void    MW_AFXUnit_Biquad_changeParameters(MW_AFXUnit_Biquad *biquad, MW_AFXUnit_BiquadType filterType, float32_t fc, float32_t Q, float32_t gain);
void    MW_AFXUnit_Biquad_process(MW_AFXUnit_Biquad *biquad, float32_t *buffer, size_t numSamples);
void    MW_AFXUnit_Biquad_reset(MW_AFXUnit_Biquad *biquad);

//  Non-standard extra functions
void    MW_AFXUnit_Biquad_calculateCoefficients(MW_AFXUnit_BiquadType filterType, float32_t *coefficientsOut, float32_t fs, float32_t fc, float32_t Q, float32_t gain);

#endif /* MW_AFXUNIT_BIQUAD_H_ */
