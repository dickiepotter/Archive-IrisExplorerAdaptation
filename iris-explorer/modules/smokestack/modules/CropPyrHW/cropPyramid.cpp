/*
 $Id: cropPyramid.cpp,v 1.3 2000/11/01 17:33:11 arnaud Exp $
 */

/* COPYRIGHT_BEGIN
 *    Copyright (c) 1991-1994 Silicon Graphics, Inc.
 *
 *    Permission to use, copy, modify, distribute, and sell this software
 *    and its documentation for any purpose is hereby granted without fee,
 *    provided that (i) the above copyright notices and this permission
 *    notice appear in all copies of the software and related
 *    documentation, and (ii) the name of Silicon Graphics may not be used
 *    in any advertising or publicity relating to the software without the
 *    specific, prior written permission of Silicon Graphics.
 *
 *    THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 *    WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *    IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR ANY SPECIAL,
 *    INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY
 *    DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *    WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY
 *    THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
 * COPYRIGHT_END */

/* ------------------------------------------- */
#include <stdlib.h> // for free()
#include <string.h>

#include <cx/DataTypes.h>
#include <cx/PortAccess.h>
#include <cx/DataAccess.h>
#include <cx/DataExtract.h>
#include <cx/Pyramid.h>
#include <cx/cxPyramid.api.h>
#include <cx/cxLattice.api.h>
#include <cx/MinMax.h>
#include <cx/UI.h>
#include <cx/Info.h>
#include <cx/cxOs.h>

static float xmin,xmax;
static float ymin,ymax;
static float zmin,zmax;

static double dmin,dmax;

#define SELECTION_COORD 0
#define SELECTION_DATA  1

/* ------------------------------------------- */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void dataMinMax( cxData *data, long channel, double *minVal, double *maxVal )
{
	long   nDim, *dims;
	void  *dataVals;
	long   nVals, i;

	cxErrorCode ec;

	*minVal = *maxVal = 0.0;
	if (!data) {
		return;
	}

	nDim = data->nDim;
	dims = data->dims;

	nVals = cxDimsProd( nDim, dims, 1 );
	if (nVals < 1) {
		return;
	}

	dataVals = cxDataDataArrayGet( data, &ec );

#define CASE(CX_TYPE, TYPE) 						\
	case CX_TYPE: {							\
		TYPE *vec;						\
		vec = (TYPE *) dataVals;				\
		vec += channel-1;					\
		*minVal = *maxVal = (double) vec[0];			\
									\
		for (i=0; i<nVals; i++, vec += data->nDataVar) {	\
			*minVal = MIN( *minVal, (double) vec[0] );	\
			*maxVal = MAX( *maxVal, (double) vec[0] );	\
		}							\
	      break;							\
	}

	switch (data->primType) {
	CASE(cx_prim_byte,unsigned char)
	CASE(cx_prim_short,short)
	CASE(cx_prim_long,long)
	CASE(cx_prim_float,float)
	CASE(cx_prim_double,double)
	default:
		break;
	}
#undef CASE
}
static void minMaxCoordCalc( long    nDim,
			   long   *dims,
			   long    NCV,
			   float  *coordVals,
			   float  *minCoord,
			   float  *maxCoord )
{
  long nVals, i, j;

  nVals = cxDimsProd( nDim, dims, 1 );
  if (nVals < 1) return;

  /* Set up the first values. */
  for (i=0; i<NCV; i++)
	minCoord[i] = maxCoord[i] = coordVals[i];

  /* Now loop over the rest of the coord array. */
  for (i=0; i<nVals; i++) {
	for (j=0; j<NCV; j++) {
	 	minCoord[j] = MIN( minCoord[j], *coordVals );
	 	maxCoord[j] = MAX( maxCoord[j], *coordVals );
		coordVals++;
	}
  }
}
/* ------------------------------------------- */
static void sampleWidgets(
		   long  newData,
	 	   cxLattice *lat,
		   float *xLo,		/* The widget cropping values */
		   float *xHi,
		   float *yLo,
		   float *yHi,
		   float *zLo,
		   float *zHi,
		   long   selection,
		   long   channel
	 )
{
  int minPort, maxPort;
  long nDim, NCV, NDV;
  long minChanged, maxChanged;
  double minVal;
  double maxVal;


  nDim = lat->nDim;

  if (selection == SELECTION_COORD) {
	NCV  = lat->coord->c.cx_coord_curvilinear.nCoordVar;
  } else {
	NDV  = lat->data->nDataVar;
  }

  /*
   * For each pair of sliders, set the min/max range.
   */


  if (newData) {
        if (selection == SELECTION_COORD) {

		float *minCoord = new float[NCV];
		float *maxCoord = new float[NCV];

	 	minMaxCoordCalc( lat->nDim,
			lat->dims,
			NCV,
			lat->coord->c.cx_coord_curvilinear.values,
			minCoord,
			maxCoord );
		if (NCV > 0) {
			cxInWdgtDblMinMaxSet( "X Min",
				(double) minCoord[0],
				(double) maxCoord[0] );
			cxInWdgtDblMinMaxSet( "X Max",
				(double) minCoord[0],
				(double) maxCoord[0] );
		}
		if (NCV > 1) {
			cxInWdgtDblMinMaxSet( "Y Min",
				(double) minCoord[1],
				(double) maxCoord[1] );
			cxInWdgtDblMinMaxSet( "Y Max",
				(double) minCoord[1],
				(double) maxCoord[1] );
		}
		if (NCV > 2) {
			cxInWdgtDblMinMaxSet( "Z Min",
				(double) minCoord[2],
				(double) maxCoord[2] );
			cxInWdgtDblMinMaxSet( "Z Max",
				(double) minCoord[2],
				(double) maxCoord[2] );
		}

	 	/* Free up temp storage */
 		delete [] minCoord;
	 	delete [] maxCoord;

	} else {
		/* SELECTION_DATA */

		if (NDV > 1) {
			cxInWdgtLongMinMaxSet( "Channel", 1L, NDV );
			cxInWdgtShow( "Channel" );
		} else {
			cxInWdgtLongSet( "Channel", 1 );
			channel = 1;
			cxInWdgtHide( "Channel" );
		}

	 	dataMinMax( lat->data, channel, &minVal, &maxVal );

//	The following causes problems when we switch levels.  So
//	we'll just set the widgets' ranges to the data range.
//
// 		double minWidget;
// 		double maxWidget;
// 		double widgetVal;
// 		cxParameter *param;
//
// 		minPort = cxInputPortOpen( "Data Min" );
// 		param = (cxParameter *) cxInputDataGet( minPort );
// 		widgetVal = cxParamDblGet( param );
//
// 		minWidget = MIN( minVal, widgetVal );
// 		maxWidget = MAX( maxVal, widgetVal );
// 		cxInWdgtDblMinMaxSet( "Data Min", minWidget, maxWidget );
//
// 		maxPort = cxInputPortOpen( "Data Max" );
// 		param = (cxParameter *) cxInputDataGet( maxPort );
// 		widgetVal = cxParamDblGet( param );
//
// 		minWidget = MIN( minVal, widgetVal );
// 		maxWidget = MAX( maxVal, widgetVal );
// 		cxInWdgtDblMinMaxSet( "Data Max", minWidget, maxWidget );

		cxInWdgtDblMinMaxSet("Data Min", minVal, maxVal);
		cxInWdgtDblMinMaxSet("Data Max", minVal, maxVal);
	}

  }

  /*
   * For each pair of sliders, set the lower and upper sliders
   * and decide whether to show/hide the widgets.
   */

  if (selection == SELECTION_COORD) {

	if (NCV > 0) {
		minPort = cxInputPortOpen( "X Min" );
		maxPort = cxInputPortOpen( "X Max" );
		minChanged = cxInputDataChanged( minPort );
		maxChanged = cxInputDataChanged( maxPort );

		if ( minChanged && *xLo > *xHi ) {
			*xHi = *xLo;
			cxInWdgtDblSet( "X Max", *xHi );
		}

		if ( maxChanged && *xLo > *xHi ) {
			*xLo = *xHi;
			cxInWdgtDblSet( "X Min", *xLo );
		}

		cxInWdgtShow( "X Min" );
		cxInWdgtShow( "X Max" );
	}
	else {
		cxInWdgtHide( "X Min" );
		cxInWdgtHide( "X Max" );
	}

	if (NCV > 1) {
		minPort = cxInputPortOpen( "Y Min" );
		maxPort = cxInputPortOpen( "Y Max" );
		minChanged = cxInputDataChanged( minPort );
		maxChanged = cxInputDataChanged( maxPort );

		if ( minChanged && *yLo > *yHi ) {
			*yHi = *yLo;
			cxInWdgtDblSet( "Y Max", *yHi );
		}

		if ( maxChanged && *yLo > *yHi ) {
			*yLo = *yHi;
			cxInWdgtDblSet( "Y Min", *yLo );
		}

	 	cxInWdgtShow( "Y Min" );
	 	cxInWdgtShow( "Y Max" );
	}
	else {
	 	cxInWdgtHide( "Y Min" );
	 	cxInWdgtHide( "Y Max" );
	}

	if (NCV > 2) {
		minPort = cxInputPortOpen( "Z Min" );
		maxPort = cxInputPortOpen( "Z Max" );
		minChanged = cxInputDataChanged( minPort );
		maxChanged = cxInputDataChanged( maxPort );

		if ( minChanged && *zLo > *zHi ) {
			*zHi = *zLo;
			cxInWdgtDblSet( "Z Max", *zHi );
		}

		if ( maxChanged && *zLo > *zHi ) {
			*zLo = *zHi;
			cxInWdgtDblSet( "Z Min", *zLo );
		}

	 	cxInWdgtShow( "Z Min" );
	 	cxInWdgtShow( "Z Max" );
	}
	else {
	 	cxInWdgtHide( "Z Min" );
	 	cxInWdgtHide( "Z Max" );
	}
  }
  else {
	/* else  (selection == SELECTION_COORD) */

	minPort = cxInputPortOpen( "Data Min" );
	maxPort = cxInputPortOpen( "Data Max" );

	minVal = cxParamDblGet( (cxParameter *) cxInputDataGet( minPort ) );
	maxVal = cxParamDblGet( (cxParameter *) cxInputDataGet( maxPort ) );

	minChanged = cxInputDataChanged( minPort );
	maxChanged = cxInputDataChanged( maxPort );

	if ( minChanged && minVal > maxVal ) {
		cxInWdgtDblSet( "Data Max", minVal );
	}
	else if ( maxChanged && minVal > maxVal ) {
		cxInWdgtDblSet( "Data Min", maxVal );
	}
  }

}
/* ------------------------------------------- */

#ifdef __cplusplus
	extern "C" {
#endif

void  cropPyramid  (void)
{
	cxPyramid *pyr, *pyr_out;
	static int first_time = 1;
	static long old_level;
	static long selection;

#ifdef RTH
	{
	  int mlen = pyr->layer[2].relation->numElements;
	  int *mark = new int[mlen];
	  memset(mark, 0, sizeof(int)*mlen);
	  
	  mark[2] = 1;
	  
	  cxPyrClean(pyr,mark);
	  delete [] mark;
	}
#endif

	/* If this is the first time this module's been called,
	 * do some initialisations. The name of the module won't
	 * change from one call to another. Also, we need to keep
	 * track of when the selected level of the pyramid changes.
	 */

	if (first_time)
	{
		first_time = 0;
		/* Is this CropPyr or CullPyr? */
		const char *name = cxModuleBaseNameGet();
		if (strcmp( name, "CropPyr" ) == 0)
			selection = SELECTION_COORD;
		else
			selection = SELECTION_DATA;

		/* Set old_level to a disallowed value to force
		 * detection of a change first_time through.
		 */
		 old_level = -1;
	}

	/* Get dataset pointers. */
	int port = cxInputPortOpen( "Input" );
	pyr = ( cxPyramid *) cxInputDataGet( port );
	long newData = cxInputDataChanged( port );
	long level;
	long channel;
	long range;

	if (selection == SELECTION_COORD)
	{
		port = cxInputPortOpen( "X Min" );
		cxParameter *xMin = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "X Max" );
		cxParameter *xMax = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Y Min" );
		cxParameter *yMin = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Y Max" );
	 	cxParameter *yMax = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Z Min" );
	 	cxParameter *zMin = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Z Max" );
	 	cxParameter *zMax = ( cxParameter *) cxInputDataGet( port );

		/* set up globals */
		xmin = cxParamDblGet( xMin );
		xmax = cxParamDblGet( xMax );
		ymin = cxParamDblGet( yMin );
		ymax = cxParamDblGet( yMax );
		zmin = cxParamDblGet( zMin );
		zmax = cxParamDblGet( zMax );

		level = 0;

	}
	else
	{
		port = cxInputPortOpen( "Data Min" );
		cxParameter *dataMin = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Data Max" );
		cxParameter *dataMax = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Channel" );
	 	cxParameter *Channel = ( cxParameter *) cxInputDataGet( port );

		port = cxInputPortOpen( "Range" );
	 	cxParameter *Range = ( cxParameter *) cxInputDataGet( port );

		/* set up globals */
		dmin = cxParamDblGet( dataMin );
		dmax = cxParamDblGet( dataMax );

		channel = cxParamLongGet( Channel );
		range = cxParamLongGet( Range );

		/* Check if we have data at base and top lattices */
	    if ( pyr->baseLattice == NULL )	/* no base so just pass to output port - HW */
		{
	    /* copy pyramid, but don't duplicate data */
	        pyr = pyr_out = cxPyramidCopy(pyr);
	        if (cxDataAllocErrorGet()) return;
	        port = cxOutputPortOpen("Output");
	        cxOutputDataSet(port, pyr_out);
  
		    return;
		}
		if (pyr->layer[pyr->count-1].nextLattice != NULL &&
			pyr->baseLattice != NULL)
		{
			cxInWdgtShow("Level");
			level =
			cxParamLongGet((cxParameter*)cxInputDataGet(cxInputPortOpen("Level")));
		}
		else
		{
			cxInWdgtHide("Level");
			level = 0;
			cxInWdgtLongSet("Level", level);
		}
	}

	cxErrorCode ec;
	int i;		/* counting variable */

	/* get lattices */
	cxLattice *baseLat = pyr->baseLattice;
	cxLattice *topLat = pyr->layer[pyr->count-1].nextLattice;

	/* do a little error checking to avoid problems with missing data/coords */
	if (!baseLat->coord) return;
	if (selection == SELECTION_DATA)
	{
		if ((level==0 && !baseLat->data) || (level==1 && !topLat->data)) return;
	}

	/* There's new data if we've got a new pyramid, or if we've changed
	 * the level.
	 */
	newData = newData || (level != old_level);
	old_level = level;

	/* copy pyramid, but don't duplicate data */
	pyr = pyr_out = cxPyramidCopy(pyr);
	if (cxDataAllocErrorGet()) return;

	/* Set the widget values and min/max limits. */
	int latLen;
	if (level==0)
	{
		sampleWidgets(newData, baseLat, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, selection, channel );
		latLen = (int) cxDimsProd(baseLat->nDim, baseLat->dims, 1);

	}
	else
	{
		sampleWidgets(newData, topLat, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax, selection, channel );
		latLen = (int) cxDimsProd(topLat->nDim, topLat->dims, 1);
	}

	/* allocate latFlag array at the lattice level */
	char *latFlag = new char[latLen];

	/*
	 * loop over lattice elements, calculating an array of
	 * Booleans saying whether vertices are in the cropped region
	 */

	if (selection == SELECTION_COORD)
	{
		/* use base lattice */
		long NCV  = baseLat->coord->c.cx_coord_curvilinear.nCoordVar;
		float *v = new float[NCV];
		for ( i = 0; i < latLen; i++ )
		{
			cxLatCoordExtract(baseLat,&i,v);

			/* test this node */
			latFlag[i] = 1;

			if ( NCV >= 1 && (v[0] < xmin || v[0] > xmax)) {
				latFlag[i] = 0;
				continue;
			}
			if ( NCV >= 2 && (v[1] < ymin || v[1] > ymax)) {
				latFlag[i] = 0;
				continue;
			}
			if ( NCV >= 3 && (v[2] < zmin || v[2] > zmax)) {
				latFlag[i] = 0;
				continue;
			}
		}
		delete [] v;
	}
	else // SELECTION_DATA
	{
		void *dataVals;
		long nDataVar;
		cxPrimType primType;

		if (level==0)
		{
			primType = baseLat->data->primType;
			nDataVar = baseLat->data->nDataVar;
			dataVals = cxDataDataArrayGet(baseLat->data, &ec);
		}
		else
		{
			primType = topLat->data->primType;
			nDataVar = topLat->data->nDataVar;
			dataVals = cxDataDataArrayGet(topLat->data, &ec);
		}

#define CASE(CX_TYPE, TYPE) 					\
		case CX_TYPE: {					\
			TYPE *vec;				\
			vec = (TYPE *) dataVals;		\
			vec += channel-1;			\
								\
			if (range == 0) {			\
				for (i=0; i<latLen; i++) {	\
					latFlag[i] = dmin <= vec[0] && vec[0] <= dmax;		\
					vec += nDataVar;	\
				}				\
			} else {				\
				for (i=0; i<latLen; i++) {	\
					latFlag[i] = vec[0] < dmin || dmax < vec[0];		\
					vec += nDataVar;	\
				}				\
			} 					\
			break;					\
		}

		switch (primType) {
		CASE(cx_prim_byte,char)
		CASE(cx_prim_short,short)
		CASE(cx_prim_long,long)
		CASE(cx_prim_float,float)
		CASE(cx_prim_double,double)
		default:
			break;
		}
#undef CASE
	}

	/*
	 * Get the connections list for the top layer, then prepare
	 * to replace it.
	 */
	cxConnection *con;
	cxPyrLayerGet( pyr, pyr->count, &con, NULL );

	/* Find the active elements in the top layer.
	 * If we're using the data at the top layer, then we've
	 * got this already; otherwise, we propagate the effect
	 * of the active vertices upwards to the top layer.
	 */
	char *flg;
	if( level != 0 )
	    flg	= latFlag;
	else
	{
	    flg = cxPyrActivePrev(pyr, 0, pyr->count, latFlag, cx_pyramid_prev_logical_and);
	    latLen = con->numElements;
	}

	/* Now make the new connections list for the top layer.*/
	long ne = 0;
	long nc = 0;
	for (i=0; i<latLen; i++)
	{
		if (flg[i] != 0)
		{
			ne ++;
			nc += con->elements[i+1] - con->elements[i];
		}
	}
	cxConnection *newCon = cxConnNew( ne, nc );

	/* Now fill the new connections list.  If we're using the top
	 * level data, also create a new top level data lattice.
	 */

	ne = 0;
	nc = 0;
	long num;

	// copy the top-level lattice if it exists
	if (!topLat)
	{
	    for (i=0; i<latLen; i++)	// copy just the connections
	    {
			if (flg[i] != 0)
			{
		    	num = con->elements[i+1] - con->elements[i];
		    	newCon->elements[ne+1] = newCon->elements[ne] + num;
		    	memmove(
			      &newCon->connections[nc],
			      &con->connections[con->elements[i]],
			      num * sizeof(long));
		    	nc += num;
		    	ne++;
			}
	    }

	    /* Now install the new connection list in the top layer. */
		cxPyrLayerSet(pyr, pyr->count, newCon, NULL);

	}
	else
	{

	    /* Make the new lattice.  Copy data and connections */

	    long nDataVar = topLat->data->nDataVar;
	    cxPrimType primType = topLat->data->primType;
	    long nCoordVar = topLat->coord->c.cx_coord_curvilinear.nCoordVar;
	    cxLattice *newLat = cxLatNew( (long)1, &(newCon->numElements),
			nDataVar, primType, nCoordVar, cx_coord_curvilinear );

	    int dvecSize = cxDataPrimSize( newLat->data ) * nDataVar;
	    int cvecSize = sizeof(float) * nCoordVar;
	    void *newDVals = cxDataDataArrayGet( newLat->data, &ec );
	    void *dataVals = cxDataDataArrayGet( topLat->data, &ec );
	    float *newCVals = cxCoordValsGet( newLat->coord);
	    float *coordVals = cxCoordValsGet( topLat->coord );

	    /* Now fill the connection list and the lattice. */

#define CASE(CX_TYPE, TYPE)					\
	    case CX_TYPE: {					\
		for (i=0; i<latLen; i++)			\
		{				    		\
		    if (flg[i]!=0)				\
		    {					    	\
				num = con->elements[i+1] - con->elements[i];	    	\
				newCon->elements[ne+1] = newCon->elements[ne] + num;	\
				memmove(\
					&newCon->connections[nc],	\
					&con->connections[con->elements[i]],\
						num * sizeof(long) );\
				nc += num;			\
								\
				memmove(			\
					&((TYPE *)newDVals)[ne*nDataVar], \
					&((TYPE *)dataVals)[i*nDataVar],\
						dvecSize );	\
				memmove(			\
						&(newCVals)[ne*nCoordVar],\
						&(coordVals)[i*nCoordVar],\
						cvecSize );	\
				ne++;				\
		    }						\
		}						\
		break;						\
	    }

	    switch (topLat->data->primType)
	    {
	    	CASE(cx_prim_byte,char)
	    	CASE(cx_prim_short,short)
	    	CASE(cx_prim_long,long)
	    	CASE(cx_prim_float,float)
	    	CASE(cx_prim_double,double)
	    	default:
			break;
	    }
#undef CASE

	    /* Now install the new connection layer and lattice
	     * in the output pyramid. */
	    cxPyrLayerSet(pyr, pyr->count, newCon, newLat);
	}

	/* free memory */
	delete [] latFlag;
	if (level == 0) { cxFree( flg ); }

	port = cxOutputPortOpen("Output");
	cxOutputDataSet(port, pyr_out);


}

/* ------------------------------------------- */

#ifdef __cplusplus
}
#endif
