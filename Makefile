# Variables created for CrazyDriver
KDIR = /lib/modules/$(shell uname -r)/build
DRIVER_PATH = $(PWD)/sources/crazydriver.o


# Default variables for Makefile in Linux
obj-m += sources/crazydriver.o
sources/crazydriver-objs = sources/cr0.o \
							   sources/module.o \
							   sources/hook.o \
							   sources/kallsyms_lookup.o \

ccflags-y += -D_KERNEL -Wimplicit-fallthrough=0 \
	-Wimplicit-fallthrough=0 -I${PWD} -I${PWD}/include -I${PWD}/sources \

# Color codes
GREEN := $(shell tput setaf 2)
RED := $(shell tput setaf 1)
BLUE := $(shell tput setaf 4)
RESET := $(shell tput sgr0)

.PHONY: all clean infos help

all:
	@echo "$(RESET)$(GREEN)[*]$(RESET) Compiling the CrazyDriver Driver ..."
	@make -C $(KDIR) M=$(PWD) modules
	@echo "$(RESET)$(GREEN)[*]$(RESET) Compilation complete."

clean:
	@echo "$(RESET)$(RED)[*]$(RESET) Deleting drivers compilations ..."
	@make -C $(KDIR) M=$(PWD) clean
	@echo "$(RESET)$(GREEN)[*]$(RESET) Deleting complete."

check_driver:
	@if [ ! -e $(DRIVER_PATH) ]; then \
		echo "$(RESET)$(RED)[!]$(RESET) CrazyDriver Driver not found: $(DRIVER_PATH)"; \
		exit 1; \
	fi

install: check_driver
	@echo "$(RESET)$(BLUE)[*]$(RESET) Installing drivers ..."
	@sudo insmod $(DRIVER_PATH)
	@echo "$(RESET)$(BLUE)[*]$(RESET) Drivers Installed Successfully ..."

uninstall: check_driver
	@echo "$(RESET)$(BLUE)[*]$(RESET) Uninstalling driver ..."
	@sudo rmmod CrazyDriver
	@echo "$(RESET)$(BLUE)[*]$(RESET) Driver Uninstalled Successfully ..."

qemu_start:
	@scripts/qemu/qemu-system-x86.run
