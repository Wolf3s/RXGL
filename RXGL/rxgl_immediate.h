/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_immediate.h  --  Immediate mode geometry batcher declarations.
 *                       Declares OGLPrimitiveVertexBuffer, which accumulates vertices
 *                       from glBegin/glEnd blocks and flushes them to D3D via
 *                       DrawPrimitiveUP. Also declares all immediate-mode GL entry
 *                       points: glVertex*, glColor*, glTexCoord*, glNormal*, glBegin,
 *                       glEnd, and the attrib stack helpers.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_IMMEDIATE_H
#define RXGL_IMMEDIATE_H

#include "rxgl_internal.h"

// ---------------------------------------------------------------------------
// OGLPrimitiveVertexBuffer
// Batches immediate-mode geometry and submits to D3D DrawPrimitiveUP.
// ---------------------------------------------------------------------------

class OGLPrimitiveVertexBuffer
{
public:
    OGLPrimitiveVertexBuffer();
    ~OGLPrimitiveVertexBuffer();

    HRESULT Initialize(IDirect3DDevice8* dev, IDirect3D8* d3d,
                       bool hwTL, DWORD typeDesc);

    DWORD GetVertexTypeDesc() const { return m_vertexTypeDesc; }
    DWORD GetVertexCount()    const { return m_vertexCount; }

    void SetColor(D3DCOLOR color)       { m_color = color; }
    void SetNormal(float nx, float ny, float nz)
    {
        m_nx = nx; m_ny = ny; m_nz = nz;
    }
    void SetTextureCoord0(float u, float v);
    void SetTextureCoord(int stage, float u, float v);
    void SetVertex(float x, float y, float z);

    int  IsMergableMode(GLenum mode);
    void Begin(GLenum mode);
    void Append(GLenum mode);
    void End();

    // Software clip plane support.
    // Called from RXGL::glEnd() after all vertices are buffered.
    // Converts the current primitive to triangles, clips against all
    // enabled planes using Sutherland-Hodgman, then submits to D3D.
    // planes[i][0..3] = (A,B,C,D) in eye space. enabled[i] flags which are active.
    bool ClipAndFlush(const GLdouble planes[6][4], const bool enabled[6],
                      const GLfloat modelView[16]);

private:
    void ConvertQuadsToTriangles();
    void ConvertQuadStripToTriangleStrip();
    void ConvertToTriangleList();  // converts any primitive to flat triangle list
    void Ensure(int extra);

    // Sutherland-Hodgman clip of a triangle list against one plane.
    // Input/output: flat arrays of vertex data. Returns new vertex count.
    int  ClipTriListAgainstPlane(char* verts, int vertCount,
                                  float A, float B, float C, float D);

    GLenum           m_drawMode;
    DWORD            m_vertexTypeDesc;
    int              m_vertexSize;
    IDirect3DDevice8* m_pD3DDev;
    char*            m_buf;
    int              m_size;
    int              m_count;
    DWORD            m_vertexCount;
    D3DCOLOR         m_color;
    float            m_nx, m_ny, m_nz;   // current normal
    float            m_textureCoords[MAXSTAGES * 2];
};

#endif // RXGL_IMMEDIATE_H
