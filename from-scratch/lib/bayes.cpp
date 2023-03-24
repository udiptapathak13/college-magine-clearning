#ifndef _BAYES_CPP
#define _BAYES_CPP

#include "dset.cpp"
#include <unordered_map>

extern "C" {

typedef struct {
	uint8_t tSize;
	double *tClass;
	uint8_t col;
	std::unordered_map<std::string, uint64_t> *freq;	
} Bayes;

std::string hpr(Dset *ds, uint64_t r, uint8_t c)
{
	double A = *(double *) gData(ds, r, c);
	double B = *(double *) gData(ds, r, ds->clen);
	return std::to_string(A) + "," + std::to_string(B);
}

Bayes *trainBayes(Dset *ds)
{
	Bayes *ret = (Bayes *) malloc(sizeof(Bayes));
	ret->col = ds->clen - 1;
	uint8_t col = ret->col;
	std::unordered_set<double> st;
	void ***data = ds->df->data;
	for (uint64_t i = 0, n = ds->rlen ; i < n ; i++)
		st.insert(*(double *) data[i][col]);
	ret->tSize = st.size();
	ret->tClass = (double *) malloc(ret->tSize * sizeof(double));
	ret->freq = new std::unordered_map<std::string, uint64_t>[col]();
	uint8_t cnt = 0;
	for (int i : st) ret->tClass[cnt++] = i;
	for (uint64_t i = 0, row = ds->df->row ; i < row ; i++)
		for (uint8_t j = 0 ; j < col ; j++)
			ret->freq[j][hpr(ds, i, j)]++;
	return ret;
}

std::string hpr2(void *a, double B)
{
	double A = *(double *) a;
	return std::to_string(A) + "," + std::to_string(B);
}

uint64_t getBayesFreq(Dset *ds, Bayes *bs, uint64_t r, uint8_t c) {
	uint64_t res = 1;
	void ***data = ds->df->data;
	for (uint8_t i = 0, n = bs->col ; i < n ; i++)
		res *= bs->freq[i][hpr2(data[ds->rlist[r]][i], bs->tClass[c])];
	return res;
}

double *classifyBayes(Bayes *bs, Dset *ds) {
	double *res = (double *) malloc(ds->rlen * sizeof(double));
	uint64_t mx, tmp, mxc;
	uint8_t tSize = bs->tSize;
	for (uint64_t i = 0, row = ds->rlen ; i < row ; i++) {
		mx = 0; mxc = -1;
		for (int j = 0 ; j < tSize ; i++) {
			tmp = getBayesFreq(ds, bs, i, j);
			if (tmp > mx) {
				mx = tmp;
				mxc = j;
			}
		}
		res[i] = mxc;
	}
	return res;
}

}

#endif
