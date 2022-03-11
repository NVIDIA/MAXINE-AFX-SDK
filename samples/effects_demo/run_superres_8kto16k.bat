SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
setlocal enabledelayedexpansion
if defined NVAFX_SDK_DIR (
findstr /m "model" superres8kto16k_cfg.txt
if "!ERRORLEVEL!"=="1" (
@echo model %NVAFX_SDK_DIR%\models\superres_8kto16k.trtpkg >> superres8kto16k_cfg.txt
))
endlocal
effects_demo  -c superres8kto16k_cfg.txt