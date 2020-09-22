SETLOCAL
SET PATH=%NVAFX_SDK_DIR%;%PATH%;
@echo off
findstr /m "filter_model" denoise16k_cfg.txt
if %errorlevel%==1 (
@echo filter_model %NVAFX_SDK_DIR%\models\denoiser_16k.trtpkg >> denoise16k_cfg.txt
)
denoise_wav  -c denoise16k_cfg.txt
