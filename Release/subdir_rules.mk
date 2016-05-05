################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ADC.obj: ../ADC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="ADC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

AUDIO.obj: ../AUDIO.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="AUDIO.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

BMP.obj: ../BMP.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="BMP.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMD.obj: ../CMD.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="CMD.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

EMMC.obj: ../EMMC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="EMMC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

FRAM.obj: ../FRAM.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="FRAM.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

GPIO.obj: ../GPIO.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="GPIO.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

GUI.obj: ../GUI.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="GUI.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

I2C.obj: ../I2C.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="I2C.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

INTERRUPTS.obj: ../INTERRUPTS.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="INTERRUPTS.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

IR.obj: ../IR.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="IR.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LCD.obj: ../LCD.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="LCD.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

LEDS.obj: ../LEDS.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="LEDS.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

MEMORY.obj: ../MEMORY.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="MEMORY.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

MISC.obj: ../MISC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="MISC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

PMIC.obj: ../PMIC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="PMIC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

POWER.obj: ../POWER.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="POWER.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

PWM.obj: ../PWM.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="PWM.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

RF.obj: ../RF.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="RF.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

RTCC.obj: ../RTCC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="RTCC.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SD.obj: ../SD.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="SD.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SPI.obj: ../SPI.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="SPI.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SYSTEM.obj: ../SYSTEM.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="SYSTEM.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

TEST.obj: ../TEST.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="TEST.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

TIMERS.obj: ../TIMERS.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="TIMERS.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

UART.obj: ../UART.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="UART.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

USB.obj: ../USB.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="USB.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

WAV.obj: ../WAV.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="WAV.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --fp_mode=relaxed --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/fatfs/ff12/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


