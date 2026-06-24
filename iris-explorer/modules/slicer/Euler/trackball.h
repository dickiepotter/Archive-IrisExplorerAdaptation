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
 * trackball.h
 * A virtual trackball implementation
 * Written by Gavin Bell for Silicon Graphics, November 1988.
 */

typedef float Matrix[4][4];

#include "vect.h"

/*
 *	Pass the x and y coordinates of the last and current positions of
 * the mouse, scaled so they are from (-1.0 ... 1.0).
 *
 * if ox,oy is the window's center and sizex,sizey is its size, then
 * the proper transformation from screen coordinates (sc) to world
 * coordinates (wc) is:
 * wcx = (2.0 * (scx-ox)) / (float)sizex - 1.0
 * wcy = (2.0 * (scy-oy)) / (float)sizey - 1.0
 *
 * For a really easy interface to this see 'ui.h'.
 */
void
trackball(float *, float, float, float, float *, float *, float *);

/*
 *	Given two sets of Euler paramaters, add them together to get an
 * equivalent third set.  When incrementally adding them, the first
 * argument here should be the new rotation, the secon and third the
 * total rotation (which will be over-written with the resulting new
 * total rotation).
 */
void
add_eulers(float *, float *, float *);

/*
 *	A useful function, builds a rotation matrix in Matrix based on
 * given Euler paramaters.
 */
void
build_rotmatrix(Matrix, float *);

/*
 * This function computes the Euler paramaters given an xyz axis (the
 * first argument, 3 floats) and angle (expressed in radians, the
 * second argument).  The result is put into the third argument, which
 * must be an array of 4 floats.
 */
void
axis_to_euler(float *, float, float *);
