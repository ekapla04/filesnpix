/*Lilianna Houston*/
/*Comp 40 - Proj 1 - 9/12*/
/*Brightness Notes File*/

#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <assert.h>

void process_image(FILE *image);
float calc_brightness(Pnmrdr_T *pnm, Pnmrdr_mapdata *data);

int main (int argc, char *argv[]){

    FILE *image;

    if (argc > 2) {
        fprintf(stderr, "Error: too many arguments\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2) {
        image = fopen(argv[1], "rb");
    }
    else if (argc == 1){
        image = stdin;
    }

    process_image(image);

return 0;
}

void process_image(FILE *image){
    
    Pnmrdr_T pnm = Pnmrdr_new(image);
    Pnmrdr_mapdata data = Pnmrdr_data(pnm);

    if (data.type != 2){
        fprintf(stderr, "%u\n", data.type);
        fprintf(stderr, "Error: not a pgm\n");
        Pnmrdr_free(&pnm);
        exit(EXIT_FAILURE);
    }

    printf("%.3f\n", calc_brightness(&pnm, &data));
}

float calc_brightness(Pnmrdr_T *pnm, Pnmrdr_mapdata *data){

    float pixel_num = data->width * data->height;
    float brightness_sum = 0;

    for (int i = 0; i < pixel_num; i++) {
        brightness_sum += Pnmrdr_get(*pnm);
    }

    return (brightness_sum/pixel_num)/data->denominator;
}