SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" dereverb_denoise16k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\dereverb_denoiser_16k.trtpkg >> dereverb_denoise16k_cfg.txt
))
endlocal
effects_demo  -c dereverb_denoise16k_cfg.txt