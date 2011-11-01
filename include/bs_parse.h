#ifndef _BS_PARSE_H_
#define _BS_PARSE_H_

#include "bs_ast.h"

typedef enum {
    BS_ERR_UNKNOW          = 1,
    BS_ERR_MISS_ID         = 2,
    BS_ERR_BAD_TYPE        = 3,
    BS_ERR_MISS_NAME       = 4,
    BS_ERR_MISS_ENUM_VALUE = 5,
    BS_ERR_MISS_EQUI       = '=',
    BS_ERR_MISS_SEMI       = ';',
    BS_ERR_MISS_COLON      = ':',
    BS_ERR_MISS_COMMA      = ',',
    BS_ERR_MISS_OBRACE     = '{',
    BS_ERR_MISS_CBRACE     = '}',
    BS_ERR_MISS_LT         = '<',
    BS_ERR_MISS_GT         = '>'
} bs_parse_error;

typedef struct {
    bs_parse_error error;
    int line;
    int column;

    bs_doc *doc;
} bs_parse_result;

bs_parse_result *bs_parse(char *code, size_t code_len);

void bs_parse_result_free(bs_parse_result *r);

char *bs_get_error_msg(bs_parse_error error);

#endif
