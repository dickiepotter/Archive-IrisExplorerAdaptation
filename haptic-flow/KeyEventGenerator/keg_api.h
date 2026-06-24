#ifndef __KEG_API_H
#define __KEG_API_H

//=============================================================================
// keg_api.h  --  Shared-library (DLL) export macro for the KeyEventGenerator
//                node package.
//
// HOW THIS WORKS
// --------------
// H3DAPI nodes are normally built into a shared library that the H3D runtime
// loads at startup. On Windows, every class/symbol that must be visible to
// code outside this DLL has to be explicitly tagged for export with
// __declspec(dllexport). The KEG_API macro provides a single place to apply
// that tag, so each public class is simply declared as:
//
//     class KEG_API SomeNode : public ... { ... };
//
// WHY THE PLATFORM SPLIT
// ----------------------
// On non-Windows platforms (Linux/macOS) symbol visibility is handled
// differently and no per-class decoration is required, so KEG_API expands to
// nothing there. This keeps the class declarations portable: the same
// "class KEG_API Foo" source compiles on every platform.
//=============================================================================

#ifdef WIN32
#include <windows.h>
// Windows build: mark the following class/symbol as exported from this DLL.
#define KEG_API __declspec(dllexport)
#else
// Non-Windows build: no decoration needed; the macro disappears.
#define KEG_API
#endif

#endif //__KEG_API_H
