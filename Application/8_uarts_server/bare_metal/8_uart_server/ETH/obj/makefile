################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
8_uart_server.hex \

SECONDARY_LIST += \
8_uart_server.lst \

SECONDARY_SIZE += \
8_uart_server.siz \


# All Target
all: 8_uart_server.elf secondary-outputs

# Tool invocations
8_uart_server.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -w -T "F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"F:\Desktop\ch32v307\Application\8_uarts_server\bare_metal\8_uart_server\ETH\User" -Wl,-Map,"8_uart_server.map" --specs=nano.specs --specs=nosys.specs -o "8_uart_server.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
8_uart_server.hex: 8_uart_server.elf
	@	riscv-none-embed-objcopy -O ihex "8_uart_server.elf"  "8_uart_server.hex"
	@	@
8_uart_server.lst: 8_uart_server.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide -M xw "8_uart_server.elf" > "8_uart_server.lst"
	@	@
8_uart_server.siz: 8_uart_server.elf
	@	riscv-none-embed-size --format=berkeley "8_uart_server.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) 8_uart_server.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
