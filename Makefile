TARGET=boot
EXECUTABLE=$(TARGET).elf

CUBE=../STM32_Cube_F7/Drivers
HALS=$(CUBE)/STM32F7xx_HAL_Driver/Src
USBD=../STM32_Cube_F7/Middlewares/ST/STM32_USB_Device_Library
WRLIB=../wrLib
WRDSP=../wrDsp
PRJ_DIR=dfu-stm32f7

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump

# BIN=$(CP) -O ihex 
BIN = $(TARGET).bin

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F7XX -DARM_MATH_CM7 -DHSE_VALUE=8000000
STARTUP = $(CUBE)/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f722xx.s

#MCFLAGS = -march=armv7e-m -mthumb
MCFLAGS = -mthumb -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16

STM32_INCLUDES = \
	-I$(WRLIB)/ \
	-I$(WRDSP)/ \
	-I$(CUBE)/CMSIS/Device/ST/STM32F7xx/Include/ \
	-I$(CUBE)/CMSIS/Include/ \
	-I$(CUBE)/STM32F7xx_HAL_Driver/Inc/ \
	-Iusbd/ \
	-I$(USBD)/Class/DFU/Inc/ \
	-I$(USBD)/Core/Inc/ \

OPTIMIZE       = -Os

CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += $(MCFLAGS)
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEFS) -I. -I./ $(STM32_INCLUDES)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -fno-common
#CFLAGS += -flto
CFLAGS += -ffunction-sections -fdata-sections

R ?= 0
ifeq ($(R), 1)
    CFLAGS += -DRELEASE
endif

#LDFLAGS = -Wl,-T,stm32_flash.ld,-flto,-gc-sections
LDFLAGS = -Wl,-T,stm32_flash.ld,-gc-sections
LIBS = -lm -lc -lnosys

SRC = main.c \
	stm32f7xx_it.c \
	system_stm32f7xx.c \
	stm32f7xx_hal_msp.c \
	$(HALS)/stm32f7xx_hal.c \
	$(HALS)/stm32f7xx_hal_cortex.c \
	$(HALS)/stm32f7xx_hal_rcc.c \
	$(HALS)/stm32f7xx_hal_rcc_ex.c \
	$(HALS)/stm32f7xx_hal_flash.c \
	$(HALS)/stm32f7xx_hal_flash_ex.c \
	$(HALS)/stm32f7xx_hal_gpio.c \
	$(HALS)/stm32f7xx_hal_dma.c \
	$(HALS)/stm32f7xx_hal_dma2d.c \
	$(HALS)/stm32f7xx_hal_pwr.c \
	$(HALS)/stm32f7xx_hal_pwr_ex.c \
	$(HALS)/stm32f7xx_hal_pcd.c \
	$(HALS)/stm32f7xx_hal_pcd_ex.c \
	$(HALS)/stm32f7xx_hal_spi.c \
	$(HALS)/stm32f7xx_hal_usart.c \
	$(HALS)/stm32f7xx_ll_fmc.c \
	$(HALS)/stm32f7xx_ll_usb.c \
	$(wildcard lib/*.c) \
	$(wildcard usbd/*.c) \
	$(USBD)/Core/Src/usbd_core.c \
	$(USBD)/Core/Src/usbd_ctlreq.c \
	$(USBD)/Core/Src/usbd_ioreq.c \
	$(USBD)/Class/DFU/Src/usbd_dfu.c \
	$(WRLIB)/str_buffer.c \
	$(WRLIB)/wrMath.c

OBJDIR = .
OBJS = $(SRC:%.c=$(OBJDIR)/%.o)
OBJS += Startup.o

# C dependencies echoed into Makefile
DEP = $(OBJS:.o=.d)  # one dependency file for each source

# OS dependent size printing
UNAME := $(shell uname)

GETSIZE = stat

ifeq ($(UNAME), Darwin)
	GETSIZE = stat -x
endif


all: $(TARGET).hex $(BIN)

# include all DEP files in the makefile
# will rebuild elements if dependent C headers are changed
-include $(DEP)

$(TARGET).hex: $(EXECUTABLE)
	@$(CP) -O ihex $^ $@

$(EXECUTABLE): $(OBJS)
	@$(LD) -g $(MCFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	@echo "linked:       $@"
	@$(OBJDUMP) --disassemble $@ > $@.lst
	@echo "disassembly:  $@.lst"

$(BIN): $(EXECUTABLE)
	@$(CP) -O binary $< $@
	@echo "binary:       $@"
	@$(OBJDUMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	@echo "symbol table: $@.dmp"
	@echo "Release: "$(R)
	@$(GETSIZE) $(TARGET).bin | grep 'Size'
	@echo "        ^ must be less than 48kB (48,000)"

flash: $(BIN)
	st-flash write $(BIN) 0x08000000

%.o: %.c
	@$(CC) -ggdb $(CFLAGS) -c $< -o $@
	@echo $@

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.s: %.c
	@$(CC) -ggdb $(CFLAGS) -S $< -o $@

Startup.o: $(STARTUP)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo $@

erase:
	st-flash erase

clean:
	@rm -f Startup.lst $(TARGET).elf.lst $(OBJS) $(AUTOGEN) \
	$(TARGET).bin  $(TARGET).out  $(TARGET).hex \
	$(TARGET).map  $(TARGET).dmp  $(EXECUTABLE) $(DEP)
