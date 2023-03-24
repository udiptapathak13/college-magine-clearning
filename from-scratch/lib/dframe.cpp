#ifndef _DFRAME_CPP
#define _DFRAME_CPP

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string>

char *to_cstr(const std::string &str)
{
	uint8_t n = str.length();
	char *ret = (char *) malloc((n + 1) * sizeof(char));
	strncpy(ret, str.c_str(), n);
	ret[n] = 0;
	return ret;
}

bool isDigit(char ch)
{
	return '0' <= ch && ch <= '9';
}

extern "C" {

typedef struct {
	void ***data;
	uint8_t col;
	uint64_t row;
	char **fname;
	bool *dtypes;
} Dframe;

void *getNum(FILE *fp)
{
	char ch;
	std::string s;
	while ((ch = fgetc(fp)) != ',' && ch != 0x0d)
		s += ch;
	double *ret = (double *) malloc(sizeof(double));
	*ret = strtod(s.c_str(), NULL);
	return ret;
}

void *getObj(FILE *fp)
{
	char ch;
	std::string s;
	while ((ch = fgetc(fp)) != ',' && ch != 0x0d)
		s += ch;
	int n = s.length();
	char *ret = (char *) malloc((n + 1) * sizeof(char));
	for (int i = 0 ; i < n ; i++)
		ret[i] = tolower(s[i]);
	ret[n] = 0;
	return ret;
	
}

Dframe *readCSV(const char* fname)
{
	FILE *fp = fopen(fname, "r");
	uint8_t row;
	uint64_t col;
	if (!fp) {
		fprintf(stderr, "Failed to open the CSV file \'%s\'\x0a", 
									fname);
		exit('1');
	}
	Dframe *ret = (Dframe *) malloc(sizeof(Dframe));
	char ch;
	uint16_t cnt = 1;
	while ((ch = getc(fp)) != 0x0a)
		if (ch == ',') cnt++;
	col = ret->col = cnt;
	cnt = 0;
	while ((ch = getc(fp)) != EOF)
		if (ch == 0x0a) cnt++;
	row = ret->row = cnt;
	ret->data = (void ***) malloc(row * sizeof(void **));
	for (int i = 0; i < row ; i++)
		ret->data[i] = (void **) malloc(col * sizeof(void *));
	ret->fname = (char **) malloc(col * sizeof(char *));
	rewind(fp);
	std::string s;
	cnt = 0;
	while ((ch = getc(fp)) != 0x0d) {
		if (ch == ',') {
			ret->fname[cnt++] = to_cstr(s);
			s = "";
			continue;
		}
		s += ch;
	}
	getc(fp);
	ret->fname[cnt] = to_cstr(s);
	ret->dtypes = (bool *) malloc(col * sizeof(bool));
	uint16_t offset = ftell(fp);
	cnt = 0;
	ret->dtypes[cnt++] = isDigit(fgetc(fp));
	while ((ch = fgetc(fp)) != 0x0d)
		if (ch == ',') ret->dtypes[cnt++] = isDigit(fgetc(fp));
	fseek(fp, offset, SEEK_SET);
	bool *dtypes = ret->dtypes;
	void ***data = ret->data;
	for (uint64_t i = 0 ; i < row ; i++)
		for (uint8_t j = 0 ; j < col ; j++)
			data[i][j] = dtypes[j] ? getNum(fp) : getObj(fp);
	
	return ret;
}

}

#endif
