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
DAC_Triangle_Generation.hex \

SECONDARY_LIST += \
DAC_Triangle_Generation.lst \

SECONDARY_SIZE += \
DAC_Triangle_Generation.siz \


# All Target
all: DAC_Triangle_Generation.elf secondary-outputs

# Tool invocations
DAC_Triangle_Generation.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -T "C:\Users\OWNER\Desktop\CH32V30x\2.new\CH32V307EVT\EVT\EXAM\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"DAC_Triangle_Generation.map" --specs=nano.specs --specs=nosys.specs -o "DAC_Triangle_Generation.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
DAC_Triangle_Generation.hex: DAC_Triangle_Generation.elf
	@	riscv-none-embed-objcopy -O ihex "DAC_Triangle_Generation.elf"  "DAC_Triangle_Generation.hex"
	@	@
DAC_Triangle_Generation.lst: DAC_Triangle_Generation.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble "DAC_Triangle_Generation.elf" > "DAC_Triangle_Generation.lst"
	@	@
DAC_Triangle_Generation.siz: DAC_Triangle_Generation.elf
	@	riscv-none-embed-size --format=berkeley "DAC_Triangle_Generation.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) DAC_Triangle_Generation.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
