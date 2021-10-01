set(SOURCE_FILES effects_demo.cpp)
set(AUDIOFX_SDK_UTILS_SRCS ../utils/wave_reader/waveReadWrite.cpp
                           ../utils/wave_reader/waveReadWrite.hpp
						   ../utils/config_reader/ConfigReader.cpp
						   ../utils/config_reader/ConfigReader.hpp)
						   
# Set Visual Studio source filters
source_group("Source Files" FILES ${SOURCE_FILES} ${AUDIOFX_SDK_UTILS_SRCS})

add_executable(effects_demo ${SOURCE_FILES} ${AUDIOFX_SDK_UTILS_SRCS})
target_include_directories(effects_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(effects_demo PUBLIC ${SDK_INCLUDES_PATH})
target_include_directories(effects_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(effects_demo PUBLIC
	NVAudioEffects
)

add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin/cublas64_11.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin/cublasLt64_11.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin/cudart64_110.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin/cufft64_10.dll
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin/nvrtc64_112_0.dll
	#${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/nvtrt/bin/myelin64_1.dll
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/nvtrt/bin/nvinfer.dll
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/nvtrt/bin/nvinfer_plugin.dll
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/nvtrt/bin/nvonnxparser.dll
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/openssl/bin/libcrypto-1_1-x64.dll
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/NVAudioEffects.dll
	$<TARGET_FILE_DIR:effects_demo>
)

add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/../../bin/models $<TARGET_FILE_DIR:effects_demo>/models)

set(DENOISER_CFG_FILE_48K_TURING $<TARGET_FILE_DIR:effects_demo>/denoise48k_cfg_turing.txt)
set(DENOISER_MODEL_48K_TURING models/turing/denoiser_48k.trtpkg)

# Generate the 48k CFG file for denoiser turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo model ${DENOISER_MODEL_48K_TURING}>> ${DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo input_wav denoiser_input_48k.wav>> ${DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo output_wav denoiser_output_48k_turing.wav>> ${DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_48K_TURING}
)

set(DENOISER_CFG_FILE_16K_TURING $<TARGET_FILE_DIR:effects_demo>/denoise16k_cfg_turing.txt)
set(DENOISER_MODEL_16K_TURING models/turing/denoiser_16k.trtpkg)

# Generate the 16k CFG file for denoiser turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo model ${DENOISER_MODEL_16K_TURING}>> ${DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo input_wav denoiser_input_16k.wav>> ${DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo output_wav denoiser_output_16k_turing.wav>> ${DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_16K_TURING}
)

set(DENOISER_CFG_FILE_48K_AMPERE $<TARGET_FILE_DIR:effects_demo>/denoise48k_cfg_ampere.txt)
set(DENOISER_MODEL_48K_AMPERE models/ampere/denoiser_48k.trtpkg)

# Generate the 48k CFG file for denoiser ampere
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo model ${DENOISER_MODEL_48K_AMPERE}>> ${DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo input_wav denoiser_input_48k.wav>> ${DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo output_wav denoiser_output_48k_ampere.wav>> ${DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_48K_AMPERE}
)

set(DENOISER_CFG_FILE_16K_AMPERE $<TARGET_FILE_DIR:effects_demo>/denoise16k_cfg_ampere.txt)
set(DENOISER_MODEL_16K_AMPERE models/ampere/denoiser_16k.trtpkg)

# Generate the 16k CFG file for denoiser ampere
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo model ${DENOISER_MODEL_16K_AMPERE}>> ${DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo input_wav denoiser_input_16k.wav>> ${DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo output_wav denoiser_output_16k_ampere.wav>> ${DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_16K_AMPERE}
)

set(DEREVERB_DENOISER_CFG_FILE_48K_TURING $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise48k_cfg_turing.txt)
set(DEREVERB_DENOISER_MODEL_48K_TURING models/turing/dereverb_denoiser_48k.trtpkg)

# Generate the 48k CFG file for dereverb_denoiser turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_48K_TURING}>> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo input_wav dereverb_denoiser_input_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo output_wav dereverb_denoiser_output_48k_turing.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_48K_TURING}
)

set(DEREVERB_DENOISER_CFG_FILE_16K_TURING $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise16k_cfg_turing.txt)
set(DEREVERB_DENOISER_MODEL_16K_TURING models/turing/dereverb_denoiser_16k.trtpkg)

# Generate the 16k CFG file for dereverb_denoiser turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_16K_TURING}>> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo input_wav dereverb_denoiser_input_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo output_wav dereverb_denoiser_output_16k_turing.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_16K_TURING}
)

set(DEREVERB_DENOISER_CFG_FILE_48K_AMPERE $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise48k_cfg_ampere.txt)
set(DEREVERB_DENOISER_MODEL_48K_AMPERE models/ampere/dereverb_denoiser_48k.trtpkg)

# Generate the 48k CFG file for dereverb_denoiser ampere
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_48K_AMPERE}>> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo input_wav dereverb_denoiser_input_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo output_wav dereverb_denoiser_output_48k_ampere.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_48K_AMPERE}
)

set(DEREVERB_DENOISER_CFG_FILE_16K_AMPERE $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise16k_cfg_ampere.txt)
set(DEREVERB_DENOISER_MODEL_16K_AMPERE models/ampere/dereverb_denoiser_16k.trtpkg)

# Generate the 16k CFG file for dereverb_denoiser ampere
add_custom_command(TARGET effects_demo POST_BUILD
	COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_16K_AMPERE}>> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo input_wav dereverb_denoiser_input_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo output_wav dereverb_denoiser_output_16k_ampere.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_16K_AMPERE}
)

set(DEREVERB_CFG_FILE_48K_TURING $<TARGET_FILE_DIR:effects_demo>/dereverb48k_cfg_turing.txt)
set(DEREVERB_MODEL_48K_TURING models/turing/dereverb_48k.trtpkg)

# Generate the 48k CFG file for dereverb turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_48K_TURING}
    COMMAND echo model ${DEREVERB_MODEL_48K_TURING}>> ${DEREVERB_CFG_FILE_48K_TURING}
    COMMAND echo input_wav dereverb_input_48k.wav>> ${DEREVERB_CFG_FILE_48K_TURING}
    COMMAND echo output_wav dereverb_output_48k_turing.wav>> ${DEREVERB_CFG_FILE_48K_TURING}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_48K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_48K_TURING}
)

set(DEREVERB_CFG_FILE_16K_TURING $<TARGET_FILE_DIR:effects_demo>/dereverb16k_cfg_turing.txt)
set(DEREVERB_MODEL_16K_TURING models/turing/dereverb_16k.trtpkg)

# Generate the 16k CFG file for dereverb turing
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_16K_TURING}
    COMMAND echo model ${DEREVERB_MODEL_16K_TURING}>> ${DEREVERB_CFG_FILE_16K_TURING}
    COMMAND echo input_wav dereverb_input_16k.wav>> ${DEREVERB_CFG_FILE_16K_TURING}
    COMMAND echo output_wav dereverb_output_16k_turing.wav>> ${DEREVERB_CFG_FILE_16K_TURING}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_16K_TURING}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_16K_TURING}
)

set(DEREVERB_CFG_FILE_48K_AMPERE $<TARGET_FILE_DIR:effects_demo>/dereverb48k_cfg_ampere.txt)
set(DEREVERB_MODEL_48K_AMPERE models/ampere/dereverb_48k.trtpkg)

# Generate the 48k CFG file for dereverb ampere
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_48K_AMPERE}
    COMMAND echo model ${DEREVERB_MODEL_48K_AMPERE}>> ${DEREVERB_CFG_FILE_48K_AMPERE}
    COMMAND echo input_wav dereverb_input_48k.wav>> ${DEREVERB_CFG_FILE_48K_AMPERE}
    COMMAND echo output_wav dereverb_output_48k_ampere.wav>> ${DEREVERB_CFG_FILE_48K_AMPERE}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_48K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_48K_AMPERE}
)

set(DEREVERB_CFG_FILE_16K_AMPERE $<TARGET_FILE_DIR:effects_demo>/dereverb16k_cfg_ampere.txt)
set(DEREVERB_MODEL_16K_AMPERE models/ampere/dereverb_16k.trtpkg)

# Generate the 16k CFG file for dereverb ampere
add_custom_command(TARGET effects_demo POST_BUILD
	COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_16K_AMPERE}
    COMMAND echo model ${DEREVERB_MODEL_16K_AMPERE}>> ${DEREVERB_CFG_FILE_16K_AMPERE}
    COMMAND echo input_wav dereverb_input_16k.wav>> ${DEREVERB_CFG_FILE_16K_AMPERE}
    COMMAND echo output_wav dereverb_output_16k_ampere.wav>> ${DEREVERB_CFG_FILE_16K_AMPERE}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_16K_AMPERE}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_16K_AMPERE}
)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/cuda/bin" CUDA_PATH_STR)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/nvtrt/bin" NVTRT_PATH_STR)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/external/openssl/bin" OPENSSL_PATH_STR)
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin" AFXSDK_PATH_STR)
set(PATH_STR "PATH=%PATH%" ${CUDA_PATH_STR} ${NVTRT_PATH_STR} ${OPENSSL_PATH_STR} ${AFXSDK_PATH_STR})
set(CMD_ARG_STR "-c denoise48k_cfg_turing.txt")
set_target_properties(effects_demo PROPERTIES 
	FOLDER SampleApps 
	VS_DEBUGGER_ENVIRONMENT "${PATH_STR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS ${CMD_ARG_STR}
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:effects_demo>
)

# For denoiser turing
set(RUN_DENOISER_16K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_denoiser_16k_turing.bat)
set(RUN_DENOISER_48K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_denoiser_48k_turing.bat)

# Generate the run_denoiser_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c denoise16k_cfg_turing.txt > ${RUN_DENOISER_16K_BAT_FILE_TURING}
)

# Generate the run_denoiser_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c denoise48k_cfg_turing.txt > ${RUN_DENOISER_48K_BAT_FILE_TURING}
)

# For denoiser ampere
set(RUN_DENOISER_16K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_denoiser_16k_ampere.bat)
set(RUN_DENOISER_48K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_denoiser_48k_ampere.bat)

# Generate the run_denoiser_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c denoise16k_cfg_ampere.txt > ${RUN_DENOISER_16K_BAT_FILE_AMPERE}
)

# Generate the run_denoiser_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c denoise48k_cfg_ampere.txt > ${RUN_DENOISER_48K_BAT_FILE_AMPERE}
)

# For dereverb denoiser turing
set(RUN_DEREVERB_DENOISER_16K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_dereverb_denoiser_16k_turing.bat)
set(RUN_DEREVERB_DENOISER_48K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_dereverb_denoiser_48k_turing.bat)

# Generate the run_dereverb_denoiser_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb_denoise16k_cfg_turing.txt > ${RUN_DEREVERB_DENOISER_16K_BAT_FILE_TURING}
)

# Generate the run_dereverb_denoiser_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb_denoise48k_cfg_turing.txt > ${RUN_DEREVERB_DENOISER_48K_BAT_FILE_TURING}
)

# For dereverb denoiser ampere
set(RUN_DEREVERB_DENOISER_16K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_dereverb_denoiser_16k_ampere.bat)
set(RUN_DEREVERB_DENOISER_48K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_dereverb_denoiser_48k_ampere.bat)

# Generate the run_dereverb_denoiser_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb_denoise16k_cfg_ampere.txt > ${RUN_DEREVERB_DENOISER_16K_BAT_FILE_AMPERE}
)

# Generate the run_dereverb_denoiser_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb_denoise48k_cfg_ampere.txt > ${RUN_DEREVERB_DENOISER_48K_BAT_FILE_AMPERE}
)

# For dereverb turing
set(RUN_DEREVERB_16K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_dereverb_16k_turing.bat)
set(RUN_DEREVERB_48K_BAT_FILE_TURING $<TARGET_FILE_DIR:effects_demo>/run_dereverb_48k_turing.bat)

# Generate the run_dereverb_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb16k_cfg_turing.txt > ${RUN_DEREVERB_16K_BAT_FILE_TURING}
)

# Generate the run_dereverb_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb48k_cfg_turing.txt > ${RUN_DEREVERB_48K_BAT_FILE_TURING}
)

# For dereverb ampere
set(RUN_DEREVERB_16K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_dereverb_16k_ampere.bat)
set(RUN_DEREVERB_48K_BAT_FILE_AMPERE $<TARGET_FILE_DIR:effects_demo>/run_dereverb_48k_ampere.bat)

# Generate the run_dereverb_16k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb16k_cfg_ampere.txt > ${RUN_DEREVERB_16K_BAT_FILE_AMPERE}
)

# Generate the run_dereverb_48k.bat
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effects_demo -c dereverb48k_cfg_ampere.txt > ${RUN_DEREVERB_48K_BAT_FILE_AMPERE}
)