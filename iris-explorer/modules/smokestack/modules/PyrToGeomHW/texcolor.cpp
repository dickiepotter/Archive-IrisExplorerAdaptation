#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <float.h> // For FLT_MAX etc.

#include <cx/DataTypes.h>
#include <cx/DataAccess.h>
#include <cx/Geometry.h>

#include "colmaps.h"

#if 0 //defined(__osf__) 
#define REDBYTE   3
#define GREENBYTE 2
#define BLUEBYTE  1
#define ALPBYTE   0
#else
#define REDBYTE   0
#define GREENBYTE 1
#define BLUEBYTE  2
#define ALPBYTE   3
#endif

static float crange[2];

#ifndef MIN
#define MIN( a, b )      (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX( a, b )      (((a) > (b)) ? (a) : (b))
#endif

#include "all_func.h"

// Add texturing
// Adapted from LatToGeom version for PyrToGeom

void tcolor(cxData *cdata, cxLattice *cmap, long channel, long ncols, long *permVec, long count)
{
	int i, ip;
	long nsamples, numPoints;

	long chan = channel-1;

	/* Use as many points in colour map as requested */
	nsamples = ncols;

	/* Number of sample points */
	if (permVec)
		numPoints = count;
	else
		numPoints = (int)(cdata->dims[0]);

	/* Number of data variables in the lattice data */
	long nDataVar = cdata->nDataVar;

                 
	/* Allocate color array and pointer */
	unsigned int *col = new unsigned int[nsamples];

	/* Check if there is a colour map and make one up if need be */
	if (cmap == NULL)
	{
		switch (nsamples)
		{
		case 2:
			col[0] = col2[0];
	  		col[1] = col2[1];
			break;
		case 4:
			for (i=0; i<nsamples; i++) col[i] = col4[i];
			break;
		case 16:
			for (i=0; i<nsamples; i++) col[i] = col16[i];
			break;		
		case 32:
			for (i=0; i<nsamples; i++) col[i] = col32[i];
			break;
		case 64:
			for (i=0; i<nsamples; i++) col[i] = col64[i];
			break;
		default:
			printf("Wrong number of colour samples\n");
			return;
		}
			
		/* If the colours are made up, use a fixed range */
		crange[0] = 0.0;
		crange[1] = 1.0;
	}
	else
	{
		cxErrorCode err;
		float *dataVals;
		long *dims;

		unsigned char *cptr = (unsigned char *) col;

		err = cxLatDescGet(cmap, NULL, &dims, NULL, NULL, NULL, NULL, NULL, NULL);

		/* step is the interval for jumping through the colourmap.  
		  If it contains dims[0] entries and the first index is 0,
		  then the last one is dims[0]-1.  */

		float step = float(dims[0]-1) / float(nsamples-1);
		cxDataFree((void *) dims);

		err = cxLatPtrGet(cmap, NULL, (void **) &dataVals, NULL, NULL);

		/* Loop over colour map lattice */
		for (i = 0; i < nsamples; i++)
		{
			long ind = long(float(i) * step);

			/* Initialise all bytes in unsigned long */ 
			col[i] = 0x0;

			/* Indexing directly into the colour lattice - its data values
    			 are red, green, blue, alpha.  */

			cptr[REDBYTE]   = (unsigned char) (255.0 * dataVals[4*ind]);
			cptr[GREENBYTE] = (unsigned char) (255.0 * dataVals[4*ind+1]);
			cptr[BLUEBYTE]  = (unsigned char) (255.0 * dataVals[4*ind+2]);
			cptr[ALPBYTE]   = (unsigned char) (255.0 * dataVals[4*ind+3]);
			cptr += sizeof(int);
		}

		/* Get the range of the colourmap and save it for use later on.  */
		cxCoord *coords;
		float *cVal;
		cxLatPtrGet(cmap, NULL, NULL, &coords, NULL);
		switch(coords->coordType)
		{
		/* We're assuming nDim = nCoordVar = 1 here for the colourmap
		* lattice - must ensure this is part of the module data
		* wrapper
		*/
		case cx_coord_uniform:
			cVal = coords->c.cx_coord_uniform.bBox;
			crange[0] = cVal[0];
			crange[1] = cVal[1];
			break;
		case cx_coord_perimeter:
			cVal = coords->c.cx_coord_perimeter.perimCoord;
			crange[0] = cVal[0];
			crange[1] = cVal[coords->dims[0]-1];
			break;
		case cx_coord_curvilinear:
			cVal = coords->c.cx_coord_curvilinear.values;
			crange[0] =  FLT_MAX;
			crange[1] = -FLT_MAX;
			for (int i = 0; i < coords->dims[0] - 1; i++)
			{
			  crange[0] = MIN(crange[0], cVal[i]);
			  crange[1] = MAX(crange[1], cVal[i]);		    
			}
			break;
		}
    }

	// Define the TextureCoordinate2 array from sampled data values
	float *values = new float[2*numPoints];

	// Get the data values and store them
	int np;
	np = nDataVar * numPoints;
	switch(cdata->primType)
	{
	case cx_prim_byte:
	{
		unsigned char *bdata = new unsigned char[np];
		if (permVec)
		{
			for (i = 0; i < numPoints * nDataVar; i++) bdata[i] = cdata->d.cx_prim_byte.values[permVec[i]];
			for (i = 0; i < numPoints; i++)
			{
				ip = permVec[i]*nDataVar;
				values[2*i] = (float) (bdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		else
		{
			for (i = 0; i < numPoints * nDataVar; i++) bdata[i] = cdata->d.cx_prim_byte.values[i];
			for (i = 0; i < numPoints; i++)
			{
				ip = i*nDataVar;
				values[2*i] = (float) (bdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
 		delete [] bdata;
		break;
	}	
	case cx_prim_short:
	{
		short *sdata = new short[np];
		if (permVec) 
		{
			for (i = 0; i < numPoints * nDataVar; i++) sdata[i] = cdata->d.cx_prim_short.values[permVec[i]];
			for (i = 0; i < numPoints; i++)
			{
				ip = permVec[i]*nDataVar;
				values[2*i] = (float) (sdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0)values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		else
		{
			for (i = 0; i < numPoints * nDataVar; i++) sdata[i] = cdata->d.cx_prim_short.values[i];
			for (i = 0; i < numPoints; i++)
			{
				ip = i*nDataVar;
				values[2*i] = (float) (sdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0)values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}		
		delete [] sdata;
		break;
	}	
	case cx_prim_long:
	{
		long *ldata = new long[np];
		if (permVec)
		{
			for (i = 0; i < numPoints * nDataVar; i++) ldata[i] = cdata->d.cx_prim_long.values[permVec[i]];
			for (i = 0; i < numPoints; i++)
			{
				ip = permVec[i]*nDataVar;
				values[2*i] = (float) (ldata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		else
		{
			for (i = 0; i < numPoints * nDataVar; i++) ldata[i] = cdata->d.cx_prim_long.values[i];
			for (i = 0; i < numPoints; i++)
			{
				ip = i*nDataVar;
				values[2*i] = (float) (ldata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}		    
		}
		delete [] ldata;
		break;
	}	
	case cx_prim_float:
	{
		float *fdata = new float[np];
		if (permVec) 
		{
			for (i = 0; i < numPoints * nDataVar; i++) fdata[i] = cdata->d.cx_prim_float.values[permVec[i]];
			for (i = 0; i < numPoints; i++)
			{
				ip = permVec[i]*nDataVar;
				values[2*i] = (fdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		else
		{
			for (i = 0; i < numPoints * nDataVar; i++) fdata[i] = cdata->d.cx_prim_float.values[i];
			for (i = 0; i < numPoints; i++)
			{
				ip = i*nDataVar;
				values[2*i] = (fdata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		
		delete [] fdata;
		break;
	}
	case cx_prim_double:
	{
		double *ddata = new double[np];
		if (permVec)
		{
			for (i = 0; i < numPoints * nDataVar; i++) ddata[i] = cdata->d.cx_prim_double.values[permVec[i]];
			for (i = 0; i < numPoints; i++)
			{
				ip = permVec[i]*nDataVar;
				values[2*i] = (float) (ddata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		else
		{
			for (i = 0; i < numPoints * nDataVar; i++) ddata[i] = cdata->d.cx_prim_double.values[i];
			for (i = 0; i < numPoints; i++)
			{
				ip = i*nDataVar;
				values[2*i] = (float) (ddata[ip+chan] - crange[0]) / (crange[1] - crange[0]);
				if (values[2*i] < 0.0) values[2*i] = 0.0;
				if (values[2*i] > 1.0) values[2*i] = 1.0;
				values[2*i+1] = 0.0;
			}
		}
		
		delete [] ddata;
		break;
	}
	default:
	  assert(0);
	}
	cxGeoTextureCoordinate2Define((int)numPoints, values);

	/* Add the colors to the scene graph.  Ask for the texture to be clamped
	in the S direction (where we have nsamples points) - otherwise, its
	repeated which, for some reason, gives speckled textures (last entry
	plus first entry in texture map) where S = 1 for some orientations 
	when the geometry is flat.  It doesn't matter what we ask for in the
	T direction (where we have just 1 point) so we use the default behaviour
	(repeating) there. */
	cxGeoTextureAdd((int)nsamples, 1, col,
		CX_GEO_TEXTURE_CLAMP, CX_GEO_TEXTURE_REPEAT, 
		CX_GEO_PER_VERTEX_INDEXED);

	/* reclaim memory */
	delete [] col;
	delete [] values;
	return;
}
