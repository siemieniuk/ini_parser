#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

int main1(int argc, char *argv[])
{
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
        if(val != NULL)
            printf("%s\n", val);
    }

    return 0;
}

int main()
{
    struct Content cont = parse_ini_file("test_file.ini");
    
    printf("Harmanem.key3 = %s\n", get_value(&cont, "Harmanem.key3"));
    if (check_equal_str("kekw", "kewkw"))
        printf("Correct\n");
    else
        printf("Wrong!\n");
    return 0;
}