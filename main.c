#include "pgm.h"
#include "ppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    double brightness;
    double contrast;
    double entropy;
    uint8_t min;
    uint8_t max;
} pic_numbers;

#define VALUE_COUNT 256

#define NO_TYPE 0
#define PGM_TYPE 1
#define PPM_TYPE 2

#define CONTRAST_FG "\e[38;5;51m"
#define CONTRAST_BG "\e[48;5;37m"
#define CONTRAST_COLOR CONTRAST_FG CONTRAST_BG

#define BRIGHTNESS_FG "\e[38;5;13m"
#define BRIGHTNESS_BG "\e[48;5;5m"
#define BRIGHTNESS_COLOR BRIGHTNESS_FG BRIGHTNESS_BG

#define MINMAX_FG "\e[38;5;214m"
#define MINMAX_BG "\e[48;5;202m"
#define MINMAX_COLOR MINMAX_FG MINMAX_BG

#define NORMAL_FG "\e[38;5;7m"
#define NORMAL_BG "\e[48;5;8m"
#define NORMAL_COLOR NORMAL_FG NORMAL_BG

#define RESET "\e[0m"
#define RED 0
#define GREEN 1
#define BLUE 2
#define GRAY 3

#define DEBUG 0

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

uint8_t histWidth = 128;
uint8_t histHeight = 10;

void getHistogram(size_t size, uint8_t *picture, uint32_t *histogram);
uint32_t maxValue();
void printBlock(uint8_t block);
void printHistogram(uint32_t *histogram, pic_numbers* p, uint8_t colorType);
uint8_t subtractSaturate(uint32_t a, uint32_t b);
double calcBrightness(size_t size, uint32_t* histogram);
double calcContrast(size_t size, uint32_t* histogram);
double calcEntropy(size_t size, uint32_t* histogram);
void printTable(pgm *picture, pic_numbers* p);
void printGeneralTable(size_t width, size_t height);
void printColorTable(pic_numbers* p, uint8_t colorType);
uint8_t getMinValue(uint32_t* histogram);
uint8_t getMaxValue(uint32_t* histogram);

int main(int argc, char** argv)
{
    pgm picture;
    ppm picture_ppm;
    int8_t loadResult = 0;
    uint8_t picType = NO_TYPE;
    if (argc < 2)
    {
        printf("Keine Datei angegeben.\n");
        return 1;
    }
    if (argc == 3)
    {
        if ((histHeight = strtoul(argv[2], NULL, 10)) == 0)
        {
            histHeight = 10;
        }
    }
    if (strcmp(&(argv[1][strlen(argv[1]) - 4]), ".ppm") == 0)
    {
        picType = PPM_TYPE;
        loadResult = getPpmPicture(argv[1], &picture_ppm);
    }
    else if (strcmp(&(argv[1][strlen(argv[1]) - 4]), ".pgm") == 0)  
    {
        picType = PGM_TYPE;
        loadResult = getPgmPicture(argv[1], &picture);
    }
    else
    {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }
    if (loadResult != 0)
    {
        printf("Datei konnte nicht geöffnet werden.\n");
        return 1;
    }
    if (picType == PPM_TYPE)
    {
        size_t size = picture_ppm.width * picture_ppm.height;
        printGeneralTable(picture_ppm.width, picture_ppm.height);
        for (uint8_t i = RED; i <= BLUE; i++)
        {
            uint32_t histogram[256] = {0};
            switch (i)
            {
            case RED:
                getHistogram(size, picture_ppm.r_map, histogram);
                break;
            case GREEN:
                getHistogram(size, picture_ppm.g_map, histogram);
                break;
            case BLUE:
                getHistogram(size, picture_ppm.b_map, histogram);
                break;
            default:
                break;
            }
#if DEBUG == 1
            printf("Histogramm: [");
            for (uint8_t i = 0; i < UINT8_MAX; i++)
            {
                printf("%u, ", histogram[i]);
            }
            println("%u]", histogram[UINT8_MAX]);
#endif
            pic_numbers p = {
                calcBrightness(size, histogram),
                calcContrast(size, histogram),
                calcEntropy(size, histogram),
                getMinValue(histogram),
                getMaxValue(histogram)
            };
            printColorTable(&p, i);
            printHistogram(histogram, &p, i);
        }
        free(picture_ppm.r_map);
        free(picture_ppm.g_map);
        free(picture_ppm.b_map);
    }
    else
    {
        uint32_t histogram[256] = {0};
        size_t size = picture.width * picture.height;
        getHistogram(size, picture.map, histogram);
#if DEBUG == 1
        printf("Histogramm: [");
        for (uint8_t i = 0; i < UINT8_MAX; i++)
        {
            printf("%u, ", histogram[i]);
        }
        println("%u]", histogram[UINT8_MAX]);
#endif
        pic_numbers p = {
                calcBrightness(size, histogram),
                calcContrast(size, histogram),
                calcEntropy(size, histogram),
                getMinValue(histogram),
                getMaxValue(histogram)
        };
        printTable(&picture, &p);
        printHistogram(histogram, &p, GRAY);
        free(picture.map);
    }

    return 0;
}

void getHistogram(size_t size, uint8_t *picture, uint32_t* histogram)
{
    for (size_t i = 0; i < size; i++)
    {
        histogram[picture[i]]++;
    }
}

double calcBrightness(size_t size, uint32_t* histogram)
{
    uint64_t sum = 0UL;
    for (uint16_t val = 0; val < 256; val++)
    {
        sum += histogram[val] * val;
    }
    return ((double)sum / size);
}

double calcContrast(size_t size, uint32_t* histogram)
{
    uint8_t brightness = (uint8_t)calcBrightness(size, histogram);
    uint64_t sum = 0UL;
    for (uint16_t val = 0; val < 256; val++)
    {
        sum += histogram[val] * (val - brightness) * (val - brightness);
    }
    return sqrt(sum / (double)size);
}

double calcEntropy(size_t size, uint32_t* histogram)
{
    double sum = 0.0;
    for (uint16_t val = 0; val < 256; val++)
    {
        if (histogram[val] == 0)
            continue;
        sum += ((double)histogram[val] / size) * log2((double)histogram[val] / size);
    }
    return -sum;
}

uint32_t maxValue(uint32_t* histogram)
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

void printHistogram(uint32_t *histogram, pic_numbers* p, uint8_t colorType)
{
    uint8_t brightness = (uint8_t)floor(p->brightness), contrast = (uint8_t)floor(p->contrast);
    uint64_t scaled_data[256];
    uint32_t max = maxValue(histogram);

    for (uint16_t i = 0; i < 256; i++)
    {
        scaled_data[i] = histogram[i] * (8 * histHeight) / max;
    }

    for (uint8_t i = 0; i < ceil(256 / (double)histWidth); i++)
    {
        for (uint16_t j = 0; j < histHeight; j++)
        {
            if (j == histHeight - 1)
            {
                printf("%3d ", i * histWidth);
            }
            else
            {
                printf("    ");
            }
            printf("\e[100m");
            for (uint16_t k = 0; k < histWidth && k < 256; k++)
            {
                uint8_t index = i * histWidth + k;
                uint8_t temp;
                temp = subtractSaturate(scaled_data[index], (histHeight - j - 1) * 8);
                if (index == brightness)
                {
                    printf(BRIGHTNESS_COLOR);
                }
                else if (index == p->min || index == p->max)
                {
                    printf(MINMAX_COLOR); 
                }
                else if (index >= brightness - contrast && index <= brightness + contrast)
                {
                    printf(CONTRAST_COLOR);
                }
                printBlock(temp > 8 ? 8 : temp);
                if (index == brightness || index == p->min || index == p->max)
                {
                    printf(NORMAL_COLOR);
                }
                else if (index >= brightness - contrast && index <= brightness + contrast)
                {
                    printf(NORMAL_COLOR);
                }
            }
            printf("\e[0m");
            if (j == histHeight - 1)
            {
                printf("%4d\n", ((i + 1) * histWidth) - 1);
            }
            else
            {
                printf("\n");
            }
        }
        printf("    ");
        for (uint8_t k = 0; k < histWidth; k++)
        {
            uint8_t index = i * histWidth + k;
            switch (colorType)
            {
            case GRAY:
                printf("\e[48;5;%um \e[0m", 232 + (uint8_t)floor((index) / 11));
                break;
            case RED:
                printf("\e[48;5;%um \e[0m", 16 + 36 * (uint8_t)floor((index) / 55));
                break;
            case GREEN:
                printf("\e[48;5;%um \e[0m", 16 + 6 * (uint8_t)floor((index) / 55));
                break;
            case BLUE:
                printf("\e[48;5;%um \e[0m", 16 + (uint8_t)floor((index) / 55));
                break;
            default:
                break;
            }
            
        }
        printf("\n\n");
    }
}

uint8_t subtractSaturate(uint32_t a, uint32_t b)
{
    int32_t sum = (int32_t)a - (int32_t)b;
    return (sum > 0) ? (sum < 8 ? sum : 8) : 0;
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

void printTable(pgm *picture, pic_numbers* p)
{
    printf("╔══════════════════════╦══════════════════════╗\n");
    printf("║ Breite               ║ %20u ║\n", picture->width);
    printf("║ Höhe                 ║ %20u ║\n", picture->height);
    printf("║ Anzahl Pixel         ║ %20u ║\n", picture->height * picture->width);
    printf("║ " MINMAX_FG "Minimaler Wert" RESET     "       ║ " MINMAX_FG "%20u" RESET       " ║\n", p->min);
    printf("║ " MINMAX_FG "Maximaler Wert" RESET     "       ║ " MINMAX_FG "%20u" RESET       " ║\n", p->max);
    printf("║ " BRIGHTNESS_FG "Helligkeit" RESET "           ║ " BRIGHTNESS_FG "%20.2f" RESET " ║\n", p->brightness);
    printf("║ " CONTRAST_FG   "Kontrast" RESET "             ║ " CONTRAST_FG   "%20.2f" RESET " ║\n", p->contrast);
    printf("║ Entropie             ║ %20.2f ║\n", p->entropy);
    printf("╚══════════════════════╩══════════════════════╝\n\n");
}

void printGeneralTable(size_t width, size_t height)
{
    printf("╔══════════════════════╦══════════════════════╗\n");
    printf("║ Breite               ║ %20lu ║\n", width);
    printf("║ Höhe                 ║ %20lu ║\n", height);
    printf("║ Anzahl Pixel         ║ %20lu ║\n", height * width);
    printf("╚══════════════════════╩══════════════════════╝\n\n");
}

void printColorTable(pic_numbers* p, uint8_t colorType)
{
    uint8_t color = 0;
    switch (colorType)
    {
    case RED:
        color = 196;
        break;
    case GREEN:
        color = 46;
        break;
    case BLUE:
        color = 21;
        break;
    default:
        break;
    }
    printf("\e[38;5;%um╔══════════════════════╦══════════════════════╗" RESET "\n", color);
    printf("\e[38;5;%1$um║" RESET " " BRIGHTNESS_FG "Helligkeit" RESET "           \e[38;5;%1$um║" RESET " " BRIGHTNESS_FG "%2$20.2f" RESET " \e[38;5;%1$um║" RESET "\n", color, p->brightness);
    printf("\e[38;5;%1$um║" RESET " " MINMAX_FG "Minimaler Wert" RESET     "       \e[38;5;%1$um║" RESET " " MINMAX_FG "%2$20u" RESET       " \e[38;5;%1$um║" RESET "\n", color, p->min);
    printf("\e[38;5;%1$um║" RESET " " MINMAX_FG "Maximaler Wert" RESET     "       \e[38;5;%1$um║" RESET " " MINMAX_FG "%2$20u" RESET       " \e[38;5;%1$um║" RESET "\n", color, p->max);
    printf("\e[38;5;%1$um║" RESET " " CONTRAST_FG   "Kontrast" RESET "             \e[38;5;%1$um║" RESET " " CONTRAST_FG   "%2$20.2f" RESET " \e[38;5;%1$um║" RESET "\n", color, p->contrast);
    printf("\e[38;5;%1$um║" RESET " Entropie             \e[38;5;%1$um║" RESET " %2$20.2f \e[38;5;%1$um║" RESET "\n", color, p->entropy);
    printf("\e[38;5;%um╚══════════════════════╩══════════════════════╝" RESET "\n\n", color);
}

uint8_t getMinValue(uint32_t* histogram)
{
    for (size_t i = 0; i < 256; i++)
    {
        if (histogram[i] != 0)
        {
            return i;
        }
    }
    return 255;
}

uint8_t getMaxValue(uint32_t* histogram)
{
    for (size_t i = 255; i > 0; i--)
    {
        if (histogram[i] != 0)
        {
            return i;
        }
    }
    return 0;
}