SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" dereverb_denoise48k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\dereverb_denoiser_48k.trtpkg >> dereverb_denoise48k_cfg.txt
))
endlocal
effects_demo  -c dereverb_denoise48k_cfg.txt