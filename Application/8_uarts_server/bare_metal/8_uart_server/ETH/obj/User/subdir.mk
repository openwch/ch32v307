################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/bsp_uart.c \
../User/ch32v30x_it.c \
../User/main.c \
../User/system_ch32v30x.c 

OBJS += \
./User/bsp_uart.o \
./User/ch32v30x_it.o \
./User/main.o \
./User/system_ch32v30x.o 

C_DEPS += \
./User/bsp_uart.d \
./User/ch32v30x_it.d \
./User/main.d \
./User/system_ch32v30x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -w -I"F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\SRC\Core" -I"F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\SRC\Debug" -I"F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\SRC\Peripheral\inc" -I"F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\ETH\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

