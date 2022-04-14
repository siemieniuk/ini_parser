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
	int size;
};

struct Content
{
	struct Section* sects;
	int num_sects;
};

bool check_legal_char(char ch);
enum Line_type classify_line(char line[]);
char* read_line(FILE* fp);
int get_length(char str[]);
struct Section create_section(char line[]);
void add_entry_to_sect(struct Section* sect, char line[]);

struct Content parse_ini_file(char path[]);

int find_dot(char ident[]);
char* get_value(struct Content* cont, char ident[]);
bool is_number(char s[]);
int str_to_int(char s[]);
bool check_equal_str(char s1[], char s2[]);
#endif