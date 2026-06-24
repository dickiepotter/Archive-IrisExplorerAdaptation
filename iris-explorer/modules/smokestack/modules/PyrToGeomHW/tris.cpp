//SA : Header file for the garbage collector

#ifdef __osf__
class cxPyramid;
#endif

#include <string.h>
#include <stdlib.h>

#include <cx/Pyramid.h>
#include <cx/cxPyramid.api.h>
#include <cx/MinMax.h>
#include <cx/DataAccess.h>

#include <cx/DataExtract.h>
#include <cx/Geometry.h>
#include <cx/cxOs.h>

#define PELEM(cxPYR,cxLAYER,cxINDEX)	cxPYR->layer[cxLAYER].relation->elements[cxINDEX]
#define PCONN(cxPYR,cxLAYER,cxINDEX)	cxPYR->layer[cxLAYER].relation->connections[cxINDEX]
#define ELEM(cxLAYER,cxINDEX)	pyr->layer[cxLAYER].relation->elements[cxINDEX]
#define CONN(cxLAYER,cxINDEX)	pyr->layer[cxLAYER].relation->connections[cxINDEX]
#define ELEMvx(cxINDEX)	con->elements[cxINDEX]
#define CONNvx(cxINDEX)	con->connections[cxINDEX]
#define DICT pyr->ref.dictionary

static void addpoly(cxPyramid*, int, int*&, int&);
//void addpolyVertex( cxPyramid *, long*, long&, long*& );
static long findParent(cxPyramid*, long, long );

#include "all_func.h"

void tris(cxPyramid *pyr, float *xform, long nDim, long level, long ivar, long **permVec, long *count)
{
	long i, j, k;
	char *flg;
	int nindex, *index;
	int ind;

	/*
	 * Handle compressed pyramids with a suite of approaches.
	 * These approaches depend on the display dimensionality and
	 * the level of compression in the pyramid.
	 *
	 *             compression dim
	 * display	none	1		2		3	>3
	 *	
	 *	0		pts		pts		pts		pts		pts
	 * 	1		orig	direct	dict	dict	dict
	 * 	2		orig	expand	direct	dict	dict
	 * 	3		orig	expand	direct*	dict+	dict+
	 *
	 * (3 is really 2 with removal of duplicate faces)
	 *
	 * KEY:
	 *	orig	Original PyrToGeom code for rev 1 pyramids
	 *	pts		Find active pts and use them only
	 *	expand	cxPyrDupExpand and call again
	 *	dict	recompress dictionary to display level
	 *			and then create geom output as concatenation
	 *			of the recompressed items
	 *	dict+	like dict, with additional suppression of
	 *			duplicated faces. XXX additional work not implemented
	 *	direct	use the compressed pyrs to generate geom directly
	 *	direct*	since the faces are the compressed entities,
	 *			we can remove duplicates
  	 */
	
	/*
	 * Note: the expand and exterior cases are now handled in
	 * pyr2geom.C. No manipulation of colour data is done in this
	 * file
	 */


	/* 
	 * Set up permuted base lattice for color lookups. 
	 * Only do this when we've selected a subset of the points/etc
	 * for display and can no longer access the base lattice directly.
	 */
	*permVec = NULL;

	cxLattice *lat = pyr->baseLattice;
	int size = (int)lat->dims[0];
	if (size <= 0) return;

	float *p = new float[3*size];
	float *ptr = p;

	cxCoord *coord;
	/* transform the coordinates if necessary */
	if (xform) coord = cxCoordTransform(lat->coord, 4, 4, xform);
	else	coord = lat->coord;

	/* first, make the coordinate array */
	for ( i = 0; i < size; i++ )
	{
		float v[16];

		/* clear out receiving vector */
		v[0] = v[1] = v[2] = 0.0;

		/* get coordinates */
		int ind[1];
		ind[0] = (int)i;

		cxCoordExtract(coord,ind,v);

		*ptr++ = v[0];
		*ptr++ = v[1];
		*ptr++ = v[2];
	}

	/* switch on layering */
	int nface;
	int *iptr;
	
	nindex = 0;
	index = NULL;


	/*
	 * Handle the uncompressed (ORIG) and display of points (PTS)
	 * cases. If there is no compression, create the lines or
	 * polygons directly. If points are requested, then find out
	 * which are active and display them. In the latter case, can
	 * skip over compression layers quite effectively.
	 */
	if (pyr->ref.compressType == cx_compress_none || nDim == 0)
	{

	  switch ( nDim ) 
	    {
	        case 0:	/* points only */
			{
			/* 
			 * Which vertices are active? Compress p to them.
			 */
			flg = cxPyrActive( pyr, 0 );
			if (!flg) {
				return;
			}

			int ct = 0;
			float *countPtr = ptr = p;

			for (i=0; i<size; i++) {
				if (flg[i]) {
					/* OK, so copy left. */
					countPtr[0] = ptr[0];
					countPtr[1] = ptr[1];
					countPtr[2] = ptr[2];
					countPtr += 3;
					ct++;
				}
				/* Always increment ptr. */
				ptr += 3;
			}

			/* Now create the permVec to handle the copy left. */
			*permVec = (long*) malloc( ct * sizeof(long));
			for (j=0, i=0; i<size; i++) {
				if (flg[i])
					(*permVec)[j++] = i;
			}
			*count = j;

			cxFree( flg );

			/* Now use the compressed array. */
			cxGeoPointsDefine(ct,p);
			}
			break;

		case 1: /* edges only */
			index = new int[
				pyr->layer[0].relation->numElements +
				pyr->layer[0].relation->numConnections];
			iptr = index;

			flg = cxPyrActive( pyr, 1 );
			if (!flg) {
				return;
			}

			for ( i = 0; i < pyr->layer[0].relation->numElements; i++ )
			{
			    if (flg[i]) {
				for ( j = ELEM(0,i); j < ELEM(0,i + 1); j++ )
					*iptr++ = CONN(0,j);
				
				if (iptr != index) /* XXX temporary */
				/* terminate the line with a -1 */
				    *iptr++ = -1;
			    }
			}

			/* create a lines object */
			cxGeoLinesDefine(size,p,iptr - index,index);
			delete [] index;
			cxFree(flg);
			break;

		case 2: /* faces */
			/* loop over faces */
			if ((nface = pyr->layer[1].relation->numElements) == 0)
				goto out;

			index = (int *)cxMalloc(sizeof(int));
			flg = cxPyrActive( pyr, 2 );
			if (!flg) {
				return;
			}

			for ( i = 0; i < nface; i++ )
				if (flg[i])
					addpoly(pyr,i,index,nindex);

			/* make the polys */
			if (nindex != 0)
				cxGeoPolysDefine(size,p,(int)nindex,index);
			cxFree(index);
			cxFree(flg);
			break;

		default: /* cells */
		
			if ((nface = pyr->layer[1].relation->numElements) == 0)
				goto out;

			index = (int *)cxMalloc(sizeof(int));
			flg = cxPyrActive( pyr, 2 );
			if (!flg) {
				return;
			}
			


			for( i=0; i<nface; i++ ) 
			if( flg[i] == 1 ) 
				addpoly(pyr, i, index, nindex);			    

			
			/* make the polys */
			if (nindex != 0)
				cxGeoPolysDefine(size,p,(int)nindex,index);
			cxFree(index);
			cxFree(flg);
			break;
	    }
	}

	else if (pyr->ref.compressType != cx_compress_none
		 && pyr->ref.dictionary->nDim == nDim ) 
	  {
  	    /*
	     * We know that nDim <= 2 and compression is at the display level.
	     *
	     * This is the DIRECT option: 
	     * either compression == display == lines, 
	     * or compression == faces and display == 2 (faces).
	     * 
	     */
	    *permVec = NULL;

	    switch ( nDim ) {
	    case 0: /* points only */
	      /* can't have these */
	      break;

	    case 1: /* edges */
	    case 2:  /* faces */
	    default: /* unique faces */



	      /* loop over display objects */
	      {
	      /* 
	       * Have compressed to display level. Handle by copying 
	       * line or face indices into the index vector with -1's.
	       */
	      if ((nface = pyr->layer[nDim-1].relation->numElements) == 0) goto out;
	      
	      nindex = 0;
	      index = (int *)cxMalloc(sizeof(int));
	      flg = cxPyrActive( pyr, nDim );
	      if (!flg) {
			return;
	      }
	      
	      // 
	      // Do some preprocessing work on the pyramid dictionary.
	      // Find out how many vertices are needed to represent each
	      // polygon/line in the dictionary. Also find out the 
	      // reordering of vertex indices for the dict's pyramids.
	      // 
 	      long cInd;

	      long  *nVerts = new long[DICT->numEntries];
	      if (!nVerts)
	      {
			cxFree( flg );
			return;
	      }
	      long **perms  = new long*[DICT->numEntries];
	      if (!nVerts) {
		cxFree( flg );
		delete [] nVerts;
		return;
	      }

	      for (i=0; i<DICT->numEntries; i++) {
		cxPyramid *p = (cxPyramid *) DICT->table[i];

		if (nDim == 1) {
		  nVerts[i] = PELEM(p,nDim-1,1) - PELEM(p,nDim-1,0);
		}
		else {
		  // nDim == 2
		  // 
		  // Count the number of vertices that make up the edges 
		  // of the polygon. 
	 	  //
		  nVerts[i] = cxConnMaxInd(p->layer[0].relation);
		}
		
		// Add in one for the -1 at the end.
		perms[i] = new long[nVerts[i]+1];

  	        if (!nVerts) {
		  cxFree( flg );
		  delete [] nVerts;
		  for (j=0; j<i; j++) {
		    delete [] perms[j];
		    perms[j] = 0;
	          }
		  delete [] perms;
 	  	  return;
	        }

		cxPyrVertOrder( 2, (cxPyramid *) DICT->table[i], nVerts[i], perms[i] );
	      }

	      // 
	      // Count the number of vertices in the active polygon.
	      // We already know how many vertices in each dictionary
	      // pyramid, so sum those numbers for each instance.
	      // 
	      for ( i = 0; i < nface; i++ ) {
		if (!flg[i]) {
		  continue;
		}

		cInd = cxPyrRefIndexGet( pyr, i );
		nindex += 1 + nVerts[cInd];
	      }
	      
	      /* Allocate space for geometry data, then fill it. */
	      index = (int *)cxMalloc(nindex*sizeof(int));
	      if (!index) {
		delete [] nVerts;
		for (j=0; j<DICT->numEntries; j++) {
		  delete [] perms[j];
		  perms[j] = 0;
	        }
		delete [] perms;
		cxFree( flg );
		return;
	      }

	      //
	      // We only allow (2D) faces that are active. Now have 
	      // allocated the space for polygons' (lines') vertices,
	      // so go and fill them.
	      //
	      nindex = 0;

		for ( i = 0; i < nface; i++ )
		{
			if (!flg[i])
				continue;
			//
			// Get the ordering of the vertices in the dictionary
			// element. Then apply that to the actual vertex indices
			// and store the result for display.
			//
			long cInd = cxPyrRefIndexGet( pyr, i );

			for( k=0; k<nVerts[cInd]; k++) 
				index[ nindex++ ] = CONN(nDim-1,ELEM(nDim-1,i)+perms[cInd][k]);
			index[ nindex++ ] = -1;
			
	    }
	      
	      /* make the polys */
	      if (nindex != 0)
	      {
			if (nDim == 1)
				cxGeoLinesDefine(size,p,(int)nindex,index);
			else
				cxGeoPolysDefine(size,p,(int)nindex,index);
	      }
	      cxFree(index);
	      cxFree(flg);
	      delete [] nVerts;
	      for (j=0; j<DICT->numEntries; j++) {
			delete [] perms[j];
			perms[j] = 0;
	      }
	      delete [] perms;
	      }
	      break;
	    }
	  }
	else 
	  {
	    /* 
	     * Compression above the display level.
	     *
	     * Now only have the DICT version to do. In this case
	     * we want to recompress the dictionary to the display level,
	     * then effectively concatenate the dictionary items to form
	     * the display list.
	     * By recompressing the dictionary,  we'll know that all
	     * entries at the display level are active and refer to 
	     * vertices, so we can do a quick transcription of the compressed
	     * entities into the destination array.
	     *
	     * Future work:	XXX Attempt to chain together edges and 
	     * faces to get polylines or Tmeshes for speed.
	     * 
	     * Go through each dictionary pyramid and represent it as a
	     * collection of the required item (face, edge, point). Then
	     * concatenate all compressed items, call cxPyrMerge, and use
	     * the resulting output as the list of items to represent.
	     */

		long disp = MIN(nDim,2);

	 	// 
	 	// Create the translation table from nDim to display layer.
 		// Then create a ragged array of vertex reorderings (in tmpCon)
		// to store the ordering information of the vertices in 
		// each polygonal dictionary element.
		//
		cxPyramidDictionary *trans, *td;
	 	trans = cxPyrDictCompress( pyr->ref.dictionary, disp );
		if (!trans) {
			return;
		}
		td = trans->table[0]->ref.dictionary;

		//
		// How many vertices (plus the -1) in each polygon?
		//
		long nte = td->numEntries;
		long ntc = 0;
		for (i=0; i<td->numEntries; i++) {
			ntc += 1 + cxConnMaxInd( td->table[i]->layer[0].relation );
		}

		//
		// Now fill the connection with the vertex reordering for
		// each polygon in the display-level dictionary.
		// 
		cxConnection *tmpCon = cxConnNew( nte, ntc );
		if (!tmpCon) {
			cxDataRefDec( trans );
			return;
		}
		/*
		 * ELSE nDim == 2, so do the dictionary hashing method.
		 */

		nte = 0;
		ntc = 0;
		long nv = 0;
		for (i=0; i<td->numEntries; i++) {
			nv = cxConnMaxInd( td->table[i]->layer[0].relation );
			ntc = tmpCon->elements[i];
			tmpCon->elements[i+1] = ntc + nv + 1;
			cxPyrVertOrder( 2, (cxPyramid *) td->table[i], nv, &tmpCon->connections[ ntc ] );
			tmpCon->connections[ ntc + nv ] = -1;
		}

		//
		// How many vertices (plus the -1) in each n-D object?
		//
		long *objCounts = new long[trans->numEntries];
		if (!objCounts) {
			cxDataRefDec( tmpCon );
			cxDataRefDec( trans );
			return;
		}

		for (i=0; i<trans->numEntries; i++) {
			objCounts[i] = 0;
			cxPyramid *p = (cxPyramid *) trans->table[i];
			cxConnection *c = p->layer[disp-1].relation;
			for (j=0; j<c->numElements; j++) {
				ind = cxPyrRefIndexGet( p, j );
				objCounts[i] += tmpCon->elements[ind+1] - tmpCon->elements[ind];
			}
			ntc += objCounts[i];
		}

		cxConnection *transCon = cxConnNew( trans->numEntries, ntc );
		if (!transCon) {
			delete [] objCounts;
			cxDataRefDec( tmpCon );
			cxDataRefDec( trans );
			return;
		}

		for (i=0; i<trans->numEntries; i++) {
			transCon->elements[i+1] = objCounts[i] + transCon->elements[i];
		}
		delete [] objCounts;

		//
		// Now fill a ragged array of the vertex indices for each
		// trans-level dictionary element.
		//
		nte = 0;
		ntc = 0;
		long *cvert;
		for (i=0; i<trans->numEntries; i++) {
			cxPyramid *p = (cxPyramid *) trans->table[i];
			cxConnection *c = p->layer[disp-1].relation;
			long *tc = &transCon->connections[ transCon->elements[i]];
			for (j=0; j<c->numElements; j++) {
				ind = cxPyrRefIndexGet( p, j );
				cvert = &c->connections[c->elements[j]];
				for (k=tmpCon->elements[ind]; k<tmpCon->elements[ind+1]; k++) {
					*tc++ = (tmpCon->connections[k] == -1) 
						? -1
						: cvert[tmpCon->connections[k]];
				}
			}
		}
		cxDataRefDec( trans );
		cxDataRefDec( tmpCon );

		// Now the Jth connection of the Ith element says how to 
		// reorder the Jth vertex of the input compressed element.

		/* How many elements all told? */
		long nEle = pyr->layer[DICT->nDim-1].relation->numElements;

		/* Compression dictionary element index */
		long cInd;

		/* 
		 * Alloc the index array. Count the contribution of each
		 * active element.
		 */
		flg = cxPyrActive( pyr, DICT->nDim );
		if (!flg) {
			cxDataRefDec( transCon );
			return;
		}

		for (i=0; i<nEle; i++) {
			if (flg[i]) {
				ind = cxPyrRefIndexGet( pyr, i );
				nindex += transCon->elements[ind+1] 
					- transCon->elements[ind];
			}
		}

		index = (int*) cxMalloc(nindex*sizeof(int) );
		if (!index) {
			cxFree( flg );
			cxDataRefDec( transCon );
			return;	
		}


		//
		// transCon contains the polygonal description in normalized
		// vertex indices, with -1 for end-of-polygon. Use this 
		// vertex permutation to index into con->connections, the
		// vertex list of the actual polygon, to get the correct
		// display order. Fill index[] and call cxGeo routines.
		//
		//long *vertices = NULL;
		//long *reorder  = NULL;
		nindex = 0;	/* reset for use as counter */
		cxConnection *con = pyr->layer[DICT->nDim-1].relation;
		for (i=0; i<nEle; i++) {
			if (!flg[i]) {
				continue;
			}

			cInd = cxPyrRefIndexGet( pyr, i );
			long *e = transCon->elements;
			for (j=e[cInd]; j<e[cInd+1]; j++) {
				index[nindex++] = (transCon->connections[j] == -1)
					? -1
					: con->connections[ con->elements[i] + 
						transCon->connections[j] ];
			}
		}
		cxFree( flg );
		cxDataRefDec( transCon );

		switch (disp) {
			case 1: /* lines */
				cxGeoLinesDefine(size,p,(int)nindex,index);
				break;
			case 2: /* faces */
				cxGeoPolysDefine(size,p,(int)nindex,index);
				break;
			default:
				break;
		}

		cxFree( index );
	
	}

out:
      /* release the data */
      delete [] p;
}

void addpoly(cxPyramid *pyr,int i,int*& index,int& nindex)
{
	int	j, k;
	int	startIndex = nindex;
	int	last;
	int	e[2], s[2];

	for (k = 0, j = ELEM(1,i); j < ELEM(1,i+1) && k < 2; j++) {
		int edge0 = ELEM(0,CONN(1,j));
		int edge1 = ELEM(0,CONN(1,j) + 1);

		if (edge0 != edge1) {
			s[k] = edge0;
			e[k] = edge1 - 1;
			k++;
		}
	}

	if (k == 0) 		// no valid edges
		return;
	else if (k == 1)	// one valid edge
		last = s[0];
	else if ((CONN(0,s[0]) == CONN(0,e[1])) || (CONN(0,s[0]) == CONN(0,s[1])))
		last = CONN(0, e[0]);
	else if ((CONN(0,e[0]) == CONN(0,e[1])) || (CONN(0,e[0]) == CONN(0,s[1])))
		last = CONN(0, s[0]);
	else			// first two edges have a hole
		return;

	/* loop edges */
	for (j = ELEM(1,i); j < ELEM(1,i+1); j++) {
		int edge0 = ELEM(0,CONN(1,j));
		int edge1 = ELEM(0,CONN(1,j) + 1);

		if (edge0 == edge1)
			continue;
		edge1--;	// they are greater than zero apart

		/* allocate space */
		index = (int *)cxRealloc(index,
			(int)(nindex + (edge1 - edge0) + 1)*sizeof(int));

		if (last == CONN(0,edge0)) {
			/* ok ordering */
			for ( k = edge0 + 1; k <= edge1; k++ )
				index[nindex++] = CONN(0,k);
		} else {
			/* reverse ordering */
			for ( k = edge1 - 1; k >= edge0; k-- )
				index[nindex++] = CONN(0,k);
		}
		last = index[nindex - 1];
	}

	if ( nindex - startIndex > 4 )
	{
		int *buf = new int[nindex + 4*(nindex - startIndex)];

		for ( i = 0, j = 0; i < nindex - startIndex; i++ )
		{
			if ( i > 3 )
			{
				buf[j++] = -1;	/* terminate */
				buf[j++] = index[startIndex];
				buf[j++] = index[startIndex + i - 1];
			}
			buf[j++] = index[startIndex + i];
		}

		index = (int *)cxRealloc(index,(int)(nindex + j)*sizeof(int));
		memmove(&index[startIndex],buf,(int)j*sizeof(int));

		nindex = startIndex + j;

		delete [] buf;
	}

	if ((nindex - startIndex) < 3) {
		/* degenerate face */
		nindex = startIndex;
	} else {
		/* terminate this face */
		index = (int *)cxRealloc(index,(int)(nindex + 2)*sizeof(int));
		index[nindex++] = -1;
	}
}

long findParent(cxPyramid *pyr, long level, long element)
{
/* This routine looks for the first element in layer level+1 of
 * the pyramid that has a connection to the element in layer
 * level.
 */
    int ne, nc;
    long *elem, *conn;
    elem = pyr->layer[level+1].relation->elements;
    conn = pyr->layer[level+1].relation->connections;

    for( ne=0; ne<pyr->layer[level+1].relation->numElements; ne++ )
	for( nc=elem[ne]; nc<elem[ne+1]; nc++ )
	    if(element == conn[nc]) return ne;

    fprintf(stderr, "Couldn't find parent of element %ld in level %ld\n", 
			element, level+1);
    return 0;
}

// /*
//  * Distinguished from addpoly in that the input connection structure 
//  * points into the vertex list that makes up the polygon. 
//  */
// void addpolyVertex( 	cxPyramid    	*pyr,
// 			long		*vertices,
// 			long	        &nindex,
// 			long	       *&index )
// {
// 	/* 
// 	 * The cxConnection is a list of objects to be concatenated
// 	 * onto index[]. However, the vertex numbers are referenced
// 	 * through vertices, since 'objects' represents a dictionary
// 	 * pyramid with [0-n) indexing into a vertex array.
// 	 */
// 
// 	register long	i, j, k;
// 	register long *e, *c;
// 
// 	e = objects->elements;
// 	c = objects->connections;
// 	k = nindex;
// 	for (i=0; i<objects->numElements; i++, e++) {
// 		for (	j=*e; j<*(e+1); j++, c++) {
// 			index[k++] = vertices[*c];
// 		}
// 		index[k++] = -1;
// 	}
// 	nindex = k;
// }
// 
// 
// 
