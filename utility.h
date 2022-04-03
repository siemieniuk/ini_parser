#ifndef _UTILITY_H
#define _UTILITY_H
#include <stdbool.h>

enum Line_type { COMMENT, SECTION, ENTRY, EMPTY, ERROR };

struct Entry
{
	char* key;
	char* value;
};

struct Section
{
	char* name;
	struct Entry* entries;
};

bool check_legal_char(char ch);
enum Line_type classify_line(char line[]);
char* read_line(FILE* fp);
int get_length(char str[]);
struct Section* create_section(char line[]);

// TODO: finish parse_ini_file (parse_ini_file is not yet functional)
void parse_ini_file(char path[]);
#endif