# Introduction
effects_demo.exe is the sample app which demonstrates multiple effects on input samples. effects_demo.exe takes config file as an argument. 

One such sample config file is denoiser48k_cfg.txt which will apply denoiser effect on noisy input file (Air_Conditioning_48k.wav)
with sample rate 48k and dump denoised output as Air_Conditioning_48k_OUT.wav.

run_denoiser_48k.bat file add required dlls to environmental path variables for execution which are 
internally used by effects_demo.exe. 
Hence, we need to run only this bat file instead of effects_demo.exe directly.

# Helper Script
run_effects_demo.bat is a windows batch file which will auto-generate config files on the go based on arguments passed to it.
The auto-generated config file will be used by effects_demo.exe to apply the corresponding effect on input files.
Sample Input files are placed in the input_files folder. 

Users can place their own input files in appropriate folder as per choice of effect and sample rate and helper script will apply the effect on those inputs
and dump the corresponding outputs to corresponding folder.

This is a single generalized batch file which can be used to demonstrate all effects on all input files.

## Usage 

### Single Effect
Usage: run_effects_demo.bat <effect> <input_sample_rate> <output_sample_rate>
- effect: Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/aec/superres
- input_sample_rate: Input Sample Rate for the effect. Supported values are: 8k/16k/48k
- output_sample_rate: Output Sample Rate for the effect. Supported values are: 16k/48k

### Chaining Effects
Usage: run_effects_demo.bat <effect_1> <input_sample_rate_1> <output_sample_rate_1> <effect_2> <input_sample_rate_2> <output_sample_rate_2>	
- effect_1: 1st Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/superres
- input_sample_rate_1: Input Sample Rate for 1st effect. Supported values are: 8k/16k/48k
- output_sample_rate_1: Output Sample Rate for 1st effect. Supported values are: 16k/48k
- effect_2: 2nd Effect to be applied. Supported values are: denoiser/dereverb/dereverb_denoiser/superres
- input_sample_rate_2: Input Sample Rate for 2nd effect. Supported values are: 8k/16k/48k
- output_sample_rate_2: Output Sample Rate for 2nd effect. Supported values are: 16k/48k

## Supported Effects

### Single Effect
Supported Single Effects are as follows: 
- denoiser 16k 16k	
- denoiser 48k 48k	
- dereverb 16k 16k	
- dereverb 48k 48k	
- dereverb_denoiser 16k 16k
- dereverb_denoiser 48k 48k	
- aec 16k 16k
- aec 48k 48k	
- superres 8k 16k
- superres 16k 48k	
- superres 8k 48k	

### Chaining Effect
Supported Effects in Chain are as follows: 
- denoiser 16k + superres 16kto48k	
- dereverb 16k + superres 16kto48k	
- dereverb_denoiser 16k + superres 16kto48k	
- superres 8kto16k + denoiser 16k
- superres 8kto16k + dereverb 16k
- superres 8kto16k + dereverb_denoiser 16k

## Supported Sample Commands

### Single Effect
Some Sample commands are as follows: 
- run_effects_demo.bat denoiser 16k 16k
- run_effects_demo.bat denoiser 48k 48k
- run_effects_demo.bat dereverb 16k 16k
- run_effects_demo.bat dereverb 48k 48k
- run_effects_demo.bat dereverb_denoiser 16k 16k
- run_effects_demo.bat dereverb_denoiser 48k 48k
- run_effects_demo.bat aec 16k 16k
- run_effects_demo.bat aec 48k 48k
- run_effects_demo.bat superres 16k 48k
- run_effects_demo.bat superres 8k 48k
- run_effects_demo.bat superres 8k 16k

### Chaining Effect
Some Sample commands are as follows:
- run_effects_demo.bat denoiser 16k 16k superres 16k 48k
- run_effects_demo.bat dereverb 16k 16k superres 16k 48k
- run_effects_demo.bat dereverb_denoiser 16k 16k superres 16k 48k
- run_effects_demo.bat superres 8k 16k denoiser 16k 16k
- run_effects_demo.bat superres 8k 16k dereverb 16k 16k
- run_effects_demo.bat superres 8k 16k dereverb_denoiser 16k 16k
