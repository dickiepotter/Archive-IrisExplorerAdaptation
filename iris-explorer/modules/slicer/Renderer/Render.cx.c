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
 * Module generic routines subprogram for module Render.
 * Automatically created by module builder at Fri Dec 13 16:00:14 2002.
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


#ifdef __cplusplus
	extern "C" {
#endif
/*****************************************************/

/* Entry points for MCW calling. */
extern long cxMDWrender(void);

cxFuncTable cx_FuncTable[] = {
		 (cxFuncTable)cxMDWrender,
		 NULL
};

#define DLL_IMPORT __declspec(dllimport)
extern int cxVerifyVersion(int, int);
extern
const char * cxMultTypeMismatchPtrMsg(
       cxPortDir,const char *,const char *,const char *,
       const char *,const char *,
       int,char const*const*,char const*const*);
extern
const char * cxMultTypeMismatchPortMsg(
       cxPortDir,const char *,const char *,const char *,
       const char *,const char *,
       int,char const*const*,char const*const*);

DLL_IMPORT
extern cxMetaType cx_cxMetaTypeMetaType;
DLL_IMPORT extern cxMetaType cx_cxGeometryMetaType;
DLL_IMPORT extern cxMetaType cx_cxParameterMetaType;
DLL_IMPORT extern cxMetaType cx_cxLatticeMetaType;
DLL_IMPORT extern cxMetaType cx_cxPickMetaType;

extern
void cxModuleMetaTypeInit(void) 
{
    int major_version = 5;
    int minor_version = 0;
    /* verify and install  Explorer Version Numbers */
    cxVerifyVersion(major_version, minor_version);

    cxMetaTypeAdd(&cx_cxMetaTypeMetaType);
    cxMetaTypeAdd(&cx_cxGeometryMetaType);
    cxMetaTypeAdd(&cx_cxParameterMetaType);
    cxMetaTypeAdd(&cx_cxLatticeMetaType);
    cxMetaTypeAdd(&cx_cxPickMetaType);

}

/*****************************************************/

/* Entry points for Hook Function calling. */
extern void render_init(void);
extern void render_disconnect_input(const char *, int);
extern void render_connect_output(const char *, int);
extern void render_disconnect_output(const char *, int);

cxHookTable cx_HookTable[] = {
    { cx_func_init, (cxVoidHookFunc)render_init },
    { cx_func_disconnect_input, (cxVoidHookFunc)render_disconnect_input },
    { cx_func_connect_output, (cxVoidHookFunc)render_connect_output },
    { cx_func_disconnect_output, (cxVoidHookFunc)render_disconnect_output },
    { cx_func_last}
};
/*****************************************************/
/* Check Input ports against constraints in module resources. */
extern
cxErrorCode cxPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) 
{
  cxErrorCode   ec;
  int           portOK;
  int           succeed;
  int           count;
  void         *cxChkDat;
  char          buf[128];
  const char   *portData, *nm, *msg;
  const char  **loStrings = NULL;
  const char  **hiStrings = NULL;
  int           linkInd, thisPort;
  static int   *checkedPort = NULL;


  /* Can't do port checking on alternate executables. */
  if (strcmp( "Render", cxModuleNameGet()))
    return cx_err_none;

  /* Storage to determine when done port type checking. */
  if (!checkedPort) {
    checkedPort = (int*) calloc(13, sizeof(int));
    if (!checkedPort) {
      cxModAlert( "Unable to allocate space for type checking." );
      return cx_err_error;
    }
  }
  /* Get ready for first port. */
  linkInd = 0;
  /* Checking contents of Input datasets. */
  thisPort = 0;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check optional port when it has data 
     * Only check the port when the data has changed,
     * since bogus data was released last time around.
     */
    if (!cxChkDat || !cxInputDataChanged(thisPort)) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxInputTypeGet(cxInputPortOpen("Input"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_input, "Input", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_input, "Input", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }
  thisPort = 1;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check optional port when it has data 
     * Only check the port when the data has changed,
     * since bogus data was released last time around.
     */
    if (!cxChkDat || !cxInputDataChanged(thisPort)) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxInputTypeGet(cxInputPortOpen("Annotation"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_input, "Annotation", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_input, "Annotation", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }
  thisPort = 2;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check optional port when it has data 
     * Only check the port when the data has changed,
     * since bogus data was released last time around.
     */
    if (!cxChkDat || !cxInputDataChanged(thisPort)) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxInputTypeGet(cxInputPortOpen("Screen"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_input, "Screen", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_input, "Screen", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }
  thisPort = 3;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check optional port when it has data 
     * Only check the port when the data has changed,
     * since bogus data was released last time around.
     */
    if (!cxChkDat || !cxInputDataChanged(thisPort)) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxInputTypeGet(cxInputPortOpen("Input Camera"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_input, "Input Camera", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_input, "Input Camera", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }

  /* No typing information for port 4. */
  thisPort = 4;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 5. */
  thisPort = 5;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 6. */
  thisPort = 6;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 7. */
  thisPort = 7;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 8. */
  thisPort = 8;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 9. */
  thisPort = 9;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 10. */
  thisPort = 10;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 11. */
  thisPort = 11;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 12. */
  thisPort = 12;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  return cx_err_none;
}
/*****************************************************/
/* Check Output ports against constraints in module resources. */
extern
cxErrorCode cxPortCheckOut ( int linkC, void *links[], long *cxMDWPortID ) 
{
  cxErrorCode   ec;
  int           portOK;
  int           succeed;
  int           count;
  void         *cxChkDat;
  char          buf[128];
  const char   *portData, *nm, *msg;
  const char  **loStrings = NULL;
  const char  **hiStrings = NULL;
  int           linkInd, thisPort;
  static int   *checkedPort = NULL;


  /* Can't do port checking on alternate executables. */
  if (strcmp( "Render", cxModuleNameGet()))
    return cx_err_none;

  /* Storage to determine when done port type checking. */
  if (!checkedPort) {
    checkedPort = (int*) calloc(5, sizeof(int));
    if (!checkedPort) {
      cxModAlert( "Unable to allocate space for type checking." );
      return cx_err_error;
    }
  }
  /* Get ready for first port. */
  linkInd = 0;
  /* Checking contents of Output datasets. */
  thisPort = 0;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check output port when it has data. */
    if (!cxChkDat) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxOutputTypeGet(cxOutputPortOpen("Output Camera"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_output, "Output Camera", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_output, "Output Camera", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }
  thisPort = 1;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check output port when it has data. */
    if (!cxChkDat) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxOutputTypeGet(cxOutputPortOpen("Picked Geometry"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_output, "Picked Geometry", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_output, "Picked Geometry", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }

  /* No typing information for port 2. */
  thisPort = 2;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }

  thisPort = 3;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check output port when it has data. */
    if (!cxChkDat) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxOutputTypeGet(cxOutputPortOpen("Snapshot"));
      if (strcmp(nm, "cxLattice")) {
        msg = cxTypeMismatchMsg(cx_port_output, "Snapshot", "cxLattice", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxLattice") != 0) {
      nm = cxDataTypeNameGet(((cxLattice *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_output, "Snapshot", "cxLattice", nm);
      cxModAlert( msg );
      return cx_err_error;
    }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = (((cxData *) 1) <= ((cxLattice *) cxChkDat)->data);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "1";
      hiStrings[count++] = "";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->data );
      portData = buf;
      msg = cxMultTypeMismatchPtrMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Data Structure", 
		    "->data", 
		    portData, 
                1, 
                loStrings, hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = (((cxCoord *) 0) <= ((cxLattice *) cxChkDat)->coord);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "0";
      hiStrings[count++] = "";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->coord );
      portData = buf;
      msg = cxMultTypeMismatchPtrMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Coord Structure", 
		    "->coord", 
		    portData, 
                1, 
                loStrings, hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = (2 == ((cxLattice *) cxChkDat)->nDim);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "2";
      hiStrings[count++] = "2";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->nDim );
      portData = buf;
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Num Dimensions", 
		    "->nDim", 
		    portData, 
                1, 
		    loStrings, 
		    hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = !((cxLattice *) cxChkDat)->data || 
	(3 == ((cxLattice *) cxChkDat)->data->nDataVar);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "3";
      hiStrings[count++] = "3";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->data->nDataVar );
      portData = buf;
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Num Data Variables", 
		    "->data->nDataVar", 
		    portData, 
                1, 
		    loStrings, 
		    hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = !((cxLattice *) cxChkDat)->data || 
	(cx_prim_byte == ((cxLattice *) cxChkDat)->data->primType);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "cx_prim_byte";
      hiStrings[count++] = "cx_prim_byte";
    }
    if (!succeed) {
      portData = cxEnumNameOf( ((cxLattice *) cxChkDat)->data->primType, 
				"cxPrimType" );
    if ( !portData ) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->data->primType );
      portData = buf;
    }
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Primitive Data Type", 
		    "->data->primType", 
		    portData, 
                1, 
		    loStrings, 
		    hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = !((cxLattice *) cxChkDat)->coord || 
	(cx_coord_uniform == ((cxLattice *) cxChkDat)->coord->coordType);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "cx_coord_uniform";
      hiStrings[count++] = "cx_coord_uniform";
    }
    if (!succeed) {
      portData = cxEnumNameOf( ((cxLattice *) cxChkDat)->coord->coordType, 
				"cxCoordType" );
    if ( !portData ) {
      sprintf( buf, "%ld", (long)((cxLattice *) cxChkDat)->coord->coordType );
      portData = buf;
    }
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Coord Type", 
		    "->coord->coordType", 
		    portData, 
                1, 
		    loStrings, 
		    hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }

    /* check on port constraints */
     succeed = 0;
     count   = 0;

     if (loStrings != NULL) {
       free((void *)loStrings);
     }
     loStrings = (const char**)calloc(1, sizeof(const char*));
     if (hiStrings != NULL) {
       free((void *)hiStrings);
     }
     hiStrings = (const char**)calloc(1, sizeof(const char*));
   if (((cxLattice *) cxChkDat)->coord && ((cxLattice *) cxChkDat)->coord->coordType == cx_coord_curvilinear) {

    /* The inverted logic (!x ||) says succeed if NIL or proceed. */
    portOK = !((cxLattice *) cxChkDat)->coord || 
	(1 <= *( (signed long *)  cxCoordNumCoordDimsGet( ((cxLattice *) cxChkDat)->coord ) ));
   } else {
     portOK = 1;
   }
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "1";
      hiStrings[count++] = "";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)*( (signed long *)  cxCoordNumCoordDimsGet( ((cxLattice *) cxChkDat)->coord ) ) );
      portData = buf;
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Snapshot", 
		    "cxLattice", 
		    "Num Coord Dimensions", 
		    "->coord->c.cx_coord_curvilinear.nCoordVar", 
		    portData, 
                1, 
		    loStrings, 
		    hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[3] );
      cxModAlert( msg );
      return cx_err_error;
      }
  }
  thisPort = 4;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];
    /* Only check output port when it has data. */
    if (!cxChkDat) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxOutputTypeGet(cxOutputPortOpen("Pick"));
      if (strcmp(nm, "cxPick")) {
        msg = cxTypeMismatchMsg(cx_port_output, "Pick", "cxPick", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxPick") != 0) {
      nm = cxDataTypeNameGet(((cxPick *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_output, "Pick", "cxPick", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }

  return cx_err_none;
}

#ifdef __cplusplus
}
#endif

