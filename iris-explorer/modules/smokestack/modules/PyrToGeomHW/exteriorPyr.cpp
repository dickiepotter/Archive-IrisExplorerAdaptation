/*
 $Id: exteriorPyr.cpp,v 1.1 2000/10/09 11:06:49 arnaud Exp $
 */

/* COPYRIGHT_BEGIN
 *    Copyright (c) 1991-1995 Silicon Graphics, Inc.
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

#if defined (__alpha)
/* need to forward reference cxPyramid explicitly on the DEC Alpha */
struct cxPyramid;
#endif

#include <stdlib.h> // for free(), calloc()
#include <string.h>
 
#include <cx/MinMax.h>
#include <cx/Typedefs.h>
#include <cx/DataAccess.h>
#include <cx/cxOs.h>

#include <cx/Pyramid.h>
#include <cx/cxPyramid.api.h>

#ifdef TESTING
# include <stdio.h>
#endif

#include "exteriorPyr.h"

//
// Create a new output pyramid that holds only the unshared faces or
// unshared edges of the input pyramid. If the input is uncompressed,
// then simply hash the uniquely active faces to find those with
// unique/multiple parents (effectively cxPyrMerge on faces or edges). 
// For the unshared faces case, if the input is compressed above the
// face level, preprocess the dictionary pyramids, then hash the faces
// as before.
// Create a 2D face compressed pyramid for output.
// 
// 	compression level of input
// none 	1D	2D	3D	4D
// 
// PA		PA	PA	hash	hash
// 
// where the types of actions are
// 	PA 	cxPyrActive() based (just watch out for 2D comp)
// 	hash	hash all faces and create uniq face list
// 	
// For the unshared edges case, if the input is compressed above the
// edge level, preprocess the dictionary pyramids, then hash the edges.
// Create a 1D edge compressed pyramid for output.
//
//      compression level of input
// none         1D      2D      3D      4D
//
// PA           PA      hash    hash    hash
//
// The "hash" method:
// 1. compress each dict pyr to 2D (for unshared faces) or 1D (for unshared
//    edges), so have vertex list per ele
// 2. for each compression element, hash face or edge vertex list marking
//    duplicates
// 3. scan hash list for unique faces or edges, saving
// 4. turn unique face or edge list into a compressed pyramid for output
// 5. clean up hash table.
// 
// 

#define DICT pyr->ref.dictionary

#ifdef __cplusplus
extern "C" {
#endif

typedef struct he {
	cxPyramid *pyr;		// ptr to this element type's dictionary pyr
	long ele;		// element number
	long workDimEle;	// face or edge number
	long count;		// 1, 2, ...
	he *next;		// next free or used
	he *chain;		// chain of siblings with 
				// same hash number
}  hashEle;

#define HASH_TAB_LEN 32749

#define ALLOCSIZE 1024
static hashEle *freeList = NULL;
static hashEle *deleteList = NULL;
static hashEle **tab;

static
void initTab()
{
	tab = new hashEle*[HASH_TAB_LEN];
	memset( tab, 0, HASH_TAB_LEN * sizeof( hashEle* ) );
}

static
void makeFree( hashEle *head )
{
	if (head) {
		head->next = freeList;
		head->chain = NULL;
		freeList = head;
	}
}
static
hashEle *nextFree()
{
	hashEle *ans;
	long	 i;

	if (freeList) {
		ans = freeList;
		freeList = freeList->next;
		ans->next = NULL;
		return ans;
	}
	else {
		// allocate a new block, assign to delete list, chain on free

		ans = (hashEle*) cxCalloc( ALLOCSIZE, sizeof(hashEle) );
		if (!ans)
			return NULL;

		ans[0].next = deleteList;
		deleteList = ans;

		for (i=1; i<ALLOCSIZE-1; i++) {
			ans[i].next = &ans[i+1];
		}
		ans[ALLOCSIZE-1].next = freeList;
		freeList = &ans[1];
	
		return nextFree();
	}
}

static 
void cleanup()
{
	hashEle *tmp;

	while (deleteList) {
		tmp = deleteList;
		deleteList = deleteList[0].next;

		cxFree( tmp );
	}

	freeList = NULL;
	deleteList = NULL;
	delete tab;
	tab = NULL;
}

#ifdef TESTING
static
void printEle( hashEle *h )
{
	if (h) {
		fprintf( stderr, 
			"ptr %0x, pyr %0x, ele %d, workDimEle %d, ct %d, nxt %0x, chain %0x\n", 
			h, h->pyr, h->ele, h->workDimEle, h->count, h->next, h->chain );
	}
	else 
		fprintf( stderr, "ptr 0x0\n" );
}

static
void printChain( hashEle *head )
{
	while (head) {
		printEle( head );
		head = head->chain;
	}
}

static
void printNext( hashEle *head )
{
	while (head) {
		printEle( head );
		head = head->next;
	}
}

#endif


static 
void separate( hashEle * &unique, hashEle * &multi )
{
	int i;
	hashEle *head;

	unique = NULL;
	multi = NULL;

	for (i=0; i<HASH_TAB_LEN; i++) {
		if (!tab[i])
			continue;

		head = tab[i];
		while (head) {

			if (head->count == 1) {
				head->next = unique;
				unique = head;
			}
			else {
				head->next = multi;
				multi = head;
			}
			
			head = head->chain;
		}
	}
}

static
long hashEqual( hashEle *head, long numVertices, long *vertices,
		cxConnection *con, long workDim )
{
	static long nVerts = 16;
	static long *verts = NULL;	// Allocated once, freed @ module death
	if (!verts)
		verts = new long[nVerts];

	long i, j, k;
	long nv;
	long *elems;
	long *srcV;
	hashEle *current;
	cxConnection *currentCon;
	cxPyrEquivType type;
	
	if (workDim == 1)
		type = CX_PYRAMID_EQUIV_IDENTICALR;
	else
		type = CX_PYRAMID_EQUIV_CYCLICR;

	for(k = 1, current = head; current != NULL;
	    k++, current = current->chain) {

		cxPyrLayerGet(current->pyr, workDim, &currentCon, NULL);
		elems = currentCon->elements;
		nv = elems[current->workDimEle + 1] -
		     elems[current->workDimEle];
		srcV = &con->connections[con->elements[current->ele]];

		// Must be the same type of face when shared faces
		// (workDim == 2) is called for.
		if (workDim == 2 && nv != numVertices) {
			if (current->chain)
				continue;
			else
				return 0;
		}

		if (nv > nVerts) {
			delete verts;
			nVerts = MIN(2*nVerts, nv);
			verts = new long[nVerts];
		}

		for (j=0, i=elems[current->workDimEle];
		     i<elems[current->workDimEle+1]; i++, j++) 
			verts[j] = srcV[ currentCon->connections[i] ];

		// Now have two vertex lists, in verts and vertices.
		// Say that the two vertex lists are equivalent if they 
		// have the same vertices in forward or reverse order. 
		// Return the ith index of the link list when matched;
		// otherwise, return 0.

		// Find v in vertices;
#ifdef TESTING
		fprintf( stderr, "hash table: " );
		for (i=0; i<nv; i++)	
			fprintf( stderr, "%d ", verts[i] );
		fprintf( stderr, "\nsrc: " );
		for (i=0; i<nv; i++)	
			fprintf( stderr, "%d ", vertices[i] );
		fprintf( stderr, "\n" );
#endif

		if (cxPyrConCompare(nv, verts, nv, vertices, type)) {
			//
			// If we are working with unshared edges, we want to
			// keep track of duplicated edges including those
			// that appears odd number of times so we can filter
			// them out later on and gather only the unique edges.
			//
			if(workDim == 1)
				current->count++;

			return k;
		}
	}

	return 0;
}

long hash( long *verts, long count )
{
	long i, tmp;

	tmp = 1;
	for ( i = 0; i < count; i++ )
		tmp *= verts[i] + 1;

	tmp &= 0x7fffffff;
	tmp %= HASH_TAB_LEN;
	
	return tmp;
}


//
// The workDimCon is the vertex-pointing layer of compressed dictionary
// pyramid.  Ele refers to the element number pointing to this dict pyr,
// workDimEle refers to the face number (for workDim == 2) or edge number
// (for workDim == 1) within it.  The vertices are the indices of vertices
// used to create this pyr.
//
static 
cxErrorCode insert( cxConnection *con, cxConnection *workDimCon,
		    cxPyramid *dPyr, long ele, long workDimEle, long workDim )
{
	
	static long nVerts = 16;
	static long *verts = NULL;	// Allocated once, freed @ module death
	if (!verts)
		verts = new long[nVerts];

	long i, j, k, kk;
	long n, nv;
	long *srcV;
	long *elems = workDimCon->elements;

	nv = elems[workDimEle+1] - elems[workDimEle];
	srcV = &con->connections[con->elements[ele]];

	if (nv > nVerts) {
		delete verts;
		nVerts = MIN(2*nVerts, nv);
		verts = new long[nVerts];
	}

	for (j=0, i=elems[workDimEle]; i<elems[workDimEle+1]; i++, j++) 
		verts[j] = srcV[ workDimCon->connections[i] ];

	n = hash( verts, j );

#ifdef TESTING
	fprintf( stderr,"pyr %0x, ele %d, workDimEle %d, hash() %d, verts: ", 
		dPyr, ele, workDimEle, n );
	for (i=0; i<nv; i++)
		fprintf( stderr, "%d ", verts[i] );
	fprintf( stderr, "\n" );
#endif
	hashEle *head, *prev, *purge;
	hashEle *tmp;
	hashEle **ptr = &tab[n];

#ifdef TESTING
	fprintf( stderr, "Chain before insertion.\n" );
	printChain( *ptr );
#endif
	head = *ptr;
	while (head) {
		// someone got here first.
		// look along the chain of similarly hashed entries trying
		// to find a duplicate

		// 
		// For workDim == 2 (unshared faces case), if the two hash to
		// same face, delete the found face.  Deleting when faces
		// could be shared multiple times will let all odd numbers
		// pass through.
		// For workDim == 1 (unshared edges case), if the two hash to
		// the same edge, increment the count in hashEqual and leave
		// them in the chain.
		// 
		k = hashEqual(head, nv, verts, con, workDim);
		if(k > 0) {
			if(workDim == 2) {
				// free up this found one
				if(k == 1) {
					tab[n] = head->chain;
					makeFree(head);
				}
				else {
					for(kk = 1,prev = purge = head; kk < k;
					    kk++) {
						prev = purge;
						purge = purge->chain;
					}
					prev->chain = purge->chain;
					makeFree(purge);
				}
			}

			return cx_err_none;
		}
		head = head->chain;
	}
	//
	// When we fall through it means that this face (or edge) has not been
	// hashed before. So do a real insertion into this chain.
	// First make a struct
	//
	tmp = nextFree();
	if (!tmp)
		return cx_err_error;

	tmp->pyr = dPyr;
	tmp->ele = ele;
	tmp->workDimEle = workDimEle;
	tmp->count = 1;

	//
	// Now insert it into used and chain.
	//
	tmp->chain = *ptr;
	*ptr = tmp;

#ifdef TESTING
	fprintf( stderr, "Chain after insertion.\n" );
	printChain( *ptr );
	fprintf( stderr, "\n" );
#endif
	return cx_err_none;
}


cxPyramid *exteriorPyr( cxPyramid *pyr, int unshared )
{
	long i, j, n, nDim, ne, nc, workDim;
	cxConnection *con;
	cxLattice    *lat;

	if (!pyr)
		return NULL;

	switch(unshared) {
		case 0:
			// Want to retain unshared edges.
			workDim = 1;
			break;
		case 1:
			// Want to retain unshared faces.
			workDim = 2;
			break;
		default:
			// Default to retain unshared faces.
			workDim = 2;
			break;
	}

	//
	// cxPyrActive()  method. Do the same for uncompressed or 2D compressed
	// if retain unshared faces or 1D compressed if retain unshared edges.
	//
	if (pyr->ref.compressType == cx_compress_none ||
	    DICT->nDim <= workDim) {
		//
		// Set all shared items to be skipped.
		//

#ifdef TESTING
		printf("ExtPyr: PA Method.\n");
#endif

		// Remove duplicate elements
		cxPyrMerge(pyr, 0, CX_PYRAMID_EQUIV_IDENTICALR);
		cxPyrMerge(pyr, 1, CX_PYRAMID_EQUIV_CYCLICR);
		if (pyr->count==3) cxPyrMerge(pyr, 2, CX_PYRAMID_EQUIV_SET);
		cxPyrClean(pyr, 0);

		// Get top level lattice of input pyramid
		cxLattice *topLat = NULL;
		cxPrimType primType;
		cxConnection *topCon;
		long nDataVar;

		topLat = pyr->layer[pyr->count-1].nextLattice;
		topCon = pyr->layer[pyr->count-1].relation;
		long topNo = topCon->numElements;
		long *topEle = topCon->elements;
		long *topConns = topCon->connections;
		
#ifdef TESTING
		printf("toplevel has %d elements.\n", topNo);
#endif
		
		if (topLat)
		{
			// printf("Transferring top-level lattice.\n");
			primType = topLat->data->primType;
			nDataVar = topLat->data->nDataVar;
		}

		cxPyrLayerGet( pyr, workDim, &con, NULL );
		n = con->numElements;

		// look for unshared items in the pyramid.
		// Find which items are active

		char *flg = cxPyrActive( pyr, workDim );

		if (!flg) 
			return NULL;

		for (i=0, ne=0, nc=0; i<n; i++)
		{
			if (flg[i] == 1)
			{
				ne++;
				nc += con->elements[i+1] - con->elements[i];
				// printf("flag %d == %d\n",i,flg[i]);
			}
			else
			{
				flg[i] = 0;
			}
		}

		// 
		// For unshared faces, make the output be a copy of the input
		// from layer 1 on down, with a new connection for the subset
		// of faces.  For unshared edges, make new connection for the
		// subset of edges and remove connection and lattices at layer
		// 2 and above it.
		//
		cxPyramid *newPyr = cxPyramidCopy( pyr );
		for (i=workDim + 1; i<=pyr->count; i++) {
			cxPyrLayerGet( newPyr, i, &con, &lat );
			cxDataRefDec( con );
			cxDataRefDec( lat );
		}

		newPyr->count = workDim;
		con = cxConnNew( ne, nc );

		//
		// Set lattice at top level of new pyramid
		//
		cxLattice *newLat = NULL;
		if (topLat) newLat = cxLatNew(1, &n, nDataVar, primType, 1, cx_coord_curvilinear);

		//
		// decrements old con
		//
		cxPyrLayerSet( newPyr, workDim, con, newLat);
		


		//
		// copy from pyr:con to newPyr:con, compressing 0's in flg.
		//
		cxConnection *src, *dest;
		cxPyrLayerGet( pyr, workDim, &src, NULL );
		cxPyrLayerGet( newPyr, workDim, &dest, NULL );

		for (i=0, ne=0, nc=0; i<n; i++) {
			if (!flg[i])
				continue;

			//
			// The connections
			//
			for (j=src->elements[i]; j<src->elements[i+1]; j++) {
				long k = nc + j - src->elements[i];
				dest->connections[k] = src->connections[j];
			}
			//
			// The element running count
			//
			nc += src->elements[i+1] - src->elements[i];
			dest->elements[ne+1] = nc;
			
			//
			// Copy top-level lattice
			//
			if (topLat)
			{
				// get parent element
				long p;
				int l, m;
				p = 0;
				for (l=0; l<topNo; l++)
				{
					for (m=topEle[l]; m<topEle[l+1]; m++)
					{
						if (i==topConns[m]) p = l;
					}
				}
				
				switch (primType)
				  {
				  case cx_prim_byte:
				    memmove(
					  &newLat->data->d.cx_prim_byte.values[ne*nDataVar],
					  &topLat->data->d.cx_prim_byte.values[p*nDataVar],
					  nDataVar*sizeof(unsigned char));
				    break;
				  case cx_prim_short:
				    memmove(
					  &newLat->data->d.cx_prim_short.values[ne*nDataVar],
					  &topLat->data->d.cx_prim_short.values[p*nDataVar],
					  nDataVar*sizeof(short));
				    break;
				  case cx_prim_long:
				    memmove(
					  &newLat->data->d.cx_prim_long.values[ne*nDataVar],
					  &topLat->data->d.cx_prim_long.values[p*nDataVar],
					  nDataVar*sizeof(long));
				    break;
				  case cx_prim_float:
				    memmove(
					  &newLat->data->d.cx_prim_float.values[ne*nDataVar],
					  &topLat->data->d.cx_prim_float.values[p*nDataVar],
					  nDataVar*sizeof(float));
				    break;
  				  case cx_prim_double:
				    memmove(
					  &newLat->data->d.cx_prim_double.values[ne*nDataVar],
					  &topLat->data->d.cx_prim_double.values[p*nDataVar],
					  nDataVar*sizeof(double));
				    break;
				  default:
				    break;
				  }
			}			
			ne++;
		}

		cxFree( flg );
		return newPyr;
	
	}
	else {
		// Have compression with nDim > workDim.
		// Do the preprocessed dictionary method on compressed pyramids

		// The "hash" method:
		// 1. compress each dict pyr to workDim, so have vertex list
		//    per ele at workDim
		// 2. for each compression element, hash face vertex list (if
		//    workDim == 2) or edge vertex list (if workDim == 1)
		//    marking duplicates
		// 3. scan hash list for unique faces (if workDim == 2) or
		//    unique edges (if workDim == 1), saving
		// 4. turn unique face (if workDim == 2) or unique edges (if
		//    workDim == 1) list into a compressed pyramid for
		//    output
		// 5. clean up hash table.
		// 
#ifdef TESTING
		printf("ExtPyr: Hash Method.\n");
#endif

		cxPyramidDictionary *trans;

	 	// 
	 	// Create the translation table from nDim to shrinklayer.
 		// 
	 	trans = cxPyrDictCompress( pyr->ref.dictionary, workDim );
	
		// 
		// For unshared faces, find active elements, translate each
		// into face list, find face vertices, then hash each face.
		// For unshared edges, find active elements, translate each
		// into edge list, find edge vertives, then hash each face.
		//
		
		// Get the top-layer lattice, if any (transfer to next layer)
		cxLattice *topLat = NULL;
		cxPrimType primType;
		long		nDataVar;
				
		nDim = pyr->ref.dictionary->nDim;
		cxPyrLayerGet( pyr, nDim, &con, &topLat );
		
		if (topLat)
		{
			// printf("Copying top-level lattice.\n");
			primType = topLat->data->primType;
			nDataVar = topLat->data->nDataVar;
		}
		
		n = con->numElements;

		char *flg = cxPyrActive( pyr, nDim );
		if (!flg) {
			cleanup();
			cxDataRefDec( trans );
			return NULL;
		}

		// 
		// Set up hash table.
		// 
		initTab();

		//
		// For each active element (i), for each face (if workDim == 2)
		// or each edge (if workDim == 1) in its dict entry (j), insert
		// the face or edge into the hash table.
		//
		long 	     ind, workDimEleInd, nv;
		long 	     *srcV, *workDimEleV;

		hashEle      *unique, *multi, *hashPtr;

		cxErrorCode   ec;
		cxPyramid    *elePyr, *newPyr, *refPyr;
		cxConnection *workDimCon, *newCon;

		for (i=0; i<n; i++) {
			if (!flg[i]) {
				continue;
			}

			ind = cxPyrRefIndexGet( pyr, i );
			if (ind == -1) {
				flg[i] = 0;
				continue;
			}
			elePyr = trans->table[ind];
			cxPyrLayerGet( elePyr, workDim, &workDimCon, NULL);
			for (j=0; j<workDimCon->numElements; j++)
			{
				insert( con, workDimCon, elePyr, i, j, workDim );
			}
		}

		// 
		// Now separate the hash table into unqiue and multi.
		//

		separate( unique, multi );

		// 
		// Count numbers of elements and connections in unique list.
		// 

		ne = 0;
		nc = 0;
		hashPtr = unique;
		while (hashPtr) {

			ind = cxPyrRefIndexGet( pyr, hashPtr->ele );
			elePyr = trans->table[ind];
			cxPyrLayerGet( elePyr, workDim, &workDimCon, NULL );
			ne++;
			nc += workDimCon->elements[ hashPtr->workDimEle+1 ] 
				- workDimCon->elements[ hashPtr->workDimEle ];

			hashPtr = hashPtr->next;
		}

		//
		// Make the output top lattice
		//
		cxLattice *newLat = NULL;
		if (topLat)
			newLat = cxLatNew(1, &ne, nDataVar, primType, 0, cx_coord_curvilinear);

		// 
		// Make up the output dictionary.
		// 

		newPyr = cxPyramidAlloc( workDim, cx_compress_multiple, ne );
		cxPyramidDictionarySet( newPyr, 
					trans->table[0]->ref.dictionary,
					&ec);

		newCon = cxConnNew( ne, nc );
		cxPyrLayerSet( newPyr, workDim, newCon, newLat );
		cxPyrLayerSet( newPyr, 0, NULL, pyr->baseLattice );
		
		

		// 
		// Now scan the list of unique faces or edges from hash table,
		// converting each one to a pyramid entry.
		// 

		ne = 0;
		nc = 0;

		hashPtr = unique;
		while (hashPtr) {

			// the vertex indices in original pyramid
			srcV = &con->connections[con->elements[hashPtr->ele]];


			// the top-level lattice data from the original pyramid
			if (topLat)
			switch (primType)
			{
			case cx_prim_byte:
			  memmove(
				  &newLat->data->d.cx_prim_byte.values[ne*nDataVar],
				  &topLat->data->d.cx_prim_byte.values[hashPtr->ele*nDataVar],
				  nDataVar*sizeof(unsigned char));
			  break;
			case cx_prim_short:
			  memmove(
				  &newLat->data->d.cx_prim_short.values[ne*nDataVar],
				  &topLat->data->d.cx_prim_short.values[hashPtr->ele*nDataVar],
				  nDataVar*sizeof(short));
			  break;
			case cx_prim_long:
			  memmove(
				&newLat->data->d.cx_prim_long.values[ne*nDataVar],
				&topLat->data->d.cx_prim_long.values[hashPtr->ele*nDataVar],
				nDataVar*sizeof(long));
			  break;
			case cx_prim_float:
			  memmove(
				&newLat->data->d.cx_prim_float.values[ne*nDataVar],
				&topLat->data->d.cx_prim_float.values[hashPtr->ele*nDataVar],
				nDataVar*sizeof(float));
			  break;
			case cx_prim_double:
			  memmove(
				&newLat->data->d.cx_prim_double.values[ne*nDataVar],
				&topLat->data->d.cx_prim_double.values[hashPtr->ele*nDataVar],
				nDataVar*sizeof(double));
			  break;
			default:
			  break;
			}
			
			// find the relevant face or edge from the ele in
			// original pyramid
			ind = cxPyrRefIndexGet( pyr, hashPtr->ele );
			elePyr = trans->table[ind];
			cxPyrLayerGet( elePyr, workDim, &workDimCon, NULL );

			// the vertex indices for face or edges, relative to
			// srcV
			workDimEleV = &workDimCon->connections[
						workDimCon->elements[
							hashPtr->workDimEle ] ];

			nv = workDimCon->elements[ hashPtr->workDimEle+1 ]
				- workDimCon->elements[ hashPtr->workDimEle ];

			// add to new connection
			// how many vertices, their indices
			// and the relevant dictionary index.
			//
			refPyr = trans->table[ind];
			workDimEleInd = cxPyrRefIndexGet( refPyr, hashPtr->workDimEle );
			newPyr->ref.r.cx_compress_multiple.indices[ ne ] = workDimEleInd;

			for (i=0; i<nv; i++)
				newCon->connections[nc+i] =srcV[ workDimEleV[i] ];
				
			nc += nv;
			newCon->elements[++ne] = nc;

			// next...
			hashPtr = hashPtr->next;
		}

		// 
		// Clean up, ie delete, the hash table storage and flags
		//
		
		
		cleanup();
		cxFree( flg );
		cxDataRefDec( trans );

		return newPyr;
	}
}


#ifdef __cplusplus
}
#endif
