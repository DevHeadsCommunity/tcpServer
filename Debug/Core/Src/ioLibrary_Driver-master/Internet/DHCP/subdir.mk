################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.c 

OBJS += \
./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.o 

C_DEPS += \
./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ioLibrary_Driver-master/Internet/DHCP/%.o Core/Src/ioLibrary_Driver-master/Internet/DHCP/%.su Core/Src/ioLibrary_Driver-master/Internet/DHCP/%.cyclo: ../Core/Src/ioLibrary_Driver-master/Internet/DHCP/%.c Core/Src/ioLibrary_Driver-master/Internet/DHCP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I../Core/Src/ioLibrary_Driver-master/Ethernet -I../Core/Src/ioLibrary_Driver-master/Internet -I../Core/Src/ioLibrary_Driver-master/Application -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-DHCP

clean-Core-2f-Src-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-DHCP:
	-$(RM) ./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.cyclo ./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.d ./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.o ./Core/Src/ioLibrary_Driver-master/Internet/DHCP/dhcp.su

.PHONY: clean-Core-2f-Src-2f-ioLibrary_Driver-2d-master-2f-Internet-2f-DHCP

