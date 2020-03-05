#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t max_value;
    uint8_t* r_map;
    uint8_t* g_map;
    uint8_t* b_map;
} ppm;

int8_t getPpmPicture(char* filename, ppm* picture);