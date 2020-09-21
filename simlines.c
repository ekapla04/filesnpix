#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <atom.h>
#include <table.h>
#include <list.h>
#include "readaline.h"

struct Line {
        int line_num;
        char* file_name;
};

void process_file(int num_args, char *argument[], Table_T library);
void process_line(FILE *inputfd, Table_T library, int counter, char *file);
char *clean_lines(char *og_line, size_t line_size, int *clean_len);
bool valid_char(char c);
void store_line(char *clean_line, Table_T library, int counter, char *file);
void store_atom(const char *atom, Table_T library, struct Line *line_info);
void print_output(Table_T library);
void printer(const void *key, void **value, void *cl);


/* Purpose : Main driver function for simlines implementation
 * Arguments : Number of arguments on command line and vector holding pointers
 *             to each argument
 * Returns : Integer (0), upon program completion
 * Notes : None
 */
int main(int argc, char *argv[]) {
        Table_T library = Table_new(0, NULL, NULL);
        process_file(argc, argv, library);
        print_output(library);
        return 0;
}

/* Purpose : Tries to open command line files and process
 *           the lines within them
 * Arguments : Number of arguments on command line, vector holding each
 *             argument (file), and a table to later store our lines in
 * Returns : Nothing
 * Notes : Checks that file can be opened. If failure occurs, program aborts
 */
void process_file(int num_args, char *argument[], Table_T library){
        // printf("num args: %d\n", num_args);
        // printf("%s\n",argument[0]);
        // goes one too many iterations.. cant -1
        for (int i = 1; i < num_args; i++) {
                FILE *inputfd;
                // check file open some other way
                inputfd = fopen(argument[i], "rb");
                assert(inputfd != NULL);
                int counter = 1;
                do {
                        process_line(inputfd, library, counter, argument[i]);
                        counter++;
                } while (feof(inputfd) == 0);
                fclose(inputfd);
        }
}

/* Purpose : Driving function of processing a given line.
 *           Processes a line after reading in each character and calls for
 *           line to be cleaned of non-word ascii values
 * Arguments : Input file, table to be used later to store lines, counter
 *             representing the location of the line in the file, and the
 *             filename itself
 * Returns : Nothing
 * Notes : Frees memory to lines once fully processed
 */
void process_line(FILE *inputfd, Table_T library, int counter, char *file){
        char *og_line = "";
        size_t line_size = readaline(inputfd, &og_line);
        // printf("og line: %s\n", og_line);
        // free(og_line);
        int clean_len = 0;
        char *clean_line = clean_lines(og_line, line_size, &clean_len);
        if (clean_len != 0){
                store_line(clean_line, library, counter, file);
        }
        free(clean_line);
        free(og_line);
}

/* Purpose : Goes through original line from file and removes non-word
 *           characters.
 * Arguments : Original line, length of original line, pointer to variable
 *             that stores the length of the clean line.
 * Returns : pointer to the newly cleaned line
 * Notes : allocates space in memory for the clean line. Uses helper
 *         function "valid_char" to determine word characters
 */
char* clean_lines(char *og_line, size_t line_size, int *clean_len){

        for (size_t i = 0; i < line_size; i++) {
                // printf("char in question: %c\n", og_line[i]);
                if (valid_char(og_line[i])) {
                        (*clean_len)++;
                }
        }
        printf("clean length: %d\n", *clean_len);
        char *clean_line = (char *) malloc(*clean_len + 1);
        // WHATS UP: random letters at end of clean line, even after mallocing
        // specific amount of space
        // memset(clean_line, ' ', *clean_len);
        // clean_line[sizeof(int)*line_size)] = { " " };
        // printf("new clean line: %d\n",(int)*clean_line );
        int j = 0;
        assert(clean_line != NULL);
        for (size_t i = 0; i < line_size; i++) {
                if (valid_char(og_line[i])) {
                        clean_line[j] = og_line[i];
                        j++;
                }
        }
        clean_line[*clean_len] = '\0';
        return clean_line;
}

bool valid_char(char c){
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_') {
                return true;
        }
        return false;
}

void store_line(char *clean_line, Table_T library, int counter, char *file){
        // is this how we save atom itself?
        // printf("%s---------------------\n", clean_line);
        const char *a = Atom_string(clean_line);
        // (void)a;
        struct Line *line_info = (struct Line *) malloc(sizeof(struct Line));
        assert(line_info != NULL);
        line_info->line_num = counter;
        line_info->file_name = file;
        store_atom(a, library, line_info);
}

void store_atom(const char *atom, Table_T library, struct Line *line_info){
        List_T curr_list = Table_get(library, atom);
        if (curr_list == NULL) {
                // whats up here: our prog thinks that keys/values that should exist
                // dont. even when recognized, does not push
                List_T list = List_push(NULL, line_info);
                Table_put(library, atom, list);
        }
        else {
                curr_list = List_push(curr_list, line_info);
                Table_put(library, atom, curr_list);
        }

        // not recognizing cleaned duplicates, ones that it does rexognize,
        // length of list isnt incremented
}

void print_output(Table_T library){
        printf("before map\n");
        Table_map(library, printer, NULL);
}

void printer(const void *key, void **value, void *cl){
        printf("in map\n");
        (void)cl;
        printf("%s\n", (char *)key);
        List_T temp_list = *value;
        temp_list = List_reverse(temp_list);

        struct Line *x;
        printf("list len 1: %d\n",List_length(temp_list));
        while (List_length(temp_list) > 0) {
                temp_list = List_pop(temp_list, (void **)&x);
                printf("line num: %d\n",x->line_num);
                printf("file num: %s\n",x->file_name);

                printf("list len 2: %d\n",List_length(temp_list));
        }
        // for (int i = 0; i < List_length(temp_list); i++) {
        //         List_pop(temp_list, (void **)&x);
        //         if (temp_list != NULL) {
        //                 printf("line num: %d\n",x->line_num);
        //                 printf("file num: %s\n",x->file_name);
        //         }
        // }
}
