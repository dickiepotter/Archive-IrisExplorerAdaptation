/* NAG NPGL sun port - file changed by geoffh */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cx/PortAccess.h>
#include <cx/DataTypes.h>
#include <cx/DataAccess.h>
#include <cx/DataOps.h>
#include <cx/UserFuncs.h>
#include <cx/UI.h>
#include <cx/Geometry.h>
#include <cx/cxLattice.api.h>
#include <cx/Pyramid.h>

#define DICT pyramid->ref.dictionary

#include "all_func.h"
#include "exteriorPyr.h"

extern "C"	void pyr2geom_init(void);

void pyr2geom_init(void)
{
	cxGeoInit();
}

extern	"C"	void pyr2geom(void);

void pyr2geom(void)
{
        cxPyramid *tmp = NULL;

	/* get input pyramid */
	cxPyramid *pyramid = (cxPyramid *)cxInputDataGet(cxInputPortOpen("Input"));

	if ( pyramid->baseLattice == NULL )	/* no base */
	{
	/* allocate a new geometry data type */
	cxGeometry *geo = cxGeoNew();
	if( cxDataAllocErrorGet() ) return;
	cxGeoBufferSelect(geo);

	/* Make the output a shared geometry if possible */
	cxGeoBufferPortSet(cxOutputPortOpen("Output"));

	/* delete any previously sent geometry */
	if( !cxDataAllocErrorGet() ) cxGeoRoot();
	if( !cxDataAllocErrorGet() ) cxGeoDelete();
	if( cxDataAllocErrorGet() ) {
		cxGeoBufferClose( geo );
		cxDataRefDec( geo );
		return;
	}
	/* close the geometry buffer */
	cxGeoBufferClose(geo);
	if( cxDataAllocErrorGet() ) {
		cxDataRefDec( geo );
		return;
	}

	/* assign to the output port */
	cxOutputDataSet(cxOutputPortOpen("Output"),geo);

		return;
	}
	
	/* get the transform, if any */
	cxLattice *xform = (cxLattice *)cxInputDataGet(cxInputPortOpen("Transform"));
	if ( xform )
	{
		/* verify size and type of lattice */
		if ( xform->data->primType != cx_prim_float )
		{
			cxModAlert("bad transform data type.");
			return;
		}
		if ( xform->data->nDim != 2 ||
			xform->data->dims[0] != 4 ||
			xform->data->dims[1] != 4 )
		{
			cxModAlert("bad transform size.");
			return;
		}
	}


	/* allocate a new geometry data type */
	cxGeometry *geo = cxGeoNew();
	if( cxDataAllocErrorGet() ) return;
	cxGeoBufferSelect(geo);

	/* Make the output a shared geometry if possible */
	cxGeoBufferPortSet(cxOutputPortOpen("Output"));

	/* delete any previously sent geometry */
	if( !cxDataAllocErrorGet() ) cxGeoRoot();
	if( !cxDataAllocErrorGet() ) cxGeoDelete();
	if( cxDataAllocErrorGet() ) {
		cxGeoBufferClose( geo );
		cxDataRefDec( geo );
		return;
	}

	/* Prepare to define the triangles.  nDim is the dimensionality of the 
	 * output geometry (0 = points, 1 = lines, etc).
	 */
	long nDim = cxParamLongGet((cxParameter *)cxInputDataGet(cxInputPortOpen("Dimension")));

	// force nDim to be no greater than pyr->count as we get bus errors
	// (don't change widget, though)
	if (nDim > pyramid->count) nDim = pyramid->count;

	long colorflag = cxParamLongGet((cxParameter *)cxInputDataGet(cxInputPortOpen("Coloring")));

	/* If we're not displaying the vertices and there's data in the pyramid
	 * at the corresponding level, we can choose between using this data and
	 * the vertex data to colour the geometry. Otherwise, use the vertex
	 * data.
	 *
	 * Add some extra checks when looking for the data.  Originally, we'd assumed
	 * that nextLattice would be NULL for no data, but it seems that for some 
	 * pyramids this isn't so.  So we check that the data part exists, and that
	 * nDataVar is finite as well.
	 *
	 * JPW 20-May-1999
	 */
	cxData *cdata;
	long level;
	if( nDim > 0 && 
            pyramid->layer[nDim-1].nextLattice != NULL &&
            (pyramid->layer[nDim-1].nextLattice)->data != NULL &&
            ((pyramid->layer[nDim-1].nextLattice)->data)->nDataVar != 0
        )
	{
	    cxInWdgtShow("Level");
	    level = cxParamLongGet((cxParameter *)cxInputDataGet(cxInputPortOpen("Level")));

		// use base lattice data
	    if( level == 0 ) cdata = pyramid->baseLattice->data;
	    else // use layer data
	    {
				cdata = pyramid->layer[nDim-1].nextLattice->data;
				level = nDim;		
				colorflag = 0; // force colormap rendering
				cxInWdgtLongSet("Coloring", colorflag);
				cxInWdgtHide("Csize");
	    }	    	    
	}
	else
	{
	    cxInWdgtHide("Level");
	    level = 0;
	    cxInWdgtLongSet("Level", level);
	    cdata = pyramid->baseLattice->data;
	}
	
	/* get the colormap, if any */
	cxLattice *cmap = (cxLattice *)cxInputDataGet(cxInputPortOpen("Colormap"));

	long ncols;

	/* switch on the texture color size widget if applicable */
	if (colorflag==2)
	{
		cxInWdgtShow("Csize");
		ncols =
cxParamLongGet((cxParameter*)cxInputDataGet(cxInputPortOpen("Csize")));
#ifdef __hpux
		ncols = pow(2.f,(int)ncols);
#else
		ncols = (long)pow((double)2.0,(double)ncols);
#endif
		if (ncols>64) ncols = 64;
	}
	else
	{
		cxInWdgtHide("Csize");
	}

	/* Allow for multiple data values in the lattice - need to select
	 * which channel is used for colouring.
	 */	
	// Make sure we set ivar to 1 to avoid wierdness on AlphaNT
	long ivar=1;
	if ((cdata==NULL) || (cdata->nDataVar==1) || (colorflag==1))
	{
		cxInWdgtHide("Component");
	}
	else
	{
		cxInWdgtLongMinMaxSet("Component", 1, cdata->nDataVar);
		cxInWdgtShow("Component");
		ivar = cxParamLongGet((cxParameter *)cxInputDataGet(cxInputPortOpen("Component")));
	}
	
	/* Make the triangles */
	long *permVec, count;

	/* handle the EXPAND CASES */
	if (pyramid->ref.compressType != cx_compress_none
		&& nDim > 0
		&& (DICT->nDim > 3 
		|| (DICT->nDim == 1 && nDim > 1)))
	{
	
		tmp = cxPyrExpand(pyramid);
		tris(tmp, xform ? xform->data->d.cx_prim_float.values : NULL,
			nDim, level, ivar, &permVec, &count);
		//cxDataRefDec(tmp);
	}
	/* handle the EXTERIOR CASES */
	else if (nDim==3)
	{
	        tmp = exteriorPyr(pyramid, 1);
		nDim--;
		tris(tmp, xform  ? xform->data->d.cx_prim_float.values : NULL,
			nDim, level, ivar, &permVec, &count);
		
		// update cdata to point at face level
		if (level) cdata = tmp->layer[1].nextLattice->data;
		//cxDataRefDec(tmp);
	}
	else
	{
		tris(pyramid, 
	    	xform ? xform->data->d.cx_prim_float.values : NULL,
	    	nDim,
	    	level, ivar, 
	    	&permVec, &count );
	}	
	
	if( cxDataAllocErrorGet() ) {
		cxGeoBufferClose( geo );
		cxDataRefDec( geo );
		if (permVec)
		  free(permVec);
		return;
	}

	/* check for color info */
        /* should handle the case when nDataVar=0 for 
 	   some lattice, usually the base lattice - panda may '99*/
	if ( cdata  && (cdata->nDataVar > 0))
	{
		if (colorflag==2 && level==0 && nDim > 0)
		{
			tcolor(cdata, cmap, ivar, ncols, permVec, count);
		}
		/* switch on number of channels in data */
		else if ( colorflag == 0 && cmap )
		{
			/* pass through colormap */
			falsecolor(cdata, cmap, ivar, permVec, level, count);
		}
		else
		{
			/* interpret as RGB or RGBa */
			truecolor(pyramid->baseLattice,permVec,count);
		}
	}

	/* free the permuation indices */
	if (permVec) {
	  free(permVec);
	  permVec = NULL;
	}

        if (tmp != NULL) 
        {
         cxDataRefDec(tmp);
        }
	/* delete the special structure created for cells */
	if( nDim == 3 && level != 0 ) cxDataRefDec((void *)cdata);

	/* close the geometry buffer */
	cxGeoBufferClose(geo);
	if( cxDataAllocErrorGet() ) {
		cxDataRefDec( geo );
		return;
	}

	/* assign to the output port */
	cxOutputDataSet(cxOutputPortOpen("Output"),geo);

	return;
}
