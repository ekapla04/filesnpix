#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>

int main(int argc, char *argv[])
{
    FILE *image;

    if (argc > 2) {
        fprintf(stderr, "Error: too many arguments\n");
        exit(EXIT_FAILURE);
    }
    else if (argc <= 2){
        if (argc == 2) {
            image = fopen(argv[1], "rb");
        }
        else {
            image = stdin;
        }
        Pnmrdr_T pnm = Pnmrdr_new(image);
        Pnmrdr_mapdata data = Pnmrdr_data(pnm);
        if (data.type != 2){
            fprintf(stderr, "Error: not a pgm\n");
            Pnmrdr_free(&pnm);
            exit(EXIT_FAILURE);
        }
        unsigned pixel_num = data.width * data.height;
        unsigned brightness_sum = 0;
        for (unsigned i = 0; i < pixel_num; i++) {
            brightness_sum += Pnmrdr_get(pnm);
        }
        float avg_brightness = (brightness_sum/pixel_num)/data.denominator;

        printf("%u, %u, %u, %.3f\n", data.denominator, pixel_num, brightness_sum, avg_brightness);

    }
return 0;
}
