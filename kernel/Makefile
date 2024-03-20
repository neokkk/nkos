TARGET = kernel.elf
OBJS = main.o console.o graphics.o font.o hankaku.o
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

ARCH = x86_64
ARCH_DASHED = $(ARCH:_=-)

CXX = g++
# CXX = clang++

CXXFLAGS += -O2 -Wall -g\
	-ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -std=c++17 \
	-I/usr/include -I/usr/include/c++/11 -I/usr/include/$(ARCH)-linux-gnu -I/usr/include/$(ARCH)-linux-gnu/c++/11 \
	-I/usr/local/$(ARCH)-elf/include \
	-L/usr/local/$(ARCH)-elf/lib \
	-D__ELF__ -D_GNU_SOURCE -D_LDBL_EQ_DBL -D_POSIX_TIMERS -D__USE_MISC

ifeq ($(CXX), clang++)
	CXXFLAGS += -nostdlibinc --target=$(ARCH)-elf -I/usr/local/src/llvm-project/libcxx/include/c++/v1
	LD = ld.lld
	LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static
endif

ifeq ($(CXX), g++)
	CXXFLAGS += -nostdlib -lefi -lgnuefi -I/usr/include/efi -I/usr/include/efi/$(ARCH) -I/usr/include/efi/protocol -DEFI_FUNCTION_WRAPPER
	LD = ld
	LDFLAGS += -T./kernel.ld -znocombreloc -static -Bsymbolic
endif

all: $(TARGET)
	@echo "ARCH_DASHED: $(ARCH_DASHED)"

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

$(TARGET): $(OBJS) Makefile
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

hankaku.bin: hankaku.txt
	../tools/make_font.py -o $@ $<

hankaku.o: hankaku.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

.%.d: %.bin
	touch $@

.PHONY: depends
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDS) *.bin
