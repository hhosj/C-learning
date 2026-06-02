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

bool triangle_example(void)
{
  olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
  int radius = 5;
  uint32_t color = FOREGROUND_COLOR;

  int x1 = WIDTH/2,   y1 = HEIGHT/8;
  int x2 = WIDTH/8,   y2 = HEIGHT/2;
  int x3 = WIDTH*7/8, y3 = HEIGHT*7/8;
  
  olivec_fill_circle(pixels, WIDTH, HEIGHT, x1, y1, radius, color);
  olivec_fill_circle(pixels, WIDTH, HEIGHT, x2, y2, radius, color);
  olivec_fill_circle(pixels, WIDTH, HEIGHT, x3, y3, radius, color);
  olivec_fill_triangle(pixels, WIDTH, HEIGHT, x1, y1, x2, y2, x3, y3, color);
  
  const char *file_path = "triangle_example.ppm";
  Errno err = olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
  if (err) {
    fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
    return false;
  }
  
  return true;
}

int main2(void)
{
  int x1 = 69,    y1 = 3;
  int x2 = 420,   y2 = 1;
  int x3 = 69420, y3 = 2;
  olivec_sort_triangle_points_by_y(&x1, &y1, &x2, &y2, &x3, &y3);
  printf("x1 = %d, y1 = %d\n", x1, y1);
  printf("x2 = %d, y2 = %d\n", x2, y2);
  printf("x3 = %d, y3 = %d\n", x3, y3);
}

int main(void)
{
  if (!checkerboard_example()) return -1;
  if (!circle_example()) return -1;
  if (!lines_example()) return -1;
  if (!triangle_example()) return -1;
  
  return 0;
}
