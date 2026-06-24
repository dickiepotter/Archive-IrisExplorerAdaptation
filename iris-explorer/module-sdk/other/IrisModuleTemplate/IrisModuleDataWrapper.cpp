#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cx/Typedefs.h>
#include <cx/ModuleCommand.h>
#include <cx/DataOps.h>
#include <cx/PortAccess.h>
#include "stdafx.h"

#define cxMDWNARGUserFunction 1

extern "C"
{
	void UserFunction ( void  );

	long cxMDWUserFunction( void )
	{
	  long  cxMDWi, cxMDWIrc;
	  long  cxMDWIfCoerce[ cxMDWNARGUserFunction ];
	  char	*cxMDWStr;
	  long  cxMDWFuncRetCode;
	  char  cxMDWFuncBuf[256];
	  void	*cxMDWObject;
	  long  cxMDWLen;
	  long  cxMDWPortIDIn[1];
	  long  cxMDWPortIDOut[1];
	  void	*tmp;
	  int	*cxMDWTypeTab;
	  cxDataCheckErrorInfo  *cxMDWCheckErr;
	  cxErrorCode   ec;

	  for (cxMDWi=0; cxMDWi< cxMDWNARGUserFunction; cxMDWi++) 
		cxMDWIfCoerce[cxMDWi] = TRUE;

	  cxEnterUserCode( 0 );
	  UserFunction();
	  cxExitUserCode();
	  
	  if (cxDataAllocErrorGet()) return( cx_err_error );
	  return( cx_err_none );
	}
}
