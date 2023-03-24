#ifndef _LABELENC_CPP
#define _LABELENC_CPP

#include "dframe.cpp"
#include <iostream>
#include <unordered_set>
#include <unordered_map>

extern "C" {

typedef struct {
	char **decode;
} LabelEncoder;

LabelEncoder *labelEncoder(Dframe *df, const char *cname)
{
	uint16_t cno = 0, row = df->row, col = df->col;
	void ***data = df->data;
	int flen = strlen(cname);
	while (cno < col && strncmp(df->fname[cno], cname, flen)) cno++;
	if (cno == col) {
		fprintf(stderr, "%s is not a field in the given dataframe",
									cname);
		exit(1);
	}
	if (df->dtypes[cno]) {
		fprintf(stderr, "%s is of numerical type and need not be "
							"endcoded\x0a", cname);
		exit(1);
	}
	LabelEncoder *ret = (LabelEncoder *) malloc(sizeof(LabelEncoder));
	std::unordered_set<std::string> hst;
	for (uint64_t i = 0 ; i < row ; i++)
		hst.insert(std::string((char *) data[i][cno]));
	uint64_t n = hst.size();
	uint64_t cnt = 0;
	ret->decode = (char **) malloc(n * sizeof(char *));
	char **decode = ret->decode;
	std::unordered_map<std::string, double> hmp;
	for (auto it = hst.begin(); it != hst.end() ; it++) {
		decode[cnt] = to_cstr(*it);
		hmp[*it] = (float) cnt++;
	}
	df->dtypes[cno] = true;
	double *tmp;
	for (uint64_t i = 0 ; i < row ; i++ ) {
		tmp = (double *) malloc(sizeof(double));
		*tmp = hmp[std::string((char *) data[i][cno])];	
		free(data[i][cno]);
		data[i][col] = tmp;
	}
	return ret;
}

}

#endif

