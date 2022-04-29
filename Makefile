# Helper for running cmake in the build folder "build"

#-------------------------------------------------------------------------------
# Make defaults
#-------------------------------------------------------------------------------
.SUFFIXES:
# .phony: always build these targets (no "is up to date" message)
.PHONY: help all asm install install_local main.bin
.DEFAULT_GOAL := help

#-------------------------------------------------------------------------------
# Hardware definitions
#-------------------------------------------------------------------------------
CLUSTERS	?= 1
UNITS		?= 1
APP_NAME	?= "HW Verification App (Version 0.0)"

#-------------------------------------------------------------------------------
# Application definitions
#-------------------------------------------------------------------------------
build ?= build
build_release ?= build_release

current_dir = $(shell pwd)
PROJECT_NAME ?= $(current_dir)

# pass configuration as parameters to cmake script
FLAGS=-DCLUSTERS=${CLUSTERS} -DUNITS=${UNITS} -DPROJECT=${PROJECT_NAME}

# default main file
all: clean dir c sim install



#-------------------------------------------------------------------------------
# Simulator Targets
#-------------------------------------------------------------------------------
dir:
	mkdir -p ${build}
	mkdir -p ${build_release}

cp:
ifneq ("$(wildcard CMakeListsAdd.txt)","")
	cp ../ref_cmakeList.txt CMakeLists.txt
	echo "## Appending verification cmake targets to CMakeLists.txt"
	sed -i 's/vpro_project_default_name/${APP_NAME}/g' CMakeLists.txt
	cat CMakeListsAdd.txt >> CMakeLists.txt
else
	cp ../ref_cmakeList.txt CMakeLists.txt
	sed -i 's/vpro_project_default_name/${APP_NAME}/g' CMakeLists.txt
endif

sim: dir cp
	cmake -B ${build} ${FLAGS}
	@$(MAKE) -s  -C ${build} sim -j
	cd ${build} && ./sim

release: dir cp
	cmake -B ${build_release} -Wno-dev -Wno-unused-variable -DCMAKE_BUILD_TYPE=Release ${FLAGS}
	$(MAKE) -s  -C ${build_release} sim -j 
	cd ${build_release} && ./sim #--windowless

# slow configuration. only one thread
# set in defines.h: calc vpro, compare
scripted: dir 
	cmake -B ${build_release} -Wno-dev -DCMAKE_BUILD_TYPE=Release ${FLAGS}
	$(MAKE) -s  -C ${build_release} sim -j 4
	cd ${build_release} && ./sim --windowless











#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------
BIN_PATH_NEW = ../../bin/x86/new_server/
BIN_PATH_OLD = ../../bin/x86/old/

# LLVM clang plugin executable
CLANG_BIN    = $(BIN_PATH_NEW)clang++
LLC_BIN      = $(BIN_PATH_NEW)llc
LLVM_LINK    = $(BIN_PATH_NEW)llvm-link
LLVM_LINKER  = $(BIN_PATH_NEW)ld.lld
LLVM_OBJDUMP = $(BIN_PATH_NEW)llvm-objdump
LLVM_SIZE    = $(BIN_PATH_NEW)llvm-size
LLVM_OBJCOPY = objcopy

# LLVM assembler executable
LLVM_MC_BIN  = $(BIN_PATH_OLD)llvm-mc

#-------------------------------------------------------------------------------
# Library/checker paths
#-------------------------------------------------------------------------------
# MIPS common files
MIPS_COMMON_LIB = ../common
LINKER_SCRIPT   = $(MIPS_COMMON_LIB)/mips_link_script.lds

CHECKER_PATH    = ../../host/$(shell basename $(CURDIR))

ISA_PATH        = ../../isa_intrinsic_lib/
#-------------------------------------------------------------------------------
# Configuration
#-------------------------------------------------------------------------------
COMPILER_EFFORT = -O3
APP_DIR	= $(shell basename $(CURDIR))
HOST_DIR	= ../../host/${APP_DIR}/

CLANG_FLAGS	= --target=mips -msoft-float -mfloat-abi=soft -emit-llvm -I. -I$(ISA_PATH) -S
LLC_FLAGS	= -march=mips -mcpu=mips32 -mattr=+mips32 
LLC_FLAGS	+= -mattr=+vpro -mips-vpro-align=4
LLC_FLAGS	+= -relocation-model=static -mno-check-zero-division -disable-simplify-libcalls 
#LLC_FLAGS	+= -mattr=+soft-float # causes error on link, incompatible to double-float...
LLC_FLAGS	+= -float-abi=soft
LLC_FLAGS	+= --data-sections --function-sections
LLVM_MC_FLAGS	= -assemble -arch=mips -mcpu=mips32 -mattr=+mips32 -mattr=+vpro -filetype=obj
LINKER_FLAGS	= -M --print-gc-sections --check-sections --color-diagnostics --define-common --gdb-index --verbose 
#LINKER_FLAGS 	+= --gc-sections #(will remove all) --cref (prints reference table / used funcs)
VPRO_FLAGS	= -DNUM_CLUSTERS=${CLUSTERS} -DNUM_VU_PER_CLUSTER=${UNITS} -DSTAT_COMMENT=${APP_NAME}

#-------------------------------------------------------------------------------
# BUILD files / dirs
#-------------------------------------------------------------------------------
# directories for different produced files / types
O_DIR = obj
ELF_DIR = elf
ASM_DIR = asm

C_FILES = main.cpp 
C_FILES += $(wildcard $(sources)/*.cpp)

# add files for MIPS/VPRO functions
C_FILES += $(MIPS_COMMON_LIB)/mips_aux.cpp $(MIPS_COMMON_LIB)/vpro_aux.cpp

# generate .bc list from C_FILES. 
# replace $(MIPS_COMMON_LIB) with path MIPS_COMMON_LIB/ (to store object files there)
O_FILES_T = $(patsubst %.cpp,$(O_DIR)/%.bc,$(C_FILES))
O_FILES = $(subst $(MIPS_COMMON_LIB)/,MIPS_COMMON_LIB/, $(O_FILES_T) )

# generate .elf list from O_FILES. 
ELF_FILES = $(patsubst $(O_DIR)/%.bc,$(ELF_DIR)/%.elf,$(O_FILES))

#-------------------------------------------------------------------------------
# Helper Functions
#-------------------------------------------------------------------------------
### Function to get index of a item in a list 
###  e.g. "main.cpp" in C_FILES
###  => used to get corresponding item in O_FILES [path replace of MIPS_COMMON_LIB and other parent dir Source Files]
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
XX := ,
IDX = $(words $(subst $(XX),$(SPACE),$(firstword $(subst $(XX)$(2)$(XX),$(XX)$(2)$(SPACE),$(XX)$(subst $(SPACE),$(XX),$(1))$(XX)))))














#-------------------------------------------------------------------------------
# Application VPRO Hardware / Machine Code Targets
#-------------------------------------------------------------------------------
# to debug
test:
	@echo "Input Source Files to compile:"
	@echo "#####################################################################"
	$(foreach file,$(C_FILES),echo $(file);)
	@echo ""
	@echo "Compile to Object Files for VPRO:"
	@echo "#####################################################################"
	$(foreach file,$(O_FILES),echo $(file);)
	@echo ""
	
# create of .bc file
$(O_DIR)/%.bc:
	@echo Creating $@
	$(shell mkdir -p $(O_DIR))
	$(shell mkdir -p $(O_DIR)/MIPS_COMMON_LIB)
	$(CLANG_BIN) $(COMPILER_EFFORT) $(CLANG_FLAGS) $(word $(call IDX,$(O_FILES),$@),$(C_FILES)) -o $@
# this is the correspondinc c file
#	@echo $(word $(call IDX,$(O_FILES),$@),$(C_FILES)) 
# this is the index in bc files
#	@echo $(call IDX,$(O_FILES),$@) 


# create of asm file
$(ASM_DIR)/%.asm: $(O_DIR)/%.bc
	@echo Creating $@
	$(shell mkdir -p $(ASM_DIR))
	$(shell mkdir -p $(ELF_DIR)/MIPS_COMMON_LIB)
	$(LLC_BIN) $(COMPILE_EFFORT) $(LLC_FLAGS) -filetype=asm --asm-show-inst --asm-verbose $< -o $@

# create of Machine Code elf file (from ASM)
$(ELF_DIR)/%.elf: $(ASM_DIR)/%.asm
	@echo Creating $@
	$(shell mkdir -p $(ASM_DIR))
	$(shell mkdir -p $(ASM_DIR)/MIPS_COMMON_LIB)
	$(LLVM_MC_BIN) $(LLVM_MC_FLAGS) $< -o $@

# Generate of Machine Code crt0.elf file (from ASM)
crt0.elf: 
$(ELF_DIR)/final/crt0.elf: $(MIPS_COMMON_LIB)/crt0.asm
	@echo Creating $@
	$(shell mkdir -p $(ELF_DIR)/final)
	$(LLVM_MC_BIN) $(LLVM_MC_FLAGS) $< -o $@

# Generate linked executbale
final.elf: $(ELF_DIR)/final/crt0.elf $(ELF_FILES)
	@echo Creating $@
	# link against other .so files (e.g. CNN_CONVERTER_LIB)
	$(LLVM_LINKER) -o final.elf --script=$(LINKER_SCRIPT) $(LINKER_FLAGS) $^
	

# S interleave with disassmbly
# w wide mode
# l line numbers
# C C++ symbol names
# r shows symbol names on relocations
# d disassembly
asm: final.elf
	@echo "Disassemle of final.elf [Machine Code including crt0 and all application code]"
	@$(LLVM_OBJDUMP) -SlCwrd final.elf

# Generate final executable binary image
main.bin: final.elf
	@echo Creating $@
	@echo "Dumping .image to image.bin"
	@$(LLVM_OBJCOPY) -j .image -O binary -I elf32-little $< image.bin
	@echo "Dumping .data to data.bin"
	@$(LLVM_OBJCOPY) -j .data -O binary -I elf32-little $< data.bin
	@echo Combine to $@
	@cat image.bin data.bin > $@
	@echo "Section Sizes:"
	@$(LLVM_SIZE) final.elf
	@echo "Cleanup image.bin and data.bin"
	@rm -rf image.bin data.bin

# copy executable to exe folder
install_local: main.bin
	@echo Copy $< to ${HOST_DIR}executable.bin
	@cp $< ${HOST_DIR}executable.bin
	
install_remote: main.cpp
	@echo "calling ../remotecompile.sh"
	@../remotecompile.sh
	@echo Copy main.bin to ${HOST_DIR}executable.bin
	@cp main.bin ${HOST_DIR}executable.bin

	
install: 
	@echo "Compile Local? [Y/n]"
	@read line; if [ $$line = "n" ]; \
	then \
	echo "Selected remote build" && $(MAKE) -s install_remote; exit 0 ; \
	else \
	echo "Installing from local build" && $(MAKE) -s install_local; exit 0; \
	fi;















#-------------------------------------------------------------------------------
# Help
#-------------------------------------------------------------------------------
# .phony: always build these targets (no "is up to date" message)
.PHONY: help
help:
	@echo "--------------------------------------------------------------"
	@echo "VPRO <<${APP_DIR}>> application compilation script"
	@echo "--------------------------------------------------------------"
	@echo "Simulation Targets:"
	@echo " cp           (ENV)  create up to date CMakelists.txt file from ref_ [parent folder] + CMakelistsAdd.txt"
	@echo " dir          (ENV)  creation of build dir for release & debug"
	@echo " sim          (SIM)  cp + compile [cmake+make] debugable executeable + run"
	@echo " release      (SIM)  cp + compile [cmake+make] fast executeable + run"
	@echo " scripted     (SIM)  compile [cmake+make] for script [low parallel] + run [no window]"
	@echo "--------------------------------------------------------------"
	@echo "Hardware Build Targets (Machine Code):"
	@echo " test         (INFO) print input files list"
	@echo " $(O_DIR)/*.bc     (VPRO) Generate LLVM bitcode from sources"
	@echo " $(ELF_DIR)/*.elf    (VPRO) Generate object file from LLVM bitcodes"
	@echo " crt0.elf     (VPRO) Assemble start-up code into object file [$(ELF_DIR)/final/crt0.elf]"
	@echo " final.elf    (VPRO) Link all object files (all elf files)"
	@echo " main.bin     (VPRO) Generate binary executable from final.elf"
	@echo " asm          (VPRO) Print assembly listing from Machine Code (final.elf)"
	@echo "--------------------------------------------------------------"
	@echo "Other Targets:"
	@echo " clean        Clean up this directory"
	@echo " help         Show this text"
	@echo " all          clean + sim + install"
	@echo "--------------------------------------------------------------"

#-------------------------------------------------------------------------------
# Clean-up
#-------------------------------------------------------------------------------
# .phony: always build these targets (no "is up to date" message)
.PHONY: clean clean_sim
clean: clean_sim
	@echo "\n\tCleaning up Hardware workspace..."
	rm -f *.bc *.elf *.bin $(dir ${APP_SRC})*.vpp.c $(dir ${APP_SRC})*.cpp.c *.asm *.sed.c
	rm -f exe/*.bin gen/*.c
	rm -rf remote_compiled
	rm -rf $(O_DIR)
	rm -rf $(ELF_DIR)
	rm -rf $(ASM_DIR)

#	@echo "Cleaning up Simulator workspace..."
#	@$(MAKE) -s  -C ${build} clean
	
clean_sim:
	@echo "\n\tCleaning up Simulator workspace..."
	rm -rf CMakeLists.txt
	rm -rf ${build}
	rm -rf ${build_release}*
	rm -rf init/archive/*.cfg.old
	rm -rf exit/archive/*.cfg.old
	rm -rf data/statistic_out.csv
	rm -rf data/sim_cmd_history.log
	rm -rf data/out*.bin
	rm -rf data/out*.bmp
	rm -rf scripts/log/*.log
	

#-------------------------------------------------------------------------------
# eof
