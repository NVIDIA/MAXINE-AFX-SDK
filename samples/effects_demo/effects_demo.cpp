/*###############################################################################
#
# Copyright 2022 NVIDIA Corporation
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

#include <cstring>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <chrono>
#include <thread>
#include <set>

#include <utils/wave_reader/waveReadWrite.hpp>
#include <utils/config_reader/ConfigReader.hpp>

#include <nvAudioEffects.h>

namespace {

const char kConfigEffectVariable[] = "effect";
const char kConfigFileInputVariable[] = "input_wav";
const char kConfigFileInputFarEndVariable[] = "input_farend_wav";
const char kConfigFileOutputVariable[] = "output_wav";
const char kConfigFileRTVariable[] = "real_time";
const char kConfigIntensityRatioVariable[] = "intensity_ratio";
const char kConfigVadEnable[] = "enable_vad";
const char kConfigFileModelVariable[] = "model";

} // namespace

std::vector<std::string> GetList(std::string effect, const char delimeter[] = ",")
{
  std::vector<std::string> List;
  char* effect_ = strdup(effect.c_str());
  char* token = std::strtok(effect_, delimeter);

  // loop until strtok() returns NULL
  while (token) {
    List.push_back(std::string(token));
    token = std::strtok(NULL, delimeter);
  }
  return List;
}

class EffectsDemoApp {
 public:
  bool run(const ConfigReader& config_reader, std::unordered_map<std::string, std::vector<std::string>>& map);
 private:
  // Validate configuration data.
  bool validate_config(const ConfigReader& config_reader, std::unordered_map<std::string, std::vector<std::string>>& map);
  bool chaining_run(const ConfigReader& config_reader,std::unordered_map<std::string, std::vector<std::string>>& map);
  bool generate_output(const ConfigReader& config_reader, NvAFX_Handle& handle_);
  // EffectsDemoApp intensity_ratio config
  float intensity_ratio_ = 1.0f;
  // inited from configuration
  bool real_time_ = false;
  // for aec effect only
  bool is_aec_ = false;
  // Inited from configuration
  bool vad_supported_ = false;
  //Model Params
  uint32_t input_sample_rate_ = 0;
  uint32_t output_sample_rate_ = 0;
  unsigned num_input_channels_ = 0;
  unsigned num_output_channels_ = 0;
  unsigned num_input_samples_per_frame_ = 0;
  unsigned num_output_samples_per_frame_ = 0;
};


bool ReadWavFile(const std::string& filename, uint32_t expected_sample_rate, std::vector<float>* data,
  int align_samples) {
  CWaveFileRead wave_file(filename);
  if (wave_file.isValid() == false) {
    return false;
  }
  std::cout << "Total number of samples: " << wave_file.GetNumSamples() << std::endl;
  std::cout << "Size in bytes: " << wave_file.GetRawPCMDataSizeInBytes() << std::endl;
  std::cout << "Sample rate: " << wave_file.GetSampleRate() << std::endl;

  auto bits_per_sample = wave_file.GetBitsPerSample();
  std::cout << "Bits/sample: " << bits_per_sample << std::endl;

  if (wave_file.GetSampleRate() != expected_sample_rate) {
    std::cout << "Sample rate mismatch" << std::endl;
    return false;
  }
  if (wave_file.GetWaveFormat().nChannels != 1) {
    std::cout << "Channel count needs to be 1" << std::endl;
    return false;
  }

  if (align_samples != -1) {
    int num_frames = wave_file.GetNumSamples() / align_samples;
    if (wave_file.GetNumSamples() % align_samples) {
      num_frames++;
    }

    // allocate potentially a bigger sized buffer to align it to requested
    data->resize(num_frames * align_samples);
  } else {
    data->resize(wave_file.GetNumSamples(), 0.f);
  }

  const float* raw_data_array = wave_file.GetFloatPCMData();
  std::copy(raw_data_array, raw_data_array + wave_file.GetNumSamples(), data->data());
  return true;
}

bool EffectsDemoApp::generate_output(const ConfigReader& config_reader, NvAFX_Handle& handle_) {
  std::string input_wav = config_reader.GetConfigValue(kConfigFileInputVariable);

  std::vector<float> audio_data;
  if (!ReadWavFile(input_wav, input_sample_rate_, &audio_data, num_input_samples_per_frame_)) {
    std::cerr << "Unable to read wav file: " << input_wav << std::endl;
    return false;
  }
  std::cout << "Input wav file: " << input_wav << std::endl
            << "Total " << audio_data.size() << " samples read" << std::endl;

  std::vector<float> farend_audio_data;
  if (is_aec_) {
    std::string input_farend_wav = config_reader.GetConfigValue(kConfigFileInputFarEndVariable);
    if (!ReadWavFile(input_farend_wav, input_sample_rate_, &farend_audio_data, num_input_samples_per_frame_)) {
      std::cerr << "Unable to read wav file: " << input_farend_wav << std::endl;
      return false;
    }
    std::cout << "Input wav file: " << input_farend_wav << std::endl
              << "Total " << farend_audio_data.size() << " samples read" << std::endl;
  }
  std::string output_wav = config_reader.GetConfigValue(kConfigFileOutputVariable);

  CWaveFileWrite wav_write(output_wav, output_sample_rate_, num_output_channels_, 32, true);

  float frame_in_secs = static_cast<float>(num_input_samples_per_frame_) / static_cast<float>(input_sample_rate_);
  float total_run_time = 0.f;
  float total_audio_duration = 0.f;
  float checkpoint = 0.1f;
  float expected_audio_duration = static_cast<float>(audio_data.size()) / static_cast<float>(input_sample_rate_);
  auto frame = std::make_unique<float[]>(num_output_samples_per_frame_);
  
  std::string progress_bar = "[          ] ";
  std::cout << "Processed: " << progress_bar << "0%\r";
  std::cout.flush();

  size_t final_audio_size = audio_data.size();
  //Taking the min size of farend and nearend if their sizes mismatch
  if (is_aec_) {
    if (audio_data.size() != farend_audio_data.size()) {
      final_audio_size = std::min(audio_data.size(), farend_audio_data.size());
    }
  }
  // wav data is already padded to align to num_samples_per_frame by ReadWavFile()
  for (size_t offset = 0; offset < final_audio_size; offset += num_input_samples_per_frame_) {
    auto start_tick = std::chrono::high_resolution_clock::now();
    if (is_aec_) {
      const float* input[2];
      float* output[1];
      input[0] = &audio_data.data()[offset];
      input[1] = &farend_audio_data.data()[offset];
      output[0] = frame.get();
      if (NvAFX_Run(handle_, input, output, num_input_samples_per_frame_, num_input_channels_) != NVAFX_STATUS_SUCCESS) {
        std::cerr << "NvAFX_Run() failed" << std::endl;
        return false;
      }
    } else {
      const float* input[1];
      float* output[1];
      input[0] = &audio_data.data()[offset];
      output[0] = frame.get();
      if (NvAFX_Run(handle_, input, output, num_input_samples_per_frame_, num_input_channels_) != NVAFX_STATUS_SUCCESS) {
        std::cerr << "NvAFX_Run() failed" << std::endl;
        return false;
      }
    }

    auto run_end_tick = std::chrono::high_resolution_clock::now();
    total_run_time += (std::chrono::duration<float>(run_end_tick - start_tick)).count();
    total_audio_duration += frame_in_secs;

    if ((total_audio_duration / expected_audio_duration) >= checkpoint) {
      progress_bar[(int)(checkpoint * 10.0f)] = '=';
      std::cout << "Processed: " << progress_bar << checkpoint * 100.f << "%" << (checkpoint >= 1 ? "\n" : "\r");
      std::cout.flush();
      checkpoint += 0.1f;
    }

      wav_write.writeChunk(frame.get(), num_output_samples_per_frame_ * sizeof(float));

    if (real_time_) {
      auto end_tick = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> elapsed = end_tick - start_tick;
      float sleep_time_secs = frame_in_secs - elapsed.count();
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep_time_secs * 1000)));
    }
  }

  std::cout << "Processing time " << std::setprecision(2) << total_run_time
            << " secs for " << total_audio_duration << std::setprecision(2)
            << " secs audio file (" << total_run_time / total_audio_duration
            << " secs processing time per sec of audio)" << std::endl;

  if (real_time_) {
    std::cout << "Note: App ran in real time mode i.e. simulated the input data rate of a mic" << std::endl
              << "'Processing time' could be less then actual run time" << std::endl;
  }

  wav_write.commitFile();
    std::cout << "Output wav file written. " << output_wav << std::endl
              << "Total " << audio_data.size() << " samples written"
              << std::endl;

  if (NvAFX_DestroyEffect(handle_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_DestroyEffect() failed" << std::endl;
    return false;
  }

  return true;
}

bool EffectsDemoApp::validate_config(const ConfigReader& config_reader, std::unordered_map<std::string, std::vector<std::string>>& map)
{
  if (config_reader.IsConfigValueAvailable(kConfigEffectVariable) == false) {
    std::cerr << "No " << kConfigEffectVariable << " variable found" << std::endl;
    return false;
  }
  std::string effect = config_reader.GetConfigValue(kConfigEffectVariable);
  if (strcmp(effect.c_str(), "aec") == 0) is_aec_ = true;
  std::vector<std::string> effects = GetList(effect);
  map[kConfigEffectVariable] = effects;


  if (config_reader.IsConfigValueAvailable(kConfigFileModelVariable) == false) {
    std::cerr << "No " << kConfigFileModelVariable << " variable found" << std::endl;
    return false;
  }
  std::string model = config_reader.GetConfigValue(kConfigFileModelVariable);
  map[kConfigFileModelVariable] = GetList(model);

  // Common params
  if (config_reader.IsConfigValueAvailable(kConfigFileInputVariable) == false) {
    std::cerr << "No " << kConfigFileInputVariable << " variable found" << std::endl;
    return false;
  }
  std::string input = config_reader.GetConfigValue(kConfigFileInputVariable);
  map[kConfigFileInputVariable] = GetList(input);

  if (config_reader.IsConfigValueAvailable(kConfigFileOutputVariable) == false) {
    std::cerr << "No " << kConfigFileOutputVariable << " variable found" << std::endl;
    return false;
  }
  std::string output = config_reader.GetConfigValue(kConfigFileOutputVariable);
  map[kConfigFileOutputVariable] = GetList(output);

  if (is_aec_) {
    if (config_reader.IsConfigValueAvailable(kConfigFileInputFarEndVariable) == false) {
      std::cerr << "No " << kConfigFileInputFarEndVariable << " variable found" << std::endl;
      return false;
    }
    std::string input_farend = config_reader.GetConfigValue(kConfigFileInputFarEndVariable);
    map[kConfigFileInputFarEndVariable] = GetList(input_farend);
  }

  std::string real_time;
  if (config_reader.GetConfigValue(kConfigFileRTVariable, &real_time) == false) {
    std::cerr << "No " << kConfigFileRTVariable << " variable found" << std::endl;
    return false;
  }
  map[kConfigFileRTVariable] = GetList(real_time);
  std::string temp = "1";
  if (map[kConfigFileRTVariable][0] == temp) {
    real_time_ = true;
  }

  //VAD is not supported for chaining.
  if (map[kConfigFileModelVariable].size() == 1) {
    const std::set<std::string> kVadSupportedEffects = { "denoiser", "dereverb_denoiser" };
    //VAD Checking
      if (kVadSupportedEffects.find(map[kConfigEffectVariable][0]) != kVadSupportedEffects.end()) {
        std::string vad_supported;
        if (config_reader.GetConfigValue(kConfigVadEnable, &vad_supported) == false) {
          std::cerr << "No " << kConfigVadEnable << " variable found" << std::endl;
          return false;
        }
        map[kConfigVadEnable] = GetList(vad_supported);
        vad_supported_ = std::strtof(map[kConfigVadEnable][0].c_str(), nullptr);
      }
  }

  std::string intensity_ratio;
  if (config_reader.GetConfigValue(kConfigIntensityRatioVariable,
                                    &intensity_ratio) == false) {
    std::cerr << "No " << kConfigIntensityRatioVariable << " variable found"
              << std::endl;
    return false;
  }
  map[kConfigIntensityRatioVariable] = GetList(intensity_ratio);

  // Intensity Ratio Checking
  for (int i = 0; i < map[kConfigFileModelVariable].size(); i++) {
    float intensity_ratio_local =
        std::strtof(map[kConfigIntensityRatioVariable][i].c_str(), nullptr);
    intensity_ratio_ = intensity_ratio_local;
    if (intensity_ratio_local < 0.0f || intensity_ratio_local > 1.0f) {
      std::cerr << kConfigIntensityRatioVariable << " not supported"
                << std::endl;
      return false;
    }
  }
  return true;
}

bool EffectsDemoApp::chaining_run(const ConfigReader& config_reader,std::unordered_map<std::string, std::vector<std::string>>& map)
{
  NvAFX_Handle chained_handle = nullptr;
  std::string effect = map[kConfigEffectVariable][0];
  if (strcmp(effect.c_str(), "denoiser16k_superres16kto48k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_DENOISER_16k_SUPERRES_16k_TO_48k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "dereverb16k_superres16kto48k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_DEREVERB_16k_SUPERRES_16k_TO_48k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "dereverb_denoiser16k_superres16kto48k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_DEREVERB_DENOISER_16k_SUPERRES_16k_TO_48k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "superres8kto16k_denoiser16k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_SUPERRES_8k_TO_16k_DENOISER_16k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "superres8kto16k_dereverb16k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_SUPERRES_8k_TO_16k_DEREVERB_16k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "superres8kto16k_dereverb_denoiser16k") == 0) {
    if (NvAFX_CreateChainedEffect(NVAFX_CHAINED_EFFECT_SUPERRES_8k_TO_16k_DEREVERB_DENOISER_16k, &chained_handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateChainedEffect() failed" << std::endl;
      return false;
    }
  } else {
    std::cerr << "NvAFX_CreateChainedEffect() failed. Invalid Effect Value : " << effect << std::endl;
    return false;
  }
  const char* model[] = {map[kConfigFileModelVariable][0].c_str(), map[kConfigFileModelVariable][1].c_str()};

  if (NvAFX_SetStringList(chained_handle, NVAFX_PARAM_MODEL_PATH, model, map[kConfigFileModelVariable].size())
      != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetStringList() failed" << std::endl;
    return false;
  }
  float intensity_ratio[2] = { std::strtof(map[kConfigIntensityRatioVariable][0].c_str(), nullptr),
                               std::strtof(map[kConfigIntensityRatioVariable][1].c_str(), nullptr) };

  if (NvAFX_SetFloatList(chained_handle, NVAFX_PARAM_INTENSITY_RATIO, intensity_ratio, map[kConfigFileModelVariable].size()) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetFloatList(Intensity Ratio: " << intensity_ratio_ << ") failed" << std::endl;
  }

  std::cout << "Loading effect" << " ... ";
  if (NvAFX_Load(chained_handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_Load() failed" << std::endl;
    return false;
  }
  std::cout << "Done" << std::endl;

  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_INPUT_SAMPLE_RATE, &input_sample_rate_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_OUTPUT_SAMPLE_RATE, &output_sample_rate_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }

  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_NUM_INPUT_CHANNELS, &num_input_channels_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_NUM_OUTPUT_CHANNELS, &num_output_channels_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_NUM_INPUT_SAMPLES_PER_FRAME, &num_input_samples_per_frame_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(chained_handle, NVAFX_PARAM_NUM_OUTPUT_SAMPLES_PER_FRAME, &num_output_samples_per_frame_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  float intensity_ratio_local[2];
  if (NvAFX_GetFloatList(chained_handle, NVAFX_PARAM_INTENSITY_RATIO, intensity_ratio_local, 2) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetFloatList() failed" << std::endl;
    return false;
  }
  std::cout << "  Effect properties            : " << std::endl
            << "  Input Sample rate            : " << input_sample_rate_ << std::endl
            << "  Output Sample rate           : " << output_sample_rate_ << std::endl
            << "  Input Channels               : " << num_input_channels_ << std::endl
            << "  Output Channels              : " << num_output_channels_ << std::endl
            << "  Input Samples per frame      : " << num_input_samples_per_frame_ << std::endl
            << "  Output Samples per frame     : " << num_output_samples_per_frame_ << std::endl
            << "  Intensity Ratio for Effect 1 : " << intensity_ratio_local[0] << std::endl
            << "  Intensity Ratio for Effect 2 : " << intensity_ratio_local[1] << std::endl;

  return (generate_output(config_reader, chained_handle));
}

bool EffectsDemoApp::run(const ConfigReader& config_reader, std::unordered_map<std::string, std::vector<std::string>>& map)
{
  if (validate_config(config_reader, map) == false)
    return false;

  if (real_time_ == true) {
    std::cout << "App will run in real time mode ..." << std::endl;
  }

  int num_effects;
  NvAFX_EffectSelector* effects;
  if (NvAFX_GetEffectList(&num_effects, &effects) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetEffectList() failed" << std::endl;
    return false;
  }
  std::cout << "Total Effects supported: " << num_effects << std::endl;
  for (int i = 0; i < num_effects; ++i) {
    std::cout << "(" << i + 1 << ") " << effects[i] << std::endl;
  }
  // Checking for Chaining
  if (map[kConfigFileModelVariable].size() == 2) {
    return chaining_run(config_reader,map);
  }

  NvAFX_Handle handle;
  std::string effect = config_reader.GetConfigValue(kConfigEffectVariable);
  if (strcmp(effect.c_str(), "denoiser") == 0) {
    if (NvAFX_CreateEffect(NVAFX_EFFECT_DENOISER, &handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "dereverb") == 0) {
    if (NvAFX_CreateEffect(NVAFX_EFFECT_DEREVERB, &handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "dereverb_denoiser") == 0) {
    if (NvAFX_CreateEffect(NVAFX_EFFECT_DEREVERB_DENOISER, &handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "aec") == 0) {
    if (NvAFX_CreateEffect(NVAFX_EFFECT_AEC, &handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
      return false;
    }
  } else if (strcmp(effect.c_str(), "superres") == 0) {
    if (NvAFX_CreateEffect(NVAFX_EFFECT_SUPERRES, &handle) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
      return false;
    }
  } else {
    std::cerr << "NvAFX_CreateEffect() failed. Invalid Effect Value : " << effect << std::endl;
    return false;
  }

  // If the system has multiple supported GPUs, then the application can either
  // use CUDA driver APIs or CUDA runtime APIs to enumerate the GPUs and select one based on the application's requirements
  // or offload the responsibility to SDK to select the GPU by setting NVAFX_PARAM_USE_DEFAULT_GPU as 1
  /*if (NvAFX_SetU32(handle, NVAFX_PARAM_USE_DEFAULT_GPU, 1) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetBool(NVAFX_PARAM_USE_DEFAULT_GPU " << ") failed" << std::endl;
    return false;
  }*/

  std::string model_file = config_reader.GetConfigValue(kConfigFileModelVariable);
  if (NvAFX_SetString(handle, NVAFX_PARAM_MODEL_PATH, model_file.c_str())
    != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetString() failed" << std::endl;
    return false;
  }

  if (NvAFX_SetFloat(handle, NVAFX_PARAM_INTENSITY_RATIO, intensity_ratio_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetFloat(Intensity Ratio: " << intensity_ratio_ << ") failed" << std::endl;
  }

  // Enabling VAD based on SDK user input!
  if (NvAFX_SetU32(handle, NVAFX_PARAM_ENABLE_VAD, vad_supported_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "Could not initialize VAD" << std::endl;
  }
  
  // Another option could be to use cudaGetDeviceCount for num
  int num_supported_devices = 0;
  if (NvAFX_GetSupportedDevices(handle, &num_supported_devices, nullptr) != NVAFX_STATUS_OUTPUT_BUFFER_TOO_SMALL) {
    std::cerr << "Could not get number of supported devices" << std::endl;
    return false;
  }

  std::cout << "Number of supported devices for this model: " << num_supported_devices << std::endl;

  std::vector<int> ret(num_supported_devices);
  if (NvAFX_GetSupportedDevices(handle, &num_supported_devices, ret.data()) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "No supported devices found" << std::endl;
    return false;
  }

  std::cout << "Devices supported (sorted by preference)" << std::endl;
  for (int device : ret) {
    std::cout << "- " << device << std::endl;
  }

  std::cout << "Loading effect" << " ... ";
  if (NvAFX_Load(handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_Load() failed" << std::endl;
    return false;
  }
  std::cout << "Done" << std::endl;

  if (NvAFX_GetU32(handle, NVAFX_PARAM_INPUT_SAMPLE_RATE, &input_sample_rate_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_OUTPUT_SAMPLE_RATE, &output_sample_rate_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_INPUT_CHANNELS, &num_input_channels_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_OUTPUT_CHANNELS, &num_output_channels_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_INPUT_SAMPLES_PER_FRAME, &num_input_samples_per_frame_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_OUTPUT_SAMPLES_PER_FRAME, &num_output_samples_per_frame_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }

  unsigned vad_enabled_local;
  if (NvAFX_GetU32(handle, NVAFX_PARAM_ENABLE_VAD, &vad_enabled_local) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_GetU32() failed" << std::endl;
      return false;
  }
  float intensity_ratio_local;
  if (NvAFX_GetFloat(handle, NVAFX_PARAM_INTENSITY_RATIO, &intensity_ratio_local) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetFloat() failed" << std::endl;
    return false;
  }

  std::cout << "  Effect properties          : " << std::endl
            << "  Input Sample rate          : " << input_sample_rate_ << std::endl
            << "  Output Sample rate         : " << output_sample_rate_ << std::endl
            << "  Input Channels             : " << num_input_channels_ << std::endl
            << "  Output Channels            : " << num_output_channels_ << std::endl
            << "  Input Samples per frame    : " << num_input_samples_per_frame_ << std::endl
            << "  Output Samples per frame   : " << num_output_samples_per_frame_ << std::endl
            << "  Intensity Ratio            : " << intensity_ratio_local << std::endl
            << "  Enable VAD                 : " << vad_enabled_local << std::endl;

  return (generate_output(config_reader, handle));
}

void ShowHelpAndExit(const char* bad_option) {
  std::ostringstream oss;
  if (bad_option) {
    oss << "Error parsing \"" << bad_option << "\"" << std::endl;
  }
  std::cout << "Command Line Options:" << std::endl
            << "-c Config file" << std::endl;
}


#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

void ParseCommandLine(int argc, char* argv[], std::string* config_file) {
  if (argc == 1) {
    ShowHelpAndExit(nullptr);
  }

  for (int i = 1; i < argc; i++) {
    if (!strcasecmp(argv[i], "-h")) {
      ShowHelpAndExit(nullptr);
    }
    if (!strcasecmp(argv[i], "-c")) {
      if (++i == argc || !config_file->empty()) {
        ShowHelpAndExit("-f");
      }
      config_file->assign(argv[i]);
      continue;
    }

    ShowHelpAndExit(argv[i]);
  }
}

int main(int argc, char* argv[]) {
  std::string config_file;
  try
  {
    ParseCommandLine(argc, argv, &config_file);

    ConfigReader config_reader;
    if (config_reader.Load(config_file) == false) {
      std::cerr << "Config file load failed" << std::endl;
      return -1;
    }
    std::unordered_map<std::string, std::vector<std::string>> effectConfigMap;
    EffectsDemoApp app;
    if (app.run(config_reader, effectConfigMap))
      return 0;
    else return -1;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception Caught : " << e.what() << std::endl;
  }
  return 0;
}


