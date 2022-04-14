#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "utility.h"

bool check_legal_char(char ch)
{
	if (isalnum(ch) || ch == '-')
		return true;
	return false;
}

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
			fclose(fp);
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

char* get_value(struct Content* cont, char ident[])
{
	int num_sects = cont->num_sects;
	int dot_ind = find_dot(ident);
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
		printf("Failed to find section [%s]\n", sect_name);
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
		printf("Failed to find key \"%s\" in section [%s]\n", key, sect_name);
		return NULL;
	}


	char* value = cont->sects[sect_ind].entries[entry_ind].value;
	return value;

}

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