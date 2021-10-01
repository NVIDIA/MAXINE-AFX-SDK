SETLOCAL
SET PATH=..\..\bin\external\openssl\bin;..\..\bin;..\..\bin\external\cuda\bin;..\..\bin\external\nvtrt\bin;%PATH%;
effects_demo.exe  -c denoise16k_cfg_turing.txt
