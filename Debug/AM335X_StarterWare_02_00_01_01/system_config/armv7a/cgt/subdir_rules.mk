################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cp15.obj: ../AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cp15.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/third_party/fatfs/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/mmcsdlib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/device" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/host" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" -g --gcc --define=am335x --define=am335x_15x15 --diag_warning=225 --display_error_number --unaligned_access=on --enum_type=packed --opt_for_cache --preproc_with_compile --preproc_dependency="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cp15.d" --obj_directory="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cpu.obj: ../AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cpu.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/third_party/fatfs/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/mmcsdlib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/device" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/host" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" -g --gcc --define=am335x --define=am335x_15x15 --diag_warning=225 --display_error_number --unaligned_access=on --enum_type=packed --opt_for_cache --preproc_with_compile --preproc_dependency="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/cpu.d" --obj_directory="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/init.obj: ../AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/init.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/third_party/fatfs/src" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/mmcsdlib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/WAV_Audio" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/Bitmaps" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/FT81x" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/device" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/host" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/usblib/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" -g --gcc --define=am335x --define=am335x_15x15 --diag_warning=225 --display_error_number --unaligned_access=on --enum_type=packed --opt_for_cache --preproc_with_compile --preproc_dependency="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt/init.d" --obj_directory="AM335X_StarterWare_02_00_01_01/system_config/armv7a/cgt" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


