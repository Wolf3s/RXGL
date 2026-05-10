/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_arrays.cpp  --  Vertex array subsystem implementation.
 *                      Handles non-immediate vertex submission via glDrawArrays
 *                      and glDrawElements. Reads typed client-side pointer data
 *                      (float, int, short, byte variants) and feeds it through
 *                      the immediate-mode batcher (OGLPrimitiveVertexBuffer).
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_arrays.h"
#include "rxgl_immediate.h"

// ---------------------------------------------------------------------------
// Helper: compute byte stride for an array if stride==0 (tightly packed)
// ---------------------------------------------------------------------------
static int DefaultStride(int size, GLenum type)
{
    int typeSize = 1;
    switch (type)
    {
    case GL_FLOAT:          typeSize = sizeof(GLfloat);  break;
    case GL_INT:            typeSize = sizeof(GLint);    break;
    case GL_UNSIGNED_INT:   typeSize = sizeof(GLuint);   break;
    case GL_SHORT:          typeSize = sizeof(GLshort);  break;
    case GL_UNSIGNED_SHORT: typeSize = sizeof(GLushort); break;
    case GL_BYTE:           typeSize = sizeof(GLbyte);   break;
    case GL_UNSIGNED_BYTE:  typeSize = sizeof(GLubyte);  break;
    }
    return size * typeSize;
}

// ---------------------------------------------------------------------------
// Helper: read one float component from a typed pointer
// ---------------------------------------------------------------------------
static float ReadFloat(const unsigned char* p, GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:          return *(const GLfloat*)p;
    case GL_INT:            return (float)*(const GLint*)p;
    case GL_UNSIGNED_INT:   return (float)*(const GLuint*)p;
    case GL_SHORT:          return (float)*(const GLshort*)p;
    case GL_UNSIGNED_SHORT: return (float)*(const GLushort*)p;
    case GL_BYTE:           return (float)*(const GLbyte*)p;
    case GL_UNSIGNED_BYTE:  return (float)*(const GLubyte*)p / 255.0f;
    }
    return 0.0f;
}

// ---------------------------------------------------------------------------
// EnableClientState / DisableClientState
// ---------------------------------------------------------------------------
void RXGLArrayState::EnableClientState(GLenum arr)
{
    switch (arr)
    {
    case GL_VERTEX_ARRAY:        vertex.enabled              = true; break;
    case GL_COLOR_ARRAY:         color.enabled               = true; break;
    case GL_TEXTURE_COORD_ARRAY: texCoord[clientStage].enabled = true; break;
    case GL_NORMAL_ARRAY:        normal.enabled              = true; break;
    }
}

void RXGLArrayState::DisableClientState(GLenum arr)
{
    switch (arr)
    {
    case GL_VERTEX_ARRAY:        vertex.enabled              = false; break;
    case GL_COLOR_ARRAY:         color.enabled               = false; break;
    case GL_TEXTURE_COORD_ARRAY: texCoord[clientStage].enabled = false; break;
    case GL_NORMAL_ARRAY:        normal.enabled              = false; break;
    }
}

// ---------------------------------------------------------------------------
// Pointer setters
// ---------------------------------------------------------------------------
void RXGLArrayState::SetVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr)
{ vertex.size=size; vertex.type=type; vertex.stride=stride; vertex.pointer=ptr; }

void RXGLArrayState::SetNormalPointer(GLenum type, GLsizei stride, const GLvoid* ptr)
{ normal.size=3; normal.type=type; normal.stride=stride; normal.pointer=ptr; }

void RXGLArrayState::SetColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr)
{ color.size=size; color.type=type; color.stride=stride; color.pointer=ptr; }

void RXGLArrayState::SetTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr)
{ texCoord[clientStage].size=size; texCoord[clientStage].type=type; texCoord[clientStage].stride=stride; texCoord[clientStage].pointer=ptr; }

// ---------------------------------------------------------------------------
// SubmitVertex -- fetch one vertex from all enabled arrays and push to VB
// ---------------------------------------------------------------------------
static void SubmitVertex(int idx,
    const VertexArrayState& vaV,
    const VertexArrayState& vaC,
    const VertexArrayState* vaT,  // array of MAXSTAGES
    int texStages,
    const VertexArrayState& vaN,
    OGLPrimitiveVertexBuffer& vb)
{
    // Normal
    if (vaN.enabled && vaN.pointer)
    {
        int stride = vaN.stride ? vaN.stride : DefaultStride(3, vaN.type);
        const unsigned char* p = (const unsigned char*)vaN.pointer + idx * stride;
        int es = DefaultStride(1, vaN.type);
        float nx = ReadFloat(p,        vaN.type);
        float ny = ReadFloat(p +   es, vaN.type);
        float nz = ReadFloat(p + 2*es, vaN.type);
        vb.SetNormal(nx, ny, nz);
    }

    // Color
    if (vaC.enabled && vaC.pointer)
    {
        int stride = vaC.stride ? vaC.stride : DefaultStride(vaC.size, vaC.type);
        const unsigned char* p = (const unsigned char*)vaC.pointer + idx * stride;
        if (vaC.type == GL_UNSIGNED_BYTE)
        {
            GLubyte r = p[0], g = p[1], b = p[2];
            GLubyte a = (vaC.size >= 4) ? p[3] : 255;
            vb.SetColor(RGBA_MAKE(r, g, b, a));
        }
        else
        {
            int es = DefaultStride(1, vaC.type);
            float r = ReadFloat(p,       vaC.type);
            float g = ReadFloat(p +  es, vaC.type);
            float b = ReadFloat(p +2*es, vaC.type);
            float a = (vaC.size >= 4) ? ReadFloat(p + 3*es, vaC.type) : 1.0f;
            vb.SetColor(D3DRGBA(Clamp(r), Clamp(g), Clamp(b), Clamp(a)));
        }
    }

    // Texture coords — per stage
    for (int stage = 0; stage < texStages; stage++)
    {
        const VertexArrayState& vt = vaT[stage];
        if (vt.enabled && vt.pointer)
        {
            int stride = vt.stride ? vt.stride : DefaultStride(vt.size, vt.type);
            const unsigned char* p = (const unsigned char*)vt.pointer + idx * stride;
            int es = DefaultStride(1, vt.type);
            float u = ReadFloat(p,      vt.type);
            float v = (vt.size >= 2) ? ReadFloat(p + es, vt.type) : 0.0f;
            if (stage == 0)
                vb.SetTextureCoord0(u, v);
            else
                vb.SetTextureCoord(stage, u, v);
        }
    }

    // Position
    if (vaV.pointer)
    {
        int stride = vaV.stride ? vaV.stride : DefaultStride(vaV.size, vaV.type);
        const unsigned char* p = (const unsigned char*)vaV.pointer + idx * stride;
        int es = DefaultStride(1, vaV.type);
        float x = ReadFloat(p,      vaV.type);
        float y = ReadFloat(p + es, vaV.type);
        float z = (vaV.size >= 3) ? ReadFloat(p + 2*es, vaV.type) : 0.0f;
        vb.SetVertex(x, y, z);
    }
}

// ---------------------------------------------------------------------------
// DrawArrays
// ---------------------------------------------------------------------------
void RXGLArrayState::DrawArrays(GLenum mode, GLint first, GLsizei count,
    OGLPrimitiveVertexBuffer& vb,
    IDirect3DDevice8* dev, IDirect3D8* d3d,
    bool hwTL, int texStages)
{
    if (!vertex.pointer || count <= 0) return;

    DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;  // always include diffuse so glColor works
    if (normal.enabled)  fvf |= D3DFVF_NORMAL;
    fvf |= (texStages << D3DFVF_TEXCOUNT_SHIFT);

    vb.Initialize(dev, d3d, hwTL, fvf);
    vb.Begin(mode);

    for (GLint i = first; i < first + count; i++)
        SubmitVertex(i, vertex, color, texCoord, texStages, normal, vb);

    vb.End();
}

// ---------------------------------------------------------------------------
// DrawElements
// ---------------------------------------------------------------------------
void RXGLArrayState::DrawElements(GLenum mode, GLsizei count, GLenum type,
    const GLvoid* indices,
    OGLPrimitiveVertexBuffer& vb,
    IDirect3DDevice8* dev, IDirect3D8* d3d,
    bool hwTL, int texStages)
{
    if (!vertex.pointer || !indices || count <= 0) return;

    DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;  // always include diffuse so glColor works
    if (normal.enabled)  fvf |= D3DFVF_NORMAL;
    fvf |= (texStages << D3DFVF_TEXCOUNT_SHIFT);

    vb.Initialize(dev, d3d, hwTL, fvf);
    vb.Begin(mode);

    const GLubyte*  idx8  = (const GLubyte*) indices;
    const GLushort* idx16 = (const GLushort*)indices;
    const GLuint*   idx32 = (const GLuint*)  indices;

    for (GLsizei i = 0; i < count; i++)
    {
        GLint idx;
        switch (type)
        {
        case GL_UNSIGNED_BYTE:  idx = (GLint)idx8[i];  break;
        case GL_UNSIGNED_SHORT: idx = (GLint)idx16[i]; break;
        case GL_UNSIGNED_INT:   idx = (GLint)idx32[i]; break;
        default:                idx = (GLint)idx8[i];  break;
        }
        SubmitVertex(idx, vertex, color, texCoord, texStages, normal, vb);
    }

    vb.End();
}
