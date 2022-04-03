#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

int main(int argc, char *argv[])
{
    parse_ini_file(argv[1]);
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("File %s not found", argv[1]);
        exit(1);
    }
    fclose(fp);
    return 0;
}
