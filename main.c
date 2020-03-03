#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG 0

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

#define HIST_HEIGHT 5

void getHistogram(pgm *picture, uint32_t *buf);
uint32_t maxValue();
void printBlock(uint8_t block);
void printHistogram(uint32_t *buf, uint8_t brightness, uint8_t contrast);
uint8_t subtractSaturate(uint8_t a, uint8_t b);
double calcBrightness(pgm *picture);
double calcContrast(pgm *picture);
double calcEntropy(pgm *picture);
void printTable(pgm* picture, double brightness, double contrast, double entropy);

uint32_t histogram[256];

int main(int argc, char *argv[])
{
    pgm picture;
    if (argc != 2)
    {
        printf("Keine Datei angegeben.\n");
        return 1;
    }
    if (getPgmPicture(argv[1], &picture) != 0)
    {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }
    getHistogram(&picture, histogram);
#if DEBUG == 1
    printf("Histogramm: [");
    for (uint8_t i = 0; i < UINT8_MAX; i++)
    {
        printf("%u, ", histogram[i]);
    }
    println("%u]", histogram[UINT8_MAX]);
#endif
    printTable(&picture, calcBrightness(&picture), calcContrast(&picture), calcEntropy(&picture));
    printHistogram(histogram, (uint8_t)floor(calcBrightness(&picture)), (uint8_t)floor(calcContrast(&picture)));
    free(picture.map);
    return 0;
}

void getHistogram(pgm *picture, uint32_t *histogram)
{
    for (uint64_t i = 0; i < picture->height * picture->width; i++)
    {
        histogram[picture->map[i]]++;
    }
}

double calcBrightness(pgm *picture)
{
    uint64_t sum = 0ULL;
    for (uint16_t val = 0; val < 256; val++)
    {
        sum += histogram[val] * val;
    }
    return ((double)sum / (picture->height * picture->width));
}

double calcContrast(pgm *picture)
{
    uint8_t brightness = (uint8_t)calcBrightness(picture);
    uint64_t sum = 0UL;
    for (uint16_t val = 0; val < 256; val++)
    {
        sum += histogram[val] * (val - brightness) * (val - brightness);
    }
    return sqrt(sum / (double)(picture->height * picture->width));
}

double calcEntropy(pgm *picture)
{
    double sum = 0.0;
    double count = picture->height * picture->width;
    for (uint16_t val = 0; val < 256; val++)
    {
        if (histogram[val] == 0) continue;
        sum += (histogram[val] / count) * log2(histogram[val] / count);
    }
    return -sum;
}

uint32_t maxValue()
{
    uint32_t max = 0;
    for (uint16_t i = 0; i <= 255; i++)
    {
        max = (histogram[i] > max) ? histogram[i] : max;
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

void printHistogram(uint32_t *buf, uint8_t brightness, uint8_t contrast)
{
    uint8_t scaled_data[256];
    uint32_t max = maxValue();
    for (uint16_t i = 0; i < 256; i++)
    {
        scaled_data[i] = buf[i] * (8 * HIST_HEIGHT) / max;
    }
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < HIST_HEIGHT; j++)
        {
            if (j == HIST_HEIGHT - 1)
            {
                printf("%3d ", i * 64);
            }
            else
            {
                printf("    ");
            }
            printf("\e[100m");
            for (uint16_t k = 0; k < 64; k++)
            {
                uint8_t temp;
                temp = subtractSaturate(scaled_data[i * 64 + k], (HIST_HEIGHT - j - 1) * 8);
                if (i * 64 + k == brightness)
                {
                    printf("\e[95m\e[45m");
                }
                else if (i * 64 + k >= brightness - contrast && i * 64 + k <= brightness + contrast)
                {
                    printf("\e[91m\e[41m");
                }
                printBlock(temp > 8 ? 8 : temp);
                if (i * 64 + k == brightness)
                {
                    printf("\e[100m\e[39m");
                }
                else if (i * 64 + k >= brightness - contrast && i * 64 + k <= brightness + contrast)
                {
                    printf("\e[100m\e[39m");
                }
                
            }
            printf("\e[0m");
            if (j == HIST_HEIGHT - 1)
            {
                printf("%4d\n", ((i + 1) * 64) - 1);
            }
            else
            {
                printf("\n");
            }
            
        }
        printf("    ");
        for (uint8_t k = 0; k < 64; k++)
        {
            printf("\e[48;5;%um \e[0m", 232 + (uint8_t)floor((i * 64 + k) / 11));
        }
        printf("\n\n");
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

void printTable(pgm* picture, double brightness, double contrast, double entropy)
{
    printf("╔══════════════════════╦══════════════════════╗\n");
    printf("║ Breite               ║ %20u ║\n", picture->width);
    printf("║ Höhe                 ║ %20u ║\n", picture->height);
    printf("║ Anzahl Pixel         ║ %20u ║\n", picture->height * picture->width);
    printf("║ \e[95mHelligkeit\e[0m           ║ \e[95m%20.2f\e[0m ║\n", brightness);
    printf("║ \e[91mKontrast\e[0m             ║ \e[91m%20.2f\e[0m ║\n", contrast);
    printf("║ Entropie             ║ %20.2f ║\n", entropy);
    printf("╚══════════════════════╩══════════════════════╝\n\n");
}