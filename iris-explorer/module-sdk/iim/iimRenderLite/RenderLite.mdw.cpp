#include "stdafx.h"

extern "C" void renderlite 
(
	 cxGeometry *	  geo, 
	 int    	  geoFlag, 
	 long   	  window, 
	 int    	  windowFlag 
);


#define cxMDWNARGrenderlite 4

extern "C" long cxMDWrenderlite( void )
{
  cxGeometry *cxP_In_geometry;
  cxParameter *cxP_In_window;

  cxGeometry * geo; 
  int     geoFlag; 
  long    window; 
  int     windowFlag; 

  long   cxMDWi, cxMDWIrc;
  long   cxMDWIfCoerce[ cxMDWNARGrenderlite ];
  char *cxMDWStr;
  long  cxMDWFuncRetCode;
  char  cxMDWFuncBuf[256];
  void *cxMDWObject;
  long  cxMDWLen;
  long  cxMDWPortIDIn[2];
  long  cxMDWPortIDOut[1];
  void *tmp;
  int  *cxMDWTypeTab;
  cxDataCheckErrorInfo  *cxMDWCheckErr;
  cxErrorCode   ec;

  cxMDWPortIDIn[0] = cxInputPortOpen( "geometry" );
  cxMDWPortIDIn[1] = cxInputPortOpen( "window" );

  cxP_In_geometry = ( cxGeometry *) cxInputDataGet( cxMDWPortIDIn[0] );
  cxP_In_window = ( cxParameter *) cxInputDataGet( cxMDWPortIDIn[1] );

  for (cxMDWi=0; cxMDWi< cxMDWNARGrenderlite; cxMDWi++) cxMDWIfCoerce[cxMDWi] = TRUE;

  if (1 && !cxP_In_geometry) {
    geo = NULL;
    cxMDWIfCoerce[0] = FALSE;
  } else {
  geo = (cxGeometry *) cxP_In_geometry;
  }

  if (0 && !cxP_In_window) 
  {
    window = 0L;
    cxMDWIfCoerce[2] = FALSE;
  } 
  else 
  {
    tmp = (void*)cxVecNew( 1, cx_prim_long );	/* return of type (long    *) */
    if (!tmp)
      return((long) cx_err_error);

    cxVecClipSet( (cxErrorCode)0, (cxErrorCode)0 );
    cxMDWIrc = cxVecCoerce( 1, 
	  cxParameterValueType( cxP_In_window, &ec ), 
	  (void*) ( (signed long *)  cxParameterValueGet( cxP_In_window, &ec ) ), 
	  cx_prim_long, 
	  (void*) tmp );

    if (cxMDWIrc != cx_err_none)
      return((long) cxMDWIrc);

    window = *(long    *) tmp;

    cxMDWIfCoerce[2] = FALSE;
    cxVecDel( tmp );
  }

  geoFlag = cxInputDataChanged( cxMDWPortIDIn[0] );
  windowFlag = cxInputDataChanged( cxMDWPortIDIn[1] );

  cxEnterUserCode( 4, geo, geoFlag, window, windowFlag );
  renderlite( geo, geoFlag, window, windowFlag );
  cxExitUserCode();

  if (cxDataAllocErrorGet()) return( cx_err_error );
  return( cx_err_none );
}
