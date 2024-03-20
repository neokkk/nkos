#include <cstring>
#include "console.h"
#include "font.h"

void Console::PutString(const char *s) {
  while (*s) {
    if (*s == '\n') {
      NewLine();
    } else if (cursor_column_ < kColumns - 1) {
      WriteAscii(writer_, cursor_column_ * 8, cursor_row_ * 16, *s, fg_color_);
      buffer_[cursor_row_][cursor_column_] = *s;
      ++cursor_column_;
    }
    ++s;
  }
}

void Console::NewLine() {
  cursor_column_ = 0;

  if (cursor_row_ < kRows - 1) {
    ++cursor_row_;
  } else {
    for (int y = 0; y < 16 * kRows; ++y) {
      for (int x = 0; x < 8 * kColumns; ++x) {
        writer_.Write(x, y, bg_color_);
      }
    }

    for (int row = 0; row < kRows - 1; ++row) {
      memcpy(buffer_[row], buffer_[row + 1], kColumns + 1);
      for (int col = 0; col < kColumns; ++col) {
        WriteAscii(writer_, 8 * col, 16 * row, buffer_[row][col], fg_color_);
      }
    }
    
    memset(buffer_[kRows - 1], 0, kColumns + 1);
  }
}