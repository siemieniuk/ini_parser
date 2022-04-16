#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "utility.h"

// Returns "true" if given char is legal
bool check_legal_char(char ch)
{
	if (isalnum(ch) || ch == '-')
		return true;
	return false;
}

// Returns Line_type of given line
enum Line_type classify_line(char line[])
{
	// If nothing in line, then it is empty
	if (line[0] == 0)
		return EMPTY;

	// If ; then line is comment
	if (line[0] == ';')
		return COMMENT;

	// Check if everything is okay in case of SECTION name
	if (line[0] == '[')
	{
		int i = 1;
		while (line[i] != ']')
		{
			if (!check_legal_char(line[i]))
				return ERROR;
			i++;
		}
		if (line[i + 1] != 0)
			return ERROR;
		return SECTION;
	}

	// Check if everything is okay in case of ENTRY
	int i = 0;
	while (line[i] != ' ')
	{
		if (!check_legal_char(line[i]))
			return ERROR;
		i++;
	}
	if (line[++i] != '=')
		return ERROR;
	if (line[++i] != ' ')
		return ERROR;
	i++;
	while (line[i] != 0)
	{
		if (!check_legal_char(line[i]))
			return ERROR;
		i++;
	}
	return ENTRY;
}

// Accepts file pointer and returns pointer to the read string
// ALLOCATES MEMORY
char* read_line(FILE* fp)
{
	int size = 30;
	int len = 0;
	char ch;
	char* line = (char*)malloc(sizeof(char) * size);
	if (!line)
		return line;

	ch = fgetc(fp);
	while (ch != EOF && ch != '\n')
	{
		line[len++] = ch;
		if (len == size)
		{
			size += 10;
			char* line_temp = (char*)realloc(line, sizeof(char) * size);
			if (!line_temp)
				return line_temp;
			line = line_temp;
		}
		ch = fgetc(fp);
	}
	line[len++] = '\0';

	return (char*)realloc(line, sizeof(char) * len);
}

int get_length(char str[])
{
	int i = 0;
	while (str[i] != '\0')
		i++;
	return i;
}

// Returns an empty section with the name, extraxcted from given line
// ALLOCATES MEMORY
struct Section create_section(char line[])
{
	int len = get_length(line);
	char* name = (char*)malloc(sizeof(char) * (len - 1));

	int line_ind = 1, name_ind = 0;
	while (line_ind <= len - 2)
	{
		name[name_ind] = line[line_ind];
		name_ind++;
		line_ind++;
	}
	name[name_ind] = '\0';

	struct Section sect;	
	sect.name = name;
	sect.size = 0;
	sect.entries = NULL;
	return sect;
}

// Returns add an entry to the given sect from given line
// ALLOCATES MEMORY
void add_entry_to_sect(struct Section* sect, char line[])
{
	// Finding position of '=' in line
	int len = get_length(line);
	int eq_ind = 0;
	for(eq_ind = 0; eq_ind < len; eq_ind++)
	{
		if (line[eq_ind] == '=')
			break;
	}


	char* key = (char*)malloc(sizeof(char) * (eq_ind));
	char * val = (char*)malloc(sizeof(char) * (len - eq_ind-1));
	
	// Copying part of line into key
	int curr_ind = 0;
	for(; curr_ind < eq_ind-1; curr_ind++)
	{
		key[curr_ind] = line[curr_ind];
	}
	key[curr_ind] = '\0';
	
	// Copyting part of line into val
	curr_ind = 0;
	int i = eq_ind + 2;
	for(; i < len; i++)
	{
		val[curr_ind] = line[i];
		curr_ind++;
	}
	val[curr_ind] = '\0';

	// Appending new entry to the entries in sect
	sect->size += 1;
	sect->entries = (struct Entry*)realloc(sect->entries, sizeof(struct Entry) * (sect->size));

	struct Entry new_entry;
	new_entry.key = key;
	new_entry.value = val;
	sect->entries[sect->size - 1] = new_entry;
}

// Returns the total Content, extracted from the given file
struct Content parse_ini_file(char path[])
{
	FILE* fp = fopen(path, "r");

	struct Section* sects = NULL;
	int num_sects = 0;
	while (!feof(fp))
	{
		char* line = NULL;
		line = read_line(fp);

		enum Line_type line_t = classify_line(line);

		if (line_t == SECTION)
		{
			num_sects++;
			sects = (struct Section*)realloc(sects, sizeof(struct Section) * num_sects);
			sects[num_sects - 1] = create_section(line);
			
		}
		else if (line_t == ENTRY)
		{
			add_entry_to_sect(&sects[num_sects - 1], line);
		}
		else if (line_t == ERROR)
		{
			printf("Bad line in file\n");
			free(line);

			struct Content content;
			content.sects = sects;
			content.num_sects = num_sects;
			clear_content(&content);

			exit(1);
			break;
		}

		free(line);
	}

	fclose(fp);
	struct Content content;
	content.sects = sects;
	content.num_sects = num_sects;
	return content;
}

// Return index of first dot in string
int find_dot(char ident[])
{
	int i = 0;
	while(ident[i] != '.' && ident[i] != '\0')
	{
		i++;
	}
	
	if(ident[i] == '\0')
		return -1;
	return i;
}

// Return the value for the given "section.key" in given Content
char* get_value(struct Content* cont, char ident[])
{
	int num_sects = cont->num_sects;
	int dot_ind = find_dot(ident);
	if(dot_ind == -1)
	{
		printf("Incorrect argument \"%s\"", ident);
		exit(1);
	}
	int ident_len = get_length(ident);
	bool correct_sect = false;


	int sect_ind = -1;
	char* sect_name = NULL;
	for(int i = 0; i < num_sects; i++)
	{
		sect_name = cont->sects[i].name;
		if(get_length(sect_name) != dot_ind)
			continue;
		
		correct_sect = true;
		for(int j = 0; j < dot_ind; j++)
		{
			if(ident[j] != sect_name[j])
			{
				correct_sect = false;
				break;
			}
		}

		if(correct_sect)
		{
			sect_ind = i;
			break;
		}
	}
	if(!correct_sect)
	{
		printf("Failed to find the section in %s\n", ident);
		return NULL;
	}


	int num_entries = cont->sects[sect_ind].size;
	bool correct_key = false;
	int entry_ind = -1;
	char* key = NULL;
	for(int i = 0; i < num_entries; i++)
	{
		key = cont->sects[sect_ind].entries[i].key;
		if(get_length(key) != (ident_len - dot_ind - 1))
			continue;

		correct_key = true;
		for(int j = dot_ind + 1; j < ident_len; j++)
		{
			if(ident[j] != key[j - dot_ind - 1])
			{
				correct_key = false;
				break;
			}
		}

		if(correct_key)
		{
			entry_ind = i;
			break;
		}
	}
	if(!correct_key)
	{
		printf("Failed to find the key in given section in \"%s\"\n", ident);
		return NULL;
	}


	char* value = cont->sects[sect_ind].entries[entry_ind].value;
	return value;

}

// Return "true" if string represents integer, "false" otherwise
bool is_number(char s[])
{
	int len = get_length(s);
	if (len < 1)
		return false;

	if (len == 1)
		return isdigit(s[0]);
	
	for(int i = 0; i < len; i++)
	{
		if(i == 0 && s[i] == '-')
			continue;
		
		if(!isdigit(s[i]))
			return false;
	}
	return true;
}

// Converts string to int and returns the int value
int str_to_int(char s[])
{
	int len = get_length(s);
	int i = 0;
	bool negative = false;
	if(s[0] == '-')
	{
		negative = true;
		i = 1;
	}
	
	int num = 0;
	for(; i < len; i++)
	{
		int digit = s[i] - '0';
		num = 10 * num + digit;
	}
	
	if(negative)
		num *= -1;

	return num;
}

// Return "true" if two given strings are equal, "false" otherwise
bool check_equal_str(char s1[], char s2[])
{
	int len1 = get_length(s1);
	int len2 = get_length(s2);
	if (len1 != len2)
		return false;
	
	for(int i = 0; i < len1; i++)
	{
		if(s1[i] != s2[i])
			return false;
	}
	return true;
}

// Returns pointer to new string, which is a substring [start, end) of given
// ALLOCATES MEMORY
char* get_substring(char s[], int start, int end)
{
	int res_len = end - start;
	char* res = (char*)malloc(sizeof(char) * (res_len + 1));
	
	for(int i = 0; i < res_len; i++)
	{
		res[i] = s[start + i];
	}
	res[res_len] = '\0';
	
	return res;
}

void run_expression(struct Content* cont, char expr[])
{
	int len = get_length(expr);
	char oper = '\0';
	int oper_ind = -1;
	for(int i = 0; i < len; i++)
	{
		if(expr[i] == ' ')
		{
			oper = expr[i + 1];
			oper_ind = i + 1;
			break;
		}
	}
	if(oper_ind == -1)
	{
		printf("Invalid expression\n");
		return;
	}

	char* ident1 = get_substring(expr, 0, oper_ind-1);
	char* ident2 = get_substring(expr, oper_ind + 2, len);

	char* val1 = get_value(cont, ident1);
	char* val2 = get_value(cont, ident2);

	free(ident1);
	free(ident2);

	if(val1 == NULL || val2 == NULL)
		return;
	
	bool first_int = is_number(val1);
	bool second_int = is_number(val2);
	
	if(first_int != second_int)
	{
		printf("Operands of different type. \"%s\" is invalid\n", expr);
		return;
	}

	if(first_int)
	{
		int num1 = str_to_int(val1);
		int num2 = str_to_int(val2);
		int res;
		switch(oper)
		{
		case '+':
			res = num1 + num2;
			break;
		case '-':
			res = num1 - num2;
			break;
		case '*':
			res = num1 * num2;
			break;
		case '/':
			res = num1 / num2;
			break;
		default:
			printf("Unknown operator '%c'\n", oper);
			return;
			break;
		}
		printf("%d\n", res);
	}
	else
	{
		if(oper != '+')
		{
			printf("Invalid or unknown operator '%c' for strings\n", oper);
		}

		printf("%s%s\n", val1, val2);
	}
}

void clear_content(struct Content* cont)
{
	int num_sects = cont->num_sects;
	for(int i = 0; i < num_sects; i++)
	{
		int num_entries = cont->sects[i].size;
		for(int j = 0; j < num_entries; j++)
		{
			free(cont->sects[i].entries[j].key);
			free(cont->sects[i].entries[j].value);
		}
		free(cont->sects[i].name);
		free(cont->sects[i].entries);
	}
	free(cont->sects);
}