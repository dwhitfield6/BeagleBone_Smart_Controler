################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Bootloader/AM335X_StarterWare_02_00_01_01/third_party/lwip-1.4.0/test/unit/lwip_unittests.obj: ../Bootloader/AM335X_StarterWare_02_00_01_01/third_party/lwip-1.4.0/test/unit/lwip_unittests.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -O2 -g --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path= --include_path= --include_path= --include_path= --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/hw" --include_path="C:/Users/dwhitfield/Documents/GitHub/BeagleBone_Smart_Controler/AM335X_StarterWare_02_00_01_01/include/armv7a/am335x" --define=_INLINE --define=am335x --diag_warning=225 --preproc_with_compile --preproc_dependency="Bootloader/AM335X_StarterWare_02_00_01_01/third_party/lwip-1.4.0/test/unit/lwip_unittests.pp" --obj_directory="Bootloader/AM335X_StarterWare_02_00_01_01/third_party/lwip-1.4.0/test/unit" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


