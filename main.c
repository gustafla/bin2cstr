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
    if (argc < 2) {
        fprintf(stderr, "Provide a filename\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "File could not be opened\n");
        return EXIT_FAILURE;
    }

    // convert filename to uppercase
    for (char *current = argv[1]; *current; current++) {
        *current = toupper(*current);
    }
    // replace spaces and dots in filename with underscores
    argv[1] = replace_char(argv[1], ' ', '_');
    argv[1] = replace_char(argv[1], '.', '_');

    // print string declaration and beginning quote
    printf("const char *%s = \"", argv[1]);

    unsigned char bytes[2]; // "current" and "next" character of file
    while(!feof(file)) {
        // read "current" and "next" character, file will be seeked back
        // by one at the end of this loop
        int read = fread(bytes, sizeof(unsigned char), 2, file);
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
        if (read == 2) { // seek back to read one new character at a time
            fseek(file, -1, SEEK_CUR);
        }
    }

    // print ending quote and length variable
    printf("\";const unsigned long %s_LEN=%lul;\n", argv[1], ftell(file));

    fclose(file);
    return EXIT_SUCCESS;
}
