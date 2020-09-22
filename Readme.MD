Readme
======
This sample app goes along with NVIDIA Audio Effects SDK redistributable. The
redistributable helps applications which have integrated against Audio Effects SDK
to deploy SDK DLLs and models on user machines.

During development for NVIDIA partners:
Application X <=> NVIDIA Audio Effects SDK
On End User machine:
Application X <=> NVIDIA Audio Effects SDK redistributable

This sample app is developed to serve as an example of how an application
can use the installed redistributable.
Sample app <=> NVIDIA Audio Effects SDK redistributable

NVIDIA Audio Effects SDK redistributable comes as an executable which is installed on
end user machine. It contains the binaries required for Audio Effects SDK to work.
The key point to note here is that the redistributable sets an environment variable
named NVAFX_SDK_DIR during installation. This variable points to the directory where
the SDK was installed.

Folder structure for sample app
===============================
samples\denoise_wav:
 The sample app. This folder contains the prebuilt sample app "denoise_wav.exe".
 The batch files (run_*.bat) use the previously set environment variable
 NVAFX_SDK_DIR to set the paths necessary for denoise_wav.exe to run. Go ahead
 and run the bat files.
nvafx\include:
 NVIDIA Audio Effects SDK header file.
nvafx\lib:
 NVIDIA Audio Effects SDK library
 
Not that the binaries are part of the redistributable and need to be installed separately.

Compiling the sample app
========================
Steps to try the Audio Effects Redistributable SDK package. CMake is used to
generate the build system.

1. Goto root folder of sample app and create a directory "build".
2. Goto build directory. Open command prompt window and navigate to
   the "build" directory.
3. Run the command "cmake-gui .." (Ensure cmake installation directory is in the path)
4. In the opened CMake GUI, select visual studio version with X64 architecture and complete
   the Configuration and Generate procedure.
5. Build the solution.
6. The output folder (Release/Debug) has the generated denoise_wav binary and associated
   batch files to run against installed redistributable.
