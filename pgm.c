#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int8_t getPgmPicture(char* filename, pgm* picture) {
    FILE* pic = fopen(filename, "rb");
    char line[64];
    // get type of picture
    fgets(line, sizeof(line), pic);
    // get dimensions of picture
    fgets(line, sizeof(line), pic);
    picture->width = strtoul(strtok(line," "), NULL, 10);
    picture->height = strtoul(strtok(NULL," "), NULL, 10);
    // get max value of graymap
    fgets(line, sizeof(line), pic);
    picture->max_value = (uint8_t)strtoul(line, NULL, 10);
    picture->map = malloc(picture->height * picture->width);
    if (picture->map == NULL) {
        return -1;
    }
    for (uint64_t i = 0; i < picture->height * picture->width; i++) {
        picture->map[i] = fgetc(pic);
    }
    fclose(pic);
    return 0;
}