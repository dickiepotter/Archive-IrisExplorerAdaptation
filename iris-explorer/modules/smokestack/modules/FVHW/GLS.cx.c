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
 * Module generic routines subprogram for module GLS.
 * Automatically created by module builder at Mon Aug 23 18:05:38 2004.
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
#include <cx/cxParameter.api.h>
#include <cx/cxPyramid.api.h>

/* No user specified include files. */


#ifdef __cplusplus
	extern "C" {
#endif
/*****************************************************/

/* Entry points for MCW calling. */
extern void FV_main(void);

cxFuncTable cx_FuncTable[] = {
		 (cxFuncTable)FV_main,
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
DLL_IMPORT extern cxMetaType cx_cxParameterMetaType;
DLL_IMPORT extern cxMetaType cx_cxPyramidMetaType;

extern
void cxModuleMetaTypeInit(void) 
{
    int major_version = 5;
    int minor_version = 0;
    /* verify and install  Explorer Version Numbers */
    cxVerifyVersion(major_version, minor_version);

    cxMetaTypeAdd(&cx_cxMetaTypeMetaType);
    cxMetaTypeAdd(&cx_cxParameterMetaType);
    cxMetaTypeAdd(&cx_cxPyramidMetaType);

}

/*****************************************************/

/* Entry points for Hook Function calling. */

cxHookTable cx_HookTable[] = {
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
  if (strcmp( "GLS", cxModuleNameGet()))
    return cx_err_none;

  /* Storage to determine when done port type checking. */
  if (!checkedPort) {
    checkedPort = (int*) calloc(20, sizeof(int));
    if (!checkedPort) {
      cxModAlert( "Unable to allocate space for type checking." );
      return cx_err_error;
    }
  }
  /* Get ready for first port. */
  linkInd = 0;
  /* Checking contents of Input datasets. */

  /* No typing information for port 0. */
  thisPort = 0;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 1. */
  thisPort = 1;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 2. */
  thisPort = 2;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 3. */
  thisPort = 3;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
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


  /* No typing information for port 13. */
  thisPort = 13;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 14. */
  thisPort = 14;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 15. */
  thisPort = 15;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 16. */
  thisPort = 16;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 17. */
  thisPort = 17;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 18. */
  thisPort = 18;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    ;
  }


  /* No typing information for port 19. */
  thisPort = 19;
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
  if (strcmp( "GLS", cxModuleNameGet()))
    return cx_err_none;

  /* Storage to determine when done port type checking. */
  if (!checkedPort) {
    checkedPort = (int*) calloc(1, sizeof(int));
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
      nm = cxOutputTypeGet(cxOutputPortOpen("Output"));
      if (strcmp(nm, "cxPyramid")) {
        msg = cxTypeMismatchMsg(cx_port_output, "Output", "cxPyramid", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxPyramid") != 0) {
      nm = cxDataTypeNameGet(((cxPyramid *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_output, "Output", "cxPyramid", nm);
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
    portOK = (((cxLattice *) 0) <= ((cxPyramid *) cxChkDat)->baseLattice);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "0";
      hiStrings[count++] = "";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxPyramid *) cxChkDat)->baseLattice );
      portData = buf;
      msg = cxMultTypeMismatchPtrMsg( 
		    cx_port_output, 
		    "Output", 
		    "cxPyramid", 
		    "Base Lattice", 
		    "->baseLattice", 
		    portData, 
                1, 
                loStrings, hiStrings );

      if (loStrings){
        free((void *)loStrings);
      }
      if (hiStrings){
        free((void *)hiStrings);
      }

      cxOutputDataRelease( cxMDWPortID[0] );
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
    portOK = (3 == ((cxPyramid *) cxChkDat)->count);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "3";
      hiStrings[count++] = "3";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxPyramid *) cxChkDat)->count );
      portData = buf;
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Output", 
		    "cxPyramid", 
		    "Num Levels", 
		    "->count", 
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

      cxOutputDataRelease( cxMDWPortID[0] );
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
    portOK = !((cxPyramid *) cxChkDat)->ref.dictionary || 
	(3 == ((cxPyramid *) cxChkDat)->ref.dictionary->nDim);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "3";
      hiStrings[count++] = "3";
    }
    if (!succeed) {
      sprintf( buf, "%ld", (long)((cxPyramid *) cxChkDat)->ref.dictionary->nDim );
      portData = buf;
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Output", 
		    "cxPyramid", 
		    "Num Dims", 
		    "->ref.dictionary->nDim", 
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

      cxOutputDataRelease( cxMDWPortID[0] );
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
    portOK = (cx_compress_none <= ((cxPyramid *) cxChkDat)->ref.compressType 
                         && ((cxPyramid *) cxChkDat)->ref.compressType<= cx_compress_multiple);
    if (portOK) {
      succeed = 1;
    } else {
      loStrings[count  ] = "cx_compress_none";
      hiStrings[count++] = "cx_compress_multiple";
    }
    if (!succeed) {
      portData = cxEnumNameOf( ((cxPyramid *) cxChkDat)->ref.compressType, 
				"cxCompressType" );
    if ( !portData ) {
      sprintf( buf, "%ld", (long)((cxPyramid *) cxChkDat)->ref.compressType );
      portData = buf;
    }
      msg = cxMultTypeMismatchPortMsg( 
		    cx_port_output, 
		    "Output", 
		    "cxPyramid", 
		    "Compression Type", 
		    "->ref.compressType", 
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

      cxOutputDataRelease( cxMDWPortID[0] );
      cxModAlert( msg );
      return cx_err_error;
      }
  }

  return cx_err_none;
}

#ifdef __cplusplus
}
#endif

