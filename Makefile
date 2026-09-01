# Сборка TF2003-qvm. По умолчанию — только QVM.
#
#   make              — QVM (по умолчанию)
#   make qvm          — только QVM
#   make csqc         — клиентский модуль csprogs.dat (csqc/, нужен fteqcc64 в PATH)
#   make all          — все 5 платформ + QVM (релиз)
#   make current      — native под текущую платформу
#   make <платформа>  — linux-amd64 | linux-armhf | linux-i686 | windows-x64 | windows-x86
#   make clean        — удалить _cmake/
#   make help         — список целей

CMAKE     := cmake
BUILD_DIR := _cmake
PLATFORMS := linux-amd64 linux-armhf linux-i686 windows-x64 windows-x86

.DEFAULT_GOAL := qvm
.PHONY: qvm csqc all current clean help $(PLATFORMS)

qvm:
	$(CMAKE) -S . -B $(BUILD_DIR)/qvm -G Ninja
	$(CMAKE) --build $(BUILD_DIR)/qvm --target qvm

csqc:
	$(MAKE) -C csqc install

all: qvm $(PLATFORMS)

current:
	$(CMAKE) -S . -B $(BUILD_DIR)/current
	$(CMAKE) --build $(BUILD_DIR)/current

# цель для каждой платформы
define PLATFORM_RULE
$(1):
	$(CMAKE) -S . -B $(BUILD_DIR)/$(1) -G Ninja -DCMAKE_TOOLCHAIN_FILE=tools/cross-cmake/$(1).cmake
	$(CMAKE) --build $(BUILD_DIR)/$(1) --config Release
endef
$(foreach p,$(PLATFORMS),$(eval $(call PLATFORM_RULE,$(p))))

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo 'make              — QVM (по умолчанию)'
	@echo 'make qvm          — только QVM'
	@echo 'make csqc         — клиентский модуль csprogs.dat (csqc/, нужен fteqcc64 в PATH)'
	@echo 'make all          — все 5 платформ + QVM (релиз)'
	@echo 'make current      — native под текущую платформу (хост, без кросс-тулчейна)'
	@echo 'make <платформа>  — linux-amd64 | linux-armhf | linux-i686 | windows-x64 | windows-x86'
	@echo 'make clean        — удалить _cmake/'
	@echo 'make help         — этот список'
