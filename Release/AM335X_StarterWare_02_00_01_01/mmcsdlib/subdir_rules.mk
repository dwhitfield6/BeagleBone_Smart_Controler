################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
AM335X_StarterWare_02_00_01_01/mmcsdlib/hs_mmcsdlib.obj: ../AM335X_StarterWare_02_00_01_01/mmcsdlib/hs_mmcsdlib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --opt_for_speed=0 --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="AM335X_StarterWare_02_00_01_01/mmcsdlib/hs_mmcsdlib.pp" --obj_directory="AM335X_StarterWare_02_00_01_01/mmcsdlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

AM335X_StarterWare_02_00_01_01/mmcsdlib/mmcsd_proto.obj: ../AM335X_StarterWare_02_00_01_01/mmcsdlib/mmcsd_proto.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --opt_for_speed=0 --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" -g --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="AM335X_StarterWare_02_00_01_01/mmcsdlib/mmcsd_proto.pp" --obj_directory="AM335X_StarterWare_02_00_01_01/mmcsdlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

