/* Comp 40 - filesnpix - readaline.c
 * Lilianna Houston (lhoust01) and Elise Kaplan (ekap04)
 * September 21, 2020
 * Purpose: Implementation of readaline.c. Reads in a line from a file char
 *          by char and returns the number of bytes in the line
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "readaline.h"

size_t readaline(FILE *inputfd, char **datapp);

/* Purpose : Reads in a line from a file char by char and returns the
 *           number of bytes in the line
 * Arguments : File pointer and char double ptr
 * Returns : size_t representing number of bytes in the line
 * Notes : If called when there are no more lines to be read, sets *datapp
 *         to null and returns 0
 */
size_t readaline(FILE *inputfd, char **datapp){
        assert(inputfd != NULL);
        assert(datapp != NULL);
        size_t size = 3;
        size_t line_loc = 0;
        char *line = (char *) malloc(size);
        int c = 0;
        while(feof(inputfd) == 0){
                c = fgetc(inputfd);
                if (feof(inputfd) != 0) {
                        free(line);
                        *datapp = NULL;
                        return 0;
                }
                if (c == '\n'){
                        break;
                }
                if (line_loc == size - 1) {
                        size = 2 * size + 1;
                        line = realloc(line, size);
                        assert(line != NULL);
                }
                line[line_loc] = c;
                line_loc++;
        }
        *datapp = line;
        return line_loc*sizeof(char);
        }
