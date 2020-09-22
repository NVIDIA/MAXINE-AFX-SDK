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

#include <utils/ReadWavFile.hpp>
#include <utils/ConfigReader.hpp>

#include <nvAudioEffects.h>

namespace {
const char kConfigSampleRateVariable[] = "sample_rate";
const char kConfigFileInputVariable[] = "input_wav";
const char kConfigFileOutputVariable[] = "output_wav";
const char kConfigFileRTVariable[] = "real_time";
const char kConfigIntensityRatioVariable[] = "intensity_ratio";

/* model */
const char kConfigFileModelVariable[] = "filter_model";
/* allowed sample rates */
const std::vector<uint32_t> kAllowedSampleRates = { 16000, 48000 };
} // namespace

class DenoiserApp {
 public:
  bool run(const ConfigReader& config_reader);

 private:
  // Validate configuration data.
  bool validate_config(const ConfigReader& config_reader);

 private:
  // Denoiser sample rate config
  uint32_t sample_rate_ = 0;
  // Denoiser intensity_ratio config
  float intensity_ratio_ = 1.0f;
  // inited from configuration
  bool real_time_ = false;
};

bool DenoiserApp::validate_config(const ConfigReader& config_reader)
{
  std::string sample_rate_str;
  if (config_reader.GetConfigValue(kConfigSampleRateVariable, &sample_rate_str)) {
    sample_rate_ = std::strtoul(sample_rate_str.c_str(), nullptr, 0);
    if (std::find(kAllowedSampleRates.begin(), kAllowedSampleRates.end(), sample_rate_) == kAllowedSampleRates.end()) {
      std::cerr << "Sample rate " << sample_rate_ << " not supported" << std::endl;
      return false;
    }
  } else {
    std::cerr << "No " << kConfigSampleRateVariable << " variable found" << std::endl;
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

bool DenoiserApp::run(const ConfigReader& config_reader)
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
  if (NvAFX_CreateEffect(NVAFX_EFFECT_DENOISER, &handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_CreateEffect() failed" << std::endl;
    return false;
  }

  if (NvAFX_SetU32(handle, NVAFX_PARAM_DENOISER_SAMPLE_RATE, sample_rate_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetU32(Sample Rate: " << sample_rate_ << ") failed" << std::endl;
    return false;
  }

  if (NvAFX_SetFloat(handle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO, intensity_ratio_) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetFloat(Intensity Ratio: " << intensity_ratio_ << ") failed" << std::endl;
    return false;
  }
  std::string filter_model_file = config_reader.GetConfigValue(kConfigFileModelVariable);
  if (NvAFX_SetString(handle, NVAFX_PARAM_DENOISER_MODEL_PATH, filter_model_file.c_str())
      != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_SetString() failed" << std::endl;
    return false;
  }

  std::string input_wav = config_reader.GetConfigValue(kConfigFileInputVariable);
  std::vector<float> audio_data;
  std::cout << "Loading effect" << " ... ";
  if (NvAFX_Load(handle) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_Load() failed" << std::endl;
    return false;
  }
  std::cout << "Done" << std::endl;
  unsigned num_channels, num_samples_per_frame;
  if (NvAFX_GetU32(handle, NVAFX_PARAM_DENOISER_NUM_CHANNELS, &num_channels) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  if (NvAFX_GetU32(handle, NVAFX_PARAM_DENOISER_NUM_SAMPLES_PER_FRAME, &num_samples_per_frame)
      != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetU32() failed" << std::endl;
    return false;
  }
  float intensity_ratio_local;
  if (NvAFX_GetFloat(handle, NVAFX_PARAM_DENOISER_INTENSITY_RATIO, &intensity_ratio_local) != NVAFX_STATUS_SUCCESS) {
    std::cerr << "NvAFX_GetFloat() failed" << std::endl;
    return false;
  }

  std::cout << "Denoiser properties:" << std::endl
    << "  Channels            : " << num_channels << std::endl
    << "  Samples per frame   : " << num_samples_per_frame << std::endl
    << "  Intensity Ratio   : " << intensity_ratio_local << std::endl;

  if (!ReadWavFile(input_wav, sample_rate_, &audio_data, num_samples_per_frame)) {
    std::cerr << "Unable to read wav file: " << input_wav << std::endl;
    return false;
  }
  std::cout << "Input wav file: " << input_wav << std::endl
    << "Total " << audio_data.size() << " samples read" << std::endl;


  std::string output_wav = config_reader.GetConfigValue(kConfigFileOutputVariable);

  CWaveFileWrite wav_write(output_wav, sample_rate_, num_channels, 32, true);
  float frame_in_secs = static_cast<float>(num_samples_per_frame) / static_cast<float>(sample_rate_);
  float total_run_time = 0.f;
  float total_audio_duration = 0.f;
  float checkpoint = 0.1f;
  float expected_audio_duration = static_cast<float>(audio_data.size()) / static_cast<float>(sample_rate_);
  auto frame = std::make_unique<float[]>(num_samples_per_frame);

  std::string progress_bar = "[          ] ";
  std::cout << "Processed: " << progress_bar << "0%\r";
  std::cout.flush();

  // wav data is already padded to align to num_samples_per_frame by ReadWavFile()
  for (size_t offset = 0; offset < audio_data.size(); offset += num_samples_per_frame) {
    const float* input[1];
    float* output[1];
    input[0] = &audio_data.data()[offset];
    output[0] = frame.get();

    auto start_tick = std::chrono::high_resolution_clock::now();
    if (NvAFX_Run(handle, input, output, num_samples_per_frame, num_channels) != NVAFX_STATUS_SUCCESS) {
      std::cerr << "NvAFX_Run() failed" << std::endl;
      return false;
    }

    auto run_end_tick = std::chrono::high_resolution_clock::now();
    total_run_time += (std::chrono::duration<float>(run_end_tick - start_tick)).count();
    total_audio_duration += frame_in_secs;

    if ((total_audio_duration / expected_audio_duration) > checkpoint) {
      progress_bar[checkpoint * 10] = '=';
      std::cout << "Processed: " << progress_bar << checkpoint * 100.f << "%" << (checkpoint >= 1 ? "\n" : "\r");
      std::cout.flush();
      checkpoint += 0.1f;
    }

    wav_write.writeChunk(frame.get(), num_samples_per_frame * sizeof(float));

    if (real_time_) {
      auto end_tick = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> elapsed = end_tick - start_tick;
      float sleep_time_secs = frame_in_secs - elapsed.count();
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep_time_secs * 1000)));
    }
  }

  std::cout << "Processing time " << std::setprecision(2) << total_run_time << " secs for "
    << total_audio_duration << std::setprecision(2) << " secs audio file (" << total_run_time / total_audio_duration
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

void ShowHelpAndExit(const char* szBadOption) {
  std::ostringstream oss;
  bool bThrowError = false;
  if (szBadOption) {
    bThrowError = false;
    oss << "Error parsing \"" << szBadOption << "\"" << std::endl;
  }
  std::cout << "Command Line Options:" << std::endl
    << "-c           Config file" << std::endl;

  if (bThrowError) {
    throw std::invalid_argument(oss.str());
  } else {
    std::cout << oss.str();
    exit(0);
  }
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
  ParseCommandLine(argc, argv, &config_file);

  ConfigReader config_reader;
  if (config_reader.Load(config_file) == false) {
    std::cerr << "Config file load failed" << std::endl;
    return -1;
  }

  DenoiserApp app;
  if (app.run(config_reader))
    return 0;
  else return -1;
}


