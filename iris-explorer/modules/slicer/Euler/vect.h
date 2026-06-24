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
 *	Definition for vector math.  Vectors are just arrays of 3 floats.
 */

#ifndef VECTDEF
#define VECTDEF

#ifndef _POLY9
#include <math.h>
#endif

float *vnew(void);
float *vclone(float *v);
void vcopy(float *, float *);
void vprint(float *);
void vset(float *, float, float, float);
void vzero(float *);
void vnormal(float *);
double vlength(float *);
void vscale(float *, float);
void vmult(float *, float *, float *);
void vadd(float *, float *, float *);
void vsub(float *, float *, float *);
void vhalf(float *, float *, float *);
double vdot(float *, float *);
void vcross(float *, float *, float *);
void vdirection(float *, float *);
void vreflect(float *, float *, float *);
void vmultmatrix(float [4][4], float [4][4], float [4][4]);
#endif /* VECTDEF */
