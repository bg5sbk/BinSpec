#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bs_ast.h"
#include "bs_parse.h"
#include "bs_print.h"
#include "bs_php.h"

typedef enum {
    bs_mode_test,
    bs_mode_format,
    bs_mode_convert
} bs_mode;

static char *file_get_contents(const char *path, size_t *size);
static void print_file(const char *path);

int main(int argc, char **argv) {
    if (argc < 2) {
HELP:
        printf(
        "\n[HOW TO USE]\n\n"
        "1) Test BS code :\n"
        "   bs -t test.bs\n\n"
        "2) Format BS code :\n"
        "   bs -f test.bs > output.bs\n\n"
        "3) Convert BS code to PHP :\n"
        "   bs -c test.bs > output.php\n\n"
        "4) Convert BS code and merge with template :\n"
        "   bs -c test.bs template.php > output.php\n\n"
        "5) Convert BS code and merge then execute :\n"
        "   bs -c test.bs template.php | php\n\n"
        );
        return 0;
    }

    bs_mode mode;

    if (0 == strcmp("-t", argv[1]))
        mode = bs_mode_test;
    else if (0 == strcmp("-f", argv[1]))
        mode = bs_mode_format;
    else if (0 == strcmp("-c", argv[1]))
        mode = bs_mode_convert;
    else
        goto HELP;

    size_t file_size;
    char *file = NULL;


    file = file_get_contents(argv[2], &file_size);

    if (-2 == file_size) {
        printf("file not found: %s\n", argv[2]);
        return 1;
    } else if (-1 == file_size) {
        printf("out of memory\n");
        return 1;
    } else if (0 == file_size) {
        printf("read file failed\n");
        return 1;
    }

    bs_parse_result *result = bs_parse(file, file_size);

    free(file);

    if (result->doc != NULL) {
        if (bs_mode_test == mode)
            printf("ok\n");
        else if (bs_mode_format == mode)
            bs_print_doc(result->doc, 0);
        else if (bs_mode_convert == mode) {
            bs_gen_php(result->doc, stdout);
            if (argc == 4)
                print_file(argv[3]);
        }
    } else {
        printf("%s at line %d column %d\n", bs_get_error_msg(result->error), result->line, result->column);
    }

    bs_parse_result_free(result);

    return 0;
}

static char *file_get_contents(const char *path, size_t *size) {
    FILE *file;
    char *buffer;

    if (NULL == (file = fopen(path, "rb"))) {
        *size = -2;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    if (NULL == (buffer = (char *)malloc(sizeof(char) * (*size)))) {
        fclose(file);
        *size = -1;
        return NULL;
    }

    if (*size != fread(buffer, 1, *size, file)) {
        fclose(file);
        free(buffer);
        *size = 0;
        return NULL;
    }

    fclose(file);

    return buffer;
}

static void print_file(const char *path) {
    FILE *file;

    if (NULL == (file = fopen(path, "r"))) {
        return;
    }

    int c = 0;
    while ((c = getc(file)) != EOF) {
        putc(c, stdout);
    }

    fclose(file);
}

