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

/* 
 * Module data wrapper subprogram for module Render.
 * Automatically created by module builder at Thu Aug 26 16:13:54 2004.
 * 
 */


/* System include files. */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* include files from $EXPLORERHOME/include  */
#include <cx/Typedefs.h>
#include <cx/ModuleCommand.h>
#include <cx/DataOps.h>
#include <cx/PortAccess.h>
#include <cx/Coerce.h>
#include <cx/cxMetaType.h>
#include <cx/TypeMismatchMsg.h>
#include <cx/UserFuncs.h>
#include <cx/cxGeneric.h>
#include <cx/UDT.h>
#include <cx/Info.h>
#include <cx/DataAccess.h>
#include <cx/cxGeometry.api.h>
#include <cx/cxParameter.api.h>
#include <cx/cxLattice.api.h>
#include <cx/cxPick.api.h>

/* No user specified include files. */



  /* --- User Fcn call prototype. --- */
#include <cx/cxGeometry.api.h>
#include <cx/cxParameter.api.h>
#include <cx/cxLattice.api.h>
#include <cx/cxPick.api.h>

#ifdef __cplusplus
	extern "C" {
#endif

void render ( void  );

#ifdef __cplusplus
}
#endif


/* Number of user function arguments. */
#define cxMDWNARGrender 1

long cxMDWrender( void )
{

/* ----- Beginning of port declarations ----- */

/* Port dataset variable declarations */

  cxGeometry *cxP_In_Input;
  cxGeometry *cxP_In_Annotation;
  cxGeometry *cxP_In_Screen;
  cxGeometry *cxP_In_Input_Camera;
  cxParameter *cxP_In_Snap;
  cxParameter *cxP_In_Snap_On_Redraw;
  cxParameter *cxP_In_Window;
  cxParameter *cxP_In_Background_Color;
  cxParameter *cxP_In_Viewing;
  cxParameter *cxP_In_UserPickMode;
  cxParameter *cxP_In_Camera_update;
  cxParameter *cxP_In_Decoration;
  cxParameter *cxP_In_Save_camera;
  cxGeometry *cxP_Out_Output_Camera;
  cxGeometry *cxP_Out_Picked_Geometry;
  cxParameter *cxP_Out_Sync;
  cxLattice *cxP_Out_Snapshot;
  cxPick *cxP_Out_Pick;

/* --- End of port and dataset declarations ---- */


/* -- Beginning of User Fcn variable declarations -- */


/* ------ End of User Fcn declarations ------ */

  long   cxMDWi, cxMDWIrc;
  long   cxMDWIfCoerce[ cxMDWNARGrender ];
  char *cxMDWStr;
  long  cxMDWFuncRetCode;
  char  cxMDWFuncBuf[256];
  void *cxMDWObject;
  long  cxMDWLen;
  long  cxMDWPortIDIn[13];
  long  cxMDWPortIDOut[5];
  void *tmp;
  int  *cxMDWTypeTab;
  cxDataCheckErrorInfo  *cxMDWCheckErr;
  cxErrorCode   ec;
/* ------------------------------------------ */
/* ----- Beginning of Port Associations ----- */

/* -------- First executable statment ------- */


  /* Get input port handles. */ 
  cxMDWPortIDIn[0] = cxInputPortOpen( "Input" );
  cxMDWPortIDIn[1] = cxInputPortOpen( "Annotation" );
  cxMDWPortIDIn[2] = cxInputPortOpen( "Screen" );
  cxMDWPortIDIn[3] = cxInputPortOpen( "Input Camera" );
  cxMDWPortIDIn[4] = cxInputPortOpen( "Snap" );
  cxMDWPortIDIn[5] = cxInputPortOpen( "Snap On Redraw" );
  cxMDWPortIDIn[6] = cxInputPortOpen( "Window" );
  cxMDWPortIDIn[7] = cxInputPortOpen( "Background Color" );
  cxMDWPortIDIn[8] = cxInputPortOpen( "Viewing" );
  cxMDWPortIDIn[9] = cxInputPortOpen( "UserPickMode" );
  cxMDWPortIDIn[10] = cxInputPortOpen( "Camera update" );
  cxMDWPortIDIn[11] = cxInputPortOpen( "Decoration" );
  cxMDWPortIDIn[12] = cxInputPortOpen( "Save camera" );

  /* Get output port handles. */ 
  cxMDWPortIDOut[0] = cxOutputPortOpen( "Output Camera" );
  cxMDWPortIDOut[1] = cxOutputPortOpen( "Picked Geometry" );
  cxMDWPortIDOut[2] = cxOutputPortOpen( "Sync" );
  cxMDWPortIDOut[3] = cxOutputPortOpen( "Snapshot" );
  cxMDWPortIDOut[4] = cxOutputPortOpen( "Pick" );

  /* Get dataset pointers. */ 
  cxP_In_Input = ( cxGeometry *) cxInputDataGet( cxMDWPortIDIn[0] );
  cxP_In_Annotation = ( cxGeometry *) cxInputDataGet( cxMDWPortIDIn[1] );
  cxP_In_Screen = ( cxGeometry *) cxInputDataGet( cxMDWPortIDIn[2] );
  cxP_In_Input_Camera = ( cxGeometry *) cxInputDataGet( cxMDWPortIDIn[3] );
  cxP_In_Snap = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[4] );
  cxP_In_Snap_On_Redraw = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[5] );
  cxP_In_Window = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[6] );
  cxP_In_Background_Color = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[7] );
  cxP_In_Viewing = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[8] );
  cxP_In_UserPickMode = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[9] );
  cxP_In_Camera_update = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[10] );
  cxP_In_Decoration = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[11] );
  cxP_In_Save_camera = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[12] );

  /* Initialize the coercion status flags. */
  for (cxMDWi=0; cxMDWi< cxMDWNARGrender; cxMDWi++) 
    cxMDWIfCoerce[cxMDWi] = TRUE;


  /* Assign user function values. */
  /* Extracting User Function variables from ports. */
  /* Assigning constant User Function variables. */
  /* Assigning whether data has changed to User Function variables. */
  /* Allocation of User Function variables. */

  cxEnterUserCode( 0 );

  render(  );

  cxExitUserCode();

  if (cxDataAllocErrorGet())
    return( cx_err_error );

/* Beginning of output port assembly. */

  /* Output port assembly for port "Output Camera". */

  cxP_Out_Output_Camera = NULL;

  /* Output port assembly for port "Picked Geometry". */

  cxP_Out_Picked_Geometry = NULL;

  /* Output port assembly for port "Sync". */

  cxP_Out_Sync = NULL;

  /* Output port assembly for port "Snapshot". */

  cxP_Out_Snapshot = NULL;

  /* Output port assembly for port "Pick". */

  cxP_Out_Pick = NULL;

/* End of output port assembly. */


/* Free temporarily assigned user vars. */

  if ( cxP_Out_Output_Camera )
    cxOutputDataSet( cxMDWPortIDOut[0], (void *)cxP_Out_Output_Camera );
  if ( cxP_Out_Picked_Geometry )
    cxOutputDataSet( cxMDWPortIDOut[1], (void *)cxP_Out_Picked_Geometry );
  if ( cxP_Out_Sync )
    cxOutputDataSet( cxMDWPortIDOut[2], (void *)cxP_Out_Sync );
  if ( cxP_Out_Snapshot )
    cxOutputDataSet( cxMDWPortIDOut[3], (void *)cxP_Out_Snapshot );
  if ( cxP_Out_Pick )
    cxOutputDataSet( cxMDWPortIDOut[4], (void *)cxP_Out_Pick );
  return( cx_err_none );
}
