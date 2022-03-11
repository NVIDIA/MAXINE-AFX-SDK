SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" denoiser48k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\denoiser_48k.trtpkg >> denoiser48k_cfg.txt
))
endlocal
effects_demo  -c denoiser48k_cfg.txt