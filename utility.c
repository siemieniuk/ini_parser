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

// Returns an empty section with the name, extraxcted from given line
// ALLOCATES MEMORY
struct Section create_section(char line[])
{
	int len = strlen(line);
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
	int len = strlen(line);
	int eq_ind = 0;
	for (eq_ind = 0; eq_ind < len; eq_ind++)
	{
		if (line[eq_ind] == '=')
			break;
	}


	char* key = (char*)malloc(sizeof(char) * (eq_ind));
	char* val = (char*)malloc(sizeof(char) * (len - eq_ind-1));
	
	// Copying part of line into key
	int curr_ind = 0;
	for (; curr_ind < eq_ind-1; curr_ind++)
	{
		key[curr_ind] = line[curr_ind];
	}
	key[curr_ind] = '\0';
	
	// Copyting part of line into val
	curr_ind = 0;
	int i = eq_ind + 2;
	for (; i < len; i++)
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
    if (!fp) {
        printf("File \"%s\" not found", path);
        exit(1);
    }

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
			printf("Incorrect identifier: \"%s\"\n", line);
			free(line);

			struct Content content;
			content.sects = sects;
			content.num_sects = num_sects;
			clear_content(&content);
			fclose(fp);
			exit(1);
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

char* get_section_name(char ident[]) {
	const int size = find_dot(ident);
	char* res = malloc(sizeof(char)*(size+1));
	int i = 0;
	while (ident[i] != '.') {
		res[i] = ident[i];
		i++;
	}
	res[size] = '\0';
	return res;
}

char* get_key_name(char ident[]) {
	const int dot_pos = find_dot(ident)+1;
	const int size = strlen(ident)-dot_pos;
	char* res = malloc(sizeof(char)*(size+1));
	int i = 0;
	while (ident[dot_pos+i] != '\0') {
		res[i] = ident[dot_pos+i];
		i++;
	}
	res[size] = '\0';
	return res;
}

// Return the value for the given "section.key" in given Content
char* get_value(struct Content* cont, char ident[])
{
	const int dot_ind = find_dot(ident);
	if (dot_ind == -1)
	{
		printf("Incorrect identifier \"%s\"", ident);
		return NULL;
	}
	const int num_sects = cont->num_sects;
	const int ident_len = strlen(ident);
	bool is_section_correct = false;


	int sect_ind = -1;
	char* sect_name = NULL;
	for (int i = 0; i < num_sects; i++)
	{
		sect_name = cont->sects[i].name;
		if (strlen(sect_name) != (long unsigned int)dot_ind)
			continue;
		
		is_section_correct = true;
		for (int j = 0; j < dot_ind; j++)
		{
			if (ident[j] != sect_name[j])
			{
				is_section_correct = false;
				break;
			}
		}

		if (is_section_correct)
		{
			sect_ind = i;
			break;
		}
	}
	if (!is_section_correct)
	{
		char* section_name = get_section_name(ident);
		printf("Failed to find section [%s]\n", section_name);
		free(section_name);
		return NULL;
	}


	int num_entries = cont->sects[sect_ind].size;
	bool correct_key = false;
	int entry_ind = -1;
	char* key = NULL;
	for (int i = 0; i < num_entries; i++)
	{
		key = cont->sects[sect_ind].entries[i].key;
		if (strlen(key) != (long unsigned int)(ident_len - dot_ind - 1))
			continue;

		correct_key = true;
		for (int j = dot_ind + 1; j < ident_len; j++)
		{
			if(ident[j] != key[j - dot_ind - 1])
			{
				correct_key = false;
				break;
			}
		}

		if (correct_key)
		{
			entry_ind = i;
			break;
		}
	}
	if (!correct_key)
	{
		char* section_name = get_section_name(ident);
		char* key_name = get_key_name(ident);
		printf("Failed to find key \"%s\" in section [%s]\n", key_name, section_name);
		free(section_name);
		free(key_name);
		return NULL;
	}


	char* value = cont->sects[sect_ind].entries[entry_ind].value;
	return value;

}

// Return "true" if string represents integer, "false" otherwise
bool is_number(char s[])
{
	int len = strlen(s);
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
	const int expr_len = strlen(expr);
	char oper = '\0';
	int oper_ind = -1;
	int whitespace_ctr = 0;
	for (int i = 0; i < expr_len; i++)
	{
		if (expr[i] == ' ')
		{
			if (oper_ind == -1) {
				oper = expr[i + 1];
				oper_ind = i + 1;
			}
			whitespace_ctr++;
		}
	}
	if (oper_ind == -1 || whitespace_ctr != 2 || expr[oper_ind+1] != ' ')
	{
		printf("Invalid expression: \"%s\"\n", expr);
		return;
	}

	char* ident1 = get_substring(expr, 0, oper_ind-1);
	char* ident2 = get_substring(expr, oper_ind + 2, expr_len);

	char* val1 = get_value(cont, ident1);
	char* val2 = get_value(cont, ident2);

	free(ident1);
	free(ident2);

	if(val1 == NULL || val2 == NULL)
		return;
	
	bool first_int = is_number(val1);
	bool second_int = is_number(val2);
	
	if (first_int != second_int)
	{
		printf("Operands of different type. \"%s\" is invalid\n", expr);
		return;
	}

	if (first_int)
	{
		long num1 = atoi(val1);
		long num2 = atoi(val2);
		long res;
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
		printf("%ld\n", res);
	}
	else
	{
		if (oper != '+')
		{
			printf("Invalid or unknown operator '%c' for strings\n", oper);
			return;
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