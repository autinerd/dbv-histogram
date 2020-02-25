#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define println(format, ...) printf(format "\n", __VA_ARGS__)

#define ZERO_EIGHTS " "
#define ONE_EIGHTS "▁"
#define TWO_EIGHTS "▂"
#define THREE_EIGHTS "▃"
#define FOUR_EIGHTS "▄"
#define FIVE_EIGHTS "▅"
#define SIX_EIGHTS "▆"
#define SEVEN_EIGHTS "▇"
#define EIGHT_EIGHTS "█"

void getHistogram(pgm *picture, uint32_t *buf);

int main(int argc, char **argv)
{
    pgm picture;
    getPgmPicture("Britishblue.pgm", &picture);
    println("Breite: %u", picture.width);
    println("Höhe: %u", picture.height);
    println("Anzahl Pixel: %u", picture.height * picture.width);
    uint32_t buf[256];
    getHistogram(&picture, buf);
    printf("Histogramm: [");
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        printf("%u, ", buf[i]);
    }
    println("%u]", buf[UINT8_MAX]);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    println("terminal width: %u", w.ws_col);
    free(picture.map);
    return 0;
}

void getHistogram(pgm *picture, uint32_t *buf)
{
    for (uint16_t i = 0; i <= 255; i++)
    {
        buf[i] = 0;
    }
    for (uint64_t i2 = 0; i2 < picture->height * picture->width; i2++)
    {
        buf[picture->map[i2]]++;
    }
}

void generateHisto(uint8_t* buf, char* char_buf)
{
    for (uint16_t i = 0; i <= 255; i++)
    {
        switch (buf[i] * 8 / 255)
        {
        case 0:
            char_buf[i] = ZERO_EIGHTS;
            break;
        
        default:
            break;
        }
    }
}