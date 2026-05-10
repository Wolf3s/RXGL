/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_arrays.h  --  Vertex array subsystem declarations.
 *                    Owns: glVertexPointer, glColorPointer, glTexCoordPointer,
 *                          glNormalPointer, glEnableClientState, glDisableClientState,
 *                          glDrawArrays, glDrawElements, glArrayElement.
 *                    Defines VertexArrayState and RXGLArrayState structs.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_ARRAYS_H
#define RXGL_ARRAYS_H

#include "rxgl_internal.h"

struct VertexArrayState
{
    bool          enabled;
    int           size;     // components (2,3,4)
    GLenum        type;     // GL_FLOAT etc.
    GLsizei       stride;
    const GLvoid* pointer;

    VertexArrayState()
        : enabled(false), size(4), type(GL_FLOAT), stride(0), pointer(NULL) {}
};

struct RXGLArrayState
{
    VertexArrayState vertex;
    VertexArrayState color;
    VertexArrayState texCoord[MAXSTAGES];  // per-stage, set by glClientActiveTextureARB
    VertexArrayState normal;
    int              clientStage;          // which stage glTexCoordPointer targets

    RXGLArrayState() : clientStage(0) {}

    void EnableClientState(GLenum arr);
    void DisableClientState(GLenum arr);
    void SetClientStage(int stage) { clientStage = stage; }

    void SetVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr);
    void SetNormalPointer(GLenum type, GLsizei stride, const GLvoid* ptr);
    void SetColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr);
    void SetTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr);

    // Submit a range of vertex array data to the vertex buffer and flush it
    void DrawArrays(GLenum mode, GLint first, GLsizei count,
                    class OGLPrimitiveVertexBuffer& vb,
                    IDirect3DDevice8* dev, IDirect3D8* d3d,
                    bool hwTL, int texStages);

    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices,
                      class OGLPrimitiveVertexBuffer& vb,
                      IDirect3DDevice8* dev, IDirect3D8* d3d,
                      bool hwTL, int texStages);
};

#endif // RXGL_ARRAYS_H
