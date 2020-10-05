# Readme

NVIDIA Broadcast Audio Effects (AFX) SDK: API Source Code and Sample Applications

NVIDIA Broadcast AFX SDK provides audio denoising for broadcast use cases with real-time audio processing. Recordings of speech made outside of a recording studio can contain a lot of background noise, which causes the speech to be garbled and difficult to understand. Audio denoising removes the background noise. The SDK is powered by NVIDIA RTX graphic processor units (GPUs) with Tensor Cores, so the algorithm throughput is greatly accelerated, and latency is reduced.

NVIDIA Broadcast AFX SDK is distributed in the following parts:

-  This open source repository that includes the [SDK API](https://github.com/NVIDIA/BROADCAST-AFX-SDK/tree/master/nvafx), and [sample applications and their dependency libraries](https://github.com/NVIDIA/BROADCAST-AFX-SDK/tree/master/samples).
- An installer hosted on [RTX broadcast engine developer page](https://www.nvidia.com/broadcast-sdk-resources) that installs the SDK DLLs, the models, and the SDK dependency libraries.

Please refer to the [SDK programming guide](https://github.com/NVIDIA/BROADCAST-AFX-SDK/blob/master/NVIDIA%20Audio%20Effects%20SDK%20Programming%20Guide.pdf) for configuring and integrating the SDK, compiling and running the sample applications. Please visit the [NVIDIA Audio Effects SDK](https://developer.nvidia.com/broadcast-engine/audio-effects-sdk) webpage for more information about the SDK.

## System requirements
The SDK is supported on NVIDIA GPUs with Tensor Cores.
* Windows OS supported: 64-bit Windows 10
* Microsoft Visual Studio: 2015 (MSVC14.0) or later
* CMake: v3.9 or later
* NVIDIA Graphics Driver for Windows: 455.95 or later

##  NVIDIA Branding Guidelines

If you integrate an NVIDIA Broadcast Engine SDK within your product, please follow the required branding guidelines that are available here:
https://nvidia.frontify.com/d/uAobRitG8H8B

## Compiling the sample app

### Steps
1. Goto the root folder of the cloned repository and create a directory called "build".
2. Open command prompt window and navigate to the "build" directory.
3. Run the command "cmake-gui .." (Ensure cmake installation directory is in the path)
4. In the opened CMake GUI, select visual studio version with x64 architecture and complete the _Configuration_ and _Generate_ process.
5. Build the solution.
6. The output folder (Release/Debug) has the generated denoise_wav binary and associated batch files to run against the installed redistributable.
