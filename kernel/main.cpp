#include <cstdarg>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include "console.h"
#include "frame_buffer_config.h"
#include "graphics.h"
#include "font.h"

char console_buf[sizeof(Console)];
Console *console;

void* operator new(std::size_t size, void *buf) {
  return buf;
}

void operator delete(void *obj) noexcept {}

int printk(const char *format, ...) {
	va_list ap;
	int n;
	char s[1024];

	va_start(ap, format);
	n = vsprintf(s, format, ap);
	va_end(ap);

	console->PutString(s);
	return n;
}

extern "C" void KernelMain(const FrameBufferConfig &frame_buffer_config) {
	char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
	PixelWriter *pixel_writer;

	switch (frame_buffer_config.pixel_format) {
		case kPixelRGBResv8BitPerColor:
			pixel_writer = new(pixel_writer_buf) RGBResv8BitPerColorPixelWriter{frame_buffer_config};
			break;
		case kPixelBGRResv8BitPerColor:
			pixel_writer = new(pixel_writer_buf) BGRResv8BitPerColorPixelWriter{frame_buffer_config};
			break;
	}

	for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
		for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
			pixel_writer->Write(x, y, {255, 255, 255}); // white
		}
	}

	for (int x = 0; x < 200; ++x) {
		for (int y = 0; y < 100; ++y) {
			pixel_writer->Write(x, y, {0, 255, 0}); // green
		}
	}

	int i = 0;
	for (char c = '!'; c < '~'; ++c) {
		WriteAscii(*pixel_writer, 8 * i, 50, c, {0, 0, 0});
		++i;
	}

	WriteString(*pixel_writer, 0, 66, "Hello, world!", {0, 0, 0});

	char buf[128];
	sprintf(buf, "1 + 2 = %d", 1 + 2);
	WriteString(*pixel_writer, 0, 82, buf, {0, 0, 0});

	console = new(console_buf) Console{*pixel_writer, {0, 0, 0}, {255, 255, 255}};

	for (int i = 0; i < 16; ++i) {
		printk("printk test: %d\n", i);
	}

	while (1) __asm__("hlt");
}
