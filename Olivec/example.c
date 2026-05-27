// Original code adapted from Tsoding`s recorded streams.
// Some modifications have been made by me.
// This is for my personal learning purpose.
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "olive.c"

#define WIDTH 800
#define HEIGHT 600

#define COLS 8
#define ROWS 6
#define CELL_WIDTH  (WIDTH/COLS)
#define CELL_HEIGHT (HEIGHT/ROWS)

#define BACKGROUND_COLOR 0xFF202020 // Background color gray 

static uint32_t pixels[HEIGHT * WIDTH];

void olivec_fill_circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                        int cx, int cy, size_t r,
                        uint32_t color) // this function creates a circle, centered (cx, cy), radius r.
{
  int x1 = cx - (int)r;
  int y1 = cy - (int)r;
  int x2 = cx + (int)r;
  int y2 = cy + (int)r;
  for (int y = y1; y <= y2; ++y) {
    if (0 <= y && y < (int)pixels_height) {
      for (int x = x1; x <= x2; ++x) {
        if (0 <= x && x < (int)pixels_width) {
          int dx = x - cx;
          int dy = y - cy;
          if (dx*dx + dy*dy <= r*r) {
            pixels[y*pixels_width + x] = color;
          }
        }
      }
    }
  }
}

bool checkerboard_example(void)
{
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);// BBGGRR

  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      uint32_t color = BACKGROUND_COLOR;
      if ((x + y) % 2 == 0) {
        color = 0xFFFFFFFF;
      }
      olivec_fill_rect(pixels, WIDTH, HEIGHT, x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, color);
    }
  }

  const char *file_path = "checkerboard_example.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
    return false;
  }

  return true;
}

bool circle_example()
{
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
  olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 100, 0xFFFFFFFF);
  
  const char *file_path = "circle_example.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
    return false;
  }
  
  return true;
}

int main(void)
{
  if (!checkerboard_example()) return -1;
  if (!circle_example()) return -1;
  return 0;
}
