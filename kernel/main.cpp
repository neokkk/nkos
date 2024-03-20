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

const PixelColor kDesktopBGColor{45, 118, 237};
const PixelColor kDesktopFGColor{255, 255, 255};

const int kMouseCursorWidth = 15;
const int kMouseCursorHeight = 24;
const char mouse_cursor_shape[kMouseCursorHeight][kMouseCursorWidth + 1] = {
	"@              ",
	"@@             ",
	"@.@            ",
	"@..@           ",
	"@...@          ",
	"@....@         ",
	"@.....@        ",
	"@......@       ",
	"@.......@      ",
	"@........@     ",
	"@.........@    ",
	"@..........@   ",
	"@...........@  ",
	"@............@ ",
	"@.............@",
	"@......@@@@@@@@",
	"@....@@.@      ",
	"@...@ @.@      ",
	"@..@   @.@     ",
	"@.@    @.@     ",
	"@@      @.@    ",
	"@       @.@    ",
	"         @.@   ",
	"          @@   ",
};

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

	const int kFrameWidth = frame_buffer_config.horizontal_resolution;
  const int kFrameHeight = frame_buffer_config.vertical_resolution;

	for (int dy = 0; dy < kMouseCursorHeight; ++dy) {
    for (int dx = 0; dx < kMouseCursorWidth; ++dx) {
      if (mouse_cursor_shape[dy][dx] == '@') {
        pixel_writer->Write(200 + dx, 100 + dy, {0, 0, 0});
      } else if (mouse_cursor_shape[dy][dx] == '.') {
        pixel_writer->Write(200 + dx, 100 + dy, {255, 255, 255});
      }
	  }
  }

	FillRect(*pixel_writer, {0, 0}, {kFrameWidth, kFrameHeight - 50}, kDesktopBGColor); // background
	FillRect(*pixel_writer, {0, kFrameHeight - 50}, {kFrameWidth, 50}, {1, 8, 17}); // task bar
	FillRect(*pixel_writer, {0, kFrameHeight - 50}, {kFrameWidth / 5, 50}, {80, 80, 80}); // start button
	DrawRect(*pixel_writer, {10, kFrameHeight - 40}, {30, 30}, {160, 160, 160}); // start button icon

	console = new(console_buf) Console{*pixel_writer, kDesktopFGColor, kDesktopBGColor};

	printk("Welcome to NkOS!\n");

	while (1) __asm__("hlt");
}
