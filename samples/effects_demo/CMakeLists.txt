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
	${CMAKE_CURRENT_SOURCE_DIR}/run_effects_demo.bat
	${CMAKE_CURRENT_SOURCE_DIR}/readme.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/denoiser48k_cfg.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/run_denoiser_48k.bat
	$<TARGET_FILE_DIR:effects_demo>
)

add_custom_command(TARGET effects_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/input_files $<TARGET_FILE_DIR:effects_demo>/input_files)

set(PATH_STR "PATH=%NVAFX_SDK_DIR%;%PATH%")
set(CMD_ARG_STR "-c denoiser48k_cfg.txt")
set_target_properties(effects_demo PROPERTIES 
	FOLDER SampleApps
	VS_DEBUGGER_ENVIRONMENT "${PATH_STR}"
	VS_DEBUGGER_COMMAND_ARGUMENTS ${CMD_ARG_STR}
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:effects_demo>
)