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
#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <utils/wave_reader/waveReadWrite.hpp>

bool ReadWavFile(const std::string& filename, uint32_t expected_sample_rate,
                 std::vector<float>* data, int align_samples = -1) {
  CWaveFileRead wave_file(filename);
  if (wave_file.isValid() == false) {
    return false;
  }
  const float* raw_data_array = wave_file.GetFloatPCMData();
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
  std::copy(raw_data_array, raw_data_array + wave_file.GetNumSamples(), data->data());
  return true;
}
