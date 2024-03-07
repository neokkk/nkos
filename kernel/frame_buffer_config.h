#pragma once

#include <stdint.h>

/**
 * EFI_GRAPHICS_PIXEL_FORMAT
 *  - PixelRedGreenBlueReserved8BitPerColor
 *  - PixelBlueGreenRedReserved8BitPerColor
 *  - PixelBitMask
 *  - PixelBltOnly
*/
enum PixelFormat {
  kPixelRGBResv8BitPerColor,
  kPixelBGRResv8BitPerColor,
};

struct FrameBufferConfig {
  uint8_t *frame_buffer;
  uint32_t pixels_per_scan_line;
  uint32_t horizontal_resolution;
  uint32_t vertical_resolution;
  enum PixelFormat pixel_format;
};

#ifdef __cplusplus
struct PixelColor {
	uint8_t r, g, b;
};

class PixelWriter {
public:
	PixelWriter(const FrameBufferConfig &config) : config_{config} {}

	virtual ~PixelWriter() = default;
	virtual void Write(int x, int y, const PixelColor &c) = 0;

protected:
	uint8_t *PixelAt(int x, int y) {
		return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
	}

private:
	const FrameBufferConfig &config_;
};

class RGBResv8BitPerColorPixelWriter : public PixelWriter {
public:
	using PixelWriter::PixelWriter;

	virtual void Write(int x, int y, const PixelColor &c) override {
		auto p = PixelAt(x, y);
		p[0] = c.r;
		p[1] = c.g;
		p[2] = c.b;
	}
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
public:
	using PixelWriter::PixelWriter;

	virtual void Write(int x, int y, const PixelColor &c) override {
		auto p = PixelAt(x, y);
		p[0] = c.b;
		p[1] = c.g;
		p[2] = c.r;
	}
};
#endif
