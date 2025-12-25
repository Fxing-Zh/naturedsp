#include "gs_iir_hifi3.h"
#include "llfw_dbg.h"
#ifdef BQR_IIR_HIFI3_LIB
#include "NatureDSP_Signal_iir.h"
#endif

#ifndef BQR_IIR_HIFI3_LIB
IIR_bq_form1_t iir_bqr_coef = {

    .total_gain = 10, // 20dB

    .biquad = {
        {
            .coef = {0x40370292, 0x800FDB45, 0x3FB924F7, 0x800FDB45, 0x3FF0278A},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
        {
            .coef = {0x412516A1, 0x8023C68C, 0x3EB7FBFE, 0x8023C68C, 0x3FDD129F},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
        {
            .coef = {0x011CA029, 0xFDDD283D, 0x010FCD4E, 0xFDDD283D, 0xC22C6D78},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
        {
            .coef = {0x408F0143, 0x810A8E38, 0x3E8B3F9C, 0x80FC4575, 0x3F0BF81C},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
        {
            .coef = {0x42E7DCEA, 0x83FE4E09, 0x39401E0F, 0x83FE4E09, 0x3C27FAFA},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
        {
            .coef = {0x43D5B16A, 0x855602FA, 0x3717AC76, 0x855602FA, 0x3AED5DE0},
            .x_pre = {0,0},
            .y_pre = {0,0},
        },
    },

};


void IIRFilterDirect1_process(ae_f32 *inout, ae_f32 *output_ptr, uint16_t size)
{
    ae_f32 *data_inout = inout;
    ae_f32 *iir_coef;
    ae_f32 *iir_x;
    ae_f32 *iir_y;

    // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
    for(uint16_t i=0; i<BIQUAD_FILTER_NUM; i++) {

        iir_coef = (ae_f32 *)iir_bqr_coef.biquad[i].coef;
        iir_x = (ae_f32 *)iir_bqr_coef.biquad[i].x_pre;
        iir_y = (ae_f32 *)iir_bqr_coef.biquad[i].y_pre;

        for(uint16_t j=0; j<size; j++) {

            ae_f32 y = iir_coef[0] * data_inout[j];
            y += iir_coef[1] * iir_x[0];
            y += iir_coef[2] * iir_x[1];

            y -= iir_coef[3] * iir_y[0];
            y -= iir_coef[4] * iir_y[1];

            y = y << 1;

            iir_x[1] = iir_x[0];
            iir_x[0] = data_inout[j];
            iir_y[1] = iir_y[0];
            iir_y[0] = y;

            data_inout[j] = y;
        }
    }

    for(uint16_t j=0; j<size; j++) {
        output_ptr[j] = data_inout[j] * (ae_f16)0x8000;
    }
}
#else

void *iir_bqr32x32_mem;
void *iir_bqr32x32_param;
void *iir_bqr32x32_scratch_mem;

/****************************************************************************
* Function Definitions
*****************************************************************************/

/*
 * API for iir bq filter(init)
 *
 * Input:
 * num_filter:  number of filter
 * coef: coeffs of filters size should be 5*num_filter
 *       b0, b1, b2, a1, a2
 * gain: total gain shift amount applied to output signal
 *       default is 0
 *
 * Output:
 * iir_param: point to iir filter params
 *
 * */
void *iir_bqr32x32_df1_init(IIR_bq_form1_t filter_param)
{
    iir_bqr32x32_mem = (void *)llfw_memory_zmalloc(bqriir32x32_df1_alloc(filter_param.biquad_num));
    if(!iir_bqr32x32_mem) {
        return NULL;
    }

    iir_bqr32x32_param = bqriir32x32_df1_init(iir_bqr32x32_mem, filter_param.biquad_num, filter_param.biquad, filter_param.gain_fr, filter_param.gain_sw);
    if(!iir_bqr32x32_param) {
        return NULL;
    }

    return iir_bqr32x32_param;
}

void *iir_bqr32x32_df1_reinit(IIR_bq_form1_t filter_param)
{
    llfw_memory_free(iir_bqr32x32_mem);
    if(!iir_bqr32x32_mem) {
        L0_DBG_MSG("iir_bqr32x32_df1_reinit  NULL \n");
        return NULL;
    }

    return iir_bqr32x32_df1_init(filter_param);
}

void IIRFilterDirect1_process(void *iir_param, ae_f32 *inout, uint16_t size)
{
    if(!iir_param) {
        L0_DBG_MSG("IIRFilterDirect1_process NULL \n");
        return;
    }

    bqriir32x32_df1(iir_param, iir_bqr32x32_scratch_mem, (int32_t *)inout, (int32_t *)inout, size);
}

#endif
