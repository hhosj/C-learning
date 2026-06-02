// Original code adapted from Tsoding`s recorded streams.
// Some modifications have been made by me.
// This is for my personal learning purpose.
#ifndef OLIVE_C_
#define OLIVE_C_

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

// WHAT are THESE??
#define OLIVEC_SWAP(T, a, b) do {T t = a; a = b; b = t;}while(0)// swap variable a and b, type of 'TYPE'.
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))// x > 1, equals to 1;x < 1, equals to -1. 
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x)*(x))// ABS function

void olivec_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{ 
  for (size_t i = 0; i < width *  height; ++i) {
    pixels[i] = color;
  }
}

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer; } while (0)

Errno olivec_save_to_ppm_file(uint32_t *pixels,size_t width, size_t height, const char *file_path)
{
  int result = 0;
  FILE *f = NULL;

  {
    f = fopen(file_path, "wb");
    if (f == NULL) return_defer(errno);
    
    fprintf(f, "P6\n%zu %zu 255\n", width, height);
    if (ferror(f)) return_defer(errno);

    for (size_t i = 0; i < width * height; ++i) {
      uint32_t pixel = pixels[i];
      uint8_t bytes[3] = {
        (pixel >> (8 * 0)) & 0xFF,//0xBB
        (pixel >> (8 * 1)) & 0xFF,//0xGG
        (pixel >> (8 * 2)) & 0xFF //0xRR
      };
      fwrite(bytes, sizeof(bytes), 1, f);
      if (ferror(f)) return_defer(errno);
    }
  }

 defer:
  if (f) fclose(f);
  return result;
}

void olivec_fill_rect(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int w, int h,
                      uint32_t color)// pixels_width and height refers to the size of the canvas, and w\h refer to                                        the size of the rect that is to be printed; 
{
  int x2 = x1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);// x2 = x1 + (w - 1)when w > 0 or (w + 1)when w < 0.
  if (x1 > x2) OLIVEC_SWAP(int, x1, x2);
  int y2 = y1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
  if (y1 > y2) OLIVEC_SWAP(int, y1, y2);
  
  for (int y = y1; y <= y2; ++y) {
    // TODO: move boundary checks out of the loops.
    if (0 <= y && y < (int) pixels_height) {
      for (int x = x1; x <= x2; ++x) {
        if (0 <= x && x < (int)pixels_width) {
          pixels[y * pixels_width + x] = color;
        }
      }
    }
  }
}

void olivec_fill_circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                        int cx, int cy, int r,
                        uint32_t color) // this function creates a circle, centered (cx, cy), radius r.
{
  if (r == 0) return;
  
  int x1 = cx - r;
  int x2 = cx + r;
  if (x1 > x2) OLIVEC_SWAP(int, x1, x2);

  int y1 = cy - r;
  int y2 = cy + r;
  if (y1 > y2) OLIVEC_SWAP(int, y1, y2);
  
  for (int y = y1; y <= y2; ++y) {
    // TODO: move boundary checks out of the loops in olivec_fill_circle
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

// TODO: lines with different thicness
void olivec_draw_line(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int x2, int y2,
                      uint32_t color) // Original version.Could be upgraded with Bresenham?
{
  // TODO: fix the olivec_draw_line stairs
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx != 0) {
    int c = y1 - dy * x1 / dx;
    if (x1 > x2) OLIVEC_SWAP(int, x1, x2);
    for (int x = x1; x <= x2; ++x) {
      if (0 <= x && x < (int)pixels_width) {
        int sy1 = dy * x / dx + c;  // sy1 and sy2 are to fill the gap when slope is steep
        int sy2 = dy * (x + 1) / dx + c;
        if (sy1 > sy2) OLIVEC_SWAP(int, sy1, sy2);
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
      if (y1 > y2) OLIVEC_SWAP(int, y1, y2);
      for (int y = y1; y < y2; ++y) {
        if (0 <= y && y < (int)pixels_height) {
          pixels[y * pixels_width + x] = color;
        }
      }
    }
  }
}

void olivec_sort_triangle_points_by_y(int *x1, int *y1,
                               int *x2, int *y2,
                               int *x3, int *y3)
{
  if (*y1 > *y2) {
    OLIVEC_SWAP(int, *x1, *x2);
    OLIVEC_SWAP(int, *y1, *y2);
  }

  if (*y2 > *y3) {
    OLIVEC_SWAP(int, *x2, *x3);
    OLIVEC_SWAP(int, *y2, *y3);
  }

  if (*y1 > *y2) {
    OLIVEC_SWAP(int, *x1, *x2);
    OLIVEC_SWAP(int, *y1, *y2);
  }
}

bool olivec_line_of_segment(int x1, int y1, int x2, int y2, int *k, int *c)
{
  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dx ==0) return false;

  *k = dy / dx;
  *c = y1 - dy * x1 / dx;

  return true;
}

// This function is slightly different form Tsoding's code.
void olivec_fill_triangle(uint32_t *pixels, size_t width, size_t height,
                          int x1, int y1,
                          int x2, int y2,
                          int x3, int y3,
                          uint32_t color)
{
  olivec_sort_triangle_points_by_y(&x1, &y1, &x2, &y2, &x3, &y3);

  int dx12 = x2 - x1;
  int dy12 = y2 - y1;
  int dx13 = x3 - x1;
  int dy13 = y3 - y1;
  
  for (int y = y1; y <= y2; ++y) {
    if (0 <= y && y < (int)height) {
      int s1 = dy12 != 0 ? (y - y2) * dx12 / dy12 + x2 : x1; // ks + c = y, s stands for x, k = dy/dx.
      int s2 = dy13 != 0 ? (y - y1) * dx13 / dy13 + x1 : x1;
      if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
      for (int x = s1; x <= s2; ++x) {
        if (0 <= x && x < (int)width) {
          pixels[y * width + x] = color; 
        }
      }
    }
  }

  int dx23 = x3 - x2;
  int dy23 = y3 - y2;
  
  for (int y = y2; y <= y3; ++y) {
    if (0 <= y && y < (int)height) {
      int s1 = dy23 != 0 ? (y - y3) * dx23 / dy23 + x3 : x2; // ks + c = y, s stands for x, k = dy/dx.
      int s2 = dy13 != 0 ? (y - y1) * dx13 / dy13 + x1 : x1;
      if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
      for (int x = s1; x <= s2; ++x) {
        if (0 <= x && x < (int)width) {
          pixels[y * width + x] = color; 
        }
      }
    }
  }
}

#endif //OLIVE_C_
