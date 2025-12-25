#ifndef __GS_IIR_HIFI3_H_
#define __GS_IIR_HIFI3_H_

#include "stdint.h"
#include "xtensa/tie/xt_hifi3.h"

#define BIQUAD_FILTER_MAX_NUM   (10)
#define BQR_IIR_HIFI3_LIB

#ifndef BQR_IIR_HIFI3_LIB
#define BIQUAD_FILTER_NUM       (6)

typedef struct
{
    int32_t coef[5];  // b0 b1 b2 a1 a2

    int32_t x_pre[2]; // x[n-1] x[n-2]
    int32_t y_pre[2]; // y[n-1] y[n-2]

}IIR_biquad_coef_t;


typedef struct {

    int32_t total_gain;

    IIR_biquad_coef_t biquad[BIQUAD_FILTER_MAX_NUM];

}IIR_bq_form1_t; //q31

#else

typedef struct {

    int16_t biquad_num;

    int16_t gain_sw;  // swift gain range from -48 to 15

    int32_t biquad[BIQUAD_FILTER_MAX_NUM*5];  // b0 b1 b2 a1 a2  Q30

    int16_t gain_fr[BIQUAD_FILTER_MAX_NUM];  // multi gain Q15 superposition for every biquad

}IIR_bq_form1_t;

void *iir_bqr32x32_df1_init(IIR_bq_form1_t filter_param);

#endif

void *iir_bqr32x32_df1_reinit(IIR_bq_form1_t filter_param);

void IIRFilterDirect1_process(void *iir_param, ae_f32 *inout, uint16_t size);

#endif