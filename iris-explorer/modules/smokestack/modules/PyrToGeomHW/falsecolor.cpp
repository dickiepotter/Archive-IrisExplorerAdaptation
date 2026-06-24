#include <stdio.h>
#include <limits.h>
#include <assert.h>
#define SMALL 0.000001
#include <cx/DataTypes.h>

#include <cx/Geometry.h>
#include <cx/Lookup.h>
#include <math.h>

#include "all_func.h"

void falsecolor(cxData *cdata,cxLattice *cmap, long ivar, 
		long *permVec, long level, long count)
{
	int	i,j;			/* loop variables */
	int unique;
	/* initialize lookup table */
	cxLookup *clut = cxLookupCreate(cmap,cx_lookup_linear);

	/* remember the number of samples */
	int nsamples;
	if (permVec)
	  nsamples = count;
	else
	  nsamples = (int)(cdata->dims[0]);

	long nDataVar = cdata->nDataVar;

	/* allocate color array */
	float *col = new float[3*nsamples];
	float *cptr = col;

	float *acol = new float[nsamples];
	float *aptr = acol;
	
	float *lut;
	float *fInd;

	switch(cdata->primType)
	{
		case cx_prim_byte:
			{
			/* set up the lookup table */
			lut = new float[4*256];

			/* fill up index array */
			for ( i = 0; i < 256; i++ )
			{
				float ind = i;
				cxLookupInterp(clut,&ind,(void *)&lut[4*i]);
			}
			
			/* set the colors */
			unsigned char *data = (unsigned char *)cdata->d.cx_prim_byte.values;
			data += ivar-1;
			if (permVec) {
			  for ( i = 0; i < nsamples; i++ )
			  {
				float *ptr = lut + 4*data[permVec[i]*nDataVar];
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*aptr++ = 1.0 - *ptr++;
			  }
		        }
			else
			  for ( i = 0; i < nsamples; i++ )
			  {
				float *ptr = lut + 4**data;
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*aptr++ = 1.0 - *ptr++; 
				data += nDataVar;
			  }

			/* reclaim memory */
			delete [] lut;
			}
			break;
		case cx_prim_short: {
			/* set up the lookup table, offset to account for sign*/
#ifndef _CRAY
			const	int	shortRange = SHRT_MAX-SHRT_MIN+1;
			const	int	shortMin = SHRT_MIN;
			const	int	shortMax = SHRT_MAX;
#else /*_CRAY - restrict range to a 16-bit short*/
			const	int	shortMin = -32768;
			const	int	shortMax = 32767;
			const	int	shortRange = shortMax-shortMin+1;
#endif /*_CRAY*/
			const	int	zeroOffset = -shortMin;
			lut = new float[4*shortRange];
			float	*realLut = lut;
			lut += 4*zeroOffset;

			/* fill up index array */
			for ( i = shortMin; i < shortMax; i++ )
			{
				float ind = i;
				cxLookupInterp(clut,&ind,(void *)&lut[4*i]);
			}

			/* set the colors */
			short *sdata = (short *)cdata->d.cx_prim_short.values;
			sdata += ivar-1;
			if (permVec) {
			  for ( i = 0; i < nsamples; i++ )
			  {
				float *ptr = lut + 4*sdata[permVec[i]*nDataVar];
				assert( ptr >= realLut );
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*aptr++ = 1.0 - *ptr++; 
			  }
		        }
			else
			  for ( i = 0; i < nsamples; i++ )
			  {
				float *ptr = lut + 4**sdata;
				assert( ptr >= realLut );
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*cptr++ = *ptr++;
				*aptr++ = 1.0 - *ptr++; 
				sdata += nDataVar;
			  }

			/* reclaim memory */
			delete [] realLut;
			}
			break;
		case cx_prim_long:
			{
			long	*lind = cdata->d.cx_prim_long.values;
			lind += ivar-1;
			lut = new float[4];

			if (permVec) {
			  for( i=0; i<nsamples; i++ ) {
				float ind = lind[permVec[i]*nDataVar];
				cxLookupInterp(clut,&ind,lut);
				*cptr++ = lut[0];
				*cptr++ = lut[1];
				*cptr++ = lut[2];
				*aptr++ = 1.0 - lut[3];
			  }
			}
			else {
				for( i=0; i<nsamples; i++ ) {
					float ind = lind[0];
					cxLookupInterp(clut,&ind,lut);
					*cptr++ = lut[0];
					*cptr++ = lut[1];
					*cptr++ = lut[2];
					*aptr++ = 1.0 - lut[3];
					
					lind += nDataVar;
				}
			}

			delete [] lut;
			}
			break;
		case cx_prim_float:
			/* set up buffers */
			lut = new float[4];
			fInd = cdata->d.cx_prim_float.values;
			fInd += ivar-1;
			if (permVec) {
			  for ( j = 0; j < nsamples; j++ )
			  {
				cxLookupInterp(clut,
					       fInd+permVec[j]*nDataVar,
					       (void *)lut);
				*cptr++ = lut[0];
				*cptr++ = lut[1];
				*cptr++ = lut[2];
				*aptr++ = 1.0 -  lut[3];
			  }
			}
			else 
			  for ( j = 0; j < cdata->dims[0]; j++ )
			  {
				cxLookupInterp(clut,fInd,(void *)lut);
				*cptr++ = lut[0];
				*cptr++ = lut[1];
				*cptr++ = lut[2];
				*aptr++ = 1.0 - lut[3];
				fInd += nDataVar;
			  }
			/* reclaim memory */
			delete [] lut;
			break;
		case cx_prim_double:
			{
			double	*dInd = cdata->d.cx_prim_double.values;
			dInd += ivar-1;
			lut = new float[4];

			if (permVec) {
			  for( i=0; i<nsamples; i++ ) {
			        // copy & convert
				float ind = dInd[permVec[i]*nDataVar];

				cxLookupInterp(clut,&ind,lut);
				*cptr++ = lut[0];
				*cptr++ = lut[1];
				*cptr++ = lut[2];
				*aptr++ = 1. - lut[3]; 
			  }
			}
			else
			  for( i=0; i<nsamples; i++ ) {
				float ind = dInd[0];   // copy & convert

				cxLookupInterp(clut,&ind,lut);
				*cptr++ = lut[0];
				*cptr++ = lut[1];
				*cptr++ = lut[2];
				*aptr++ = 1.0 - lut[3]; 
				dInd += nDataVar;
			  }

			delete [] lut;
			}
			break;
	default:
	  assert(0);
	  break;
	}


	/* add the colors to the scene graph.  If we're using the
	   data values at the vertices to colour the geometry, 
	   then select PER_VERTEX_INDEXED binding - this generates
	   shaded lines and polygons and is the old behaviour of 
	   this module.  Otherwise, select PER_PART binding - i.e. 
	   per face (for a face set) or per line (for a line set) 
	   or per vertex (for a point set), etc */
	if( level == 0 )
	{
	    cxGeoColorAdd(nsamples, col, CX_GEO_PER_VERTEX_INDEXED);
	}
	else
	{
	    cxGeoColorAdd(nsamples, col, CX_GEO_PER_PART);
	}
	
// Find out if all the transparency values are roughly the same.

	unique = TRUE;
	i = 1;
	while( i<nsamples && unique==TRUE ) {
	  if(fabs(acol[i] - acol[i-1]) > SMALL) unique = FALSE;
          i++;
	}

// If they are, and if the first value is close to zero, don't do 
// anything (leave the transparency set at zero).  Otherwise, set
// the transparencies explicitly.  Note that we need to do this 
// even if they're all the same (and non-zero) because we can't
// apply transparency to the whole object without breaking the 
// binding of the colours, which we've already set to be 
// PER_PART.

	if (!unique || acol[0] > SMALL)
	{
		if (level==0)
	 		cxGeoTransparencyAdd(nsamples, acol, CX_GEO_PER_VERTEX_INDEXED);
	 	else
	   	cxGeoTransparencyAdd(nsamples, acol, CX_GEO_PER_PART);
	}
		
	/* reclaim memory */
	delete [] col;
	delete [] acol;
	cxLookupDestroy(clut);
}


