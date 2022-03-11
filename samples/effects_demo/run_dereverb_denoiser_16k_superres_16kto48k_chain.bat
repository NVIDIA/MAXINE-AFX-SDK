SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" dereverb_denoiser16k_superres16kto48k_chain_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\dereverb_denoiser_16k.trtpkg,%NVAFX_SDK_DIR%\models\superres_16kto48k.trtpkg >> dereverb_denoiser16k_superres16kto48k_chain_cfg.txt
))
endlocal
effects_demo  -c dereverb_denoiser16k_superres16kto48k_chain_cfg.txt