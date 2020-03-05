#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define check_null(func) if ((func) == NULL) return -1;

int8_t getPpmPicture(char* filename, ppm* picture) {
    FILE* pic = fopen(filename, "rb");
    if (pic == NULL)
    {
        return -1;
    }
    char line[64];
    // get type of picture
    check_null(fgets(line, sizeof(line), pic));
    if (strcmp(line, "P6") != 0)
    {
        return -1;
    }
    // get dimensions of picture
    check_null(fgets(line, sizeof(line), pic));
    picture->width = strtoul(strtok(line," "), NULL, 10);
    picture->height = strtoul(strtok(NULL," "), NULL, 10);
    // get max value of graymap
    check_null(fgets(line, sizeof(line), pic));
    picture->max_value = (uint8_t)strtoul(line, NULL, 10);
    picture->r_map = malloc(picture->height * picture->width);
    picture->g_map = malloc(picture->height * picture->width);
    picture->b_map = malloc(picture->height * picture->width);
    if (picture->r_map == NULL || picture->g_map == NULL || picture->b_map == NULL) {
        return -1;
    }
    for (uint64_t i = 0; i < picture->height * picture->width; i++) {
        picture->r_map[i] = (uint8_t)fgetc(pic);
        picture->g_map[i] = (uint8_t)fgetc(pic);
        picture->b_map[i] = (uint8_t)fgetc(pic);
    }
    fclose(pic);
    return 0;
}