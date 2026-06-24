#include <cx/typedefs.h>
#include <cx/userfuncs.h>
#include <cx/UDT.h>
#include <mUserFunction.h>

#include <Windows.h> //for sleep method

extern "C"
{
	long UserFunction(void) 
	{ 
		cxEnterUserCode(0);
		long toRet = mUserFunction();
		cxExitUserCode();
		return toRet;
	}

	void Init(void) { mInit(); }
	void Remove(void) { mRemove(); }
	void ConnectIn(const char * portName, int id) { mConnectIn( portName, id ); }
	void DisconnectIn(const char * portName, int id) { mDisconnectIn( portName, id ); }
	void ConnectOut(const char * portName, int id) { mConnectOut( portName, id ); }
	void DisconnectOut(const char * portName, int id) { mDisconnectOut( portName, id ); }
	void Create(void) { mCreate(); }

	/********** BETTER IF MOVED TO MANAGED **********************/
	/*__declspec(dllimport) extern cxMetaType cx_cxMetaTypeMetaType;
	__declspec(dllimport) extern cxMetaType cx_cxGeometryMetaType;
	__declspec(dllimport) extern cxMetaType cx_cxParameterMetaType;
	__declspec(dllimport) extern cxMetaType cx_cxLatticeMetaType;*/

	void cxModuleMetaTypeInit(void)
	{ 
		Sleep(10000);
		/*cxVerifyVersion(5,0);
		cxMetaTypeAdd(&cx_cxMetaTypeMetaType);
		cxMetaTypeAdd(&cx_cxLatticeMetaType);
		cxMetaTypeAdd(&cx_cxGeometryMetaType);
		cxMetaTypeAdd(&cx_cxParameterMetaType);*/
	}
	/************************************************************/

	long cxPortCheckIn ( int linkC, void *links[], long *cxMDWPortID, long *linkIDs ) { Sleep(10000); return mPortCheckIn ( linkC, links, cxMDWPortID, linkIDs ); }
	long cxPortCheckOut ( int linkC, void *links[], long *cxMDWPortID ) { Sleep(10000); return mPortCheckOut ( linkC, links, cxMDWPortID ); }

	
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
}
