/// \file ibfvss_api.h
/// \brief Defines the IBFVSS_API symbol-export macro for this library.
///
/// On Windows the IBFV stream-surface code is built as a DLL, so every class
/// and free function that needs to be visible to other modules (the H3D
/// runtime, the X3D loader, the stand-alone test program) must be tagged with
/// __declspec(dllexport). On all other platforms no decoration is required and
/// the macro expands to nothing.
///
/// IMPORTANT: The macro name IBFVSS_API and this export logic are part of the
/// library's binary contract and must not be renamed or altered.
#ifndef __IBFVSS_API_H
#define __IBFVSS_API_H
#ifdef WIN32
// windows.h is pulled in because __declspec and the GL headers used by this
// library expect the Win32 types/macros to already be defined.
#include <windows.h>
// Mark the following symbol as exported from this DLL.
#define IBFVSS_API __declspec(dllexport)
#else
// Non-Windows builds need no export decoration.
#define IBFVSS_API
#endif
#endif //__IBFVSS_API_H
