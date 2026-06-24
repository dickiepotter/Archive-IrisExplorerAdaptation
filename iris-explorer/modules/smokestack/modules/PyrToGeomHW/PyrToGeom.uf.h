/* COPYRIGHT_BEGIN
 *    Copyright (c) 1991       SGI   All Rights Reserved
 *    Copyright (c) 1994-2000  The Numerical Algorithms Group Ltd.,
 *                             Oxford, U.K.  All Rights Reserved
 *    THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF SGI
 *    The copyright notice above does not evidence any
 *    actual or intended publication of such source code,
 *    and is an unpublished work by Silicon Graphics, Inc.
 *    This material contains CONFIDENTIAL INFORMATION that
 *    is the property of Silicon Graphics, Inc. Any use,
 *    duplication or disclosure not specifically authorized
 *    by Silicon Graphics is strictly prohibited.
 *    
 *    RESTRICTED RIGHTS LEGEND:
 *    
 *    Use, duplication or disclosure by the Government is
 *    subject to restrictions as set forth in subdivision
 *    (c)(1)(ii) of the Rights in Technical Data and Computer
 *    Software clause at DFARS 52.227-7013, and/or in similar
 *    or successor clauses in the FAR, DOD or NASA FAR
 *    Supplement.  Unpublished- rights reserved under the
 *    Copyright Laws of the United States.  Contractor is
 *    SILICON GRAPHICS, INC., 2011 N. Shoreline Blvd.,
 *    Mountain View, CA 94039-7311
 * COPYRIGHT_END */

#ifndef _PyrToGeom_UF_H_
#define _PyrToGeom_UF_H_

/* 
 * User function prototypes subprogram for module PyrToGeom.
 * Automatically created by module builder at Wed Aug 18 12:11:57 2004.
 * 
 */

  /* --- User Fcn call prototype. --- */
#include <cx/cxPyramid.api.h>
#include <cx/cxLattice.api.h>
#include <cx/cxParameter.api.h>
#include <cx/cxGeometry.api.h>

#ifdef __cplusplus
	extern "C" {
#endif

void pyr2geom ( void  );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
	extern "C" {
#endif
/*****************************************************/

/* Entry points for Hook Function calling. */
extern void pyr2geom_init(void);

#ifdef __cplusplus
}
#endif

#endif
