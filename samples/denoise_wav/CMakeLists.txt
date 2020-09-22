set(SOURCE_FILES denoise_wav.cpp)

# Set Visual Studio source filters
source_group("Source Files" FILES ${SOURCE_FILES})

add_executable(denoise_wav ${SOURCE_FILES})
target_include_directories(denoise_wav PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(denoise_wav PUBLIC ${SDK_INCLUDES_PATH})
target_include_directories(denoise_wav PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(denoise_wav PUBLIC
	NVAudioEffects
)

target_link_libraries(denoise_wav PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils/NVAudioEffectsAppUtils.lib
)

add_custom_command(TARGET denoise_wav POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/noisy_16k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/noisy_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/run_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_48k.bat
	$<TARGET_FILE_DIR:denoise_wav>
)

set(CFG_FILE_16K $<TARGET_FILE_DIR:denoise_wav>/denoise16k_cfg.txt)
set(MODEL_16K %NVAFX_SDK_DIR%/models/denoiser_16k.trtpkg)

# Generate the 16k CFG file for turing
add_custom_command(TARGET denoise_wav POST_BUILD
    COMMAND echo sample_rate 16000 > ${CFG_FILE_16K}
    COMMAND echo filter_model ${MODEL_16K}  >> ${CFG_FILE_16K}
    COMMAND echo input_wav noisy_16k.wav  >> ${CFG_FILE_16K}
    COMMAND echo output_wav out_16k.wav  >> ${CFG_FILE_16K}
    COMMAND echo real_time 0  >> ${CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0 >> ${CFG_FILE_16K}
)

set(CFG_FILE_48K $<TARGET_FILE_DIR:denoise_wav>/denoise48k_cfg.txt)
set(MODEL_48K %NVAFX_SDK_DIR%/models/denoiser_48k.trtpkg)

# Generate the 48k CFG file for turing
add_custom_command(TARGET denoise_wav POST_BUILD
    COMMAND echo sample_rate 48000 > ${CFG_FILE_48K}
    COMMAND echo filter_model ${MODEL_48K}  >> ${CFG_FILE_48K}
    COMMAND echo input_wav noisy_48k.wav  >> ${CFG_FILE_48K}
    COMMAND echo output_wav out_48k.wav  >> ${CFG_FILE_48K}
    COMMAND echo real_time 0  >> ${CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0 >> ${CFG_FILE_48K}
)

set(PATH_STR "PATH=%NVAFX_SDK_DIR%;%PATH%")
set(CMD_ARG_STR "-c denoise48k_cfg.txt")
set_target_properties(denoise_wav PROPERTIES 
	FOLDER SampleApps
	VS_DEBUGGER_ENVIRONMENT "${PATH_STR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS ${CMD_ARG_STR}
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:denoise_wav>
)