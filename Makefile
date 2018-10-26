BMP_PORT ?= /dev/cu.usbmodemBDE5A901
ADDR ?= 2


compile:
	mbed compile --profile debug -DADDR=$(ADDR)

flash: compile
	@printf "  BMP $(BMP_PORT) BUILD/DWM1000_STM32L443CC/GCC_ARM-DEBUG/DWM1000_Copter_Integration.elf (flash)\n"
	arm-none-eabi-gdb -nx --batch \
	           -ex 'target extended-remote $(BMP_PORT)' \
	           -x black_magic_probe_flash.scr \
	           BUILD/DWM1000_STM32L443CC/GCC_ARM-DEBUG/DWM1000_Copter_Integration.elf

