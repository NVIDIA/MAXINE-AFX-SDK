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

target_link_libraries(effects_demo ${LINK_LIBS})

add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_denoiser_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_denoiser_48k.bat
	$<TARGET_FILE_DIR:effects_demo>
)

set(DENOISER_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/denoise16k_cfg.txt)
set(DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/denoiser_16k.trtpkg)

# Generate the Denoiser 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo sample_rate 16000>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo model ${DENOISER_MODEL_16K}>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo input_wav denoiser_input_16k.wav>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo output_wav denoiser_output_16k.wav>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_16K}
)

set(DENOISER_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/denoise48k_cfg.txt)
set(DENOISER_MODEL_48K %NVAFX_SDK_DIR%/models/denoiser_48k.trtpkg)

# Generate the Denoiser 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo sample_rate 48000>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo model ${DENOISER_MODEL_48K}>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo input_wav denoiser_input_48k.wav>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo output_wav denoiser_output_48k.wav>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_48K}
)

set(DEREVERB_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/dereverb16k_cfg.txt)
set(DEREVERB_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_16k.trtpkg)

# Generate the Dereverb 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb>> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo sample_rate 16000>> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo model ${DEREVERB_MODEL_16K}>> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo input_wav dereverb_input_16k.wav>> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo output_wav dereverb_output_16k.wav>> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_16K}
)

set(DEREVERB_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/dereverb48k_cfg.txt)
set(DEREVERB_MODEL_48K %NVAFX_SDK_DIR%/models/dereverb_48k.trtpkg)

# Generate the Dereverb 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo sample_rate 48000>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo model ${DEREVERB_MODEL_48K}>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo input_wav dereverb_input_48k.wav>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo output_wav dereverb_output_48k.wav>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_48K}
)

set(DEREVERB_DENOISER_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise16k_cfg.txt)
set(DEREVERB_DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_denoiser_16k.trtpkg)

# Generate the Dereverb_Denoiser 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo sample_rate 16000>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_16K}>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo input_wav dereverb_denoiser_input_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo output_wav dereverb_denoiser_output_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_16K}
)

set(DEREVERB_DENOISER_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/dereverb_denoise48k_cfg.txt)
set(DEREVERB_DENOISER_MODEL_48K %NVAFX_SDK_DIR%/models/dereverb_denoiser_48k.trtpkg)

# Generate the Dereverb_Denoiser 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo sample_rate 48000>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_48K}>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo input_wav dereverb_denoiser_input_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo output_wav dereverb_denoiser_output_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_48K}
)

set(PATH_STR "PATH=%NVAFX_SDK_DIR%;%PATH%")
set(CMD_ARG_STR "-c denoise48k_cfg.txt")
set_target_properties(effects_demo PROPERTIES 
	FOLDER SampleApps
	VS_DEBUGGER_ENVIRONMENT "${PATH_STR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS ${CMD_ARG_STR}
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:effects_demo>
)