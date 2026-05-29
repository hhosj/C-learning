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

#define COLS (8*3)
#define ROWS (6*3)
#define CELL_WIDTH  (WIDTH/COLS)
#define CELL_HEIGHT (HEIGHT/ROWS)

#define BACKGROUND_COLOR 0xFF202020 // Background color gray 
#define FOREGROUND_COLOR 0xFFFFFFFF // Foreground color white

static uint32_t pixels[HEIGHT * WIDTH];

void swap_int(int *a, int *b) // Swap a and b.
{
  int t = *a;
  *a = *b;
  *b = t;
}

void olivec_draw_line(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int x2, int y2,
                      uint32_t color) // Original version.Could be upgraded with Bresenham?
{
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx != 0) {
    int c = y1 - dy * x1 / dx;
    if (x1 > x2) swap_int(&x1, &x2);
    for (int x = x1; x <= x2; ++x) {
      if (0 <= x && x < (int)pixels_width) {
        int sy1 = dy * x / dx + c;  // sy1 and sy2 are to fill the gap when slope is steep
        int sy2 = dy * (x + 1) / dx + c;
        if (sy1 > sy2) swap_int(&sy1, &sy2);
        for (int y = sy1; y <= sy2; ++y){
          if (0 <= y && y < (int)pixels_height) {
            pixels[y * pixels_width + x] = color;
          }
        }
      }
    }
  } else { // Vertical line.
    int x = x1;
    if (0 <= x && x < (int)pixels_width) {
      if (y1 > y2) swap_int(&y1, &y2);
      for (int y = y1; y < y2; ++y) {
        if (0 <= y && y < (int)pixels_height) {
          pixels[y * pixels_width + x] = color;
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

float lerpf(float a, float b, float t)// Linear Interpolation, lerpf(a,b,t) = a+t*(b-a), float.
{
  return a + (b - a)*t;
}
  
bool circle_example()
{
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

  size_t radius = CELL_WIDTH / 2;
  if (CELL_HEIGHT < CELL_WIDTH) radius = CELL_HEIGHT / 2;
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      float u = (float)x/COLS;
      float v = (float)y/ROWS;

      float t = (u + v) / 2; 
      olivec_fill_circle(pixels, WIDTH, HEIGHT, x*CELL_WIDTH + CELL_WIDTH/2, y*CELL_HEIGHT + CELL_HEIGHT/2, lerpf(radius/2, radius, t), FOREGROUND_COLOR);
    }
  }
  
  const char *file_path = "circle_example.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
    return false;
  }
  
  return true;
}

bool lines_example(void)
{
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

  olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH/2, 0, WIDTH/2, HEIGHT, FOREGROUND_COLOR);
  
  const char *file_path = "lines_example.ppm";
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
  if (!lines_example()) return -1;
  
  return 0;
}
