#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char *replace_char(char *str, char find, char replace) {
    for (char *c = strchr(str, find); c; c=strchr(c, find)) {
        *c = replace;
    }
    return str;
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    int printable_print = 0;
    for (int i=1; i<argc; i++) { // parse args
        if (strcmp(argv[i], "-p") == 0) {
            printable_print = 1;
        } else {
            filename = argv[i];
        }
    }

    if (!filename) {
        fprintf(stderr, "Provide a filename\n");
        return EXIT_FAILURE;
    }

    if (printable_print) { // Print warning about using -p
        fprintf(stderr, "WARNING!!!\n"
                "I've been having flaky results with mixing printable\n"
                "characters and escaped hex values. Also, sticking to just\n"
                "escaped hex generally gives better compressibility.\n");
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "File %s cannot not be read\n", filename);
        return EXIT_FAILURE;
    }

    // convert filename to uppercase
    for (char *current = filename; *current; current++) {
        *current = toupper(*current);
    }
    // replace spaces and dots in filename with underscores
    filename = replace_char(filename, ' ', '_');
    filename = replace_char(filename, '.', '_');

    // print string declaration and beginning quote
    printf("const char *%s = \"", filename);

    unsigned char bytes[2]; // "current" and "next" character of file
    while(!feof(file)) {
        // read "current" and "next" character, file will be seeked back
        // by one at the end of this loop
        int read = fread(bytes, sizeof(unsigned char), 2, file);
        if (printable_print) { // try to use printable characters
            if (isprint(bytes[0])) {
                switch (bytes[0]) { // some of these characters need escaping
                    case '\\': printf("\\\\"); break;
                    case '\"': printf("\\\""); break;
                    default: putchar(bytes[0]);
                }
            } else {
                if(bytes[0] == '\n') { // print newlines as such
                    printf("\\n");
                    // TODO add all escape-able characters
                } else {
                    printf("\\x%02x", bytes[0]);
                    // if next character can be mistaken for a hex number
                    if (read == 2 && (bytes[1] >= 'a' && bytes[1] <= 'f')
                            || (bytes[1] >= 'A' && bytes[1] <= 'F')
                            || (bytes[1] >= '0' && bytes[1] <= '9')) {
                        printf("\"\""); // split the string literal to disambiguate
                    }
                }
            }
        } else { // only print using hex escapes
            printf("\\x%02x", bytes[0]);
        }
        if (read == 2) { // seek back to read one new character at a time
            fseek(file, -1, SEEK_CUR);
        }
    }

    // print ending quote and length variable
    printf("\";const unsigned long %s_LEN=%lul;\n", filename, ftell(file));

    fclose(file);
    return EXIT_SUCCESS;
}
