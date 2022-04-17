#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

int main(int argc, char *argv[])
{
    // check if there is correct number of parameters
    if (!(argc >= 3 && argc <= 4))
    {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    struct Content cont = parse_ini_file(argv[1]);
    // check if it is "expression" case or not
    if (argc == 4 && strcmp(argv[2], "expression") == 0)
    {
        run_expression(&cont, argv[3]);
    }
    else
    {
        char* val = get_value(&cont, argv[2]);
        if (val == NULL)
            exit(1);
        printf("%s\n", val);
    }
    clear_content(&cont);

    return 0;
}