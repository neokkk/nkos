CXX = clang++

TARGET = kernel.elf
OBJS = main.o

ARCH = x86_64

CXXFLAGS += -O2 -Wall -g --target=$(ARCH)-elf\
	-ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlibinc -std=c++17 \
	-I/usr/include -I/usr/include/c++/11 -I/usr/include/$(ARCH)-linux-gnu -I/usr/include/$(ARCH)-linux-gnu/c++/11 \
	-D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS

LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static

all: $(TARGET)

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<

kernel.elf: $(OBJS) Makefile
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)