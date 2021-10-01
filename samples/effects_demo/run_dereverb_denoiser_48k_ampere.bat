SETLOCAL
SET PATH=..\..\bin\external\openssl\bin;..\..\bin;..\..\bin\external\cuda\bin;..\..\bin\external\nvtrt\bin;%PATH%;
effects_demo.exe  -c dereverb_denoise48k_cfg_ampere.txt
