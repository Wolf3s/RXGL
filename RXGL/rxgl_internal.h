/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_internal.h  --  Internal shared definitions for all RXGL subsystem modules.
 *                      NOT included by user/test code. Only included by the shim's
 *                      own .cpp files. Pulls in rxgl_api.h, XDK headers, defines
 *                      shared macros (RELEASENULL, DX7 compat, debug helpers), and
 *                      declares the GL->D3D conversion functions shared across modules.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

#ifndef RXGL_INTERNAL_H
#define RXGL_INTERNAL_H

#include "rxgl_api.h"

#pragma warning(disable : 4244)  // float<->double conversion
#pragma warning(disable : 4820)  // struct padding
#pragma warning(disable : 4273)  // inconsistent DLL linkage

#define D3D_OVERLOADS
#define RELEASENULL(obj) do { if (obj) { (obj)->Release(); (obj) = NULL; } } while(0)

#include "xgraphics.h"
#include <stdio.h>

// Compatibility shims for DX7 macros still used in callers
#ifdef D3DRGBA
#undef D3DRGBA
#endif
#define D3DRGBA(r,g,b,a)    D3DCOLOR_COLORVALUE(r,g,b,a)
#define D3DRGB(r,g,b)       D3DCOLOR_COLORVALUE(r,g,b,1.f)
#define RGBA_MAKE           D3DCOLOR_RGBA

// SGIS multitexture tokens (old D3DQuake extension)
#define TEXTURE0_SGIS       0x835E
#define TEXTURE1_SGIS       0x835F

// Xbox caps
#define XBOX_MAX_TEXTURE_SIZE   2048
#define XBOX_MAX_TEXTURE_UNITS  4

// Max texture stages
#define MAXSTAGES 4

// Max display lists
#define RXGL_MAX_DISPLAY_LISTS 8192

// ---------------------------------------------------------------------------
// Clamp helper
// ---------------------------------------------------------------------------
#define Clamp(x) (x)   // Callers guarantee [0,1]

// ---------------------------------------------------------------------------
// Debug helpers
// ---------------------------------------------------------------------------
static inline void LocalDebugBreak()
{
    // Enable for debugging: DebugBreak();
}

static inline void DbgPrint(const char* fmt, ...)
{
#ifdef _DEBUG
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    OutputDebugString(buf);
#else
    (void)fmt;
#endif
}

// ---------------------------------------------------------------------------
// GL -> D3D conversion helpers (used by multiple modules)
// ---------------------------------------------------------------------------
D3DBLEND            GLToDXSBlend(GLenum glBlend);
D3DBLEND            GLToDXDBlend(GLenum glBlend);
D3DCMPFUNC          GLToDXCompare(GLenum func);
D3DSTENCILOP        GLToDXStencilOp(GLenum op);
D3DTEXTUREFILTERTYPE GLToDXMinFilter(GLint filter);
D3DTEXTUREFILTERTYPE GLToDXMipFilter(GLint filter);
D3DTEXTUREFILTERTYPE GLToDXMagFilter(GLint filter);
D3DTEXTUREADDRESS   GLToDXWrap(GLint wrap);
D3DTEXTUREOP        GLToDXTextEnvMode(GLint mode);

#endif // RXGL_INTERNAL_H