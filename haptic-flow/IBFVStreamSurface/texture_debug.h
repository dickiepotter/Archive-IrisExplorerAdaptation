/// \file texture_debug.h
/// \brief Small OpenGL diagnostic helpers shared across the IBFV library.
///
/// These two free functions exist purely to make debugging the texture and
/// framebuffer code easier. They are deliberately exported (IBFVSS_API) and
/// called from several translation units, so their names and signatures are
/// part of the library's internal contract and must not change:
///   - checkError()      : poll glGetError() and print any pending GL error.
///   - dumpTexStates()   : dump a fixed list of texture-related GL state to
///                         stderr (rate-limited so it does not spam the log).
#ifndef _TEXTURE_DEBUG_H
#define _TEXTURE_DEBUG_H
#include <iostream>
#include "ibfvss_api.h"

/// Print the value of a fixed set of texture-related OpenGL state variables to
/// stderr. Intended only for interactive debugging.
void IBFVSS_API dumpTexStates(void);

/// Poll glGetError() and, if an error is pending, print it to stderr. The
/// optional \p id string is prepended to the message so the caller can tell
/// which call site detected the error.
void IBFVSS_API checkError(char *id=NULL);

#endif // _TEXTURE_DEBUG_H
