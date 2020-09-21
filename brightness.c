/* Comp 40 - filesnpix - brightness.c
 * Lilianna Houston (lhoust01) and Elise Kaplan (ekap04)
 * September 21, 2020
 * Purpose: Implementation of brightness.c. Accepts images, converts them
 *          to grayscale, and computes the average brightness
 */

#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <assert.h>

void process_image(FILE *image);
float calc_brightness(Pnmrdr_T *pnm, Pnmrdr_mapdata *data);

/* Purpose : Main driver function for brightness implementation
 * Arguments : Number of arguments on command line and vector holding pointers
 *             to each argument
 * Returns : Integer (0), upon program completion
 * Notes : None
 */
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

/* Purpose : Processes image by converting it to Pnmrdr format and checking
 *           that it's a valid file
 * Arguments : File pointer to the image
 * Returns : Nothing
 * Notes : Calls helper function calc_brightness
 */
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

/* Purpose : Calculates the average brightness of the image
 * Arguments : The image and its data/information
 * Returns : Float containing the avg brightness
 * Notes : None
 */
float calc_brightness(Pnmrdr_T *pnm, Pnmrdr_mapdata *data){

    float pixel_num = data->width * data->height;
    float brightness_sum = 0;

    for (int i = 0; i < pixel_num; i++) {
        brightness_sum += Pnmrdr_get(*pnm);
    }

    return (brightness_sum/pixel_num)/data->denominator;
}
