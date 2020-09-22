SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
findstr /m "filter_model" denoise48k_cfg.txt
if %errorlevel%==1 (
@echo filter_model %NVAFX_SDK_DIR%\models\denoiser_48k.trtpkg >> denoise48k_cfg.txt
)
denoise_wav  -c denoise48k_cfg.txt