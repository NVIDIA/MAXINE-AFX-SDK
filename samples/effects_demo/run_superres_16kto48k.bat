SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" superres16kto48k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\superres_16kto48k.trtpkg >> superres16kto48k_cfg.txt
))
endlocal
effects_demo  -c superres16kto48k_cfg.txt