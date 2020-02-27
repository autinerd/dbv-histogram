#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define check_null(func) if ((func) == NULL) return -1;

int8_t getPgmPicture(char* filename, pgm* picture) {
    FILE* pic = fopen(filename, "rb");
    if (pic == NULL)
    {
        return -1;
    }
    char line[64];
    // get type of picture
    check_null(fgets(line, sizeof(line), pic));
    // get dimensions of picture
    check_null(fgets(line, sizeof(line), pic));
    picture->width = strtoul(strtok(line," "), NULL, 10);
    picture->height = strtoul(strtok(NULL," "), NULL, 10);
    // get max value of graymap
    check_null(fgets(line, sizeof(line), pic));
    picture->max_value = (uint8_t)strtoul(line, NULL, 10);
    picture->map = malloc(picture->height * picture->width);
    if (picture->map == NULL) {
        return -1;
    }
    for (uint64_t i = 0; i < picture->height * picture->width; i++) {
        int d = fgetc(pic);
        printf("%d, %u\n", d, (uint8_t)d);
        picture->map[i] = (uint8_t)d;
    }
    fclose(pic);
    return 0;
}