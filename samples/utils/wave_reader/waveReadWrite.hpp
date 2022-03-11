/*
* Copyright (c) 2022, NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA Corporation is strictly prohibited.
*/

#pragma once

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#endif
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

// 'EVAW' (little endian WAVE)
#define WAVE_WAVE 0x45564157
// 'FFIR' (little endian RIFF)
#define WAVE_RIFF 0x46464952
// 'tmf' (little endian fmt )
#define WAVE_FORMAT 0x20746D66
// 'atad' (little endian data)
#define WAVE_DATA 0x61746164

#define DEBUG 1

#if DEBUG
#define PRINTCONTROL(x) x;
#define WAVE_ZERO_ON_ALLOCATE 1
#else
#define PRINTCONTROL(x)
#define WAVE_ZERO_ON_ALLOCATE 0
#endif

#define MAX_CHANNELS 64

#define MAKEFOURCC(a, b, c, d) ((uint32_t)(((d) << 24) | ((c) << 16) | ((b) << 8) | (a)))

typedef struct {
  // Check ID
  int ckId;
  // Check size
  int cksize;
  // Wave ID
  int waveId;
  // Number of wave chunks
  int waveChunks;
} WaveHeader;

typedef struct {
  // Format type
  uint16_t formatTag;
  // Number of channels (i.e. mono, stereo...)
  uint16_t nChannels;
  // Sample rate
  unsigned int nSamplesPerSec;
  // For buffer estimation
  unsigned int nAvgBytesPerSec;
  // Block size of data
  uint16_t nBlockAlign;
} waveFormat_basic_nopcm;

typedef struct {
  // Format type
  uint16_t formatTag;
  // Number of channels (i.e. mono, stereo...)
  uint16_t nChannels;
  // Sample rate
  unsigned int nSamplesPerSec;
  // For buffer estimation
  unsigned int nAvgBytesPerSec;
  // Block size of data
  uint16_t nBlockAlign;
  // Number of bits per sample of mono data
  uint16_t wBitsPerSample;
} waveFormat_basic;

typedef struct {
  // Format type
  uint16_t wFormatTag;
  // Number of channels (i.e. mono, stereo...)
  uint16_t nChannels;
  // Sample rate
  unsigned int nSamplesPerSec;
  // For buffer estimation
  unsigned int nAvgBytesPerSec;
  // Block size of data
  uint16_t nBlockAlign;
  // Number of bits per sample of mono data
  uint16_t wBitsPerSample;
  // The count in bytes of the size of extra information (after cbSize)
  uint16_t cbSize;
} waveFormat_ext;

struct RiffHeader {
  // Chunk ID
  uint32_t chunkId;
  // Chunk size
  uint32_t chunkSize;
  // File tag
  uint32_t fileTag;
};

struct RiffChunk {
  // Chunk ID
  uint32_t chunkId;
  // Chunk size
  uint32_t chunkSize;
};

struct WaveFileInfo {
  // wave format extension pointer
  waveFormat_ext wfx;
  // audio data pointer
  uint8_t* audioData;
  // audio data size
  uint32_t audioDataSize;
};

enum WaveFileFlags {
  READ_WAVEFILE = 0,
  WRITE_WAVEFILE = 1
};

class CWaveFileRead {
 public:
   // Constructor
  explicit CWaveFileRead(std::string wavFile);
  // Returns sample rate of wav file
  uint32_t GetSampleRate() const { return m_WaveFormatEx.nSamplesPerSec; }
  // Returns size of wav data in bytes
  uint32_t GetRawPCMDataSizeInBytes() const { return m_WaveDataSize; }
  // Returns number of samples in wav file
  uint32_t GetNumSamples() const { return m_nNumSamples; }
  // Returns alligned number of samples in wav file
  uint32_t GetNumAlignedSamples() const { return m_NumAlignedSamples; }
  // Returns pointer to raw audio data in wav file
  uint8_t* GetRawPCMData() { return m_WaveData.get(); }
  // Returns float pointer to audio data in wav file
  const float *GetFloatPCMData();
  // Returns float pointer to aligned audio data in wav file
  const float *GetFloatPCMDataAligned(int alignSamples);
  // Returns wav file format
  waveFormat_ext& GetWaveFormat() { return m_WaveFormatEx; }
  // Returns number of bits per sample
  int GetBitsPerSample();
  // Returns true, if file provided is valid wav file
  bool isValid() const { return m_validFile; }

 private:
  // Finds appropriate chunk
  const RiffChunk* FindChunk(const uint8_t* data, size_t sizeBytes, uint32_t fourcc);
  // Load file and reads PCM data
  int readPCM(const char* szFileName);

 private:
  // Path to wav file
  std::string m_wavFile;
  // Number of samples variable
  uint32_t m_nNumSamples;
  // File Validation variable
  bool m_validFile;
  // Uint8 wave data pointer
  std::unique_ptr<uint8_t[]> m_WaveData;
  // Float wave data pointer
  std::unique_ptr<float[]> m_floatWaveData;
  // Wave data size
  uint32_t m_WaveDataSize;
  // Aligned float wav data pointer
  std::unique_ptr<float[]> m_floatWaveDataAligned;
  // Wave format extension
  waveFormat_ext m_WaveFormatEx;
  // Number of aligned samples
  uint32_t m_NumAlignedSamples;
};

class CWaveFileWrite {
 public:
  // Constructor
  CWaveFileWrite(std::string wavFile, uint32_t samplesPerSec, uint32_t numChannels,
                 uint16_t bitsPerSample, bool isFloat);
  // Destructor
  ~CWaveFileWrite();
  // Can be called 'n' times.
  bool writeChunk(const void *data, uint32_t len);
  // Commit file
  bool commitFile();
  // Returns write count 
  uint32_t getWrittenCount() { return m_cumulativeCount; }
 private:
  // State validation variable
  bool m_validState = false;
  // Path to wav file
  std::string m_wavFile;
  // File pointer
  FILE *m_fp = nullptr;
  // Cumulative count
  uint32_t m_cumulativeCount = 0;
  // Wave format extension
  waveFormat_ext m_wfx;
  // Commit check variable
  bool m_commitDone = false;
};
