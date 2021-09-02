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

#include <cstring>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <chrono>
#include <thread>

#include <utils/wave_reader/waveReadWrite.hpp>
#include <utils/config_reader/ConfigReader.hpp>

#include <nvAudioEffects.h>

namespace {

const char kConfigEffectVariable[] = "effect";
const char kConfigFileInputVariable[] = "input_wav";
const char kConfigFileOutputVariable[] = "output_wav";
const char kConfigFileRTVariable[] = "real_time";
const char kConfigIntensityRatioVariable[] = "intensity_ratio";

/* model */
const char kConfigFileModelVariable[] = "model";
} // namespace

class EffectsDemoApp {
 public:
  bool run(const ConfigReader& config_reader);

 private:
  // Validate configuration data.
  bool validate_config(const ConfigReader& config_reader);
  // EffectsDemoApp intensity_ratio config
  float intensity_ratio_ = 1.0f;
  // inited from configuration
  bool real_time_ = false;
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

bool EffectsDemoApp::validate_config(const ConfigReader& config_reader)
{
  if (config_reader.IsConfigValueAvailable(kConfigEffectVariable) == false) {
    std::cerr << "No " << kConfigEffectVariable << " variable found" << std::endl;
    return false;
  }

  if (config_reader.IsConfigValueAvailable(kConfigFileModelVariable) == false) {
    std::cerr << "No " << kConfigFileModelVariable << " variable found" << std::endl;
    return false;
  }

  // Common params
  if (config_reader.IsConfigValueAvailable(kConfigFileInputVariable) == false) {
    std::cerr << "No " << kConfigFileInputVariable << " variable found" << std::endl;
    return false;
  }

  if (config_reader.IsConfigValueAvailable(kConfigFileOutputVariable) == false) {
    std::cerr << "No " << kConfigFileOutputVariable << " variable found" << std::endl;
    return false;
  }

  std::string real_time;
  if (config_reader.GetConfigValue(kConfigFileRTVariable, &real_time) == false) {
    std::cerr << "No " << kConfigFileRTVariable << " variable found" << std::endl;
    return false;
  }

  if (real_time[0] != '0') {
    real_time_ = true;
  }

  std::string intensity_ratio;
  float intensity_ratio_local;
  if (config_reader.GetConfigValue(kConfigIntensityRatioVariable, &intensity_ratio)) {
    intensity_ratio_local = std::strtof(intensity_ratio.c_str(), nullptr);
    if (intensity_ratio_local < 0.0f || intensity_ratio_local > 1.0f) {
      std::cerr << kConfigIntensityRatioVariable << " not supported" << std::endl;
      return false;
    }
  } else {
    std::cerr << "No " << kConfigIntensityRatioVariable << " variable found" << std::endl;
    return false;
  }
  intensity_ratio_ = intensity_ratio_local;
  std::cout << "intensity ratio is ::" << intensity_ratio_ << "!!" << std::endl;
  return true;
}

bool EffectsDemoApp::run(const ConfigReader& config_reader)
{
  if (validate_config(config_reader) == false)
    return false;

  if (real_time_) {
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
  for (int device: ret) {
    std::cout << "- " << device <<std::endl;
  }

  std::cout << "Loading effect" << " ... ";
  if (NvAFX_Load(handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_Load() failed" << std::endl;
    return false;
  }
  std::cout << "Done" << std::endl;
  uint32_t input_sample_rate, output_sample_rate;
  if (NvAFX_GetU32(handle, NVAFX_PARAM_INPUT_SAMPLE_RATE, &input_sample_rate) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_OUTPUT_SAMPLE_RATE, &output_sample_rate) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_GetU32() failed" << std::endl;
      return false;
  }

  unsigned num_input_channels, num_output_channels, num_input_samples_per_frame, num_output_samples_per_frame;
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_INPUT_CHANNELS, &num_input_channels) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_OUTPUT_CHANNELS, &num_output_channels) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_INPUT_SAMPLES_PER_FRAME, &num_input_samples_per_frame) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_NUM_OUTPUT_SAMPLES_PER_FRAME, &num_output_samples_per_frame) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_GetU32() failed" << std::endl;
      return false;
  }
  float intensity_ratio_local;
  if (NvAFX_GetFloat(handle, NVAFX_PARAM_INTENSITY_RATIO, &intensity_ratio_local) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetFloat() failed" << std::endl;
    return false;
  }

  std::cout << "  Effect properties          : " << std::endl
            << "  Input Sample rate          : " << input_sample_rate << std::endl
            << "  Output Sample rate         : " << output_sample_rate << std::endl
            << "  Input Channels             : " << num_input_channels << std::endl
            << "  Output Channels            : " << num_output_channels << std::endl
            << "  Input Samples per frame    : " << num_input_samples_per_frame << std::endl
            << "  Output Samples per frame   : " << num_output_samples_per_frame << std::endl
            << "  Intensity Ratio            : " << intensity_ratio_local << std::endl;

  std::string input_wav = config_reader.GetConfigValue(kConfigFileInputVariable);
  std::vector<float> audio_data;
  if (!ReadWavFile(input_wav, input_sample_rate, &audio_data, num_input_samples_per_frame)) {
    std::cerr << "Unable to read wav file: " << input_wav << std::endl;
    return false;
  }
  std::cout << "Input wav file: " << input_wav << std::endl
            << "Total " << audio_data.size() << " samples read" << std::endl;
  std::string output_wav = config_reader.GetConfigValue(kConfigFileOutputVariable);

  CWaveFileWrite wav_write(output_wav, output_sample_rate, num_output_channels, 32, true);
  float frame_in_secs = static_cast<float>(num_input_samples_per_frame) / static_cast<float>(input_sample_rate);
  float total_run_time = 0.f;
  float total_audio_duration = 0.f;
  float checkpoint = 0.1f;
  float expected_audio_duration = static_cast<float>(audio_data.size()) / static_cast<float>(input_sample_rate);
  auto frame = std::make_unique<float[]>(num_output_samples_per_frame);

  std::string progress_bar = "[          ] ";
  std::cout << "Processed: " << progress_bar << "0%\r";
  std::cout.flush();

  // wav data is already padded to align to num_samples_per_frame by ReadWavFile()
  for (size_t offset = 0; offset < audio_data.size(); offset += num_input_samples_per_frame) {

    auto start_tick = std::chrono::high_resolution_clock::now();
    const float* input[1];
    float* output[1];
    input[0] = &audio_data.data()[offset];
    output[0] = frame.get();
    if (NvAFX_Run(handle, input, output, num_input_samples_per_frame, num_input_channels) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_Run() failed" << std::endl;
      return false;
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

    wav_write.writeChunk(frame.get(), num_output_samples_per_frame * sizeof(float));

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
            << "Total " << audio_data.size() << " samples written" << std::endl;

  if (NvAFX_DestroyEffect(handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_Release() failed" << std::endl;
    return false;
  }

  return true;
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

int main(int argc, char *argv[]) {
  std::string config_file;
  try
  {
      ParseCommandLine(argc, argv, &config_file);

      ConfigReader config_reader;
      if (config_reader.Load(config_file) == false) {
          std::cerr << "Config file load failed" << std::endl;
          return -1;
      }

      EffectsDemoApp app;
      if (app.run(config_reader))
          return 0;
      else return -1;
  }
  catch (const std::exception& e)
  {
      std::cerr << "Exception Caught : " << e.what() << std::endl;
  }
  return 0;
}


