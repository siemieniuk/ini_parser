#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        printf("File %s not found", argv[1]);
        exit(1);
    }
    fclose(fp);
    
    
    struct Content cont = parse_ini_file(argv[1]);

    if(check_equal_str(argv[2], "expression"))
    {
        run_expression(&cont, argv[3]);
    }
    else
    {
        char* val = get_value(&cont, argv[2]);
        if(val == NULL)
            exit(1);
        printf("%s\n", val);
    }

    clear_content(&cont);

    return 0;
}