//////////////////////////////////////////////////////////////////////////////
/// \file torque_api.h
/// \brief Defines the TORQUE_API symbol-visibility macro for this library.
///
/// This small header centralises the platform-specific decoration that must
/// be attached to every class/function that is meant to be visible from
/// *outside* the compiled "torque" library (the DLL on Windows).
///
/// HOW IT WORKS:
///   On Windows, symbols are not exported from a DLL unless they are explicitly
///   marked with __declspec(dllexport) when the DLL is built. By placing
///   TORQUE_API in front of a class (e.g. "class TORQUE_API ForceTorqueField"),
///   the compiler is told to export that class so other modules (the H3D
///   runtime, other plugins, the Python bindings, X3D loaders, etc.) can link
///   against and instantiate it.
///
///   On non-Windows platforms the symbol-visibility model differs and no
///   decoration is required here, so TORQUE_API expands to nothing.
///
/// IMPORTANT: The literal name "TORQUE_API" is referenced verbatim by every
/// exported class in this library. Do not rename it.
//////////////////////////////////////////////////////////////////////////////
#ifndef __TORQUE_API_H
#define __TORQUE_API_H

#ifdef WIN32
// On Windows we need the SDK header that declares __declspec and friends,
// and we mark exported entities so they end up in the DLL's export table.
#include <windows.h>
#define TORQUE_API __declspec(dllexport)
#else
// On other platforms no special export decoration is needed.
#define TORQUE_API
#endif

#endif //__TORQUE_API_H
