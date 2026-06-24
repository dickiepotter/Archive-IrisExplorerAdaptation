#include "stdafx.h"


extern "C" 
{

extern long cxMDWrenderlite(void);

cxFuncTable cx_FuncTable[] = { (cxFuncTable)cxMDWrenderlite, NULL };

extern int cxVerifyVersion(int, int);

extern const char * cxMultTypeMismatchPtrMsg(
       cxPortDir,const char *,const char *,const char *,
       const char *,const char *,
       int,char const*const*,char const*const*);

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
__declspec(dllimport) extern cxMetaType cx_cxGeometryMetaType;
__declspec(dllimport) extern cxMetaType cx_cxParameterMetaType;

extern void cxModuleMetaTypeInit(void) 
{
    int major_version = 5;
    int minor_version = 0;

    cxVerifyVersion(major_version, minor_version);

    cxMetaTypeAdd(&cx_cxMetaTypeMetaType);
    cxMetaTypeAdd(&cx_cxGeometryMetaType);
    cxMetaTypeAdd(&cx_cxParameterMetaType);

}

extern void render_init(void);
extern void render_Remove(void);
extern void render_connect_input(const char *, int);
extern void render_disconnect_input(const char *, int);
extern void render_connect_output(const char *, int);
extern void render_disconnect_output(const char *, int);
extern void render_create(void);

cxHookTable cx_HookTable[] = {
    { cx_func_init, (cxVoidHookFunc)render_init },
    { cx_func_removed, (cxVoidHookFunc)render_Remove },
    { cx_func_connect_input, (cxVoidHookFunc)render_connect_input },
    { cx_func_disconnect_input, (cxVoidHookFunc)render_disconnect_input },
    { cx_func_connect_output, (cxVoidHookFunc)render_connect_output },
    { cx_func_disconnect_output, (cxVoidHookFunc)render_disconnect_output },
    { cx_func_create, (cxVoidHookFunc)render_create },
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

  if (strcmp( "RenderLite", cxModuleNameGet()))
    return cx_err_none;

  if (!checkedPort) {
    checkedPort = (int*) calloc(2, sizeof(int));
    if (!checkedPort) {
      cxModAlert( "Unable to allocate space for type checking." );
      return cx_err_error;
    }
  }

  linkInd = 0;
  thisPort = 0;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {
    cxChkDat = links[linkInd];

    if (!cxChkDat || !cxInputDataChanged(thisPort)) {
      continue;
    }
    if (!checkedPort[thisPort]) {
      checkedPort[thisPort] = 1;
      nm = cxInputTypeGet(cxInputPortOpen("geometry"));
      if (strcmp(nm, "cxGeometry")) {
        msg = cxTypeMismatchMsg(cx_port_input, "geometry", "cxGeometry", nm );
        cxModAlert( msg );
        return cx_err_error;
      }
    }
    if (strcmp(cxDataTypeNameGet(cxChkDat), "cxGeometry") != 0) {
      nm = cxDataTypeNameGet(((cxGeometry *) cxChkDat));
      if (strcmp(nm, "cxGeneric") == 0) {
        nm = ((cxGeneric *) cxChkDat)->name;
      }
      msg = cxTypeMismatchMsg(cx_port_input, "geometry", "cxGeometry", nm);
      cxModAlert( msg );
      return cx_err_error;
    }
  }

  thisPort = 1;
  for (; linkInd < linkC && cxMDWPortID[linkInd] == thisPort; linkInd++) {;}
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

  if (strcmp( "RenderLite", cxModuleNameGet())) return cx_err_none;
  checkedPort = (int *) NULL;
  linkInd = 0;
  return cx_err_none;
}

}

