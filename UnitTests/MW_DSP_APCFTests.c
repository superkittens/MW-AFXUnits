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


#include "MW_DSP_APCFTests.h"


static float32_t EPSILON = 0.1f;



static int32_t MW_DSP_APCF_APCFInitializationTests()
{
    MW_DSP_APCF apcf;
    float32_t   delayLineBuffer[10];
    int32_t     delayLineLength = 10;
    float32_t   apcfGain = 0.8f;

    int32_t success = MW_DSP_APCF_init(NULL, delayLineBuffer, delayLineLength, apcfGain);
    if (success)
        return 0;

    success = MW_DSP_APCF_init(&apcf, NULL, delayLineLength, apcfGain);
    if (success)
        return 0;

    success = MW_DSP_APCF_init(&apcf, delayLineBuffer, -1, apcfGain);
    if (success)
        return 0;

    //  Make sure that the APCF gain is less than 1.0
    success = MW_DSP_APCF_init(&apcf, delayLineBuffer, delayLineLength, 2.7);
    if (success)
        return 0;

    success = MW_DSP_APCF_init(&apcf, delayLineBuffer, delayLineLength, apcfGain);
    if (!success)
        return 0;

    if (apcf.delayLine != delayLineBuffer)
        return 0;

    if (apcf.N != delayLineLength)
        return 0;

    if (apcf.gain <= apcfGain - EPSILON || apcf.gain >= apcfGain + EPSILON)
        return 0;

    if (apcf.currentPtr != 0)
        return 0;

    return 1;
}


static int32_t MW_DSP_APCF_NestedAPCFInitializationTests()
{
    MW_DSP_NestedAPCF   nestedAPCF;
    MW_DSP_APCF         innerAPCFs[2];
    float32_t           innerAPCFDelayLines[2][10];
    int32_t             numInnerAPCFs = 2;
    float32_t           outerAPCFDelayLine[10];

    int32_t             delayLineLength = 10;
    float32_t           apcfGain = 0.8f;

    //  Initialize inner APCFs first before initializing the outer APCF
    for (int32_t i = 0; i < 2; ++i)
    {
        int32_t success = MW_DSP_APCF_init(&innerAPCFs[i], innerAPCFDelayLines[i], delayLineLength, apcfGain);
        if (!success)
            return 0;
    }

    //  Null pointer check
    int32_t success = MW_DSP_NestedAPCF_init(NULL, outerAPCFDelayLine, delayLineLength, apcfGain, innerAPCFs, numInnerAPCFs);
    if (success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&nestedAPCF, NULL, delayLineLength, apcfGain, innerAPCFs, numInnerAPCFs);
    if (success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&nestedAPCF, outerAPCFDelayLine, -1, apcfGain, innerAPCFs, numInnerAPCFs);
    if (success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&nestedAPCF, outerAPCFDelayLine, delayLineLength, 2.7, innerAPCFs, numInnerAPCFs);
    if (success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&nestedAPCF, outerAPCFDelayLine, delayLineLength, apcfGain, NULL, numInnerAPCFs);
    if (success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&nestedAPCF, outerAPCFDelayLine, delayLineLength, apcfGain, innerAPCFs, 0);
    if (success)
        return 0;

    //  Inner and outer APCFs can have different gain and delay line lengths.  They are all the same in this example as a convenience
    success = MW_DSP_NestedAPCF_init(&nestedAPCF, outerAPCFDelayLine, delayLineLength, apcfGain, innerAPCFs, numInnerAPCFs);
    if (!success)
        return 0;

    if (nestedAPCF.N != delayLineLength)
        return 0;
    
    if (nestedAPCF.delayLine != outerAPCFDelayLine)
        return 0;

    if (nestedAPCF.gain <= apcfGain - EPSILON || nestedAPCF.gain >= apcfGain + EPSILON)
        return 0;

    if (nestedAPCF.currentPtr != 0)
        return 0;

    return 1;
}



int32_t MW_DSP_APCF_runUnitTests()
{
    if (!MW_DSP_APCF_APCFInitializationTests())
        return 0;

    if (!MW_DSP_APCF_NestedAPCFInitializationTests())
        return 0;

    return 1;
}