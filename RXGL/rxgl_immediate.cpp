/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_immediate.cpp  --  OGLPrimitiveVertexBuffer implementation.
 *                         Accumulates per-vertex data (position, color, texcoords,
 *                         normal) during a glBegin/glEnd block, builds a packed
 *                         D3D FVF vertex buffer on the fly, and submits it as
 *                         DrawPrimitiveUP at glEnd time. Handles quad and polygon
 *                         decomposition to triangles.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_immediate.h"

OGLPrimitiveVertexBuffer::OGLPrimitiveVertexBuffer()
    : m_drawMode((GLenum)-1), m_size(0), m_count(0),
      m_buf(NULL), m_vertexCount(0),
      m_vertexTypeDesc(0), m_vertexSize(0),
      m_pD3DDev(NULL), m_color(0xff000000),
      m_nx(0.0f), m_ny(0.0f), m_nz(1.0f)
{
    memset(m_textureCoords, 0, sizeof(m_textureCoords));
}

OGLPrimitiveVertexBuffer::~OGLPrimitiveVertexBuffer()
{
    delete[] m_buf;
}

HRESULT OGLPrimitiveVertexBuffer::Initialize(IDirect3DDevice8* dev,
                                              IDirect3D8* /*d3d*/,
                                              bool /*hwTL*/, DWORD typeDesc)
{
    m_pD3DDev = dev;
    if (m_vertexTypeDesc != typeDesc)
    {
        m_vertexTypeDesc = typeDesc;
        m_vertexSize = 0;
        if (m_vertexTypeDesc & D3DFVF_XYZ)
            m_vertexSize += 3 * sizeof(float);
        if (m_vertexTypeDesc & D3DFVF_NORMAL)
            m_vertexSize += 3 * sizeof(float);
        if (m_vertexTypeDesc & D3DFVF_DIFFUSE)
            m_vertexSize += 4;
        int stages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
        m_vertexSize += 2 * sizeof(float) * stages;
    }
    return S_OK;
}

void OGLPrimitiveVertexBuffer::SetTextureCoord0(float u, float v)
{
    DWORD* p = (DWORD*)m_textureCoords;
    p[0] = *(DWORD*)&u;
    p[1] = *(DWORD*)&v;
}

void OGLPrimitiveVertexBuffer::SetTextureCoord(int stage, float u, float v)
{
    DWORD* p = (DWORD*)m_textureCoords + (stage << 1);
    p[0] = *(DWORD*)&u;
    p[1] = *(DWORD*)&v;
}

void OGLPrimitiveVertexBuffer::SetVertex(float x, float y, float z)
{
    if (m_count + m_vertexSize > m_size)
        Ensure(m_vertexSize);

    float* p = (float*)(m_buf + m_count);
    *p++ = x; *p++ = y; *p++ = z;

    if (m_vertexTypeDesc & D3DFVF_NORMAL)
    {
        *p++ = m_nx; *p++ = m_ny; *p++ = m_nz;
    }

    if (m_vertexTypeDesc & D3DFVF_DIFFUSE)
    {
        *(DWORD*)p = m_color; p++;
    }

    int stages = (m_vertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    const float* tc = m_textureCoords;
    for (int i = 0; i < stages; i++) { *p++ = *tc++; *p++ = *tc++; }

    m_count += m_vertexSize;
    m_vertexCount++;
}

int OGLPrimitiveVertexBuffer::IsMergableMode(GLenum mode)
{
    return (mode == m_drawMode) && (mode == GL_QUADS || mode == GL_TRIANGLES);
}

void OGLPrimitiveVertexBuffer::Begin(GLenum mode)  { m_drawMode = mode; }
void OGLPrimitiveVertexBuffer::Append(GLenum /*mode*/) {}

void OGLPrimitiveVertexBuffer::End()
{
    if (m_vertexCount == 0)
        return;

    D3DPRIMITIVETYPE pt;
    DWORD primCount;

    switch (m_drawMode)
    {
    case GL_POINTS:
        pt = D3DPT_POINTLIST; primCount = m_vertexCount; break;

    case GL_LINES:
        pt = D3DPT_LINELIST; primCount = m_vertexCount / 2; break;

    case GL_LINE_STRIP:
        pt = D3DPT_LINESTRIP; primCount = m_vertexCount - 1; break;

    case GL_LINE_LOOP:
        if (m_vertexCount >= 2)
        {
            if (m_count + m_vertexSize > m_size) Ensure(m_vertexSize);
            memcpy(m_buf + m_count, m_buf, m_vertexSize);
            m_count += m_vertexSize;
            m_vertexCount++;
        }
        pt = D3DPT_LINESTRIP; primCount = m_vertexCount - 1; break;

    case GL_TRIANGLES:
        pt = D3DPT_TRIANGLELIST; primCount = m_vertexCount / 3; break;

    case GL_TRIANGLE_STRIP:
        pt = D3DPT_TRIANGLESTRIP; primCount = m_vertexCount - 2; break;

    case GL_TRIANGLE_FAN:
        pt = D3DPT_TRIANGLEFAN; primCount = m_vertexCount - 2; break;

    case GL_QUADS:
        if ((m_vertexCount % 4) == 0)
        {
            pt = D3DPT_QUADLIST; primCount = m_vertexCount / 4;
        }
        else
        {
            ConvertQuadsToTriangles();
            pt = D3DPT_TRIANGLELIST; primCount = m_vertexCount / 3;
        }
        break;

    case GL_QUAD_STRIP:
        if (m_vertexCount < 4) goto exit;
        ConvertQuadStripToTriangleStrip();
        pt = D3DPT_TRIANGLESTRIP; primCount = m_vertexCount - 2; break;

    case GL_POLYGON:
        if (m_vertexCount < 3) goto exit;
        pt = D3DPT_TRIANGLEFAN; primCount = m_vertexCount - 2; break;

    default:
        LocalDebugBreak();
        goto exit;
    }

    m_pD3DDev->SetVertexShader(m_vertexTypeDesc);
    m_pD3DDev->DrawPrimitiveUP(pt, primCount, m_buf, m_vertexSize);

exit:
    m_vertexCount = 0;
    m_count = 0;
}

void OGLPrimitiveVertexBuffer::ConvertQuadsToTriangles()
{
    int quadCount = m_vertexCount / 4;
    int triVerts  = quadCount * 6;
    int newSize   = triVerts * m_vertexSize;
    char* out = new char[newSize];

    for (int i = 0; i < quadCount; i++)
    {
        const char* q = m_buf + i * 4 * m_vertexSize;
        char*       d = out  + i * 6 * m_vertexSize;
        memcpy(d + 0 * m_vertexSize, q + 0 * m_vertexSize, m_vertexSize);
        memcpy(d + 1 * m_vertexSize, q + 1 * m_vertexSize, m_vertexSize);
        memcpy(d + 2 * m_vertexSize, q + 2 * m_vertexSize, m_vertexSize);
        memcpy(d + 3 * m_vertexSize, q + 0 * m_vertexSize, m_vertexSize);
        memcpy(d + 4 * m_vertexSize, q + 2 * m_vertexSize, m_vertexSize);
        memcpy(d + 5 * m_vertexSize, q + 3 * m_vertexSize, m_vertexSize);
    }

    delete[] m_buf;
    m_buf = out; m_size = newSize; m_vertexCount = triVerts; m_count = newSize;
}

void OGLPrimitiveVertexBuffer::ConvertQuadStripToTriangleStrip()
{
    char* tmp = new char[m_vertexSize];
    int pairs = m_vertexCount / 2;
    for (int i = 0; i < pairs; i++)
    {
        char* a = m_buf + (i * 2)     * m_vertexSize;
        char* b = m_buf + (i * 2 + 1) * m_vertexSize;
        memcpy(tmp, a, m_vertexSize);
        memcpy(a, b, m_vertexSize);
        memcpy(b, tmp, m_vertexSize);
    }
    delete[] tmp;
}

void OGLPrimitiveVertexBuffer::Ensure(int extra)
{
    if (m_count + extra <= m_size) return;
    int newSize = m_size * 2;
    if (newSize < m_count + extra) newSize = m_count + extra;
    char* nb = new char[newSize];
    if (m_buf) memcpy(nb, m_buf, m_count);
    delete[] m_buf;
    m_buf = nb; m_size = newSize;
}

// ---------------------------------------------------------------------------
// ConvertToTriangleList
// Converts whatever primitive is in the buffer into a flat triangle list.
// No lambdas -- C++03 compatible for Xbox XDK compiler.
// ---------------------------------------------------------------------------
void OGLPrimitiveVertexBuffer::ConvertToTriangleList()
{
    if (m_vertexCount < 3) { m_vertexCount = 0; m_count = 0; return; }

    int vs = m_vertexSize;
    int nv = (int)m_vertexCount;

    // Allocate worst-case output: each vert could generate 2 tris
    int maxOut = nv * 2 * 3 + 6;
    char* out = new char[(size_t)maxOut * vs];
    int outCount = 0;

    #define EMIT(idx) memcpy(out + outCount * vs, m_buf + (idx) * vs, vs); outCount++;

    switch (m_drawMode)
    {
    case GL_TRIANGLES:
        memcpy(out, m_buf, (size_t)nv * vs);
        outCount = nv;
        break;

    case GL_TRIANGLE_STRIP:
        for (int i = 0; i + 2 < nv; i++)
        {
            if (i & 1) { EMIT(i+1); EMIT(i);   EMIT(i+2); }
            else       { EMIT(i);   EMIT(i+1); EMIT(i+2); }
        }
        break;

    case GL_TRIANGLE_FAN:
    case GL_POLYGON:
        for (int i = 1; i + 1 < nv; i++)
        {
            EMIT(0); EMIT(i); EMIT(i+1);
        }
        break;

    case GL_QUADS:
    {
        int quads = nv / 4;
        for (int i = 0; i < quads; i++)
        {
            int b = i * 4;
            EMIT(b); EMIT(b+1); EMIT(b+2);
            EMIT(b); EMIT(b+2); EMIT(b+3);
        }
        break;
    }

    case GL_QUAD_STRIP:
        for (int i = 0; i + 3 < nv; i += 2)
        {
            EMIT(i); EMIT(i+1); EMIT(i+2);
            EMIT(i+1); EMIT(i+3); EMIT(i+2);
        }
        break;

    default:
        // Lines/points can't be triangulated -- leave empty
        break;
    }

    #undef EMIT

    delete[] m_buf;
    m_buf = out;
    m_size = maxOut * vs;
    m_vertexCount = (DWORD)outCount;
    m_count = outCount * vs;
    m_drawMode = GL_TRIANGLES;
}

// ---------------------------------------------------------------------------
// ClipTriListAgainstPlane
// Sutherland-Hodgman clipping of a flat triangle list against one plane.
// Plane: Ax+By+Cz+D >= 0 means inside.
// All vertex attributes are linearly interpolated at intersections.
// No lambdas -- C++03 compatible.
// Returns new vertex count (multiple of 3).
// ---------------------------------------------------------------------------

static inline float SH_Dot(const char* v, float A, float B, float C, float D)
{
    const float* f = (const float*)v;
    return A*f[0] + B*f[1] + C*f[2] + D;
}

static void SH_Lerp(char* dst, const char* va, const char* vb, float t,
                    int vs, DWORD vertexTypeDesc)
{
    // Lerp all data as floats
    int floatCount = vs / (int)sizeof(float);
    const float* fa = (const float*)va;
    const float* fb = (const float*)vb;
    float* fd = (float*)dst;
    for (int i = 0; i < floatCount; i++)
        fd[i] = fa[i] + t * (fb[i] - fa[i]);

    // Fix up DIFFUSE DWORD -- lerp each byte separately
    if (vertexTypeDesc & D3DFVF_DIFFUSE)
    {
        int diffOff = 3 * (int)sizeof(float);
        if (vertexTypeDesc & D3DFVF_NORMAL) diffOff += 3 * (int)sizeof(float);
        const unsigned char* ca = (const unsigned char*)(va + diffOff);
        const unsigned char* cb = (const unsigned char*)(vb + diffOff);
        unsigned char* cd = (unsigned char*)(dst + diffOff);
        for (int i = 0; i < 4; i++)
            cd[i] = (unsigned char)((float)ca[i] + t * ((float)cb[i] - (float)ca[i]));
    }
}

int OGLPrimitiveVertexBuffer::ClipTriListAgainstPlane(
    char* verts, int vertCount, float A, float B, float C, float D)
{
    if (vertCount == 0) return 0;

    int vs = m_vertexSize;
    // Each triangle (3 verts) can produce at most 4 verts output = 2 tris = 6 verts.
    // Allocate worst case: 2x input verts (each tri can double in vert count).
    // Add 6 for safety rounding.
    int maxOut = vertCount * 2 + 6;
    char* out = new char[(size_t)maxOut * vs];
    int outCount = 0;

    // Temp polygon storage -- SH on a triangle produces max 4 verts.
    // Use 8 for safety.
    char* poly  = new char[8 * vs];
    char* poly2 = new char[8 * vs];

    for (int tri = 0; tri + 2 < vertCount; tri += 3)
    {
        // Load triangle into poly[]
        memcpy(poly,         verts + tri*vs,       vs);
        memcpy(poly +   vs,  verts + (tri+1)*vs,   vs);
        memcpy(poly + 2*vs,  verts + (tri+2)*vs,   vs);
        int polyCount = 3;

        // Clip polygon against plane
        int outPoly = 0;
        for (int i = 0; i < polyCount; i++)
        {
            char* cur  = poly + i * vs;
            char* next = poly + ((i+1) % polyCount) * vs;
            float da = SH_Dot(cur,  A,B,C,D);
            float db = SH_Dot(next, A,B,C,D);
            bool curIn  = (da >= 0.0f);
            bool nextIn = (db >= 0.0f);

            if (curIn)
            {
                memcpy(poly2 + outPoly * vs, cur, vs);
                outPoly++;
            }
            if (curIn != nextIn)
            {
                // Intersection
                float denom = da - db;
                float t = (fabsf(denom) > 1e-7f) ? (da / denom) : 0.5f;
                SH_Lerp(poly2 + outPoly * vs, cur, next, t, vs, m_vertexTypeDesc);
                outPoly++;
            }
        }

        // Fan-triangulate the clipped polygon into out[]
        // A 4-vert polygon produces 2 triangles = 6 verts.
        // Check we won't overflow out[].
        int newTris = (outPoly >= 3) ? (outPoly - 2) : 0;
        if (outCount + newTris * 3 > maxOut)
        {
            // Grow output buffer
            int newMax = (outCount + newTris * 3) * 2 + 6;
            char* newOut = new char[(size_t)newMax * vs];
            memcpy(newOut, out, (size_t)outCount * vs);
            delete[] out;
            out = newOut;
            maxOut = newMax;
        }

        for (int i = 1; i + 1 < outPoly; i++)
        {
            memcpy(out + outCount*vs,       poly2,             vs);
            memcpy(out + (outCount+1)*vs,   poly2 + i*vs,     vs);
            memcpy(out + (outCount+2)*vs,   poly2 + (i+1)*vs, vs);
            outCount += 3;
        }
    }

    delete[] poly;
    delete[] poly2;

    memcpy(verts, out, (size_t)outCount * vs);
    delete[] out;
    return outCount;
}

// ---------------------------------------------------------------------------
// ClipAndFlush
// Converts buffered primitive to triangle list, clips in eye space against
// all enabled planes, submits clipped result to D3D.
// Returns true if it handled the flush (caller should not call End()).
// ---------------------------------------------------------------------------
bool OGLPrimitiveVertexBuffer::ClipAndFlush(
    const GLdouble planes[6][4], const bool enabled[6],
    const GLfloat mv[16])
{
    if (m_vertexCount == 0) return true;

    // Lines/points: can't clip as triangles, fall through to normal End()
    if (m_drawMode == GL_LINES || m_drawMode == GL_LINE_STRIP ||
        m_drawMode == GL_LINE_LOOP || m_drawMode == GL_POINTS)
        return false;

    ConvertToTriangleList();
    if (m_vertexCount == 0) { m_count = 0; return true; }

    int vs = m_vertexSize;
    int n  = (int)m_vertexCount;

    // Transform eye-space planes to object space using current modelview.
    // GL spec stores planes in eye space; vertices are in object space.
    // object_plane = D3D_MV * eye_plane  (column vector: result[r] = sum_c m[r][c]*e[c])
    // With mv packed column-major (mv[r+c*4] = D3D_MV[r][c]):
    //   oA = mv[0]*eA + mv[4]*eB + mv[8]*eC  + mv[12]*eD
    //   oB = mv[1]*eA + mv[5]*eB + mv[9]*eC  + mv[13]*eD  etc.
    float objPlanes[6][4];
    int activePlaneCount = 0;
    for (int i = 0; i < 6; i++)
    {
        if (!enabled[i]) continue;
        float eA = (float)planes[i][0];
        float eB = (float)planes[i][1];
        float eC = (float)planes[i][2];
        float eD = (float)planes[i][3];
        objPlanes[activePlaneCount][0] = mv[0]*eA + mv[4]*eB + mv[8]*eC  + mv[12]*eD;
        objPlanes[activePlaneCount][1] = mv[1]*eA + mv[5]*eB + mv[9]*eC  + mv[13]*eD;
        objPlanes[activePlaneCount][2] = mv[2]*eA + mv[6]*eB + mv[10]*eC + mv[14]*eD;
        objPlanes[activePlaneCount][3] = mv[3]*eA + mv[7]*eB + mv[11]*eC + mv[15]*eD;
        activePlaneCount++;
    }

    if (activePlaneCount == 0) return false;

    // Allocate working buffer -- 64x headroom for worst-case expansion
    int bufCapacity = n * 64 + 64;
    char* workBuf = new char[(size_t)bufCapacity * vs];
    memcpy(workBuf, m_buf, (size_t)n * vs);

    // Clip in object space against each active plane
    for (int i = 0; i < activePlaneCount; i++)
    {
        if (n == 0) break;
        n = ClipTriListAgainstPlane(workBuf, n,
            objPlanes[i][0], objPlanes[i][1],
            objPlanes[i][2], objPlanes[i][3]);
    }

    if (n == 0) { delete[] workBuf; m_vertexCount=0; m_count=0; return true; }

    // Copy clipped result back into m_buf and submit normally
    // D3DTS_WORLD is still set to the modelview -- no need to change transforms
    if (n * vs > m_size)
    {
        delete[] m_buf;
        m_buf = new char[(size_t)n * vs];
        m_size = n * vs;
    }
    memcpy(m_buf, workBuf, (size_t)n * vs);
    delete[] workBuf;

    m_vertexCount = (DWORD)n;
    m_count = n * vs;

    // Submit normally -- D3DTS_WORLD already has the correct modelview transform
    m_pD3DDev->SetVertexShader(m_vertexTypeDesc);
    DWORD primCount = m_vertexCount / 3;
    if (primCount > 0)
        m_pD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, primCount, m_buf, m_vertexSize);

    m_vertexCount = 0;
    m_count = 0;
    return true;
}
