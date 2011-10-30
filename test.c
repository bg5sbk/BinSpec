#include <stdio.h>
#include <stdlib.h>

/*
#define MY_TYPE(T, NAME) \
struct Name { \
    T value; \
}; \

typedef struct doc doc;
typedef struct doc_content doc_content;

MY_TYPE(struct doc {
    doc_content content;
}, doc_content);
*/

typedef struct a a;

struct a {
    struct b {
        a y;
    } x;
};

int main(int argc, char **argv) {
    return 0;
}
