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
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_8k_chain.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_8k_chain.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_8k_chain.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_16k_chain.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_16k_chain.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_16k_chain.wav	
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_16k_vad.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_48k_vad.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_16k_vad.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_48k_vad.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_16k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/dereverb_denoiser_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_nearend_input_16k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_farend_input_16k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_nearend_input_48k.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/aec_farend_input_48k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_nearend_input_16k_1.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_farend_input_16k_1.wav
	${CMAKE_CURRENT_SOURCE_DIR}/aec_nearend_input_48k_1.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/aec_farend_input_48k_1.wav
	${CMAKE_CURRENT_SOURCE_DIR}/superres_input_8k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/superres_input_16k.wav
	${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_denoiser_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_denoiser_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_aec_16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_aec_48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_8kto16k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_16kto48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_8kto48k.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_16k_superres_16kto48k_chain.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_16k_superres_16kto48k_chain.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_dereverb_denoiser_16k_superres_16kto48k_chain.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_8kto16k_denoiser_16k_chain.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_8kto16k_dereverb_16k_chain.bat
	${CMAKE_CURRENT_SOURCE_DIR}/run_superres_8kto16k_dereverb_denoiser_16k_chain.bat
	$<TARGET_FILE_DIR:effects_demo>
)

set(DENOISER_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/denoiser16k_cfg.txt)
set(DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/denoiser_16k.trtpkg)

# Generate the Denoiser 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_16K}
    COMMAND echo model ${DENOISER_MODEL_16K}>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo input_wav denoiser_input_16k.wav>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo output_wav denoiser_output_16k.wav>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_16K}
    COMMAND echo enable_vad 0 >> ${DENOISER_CFG_FILE_16K}
)

set(DENOISER_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/denoiser48k_cfg.txt)
set(DENOISER_MODEL_48K %NVAFX_SDK_DIR%/models/denoiser_48k.trtpkg)

# Generate the Denoiser 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser> ${DENOISER_CFG_FILE_48K}
    COMMAND echo model ${DENOISER_MODEL_48K}>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo input_wav denoiser_input_48k.wav>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo output_wav denoiser_output_48k.wav>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DENOISER_CFG_FILE_48K}
    COMMAND echo enable_vad 0 >> ${DENOISER_CFG_FILE_48K}
)

set(DEREVERB_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/dereverb16k_cfg.txt)
set(DEREVERB_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_16k.trtpkg)

# Generate the Dereverb 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_16K}
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
    COMMAND echo effect dereverb> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo model ${DEREVERB_MODEL_48K}>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo input_wav dereverb_input_48k.wav>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo output_wav dereverb_output_48k.wav>> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_CFG_FILE_48K}
)

set(DEREVERB_DENOISER_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/dereverb_denoiser16k_cfg.txt)
set(DEREVERB_DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_denoiser_16k.trtpkg)

# Generate the Dereverb_Denoiser 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_16K}>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo input_wav dereverb_denoiser_input_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo output_wav dereverb_denoiser_output_16k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_16K}
	COMMAND echo enable_vad 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K}
)

set(DEREVERB_DENOISER_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/dereverb_denoiser48k_cfg.txt)
set(DEREVERB_DENOISER_MODEL_48K %NVAFX_SDK_DIR%/models/dereverb_denoiser_48k.trtpkg)

# Generate the Dereverb_Denoiser 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_48K}>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo input_wav dereverb_denoiser_input_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo output_wav dereverb_denoiser_output_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${DEREVERB_DENOISER_CFG_FILE_48K}
    COMMAND echo enable_vad 0 >> ${DEREVERB_DENOISER_CFG_FILE_48K}
)




set(AEC_CFG_FILE_16K $<TARGET_FILE_DIR:effects_demo>/aec16k_cfg.txt)
set(AEC_MODEL_16K %NVAFX_SDK_DIR%/models/aec_16k.trtpkg)

# Generate AEC 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect aec> ${AEC_CFG_FILE_16K}
    COMMAND echo model ${AEC_MODEL_16K}>> ${AEC_CFG_FILE_16K}
    COMMAND echo input_wav aec_nearend_input_16k.wav>> ${AEC_CFG_FILE_16K}
	COMMAND echo input_farend_wav aec_farend_input_16k.wav>> ${AEC_CFG_FILE_16K}
    COMMAND echo output_wav aec_output_16k.wav>> ${AEC_CFG_FILE_16K}
    COMMAND echo real_time 0 >> ${AEC_CFG_FILE_16K}
    COMMAND echo intensity_ratio 1.0>> ${AEC_CFG_FILE_16K}
)

set(AEC_CFG_FILE_48K $<TARGET_FILE_DIR:effects_demo>/aec48k_cfg.txt)
set(AEC_MODEL_48K %NVAFX_SDK_DIR%/models/aec_48k.trtpkg)

# Generate the AEC 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect aec> ${AEC_CFG_FILE_48K}
    COMMAND echo model ${AEC_MODEL_48K}>> ${AEC_CFG_FILE_48K}
    COMMAND echo input_wav aec_nearend_input_48k.wav>> ${AEC_CFG_FILE_48K}
	COMMAND echo input_farend_wav aec_farend_input_48k.wav>> ${AEC_CFG_FILE_48K}
    COMMAND echo output_wav aec_output_48k.wav>> ${AEC_CFG_FILE_48K}
    COMMAND echo real_time 0 >> ${AEC_CFG_FILE_48K}
    COMMAND echo intensity_ratio 1.0>> ${AEC_CFG_FILE_48K}
)



set(SUPERRES_CFG_FILE_8KTO16K $<TARGET_FILE_DIR:effects_demo>/superres8kto16k_cfg.txt)
set(SUPERRES_MODEL_8KTO16K %NVAFX_SDK_DIR%/models/superres_8kto16k.trtpkg)

# Generate the Superres 16k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres> ${SUPERRES_CFG_FILE_8KTO16K}
    COMMAND echo model ${SUPERRES_MODEL_8KTO16K}>> ${SUPERRES_CFG_FILE_8KTO16K}
    COMMAND echo input_wav superres_input_8k.wav>> ${SUPERRES_CFG_FILE_8KTO16K}
    COMMAND echo output_wav superres_output_16k.wav>> ${SUPERRES_CFG_FILE_8KTO16K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_8KTO16K}
    COMMAND echo intensity_ratio 1.0>> ${SUPERRES_CFG_FILE_8KTO16K}
)

set(SUPERRES_CFG_FILE_16KTO48K $<TARGET_FILE_DIR:effects_demo>/superres16kto48k_cfg.txt)
set(SUPERRES_MODEL_16KTO48K %NVAFX_SDK_DIR%/models/superres_16kto48k.trtpkg)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres> ${SUPERRES_CFG_FILE_16KTO48K}
    COMMAND echo model ${SUPERRES_MODEL_16KTO48K}>> ${SUPERRES_CFG_FILE_16KTO48K}
    COMMAND echo input_wav superres_input_16k.wav>> ${SUPERRES_CFG_FILE_16KTO48K}
    COMMAND echo output_wav superres_output_48k.wav>> ${SUPERRES_CFG_FILE_16KTO48K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_16KTO48K}
    COMMAND echo intensity_ratio 1.0>> ${SUPERRES_CFG_FILE_16KTO48K}
)

set(SUPERRES_CFG_FILE_8KTO48K $<TARGET_FILE_DIR:effects_demo>/superres8kto48k_cfg.txt)
set(SUPERRES_MODEL_8KTO48K %NVAFX_SDK_DIR%/models/superres_8kto48k.trtpkg)

# Generate the Superres 8k-48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres> ${SUPERRES_CFG_FILE_8KTO48K}
    COMMAND echo model ${SUPERRES_MODEL_8KTO48K}>> ${SUPERRES_CFG_FILE_8KTO48K}
    COMMAND echo input_wav superres_input_8k.wav>> ${SUPERRES_CFG_FILE_8KTO48K}
    COMMAND echo output_wav superres_output_48k.wav>> ${SUPERRES_CFG_FILE_8KTO48K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_8KTO48K}
    COMMAND echo intensity_ratio 1.0>> ${SUPERRES_CFG_FILE_8KTO48K}
)

# chaining
set(SUPERRES_MODEL_8KTO16K %NVAFX_SDK_DIR%/models/superres_8kto16k.trtpkg)
set(SUPERRES_MODEL_16KTO48K %NVAFX_SDK_DIR%/models/superres_16kto48k.trtpkg)
set(DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/denoiser_16k.trtpkg)
set(DEREVERB_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_16k.trtpkg)
set(DEREVERB_DENOISER_MODEL_16K %NVAFX_SDK_DIR%/models/dereverb_denoiser_16k.trtpkg)


set(SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K $<TARGET_FILE_DIR:effects_demo>/superres8kto16k_denoiser16k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres8kto16k_denoiser16k> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
    COMMAND echo model ${SUPERRES_MODEL_8KTO16K},${DENOISER_MODEL_16K}>> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
    COMMAND echo input_wav denoiser_input_8k_chain.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
    COMMAND echo output_wav superres_denoiser_output_16k.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${SUPERRES_CFG_FILE_8KTO16K_DENOISER_16K}
)

set(SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K $<TARGET_FILE_DIR:effects_demo>/superres8kto16k_dereverb16k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres8kto16k_dereverb16k> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
    COMMAND echo model ${SUPERRES_MODEL_8KTO16K},${DEREVERB_MODEL_16K}>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
    COMMAND echo input_wav dereverb_input_8k_chain.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
    COMMAND echo output_wav superres_dereverb_output_16k.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_16K}
)

set(SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K $<TARGET_FILE_DIR:effects_demo>/superres8kto16k_dereverb_denoiser16k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect superres8kto16k_denoiser16k> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
    COMMAND echo model ${SUPERRES_MODEL_8KTO16K},${DEREVERB_DENOISER_MODEL_16K}>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
    COMMAND echo input_wav dereverb_denoiser_input_8k_chain.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
    COMMAND echo output_wav superres_dereverb_denoiser_output_16k.wav>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
    COMMAND echo real_time 0 >> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${SUPERRES_CFG_FILE_8KTO16K_DEREVERB_DENOISER_16K}
)

set(DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K $<TARGET_FILE_DIR:effects_demo>/denoiser16k_superres16kto48k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect denoiser16k_superres16kto48k> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo model ${DENOISER_MODEL_16K},${SUPERRES_MODEL_16KTO48K}>> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo input_wav denoiser_input_16k_chain.wav>> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo output_wav superres_denoiser_output_48k.wav>> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo real_time 0 >> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
)

set(DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K $<TARGET_FILE_DIR:effects_demo>/dereverb16k_superres16kto48k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb16k_superres16kto48k> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo model ${DEREVERB_MODEL_16K},${SUPERRES_MODEL_16KTO48K}>> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo input_wav dereverb_input_16k_chain.wav>> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo output_wav superres_dereverb_output_48k.wav>> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo real_time 0 >> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${DEREVERB_CFG_FILE_16K_SUPERRES_16KTO48K}
)

set(DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K $<TARGET_FILE_DIR:effects_demo>/dereverb_denoiser16k_superres16kto48k_chain_cfg.txt)

# Generate the Superres 48k CFG file
add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND echo effect dereverb_denoiser16k_superres16kto48k> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo model ${DEREVERB_DENOISER_MODEL_16K},${SUPERRES_MODEL_16KTO48K}>> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo input_wav dereverb_denoiser_input_16k_chain.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo output_wav superres_dereverb_denoiser_output_48k.wav>> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo real_time 0 >> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
    COMMAND echo intensity_ratio 1.0,1.0>> ${DEREVERB_DENOISER_CFG_FILE_16K_SUPERRES_16KTO48K}
)




set(PATH_STR "PATH=%NVAFX_SDK_DIR%;%PATH%")
set(CMD_ARG_STR "-c denoiser48k_cfg.txt")
set_target_properties(effects_demo PROPERTIES 
	FOLDER SampleApps
	VS_DEBUGGER_ENVIRONMENT "${PATH_STR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS ${CMD_ARG_STR}
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:effects_demo>
)