#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define println(format, ...) printf(format "\n", __VA_ARGS__)

#define ZERO_EIGHTS " "
#define ONE_EIGHTS "▁"
#define TWO_EIGHTS "▂"
#define THREE_EIGHTS "▃"
#define FOUR_EIGHTS "▄"
#define FIVE_EIGHTS "▅"
#define SIX_EIGHTS "▆"
#define SEVEN_EIGHTS "▇"
#define EIGHT_EIGHTS "█"
#define HIST_CHAR_LEN 3

void getHistogram(pgm *picture, uint32_t *buf);
void generateHisto(uint32_t *buf, char *char_buf);
uint32_t maxValue(uint32_t *buf);
void printBlock(uint8_t block);
void printHistogram(uint32_t *buf);
uint8_t subtractSaturate(uint8_t a, uint8_t b);

int main(int argc, char *argv[])
{
    pgm picture;
    getPgmPicture(argc > 0 ? argv[1] : "Britishblue.pgm", &picture);
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
    printHistogram(buf);
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

void generateHisto(uint32_t *buf, char *char_buf)
{
    uint32_t max = maxValue(buf);

    for (uint16_t i = 0; i <= 255; i++)
    {
        switch (buf[i] * 8 / max)
        {
        case 0:
            strcat(char_buf, ZERO_EIGHTS);
            break;
        case 1:
            strcat(char_buf, ONE_EIGHTS);
            break;
        case 2:
            strcat(char_buf, TWO_EIGHTS);
            break;
        case 3:
            strcat(char_buf, THREE_EIGHTS);
            break;
        case 4:
            strcat(char_buf, FOUR_EIGHTS);
            break;
        case 5:
            strcat(char_buf, FIVE_EIGHTS);
            break;
        case 6:
            strcat(char_buf, SIX_EIGHTS);
            break;
        case 7:
            strcat(char_buf, SEVEN_EIGHTS);
            break;
        case 8:
            strcat(char_buf, EIGHT_EIGHTS);
            break;
        default:
            break;
        }
    }
}

uint32_t maxValue(uint32_t *buf)
{
    uint32_t max = 0;
    for (uint16_t i = 0; i <= 255; i++)
    {
        max = (buf[i] > max) ? buf[i] : max;
    }
    return max;
}

/**    ████████████████████████████████████████████████████████████████
 *     ████████████████████████████████████████████████████████████████
 *   0 ████████████████████████████████████████████████████████████████  63
 *
 *     ████████████████████████████████████████████████████████████████
 *     ████████████████████████████████████████████████████████████████
 *  64 ████████████████████████████████████████████████████████████████ 127
 * 
 *     ████████████████████████████████████████████████████████████████
 *     ████████████████████████████████████████████████████████████████
 * 128 ████████████████████████████████████████████████████████████████ 191
 * 
 *     ████████████████████████████████████████████████████████████████
 *     ████████████████████████████████████████████████████████████████
 * 192 ████████████████████████████████████████████████████████████████ 255
 */

void printHistogram(uint32_t *buf)
{
    uint8_t scaled_data[256];
    uint32_t max = maxValue(buf);
    for (uint16_t i = 0; i < 256; i++)
    {
        scaled_data[i] = buf[i] * 24 / max;
    }
    printf("Histogramm (skaliert): [");
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        printf("%u, ", scaled_data[i]);
    }
    println("%u]", scaled_data[UINT8_MAX]);
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 3; j++)
        {
            if (j == 2)
            {
                printf("%3d ", i * 64);
            }
            else
            {
                printf("    ");
            }
            for (uint16_t k = 0; k < 64; k++)
            {
                uint8_t temp;
                switch (j)
                {
                case 0:
                    temp = subtractSaturate(scaled_data[i * 64 + k], 16);
                    printBlock(temp > 8 ? 8 : temp);
                    break;
                case 1:
                    temp = subtractSaturate(scaled_data[i * 64 + k], 8);
                    printBlock(temp > 8 ? 8 : temp);
                    break;
                case 2:
                    printBlock(scaled_data[i * 64 + k] > 8 ? 8 : scaled_data[i * 64 + k]);
                    break;
                default:
                    break;
                }
            }
            if (j == 2)
            {
                printf("%4d\n", ((i + 1) * 64) - 1);
            }
            else
            {
                printf("\n");
            }
            
        }
    }
}

uint8_t subtractSaturate(uint8_t a, uint8_t b)
{
    return (((int32_t)a - b) < 0) ? 0 : a - b;
}

void printBlock(uint8_t block)
{
    switch (block)
    {
    case 0:
        printf(ZERO_EIGHTS);
        break;

    case 1:
        printf(ONE_EIGHTS);
        break;

    case 2:
        printf(TWO_EIGHTS);
        break;

    case 3:
        printf(THREE_EIGHTS);
        break;

    case 4:
        printf(FOUR_EIGHTS);
        break;

    case 5:
        printf(FIVE_EIGHTS);
        break;

    case 6:
        printf(SIX_EIGHTS);
        break;

    case 7:
        printf(SEVEN_EIGHTS);
        break;

    case 8:
        printf(EIGHT_EIGHTS);
        break;
    default:
        break;
    }
}