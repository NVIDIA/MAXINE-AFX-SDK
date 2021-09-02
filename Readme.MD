# Readme

NVIDIA MAXINE Audio Effects (AFX) SDK: API Source Code and Sample Applications

NVIDIA MAXINE AFX SDK provides various audio effects for broadcast use cases with real-time audio processing. Recordings of speech made outside of a recording studio can contain a lot of background noise, which causes the speech to be garbled and difficult to understand. This SDK comes with following audio effects to help clean the audio stream in real time and non real time use cases:
1. Background Noise Suppression (Denoising)
2. Room Echo Cancellation  (Dereverb)
3. Room Echo Cancellation + Background Noise Suppression (Dereverb + Denoiser)

The SDK is powered by NVIDIA RTX graphics processor units (GPUs) with Tensor Cores, thus the algorithm throughput is greatly accelerated, and the latency is reduced.

NVIDIA MAXINE AFX SDK is distributed in the following parts:

-  This open source repository that includes the [SDK API](https://github.com/NVIDIA/MAXINE-AFX-SDK/tree/v1.0.5/nvafx), and [sample applications and their dependency libraries](https://github.com/NVIDIA/MAXINE-AFX-SDK/tree/v1.0.5/samples).
- An installer hosted on [NVIDIA MAXINE End-user Redistributables Page](https://www.nvidia.com/broadcast-sdk-resources) that installs the SDK DLLs, the models, and the SDK dependency libraries.

Please refer to the [SDK programming guide](https://docs.nvidia.com/deeplearning/maxine/audio-effects-sdk-windows/index.html) for configuring and integrating the SDK, compiling and running the sample applications. Please visit the [NVIDIA MAXINE Audio Effects SDK](https://developer.nvidia.com/maxine-getting-started) webpage for more information about the SDK.

## System requirements
The SDK is supported on NVIDIA GPUs with Tensor Cores.
* Windows OS supported: 64-bit Windows 10
* Microsoft Visual Studio: 2015 (MSVC14.0) or later
* CMake: v3.9 or later
* NVIDIA Graphics Driver for Windows: 465.89 or later

##  NVIDIA MAXINE Branding Guidelines

If you integrate an NVIDIA MAXINE SDK within your product, please follow the required branding guidelines that are available here:
https://www.nvidia.com/maxine-sdk-guidelines

## Compiling the sample app

### Steps
1. Goto the root folder of the cloned repository and create a directory called "build".
2. Open command prompt window and navigate to the "build" directory.
3. Run the command "cmake-gui .." (Ensure cmake installation directory is in the path)
4. In the opened CMake GUI, select visual studio version with x64 architecture and complete the _Configuration_ and _Generate_ process.
5. Build the solution.
6. The output folder (Release/Debug) has the generated effects_demo binary and associated batch files to run against the installed redistributable.
