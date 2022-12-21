SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
	set base_model_path=%NVAFX_SDK_DIR%\models
) else (
	goto single_effect_exit
)
set argCount=0
for %%x in (%*) do (
   set /A argCount+=1
)

if not %argCount% LEQ 6 (
	echo Invalid Arguments passed.
	goto chain_effect_exit
)

@rem Check for empty arguments.
if [%1]==[] goto single_effect_exit
if [%2]==[] goto single_effect_exit
if [%3]==[] goto single_effect_exit

@rem Set variables
set effect=%1
set input_sample_rate=%2
set output_sample_rate=%3
set intensity_ratio=1.0

set /A real_time=0
set /A enable_vad=0

@rem Supported effects with sample rates
if %effect%==denoiser           goto validate_sample_rates
if %effect%==dereverb           goto validate_sample_rates
if %effect%==dereverb_denoiser  goto validate_sample_rates
if %effect%==aec                goto validate_sample_rates
if %effect%==superres           goto valid_superres_sample_rates
goto invalid_effect

:validate_16k_sample_rates
@rem Validate Input and Output Sample Rates
if %input_sample_rate%==16k if %output_sample_rate%==16k (
	goto valid_single_effect
) else (
	goto invalid_effect
)
if %input_sample_rate%==48k if %output_sample_rate%==48k (
	goto invalid_effect
)

:validate_sample_rates
@rem Validate Input and Output Sample Rates
if %input_sample_rate%==16k if %output_sample_rate%==16k (
	goto valid_single_effect
) else (
	goto invalid_effect
)
if %input_sample_rate%==48k if %output_sample_rate%==48k (
	goto valid_single_effect
) else (
	goto invalid_effect
)

:valid_superres_sample_rates
if %input_sample_rate%==8k (
	if %output_sample_rate%==16k (
		goto valid_single_effect
	)
	if %output_sample_rate%==48k (
		goto valid_single_effect
	)
	goto invalid_effect
)
if %input_sample_rate%==16k if %output_sample_rate%==48k (
	goto valid_single_effect
) else (
	goto invalid_effect
)

:invalid_effect
if [%4]==[] (goto single_effect_not_supported) else (goto chain_effect_not_supported)

:valid_single_effect
@rem Generate model file name
if %effect%==superres (
	set model_file=%base_model_path%\%effect%_%input_sample_rate%to%output_sample_rate%.trtpkg
) else (
	set model_file=%base_model_path%\%effect%_%input_sample_rate%.trtpkg
)

@rem If 2nd effect is passed as 4th argument then chaining block will execute
if not [%4]==[] goto chaining

@rem Generate input folder
set input_folder=input_files\%effect%\%input_sample_rate%

@rem Generate output folder
if %effect%==superres (
	set output_folder=output_files\%effect%\%input_sample_rate%to%output_sample_rate%
) else (
	set output_folder=output_files\%effect%\%output_sample_rate%
)

@rem Create output folder if doesnt exists
if not exist %output_folder% mkdir %output_folder%

@rem Print all config file parameters
echo ===============================================================
echo Following Values are passed/calculated
echo.
echo Effect: %effect%
echo Input Sample Rate: %input_sample_rate%
echo Output Sample Rate: %output_sample_rate%
echo Model File: %model_file%
echo Intensity Ratio: %intensity_ratio%
echo Real Time: %real_time%
echo Enable VAD: %enable_vad%
echo.
echo Input Folder: %input_folder%
echo Output Folder: %output_folder%
echo.
echo ===============================================================

@rem If effect is AEC then populate nearend and farend files
if %effect%==aec (
	@rem Convert the lists into arrays
	set i=0
	for %%D in (%input_folder%\farend\*.wav) do (
		set /A i+=1
		set "D[!i!]=%%D"
	)
	set i=0
	for %%C in (%input_folder%\nearend\*.wav) do (
		set /A i+=1
		set "C[!i!]=%%C"
	)

	for /l %%a in (1 , 1, !i!+1) do (
		echo Element %%a in Farend array is: !D[%%a]!
		echo Element %%a in Nearend array is: !C[%%a]!
		echo .
	)

	for /l %%a in (1,1,!i!+1) do (
		for /F "delims=" %%i in ("!D[%%a]!") do (
			echo effect %effect%
			echo model %model_file%
			echo input_wav !C[%%a]!
			echo input_farend_wav !D[%%a]!
			echo output_wav %output_folder%\%%~ni_OUT.wav
			echo real_time %real_time%
			echo intensity_ratio %intensity_ratio%
			echo enable_vad %enable_vad%
			) > _tmp_config.txt
			
			effects_demo.exe -c _tmp_config.txt
	)
			
	::del _tmp_config.txt
	goto done
) else (
	for %%v in (%input_folder%\*.wav) do (
		for /F "delims=" %%i in ("%%v") do (
			echo effect %effect%
			echo model %model_file%
			echo input_wav %%v
			echo output_wav %output_folder%\%%~ni_OUT.wav
			echo real_time %real_time%
			echo intensity_ratio %intensity_ratio%
			echo enable_vad %enable_vad%
			) > _tmp_config.txt
					
			effects_demo.exe -c _tmp_config.txt
	)
			
	::del _tmp_config.txt
	goto done
)

@rem Chaining block
:chaining

@rem Check for empty arguments.
if [%4]==[] goto chain_effect_exit
if [%5]==[] goto chain_effect_exit
if [%6]==[] goto chain_effect_exit

@rem Set variables
set effect_2=%4
set input_sample_rate_2=%5
set output_sample_rate_2=%6
set intensity_ratio_2=1.0

@rem Supported chains
@rem Generate input and output folders

set input_folder=input_files\chaining\%effect%_%input_sample_rate%_%effect_2%_%output_sample_rate_2%

if %effect_2%==denoiser if %input_sample_rate_2%==48k if %output_sample_rate_2%==48k (
	if %input_sample_rate%==48k if %output_sample_rate%==48k (
		goto chain_effect_not_supported
	)
)
if %effect_2%==denoiser if %input_sample_rate_2%==16k if %output_sample_rate_2%==16k (
	if %input_sample_rate%==16k if %output_sample_rate%==16k (
		goto chain_effect_not_supported
	)
)
if %effect_2%==superres if %input_sample_rate_2%==16k if %output_sample_rate_2%==48k (
	if %input_sample_rate%==16k if %output_sample_rate%==16k (
		if %effect%==denoiser goto valid_chain_effect
		if %effect%==dereverb  goto valid_chain_effect
		if %effect%==dereverb_denoiser  goto valid_chain_effect
		goto chain_effect_not_supported
	)
)

if %effect%==superres if %input_sample_rate%==8k if %output_sample_rate%==16k (
	if %input_sample_rate_2%==16k if %output_sample_rate_2%==16k (
		if %effect_2%==denoiser goto valid_chain_effect
		if %effect_2%==dereverb  goto valid_chain_effect
		if %effect_2%==dereverb_denoiser  goto valid_chain_effect
		goto chain_effect_not_supported
	)
)
goto chain_effect_not_supported

:valid_chain_effect
@rem Generate model file name for 2nd effect
@rem create combined effect name as supported by API

if %effect_2%==superres (
	set model_file_2=%base_model_path%\%effect_2%_%input_sample_rate_2%to%output_sample_rate_2%.trtpkg
	set combined_effect_name=%effect%%input_sample_rate%_%effect_2%%input_sample_rate_2%to%output_sample_rate_2%
) else (
	set model_file_2=%base_model_path%\%effect_2%_%input_sample_rate_2%.trtpkg
	set combined_effect_name=%effect%%input_sample_rate%to%output_sample_rate%_%effect_2%%input_sample_rate_2%
)

@rem Create output folder if doesnt exists
set output_folder=output_files\chaining\%combined_effect_name%
if not exist %output_folder% mkdir %output_folder%

@rem Print all config file parameters
echo ===============================================================
echo Following Values are passed/calculated
echo.
echo Effect_1: %effect%
echo Input Sample Rate for 1st effect: %input_sample_rate%
echo Output Sample Rate for 1st effect: %output_sample_rate%
echo Model File for 1st effect: %model_file%
echo Intensity Ratio for 1st effect: %intensity_ratio%
echo .
echo Effect_2: %effect_2%
echo Input Sample Rate for 2nd effect: %input_sample_rate_2%
echo Output Sample Rate for 2nd effect: %output_sample_rate_2%
echo Model File for 2nd effect: %model_file_2%
echo Intensity Ratio for 2nd effect: %intensity_ratio_2%
echo.
echo Real Time: %real_time%
echo.
echo Input Folder: %input_folder%
echo Output Folder: %output_folder%
echo.
echo ===============================================================

for %%v in (%input_folder%\*.wav) do (
	for /F "delims=" %%i in ("%%v") do (
		echo effect %combined_effect_name%
		echo model %model_file%,%model_file_2%
		echo input_wav %%v
		echo output_wav %output_folder%\%%~ni_OUT.wav
		echo real_time %real_time%
		echo intensity_ratio %intensity_ratio%,%intensity_ratio_2%
		) > _tmp_config.txt
				
		effects_demo.exe -c _tmp_config.txt
)
		
::del _tmp_config.txt
goto done

:single_effect_not_supported
	echo Supported Single Effects are as follows: 
	echo 	- denoiser 16k 16k	
	echo 	- denoiser 48k 48k	
	echo 	- dereverb 16k 16k	
	echo 	- dereverb 48k 48k	
	echo 	- dereverb_denoiser 16k 16k
	echo 	- dereverb_denoiser 48k 48k	
	echo 	- aec 16k 16k
	echo 	- aec 48k 48k	
	echo 	- superres 8k 16k
	echo 	- superres 16k 48k	
	goto single_effect_exit

:chain_effect_not_supported
	echo Supported Effects in Chain are as follows: 
	echo 	- denoiser 16k + superres 16kto48k	
	echo 	- dereverb 16k + superres 16kto48k	
	echo 	- dereverb_denoiser 16k + superres 16kto48k	
	echo 	- superres 8kto16k + denoiser 16k
	echo 	- superres 8kto16k + dereverb 16k	
	echo 	- superres 8kto16k + dereverb_denoiser 16k
	goto chain_effect_exit

:chain_effect_exit
	echo Usage: run_effects_demo.bat effect_1 input_sample_rate_1 output_sample_rate_1  effect_2 input_sample_rate_2 output_sample_rate_2
	echo 	- effect_1: 1st Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/superres
	echo 	- input_sample_rate_1: Input Sample Rate for 1st effect. Supported values are: 8k/16k/48k
	echo 	- output_sample_rate_1: Output Sample Rate for 1st effect. Supported values are: 16k/48k
	
	echo 	- effect_2: 2nd Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/superres
	echo 	- input_sample_rate_2: Input Sample Rate for 2nd effect. Supported values are: 8k/16k/48k
	echo 	- output_sample_rate_2: Output Sample Rate for 2nd effect. Supported values are: 16k/48k
	
	echo Some Sample commands are as follows:
	echo 	- run_effects_demo.bat denoiser 16k 16k superres 16k 48k
	echo 	- run_effects_demo.bat dereverb 16k 16k superres 16k 48k
	echo 	- run_effects_demo.bat dereverb_denoiser 16k 16k superres 16k 48k
	echo 	- run_effects_demo.bat superres 8k 16k denoiser 16k 16k
	echo 	- run_effects_demo.bat superres 8k 16k dereverb 16k 16k
	echo 	- run_effects_demo.bat superres 8k 16k dereverb_denoiser 16k 16k
	goto done
	
:single_effect_exit
	echo Usage: run_effects_demo.bat  effect  input_sample_rate  output_sample_rate
	echo 	- effect: Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/aec/superres
	echo 	- input_sample_rate: Input Sample Rate for the effect. Supported values are: 8k/16k/48k
	echo 	- output_sample_rate: Output Sample Rate for the effect. Supported values are: 16k/48k

	echo Some Sample commands are as follows:
	echo 	- run_effects_demo.bat denoiser 16k 16k
	echo 	- run_effects_demo.bat dereverb 48k 48k
	echo 	- run_effects_demo.bat dereverb_denoiser 16k 16k
	echo 	- run_effects_demo.bat aec 16k 16k
	echo 	- run_effects_demo.bat superres 16k 48k
	goto done
	
:done