SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" denoise16k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\denoiser_16k.trtpkg >> denoise16k_cfg.txt
))
endlocal
effects_demo  -c denoise16k_cfg.txt