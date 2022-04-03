#ifndef _UTILITY_H
#define _UTILITY_H
#include <stdbool.h>

enum Line_type { COMMENT, SECTION, ENTRY, EMPTY, ERROR };

struct Entry
{
	char* key;
	char* value;
	struct Entry* next;
};

struct Section
{
	char* name;
	struct Entry* entries;
	struct Section* next;
};

/**
 * Checks if a specified string is a valid name in INI file
 * @param s A string which will be checked
 * @return true if valid, false otherwise
 */
bool is_valid_ini_name(char s[]);

bool check_legal_char(char ch);
enum Line_type classify_line(char line[]);
char* read_line(FILE* fp);
int get_length(char str[]);
struct Section* create_section(char line[]);

// TODO: finish parse_ini_file (parse_ini_file is not yet functional)
void parse_ini_file(int argc, char *argv[]);
#endif