#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "readaline.h"

size_t readaline(FILE *inputfd, char **datapp);

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
