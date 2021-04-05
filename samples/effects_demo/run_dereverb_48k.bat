SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" dereverb48k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\dereverb_48k.trtpkg >> dereverb48k_cfg.txt
))
endlocal
effects_demo  -c dereverb48k_cfg.txt