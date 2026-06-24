#include <cx/typedefs.h>
#include <cx/userfuncs.h>

extern "C"
{
	long UserFunction(void);
	void Init(void);
	void Remove(void);
	void ConnectIn(const char *, int);
	void DisconnectIn(const char *, int) ;
	void ConnectOut(const char *, int);
	void DisconnectOut(const char *, int);
	void Create(void);

	cxFuncTable cx_FuncTable[] = 
	{
			 (cxFuncTable)UserFunction,
			 0
	};

	cxHookTable cx_HookTable[] = 
	{
		{ cx_func_init, (cxVoidHookFunc)Init },
		{ cx_func_removed, (cxVoidHookFunc)Remove },
		{ cx_func_connect_input, (cxVoidHookFunc)ConnectIn },
		{ cx_func_disconnect_input, (cxVoidHookFunc)DisconnectIn },
		{ cx_func_connect_output, (cxVoidHookFunc)ConnectOut },
		{ cx_func_disconnect_output, (cxVoidHookFunc)DisconnectOut },
		{ cx_func_create, (cxVoidHookFunc)Create },
		{ cx_func_last}
	};

	void cxModuleMetaTypeInit(void) ;
	long cxPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) ;
	long cxPortCheckOut ( int linkC, void *links[], long *cxMDWPortID );
}
