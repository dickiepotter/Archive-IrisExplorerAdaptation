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

/*
 *	Implementation of a virtual trackball.  See trackball.h for the
 * interface to these routines.
 *	Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 * the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 */

/* modified Aug 2003 to work on vector component inputs HW */

#include <stdio.h>
#include <math.h>
#include "trackball.h"

#ifndef __sgi
# define	fcos(x)		cos((double)x)
# define	fsin(x)		sin((double)x)
# define	fsqrt(x)	sqrt((double)x)
#endif

#if defined(WIN32)
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#define M_PI		3.14159265358979323846
#endif

void normalize_euler(float *);

/*
 * Given the vector components,
 * figure out the axis of rotation, which is the cross
 * product of P1 P2  
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void
trackball(float *e, float p1x, float p1y, float p1z, float *p2x, float *p2y, float *p2z)
{
	float p1[3];
	float p2[3];
	float d[3];
	float a[3];	/* Axis of rotation */
      float s;
      float p12;

	vzero(a);

/*
 * First, fill in coordinates 
 */
	vset(p1, p1x, p1y, p1z);
	vset(p2, *p2x, *p2y, *p2z);

/*
 * Now, we want the cross product of P1 and P2
 * (Or cross product of p2 and p1... this was determined by trial and
 * error, so there may be a compensating mathematical boo-boo
 * somewhere else).
 */
	vcross(p2, p1, a);

/* finger trouble at the interactor - return zero rotn,  */
/* reset the values and it should sort itself out at the next round */
	if (vlength(a) < 0.000001)
	{
		vzero(e);	/* Zero rotation */
		e[3] = 1.0;
            *p2x = p1x;
            *p2y = p1y;
            *p2z = p1z;
		return ;
	}

/*
 *	Given two axes, compute rotation and generate euler paramaters
 */
	vsub(p1, p2, d);
      s = vlength(p1)+vlength(p2)+vlength(d);
      s = s/2.0;
      p12 = vlength(p1)*vlength(p2);
      vnormal(a);	/* Normalize axis to rotate around */
	vcopy(a, e);
	vscale(e, fsqrt((s-vlength(p2))*(s-vlength(p1))/p12));
	e[3] = fsqrt(s*(s-vlength(d))/p12);
}


/*
 *	Given two rotations, e1 and e2, expressed as Euler paramaters,
 * figure out the equivalent single rotation and stuff it into dest.
 * 
 * This routine also normalizes the result every COUNT times it is
 * called, to keep error from creeping in.
 */
#define COUNT 100
void
add_eulers(float *e1, float *e2, float *dest)
{
	static int count=0;
	register int i;
	float t1[3], t2[3], t3[3];
	float tf[4];

	vcopy(e1, t1); vscale(t1, e2[3]);
	vcopy(e2, t2); vscale(t2, e1[3]);
	vcross(e2, e1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = e1[3] * e2[3] - vdot(e1, e2);

	for (i = 0 ; i < 4 ;i++)
	{
		dest[i] = tf[i];
	}

	if (++count > COUNT)
	{

		count = 0;
		normalize_euler(dest);
	}
}

/*
 * Euler paramaters always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * We'll normalize based on this formula.  Also, normalize greatest
 * component, to avoid problems that occur when the component we're
 * normalizing gets close to zero (and the other components may add up
 * to more than 1.0 because of rounding error).
 */
void
normalize_euler(float *e)
{	/* Normalize result */
#if 0
	int which, i;
	float gr;

	which = 0;
	gr = e[which];
	for (i = 1 ; i < 4 ; i++)
	{
		if (fabs(e[i]) > fabs(gr)) {
			gr = e[i];
			which = i;
		}
	}

	e[which] = 0.0;

	e[which] = fsqrt(1.0 - (e[0]*e[0] + e[1]*e[1] +
		e[2]*e[2] + e[3]*e[3]));

	/* Check to see if we need negative square root */
	if (gr < 0.0)
		e[which] = -e[which];
#else
	float	len = 0.0;
	int	i;

	for (i = 0; i < 4; i++)
		len += e[i] * e[i];
	
	len = sqrt(len);

	for (i = 0; i < 4; i++)
		e[i] /= len;
#endif
}

/*
 * Build a rotation matrix, given Euler paramaters.
 */
void
build_rotmatrix(Matrix m, float *e)
{
	m[0][0] = 1 - 2.0 * (e[1] * e[1] + e[2] * e[2]);
	m[0][1] = 2.0 * (e[0] * e[1] - e[2] * e[3]);
	m[0][2] = 2.0 * (e[2] * e[0] + e[1] * e[3]);
	m[0][3] = 0.0;

	m[1][0] = 2.0 * (e[0] * e[1] + e[2] * e[3]);
	m[1][1] = 1 - 2.0 * (e[2] * e[2] + e[0] * e[0]);
	m[1][2] = 2.0 * (e[1] * e[2] - e[0] * e[3]);
	m[1][3] = 0.0;

	m[2][0] = 2.0 * (e[2] * e[0] - e[1] * e[3]);
	m[2][1] = 2.0 * (e[1] * e[2] + e[0] * e[3]);
	m[2][2] = 1 - 2.0 * (e[1] * e[1] + e[0] * e[0]);
	m[2][3] = 0.0;

	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
}
