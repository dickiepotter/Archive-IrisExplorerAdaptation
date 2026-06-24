/*
From pharnden@nag.com  Thu May 15 17:38:26 1997
Date: Thu, 15 May 97 11:36 CDT
From: pharnden@nag.com (Phillip Harnden)
To: alang@nag.co.uk, davidk@nag.co.uk, jeremyw@nag.co.uk
Subject: PyrToGeom

hey...

the truecolor.C file included below allows PyrToGeom to color
as the help file suggests...

no more yellow (for positive float/double) and no more blue
(for negative float/double) for single and double data channels...

grey for one channel, red and cyan for two, and rgb for three...
after three, you get rgb with whatever's in the fourth channel as opacity...

phil

----- begin truecolor.C -----
*/

#include <cx/DataTypes.h>
#define SMALL 0.000001
#include <math.h>
#include <assert.h>
#include <cx/Geometry.h>

#include <float.h>
#include <cx/DataAccess.h>

#define max(x,y) ((x)>=(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))

#include "all_func.h"

void truecolor(cxLattice *lat, long *permVec, long count )
{
	int	i;		/* loop variables */
	int hasAlpha,unique;

/* local variables to handle float and double precision */
long j, k;
long nDV;
long ndata;
long chan2, chan3, chan4;
float flt_offset;
float min_flt;
float max_flt;
float *flt_data;
double dbl_offset;
double min_dbl;
double max_dbl;
double *dbl_data;

	/* remember the number of samples */
	int nsamples;
	if (permVec)
	  nsamples = count;
	else
	  nsamples = (int)(lat->dims[0]);

	/* allocate color array */
	float *col = new float[3*nsamples];
	float *cptr = col;
	hasAlpha = lat->data->nDataVar > 3;
	float *acol = new float[nsamples];
	float *aptr = acol;

#define LOOP(TYPE,RED,GRN,BLU,SCALE) \
	{ \
        if (permVec) {		 \
	  for ( i = 0; i < nsamples; i++ ) { \
	        TYPE *data = permVec[i] * lat->data->nDataVar + (TYPE *)ptr; \
		cptr[0] = data[RED] SCALE; \
		cptr[1] = data[GRN] SCALE; \
		cptr[2] = data[BLU] SCALE; \
		if(hasAlpha){ *aptr++ = 1.0 - data[3] SCALE;} \
		cptr += 3; \
	  } \
	} \
	else {\
	  TYPE *data = (TYPE *)ptr; \
	  for ( i = 0; i < nsamples; i++ ) { \
		cptr[0] = data[RED] SCALE; \
		cptr[1] = data[GRN] SCALE; \
		cptr[2] = data[BLU] SCALE; \
		if(hasAlpha){ *aptr++ = 1.0 - data[3] SCALE;} \
		cptr += 3; \
		data += lat->data->nDataVar; \
	  } \
	} \
	}

	void *ptr;
	switch(lat->data->primType)
	{
		case cx_prim_byte:
			ptr = (void *)lat->data->d.cx_prim_byte.values;
			switch(lat->data->nDataVar)
			{
				case 1:
					LOOP(unsigned char,0,0,0,/255.0)
					break;
				case 2:
					LOOP(unsigned char,0,1,1,/255.0)
					break;
				default:
					LOOP(unsigned char,0,1,2,/255.0)
					break;
			}
			break;
		case cx_prim_short:
			ptr = (void *)lat->data->d.cx_prim_short.values;
			switch(lat->data->nDataVar)
			{
				case 1:
					LOOP(unsigned short,0,0,0,/65535.0)
					break;
				case 2:
					LOOP(unsigned short,0,1,1,/65535.0)
					break;
				default:
					LOOP(unsigned short,0,1,2,/65535.0)
					break;
			}
			break;
		case cx_prim_long:
			ptr = (void *)lat->data->d.cx_prim_long.values;
			switch(lat->data->nDataVar)
			{
				case 1:
					LOOP(signed long,0,0,0,/2147483647.0 + 0.5)
					break;
				case 2:
					LOOP(signed long,0,1,1,/2147483647.0 + 0.5)
					break;
				default:
					LOOP(signed long,0,1,2,/2147483647.0 + 0.5)
					break;
			}
			break;
		case cx_prim_float:

cxLatDescGet(lat,NULL,NULL,NULL,&nDV,NULL,NULL,NULL,NULL);
ndata = (long)nsamples*nDV;

flt_offset = 0.0;
min_flt = FLT_MAX;
cxLatPtrGet(lat,NULL,(void **)&flt_data,NULL,NULL);
for (j=0;j<ndata;j++) min_flt = min(min_flt,flt_data[j]);
max_flt = min_flt;
for (j=0;j<ndata;j++) max_flt = max(max_flt,flt_data[j]);
if (min_flt < 0)
flt_offset = (-min_flt);
else flt_offset -= min_flt;
max_flt += flt_offset;

if (nDV == 1) { chan2 = 0; chan3 = 0; }
if (nDV == 2) { chan2 = 1; chan3 = 1; }
if (nDV == 3) { chan2 = 1; chan3 = 2; }
if (nDV > 3 ) { chan2 = 1; chan3 = 2; chan4 = 3; }

for (j=k=0;j<(ndata/nDV);j++,k+=3) {
col[k] = (flt_data[j]+flt_offset)/max_flt;
col[k+1] = (flt_data[j+chan2]+flt_offset)/max_flt;
col[k+2] = (flt_data[j+chan3]+flt_offset)/max_flt;
if(hasAlpha){ acol[j] = 1.0 - (flt_data[j+chan4]+flt_offset)/max_flt; }
}

			break;
		case cx_prim_double:

cxLatDescGet(lat,NULL,NULL,NULL,&nDV,NULL,NULL,NULL,NULL);
ndata = (long)nsamples*nDV;

dbl_offset = 0.0;
min_dbl = DBL_MAX;
cxLatPtrGet(lat,NULL,(void **)&dbl_data,NULL,NULL);
for (j=0;j<ndata;j++) min_dbl = min(min_dbl,dbl_data[j]);
max_dbl = min_dbl;
for (j=0;j<ndata;j++) max_dbl = max(max_dbl,dbl_data[j]);
if (min_dbl < 0)
dbl_offset = (-min_dbl);
else dbl_offset -= min_dbl;
max_dbl += dbl_offset;

if (nDV == 1) { chan2 = 0; chan3 = 0; }
if (nDV == 2) { chan2 = 1; chan3 = 1; }
if (nDV == 3) { chan2 = 1; chan3 = 2; }
if (nDV > 3 ) { chan2 = 1; chan3 = 2; chan4 = 3; }

for (j=k=0;j<(ndata/nDV);j++,k+=3) {
col[k] = (dbl_data[j]+dbl_offset)/max_dbl;
col[k+1] = (dbl_data[j+chan2]+dbl_offset)/max_dbl;
col[k+2] = (dbl_data[j+chan3]+dbl_offset)/max_dbl;
if(hasAlpha){ acol[j] = 1.0 - (dbl_data[j+chan4]+dbl_offset)/max_dbl; }
}
			break;
	default:
	  assert(0);
	}


	/* add the colors to the scene graph */
	cxGeoColorAdd(nsamples,col,CX_GEO_PER_VERTEX_INDEXED);

	if(hasAlpha){
	  unique = TRUE;
	  for(i=1;i<nsamples;i++){
	    if(fabs(acol[i] - acol[i-1]) > SMALL)
	      unique = FALSE;
	  }
	  if(unique) {
	    if(acol[0] != 0.0)
	    cxGeoTransparencyAdd(1,acol,CX_GEO_PER_OBJECT);
	  }else{ 
	    cxGeoTransparencyAdd(nsamples,acol,CX_GEO_PER_VERTEX_INDEXED);
	  }
	}

	/* reclaim memory */
	delete [] col;
	delete [] acol;
}

