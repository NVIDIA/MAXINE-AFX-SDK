/*###############################################################################
#
# Copyright 2020 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################*/

#ifndef __NVAUDIOEFFECTS_H__
#define __NVAUDIOEFFECTS_H__

#ifdef WIN32
#if defined NVAFX_API_EXPORT
#define NVAFX_API __declspec(dllexport)
#else
#define NVAFX_API __declspec(dllimport)
#endif
#else
// Exports are controlled by version map for Linux, hence this is not needed
#define NVAFX_API 
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** API return values */
typedef enum {
  /** Success */
  NVAFX_STATUS_SUCCESS = 0,
  /** Failure */
  NVAFX_STATUS_FAILED = 1,
  /** Handle invalid */
  NVAFX_STATUS_INVALID_HANDLE = 2,
  /** Parameter value invalid */
  NVAFX_STATUS_INVALID_PARAM = 3,
  /** Parameter value immutable */
  NVAFX_STATUS_IMMUTABLE_PARAM = 4,
  /** Insufficient data to process */
  NVAFX_STATUS_INSUFFICIENT_DATA = 5,
  /** Effect not supported */
  NVAFX_STATUS_EFFECT_NOT_AVAILABLE = 6,
  /** Given buffer length too small to hold requested data */
  NVAFX_STATUS_OUTPUT_BUFFER_TOO_SMALL = 7,
  /** Model file could not be loaded */
  NVAFX_STATUS_MODEL_LOAD_FAILED = 8,

  /** (32 bit SDK only) COM server was not registered, please see user manual for details */
  NVAFX_STATUS_32_SERVER_NOT_REGISTERED = 9,
  /** (32 bit SDK only) COM operation failed */
  NVAFX_STATUS_32_COM_ERROR = 10,
  /** The selected GPU is not supported. The SDK requires Turing and above GPU with Tensor cores */
  NVAFX_STATUS_GPU_UNSUPPORTED = 11
} NvAFX_Status;

/** We use strings as effect selectors */
typedef const char* NvAFX_EffectSelector;

/** We use strings as parameter selectors. */
typedef const char* NvAFX_ParameterSelector;

/** Each effect instantiation is associated with an opaque handle. */
typedef void* NvAFX_Handle;

/** @brief Get a list of audio effects supported
 *
 * @param[out] num_effects Number of effects returned in effects array
 * @param[out] effects A list of effects returned by the API. This list is
 *                     statically allocated by the API implementation. Caller
 *                     does not need to allocate.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_GetEffectList(int* num_effects, NvAFX_EffectSelector* effects[]);

/** @brief Create a new instance of an audio effect.
 *
 * @param[in] code   The selector code for the desired audio Effect.
 * @param[out] effect   A handle to the Audio Effect instantiation.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_CreateEffect(NvAFX_EffectSelector code, NvAFX_Handle* effect);

/** @brief Delete a previously instantiated audio Effect.
 *
 * @param[in]  effect A handle to the audio Effect to be deleted.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_DestroyEffect(NvAFX_Handle effect);

/** Set the value of the selected parameter (unsigned int, char*)
 *
 * @param[in]  effect      The effect to configure.
 * @param[in]  param_name   The selector of the effect parameter to configure.
 * @param[in]  val         The value to be assigned to the selected effect parameter.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_SetU32(NvAFX_Handle effect, NvAFX_ParameterSelector param_name, unsigned int val);
NvAFX_Status NVAFX_API NvAFX_SetString(NvAFX_Handle effect, NvAFX_ParameterSelector param_name, const char* val);
NvAFX_Status NVAFX_API NvAFX_SetFloat(NvAFX_Handle effect, NvAFX_ParameterSelector param_name, float val);

/** Get the value of the selected parameter (unsigned int, char*)
*
* @param[in]  effect      The effect handle.
* @param[in]  param_name   The selector of the effect parameter to read.
* @param[out]  val         Buffer in which the parameter value will be assigned.
* @param[in]  max_length  The length in bytes of the buffer provided.
*
* @return Status values as enumerated in @ref NvAFX_Status
*/
NvAFX_Status NVAFX_API NvAFX_GetU32(NvAFX_Handle effect, NvAFX_ParameterSelector param_name, unsigned int* val);
NvAFX_Status NVAFX_API NvAFX_GetString(NvAFX_Handle effect, NvAFX_ParameterSelector param_name,
                                       char* val, int max_length);
NvAFX_Status NVAFX_API NvAFX_GetFloat(NvAFX_Handle effect, NvAFX_ParameterSelector param_name, float* val);

/** Load the Effect based on the set params.
 *
 * @param[in]  effect     The effect object handle.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_Load(NvAFX_Handle effect);

/** Get the devices supported by the model.
 *
 * @note This method must be called after setting model path.
 *
 * @param[in]      effect     The effect object handle.
 * @param[in,out]  num        The size of the input array. This value will be set by the function if call succeeds.
 * @param[in,out]  devices    Array of size num. The function will fill the array with CUDA device indices of devices
                              supported by the model, in descending order of preference (first = most preferred device)
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_GetSupportedDevices(NvAFX_Handle effect, int *num, int *devices);

/** Process the input buffer as per the effect selected. e.g. denoising
 *
 * @note The input float data is expected to be standard 32-bit float type with values in range [-1.0, +1.0]
 *
 * @param[in]  effect        The effect handle.
 * @param[in]  input         Input float buffer array. It points to an array of buffers where each buffer holds
 *                           audio data for a single channel. Array size should be same as number of channels
 *                           expected by the effect. Also ensure sampling rate is same as expected by the Effect.
 *                           For e.g. for denoiser it should be equal to the value returned by NvAFX_GetU32()
 *                           returned value for NVAFX_FIXED_PARAM_DENOISER_SAMPLE_RATE parameter.
 * @param[out]  output       Output float buffer array. The layout is same as input. It points to an an array of
 *                           buffers where buffer has audio data corresponding to that channel. The buffers have
 *                           to be preallocated by caller. Size of each buffer (i.e. channel) is same as that of
 *                           input.
 * @param[in]  num_samples   The number of samples in the input buffer. After this call returns output will
 *                           have same number of samples.
 * @param[in]  num_channels  The number of channels in the input buffer. The @a input and @a output should point
 *                           to @ num_channels number of buffers, one for each channel.
 *
 * @return Status values as enumerated in @ref NvAFX_Status
 */
NvAFX_Status NVAFX_API NvAFX_Run(NvAFX_Handle effect, const float** input, float** output,
                                 unsigned num_samples, unsigned num_channels);

/** Effect selectors. @ref NvAFX_EffectSelector */

/** Denoiser Effect */
#define NVAFX_EFFECT_DENOISER "denoiser"
/** Dereverb Effect */
#define NVAFX_EFFECT_DEREVERB "dereverb"
/** Dereverb Denoiser Effect */
#define NVAFX_EFFECT_DEREVERB_DENOISER "dereverb_denoiser"

/** Parameter selectors */

/** Common Effect parameters. */
/** To set if SDK should select the default GPU to run the effects in a Multi-GPU setup(unsigned int).
    Default value is 0. Please see user manual for details.*/
#define NVAFX_PARAM_USE_DEFAULT_GPU "use_default_gpu"

/** Effect parameters. @ref NvAFX_ParameterSelector */
/** Model path (char*) */
#define NVAFX_PARAM_MODEL_PATH "model_path"
/** Sample rate (unsigned int). Currently supported sample rate(s): 48000, 16000 */
#define NVAFX_PARAM_SAMPLE_RATE "sample_rate"
/** Number of samples per frame (unsigned int). This is immutable parameter */
#define NVAFX_PARAM_NUM_SAMPLES_PER_FRAME "num_samples_per_frame"
/** Number of channels in I/O (unsigned int). This is immutable parameter */
#define NVAFX_PARAM_NUM_CHANNELS "num_channels"
/** Effect intensity factor (float) */
#define NVAFX_PARAM_INTENSITY_RATIO "intensity_ratio"

/** Deprecated parameters */
#pragma deprecated(NVAFX_PARAM_DENOISER_MODEL_PATH)
#define NVAFX_PARAM_DENOISER_MODEL_PATH NVAFX_PARAM_MODEL_PATH
#pragma deprecated(NVAFX_PARAM_DENOISER_SAMPLE_RATE)
#define NVAFX_PARAM_DENOISER_SAMPLE_RATE NVAFX_PARAM_SAMPLE_RATE
#pragma deprecated(NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME)
#define NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME NVAFX_PARAM_NUM_SAMPLES_PER_FRAME
#pragma deprecated(NVAFX_PARAM_DENOISER_NUM_CHANNELS)
#define NVAFX_PARAM_DENOISER_NUM_CHANNELS NVAFX_PARAM_NUM_CHANNELS
#pragma deprecated(NVAFX_PARAM_DENOISER_INTENSITY_RATIO)
#define NVAFX_PARAM_DENOISER_INTENSITY_RATIO NVAFX_PARAM_INTENSITY_RATIO

#if defined(__cplusplus)
}
#endif

#endif  // __NVAUDIOEFFECTS_H__
