SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" denoiser16k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\denoiser_16k.trtpkg >> denoiser16k_cfg.txt
))
endlocal
effects_demo  -c denoiser16k_cfg.txt