#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t max_value;
    uint8_t* map;
} pgm;

int8_t getPgmPicture(char* filename, pgm* picture);