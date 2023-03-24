#ifndef _DSET_CPP
#define _DSET_CPP

#include "dframe.cpp"
#include <unordered_set>

extern "C" {

typedef struct {
	Dframe *df;
	uint8_t clen;
	uint8_t *clist;
	uint64_t rlen;
	uint64_t* rlist;
} Dset;

Dset *dataSetfromFrame(Dframe *df)
{
	Dset *ret = (Dset *) malloc(sizeof(Dset));
	ret->df = df;
	ret->clen = df->col;
	ret->rlen = df->row;
	uint8_t *clist = ret->clist;
	clist = (uint8_t *) malloc(df->col * sizeof(uint8_t));
	for (int i = 0, n = df->col ; i < n ; i++)
		clist[i] = i;
	uint64_t *rlist = ret->rlist;
	rlist = (uint64_t *) malloc(df->row * sizeof(uint64_t));
	for (int i = 0, n = df->row ; i < n ; i++)
		rlist[i] = i;
	return ret;
}

void *gData(Dset *ds, uint64_t row, uint8_t col) {
	return ds->df->data[ds->rlist[row]][ds->clist[col]];
}

}

#endif
