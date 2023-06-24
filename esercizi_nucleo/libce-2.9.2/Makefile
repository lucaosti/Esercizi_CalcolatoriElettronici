PREFIX ?= $(HOME)/CE
HD_SIZE=20M

ETC=$(PREFIX)/etc
BIN=$(PREFIX)/bin
LIB32=$(PREFIX)/lib/ce
LIB64=$(PREFIX)/lib64/ce
INCLUDE=$(PREFIX)/include/ce
HD_PATH=$(PREFIX)/share/hd.img
QEMU_BOOT=$(LIB32)/boot.bin
LIBCE_SOURCES=$(PWD)

# Useremo una macchina ospite per compilare e una macchina virtuale
# per eseguire i programmi compilati.
#
# Definiamo le seguenti varibili, in base al sistema operativo/cpu
# della macchina ospite:
#
# - compilatore C++, assembler, linker, archivio per i programmi
#   che devono girare sulla macchina virtuale (a 32 e 64 bit):
#
#       CE_CXX32 CE_CXX64 CE_AS32 CE_AS64 CE_LD32 CE_LD64 CE_AR
#
# - debugger e convertitore indirizzo->linea di codice, da usare sulla macchina
#   ospite, ma per i programmi della macchina virtuale:
#
#   	CE_GDB CE_ADDR2LINE
#
# Per i programmi che devono girare direttamente sulla macchina
# ospite useremo direttamente g++, as, ld, etc.

os:=$(shell uname -s)
cpu:=$(shell uname -m)
ifeq ($(os),Linux)
    ifeq ($(cpu),x86_64)
	CE_CXX32    := g++ -m32
	CE_CXX64    := g++
	CE_AS32     := gcc -c -x assembler-with-cpp -m32
	CE_AS64     := gcc -c -x assembler-with-cpp
	CE_LD32     := ld.gold -melf_i386
	CE_LD64     := ld.gold
	CE_AR       := ar
	CE_GDB      := gdb -nx
	CE_ADDR2LINE:= addr2line
    else ifeq ($(cpu),aarch64)
	CE_CXX32    := i686-linux-gnu-g++
	CE_CXX64    := i686-linux-gnu-g++ -m64
	CE_AS32     := i686-linux-gnu-gcc -c -x assembler-with-cpp
	CE_AS64     := i686-linux-gnu-gcc -c -x assembler-with-cpp -m64
	CE_LD32     := i686-linux-gnu-ld.gold
	CE_LD64     := i686-linux-gnu-ld.gold -melf_x86_64
	CE_AR       := i686-linux-gnu-ar
	CE_GDB      := gdb-multiarch -nx
	CE_ADDR2LINE:= addr2line
    else
$(error architettura sconosciuta: $(cpu))
    endif
else ifeq ($(os),Darwin)
	CE_CXX32    := x86_64-elf-g++ -m32
	CE_CXX64    := x86_64-elf-g++
	CE_AS32     := x86_64-elf-gcc -c -x assembler-with-cpp -m32
	CE_AS64     := x86_64-elf-gcc -c -x assembler-with-cpp
	CE_LD32     := x86_64-ld.gold -melf_i386
	CE_LD64     := x86_64-ld.gold
	CE_AR       := x86_64-elf-ar
	CE_GDB      := x86_64-linux-gnu-gdb --nx
	CE_ADDR2LINE:= x86_64-elf-addr2line
else
$(error sistema operativo sconosciuto: $(os))
endif

CXXFLAGS=-nostdlib -mno-sse -fno-exceptions -g -fno-rtti -fno-stack-protector -mno-red-zone -gdwarf-2 -fpic -fuse-ld=gold -fno-omit-frame-pointer -fcf-protection=none -Werror
ASFLAGS=-g3
BCFLAGS:=$(CXXFLAGS) -fno-pic -fno-PIC -I.
BLDFLAGS:=$(LDFLAGS) -no-pie -L. -L..
BLDLIBS:=-lce32
START_BOOT=	 0x0000000000100000
CXX_SOURCES:=$(wildcard *.cpp)
AS32_SOURCES:=$(wildcard 32/*.s)
AS64_SOURCES:=$(wildcard 64/*.s)
HEADERS:=$(wildcard *.h)
OBJECTS32:=$(CXX_SOURCES:.cpp=-m32.o) $(AS32_SOURCES:.s=_s.o)
OBJECTS64:=$(CXX_SOURCES:.cpp=-m64.o) $(AS64_SOURCES:.s=_s.o)
SCRIPTS_IN:=$(wildcard scripts/*.sh)
SCRIPTS:=$(SCRIPTS_IN:.sh=)

all: libce32.a libce64.a boot.bin start64.o libce.conf $(SCRIPTS)

$(OBJECTS32) $(OBJECTS64): $(HEADERS)

libce32.a: $(OBJECTS32)
	$(CE_AR) r libce32.a $?

libce64.a: $(OBJECTS64)
	$(CE_AR) r libce64.a $?

32/%_s.o: 32/%.s
	$(CE_AS32) $(ASFLAGS) 32/$*.s -o $@

64/%_s.o: 64/%.s
	$(CE_AS64) $(ASFLAGS) 64/$*.s -o $@

%-m32.o: %.cpp
	$(CE_CXX32) -c $(CXXFLAGS) -o $@ $*.cpp

%-m64.o: %.cpp
	$(CE_CXX64) -c $(CXXFLAGS) -o $@ $*.cpp

start64.o: rt/start64.s 
	$(CE_AS64) $(ASFLAGS) rt/start64.s -o $@

boot.bin: boot64/boot_s.o boot64/boot_cpp.o
	$(CE_LD32) $(BLDFLAGS) -o boot.bin -Ttext $(START_BOOT) boot64/boot_s.o boot64/boot_cpp.o $(BLDLIBS)

# compilazione di boot.s e boot.cpp
boot64/boot_s.o: boot64/boot.s mboot.h
	$(CE_AS32) $(BCFLAGS) -c boot64/boot.s -o boot64/boot_s.o

boot64/boot_cpp.o: boot64/boot.cpp mboot.h elf64.h libce.h tipo.h vm.h
	$(CE_CXX32) $(BCFLAGS) -c boot64/boot.cpp -o boot64/boot_cpp.o
     

.PHONY: zerohd clean install libce.conf

$(HD_PATH):
	mkdir -p $(dir $(HD_PATH))
	truncate -s $(HD_SIZE) $(HD_PATH)

zerohd:
	dd if=/dev/zero of=$(HD_PATH) bs=$(HD_SIZE) count=1

libce.conf:
	rm -f $@
	( $(foreach v,BIN LIB32 LIB64 INCLUDE HD_SIZE HD_PATH QEMU_BOOT LIBCE_SOURCES CE_AS32 CE_AS64 CE_CXX32 CE_CXX64 CE_LD32 CE_LD64 CE_AR CE_GDB CE_ADDR2LINE,echo $v="'$($v)'";) ) > $@

scripts/%: scripts/%.sh
	sed 's|@ETC@|$(ETC)|g' $^ > $@

install: $(HD_PATH) 
	install -d 			$(ETC)
	install -m 0444 libce.conf	$(ETC)
	install -d 			$(BIN)
	install -d 			$(LIB32)
	install -d 			$(LIB64)
	install -m 0444 libce32.a	$(LIB32)
	install -m 0444 libce64.a	$(LIB64)
	install -m 0444 start64.o	$(LIB64)
	install -m 0444 libce-debug.py	$(LIB64)
	install -d 			$(INCLUDE)
	install -m 0444 *.h		$(INCLUDE)
	install -m 0444 iostream	$(INCLUDE)
	install -m 0444 libce.s		$(INCLUDE)
	install -m 0444 boot.bin	$(LIB32)
	install -m 0755 $(SCRIPTS)	$(BIN)

clean:
	rm -f *.o 32/*.o 64/*.o libce32.a libce64.a boot.bin boot64/*.o $(SCRIPTS)

+%:
	@echo $*=$($*)
