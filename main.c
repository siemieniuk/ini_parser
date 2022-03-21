#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct section {
    char* name;
};

struct entry {
    char* key[31];
    char* val[31];
};

struct entry getKeyVal(char* arg) {
    int i=0;
    struct entry e;

    while (arg[i] != '\0' && arg[i] != '.') {
        e.key[i] = (char)arg[i];
    }
    i++;
    int j = 0;
    while (arg[i] != '\0') {
        e.val[j] = (char)arg[i];
        j++;
        i++;
    }
    return e;
}

int main(int argc, char *argv[])
{
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("File %s not found", argv[1]);
        exit(1);
    }

    char* buf[30];

    if (strcpy(argv[2], "expression") == 0) {
        // TODO: expressions
    } else {

    }

    fclose(fp);
    struct entry test = getKeyVal(argv[2]);
    return 0;
}
