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
void free_table(Table_T library);
void freer(const void *key, void **value, void *cl);


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
        free_table(library);
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

        char *clean_line = (char *) malloc(line_size + 1);

        int prev_char_valid = 0;

        for (size_t i = 0; i < line_size; i++) {
                if (valid_char(og_line[i])) {
                        clean_line[*clean_len] = og_line[i];
                        (*clean_len)++;
                        prev_char_valid = 1;
                }
                else if (prev_char_valid == 1) {
                        clean_line[*clean_len] = ' ';
                        (*clean_len)++;
                        prev_char_valid = 0;
                }
        }

        clean_line[(*clean_len)] = '\0';
        return clean_line;
}

/* Purpose : Checks if character is a word-character or not
 * Arguments : Character from line
 * Returns : Boolean true if character is word char, false otherwise
 * Notes : None
 */
bool valid_char(char c){
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_') {
                return true;
        }
        return false;
}

/* Purpose : Stores "cleaned" line in a Hanson Atom which is then stored
 *           in a Hanson Table.
 * Arguments : Clean line, Table that will hold all atoms, line number in
 *             file, and filename
 * Returns : Nothing
 * Notes : The struct Line holds information about where the line is found.
 *         Allocates space in memory to hold this information.
 */
void store_line(char *clean_line, Table_T library, int counter, char *file){
        const char *a = Atom_string(clean_line);
        struct Line *line_info = (struct Line *) malloc(sizeof(struct Line));
        assert(line_info != NULL);
        line_info->line_num = counter;
        line_info->file_name = file;
        store_atom(a, library, line_info);
}

/* Purpose : Stores an Atom in a Hanson Table with its associated List of
 *           Line structs.
 * Arguments : an Atom, a Table, and a pointer to a Line struct
 * Returns : Nothing
 * Notes : If no list is yet associated with an atom, it is created, otherwise
 *         the Line struct is merely appended to the existing list
 */
void store_atom(const char *atom, Table_T library, struct Line *line_info){
        List_T curr_list = Table_get(library, atom);
        if (curr_list == NULL) {
                List_T list = List_push(NULL, line_info);
                Table_put(library, atom, list);
        }
        else {
                curr_list = List_push(curr_list, line_info);
                Table_put(library, atom, curr_list);
        }
}

/* Purpose : Call the mapping funtion that prints output
 * Arguments : Table holding atoms and lists of line structs
 * Returns : Nothing
 * Notes : None
 */
void print_output(Table_T library){
        int first = 1;
        Table_map(library, printer, &first);
}

/* Purpose : "Apply" function, prints the contents of a list associated with
 *            an atom given that the list length is greater than one and there
 *            is more than one occurance of the line
 * Arguments : void ptr to key (atom), void double ptr to value (list), and
 *             void ptr to closure (int indicating whether or not it is the
 *             first line of output)
 * Returns : Nothing
 * Notes : None
 */
void printer(const void *key, void **value, void *cl){

        int *first = cl;
        List_T temp_list = *value;
        temp_list = List_reverse(temp_list);

        struct Line *x;
        if (List_length(temp_list) > 1)
        {
                if (*first == 1){
                        *first = 0;
                }
                else {
                        printf("\n");
                }
                printf("%s\n", (char *)key);
                while (List_length(temp_list) > 0) {
                        temp_list = List_pop(temp_list, (void **)&x);
                        size_t fnl = strlen(x->file_name);
                        if (fnl < 20)
                        {
                                size_t num_spaces = 20 - fnl;
                                printf("%s", x->file_name);
                                for (size_t i = 0; i < num_spaces; ++i)
                                {
                                        printf(" ");
                                }
                                printf(" %d\n", x->line_num);
                        }
                        else if (fnl >= 20){
                                printf("%s %d\n", x->file_name, x->line_num);
                        }

                }
        }
}

/* Purpose : Mapping function that frees table contents
 * Arguments : Table
 * Returns : None
 * Notes : None
 */
void free_table(Table_T library){
        Table_map(library, freer, NULL);
}

/* Purpose : Frees the contents of each list in the table
 * Arguments : void ptr to key (atom), void double ptr to value (list), and
 *             void ptr to closure
 * Returns : Nothing
 * Notes : Atoms cannot be freed from memory 
 */
void freer(const void *key, void **value, void *cl){

}
