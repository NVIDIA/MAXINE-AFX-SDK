cmake_minimum_required(VERSION 3.9.0)


project(NvAudioEffects_SDK CXX)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo")

# Require C++11 and disable non-standard extensions
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)

# Set common build path for all targets
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SDK_INCLUDES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/nvafx/include)
# Add target for NVAudioEffects
add_library(NVAudioEffects INTERFACE)
target_include_directories(NVAudioEffects INTERFACE ${SDK_INCLUDES_PATH})
target_link_libraries(NVAudioEffects INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/NVAudioEffects.lib)

set(ENABLE_SAMPLES TRUE)
add_subdirectory(samples)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT effects_demo)


