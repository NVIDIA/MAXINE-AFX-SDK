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

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <memory>
#include <string>
#include <vector>

#include "wave.hpp"

#define MAKEFOURCC(a, b, c, d) ( (uint32_t) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a)) )

struct RiffHeader {
  uint32_t chunkId;
  uint32_t chunkSize;
  uint32_t fileTag;
};

struct RiffChunk {
  uint32_t chunkId;
  uint32_t chunkSize;
};

struct WaveFileInfo {
  waveFormat_ext wfx;
  uint8_t* audioData;
  uint32_t audioDataSize;
};

enum WaveFileFlags {
  READ_WAVEFILE = 0,
  WRITE_WAVEFILE = 1
};

class CWaveFileRead {
 public:
  explicit CWaveFileRead(std::string wavFile);
  uint32_t GetSampleRate() const { return m_WaveFormatEx.nSamplesPerSec; }
  uint32_t GetRawPCMDataSizeInBytes() const { return m_WaveDataSize; }
  uint32_t GetNumSamples() const { return m_nNumSamples; }
  uint32_t GetNumAlignedSamples() const { return m_NumAlignedSamples; }
  uint8_t* GetRawPCMData() { return m_WaveData.get(); }
  const float *GetFloatPCMData();
  const float *GetFloatPCMDataAligned(int alignSamples);
  waveFormat_ext& GetWaveFormat() { return m_WaveFormatEx; }
  int GetBitsPerSample();
  bool isValid() const { return validFile; }

 private:
  const RiffChunk* FindChunk(const uint8_t* data, size_t sizeBytes, uint32_t fourcc);
  int readPCM(const char* szFileName);

 private:
  std::string m_wavFile;
  uint32_t m_nNumSamples;
  bool validFile;
  std::unique_ptr<uint8_t[]> m_WaveData;
  std::unique_ptr<float[]> m_floatWaveData;
  uint32_t m_WaveDataSize;
  std::unique_ptr<float[]> m_floatWaveDataAligned;
  waveFormat_ext m_WaveFormatEx;
  uint32_t m_NumAlignedSamples;
};

class CWaveFileWrite {
 public:
  CWaveFileWrite(std::string wavFile, uint32_t samplesPerSec, uint32_t numChannels,
                 uint16_t bitsPerSample, bool isFloat);
  ~CWaveFileWrite();
  // can be called 'n' times.
  bool writeChunk(const void *data, uint32_t len);
  bool commitFile();
  uint32_t getWrittenCount() { return m_cumulativeCount; }
 private:
  bool m_validState = false;
  std::string m_wavFile;
  FILE *m_fp = nullptr;
  uint32_t m_cumulativeCount = 0;
  waveFormat_ext wfx;
  bool m_commitDone = false;
};
