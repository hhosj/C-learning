#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#include "olive.c"

#define return_defer(value) do { result = (value); goto defer; } while(0)
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(message)\
  do {\
    fprintf(stderr, "%s:%d: UNIMPLEMENTED: %s\n", __FILE__, __LINE__, message);\
    exit(1);\
  }while(0)
#define UNREACHABLE(message)\
  do {\
    fprintf(stderr, "%s:%d: UNREACHEABLE: %s\n", __FILE__, __LINE__, message);\
    exit(1);\
  }while(0)

#define WIDTH  128
#define HEIGHT 128

#define TEST_DIR_PATH "./test"

uint32_t pixels[WIDTH * HEIGHT]

bool record_test_case(const char *file_path)
{
  if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
    fprintf(stderr, "ERROR: could not write file %s: %s\n", file_path, strerror(errno));
    return(false);
  }
  printf("Generated %s\n", file_path);
  return(true);
}

int main(int argc, char **argv)
{
  assert(argc >= 1);
  const char *program_path = argv[0];
  bool record = argc >= 2 && strcmp(argv[1], "record") == 0;

  for (size_t i = 0; i < TEST_CASES_COUNT; ++i) {
    test_cases[i].run();
    if (record) {
      if (!record_test_case(test_cases[i].file_path)) return 1;
    } else {
      if (!replay_test_case(program_path, test_cases[i].file_path, test_cases[i].failure.))
    }
  }
}
