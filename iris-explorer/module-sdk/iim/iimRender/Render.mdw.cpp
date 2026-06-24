#include "stdafx.h"


extern "C" { void render ( void  ); }

/* Number of user function arguments. */
#define cxMDWNARGrender 1

extern "C" long cxMDWrender( void )
{
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

  cxMDWPortIDOut[0] = cxOutputPortOpen( "Output Camera" );
  cxMDWPortIDOut[1] = cxOutputPortOpen( "Picked Geometry" );
  cxMDWPortIDOut[2] = cxOutputPortOpen( "Sync" );
  cxMDWPortIDOut[3] = cxOutputPortOpen( "Snapshot" );
  cxMDWPortIDOut[4] = cxOutputPortOpen( "Pick" );

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

  for (cxMDWi=0; cxMDWi< cxMDWNARGrender; cxMDWi++) 
    cxMDWIfCoerce[cxMDWi] = TRUE;

  cxEnterUserCode( 0 );
  render(  );
  cxExitUserCode();

  if (cxDataAllocErrorGet())
    return( cx_err_error );

  cxP_Out_Output_Camera = NULL;

  cxP_Out_Picked_Geometry = NULL;

  cxP_Out_Sync = NULL;

  cxP_Out_Snapshot = NULL;

  cxP_Out_Pick = NULL;

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
