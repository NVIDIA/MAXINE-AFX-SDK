/*
* Copyright (c) 2022, NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA Corporation is strictly prohibited.
*/

#define _CRT_SECURE_NO_DEPRECATE

#include <sys/stat.h>

#include <cstdint>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <unistd.h>
#endif

#include "waveReadWrite.hpp"

const float * CWaveFileRead::GetFloatPCMData() {
  int8_t* audioDataPtr = reinterpret_cast<int8_t*>(m_WaveData.get());

  if (m_floatWaveData.get())
    return m_floatWaveData.get();

  m_floatWaveData.reset(new float[m_nNumSamples]);
  float* outputWaveData = m_floatWaveData.get();
  if (m_WaveFormatEx.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
    memcpy(outputWaveData, audioDataPtr, m_nNumSamples * sizeof(float));
    return outputWaveData;
  }

  for (uint32_t i = 0; i < m_nNumSamples; i++)   {
    switch (m_WaveFormatEx.wBitsPerSample)     {
    case 8: {
      uint8_t audioSample = *(reinterpret_cast<uint8_t*>(audioDataPtr));
      outputWaveData[i] = (audioSample - 128) / 128.0f;
    }
    break;
    case 16: {
      int16_t audioSample = *(reinterpret_cast<int16_t*>(audioDataPtr));
      outputWaveData[i] = audioSample / 32768.0f;
    }
    break;
    case 24: {
      int32_t audioSample = *(reinterpret_cast<int32_t*>(audioDataPtr));
      uint8_t data0 = audioSample & 0x000000ff;
      uint8_t data1 = static_cast<uint8_t>((audioSample & 0x0000ff00) >> 8);
      uint8_t data2 = static_cast<uint8_t>((audioSample & 0x00ff0000) >> 16);
      int32_t Value = ((data2 << 24) | (data1 << 16) | (data0 << 8)) >> 8;
      outputWaveData[i] = Value / 8388608.0f;
    }
    break;
    case 32: {
      int32_t audioSample = *(reinterpret_cast<int32_t*>(audioDataPtr));
      outputWaveData[i] = audioSample / 2147483648.0f;
    }
    break;
    }
    audioDataPtr += m_WaveFormatEx.nBlockAlign;
  }

  return outputWaveData;
}

const float * CWaveFileRead::GetFloatPCMDataAligned(int alignSamples) {
  if (!GetFloatPCMData())
    return nullptr;

  int totalAlignedSamples;
  if (!(m_nNumSamples % alignSamples))
    totalAlignedSamples = m_nNumSamples;
  else
    totalAlignedSamples = m_nNumSamples + (alignSamples - (m_nNumSamples % alignSamples));

  m_floatWaveDataAligned.reset(new float[totalAlignedSamples]());

  for (uint32_t i = 0; i < m_nNumSamples; i++)
    m_floatWaveDataAligned[i] = m_floatWaveData[i];

  m_NumAlignedSamples = totalAlignedSamples;
  return m_floatWaveDataAligned.get();
}

int CWaveFileRead::GetBitsPerSample() {
  if (m_WaveFormatEx.wBitsPerSample == 0)
    assert(0);

  return m_WaveFormatEx.wBitsPerSample;
}

const RiffChunk* CWaveFileRead::FindChunk(const uint8_t* data, size_t sizeBytes, uint32_t fourcc) {
  if (!data)
    return nullptr;

  const uint8_t* ptr = data;
  const uint8_t* end = data + sizeBytes;

  while (end > (ptr + sizeof(RiffChunk))) {
    const RiffChunk* header = reinterpret_cast<const RiffChunk*>(ptr);
    if (header->chunkId == fourcc)
      return header;

    ptr += (header->chunkSize + sizeof(RiffChunk));
  }

  return nullptr;
}

CWaveFileRead::CWaveFileRead(std::string wavFile)
  : m_wavFile(wavFile)
  , m_nNumSamples(0)
  , m_validFile(false)
  , m_floatWaveData(nullptr)
  , m_WaveDataSize(0)
  , m_NumAlignedSamples(0) {
  memset(&m_WaveFormatEx, 0, sizeof(m_WaveFormatEx));
#ifdef __linux__
  if (access(m_wavFile.c_str(), R_OK) == 0)
#else
  if (PathFileExistsA(m_wavFile.c_str()))
#endif
  {
    if (readPCM(m_wavFile.c_str()) == 0)
      m_validFile = true;
  }
}

inline bool loadFile(std::string const& infilename, std::string* outData) {
  std::string result;
  std::string filename = infilename;

  std::ifstream stream(filename.c_str(), std::ios::binary | std::ios::in);
  if (!stream.is_open()) {
    return false;
  }

  stream.seekg(0, std::ios::end);
  result.reserve(stream.tellg());
  stream.seekg(0, std::ios::beg);

  result.assign(
    (std::istreambuf_iterator<char>(stream)),
    std::istreambuf_iterator<char>());

  *outData = result;

  return true;
}

inline std::string loadFile(const std::string& infilename) {
  std::string result;
  loadFile(infilename, &result);

  return result;
}

int CWaveFileRead::readPCM(const char* szFileName) {
  std::string fileData;
  if (loadFile(std::string(szFileName), &fileData) != true) {
    return -1;
  }

  const uint8_t* waveData = reinterpret_cast<const uint8_t*>(fileData.data());
  size_t waveDataSize = fileData.length();
  const uint8_t* waveEnd = waveData + waveDataSize;


  // Locate RIFF 'WAVE'
  const RiffChunk* riffChunk = FindChunk(waveData, waveDataSize, MAKEFOURCC('R', 'I', 'F', 'F'));
  if (!riffChunk || riffChunk->chunkSize < 4) {
    return -1;
  }

  const RiffHeader* riffHeader = reinterpret_cast<const RiffHeader*>(riffChunk);
  if (riffHeader->fileTag != MAKEFOURCC('W', 'A', 'V', 'E')) {
    return -1;
  }

  // Locate 'fmt '
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RiffHeader);
  if ((ptr + sizeof(RiffChunk)) > waveEnd) {
    return -1;
  }

  const RiffChunk* fmtChunk = FindChunk(ptr, riffHeader->chunkSize, MAKEFOURCC('f', 'm', 't', ' '));
  if (!fmtChunk || fmtChunk->chunkSize < sizeof(waveFormat_basic)) {
    return -1;
  }

  ptr = reinterpret_cast<const uint8_t*>(fmtChunk) + sizeof(RiffChunk);
  if (ptr + fmtChunk->chunkSize > waveEnd) {
    return -1;
  }

  const waveFormat_basic_nopcm* wf = reinterpret_cast<const waveFormat_basic_nopcm*>(ptr);

  if (!(wf->formatTag == WAVE_FORMAT_PCM || wf->formatTag == WAVE_FORMAT_IEEE_FLOAT)) {
    if (wf->formatTag == WAVE_FORMAT_EXTENSIBLE) {
      printf("WAVE_FORMAT_EXTENSIBLE is not supported. Please convert\n");
    }

    return -1;
  }

  ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RiffHeader);
  if ((ptr + sizeof(RiffChunk)) > waveEnd) {
    return -1;
  }

  const RiffChunk* dataChunk = FindChunk(ptr, riffChunk->chunkSize, MAKEFOURCC('d', 'a', 't', 'a'));
  if (!dataChunk || !dataChunk->chunkSize) {
    return -1;
  }


  ptr = reinterpret_cast<const uint8_t*>(dataChunk) + sizeof(RiffChunk);
  if (ptr + dataChunk->chunkSize > waveEnd) {
    return -1;
  }

  m_WaveData = std::make_unique<uint8_t[]>(dataChunk->chunkSize);
  m_WaveDataSize = dataChunk->chunkSize;
  memcpy(m_WaveData.get(), ptr, dataChunk->chunkSize);
  if (wf->formatTag == WAVE_FORMAT_PCM) {
    memcpy(&m_WaveFormatEx, reinterpret_cast<const waveFormat_basic*>(wf), sizeof(waveFormat_basic));
    m_WaveFormatEx.cbSize = 0;
  } else {
    memcpy(&m_WaveFormatEx, reinterpret_cast<const waveFormat_ext*>(wf), sizeof(waveFormat_ext));
  }

  m_nNumSamples = m_WaveDataSize / (m_WaveFormatEx.nBlockAlign / m_WaveFormatEx.nChannels);

  return 0;
}

CWaveFileWrite::CWaveFileWrite(std::string wavFile, uint32_t samplesPerSec, uint32_t numChannels,
                               uint16_t bitsPerSample, bool isFloat)
  :m_wavFile(wavFile) {
  m_wfx.wFormatTag = isFloat ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
  m_wfx.nChannels = static_cast<uint16_t>(numChannels);
  m_wfx.nSamplesPerSec = samplesPerSec;
  m_wfx.nBlockAlign = static_cast<uint16_t>((numChannels * bitsPerSample) / 8);
  m_wfx.nAvgBytesPerSec = samplesPerSec * m_wfx.nBlockAlign;
  m_wfx.wBitsPerSample = bitsPerSample;
  m_wfx.cbSize = 0;

  m_validState = true;
}

CWaveFileWrite::~CWaveFileWrite() {
  if (m_commitDone == false)
    commitFile();

  if (m_fp) {
    fclose(m_fp);
    m_fp = nullptr;
  }
}

bool CWaveFileWrite::writeChunk(const void* data, uint32_t len) {
  if (!m_validState)
    return false;

  if (!m_fp) {
    m_fp = fopen(m_wavFile.c_str(), "wb");
    if (!m_fp)
      return false;

    int64_t offset = sizeof(RiffHeader) + sizeof(RiffChunk) +
      sizeof(waveFormat_basic) + sizeof(RiffChunk);
    if (fseek(m_fp, static_cast<long>(offset), SEEK_SET) != 0) {
      fclose(m_fp);
      m_fp = nullptr;
      return false;
    }
  }

  size_t written = fwrite(data, len, 1, m_fp);
  if (written != 1)
    return false;

  m_cumulativeCount += len;
  return true;
}

bool CWaveFileWrite::commitFile() {
  if (!m_validState)
    return false;

  if (!m_fp)
    return false;

  // pull fp to start of file to write headers.
  fseek(m_fp, 0, SEEK_SET);

  // write the riff chunk header
  uint32_t fmtChunkSize = sizeof(waveFormat_basic);
  RiffHeader riffHeader;
  riffHeader.chunkId = MAKEFOURCC('R', 'I', 'F', 'F');
  riffHeader.chunkSize = 4 + sizeof(RiffChunk) + sizeof(RiffChunk) + fmtChunkSize + m_cumulativeCount;
  riffHeader.fileTag = MAKEFOURCC('W', 'A', 'V', 'E');
  if (fwrite(&riffHeader, sizeof(riffHeader), 1, m_fp) != 1)
    return false;

  // fmt riff chunk
  RiffChunk fmtChunk;
  fmtChunk.chunkId = MAKEFOURCC('f', 'm', 't', ' ');
  fmtChunk.chunkSize = sizeof(waveFormat_basic);
  if (fwrite(&fmtChunk, sizeof(RiffChunk), 1, m_fp) != 1)
    return false;

  // fixme: try using WAVEFORMATEX for size
  if (fwrite(&m_wfx, sizeof(waveFormat_basic), 1, m_fp) != 1)
    return false;

  // data riff chunk
  RiffChunk dataChunk;
  dataChunk.chunkId = MAKEFOURCC('d', 'a', 't', 'a');
  dataChunk.chunkSize = m_cumulativeCount;
  if (fwrite(&dataChunk, sizeof(RiffChunk), 1, m_fp) != 1)
    return false;

  fclose(m_fp);
  m_fp = nullptr;

  m_commitDone = true;
  m_validState = false;
  return true;
}
