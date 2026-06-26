// Original code adapted from Tsoding`s recorded streams.
// Some modifications have been made by me.
// This is for my personal learning purpose.
#ifndef OLIVE_C_
#define OLIVE_C_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define OLIVEC_SWAP(T, a, b) do {T t = a; a = b; b = t;}while(0)// swap variable a and b, type of 'TYPE'.
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))// x > 1, equals to 1;x < 1, equals to -1. 
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x)*(x))// ABS function

void olivec_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{ 
  for (size_t i = 0; i < width *  height; ++i) {
    pixels[i] = color;
  }
}

// The point of this function is to produce two ranges x1..x2 and y1..y2 that are guaranteed to be safe to iterate over the canvas of size pixels_width by pixels_height without any boundry checks.

//if (olivec_normalize_rect(x, y, w, h, WIDTH, HEIGHT, &x1, &x2, &y1, &y2)) {
//  for (int x = x1; x <= x2; ++x) {
//    for (int y = y1; y <= y2; ++y) {
//      pixels[y*pixels_width + x] = 0x696969;
//    }
//  }
// } else {
//  Rectangle is invisible because it's completely out_of_bounds.//MARK::
// }

bool olivec_normalize_rect(int x, int y, int w, int h,
                           size_t pixels_width, size_t pixels_height,
                           int *x1, int *x2, int *y1, int *y2)
{
  *x1 = x;
  *y1 = y;

  // Convert the rectangle to 2-points representation
  *x2 = *x1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
  if (*x1 > *x2) OLIVEC_SWAP(int, *x1, *x2);
  *y2 = *y1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
  if (*y1 > *y2) OLIVEC_SWAP(int, *y1, *y2);

  // Cull out invisible rectangle
  if (*x1 >= (int) pixels_width) return false;
  if (*x2 < 0) return false;
  if (*y1 >= (int) pixels_height) return false;
  if (*y2 < 0) return false;

  // Clamp the rectangle to the boundaries
  if (*x1 < 0) *x1 = 0;
  if (*x2 >= (int) pixels_width) *x2 = (int) pixels_width - 1;
  if (*y1 < 0) *y1 = 0;
  if (*y2 >= (int) pixels_height) *y2 = (int) pixels_height - 1;

  return true;
} 

void olivec_fill_rect(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x, int y, int w, int h,
                      uint32_t color)// pixels_width and height refers to the size of the canvas, and w\h refer to                                        the size of the rect that is to be printed; 
{
  int x1, y1, x2, y2;
  if (!olivec_normalize_rect(x, y, w, h, pixels_width, pixels_height, &x1, &x2, &y1, &y2)) return;
  
  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      pixels[y*pixels_width + x] = color;
    }
  }
}

void olivec_fill_circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                        int cx, int cy, int r,
                        uint32_t color) // this function creates a circle, centered (cx, cy), radius r.
{
  int x1, y1, x2, y2;
  int r1 = r + OLIVEC_SIGN(int, r);
  if (!olivec_normalize_rect(cx - r1, cy - r1, 2*r1, 2*r1, pixels_width, pixels_height, &x1, &x2, &y1, &y2)) return;
  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      int dx = x - cx;
      int dy = y - cy;
      if (dx*dx + dy*dy <= r*r) {
        pixels[y*pixels_width+x] = color;
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
  if (y1 > y2) {
    OLIVEC_SWAP(int, x1, x2);
    OLIVEC_SWAP(int, y1, y2);
  }
  
  if (y2 > y3) {
    OLIVEC_SWAP(int, x2, x3);
    OLIVEC_SWAP(int, y2, y3);
  }
  
  if (y1 > y2) {
    OLIVEC_SWAP(int, x1, x2);
    OLIVEC_SWAP(int, y1, y2);
  } 
  
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
