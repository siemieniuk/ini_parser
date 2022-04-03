#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
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


struct Section* create_section(char line[])
{
	int len = get_length(line);
	char* name = (char*)malloc(sizeof(char) * (len - 1));
	if (!name)
		return NULL;

	int line_ind = 1, name_ind = 0;
	while (line_ind <= len - 2)
	{
		name[name_ind] = line[line_ind];
		name_ind++;
		line_ind++;
	}
	name[name_ind] = '\0';

	struct Section* sect = (struct Section*)malloc(sizeof(struct Section));
	if (!sect)
		return sect;
	sect->name = name;
	return sect;
}

// TODO: finish parse_ini_file (parse_ini_file is not yet functional)
void parse_ini_file(char path[])
{
	FILE* fp = fopen(path, "r");

	char* line;
	struct Section* sect;
	struct Entry* entry;
	while (!feof(fp))
	{
		line = read_line(fp);

		enum Line_type line_t = classify_line(line);

		if (line_t == SECTION)
		{
			sect = create_section(line);
			printf(sect->name);
		}
		// TODO for other line_types

		free(line);
	}

	fclose(fp);
	return;
}
