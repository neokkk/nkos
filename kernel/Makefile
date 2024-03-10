TARGET = kernel.elf
OBJS = main.o graphics.o font.o hankaku.o newlib_support.o
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

ARCH = x86_64
ARCH_DASHED = $(ARCH:_=-)

CXX = clang++

CXXFLAGS += -O2 -Wall -g --target=$(ARCH)-elf\
	-ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlibinc -std=c++17 \
	-I/usr/include -I/usr/include/c++/11 -I/usr/include/$(ARCH)-linux-gnu -I/usr/include/$(ARCH)-linux-gnu/c++/11 \
	-D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS

LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static

all: $(TARGET)
	@echo "ARCH_DASHED: $(ARCH_DASHED)"

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@

$(TARGET): $(OBJS) Makefile
	ld.lld $(LDFLAGS) -o $(TARGET) $(OBJS)

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
