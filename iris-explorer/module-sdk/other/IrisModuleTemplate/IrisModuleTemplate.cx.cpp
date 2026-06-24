#include "stdafx.h"

extern "C" 
{
	extern long cxMDWUserFunction(void);
	cxFuncTable cx_FuncTable[] = {(cxFuncTable)cxMDWUserFunction,NULL};
	extern int cxVerifyVersion(int, int);

	extern const char * cxMultTypeMismatchPtrMsg
		(
		   cxPortDir,
		   const char *,
		   const char *,
		   const char *,
		   const char *,
		   const char *,
		   int,
		   char const*const*,
		   char const*const*
		);

	extern const char * cxMultTypeMismatchPortMsg
		(
		   cxPortDir,
		   const char *,
		   const char *,
		   const char *,
		   const char *,
		   const char *,
		   int,
		   char const*const*,
		   char const*const*
		);

	__declspec(dllimport) extern cxMetaType cx_cxMetaTypeMetaType;

	extern void cxModuleMetaTypeInit(void) 
	{
		int major_version = 5;
		int minor_version = 0;
		cxVerifyVersion(major_version, minor_version);
		cxMetaTypeAdd(&cx_cxMetaTypeMetaType);
	}

	extern void Init(void);
	extern void Remove(void);
	extern void ConnectInput(const char *, int);
	extern void DiconnectInput(const char *, int);
	extern void ConnectOutput(const char *, int);
	extern void DisconnectOutput(const char *, int);
	extern void Create(void);

	cxHookTable cx_HookTable[] = 
	{
		{ cx_func_init, (cxVoidHookFunc)Init },
		{ cx_func_removed, (cxVoidHookFunc)Remove },
		{ cx_func_connect_input, (cxVoidHookFunc)ConnectInput },
		{ cx_func_disconnect_input, (cxVoidHookFunc)DiconnectInput },
		{ cx_func_connect_output, (cxVoidHookFunc)ConnectOutput },
		{ cx_func_disconnect_output, (cxVoidHookFunc)DisconnectOutput },
		{ cx_func_create, (cxVoidHookFunc)Create },
		{ cx_func_last}
	};

	extern cxErrorCode cxPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) 
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

	  if (strcmp( "IrisModuleTemplate", cxModuleNameGet())) return cx_err_none;
	  checkedPort = (int *) NULL;
	  linkInd = 0;
	  return cx_err_none;
	}
	
	extern cxErrorCode cxPortCheckOut ( int linkC, void *links[], long *cxMDWPortID ) 
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

	  if (strcmp( "IrisModuleTemplate", cxModuleNameGet())) return cx_err_none;
	  checkedPort = (int *) NULL;
	  linkInd = 0;
	  return cx_err_none;
	}
}


