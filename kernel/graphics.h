#pragma once

#include "frame_buffer_config.h"

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

	virtual void Write(int x, int y, const PixelColor &c) override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
public:
	using PixelWriter::PixelWriter;

	virtual void Write(int x, int y, const PixelColor &c) override;
};

template <typename T>
struct Vector2D {
	T x, y;

	Vector2D<T> &operator+=(const Vector2D<T> &rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
};

void FillRect(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);

void DrawRect(PixelWriter &writer, const Vector2D<int> &pos, const Vector2D<int> &size, const PixelColor &c);
#endif
