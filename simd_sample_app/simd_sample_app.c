/**
 * \file simd_sample_app.c
 *
 * \brief Tests and compares KISS FFT to FFTW and KISS FFT with vectorization
 * \details See README
 *
 * \copyright Copyright (c) 2018, DornerWorks, Ltd. 
 * \license See LICENSE for full licensing and copying information
 */


/******************************************************************************
 *                                                                 Inclusions *
 ******************************************************************************/
#include "comparison_types.h"
#include "run_ffts.h"
#include "print_comparisons.h"

#include <stdlib.h>
#include <time.h>


/******************************************************************************
 *                                                                    Defines *
 ******************************************************************************/
#define NSEC_PER_SEC (1000000000u)


/******************************************************************************
 *                                                      Function declarations *
 ******************************************************************************/
void runFftComparisons(void);
void runFftrComparisons(void);

pfft_input_t getInputFft(void);
pfftr_input_t getInputFftr(void);

uint32_t getTimestampNs(void);


/******************************************************************************
 *                                                         External functions *
 ******************************************************************************/
int main(void)
{
   runFftComparisons();
   runFftrComparisons();

   return 0;
}


/******************************************************************************
 *                                                         Internal functions *
 ******************************************************************************/
void runFftComparisons(void)
{
   uint32_t time0, time1, time2, time3, time4;

   pfft_input_t fft_input = getInputFft();

   time0 = getTimestampNs();
   pfft_output_t fftw_fft_output = getOutputFftwFft(fft_input);
   time1 = getTimestampNs();
   pfft_output_t kiss_fft_plain_output = getOutputKissFftPlain(fft_input);
   time2 = getTimestampNs();
   pfft_output_t kiss_fft_sse_output = getOutputKissFftSse(fft_input);
   time3 = getTimestampNs();
   pfft_output_t kiss_fft_avx_output = getOutputKissFftAvx(fft_input);
   time4 = getTimestampNs();

   results_t master = {{.pfft=kiss_fft_plain_output}, "KISS FFT Plain", time2 - time1, NOT_VERBOSE};
   results_t results_array[NUM_FFT_COMPARISONS] = {
      {{.pfft=fftw_fft_output}, "FFTW FFT", time1 - time0, NOT_VERBOSE},
      {{.pfft=kiss_fft_sse_output}, "KISS FFT SSE", time3 - time2, VERBOSE},
      {{.pfft=kiss_fft_avx_output}, "KISS FFT AVX", time4 - time3, VERBOSE},
   };

   printFftComparisons(master, results_array);
}

void runFftrComparisons(void)
{
   uint32_t time0, time1, time2, time3, time4;

   pfftr_input_t fftr_input = getInputFftr();

   time0 = getTimestampNs();
   pfftr_output_t fftw_fftr_output = getOutputFftwFftr(fftr_input);
   time1 = getTimestampNs();
   pfftr_output_t kiss_fftr_plain_output = getOutputKissFftrPlain(fftr_input);
   time2 = getTimestampNs();
   pfftr_output_t kiss_fftr_sse_output = getOutputKissFftrSse(fftr_input);
   time3 = getTimestampNs();
   pfftr_output_t kiss_fftr_avx_output = getOutputKissFftrAvx(fftr_input);
   time4 = getTimestampNs();

   results_t master = {{.pfftr=kiss_fftr_plain_output}, "KISS FFTR Plain", time2 - time1, NOT_VERBOSE};
   results_t results_array[NUM_FFT_COMPARISONS] = {
      {{.pfftr=fftw_fftr_output}, "FFTW FFTR", time1 - time0, NOT_VERBOSE},
      {{.pfftr=kiss_fftr_sse_output}, "KISS FFTR SSE", time3 - time2, VERBOSE},
      {{.pfftr=kiss_fftr_avx_output}, "KISS FFTR AVX", time4 - time3, VERBOSE},
   };

   printFftrComparisons(master, results_array);
}

pfft_input_t getInputFft(void)
{
   static SIMD_ALIGNED fft_input_t input;

   for (uint32_t i = 0; i < FFT_LEN; i++)
   {
      input[i].r = (rand() % 65536) - 32768;
      input[i].i = (rand() % 65536) - 32768;
   }

   return &input;
}

pfftr_input_t getInputFftr(void)
{
   static SIMD_ALIGNED fftr_input_t input;

   for (uint32_t i = 0; i < FFTR_LEN; i++)
   {
      input[i] = (rand() % 65536) - 32768;
   }

   return &input;
}

uint32_t getTimestampNs(void)
{
    uint32_t time = 0;
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time = ((uint32_t)ts.tv_sec) * NSEC_PER_SEC + ((uint32_t)ts.tv_nsec);

    return time;
}

