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

#include <string>

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM        0x0001
#endif
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW       0x0006
#define WAVE_FORMAT_MULAW      0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

#define WAVE_WAVE   0x45564157  // 'EVAW' (little endian WAVE)
#define WAVE_RIFF   0x46464952  // 'FFIR' (little endian RIFF)
#define WAVE_FORMAT 0x20746D66  // ' tmf' (little endian fmt )
#define WAVE_DATA   0x61746164  // 'atad' (little endian data)

#define DEBUG 1

#if DEBUG
#define PRINTCONTROL(x) x;
#define WAVE_ZERO_ON_ALLOCATE 1
#else
#define PRINTCONTROL(x)
#define WAVE_ZERO_ON_ALLOCATE 0
#endif

#define MAX_CHANNELS 64

typedef struct {
  int ckId;
  int cksize;
  int waveId;
  int waveChunks;
} WaveHeader;

typedef struct {
  uint16_t   formatTag;         /* format type */
  uint16_t   nChannels;         /* number of channels (i.e. mono, stereo...) */
  unsigned int     nSamplesPerSec;    /* sample rate */
  unsigned int     nAvgBytesPerSec;   /* for buffer estimation */
  uint16_t   nBlockAlign;       /* block size of data */
} waveFormat_basic_nopcm;

typedef struct {
  uint16_t   formatTag;         /* format type */
  uint16_t   nChannels;         /* number of channels (i.e. mono, stereo...) */
  unsigned int     nSamplesPerSec;    /* sample rate */
  unsigned int     nAvgBytesPerSec;   /* for buffer estimation */
  uint16_t   nBlockAlign;       /* block size of data */
  uint16_t   wBitsPerSample;    /* Number of bits per sample of mono data */
} waveFormat_basic;

typedef struct {
  uint16_t   wFormatTag;         /* format type */
  uint16_t   nChannels;         /* number of channels (i.e. mono, stereo...) */
  unsigned int     nSamplesPerSec;    /* sample rate */
  unsigned int     nAvgBytesPerSec;   /* for buffer estimation */
  uint16_t   nBlockAlign;       /* block size of data */
  uint16_t   wBitsPerSample;    /* Number of bits per sample of mono data */
  uint16_t   cbSize;      /* the count in bytes of the size of */
  /* extra information (after cbSize) */
} waveFormat_ext;

class Wave {
 public:
  Wave();
  explicit Wave(std::string filename);
  Wave(const Wave & argB);
  ~Wave();

  /* Tools for creating waves in software */
  Wave(unsigned int nSamples, unsigned int nChannels, unsigned int sampleRate, unsigned int bitDepth);

  // Special Constructor used for construction by extractChannel
  Wave(unsigned int nSamples, unsigned int sampleRate, unsigned int bitDepth, float * singleChannel);

  /* Tools for allowing waves to interact */
  Wave & operator -=(const Wave & argB);
  Wave & operator *=(float scale);
  void operator >> (int shiftSamples);
  void operator >> (float shiftSeconds);
  void operator <<(int shiftSamples);
  void operator <<(float shiftSeconds);

  float& operator[](int sampleIndex);
  float* getDataPtr(int channelIndex, int sampleIndex);

  Wave extractChannel(int channelIndex);
  void setChannel(int channelIndex, Wave* argB);

  unsigned int getNumSamples();
  unsigned int getNumChannels();
  unsigned int getBitDepth();
  unsigned int getSampleRate();

  void         writeFile(std::string filename);
  int          maxInt(int channelId);
  int          minInt(int channelId);
  float        maxFloat(int channelId);
  float        minFloat(int channelId);
  void         append(int numChannels, int numAppendedSamples, float ** buffers);

  void         normalize();

 private:
  unsigned int m_numChannels;
  unsigned int m_sampleRate;
  unsigned int m_bitDepth;
  unsigned int m_numSamples;
  unsigned int m_bufferAllocation;

  int  ** m_intData;
  float ** m_floatData;

  bool    m_PCMIntValid;
  bool    m_PCMFloatValid;

  bool allocateInt();
  bool allocateFloat();
  void reallocate();

  void freeInt();
  void freeFloat();

  void fillFloatFromInt();
  void fillIntFromFloat();
  void validFloat();

  void readFromFile(std::string filename);
};

