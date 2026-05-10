/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_displaylist.h  --  Display list subsystem declarations.
 *                         Defines the command opcode enum (RXGLDisplayListOp),
 *                         the command union (RXGLDisplayListCmd), the list object
 *                         (RXGLDisplayList), and the list table (RXGLDisplayLists).
 *                         Owns: glNewList, glEndList, glCallList, glCallLists,
 *                               glGenLists, glDeleteLists, glListBase, glIsList.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_DISPLAYLIST_H
#define RXGL_DISPLAYLIST_H

#include "rxgl_internal.h"

enum RXGLDisplayListOp
{
    DL_OP_BEGIN,
    DL_OP_END,
    DL_OP_COLOR3F,
    DL_OP_COLOR4F,
    DL_OP_TEXCOORD2F,
    DL_OP_VERTEX2F,
    DL_OP_VERTEX3F,
    DL_OP_LOADIDENTITY,
    DL_OP_TRANSLATEF,
    DL_OP_ROTATEF,
    DL_OP_PUSHMATRIX,
    DL_OP_POPMATRIX,
    DL_OP_MATRIXMODE,
    DL_OP_ENABLE,
    DL_OP_DISABLE,
    DL_OP_BINDTEXTURE,
    DL_OP_TEXENVI,
    // --- newly added ---
    DL_OP_VERTEX3FV,
    DL_OP_NORMAL3F,
    DL_OP_COLOR3UB,
    DL_OP_COLOR4UB,
    DL_OP_COLOR4FV,
    DL_OP_SCALEF,
    DL_OP_MULTMATRIXF,
    DL_OP_LOADMATRIXF,
    DL_OP_BLENDFUNC,
    DL_OP_BLENDEQUATION,
    DL_OP_ALPHAFUNC,
    DL_OP_FOGF,
    DL_OP_FOGI,
    DL_OP_FOGFV,
    DL_OP_TEXENVF,
    DL_OP_MATERIALFV,
    DL_OP_LIGHTFV,
    DL_OP_CALLLIST,
    DL_OP_DEPTHFUNC,
    DL_OP_DEPTHMASK,
    DL_OP_CULLFACE,
    DL_OP_FRONTFACE,
    DL_OP_SHADEMODEL,
    DL_OP_COLOR3UBV,
    DL_OP_COLOR4UBV,
    DL_OP_ORTHO,
    DL_OP_FRUSTUM
};

struct RXGLDisplayListCmd
{
    int     op;
    GLenum  e0;
    GLenum  e1;
    GLint   i0;
    GLuint  u0;
    GLubyte ub[4];   // for COLOR3UB / COLOR4UB / COLOR3UBV / COLOR4UBV
    GLfloat f[16];   // f[0..3] for most ops, f[0..15] for matrix ops
};

class RXGLDisplayList
{
public:
    RXGLDisplayList() : valid(false), cmds(NULL), count(0), capacity(0) {}
    ~RXGLDisplayList() { delete[] cmds; }

    void Clear();
    void Add(const RXGLDisplayListCmd& c);

    bool                  valid;
    RXGLDisplayListCmd* cmds;
    int                   count;
    int                   capacity;
};

// ---------------------------------------------------------------------------
// RXGLDisplayListManager
// Owns the list array and compile/execute state machine.
// ---------------------------------------------------------------------------

class RXGLDisplayListManager
{
public:
    RXGLDisplayListManager();

    void Init();

    // State query helpers
    bool IsCompiling()    const { return m_compiling; }
    bool IsExecuting()    const { return m_executing; }
    bool IsCompileOnly()  const { return m_compiling && !m_executing && m_mode == GL_COMPILE; }

    // Record a command into the currently open list
    void Record(const RXGLDisplayListCmd& c);

    // GL entry points
    GLuint GenLists(GLsizei range);
    void   NewList(GLuint list, GLenum mode);
    void   EndList();
    void   DeleteLists(GLuint list, GLsizei range);

    // Execute a list — caller provides a callback to dispatch each command.
    // Using a function pointer keeps this module decoupled from RXGL.
    typedef void (*DispatchFn)(const RXGLDisplayListCmd& cmd, void* ctx);
    void CallList(GLuint list, DispatchFn dispatch, void* ctx);

private:
    RXGLDisplayList m_lists[RXGL_MAX_DISPLAY_LISTS];
    GLuint            m_nextID;
    bool              m_compiling;
    bool              m_executing;
    GLuint            m_currentList;
    GLenum            m_mode;
};

#endif // RXGL_DISPLAYLIST_H
