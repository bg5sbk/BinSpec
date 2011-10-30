#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "bs_parse.h"

#define KEYWORD_INDEX_SIZE 26
#define KEYWORD_CHUNK_SIZE 8

typedef enum {
    TK_UNKNOW = 0,
    TK_EOF,
 
    TK_NAME,  // [a-zA-Z_][a-zA-Z0-9_]*
    TK_NUM,   // [0-9]+
    TK_PKG,   // pkg  
    TK_DEF,   // def 
    TK_ENUM,  // enum
    TK_EQUAL  =  '=',
    TK_COLON  =  ':',
    TK_SEMI   =  ';',
    TK_COMMA  =  ',',
    TK_OBRACE =  '{',
    TK_CBRACE =  '}',
    TK_LT     =  '<',
    TK_GT     =  '>',

    // int[8|16|32|64]
    TK_I8 = 130, TK_I16, TK_I32, TK_I64,

    // uint[8|16|32|64]
    TK_U8, TK_U16, TK_U32, TK_U64,

    // str[8|16|32|64]
    TK_S8, TK_S16, TK_S32, TK_S64,

    // list[8|16|32|64]
    TK_L8, TK_L16, TK_L32, TK_L64,

    // enum
    TK_E8, TK_E16, TK_E32, TK_E64
} token_type;

typedef struct {
    token_type type;
    char*      str;
    size_t     len;
} token;

typedef struct {
    char *cur;
    char *end;
    int   error;
    int   line;
    int   column;
} context;

typedef struct {
    char      *str;
    size_t     len;
    token_type type;
} keyword;

typedef struct {
    char c;
    keyword chunks[KEYWORD_CHUNK_SIZE];
} keyword_tree;

static keyword_tree keywords[KEYWORD_INDEX_SIZE] = {
    {'a', {}},
    {'b', {
      {"byte",     4, TK_I8}
    }},
    {'c', {}},
    {'d', {
      {"def",      3, TK_DEF},
    }},
    {'e', {
      {"enum",     4, TK_ENUM},
    }},
    {'f', {}},
    {'g', {}},
    {'h', {}},
    {'i', {
      {"int",      3, TK_I32},
      {"int8",     4, TK_I8},
      {"int16",    5, TK_I16},
      {"int32",    5, TK_I32},
      {"int64",    5, TK_I64},
    }},
    {'j', {}},
    {'k', {}},
    {'l', {
      {"long",     4, TK_I64},
      {"list",     4, TK_L8},
      {"list8",    5, TK_L8},
      {"list16",   6, TK_L16},
      {"list32",   6, TK_L32},
      {"list64",   6, TK_L64}
    }},
    {'m', {}},
    {'n', {}},
    {'o', {}},
    {'p', {
      {"pkg",      3, TK_PKG},
    }},
    {'q', {}},
    {'r', {}},
    {'s', {
      {"short",    5, TK_I16},
      {"string",   6, TK_S8},
      {"string8",  7, TK_S8},
      {"string16", 8, TK_S16},
      {"string32", 8, TK_S32},
      {"string64", 8, TK_S64},
    }},
    {'t', {}},
    {'u', {
      {"uint",     4, TK_U32},
      {"uint8",    5, TK_U8},
      {"uint16",   6, TK_U16},
      {"uint32",   6, TK_U32},
      {"uint64",   6, TK_U64},
      {"ubyte",    5, TK_U8},
      {"ushort",   6, TK_U16},
      {"ulong",    5, TK_U64}
    }},
    {'v', {}},
    {'w', {}},
    {'x', {}},
    {'y', {}},
    {'z', {}}
};

#define NEXT_CHAR() { ctx->column ++; ctx->cur ++; c = *(ctx->cur); }

#define LOOKUP(tk, err)     \
    t = next_token(ctx);    \
    if (tk != t.type) {     \
        ctx->error = err;   \
        return;             \
    }                       \

#define LOOKUP2(tk, err)    \
    if (tk != t.type) {     \
        ctx->error = err;   \
        return;             \
    }                       \

inline static int parse_int(char *str, size_t len) {
    int val = 0;
    int i;
    for (i = 0; i < len; i ++) {
        val += (str[i] - '0') * pow(10, len - i - 1);
    }
    return val;
}

inline static token_type match_keyword(char *code, size_t len) {
    int i = code[0] - 'a';

    if (i < 0 || i >= KEYWORD_INDEX_SIZE)
        return TK_UNKNOW;

    keyword *chunk = keywords[i].chunks;

    int j;
    
    for (j = 0; j < KEYWORD_CHUNK_SIZE; j ++) {
        if (chunk[j].len != len)
            continue;

        if (0 == strncmp(code, chunk[j].str, len))
            return chunk[j].type;
    }

    return TK_NAME;
}

inline static void ignore_wac(context *ctx) {
    char c = *(ctx->cur);

START:

    /* ignore whitespace */
    while (ctx->cur != ctx->end &&
    (' ' == c || '\t' == c || '\r' == c || '\n' == c)) {
        if ('\r' == c || '\n' == c) {
            char t = c;
            ctx->line ++;
            ctx->column = 1;

            NEXT_CHAR();

            if ('\r' == t && '\n' == c && ctx->cur != ctx->end)
                NEXT_CHAR();
        } else {
            NEXT_CHAR();
        }
    }

    /* ignore single line comment */
    if (ctx->cur != ctx->end - 1 && '/' == c && '/' == *(ctx->cur + 1)) {
        for (;;) {
            NEXT_CHAR();

            if (ctx->cur == ctx->end)
                return;

            if ('\r' == c || '\n' == c) {
                char t = c;
                ctx->line ++;
                ctx->column = 1;

                NEXT_CHAR();

                if ('\r' == t && '\n' == c && ctx->cur != ctx->end)
                    NEXT_CHAR();

                goto START;
            }
        }
    }

    /* ignore multi line comment */
    if (ctx->cur != ctx->end - 1 && '/' == c && '*' == *(ctx->cur + 1)) {
        for(;;) {
            NEXT_CHAR();

            if (ctx->cur >= ctx->end - 1)
                return;

            if ('*' == c && '/' == *(ctx->cur + 1)) {
                NEXT_CHAR();
                NEXT_CHAR();

                goto START;
            }

            if ('\r' == c || '\n' == c) {
                char t = c;
                ctx->line ++;
                ctx->column = 1;

                NEXT_CHAR();

                if ('\r' == t && '\n' == c && ctx->cur != ctx->end)
                    NEXT_CHAR();
            }
        }
    }
}

inline static token next_token(context *ctx) {
    ignore_wac(ctx);

    char c = *(ctx->cur);

    token t = {TK_UNKNOW, ctx->cur, 1};
    
    /* end of file */
    if (ctx->cur == ctx->end) {
        t.type = TK_EOF;
        return t;
    }

    /* operators */
    switch (c) {
        case '=' : t.type = '='; ctx->cur ++; return t;
        case ':' : t.type = ':'; ctx->cur ++; return t;
        case ',' : t.type = ','; ctx->cur ++; return t;
        case '{' : t.type = '{'; ctx->cur ++; return t;
        case '}' : t.type = '}'; ctx->cur ++; return t;
        case ';' : t.type = ';'; ctx->cur ++; return t;
        case '<' : t.type = '<'; ctx->cur ++; return t;
        case '>' : t.type = '>'; ctx->cur ++; return t;
    }

    /* numeric */
    if (isdigit(c)) {
        NEXT_CHAR();
        while (ctx->cur != ctx->end && isdigit(c)) {
            NEXT_CHAR();
            t.len ++;
        }
        t.type = TK_NUM;
        return t;
    }

    /* name */
    if (isalpha(c) || '_' == c) {
        NEXT_CHAR();
        while (ctx->cur != ctx->end && 
        (isalpha(c) || isdigit(c) || '_' == c)) {
            NEXT_CHAR();
            t.len ++;
        }
        t.type = match_keyword(t.str, t.len);
        return t;
    }

    return t;
}

static void parse_cols(context *ctx, bs_def *def, bs_col_list *list) {
    token t = next_token(ctx);
    
    while (TK_EOF != t.type && '}' != t.type) {
        LOOKUP2(TK_NAME, BS_ERR_MISS_NAME);

        char *name = t.str;
        size_t name_len = t.len;

        LOOKUP(':', ':');
        
        if (TK_I8 > (t = next_token(ctx)).type) {
            ctx->error = BS_ERR_BAD_TYPE;
            return;
        }

        bs_type type = t.type;
        bs_col *col = bs_col_new(def, name, name_len, type);
        bs_col_list_add(list, col);

        if (TK_L8 <= t.type && TK_E64 >= t.type) {
            LOOKUP('{', '{');

            parse_cols(ctx, def, col->cols);

            if (ctx->error != 0)
                return;
        }

        if (',' == (t = next_token(ctx)).type)
            t = next_token(ctx);
        else {
            LOOKUP2('}', '}');
            break;
        }
    }
}

static void parse_def(context *ctx, bs_pkg *pkg) {
    token t;

    LOOKUP(TK_NAME, BS_ERR_MISS_NAME);    

    char *name = t.str;
    size_t name_len = t.len;

    LOOKUP('=', '=');
    LOOKUP(TK_NUM, BS_ERR_MISS_ID);

    int id = parse_int(t.str, t.len);

    LOOKUP('{', '{');

    bs_def *def = bs_def_new(pkg, id, name, name_len + 0);
    bs_def_list_add(pkg->defs, def);

    parse_cols(ctx, def, def->cols);
}

static void parse_enum(context *ctx, bs_pkg *pkg) {
    token t;

    LOOKUP(TK_NAME, BS_ERR_MISS_NAME);

    char *name = t.str;
    size_t name_len = t.len;

    bs_enum *em = bs_enum_new(name, name_len, BS_TI8);
    bs_enum_list_add(pkg->enums, em);

    if (':' == (t = next_token(ctx)).type) {
        if (TK_I8 > (t = next_token(ctx)).type || TK_U64 < t.type) {
            ctx->error = BS_ERR_BAD_TYPE;
            return;
        }
        em->type = t.type;
        t = next_token(ctx);
    }

    LOOKUP2('{', '{');

    t = next_token(ctx);   
 
    while (TK_EOF != t.type && '}' != t.type) {
        LOOKUP2(TK_NAME, BS_ERR_MISS_NAME);

        name = t.str;
        name_len = t.len;

        LOOKUP('=', '=');
        LOOKUP(TK_NUM, BS_ERR_MISS_ENUM_VALUE);

        long value = parse_int(t.str, t.len);

        bs_enum_item *item = bs_enum_item_new(name, name_len, value);
        bs_enum_item_list_add(em->items, item);

        if (',' == (t = next_token(ctx)).type)
            t = next_token(ctx);
        else {
            LOOKUP2('}', '}');
            break;
        }
    }
}

static void parse_pkg(context *ctx, bs_doc *doc, bs_pkg_list *list) {
    token t; 

    LOOKUP(TK_NAME, BS_ERR_MISS_NAME);

    char *name = t.str;
    size_t name_len = t.len;

    LOOKUP('=', '=');
    LOOKUP(TK_NUM, BS_ERR_MISS_ID);

    int id = parse_int(t.str, t.len);

    LOOKUP('{', '{');

    bs_pkg *pkg = bs_pkg_new(doc, id, name, name_len + 0);
    bs_pkg_list_add(list, pkg);

    while (TK_EOF != (t = next_token(ctx)).type && '}' != t.type) {
        switch (t.type) {
            case TK_DEF: parse_def(ctx, pkg); break;
            case TK_PKG: parse_pkg(ctx, doc, list); break;
            case TK_ENUM: parse_enum(ctx, pkg); break;
            default: ctx->error = BS_ERR_UNKNOW; break;
        }

        if (ctx->error != 0)
            return;
    }

    LOOKUP2('}', '}');
}

bs_parse_result *bs_parse(char *code, size_t code_len) {
    token t;
    bs_doc *doc = bs_doc_new();
    context ctx = {code, code + code_len, 0, 1, 0};

    while (TK_EOF != (t = next_token(&ctx)).type && ctx.error == 0) {
        if (TK_PKG == t.type) {
            parse_pkg(&ctx, doc, doc->pkgs);
        } else {
            ctx.error = BS_ERR_UNKNOW;
        }
    }

    bs_parse_result *result = (bs_parse_result *)malloc(sizeof(bs_parse_result));
    
    if (ctx.error == 0) {
        result->doc    = doc;
        result->error  = 0;
        result->line   = 0;
        result->column = 0;
    } else {
        result->doc    = NULL;
        result->error  = ctx.error;
        result->line   = ctx.line;
        result->column = ctx.column;

        bs_doc_free(doc);
    }

    return result;
}

void bs_parse_result_free(bs_parse_result *r) {
    if (r->doc != NULL)
        bs_doc_free(r->doc);

    free(r);
}

char *bs_get_error_msg(bs_parse_error error) {
    switch (error) {
        case BS_ERR_UNKNOW: return "unknow token";
        case BS_ERR_MISS_ID: return "missing id";
        case BS_ERR_BAD_TYPE: return "bad type";
        case BS_ERR_MISS_NAME: return "missing name";
        case BS_ERR_MISS_ENUM_VALUE: return "missing enum value";
        case BS_ERR_MISS_EQUI: return "missing '='";
        case BS_ERR_MISS_SEMI: return "missing ';'";
        case BS_ERR_MISS_COLON: return "missing ':'";
        case BS_ERR_MISS_COMMA: return "missing ','";
        case BS_ERR_MISS_OBRACE: return "missing '{'";
        case BS_ERR_MISS_CBRACE: return "missing '}'";
        case BS_ERR_MISS_LT: return "missing '<'";
        case BS_ERR_MISS_GT: return "missing '>'";
    }
    return "";
}
