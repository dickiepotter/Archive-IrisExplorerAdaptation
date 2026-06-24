#ifndef __IVV_API_H
#define __IVV_API_H
// IVV_API controls symbol visibility for this library. On Windows it expands to
// __declspec(dllexport) so the class is exported from the DLL; on other
// platforms it expands to nothing (default visibility).
#ifdef WIN32
#include <windows.h>
#define IVV_API __declspec(dllexport)
#else
#define IVV_API
#endif
#endif //__IVV_API_H
