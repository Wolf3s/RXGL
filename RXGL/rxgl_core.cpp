/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_core.cpp  --  Top-level coordinator and global C-callable GL wrappers.
 *                    This is the main entry point that wires all subsystems together:
 *                    render state, matrix stack, texture manager, lighting, immediate
 *                    mode batcher, vertex arrays, and display lists. Also owns the
 *                    GL->D3D enum conversion tables (GLToDXSBlend, GLToDXCompare, etc.)
 *                    shared by all modules via rxgl_internal.h.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_internal.h"
#include "rxgl_renderstate.h"
#include "rxgl_matrix.h"
#include "rxgl_texture.h"
#include "rxgl_lighting.h"
#include "rxgl_immediate.h"
#include "rxgl_arrays.h"
#include "rxgl_displaylist.h"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------

bool  g_force16bitTextures = true;
DWORD gWidth               = 640;
DWORD gHeight              = 480;
static int gVideoMode      = 0;

// ---------------------------------------------------------------------------
// GL -> D3D conversion tables (shared by all modules via rxgl_internal.h)
// ---------------------------------------------------------------------------

D3DBLEND GLToDXSBlend(GLenum g)
{
    switch (g)
    {
    case GL_ZERO:                return D3DBLEND_ZERO;
    case GL_ONE:                 return D3DBLEND_ONE;
    case GL_DST_COLOR:           return D3DBLEND_DESTCOLOR;
    case GL_ONE_MINUS_DST_COLOR: return D3DBLEND_INVDESTCOLOR;
    case GL_SRC_ALPHA:           return D3DBLEND_SRCALPHA;
    case GL_ONE_MINUS_SRC_ALPHA: return D3DBLEND_INVSRCALPHA;
    case GL_DST_ALPHA:           return D3DBLEND_DESTALPHA;
    case GL_ONE_MINUS_DST_ALPHA: return D3DBLEND_INVDESTALPHA;
    case GL_SRC_ALPHA_SATURATE:       return D3DBLEND_SRCALPHASAT;
    case GL_CONSTANT_COLOR:           return D3DBLEND_CONSTANTCOLOR;
    case GL_ONE_MINUS_CONSTANT_COLOR: return D3DBLEND_INVCONSTANTCOLOR;
    case GL_CONSTANT_ALPHA:           return D3DBLEND_CONSTANTALPHA;
    case GL_ONE_MINUS_CONSTANT_ALPHA: return D3DBLEND_INVCONSTANTALPHA;
    default: LocalDebugBreak();       return D3DBLEND_ONE;
    }
}

D3DBLEND GLToDXDBlend(GLenum g)
{
    switch (g)
    {
    case GL_ZERO:                return D3DBLEND_ZERO;
    case GL_ONE:                 return D3DBLEND_ONE;
    case GL_SRC_COLOR:           return D3DBLEND_SRCCOLOR;
    case GL_ONE_MINUS_SRC_COLOR: return D3DBLEND_INVSRCCOLOR;
    case GL_SRC_ALPHA:           return D3DBLEND_SRCALPHA;
    case GL_ONE_MINUS_SRC_ALPHA: return D3DBLEND_INVSRCALPHA;
    case GL_DST_ALPHA:           return D3DBLEND_DESTALPHA;
    case GL_ONE_MINUS_DST_ALPHA:      return D3DBLEND_INVDESTALPHA;
    case GL_CONSTANT_COLOR:           return D3DBLEND_CONSTANTCOLOR;
    case GL_ONE_MINUS_CONSTANT_COLOR: return D3DBLEND_INVCONSTANTCOLOR;
    case GL_CONSTANT_ALPHA:           return D3DBLEND_CONSTANTALPHA;
    case GL_ONE_MINUS_CONSTANT_ALPHA: return D3DBLEND_INVCONSTANTALPHA;
    default: LocalDebugBreak();       return D3DBLEND_ZERO;
    }
}

D3DCMPFUNC GLToDXCompare(GLenum func)
{
    switch (func)
    {
    case GL_NEVER:    return D3DCMP_NEVER;
    case GL_LESS:     return D3DCMP_LESS;
    case GL_EQUAL:    return D3DCMP_EQUAL;
    case GL_LEQUAL:   return D3DCMP_LESSEQUAL;
    case GL_GREATER:  return D3DCMP_GREATER;
    case GL_NOTEQUAL: return D3DCMP_NOTEQUAL;
    case GL_GEQUAL:   return D3DCMP_GREATEREQUAL;
    case GL_ALWAYS:   return D3DCMP_ALWAYS;
    default:          return D3DCMP_ALWAYS;
    }
}

D3DSTENCILOP GLToDXStencilOp(GLenum op)
{
    switch (op)
    {
    case GL_KEEP:    return D3DSTENCILOP_KEEP;
    case GL_ZERO:    return D3DSTENCILOP_ZERO;
    case GL_REPLACE: return D3DSTENCILOP_REPLACE;
    case GL_INCR:    return D3DSTENCILOP_INCRSAT;
    case GL_DECR:    return D3DSTENCILOP_DECRSAT;
    case GL_INVERT:  return D3DSTENCILOP_INVERT;
    default:         return D3DSTENCILOP_KEEP;
    }
}

D3DTEXTUREFILTERTYPE GLToDXMinFilter(GLint f)
{
    switch (f)
    {
    case GL_NEAREST: case GL_NEAREST_MIPMAP_NEAREST: case GL_NEAREST_MIPMAP_LINEAR:
        return D3DTEXF_POINT;
    default:
        return D3DTEXF_LINEAR;
    }
}

D3DTEXTUREFILTERTYPE GLToDXMipFilter(GLint f)
{
    switch (f)
    {
    case GL_NEAREST: case GL_LINEAR:                 return D3DTEXF_NONE;
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR_MIPMAP_NEAREST:                   return D3DTEXF_POINT;
    default:                                         return D3DTEXF_LINEAR;
    }
}

D3DTEXTUREFILTERTYPE GLToDXMagFilter(GLint f)
{ return (f == GL_NEAREST) ? D3DTEXF_POINT : D3DTEXF_LINEAR; }

D3DTEXTUREADDRESS GLToDXWrap(GLint wrap)
{
    switch (wrap)
    {
    case GL_CLAMP: case GL_CLAMP_TO_EDGE: return D3DTADDRESS_CLAMP;
    case GL_MIRRORED_REPEAT:              return D3DTADDRESS_MIRROR;
    default:                              return D3DTADDRESS_WRAP;
    }
}

D3DTEXTUREOP GLToDXTextEnvMode(GLint mode)
{
    switch (mode)
    {
    case GL_MODULATE: return D3DTOP_MODULATE;
    case GL_DECAL:
    case GL_REPLACE:  return D3DTOP_SELECTARG1;
    case GL_ADD:      return D3DTOP_ADD;
    case GL_BLEND:    return D3DTOP_MODULATE; // special-cased in TextureState
    default:          return D3DTOP_MODULATE;
    }
}

// ---------------------------------------------------------------------------
// RXGL - thin coordinator
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Attrib stack snapshot
// ---------------------------------------------------------------------------

#define RXGL_ATTRIB_STACK_DEPTH 16

struct RXGLAttribSnapshot
{
    GLbitfield mask;

    // GL_ENABLE_BIT
    bool blend, alphaTest, depthTest, stencilTest;
    bool fog, lighting, lightsEnabled[MAXLIGHTS], normalize, colorMaterial;
    bool cullFace, scissorTest, polygonOffsetFill, texture2D;

    // GL_COLOR_BUFFER_BIT
    GLenum   blendSFactor, blendDFactor, blendEquation;
    GLenum   alphaFunc;
    GLclampf alphaFuncRef;
    GLboolean colorMaskR, colorMaskG, colorMaskB, colorMaskA;
    GLfloat  clearColor[4];

    // GL_DEPTH_BUFFER_BIT
    GLenum depthFunc;
    bool   depthMask;

    // GL_STENCIL_BUFFER_BIT
    GLenum stencilFunc;
    GLint  stencilRef;
    GLuint stencilValueMask, stencilWriteMask;
    GLenum stencilFail, stencilZFail, stencilZPass;

    // GL_FOG_BIT
    GLenum  fogMode;
    GLfloat fogDensity, fogStart, fogEnd, fogColor[4];

    // GL_LIGHTING_BIT
    GLfloat lightModelAmbient[4];
    GLfloat materialAmbient[4], materialDiffuse[4];
    GLfloat materialSpecular[4], materialEmission[4];
    GLfloat materialShininess;
    // Per-light saved state for all MAXLIGHTS lights
    GLfloat lightPosition[MAXLIGHTS][4];
    GLfloat lightDiffuse[MAXLIGHTS][4];
    GLfloat lightAmbient[MAXLIGHTS][4];
    GLfloat lightSpecular[MAXLIGHTS][4];

    // GL_CURRENT_BIT
    GLfloat currentColor[4], currentNormal[3];

    // GL_POLYGON_BIT + GL_LINE_BIT
    GLenum  polygonModeFront, polygonModeBack;
    GLenum  cullFaceMode, frontFaceMode, shadeModel;
    GLfloat lineWidth;
    GLfloat polygonOffsetFactor, polygonOffsetUnits;

    // GL_SCISSOR_BIT
    GLint   scissorX, scissorY, scissorW, scissorH;
};


static GLdouble g_clipPlaneEq[6][4] = {};      // eye-space, used for clipping
static GLdouble g_clipPlaneRaw[6][4] = {};     // original equation, returned by glGetClipPlane
static bool     g_clipPlaneEnabled[6] = {};

class RXGL
{
public:
    RXGL();
    ~RXGL();

    // ---- Subsystems -------------------------------------------------------
    RXGLRenderState        m_rs;
    RXGLMatrixState        m_mx;
    RXGLTextureSystem      m_tex;
    RXGLLightingState      m_lighting;
    OGLPrimitiveVertexBuffer m_vb;
    RXGLArrayState         m_arrays;
    RXGLDisplayListManager m_dl;
    GLuint            m_listBase;      // offset for glCallLists
    GLenum            m_glError;       // oldest unread GL error (GL spec: first error wins)
    bool              m_inPrimitive;   // true between glBegin/glEnd
    GLenum            m_logicOp;        // last glLogicOp value
    bool              m_logicOpEnabled; // GL_COLOR_LOGIC_OP state

    // Raster position (window coords, set by glRasterPos* / glWindowPos*)
    GLfloat           m_rasterX, m_rasterY;   // window-space position
    bool              m_rasterValid;           // false after invalid rasterpos
    GLfloat           m_pixelZoomX, m_pixelZoomY;


    // Texture coordinate generation state (per stage, S+T+R+Q)
    // mode: GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, GL_REFLECTION_MAP
    struct TexGenState {
        bool   enabled[4];  // S, T, R, Q
        GLenum mode[4];     // per-coordinate mode
        GLfloat objectPlane[4][4];
        GLfloat eyePlane[4][4];
        TexGenState() { memset(this, 0, sizeof(*this)); }
    } m_texGen;

    // Attrib stack
    RXGLAttribSnapshot m_attribStack[RXGL_ATTRIB_STACK_DEPTH];
    int                  m_attribStackDepth;

    // Client attrib stack -- saves/restores vertex array state
    static const int RXGL_CLIENT_ATTRIB_STACK_DEPTH = 16;
    struct RXGLClientAttribSnapshot
    {
        GLbitfield          mask;
        RXGLArrayState    arrays;  // full copy of all array pointers + enables
    };
    RXGLClientAttribSnapshot m_clientAttribStack[RXGL_CLIENT_ATTRIB_STACK_DEPTH];
    int                        m_clientAttribStackDepth;

    // ---- D3D device -------------------------------------------------------
    IDirect3DDevice8* m_dev;
    LPDIRECT3D8       m_d3d;
    D3DSURFACE_DESC   m_backBufferDesc;
    bool              m_hwTnL;
    bool              m_needBeginScene;

    // ---- Driver info -------------------------------------------------------
    const char* m_vendor;
    const char* m_renderer;
    char        m_version[64];
    const char* m_extensions;
    D3DADAPTER_IDENTIFIER8 m_dddi;

    // ---- Overall render dirty flag ----------------------------------------
    bool m_renderStateDirty;

    // ---- Helpers ----------------------------------------------------------
    void InterpretError(HRESULT hr);
    void MarkDirty();
    void FlushVB();                    // flush pending geometry
    void ApplyAllState();              // push all dirty state to D3D
    void EnsureBeginScene();
    void EnsureDriverInfo();

    HRESULT InitD3D();

    // ---- Display list dispatch --------------------------------------------
    static void DLDispatch(const RXGLDisplayListCmd& c, void* ctx);

    // ---- GL methods -------------------------------------------------------

    // State
    void glAlphaFunc(GLenum f, GLclampf r)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_ALPHAFUNC,f,0,0,0,r,0,0,0); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetAlphaFunc(f,r);
    }
    void glBlendFunc(GLenum sf, GLenum df)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_BLENDFUNC,sf,df); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetBlendFunc(sf,df);
        m_tex.GetState().SetMainBlend(m_rs.blend);
    }
    void glBlendEquation(GLenum mode)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_BLENDEQUATION,mode); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetBlendEquation(mode);
    }
    void glBlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        if (!m_dev) return;
        m_dev->SetRenderState((D3DRENDERSTATETYPE)75, D3DCOLOR_COLORVALUE(r,g,b,a));
    }
    void glLogicOp(GLenum opcode)
    {
        // D3DRS_LOGICOP=150, D3DLOGICOP_* values confirmed in XDK D3D8Types.h.
        // GL opcode values 0x1500-0x150F match D3DLOGICOP values exactly.
        if (m_logicOp != opcode)
        {
            m_logicOp = opcode;
            if (m_logicOpEnabled)
            {
                // Flush pending geometry so it draws with OLD op,
                // then set new op immediately for next draw.
                if (m_vb.GetVertexCount() > 0)
                    FlushVB();
                if (m_dev)
                    m_dev->SetRenderState((D3DRENDERSTATETYPE)150, (DWORD)opcode);
            }
        }
    }
    void glCullFace(GLenum m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_CULLFACE,m); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetCullFace(m);
    }
    void glFrontFace(GLenum m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_FRONTFACE,m); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetFrontFace(m);
    }
    void glDepthFunc(GLenum f)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_DEPTHFUNC,f); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetDepthFunc(f);
    }
    void glDepthMask(GLboolean f)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_DEPTHMASK,0,0,(GLint)f); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetDepthMask(f);
    }
    void glDepthRange(GLclampd n, GLclampd f)        { MarkDirty(); m_rs.SetDepthRange(n,f); }
    void glClearDepth(GLclampd d)                    { m_rs.SetClearDepth(d); }
    void glStencilFunc(GLenum f, GLint r, GLuint m)  { MarkDirty(); m_rs.SetStencilFunc(f,r,m); }
    void glStencilMask(GLuint m)                     { MarkDirty(); m_rs.SetStencilMask(m); }
    void glStencilOp(GLenum f, GLenum z, GLenum p)   { MarkDirty(); m_rs.SetStencilOp(f,z,p); }
    void glClearStencil(GLint s)                     { m_rs.SetClearStencil(s); }
    void glScissor(GLint x, GLint y, GLsizei w, GLsizei h) { MarkDirty(); m_rs.SetScissor(x,y,w,h); }
    void glFogf(GLenum p, GLfloat v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_FOGF,p,0,0,0,v,0,0,0); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetFogf(p,v);
    }
    void glFogi(GLenum p, GLint v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_FOGI,p,0,v); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetFogi(p,v);
    }
    void glFogfv(GLenum p, const GLfloat* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_FOGFV,p,0,0,0,v[0],v[1],v[2],v[3]); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetFogfv(p,v);
    }
    void glColorMask(GLboolean r,GLboolean g,GLboolean b,GLboolean a)
                                                     { MarkDirty(); m_rs.SetColorMask(r,g,b,a); }
    void glShadeModel(GLenum m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_SHADEMODEL,m); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_rs.SetShadeModel(m);
    }
    void glPolygonMode(GLenum f, GLenum m)           { MarkDirty(); m_rs.SetPolygonMode(f,m); }
    void glPolygonOffset(GLfloat f, GLfloat u)       { MarkDirty(); m_rs.SetPolygonOffset(f,u); }
    void glPixelStorei(GLenum p, GLint v)            { m_rs.SetPixelStorei(p,v); }
    void glViewport(GLint x, GLint y, GLsizei w, GLsizei h)
                                                     { MarkDirty(); m_rs.SetViewport(x,y,w,h,m_mx.projectionDirty); }

    void glEnable(GLenum cap)  { glEnableDisable(cap, true); }
    void glDisable(GLenum cap) { glEnableDisable(cap, false); }
    void glEnableDisable(GLenum cap, bool value);

    GLboolean glIsEnabled(GLenum cap);

    // Clear
    void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
                                                     { m_rs.SetClearColor(r,g,b,a); }
    void glClear(GLbitfield mask);

    // Color
    void glColor3f(GLfloat r, GLfloat g, GLfloat b);
    void glColor3fv(const GLfloat* v)  { glColor3f(v[0],v[1],v[2]); }
    void glColor3d(GLdouble r, GLdouble g, GLdouble b) { glColor3f((GLfloat)r,(GLfloat)g,(GLfloat)b); }
    void glColor3dv(const GLdouble* v) { glColor3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
    // Signed byte: [-128..127] -> [-1..1]; unsigned byte: [0..255] -> [0..1]
    void glColor3b(GLbyte r, GLbyte g, GLbyte b)
    {
        float fr=r<0?0.0f:r/127.0f, fg=g<0?0.0f:g/127.0f, fb=b<0?0.0f:b/127.0f;
        glColor3f(fr,fg,fb);
    }
    void glColor3bv(const GLbyte* v)   { glColor3b(v[0],v[1],v[2]); }
    void glColor3s(GLshort r, GLshort g, GLshort b)
    {
        // GL spec: signed short maps [-32768,32767] to [-1,1] via /32767, clamped to [0,1] for color
        float fr = r < 0 ? 0.0f : r/32767.0f;
        float fg = g < 0 ? 0.0f : g/32767.0f;
        float fb = b < 0 ? 0.0f : b/32767.0f;
        glColor3f(fr,fg,fb);
    }
    void glColor3sv(const GLshort* v)  { glColor3s(v[0],v[1],v[2]); }
    void glColor3us(GLushort r, GLushort g, GLushort b)
    {
        glColor3f(r/65535.0f, g/65535.0f, b/65535.0f);
    }
    void glColor3usv(const GLushort* v){ glColor3us(v[0],v[1],v[2]); }    void glColor3ui(GLuint r, GLuint g, GLuint b)
    {
        glColor3f(r/4294967295.0f, g/4294967295.0f, b/4294967295.0f);
    }
    void glColor3uiv(const GLuint* v)  { glColor3ui(v[0],v[1],v[2]); }
    void glColor3i(GLint r, GLint g, GLint b)
    {
        float fr=r<0?0.0f:r/2147483647.0f, fg=g<0?0.0f:g/2147483647.0f, fb=b<0?0.0f:b/2147483647.0f;
        glColor3f(fr,fg,fb);
    }
    void glColor3iv(const GLint* v) { glColor3i(v[0],v[1],v[2]); }
    void glColor3ub(GLubyte r, GLubyte g, GLubyte b)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_COLOR3UB; c.ub[0]=r; c.ub[1]=g; c.ub[2]=b; c.ub[3]=255;
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        GLfloat fr=r/255.0f, fg=g/255.0f, fb=b/255.0f;
        m_lighting.currentColor[0]=fr; m_lighting.currentColor[1]=fg;
        m_lighting.currentColor[2]=fb; m_lighting.currentColor[3]=1.0f;
        m_vb.SetColor(RGBA_MAKE(r,g,b,0xff));
        if (m_lighting.colorMaterial) m_lighting.UpdateMaterialFromColor(fr,fg,fb,1.0f);
    }
    void glColor3ubv(const GLubyte* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_COLOR3UBV; c.ub[0]=v[0]; c.ub[1]=v[1]; c.ub[2]=v[2]; c.ub[3]=255;
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        GLfloat fr=v[0]/255.0f, fg=v[1]/255.0f, fb=v[2]/255.0f;
        m_lighting.currentColor[0]=fr; m_lighting.currentColor[1]=fg;
        m_lighting.currentColor[2]=fb; m_lighting.currentColor[3]=1.0f;
        m_vb.SetColor(RGBA_MAKE(v[0],v[1],v[2],0xff));
        if (m_lighting.colorMaterial) m_lighting.UpdateMaterialFromColor(fr,fg,fb,1.0f);
    }
    void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    void glColor4fv(const GLfloat* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RecordDLCmd(DL_OP_COLOR4FV,0,0,0,0,v[0],v[1],v[2],v[3]);
            if (m_dl.IsCompileOnly()) return;
        }
        m_lighting.currentColor[0]=v[0]; m_lighting.currentColor[1]=v[1];
        m_lighting.currentColor[2]=v[2]; m_lighting.currentColor[3]=v[3];
        m_vb.SetColor(D3DRGBA(v[0],v[1],v[2],v[3]));
        if (m_lighting.colorMaterial) m_lighting.UpdateMaterialFromColor(v[0],v[1],v[2],v[3]);
    }
    void glColor4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a) { glColor4f((GLfloat)r,(GLfloat)g,(GLfloat)b,(GLfloat)a); }
    void glColor4dv(const GLdouble* v) { glColor4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
    void glColor4b(GLbyte r, GLbyte g, GLbyte b, GLbyte a)
    {
        float fr=r<0?0.0f:r/127.0f, fg=g<0?0.0f:g/127.0f;
        float fb=b<0?0.0f:b/127.0f, fa=a<0?0.0f:a/127.0f;
        glColor4f(fr,fg,fb,fa);
    }
    void glColor4bv(const GLbyte* v)   { glColor4b(v[0],v[1],v[2],v[3]); }
    void glColor4s(GLshort r, GLshort g, GLshort b, GLshort a)
    {
        float fr = r < 0 ? 0.0f : r/32767.0f;
        float fg = g < 0 ? 0.0f : g/32767.0f;
        float fb = b < 0 ? 0.0f : b/32767.0f;
        float fa = a < 0 ? 0.0f : a/32767.0f;
        glColor4f(fr,fg,fb,fa);
    }
    void glColor4sv(const GLshort* v)  { glColor4s(v[0],v[1],v[2],v[3]); }
    void glColor4us(GLushort r, GLushort g, GLushort b, GLushort a)
    {
        glColor4f(r/65535.0f, g/65535.0f, b/65535.0f, a/65535.0f);
    }
    void glColor4usv(const GLushort* v){ glColor4us(v[0],v[1],v[2],v[3]); }
    void glColor4ui(GLuint r, GLuint g, GLuint b, GLuint a)
    {
        glColor4f(r/4294967295.0f, g/4294967295.0f, b/4294967295.0f, a/4294967295.0f);
    }
    void glColor4uiv(const GLuint* v)  { glColor4ui(v[0],v[1],v[2],v[3]); }
    void glColor4i(GLint r, GLint g, GLint b, GLint a)
    {
        float fr=r<0?0.0f:r/2147483647.0f, fg=g<0?0.0f:g/2147483647.0f;
        float fb=b<0?0.0f:b/2147483647.0f, fa=a<0?0.0f:a/2147483647.0f;
        glColor4f(fr,fg,fb,fa);
    }
    void glColor4iv(const GLint* v)    { glColor4i(v[0],v[1],v[2],v[3]); }
    void glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_COLOR4UB; c.ub[0]=r; c.ub[1]=g; c.ub[2]=b; c.ub[3]=a;
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        GLfloat fr=r/255.0f, fg=g/255.0f, fb=b/255.0f, fa=a/255.0f;
        m_lighting.currentColor[0]=fr; m_lighting.currentColor[1]=fg;
        m_lighting.currentColor[2]=fb; m_lighting.currentColor[3]=fa;
        m_vb.SetColor(RGBA_MAKE(r,g,b,a));
        if (m_lighting.colorMaterial) m_lighting.UpdateMaterialFromColor(fr,fg,fb,fa);
    }
    void glColor4ubv(const GLubyte* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_COLOR4UBV; c.ub[0]=v[0]; c.ub[1]=v[1]; c.ub[2]=v[2]; c.ub[3]=v[3];
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        GLfloat fr=v[0]/255.0f, fg=v[1]/255.0f, fb=v[2]/255.0f, fa=v[3]/255.0f;
        m_lighting.currentColor[0]=fr; m_lighting.currentColor[1]=fg;
        m_lighting.currentColor[2]=fb; m_lighting.currentColor[3]=fa;
        m_vb.SetColor(RGBA_MAKE(v[0],v[1],v[2],v[3]));
        if (m_lighting.colorMaterial) m_lighting.UpdateMaterialFromColor(fr,fg,fb,fa);
    }

    // Vertex immediate mode
    void glBegin(GLenum mode);
    void glEnd();
    void glTexCoord2f(GLfloat s, GLfloat t);
    void glTexCoord2fv(const GLfloat* v)  { glTexCoord2f(v[0], v[1]); }
    void glTexCoord2i(GLint s, GLint t)   { glTexCoord2f((GLfloat)s, (GLfloat)t); }
    void glTexCoord2s(GLshort s, GLshort t){ glTexCoord2f((GLfloat)s, (GLfloat)t); }
    void glTexCoord1f(GLfloat s)          { glTexCoord2f(s, 0.0f); }
    void glTexCoord3f(GLfloat s, GLfloat t, GLfloat /*r*/) { glTexCoord2f(s, t); }
    void glTexCoord4f(GLfloat s, GLfloat t, GLfloat /*r*/, GLfloat /*q*/) { glTexCoord2f(s, t); }
    void glTexCoord1d(GLdouble s)                                { glTexCoord2f((GLfloat)s, 0.0f); }
    void glTexCoord2d(GLdouble s, GLdouble t)                    { glTexCoord2f((GLfloat)s,(GLfloat)t); }
    void glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)        { glTexCoord2f((GLfloat)s,(GLfloat)t); }
    void glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q) { glTexCoord2f((GLfloat)s,(GLfloat)t); }
    void glTexCoord1fv(const GLfloat* v)                         { glTexCoord2f(v[0], 0.0f); }
    void glTexCoord3fv(const GLfloat* v)                         { glTexCoord2f(v[0], v[1]); }
    void glTexCoord4fv(const GLfloat* v)                         { glTexCoord2f(v[0], v[1]); }
    void glTexCoord2iv(const GLint* v)                           { glTexCoord2f((GLfloat)v[0],(GLfloat)v[1]); }
    void glTexCoord2sv(const GLshort* v)                         { glTexCoord2f((GLfloat)v[0],(GLfloat)v[1]); }
    void glTexCoord2dv(const GLdouble* v)                        { glTexCoord2f((GLfloat)v[0],(GLfloat)v[1]); }
    void glVertex2f(GLfloat x, GLfloat y);
    void glVertex2fv(const GLfloat* v)   { glVertex2f(v[0], v[1]); }
    void glVertex2i(GLint x, GLint y)    { glVertex2f((GLfloat)x, (GLfloat)y); }
    void glVertex2iv(const GLint* v)     { glVertex2f((GLfloat)v[0], (GLfloat)v[1]); }
    void glVertex2s(GLshort x, GLshort y){ glVertex2f((GLfloat)x, (GLfloat)y); }
    void glVertex2d(GLdouble x, GLdouble y)             { glVertex2f((GLfloat)x,(GLfloat)y); }
    void glVertex2dv(const GLdouble* v)                 { glVertex2f((GLfloat)v[0],(GLfloat)v[1]); }
    void glVertex2sv(const GLshort* v)                  { glVertex2f((GLfloat)v[0],(GLfloat)v[1]); }
    void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
    void glVertex3fv(const GLfloat* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_VERTEX3FV,0,0,0,0,v[0],v[1],v[2],0); if (m_dl.IsCompileOnly()) return; }
        EmitVertex(v[0], v[1], v[2]);
    }
    void glVertex3i(GLint x, GLint y, GLint z)       { glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glVertex3iv(const GLint* v)                  { glVertex3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
    void glVertex3s(GLshort x, GLshort y, GLshort z) { glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glVertex3d(GLdouble x, GLdouble y, GLdouble z) { glVertex3f((GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glVertex3dv(const GLdouble* v)                  { glVertex3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
    void glVertex3sv(const GLshort* v)                   { glVertex3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
    void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
    {
        // Perspective divide for w != 1
        if (w != 0.0f && w != 1.0f) { GLfloat rw = 1.0f/w; glVertex3f(x*rw, y*rw, z*rw); }
        else glVertex3f(x, y, z);
    }
    void glVertex4fv(const GLfloat* v) { glVertex4f(v[0],v[1],v[2],v[3]); }
    void glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w) { glVertex4f((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
    void glVertex4dv(const GLdouble* v) { glVertex4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
    void glVertex4i(GLint x, GLint y, GLint z, GLint w) { glVertex4f((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
    void glVertex4iv(const GLint* v)    { glVertex4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
    void glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w) { glVertex4f((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
    void glVertex4sv(const GLshort* v)  { glVertex4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
    void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_NORMAL3F,0,0,0,0,nx,ny,nz,0); if (m_dl.IsCompileOnly()) return; }
        m_lighting.SetNormal3f(nx,ny,nz);
    }
    void glNormal3fv(const GLfloat* v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_NORMAL3F,0,0,0,0,v[0],v[1],v[2],0); if (m_dl.IsCompileOnly()) return; }
        m_lighting.SetNormal3f(v[0],v[1],v[2]);
    }
    // Normal variants -- all forward to glNormal3f after converting to float.
    // GLbyte  range [-128..127]  maps to [-1..1] via /127.0f
    // GLshort range [-32768..32767] maps to [-1..1] via /32767.0f
    // GLint   range [-2^31..2^31-1] maps to [-1..1] via /2147483647.0f
    void glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
    {
        glNormal3f(nx/127.0f, ny/127.0f, nz/127.0f);
    }
    void glNormal3bv(const GLbyte* v)  { glNormal3b(v[0],v[1],v[2]); }
    void glNormal3s(GLshort nx, GLshort ny, GLshort nz)
    {
        glNormal3f(nx/32767.0f, ny/32767.0f, nz/32767.0f);
    }
    void glNormal3sv(const GLshort* v) { glNormal3s(v[0],v[1],v[2]); }
    void glNormal3i(GLint nx, GLint ny, GLint nz)
    {
        glNormal3f(nx/2147483647.0f, ny/2147483647.0f, nz/2147483647.0f);
    }
    void glNormal3iv(const GLint* v)   { glNormal3i(v[0],v[1],v[2]); }
    void glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
    {
        glNormal3f((GLfloat)nx,(GLfloat)ny,(GLfloat)nz);
    }
    void glNormal3dv(const GLdouble* v) { glNormal3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }

    // Lighting
    void glLightfv(GLenum light, GLenum pname, const GLfloat* p)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            // Store light index in i0, pname in e1, 4 floats in f[]
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c));
            c.op = DL_OP_LIGHTFV; c.e0 = light; c.e1 = pname;
            if (p) { c.f[0]=p[0]; c.f[1]=p[1]; c.f[2]=p[2]; c.f[3]=p[3]; }
            m_dl.Record(c);
            if (m_dl.IsCompileOnly()) return;
        }
        m_lighting.SetLightfv(light, pname, p);
    }
    void glLightf(GLenum light, GLenum pname, GLfloat param)     { m_lighting.SetLightf(light, pname, param); }
    void glLightModelfv(GLenum pname, const GLfloat* p)          { m_lighting.SetLightModelfv(pname,p); }
    void glLightModelf(GLenum pname, GLfloat p)                  { m_lighting.SetLightModelf(pname,p); }
    void glLightModeli(GLenum pname, GLint p)                    { m_lighting.SetLightModeli(pname,p); }
    void glMaterialfv(GLenum face, GLenum pname, const GLfloat* p)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RecordDLCmd(DL_OP_MATERIALFV, face, pname, 0, 0, p[0], p[1], p[2], p[3]);
            if (m_dl.IsCompileOnly()) return;
        }
        m_lighting.SetMaterialfv(face,pname,p);
    }
    void glMaterialf(GLenum face, GLenum pname, GLfloat p)       { m_lighting.SetMaterialf(face,pname,p); }
    void glColorMaterial(GLenum face, GLenum mode)               { m_lighting.SetColorMaterial(face,mode); }

    void glGetLightfv(GLenum light, GLenum pname, GLfloat* params)
    {
        int i = (int)(light - GL_LIGHT0);
        if (i < 0 || i >= MAXLIGHTS) return;
        const RXGLLight& L = m_lighting.lights[i];
        switch (pname) {
        case GL_AMBIENT:               params[0]=L.ambient[0];  params[1]=L.ambient[1];  params[2]=L.ambient[2];  params[3]=L.ambient[3];  break;
        case GL_DIFFUSE:               params[0]=L.diffuse[0];  params[1]=L.diffuse[1];  params[2]=L.diffuse[2];  params[3]=L.diffuse[3];  break;
        case GL_SPECULAR:              params[0]=L.specular[0]; params[1]=L.specular[1]; params[2]=L.specular[2]; params[3]=L.specular[3]; break;
        case GL_POSITION:              params[0]=L.position[0]; params[1]=L.position[1]; params[2]=L.position[2]; params[3]=L.position[3]; break;
        case GL_SPOT_DIRECTION:        params[0]=L.spotDirection[0]; params[1]=L.spotDirection[1]; params[2]=L.spotDirection[2]; break;
        case GL_SPOT_EXPONENT:         params[0]=L.spotExponent; break;
        case GL_SPOT_CUTOFF:           params[0]=L.spotCutoff; break;
        case GL_CONSTANT_ATTENUATION:  params[0]=L.constAtten; break;
        case GL_LINEAR_ATTENUATION:    params[0]=L.linearAtten; break;
        case GL_QUADRATIC_ATTENUATION: params[0]=L.quadAtten; break;
        default: break;
        }
    }

    void glGetMaterialfv(GLenum face, GLenum pname, GLfloat* params)
    {
        (void)face;
        switch (pname) {
        case GL_AMBIENT:   params[0]=m_lighting.materialAmbient[0];  params[1]=m_lighting.materialAmbient[1];  params[2]=m_lighting.materialAmbient[2];  params[3]=m_lighting.materialAmbient[3];  break;
        case GL_DIFFUSE:   params[0]=m_lighting.materialDiffuse[0];  params[1]=m_lighting.materialDiffuse[1];  params[2]=m_lighting.materialDiffuse[2];  params[3]=m_lighting.materialDiffuse[3];  break;
        case GL_SPECULAR:  params[0]=m_lighting.materialSpecular[0]; params[1]=m_lighting.materialSpecular[1]; params[2]=m_lighting.materialSpecular[2]; params[3]=m_lighting.materialSpecular[3]; break;
        case GL_EMISSION:  params[0]=m_lighting.materialEmission[0]; params[1]=m_lighting.materialEmission[1]; params[2]=m_lighting.materialEmission[2]; params[3]=m_lighting.materialEmission[3]; break;
        case GL_SHININESS: params[0]=m_lighting.materialShininess; break;
        default: break;
        }
    }

    void glGetLightiv(GLenum light, GLenum pname, GLint* params)
    {
        GLfloat f[4]={0,0,0,0}; glGetLightfv(light,pname,f);
        params[0]=(GLint)f[0]; params[1]=(GLint)f[1]; params[2]=(GLint)f[2]; params[3]=(GLint)f[3];
    }
    void glGetMaterialiv(GLenum face, GLenum pname, GLint* params)
    {
        GLfloat f[4]={0,0,0,0}; glGetMaterialfv(face,pname,f);
        params[0]=(GLint)f[0]; params[1]=(GLint)f[1]; params[2]=(GLint)f[2]; params[3]=(GLint)f[3];
    }
    void glGetDoublev(GLenum pname, GLdouble* params)
    {
        // Implemented inline to avoid forward-reference to glGetFloatv
        switch (pname) {
        case GL_MODELVIEW_MATRIX:
            { const D3DXMATRIX* m=m_mx.modelView->GetTop();
              for(int i=0;i<16;i++) params[i]=(GLdouble)((const float*)m)[i]; } break;
        case GL_PROJECTION_MATRIX:
            { const D3DXMATRIX* m=m_mx.projection->GetTop();
              for(int i=0;i<16;i++) params[i]=(GLdouble)((const float*)m)[i]; } break;
        case GL_TEXTURE_MATRIX:
            { const D3DXMATRIX* m=m_mx.GetTextureStack()->GetTop();
              for(int i=0;i<16;i++) params[i]=(GLdouble)((const float*)m)[i]; } break;
        default: params[0]=0.0; break;
        }
    }
    void glGetTexEnvfv(GLenum target, GLenum pname, GLfloat* params)
    {
        (void)target;
        switch (pname) {
        case GL_TEXTURE_ENV_MODE:  params[0]=(GLfloat)m_tex.GetState().Get()->GetTextEnvMode(); break;
        case GL_TEXTURE_ENV_COLOR: { const GLfloat* c=m_tex.GetState().Get()->GetEnvColor();
                                     params[0]=c[0];params[1]=c[1];params[2]=c[2];params[3]=c[3]; } break;
        default: params[0]=0.0f; break;
        }
    }
    void glGetTexEnviv(GLenum target, GLenum pname, GLint* params)
    {
        GLfloat f[4]={0,0,0,0}; glGetTexEnvfv(target,pname,f);
        params[0]=(GLint)f[0]; params[1]=(GLint)f[1]; params[2]=(GLint)f[2]; params[3]=(GLint)f[3];
    }
    void glGetTexGenfv(GLenum coord, GLenum pname, GLfloat* params)
    {
        if (!params) return;
        int c=(coord==GL_S)?0:(coord==GL_T)?1:(coord==GL_R)?2:3;
        if (pname==GL_TEXTURE_GEN_MODE)
            { params[0]=(GLfloat)m_texGen.mode[c]; }
        else if (pname==GL_OBJECT_PLANE)
            { params[0]=m_texGen.objectPlane[c][0]; params[1]=m_texGen.objectPlane[c][1];
              params[2]=m_texGen.objectPlane[c][2]; params[3]=m_texGen.objectPlane[c][3]; }
        else if (pname==GL_EYE_PLANE)
            { params[0]=m_texGen.eyePlane[c][0]; params[1]=m_texGen.eyePlane[c][1];
              params[2]=m_texGen.eyePlane[c][2]; params[3]=m_texGen.eyePlane[c][3]; }
    }
    void glGetTexGendv(GLenum coord, GLenum pname, GLdouble* params)
    { GLfloat f[4]={0,0,0,0}; glGetTexGenfv(coord,pname,f);
      int n=(pname==GL_TEXTURE_GEN_MODE)?1:4;
      for(int i=0;i<n;i++) params[i]=(GLdouble)f[i]; }
    void glGetTexGeniv(GLenum coord, GLenum pname, GLint* params)
    { GLfloat f[4]={0,0,0,0}; glGetTexGenfv(coord,pname,f);
      int n=(pname==GL_TEXTURE_GEN_MODE)?1:4;
      for(int i=0;i<n;i++) params[i]=(GLint)f[i]; }
    void glGetPointerv(GLenum pname, GLvoid** params)
    {
        if (!params) return;
        switch (pname) {
        case 0x808E: *params=(GLvoid*)m_arrays.vertex.pointer;      break; // GL_VERTEX_ARRAY_POINTER
        case 0x808F: *params=(GLvoid*)m_arrays.normal.pointer;      break; // GL_NORMAL_ARRAY_POINTER
        case 0x8090: *params=(GLvoid*)m_arrays.color.pointer;       break; // GL_COLOR_ARRAY_POINTER
        case 0x8092: *params=(GLvoid*)m_arrays.texCoord[0].pointer; break; // GL_TEXTURE_COORD_ARRAY_POINTER
        default: *params=NULL; break;
        }
    }
    GLboolean glIsList(GLuint list)
    {
        if (list == 0 || list >= RXGL_MAX_DISPLAY_LISTS) return GL_FALSE;
        return GL_TRUE;
    }
    void glGetTexLevelParameterfv(GLenum target,GLint level,GLenum pname,GLfloat* params)
    {
        (void)target;(void)level;
        switch(pname){
        case GL_TEXTURE_INTERNAL_FORMAT: params[0]=(GLfloat)GL_RGBA; break;
        default: params[0]=0.0f; break;
        }
    }
    void glGetTexLevelParameteriv(GLenum target,GLint level,GLenum pname,GLint* params)
        { GLfloat f=0; glGetTexLevelParameterfv(target,level,pname,&f); params[0]=(GLint)f; }
    void glLineWidth(GLfloat w)  { MarkDirty(); m_rs.SetLineWidth(w); }
    void glPointSize(GLfloat s)  { MarkDirty(); m_rs.SetPointSize(s); }

    // Vertex arrays
    void glEnableClientState(GLenum a)               { m_arrays.EnableClientState(a); }
    void glDisableClientState(GLenum a)              { m_arrays.DisableClientState(a); }
    void glVertexPointer(GLint s, GLenum t, GLsizei st, const GLvoid* p)  { m_arrays.SetVertexPointer(s,t,st,p); }
    void glNormalPointer(GLenum t, GLsizei st, const GLvoid* p)            { m_arrays.SetNormalPointer(t,st,p); }
    void glColorPointer(GLint s, GLenum t, GLsizei st, const GLvoid* p)   { m_arrays.SetColorPointer(s,t,st,p); }
    void glTexCoordPointer(GLint s, GLenum t, GLsizei st, const GLvoid* p){ m_arrays.SetTexCoordPointer(s,t,st,p); }
    void glDrawArrays(GLenum mode, GLint first, GLsizei count);
    void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

    // Matrix
    void glMatrixMode(GLenum m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_MATRIXMODE, m); if (m_dl.IsCompileOnly()) return; }
        m_mx.SetMatrixMode(m);
    }
    void glLoadIdentity()
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_LOADIDENTITY); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_mx.LoadIdentity();
    }
    void glPushMatrix()
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_PUSHMATRIX); if (m_dl.IsCompileOnly()) return; }
        m_mx.PushMatrix();
    }
    void glPopMatrix()
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_POPMATRIX); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_mx.PopMatrix();
    }
    void glLoadMatrixf(const GLfloat* m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_LOADMATRIXF;
            for(int i=0;i<16;i++) c.f[i]=m[i];
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        MarkDirty(); m_mx.LoadMatrixf(m);
    }
    void glMultMatrixf(const GLfloat* m)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_MULTMATRIXF;
            for(int i=0;i<16;i++) c.f[i]=m[i];
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        MarkDirty(); m_mx.MultMatrixf(m);
    }
    void glMultMatrixd(const GLdouble* m)      { MarkDirty(); m_mx.MultMatrixd(m); }
    void glLoadMatrixd(const GLdouble* m)
        { GLfloat f[16]; for(int i=0;i<16;i++) f[i]=(GLfloat)m[i]; glLoadMatrixf(f); }
    void glLoadTransposeMatrixf(const GLfloat* m)
    {
        GLfloat t[16];
        t[0]=m[0]; t[1]=m[4]; t[2]=m[8];  t[3]=m[12];
        t[4]=m[1]; t[5]=m[5]; t[6]=m[9];  t[7]=m[13];
        t[8]=m[2]; t[9]=m[6]; t[10]=m[10];t[11]=m[14];
        t[12]=m[3];t[13]=m[7];t[14]=m[11];t[15]=m[15];
        glLoadMatrixf(t);
    }
    void glLoadTransposeMatrixd(const GLdouble* m)
    {
        GLfloat t[16];
        t[0]=(GLfloat)m[0];  t[1]=(GLfloat)m[4];  t[2]=(GLfloat)m[8];  t[3]=(GLfloat)m[12];
        t[4]=(GLfloat)m[1];  t[5]=(GLfloat)m[5];  t[6]=(GLfloat)m[9];  t[7]=(GLfloat)m[13];
        t[8]=(GLfloat)m[2];  t[9]=(GLfloat)m[6];  t[10]=(GLfloat)m[10];t[11]=(GLfloat)m[14];
        t[12]=(GLfloat)m[3]; t[13]=(GLfloat)m[7]; t[14]=(GLfloat)m[11];t[15]=(GLfloat)m[15];
        glLoadMatrixf(t);
    }
    void glMultTransposeMatrixf(const GLfloat* m)
    {
        GLfloat t[16];
        t[0]=m[0]; t[1]=m[4]; t[2]=m[8];  t[3]=m[12];
        t[4]=m[1]; t[5]=m[5]; t[6]=m[9];  t[7]=m[13];
        t[8]=m[2]; t[9]=m[6]; t[10]=m[10];t[11]=m[14];
        t[12]=m[3];t[13]=m[7];t[14]=m[11];t[15]=m[15];
        glMultMatrixf(t);
    }
    void glMultTransposeMatrixd(const GLdouble* m)
    {
        GLfloat t[16];
        t[0]=(GLfloat)m[0];  t[1]=(GLfloat)m[4];  t[2]=(GLfloat)m[8];  t[3]=(GLfloat)m[12];
        t[4]=(GLfloat)m[1];  t[5]=(GLfloat)m[5];  t[6]=(GLfloat)m[9];  t[7]=(GLfloat)m[13];
        t[8]=(GLfloat)m[2];  t[9]=(GLfloat)m[6];  t[10]=(GLfloat)m[10];t[11]=(GLfloat)m[14];
        t[12]=(GLfloat)m[3]; t[13]=(GLfloat)m[7]; t[14]=(GLfloat)m[11];t[15]=(GLfloat)m[15];
        glMultMatrixf(t);
    }
    void glRotated(GLdouble a,GLdouble x,GLdouble y,GLdouble z)
        { glRotatef((GLfloat)a,(GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glScaled(GLdouble x,GLdouble y,GLdouble z)
        { glScalef((GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glTranslated(GLdouble x,GLdouble y,GLdouble z)
        { glTranslatef((GLfloat)x,(GLfloat)y,(GLfloat)z); }
    void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_TRANSLATEF,0,0,0,0,x,y,z,0); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_mx.Translatef(x,y,z);
    }
    void glScalef(GLfloat x, GLfloat y, GLfloat z)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_SCALEF,0,0,0,0,x,y,z,0); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_mx.Scalef(x,y,z);
    }
    void glRotatef(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_ROTATEF,0,0,0,0,a,x,y,z); if (m_dl.IsCompileOnly()) return; }
        MarkDirty(); m_mx.Rotatef(a,x,y,z);
    }
    void glOrtho(GLdouble l,GLdouble r,GLdouble b,GLdouble t,GLdouble zn,GLdouble zf)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_ORTHO;
            c.f[0]=(GLfloat)l; c.f[1]=(GLfloat)r; c.f[2]=(GLfloat)b;
            c.f[3]=(GLfloat)t; c.f[4]=(GLfloat)zn; c.f[5]=(GLfloat)zf;
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        MarkDirty(); m_mx.Ortho(l,r,b,t,zn,zf);
    }
    void glFrustum(GLdouble l,GLdouble r,GLdouble b,GLdouble t,GLdouble zn,GLdouble zf)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RXGLDisplayListCmd c; memset(&c,0,sizeof(c)); c.op=DL_OP_FRUSTUM;
            c.f[0]=(GLfloat)l; c.f[1]=(GLfloat)r; c.f[2]=(GLfloat)b;
            c.f[3]=(GLfloat)t; c.f[4]=(GLfloat)zn; c.f[5]=(GLfloat)zf;
            m_dl.Record(c); if (m_dl.IsCompileOnly()) return;
        }
        MarkDirty(); m_mx.Frustum(l,r,b,t,zn,zf);
    }

    // Textures
    void glBindTexture(GLenum target, GLuint texture) { m_tex.BindTexture(target, texture, m_renderStateDirty); }
    void glGenerateMipmap(GLenum target)
    {
        (void)target;
        TextureEntry* e = m_tex.GetTable().GetCurrentEntry();
        if (!e || !e->m_mipMap) return;
        // Only call D3DXFilterTexture if the texture has >1 mip level.
        // On NV2A, calling it on a 1-level texture writes past the end of
        // the allocation and corrupts adjacent textures in VRAM.
        // GetLevelCount()==1 means the texture was created without mips --
        // glGenerateMipmap is a no-op in that case (nothing to generate).
        if (e->m_mipMap->GetLevelCount() > 1)
            D3DXFilterTexture(e->m_mipMap, NULL, 0, D3DX_DEFAULT);
    }
    void glGenTextures(GLsizei n, GLuint* t)  { m_tex.GenTextures(n, t); }
    void glDeleteTextures(GLsizei n, const GLuint* t) { m_tex.DeleteTextures(n, t, m_renderStateDirty); }
    GLboolean glIsTexture(GLuint t)           { return m_tex.IsTexture(t); }
    GLboolean glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences)
    {
        // Xbox unified VRAM -- all textures always resident
        if (residences) for(GLsizei i=0;i<n;i++) residences[i]=GL_TRUE;
        return GL_TRUE;
    }
    // ---- Stubs with genuine hardware reasons --------------------------------
    // Accumulation buffer: NV2A has no accumulation hardware
    void glAccum(GLenum op, GLfloat value)          { (void)op; (void)value; }
    void glClearAccum(GLfloat r,GLfloat g,GLfloat b,GLfloat a){ (void)r;(void)g;(void)b;(void)a; }
    // 1D textures: D3D8/NV2A has no 1D texture type
    void glTexImage1D(GLenum t,GLint lv,GLint fmt,GLsizei w,GLint b,GLenum f,GLenum ty,const GLvoid* px)
        { (void)t;(void)lv;(void)fmt;(void)w;(void)b;(void)f;(void)ty;(void)px; }
    void glTexSubImage1D(GLenum t,GLint lv,GLint x,GLsizei w,GLenum f,GLenum ty,const GLvoid* px)
        { (void)t;(void)lv;(void)x;(void)w;(void)f;(void)ty;(void)px; }
    void glCopyTexSubImage1D(GLenum t,GLint lv,GLint xo,GLint x,GLint y,GLsizei w)
        { (void)t;(void)lv;(void)xo;(void)x;(void)y;(void)w; }
    // Edge flags: D3D8 has no per-edge wireframe selection
    void glEdgeFlag(GLboolean f)                    { (void)f; }
    void glEdgeFlagv(const GLboolean* f)            { (void)f; }
    void glEdgeFlagPointer(GLsizei s,const GLvoid* p){ (void)s;(void)p; }
    // Texture priority: Xbox unified VRAM -- priority is meaningless
    void glPrioritizeTextures(GLsizei n,const GLuint* t,const GLclampf* p){ (void)n;(void)t;(void)p; }
    void glTexParameterf(GLenum t, GLenum p, GLfloat v)   { m_tex.TexParameterf(t,p,v,m_renderStateDirty); }
    void glTexParameteri(GLenum t, GLenum p, GLint v)     { m_tex.TexParameteri(t,p,v,m_renderStateDirty); }
    void glTexEnvf(GLenum t, GLenum p, GLfloat v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_TEXENVF,t,p,0,0,v,0,0,0); if (m_dl.IsCompileOnly()) return; }
        m_tex.TexEnvf(t,p,v,m_renderStateDirty);
    }
    void glTexEnvi(GLenum t, GLenum p, GLint v)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting()) { RecordDLCmd(DL_OP_TEXENVI,t,p,v); if (m_dl.IsCompileOnly()) return; }
        m_tex.TexEnvi(t,p,v,m_renderStateDirty);
    }
    void glTexEnvfv(GLenum t, GLenum p, const GLfloat* v) { m_tex.TexEnvfv(t,p,v,m_renderStateDirty); }
    void glTexImage2D(GLenum tg,GLint lv,GLint ifmt,GLsizei w,GLsizei h,
                      GLint b,GLenum f,GLenum t,const GLvoid* p)
                                              { m_tex.TexImage2D(tg,lv,ifmt,w,h,b,f,t,p); }
    void glTexSubImage2D(GLenum tg,GLint lv,GLint xo,GLint yo,GLsizei w,GLsizei h,
                         GLenum f,GLenum t,const GLvoid* p)
                                              { m_tex.TexSubImage2D(tg,lv,xo,yo,w,h,f,t,p,m_vb); }
    void glCopyTexSubImage2D(GLenum target, GLint level,
                              GLint xoffset, GLint yoffset,
                              GLint x, GLint y, GLsizei width, GLsizei height)
    {
        FlushVB();              // submit any pending geometry to D3D
        m_dev->KickPushBuffer(); // flush GPU command buffer
        m_dev->BlockUntilIdle(); // wait for GPU to finish all pending draws
        // NOTE: we deliberately do NOT call EndScene/BeginScene here.
        // The backbuffer is readable after BlockUntilIdle even mid-scene.
        m_tex.CopyTexSubImage2D(target, level, xoffset, yoffset,
                                x, y, width, height, m_dev, m_backBufferDesc);
    }
    void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat,
                          GLint x, GLint y, GLsizei width, GLsizei height, GLint /*border*/)
    {
        FlushVB();
        m_tex.CopyTexImage2D(target, level, internalformat,
                             x, y, width, height, m_dev, m_d3d, m_backBufferDesc);
    }

    // Multitexture
    void glMTexCoord2fSGIS(GLenum t, GLfloat s, GLfloat u)
                                              { m_vb.SetTextureCoord((int)(t - TEXTURE0_SGIS), s, u); }
    void glSelectTextureSGIS(GLenum t)        { m_tex.SelectTextureSGIS(t); }
    void glActiveTextureARB(GLenum t)         { m_tex.ActiveTextureARB(t); }
    void glClientActiveTextureARB(GLenum t)
    {
        m_tex.ClientActiveTextureARB(t);
        m_arrays.SetClientStage((int)(t - GL_TEXTURE0_ARB));
    }
    void glMultiTexCoord2fARB(GLenum t, GLfloat s, GLfloat u)
                                              { m_vb.SetTextureCoord((int)(t - GL_TEXTURE0_ARB), s, u); }
    void glMultiTexCoord3f(GLenum t, GLfloat s, GLfloat u, GLfloat r)
        { m_vb.SetTextureCoord((int)(t - GL_TEXTURE0_ARB), s, u); (void)r; }
    void glMultiTexCoord4f(GLenum t, GLfloat s, GLfloat u, GLfloat r, GLfloat q)
        { m_vb.SetTextureCoord((int)(t - GL_TEXTURE0_ARB), s, u); (void)r; (void)q; }

    // Display lists
    GLuint glGenLists(GLsizei r)              { return m_dl.GenLists(r); }
    void glNewList(GLuint l, GLenum m)        { FlushVB(); m_dl.NewList(l, m); }
    void glEndList()                          { m_dl.EndList(); }
    void glCallList(GLuint l)
    {
        if (m_dl.IsCompiling() && !m_dl.IsExecuting())
        {
            RecordDLCmd(DL_OP_CALLLIST,0,0,0,l);
            if (m_dl.IsCompileOnly()) return;
        }
        m_dl.CallList(l, DLDispatch, this);
    }
    void glListBase(GLuint base)              { m_listBase = base; }

    // Texture coordinate generation
    void glTexGeni(GLenum coord, GLenum pname, GLint param)
    {
        int c = (int)coord - (int)GL_S;
        if (c < 0 || c > 3) return;
        if (pname == GL_TEXTURE_GEN_MODE)
        {
            m_texGen.mode[c] = (GLenum)param;
            MarkDirty();
        }
    }
    void glTexGenf(GLenum coord, GLenum pname, GLfloat param)
    {
        glTexGeni(coord, pname, (GLint)param);
    }
    void glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params)
    {
        if (!params) return;
        int c = (int)coord - (int)GL_S;
        if (c < 0 || c > 3) return;
        if (pname == GL_TEXTURE_GEN_MODE)
        {
            m_texGen.mode[c] = (GLenum)(GLint)params[0];
            MarkDirty();
        }
        else if (pname == GL_OBJECT_PLANE)
        {
            m_texGen.objectPlane[c][0] = params[0];
            m_texGen.objectPlane[c][1] = params[1];
            m_texGen.objectPlane[c][2] = params[2];
            m_texGen.objectPlane[c][3] = params[3];
        }
        else if (pname == GL_EYE_PLANE)
        {
            // GL spec: eye plane is transformed by inverse(MV) at call time
            // and stored in eye space. We then dot against eye-space vertex pos.
            // Without this transform, the plane would counter-rotate and appear static.
            const D3DXMATRIX* mv = m_mx.modelView->GetTop();
            if (mv)
            {
                D3DXMATRIX invMV;
                D3DXMatrixInverse(&invMV, NULL, mv);
                // Transform plane (nx,ny,nz,d) by transpose(invMV) = (MV)^T
                // p_eye = p_obj * transpose(inv(MV))
                GLfloat px = params[0], py = params[1], pz = params[2], pw = params[3];
                m_texGen.eyePlane[c][0] = px*invMV._11 + py*invMV._12 + pz*invMV._13 + pw*invMV._14;
                m_texGen.eyePlane[c][1] = px*invMV._21 + py*invMV._22 + pz*invMV._23 + pw*invMV._24;
                m_texGen.eyePlane[c][2] = px*invMV._31 + py*invMV._32 + pz*invMV._33 + pw*invMV._34;
                m_texGen.eyePlane[c][3] = px*invMV._41 + py*invMV._42 + pz*invMV._43 + pw*invMV._44;
            }
            else
            {
                m_texGen.eyePlane[c][0] = params[0];
                m_texGen.eyePlane[c][1] = params[1];
                m_texGen.eyePlane[c][2] = params[2];
                m_texGen.eyePlane[c][3] = params[3];
            }
        }
    }
    void glTexGeniv(GLenum coord, GLenum pname, const GLint* params)
    {
        if (!params) return;
        glTexGeni(coord, pname, params[0]);
    }
    void glCallLists(GLsizei n, GLenum type, const GLvoid* lists)
    {
        if (!lists || n <= 0) return;
        for (GLsizei i = 0; i < n; i++)
        {
            GLuint id = 0;
            switch (type)
            {
            case GL_UNSIGNED_BYTE:  id = ((const GLubyte*)lists)[i];  break;
            case GL_BYTE:           id = ((const GLbyte*)lists)[i];   break;
            case GL_UNSIGNED_SHORT: id = ((const GLushort*)lists)[i]; break;
            case GL_SHORT:          id = ((const GLshort*)lists)[i];  break;
            case GL_UNSIGNED_INT:   id = ((const GLuint*)lists)[i];   break;
            case GL_INT:            id = (GLuint)((const GLint*)lists)[i]; break;
            case GL_FLOAT:          id = (GLuint)((const GLfloat*)lists)[i]; break;
            case GL_2_BYTES:
            {
                const GLubyte* p = (const GLubyte*)lists + i * 2;
                id = ((GLuint)p[0] << 8) | p[1]; break;
            }
            case GL_3_BYTES:
            {
                const GLubyte* p = (const GLubyte*)lists + i * 3;
                id = ((GLuint)p[0] << 16) | ((GLuint)p[1] << 8) | p[2]; break;
            }
            case GL_4_BYTES:
            {
                const GLubyte* p = (const GLubyte*)lists + i * 4;
                id = ((GLuint)p[0]<<24)|((GLuint)p[1]<<16)|((GLuint)p[2]<<8)|p[3]; break;
            }
            default: continue;
            }
            m_dl.CallList(m_listBase + id, DLDispatch, this);
        }
    }
    void glDeleteLists(GLuint l, GLsizei r)   { m_dl.DeleteLists(l, r); }

    void glPushAttrib(GLbitfield mask)
    {
        if (m_attribStackDepth >= RXGL_ATTRIB_STACK_DEPTH)
        { SetError(GL_STACK_OVERFLOW); return; }

        RXGLAttribSnapshot& s = m_attribStack[m_attribStackDepth++];
        memset(&s, 0, sizeof(s));
        s.mask = mask;

        const RXGLRenderState& rs = m_rs;
        const RXGLLightingState& lt = m_lighting;

        if (mask & (GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                    GL_STENCIL_BUFFER_BIT | GL_FOG_BIT | GL_LIGHTING_BIT |
                    GL_SCISSOR_BIT | GL_POLYGON_BIT | GL_LINE_BIT))
        {
            // Enable bits
            s.blend          = rs.blend;
            s.alphaTest      = rs.alphaTest;
            s.depthTest      = rs.depthTest;
            s.stencilTest    = rs.stencilTest;
            s.fog            = rs.fog;
            s.lighting       = lt.lighting;
            s.normalize      = lt.normalize;
            s.colorMaterial  = lt.colorMaterial;
            for (int _i=0; _i<MAXLIGHTS; _i++)
                s.lightsEnabled[_i] = lt.lights[_i].enabled;
            s.cullFace       = rs.cullFace;
            s.scissorTest    = rs.scissorTest;
            s.polygonOffsetFill = rs.polygonOffsetFill;
            s.texture2D      = m_tex.GetState().GetTexture2D();
        }

        if (mask & GL_COLOR_BUFFER_BIT)
        {
            s.blendSFactor   = rs.blendSFactor;
            s.blendDFactor   = rs.blendDFactor;
            s.blendEquation  = rs.blendEquation;
            s.alphaFunc      = rs.alphaFunc;
            s.alphaFuncRef   = rs.alphaFuncRef;
            s.colorMaskR     = rs.colorMaskR;
            s.colorMaskG     = rs.colorMaskG;
            s.colorMaskB     = rs.colorMaskB;
            s.colorMaskA     = rs.colorMaskA;
            s.clearColor[0]  = rs.clearR; s.clearColor[1] = rs.clearG;
            s.clearColor[2]  = rs.clearB; s.clearColor[3] = rs.clearA;
        }

        if (mask & GL_DEPTH_BUFFER_BIT)
        {
            s.depthFunc  = rs.depthFunc;
            s.depthMask  = rs.depthMask;
        }

        if (mask & GL_STENCIL_BUFFER_BIT)
        {
            s.stencilFunc       = rs.stencilFunc;
            s.stencilRef        = rs.stencilRef;
            s.stencilValueMask  = rs.stencilValueMask;
            s.stencilWriteMask  = rs.stencilWriteMask;
            s.stencilFail       = rs.stencilFail;
            s.stencilZFail      = rs.stencilZFail;
            s.stencilZPass      = rs.stencilZPass;
        }

        if (mask & GL_FOG_BIT)
        {
            s.fogMode    = rs.fogMode;
            s.fogDensity = rs.fogDensity;
            s.fogStart   = rs.fogStart;
            s.fogEnd     = rs.fogEnd;
            s.fogColor[0]=rs.fogColor[0]; s.fogColor[1]=rs.fogColor[1];
            s.fogColor[2]=rs.fogColor[2]; s.fogColor[3]=rs.fogColor[3];
        }

        if (mask & GL_LIGHTING_BIT)
        {
            memcpy(s.lightModelAmbient, lt.lightModelAmbient, 16);
            memcpy(s.materialAmbient,   lt.materialAmbient,   16);
            memcpy(s.materialDiffuse,   lt.materialDiffuse,   16);
            memcpy(s.materialSpecular,  lt.materialSpecular,  16);
            memcpy(s.materialEmission,  lt.materialEmission,  16);
            s.materialShininess = lt.materialShininess;
            for (int _i=0; _i<MAXLIGHTS; _i++)
            {
                memcpy(s.lightPosition[_i], lt.lights[_i].position, 16);
                memcpy(s.lightDiffuse[_i],  lt.lights[_i].diffuse,  16);
                memcpy(s.lightAmbient[_i],  lt.lights[_i].ambient,  16);
                memcpy(s.lightSpecular[_i], lt.lights[_i].specular, 16);
            }
        }

        if (mask & GL_CURRENT_BIT)
        {
            memcpy(s.currentColor,  lt.currentColor,  16);
            memcpy(s.currentNormal, lt.currentNormal, 12);
        }

        if (mask & (GL_POLYGON_BIT | GL_LINE_BIT))
        {
            s.polygonModeFront    = rs.polygonModeFront;
            s.polygonModeBack     = rs.polygonModeBack;
            s.cullFaceMode        = rs.cullFaceMode;
            s.frontFaceMode       = rs.frontFaceMode;
            s.shadeModel          = rs.shadeModel;
            s.lineWidth           = rs.lineWidth;
            s.polygonOffsetFactor = rs.polygonOffsetFactor;
            s.polygonOffsetUnits  = rs.polygonOffsetUnits;
        }

        if (mask & GL_SCISSOR_BIT)
        {
            s.scissorX = rs.scissorX; s.scissorY = rs.scissorY;
            s.scissorW = rs.scissorW; s.scissorH = rs.scissorH;
        }
    }

    void glPopAttrib()
    {
        if (m_attribStackDepth <= 0)
        { SetError(GL_STACK_UNDERFLOW); return; }

        const RXGLAttribSnapshot& s = m_attribStack[--m_attribStackDepth];
        GLbitfield mask = s.mask;

        MarkDirty();

        if (mask & (GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                    GL_STENCIL_BUFFER_BIT | GL_FOG_BIT | GL_LIGHTING_BIT |
                    GL_SCISSOR_BIT | GL_POLYGON_BIT | GL_LINE_BIT))
        {
            m_rs.EnableDisable(GL_BLEND,          s.blend);
            m_rs.EnableDisable(GL_ALPHA_TEST,      s.alphaTest);
            m_rs.EnableDisable(GL_DEPTH_TEST,      s.depthTest);
            m_rs.EnableDisable(GL_STENCIL_TEST,    s.stencilTest);
            m_rs.EnableDisable(GL_FOG,             s.fog);
            m_rs.EnableDisable(GL_CULL_FACE,       s.cullFace);
            m_rs.EnableDisable(GL_SCISSOR_TEST,    s.scissorTest);
            m_rs.EnableDisable(GL_POLYGON_OFFSET_FILL, s.polygonOffsetFill);
            m_lighting.EnableDisable(GL_LIGHTING,      s.lighting);
            for (int _i=0; _i<MAXLIGHTS; _i++)
                m_lighting.EnableDisable((GLenum)(GL_LIGHT0+_i), s.lightsEnabled[_i]);
            m_lighting.EnableDisable(GL_NORMALIZE,     s.normalize);
            m_lighting.EnableDisable(GL_COLOR_MATERIAL,s.colorMaterial);
            if (m_tex.GetState().GetTexture2D() != s.texture2D)
                m_tex.GetState().SetTexture2D(s.texture2D);
            m_tex.GetState().SetMainBlend(m_rs.blend);
        }

        if (mask & GL_COLOR_BUFFER_BIT)
        {
            m_rs.SetBlendFunc(s.blendSFactor, s.blendDFactor);
            m_rs.SetBlendEquation(s.blendEquation);
            m_rs.SetAlphaFunc(s.alphaFunc, s.alphaFuncRef);
            m_rs.SetColorMask(s.colorMaskR, s.colorMaskG, s.colorMaskB, s.colorMaskA);
            m_rs.SetClearColor(s.clearColor[0], s.clearColor[1],
                               s.clearColor[2], s.clearColor[3]);
        }

        if (mask & GL_DEPTH_BUFFER_BIT)
        {
            m_rs.SetDepthFunc(s.depthFunc);
            m_rs.SetDepthMask(s.depthMask ? GL_TRUE : GL_FALSE);
        }

        if (mask & GL_STENCIL_BUFFER_BIT)
        {
            m_rs.SetStencilFunc(s.stencilFunc, s.stencilRef, s.stencilValueMask);
            m_rs.SetStencilOp(s.stencilFail, s.stencilZFail, s.stencilZPass);
            m_rs.SetStencilMask(s.stencilWriteMask);
        }

        if (mask & GL_FOG_BIT)
        {
            m_rs.SetFogi(GL_FOG_MODE,    s.fogMode);
            m_rs.SetFogf(GL_FOG_DENSITY, s.fogDensity);
            m_rs.SetFogf(GL_FOG_START,   s.fogStart);
            m_rs.SetFogf(GL_FOG_END,     s.fogEnd);
            m_rs.SetFogfv(GL_FOG_COLOR,  s.fogColor);
        }

        if (mask & GL_LIGHTING_BIT)
        {
            m_lighting.SetLightModelfv(GL_LIGHT_MODEL_AMBIENT, s.lightModelAmbient);
            m_lighting.SetMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   s.materialAmbient);
            m_lighting.SetMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   s.materialDiffuse);
            m_lighting.SetMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  s.materialSpecular);
            m_lighting.SetMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  s.materialEmission);
            m_lighting.SetMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, s.materialShininess);
            for (int _i=0; _i<MAXLIGHTS; _i++)
            {
                GLenum _l = (GLenum)(GL_LIGHT0+_i);
                m_lighting.SetLightfv(_l, GL_POSITION, s.lightPosition[_i]);
                m_lighting.SetLightfv(_l, GL_DIFFUSE,  s.lightDiffuse[_i]);
                m_lighting.SetLightfv(_l, GL_AMBIENT,  s.lightAmbient[_i]);
                m_lighting.SetLightfv(_l, GL_SPECULAR, s.lightSpecular[_i]);
            }
        }

        if (mask & GL_CURRENT_BIT)
        {
            memcpy(m_lighting.currentColor,  s.currentColor,  16);
            memcpy(m_lighting.currentNormal, s.currentNormal, 12);
            m_vb.SetColor(D3DRGBA(s.currentColor[0], s.currentColor[1],
                                  s.currentColor[2], s.currentColor[3]));
        }

        if (mask & (GL_POLYGON_BIT | GL_LINE_BIT))
        {
            m_rs.SetPolygonMode(GL_FRONT, s.polygonModeFront);
            m_rs.SetPolygonMode(GL_BACK,  s.polygonModeBack);
            m_rs.SetCullFace(s.cullFaceMode);
            m_rs.SetFrontFace(s.frontFaceMode);
            m_rs.SetShadeModel(s.shadeModel);
            m_rs.SetLineWidth(s.lineWidth);
            m_rs.SetPolygonOffset(s.polygonOffsetFactor, s.polygonOffsetUnits);
        }

        if (mask & GL_SCISSOR_BIT)
        {
            m_rs.SetScissor(s.scissorX, s.scissorY, s.scissorW, s.scissorH);
        }
    }

    // -----------------------------------------------------------------------
    // glPushClientAttrib / glPopClientAttrib
    //
    // Saves and restores client-side state: vertex array enable flags,
    // pointers, strides, types, and the active client texture stage.
    // GL_CLIENT_VERTEX_ARRAY_BIT covers all of this.
    // GL_CLIENT_PIXEL_STORE_BIT (pixel pack/unpack params) is accepted
    // but not currently saved -- pixel store state is rarely pushed.
    // -----------------------------------------------------------------------

    void glPushClientAttrib(GLbitfield mask)
    {
        if (m_clientAttribStackDepth >= RXGL_CLIENT_ATTRIB_STACK_DEPTH)
        {
            DbgPrint("glPushClientAttrib: stack overflow (depth %d)\n",
                     m_clientAttribStackDepth);
            SetError(GL_STACK_OVERFLOW);
            return;
        }
        RXGLClientAttribSnapshot& s =
            m_clientAttribStack[m_clientAttribStackDepth++];
        s.mask = mask;

        if (mask & GL_CLIENT_VERTEX_ARRAY_BIT)
        {
            // Full copy of array state -- pointers, strides, types, enables
            s.arrays = m_arrays;
        }
    }

    void glPopClientAttrib()
    {
        if (m_clientAttribStackDepth <= 0)
        {
            DbgPrint("glPopClientAttrib: stack underflow\n");
            SetError(GL_STACK_UNDERFLOW);
            return;
        }
        const RXGLClientAttribSnapshot& s =
            m_clientAttribStack[--m_clientAttribStackDepth];

        if (s.mask & GL_CLIENT_VERTEX_ARRAY_BIT)
        {
            m_arrays = s.arrays;
        }
    }
    void glGetFloatv(GLenum pname, GLfloat* params);
    void glGetIntegerv(GLenum pname, GLint* params);
    void glGetBooleanv(GLenum pname, GLboolean* params);
    // Record an error -- GL spec says first error wins until cleared
    void SetError(GLenum err)
    {
        if (m_glError == GL_NO_ERROR)
            m_glError = err;
    }

    GLenum glGetError()
    {
        GLenum e = m_glError;
        m_glError = GL_NO_ERROR;  // reading clears the error
        return e;
    }
    const GLubyte* glGetString(GLenum name);

    // Pixel readback
    void glReadPixels(GLint x, GLint y, GLsizei w, GLsizei h,
                      GLenum format, GLenum type, GLvoid* pixels);
    void glArrayElement(GLint idx)
    {
        // Read one vertex from all enabled client arrays at index idx
        // and emit it as immediate-mode calls (for use inside glBegin/glEnd).
        const RXGLArrayState& a = m_arrays;

        // Helper: read a float from a typed pointer
        #define AE_GETF(ptr,type,stride,i,comp)             ( (const float*)((const unsigned char*)(ptr) + (i)*((stride)?(stride):((comp)*sizeof(float))) ) )

        // Normal
        if (a.normal.enabled && a.normal.pointer)
        {
            int s = a.normal.stride ? a.normal.stride : 3*(int)sizeof(float);
            const float* p = (const float*)((const unsigned char*)a.normal.pointer + idx*s);
            glNormal3f(p[0], p[1], p[2]);
        }
        // Color
        if (a.color.enabled && a.color.pointer)
        {
            int comps = a.color.size;
            int s = a.color.stride ? a.color.stride : comps*(int)sizeof(float);
            if (a.color.type == GL_UNSIGNED_BYTE)
            {
                const unsigned char* p = (const unsigned char*)a.color.pointer + idx*s;
                if (comps==4) glColor4ub(p[0],p[1],p[2],p[3]);
                else          glColor3ub(p[0],p[1],p[2]);
            }
            else
            {
                const float* p = (const float*)((const unsigned char*)a.color.pointer + idx*s);
                if (comps==4) glColor4f(p[0],p[1],p[2],p[3]);
                else          glColor3f(p[0],p[1],p[2]);
            }
        }
        // TexCoord (stage 0)
        if (a.texCoord[0].enabled && a.texCoord[0].pointer)
        {
            int comps = a.texCoord[0].size;
            int s = a.texCoord[0].stride ? a.texCoord[0].stride : comps*(int)sizeof(float);
            const float* p = (const float*)((const unsigned char*)a.texCoord[0].pointer + idx*s);
            glTexCoord2f(p[0], comps>1?p[1]:0.0f);
        }
        // Vertex -- must be last (triggers vertex emission)
        if (a.vertex.enabled && a.vertex.pointer)
        {
            int comps = a.vertex.size;
            int s = a.vertex.stride ? a.vertex.stride : comps*(int)sizeof(float);
            const float* p = (const float*)((const unsigned char*)a.vertex.pointer + idx*s);
            if      (comps==2) glVertex2f(p[0],p[1]);
            else if (comps==4) glVertex4f(p[0],p[1],p[2],p[3]);
            else               glVertex3f(p[0],p[1],p[2]);
        }
        #undef AE_GETF
    }
    void glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
    void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels) { DbgPrint("glGetTexImage: stub\n"); (void)target;(void)level;(void)format;(void)type;(void)pixels; }

    // Misc no-ops
    void glDrawBuffer(GLenum)  {}
    void glReadBuffer(GLenum)  {}
    void glHint(GLenum, GLenum) {}
    void glFinish()            { FlushVB(); }
    void glFlush()             { FlushVB(); }

    // Xbox helpers
    void SwapBuffers();
    void SetGammaRamp(const unsigned char* table);
    void Hint_GenerateMipMaps(int v)          { m_tex.SetHintGenerateMipMaps(v != 0); }
    void EvictTextures()                      {}

private:
    void EmitVertex(GLfloat x, GLfloat y, GLfloat z);
    void RecordDLCmd(int op, GLenum e0=0, GLenum e1=0, GLint i0=0, GLuint u0=0,
                     GLfloat f0=0, GLfloat f1=0, GLfloat f2=0, GLfloat f3=0);
};

// ---------------------------------------------------------------------------
// RXGL implementation
// ---------------------------------------------------------------------------

// Static callback so texture system can report errors back to RXGL
static void TextureErrorCallback(GLenum err, void* ctx)
{
    static_cast<RXGL*>(ctx)->SetError(err);
}

RXGL::RXGL()
    : m_dev(NULL), m_d3d(NULL), m_hwTnL(false),
      m_needBeginScene(true), m_renderStateDirty(true),
      m_listBase(0), m_glError(GL_NO_ERROR), m_inPrimitive(false), m_logicOp(GL_COPY), m_logicOpEnabled(false),
      m_attribStackDepth(0),
      m_clientAttribStackDepth(0),
      m_rasterX(0), m_rasterY(0), m_rasterValid(false),
      m_pixelZoomX(1.0f), m_pixelZoomY(1.0f),
      m_vendor(NULL), m_renderer(NULL), m_extensions(NULL)
{
    m_rs.Init((GLsizei)gWidth, (GLsizei)gHeight);
    m_dl.Init();

    HRESULT hr = InitD3D();
    if (FAILED(hr)) { InterpretError(hr); return; }

    m_mx.Init();

    D3DCAPS8 caps;
    if (SUCCEEDED(m_dev->GetDeviceCaps(&caps)))
    {
        int maxStages = (int)caps.MaxTextureBlendStages;
        if (maxStages > MAXSTAGES) maxStages = MAXSTAGES;
        m_tex.GetState().SetMaxStages(maxStages);
        m_hwTnL = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
    }

    m_tex.Init(m_dev, m_d3d, m_backBufferDesc, g_force16bitTextures);
    m_tex.SetErrorCallback(TextureErrorCallback, this);
    m_lighting.Init();

    // One-time D3D state
    m_dev->SetRenderState(D3DRS_TEXTUREFACTOR,  0x00000000);
    m_dev->SetRenderState(D3DRS_DITHERENABLE,   FALSE);
    m_dev->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    m_dev->SetRenderState(D3DRS_LIGHTING,       FALSE);
    m_dev->SetRenderState(D3DRS_FRONTFACE,      D3DFRONT_CCW);
    m_dev->SetRenderState(D3DRS_CULLMODE,       D3DCULL_NONE);
}

RXGL::~RXGL()
{
    m_mx.Destroy();
    RELEASENULL(m_dev);
    RELEASENULL(m_d3d);
}

void RXGL::InterpretError(HRESULT hr)
{
    char buf[128];
    D3DXGetErrorString(hr, buf, sizeof(buf));
    OutputDebugString(buf);
    LocalDebugBreak();
}

void RXGL::MarkDirty()
{
    if (!m_renderStateDirty)
    {
        FlushVB();
        m_renderStateDirty = true;
    }
}

void RXGL::FlushVB()
{
    // Check if any clip plane is enabled -- if so, use software clipping
    bool anyClip = false;
    for (int i = 0; i < 6; i++) if (g_clipPlaneEnabled[i]) { anyClip = true; break; }

    if (anyClip)
    {
        // Get current modelview matrix in column-major GL float form
        GLfloat mv[16] = {0};
        const D3DXMATRIX* d3dmv = m_mx.modelView->GetTop();
        if (d3dmv)
        {
            // D3D matrix is row-major, GL is column-major
            for (int r=0;r<4;r++) for(int c=0;c<4;c++)
                mv[r+c*4] = d3dmv->m[r][c];
        }
        else
        {
            // Identity
            mv[0]=mv[5]=mv[10]=mv[15]=1.0f;
        }

        EnsureBeginScene();
        ApplyAllState();

        if (m_vb.ClipAndFlush(g_clipPlaneEq, g_clipPlaneEnabled, mv))
            return; // clip path handled the flush
    }

    m_vb.End();
}

void RXGL::EnsureBeginScene()
{
    if (m_needBeginScene)
    {
        m_needBeginScene = false;
        HRESULT hr = m_dev->BeginScene();
        if (FAILED(hr)) InterpretError(hr);
    }
}

void RXGL::ApplyAllState()
{
    if (!m_renderStateDirty) return;
    m_renderStateDirty = false;

    m_rs.Apply(m_dev, m_rs.viewportW, m_rs.viewportH);
    m_tex.Apply(m_dev);
    m_mx.Apply(m_dev, m_rs.viewportW, m_rs.viewportH);

    // Texgen is computed in software in EmitVertex (see comment there for why).
    // Always set TCI to passthrough -- EmitVertex has already written correct UVs.
    for (int i = 0; i < m_tex.GetState().GetMaxStages(); i++)
    {
        m_dev->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD)i);
        m_dev->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }

}
// Note: D3DRS_LOGICOP is written directly by glLogicOp/glEnable,
// not via ApplyAllState, to avoid blend state ordering issues on NV2A.

HRESULT RXGL::InitD3D()
{
    m_d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (!m_d3d) return E_FAIL;

    D3DPRESENT_PARAMETERS params;
    ZeroMemory(&params, sizeof(params));
    params.BackBufferWidth            = gWidth;
    params.BackBufferHeight           = gHeight;
    params.BackBufferFormat           = D3DFMT_LIN_X8R8G8B8;
    params.BackBufferCount            = 1;
    params.EnableAutoDepthStencil     = TRUE;
    params.AutoDepthStencilFormat     = D3DFMT_D24S8;
    params.SwapEffect                 = D3DSWAPEFFECT_FLIP;
    params.FullScreen_RefreshRateInHz = 60;
    params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    Direct3D_SetPushBufferSize(768 * 1024, 128 * 1024);

    DWORD videoFlags = XGetVideoFlags();
    if (XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
        params.FullScreen_RefreshRateInHz = (videoFlags & XC_VIDEO_FLAGS_PAL_60Hz) ? 60 : 50;

    if (XGetAVPack() == XC_AV_PACK_HDTV && gVideoMode > 0)
    {
        if (videoFlags & XC_VIDEO_FLAGS_HDTV_720p && gWidth == 1280 && gHeight == 720)
            params.Flags = D3DPRESENTFLAG_PROGRESSIVE | D3DPRESENTFLAG_WIDESCREEN;
        else if (videoFlags & XC_VIDEO_FLAGS_HDTV_480p && gWidth == 640 && gHeight == 480)
            params.Flags = D3DPRESENTFLAG_PROGRESSIVE;
        else if (videoFlags & XC_VIDEO_FLAGS_HDTV_480p)
        {
            gWidth = 640; gHeight = 480;
            params.BackBufferWidth = 640; params.BackBufferHeight = 480;
            params.Flags = D3DPRESENTFLAG_PROGRESSIVE;
        }
        else gVideoMode = 0;
    }
    else
    {
        params.BackBufferWidth = gWidth = 640;
        params.BackBufferHeight = gHeight = 480;
        params.Flags = D3DPRESENTFLAG_INTERLACED;
    }

    HRESULT hr = m_d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &m_dev);
    if (FAILED(hr)) return hr;

    LPDIRECT3DSURFACE8 pBB = NULL;
    m_dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBB);
    if (pBB) { pBB->GetDesc(&m_backBufferDesc); pBB->Release(); }

    m_dev->SetFlickerFilter(1);
    m_dev->SetSoftDisplayFilter(false);
    return S_OK;
}

void RXGL::EnsureDriverInfo()
{
    if (m_vendor) return;
    m_d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &m_dddi);
    m_vendor   = m_dddi.Driver;
    m_renderer = m_dddi.Description;
    wsprintf(m_version, "%u.%u.%u.%u",
        HIWORD(m_dddi.DriverVersion.HighPart), LOWORD(m_dddi.DriverVersion.HighPart),
        HIWORD(m_dddi.DriverVersion.LowPart),  LOWORD(m_dddi.DriverVersion.LowPart));
    m_extensions = (m_tex.GetState().GetMaxStages() > 1)
        ? " GL_SGIS_multitexture GL_ARB_multitexture GL_EXT_texture_object "
        : " GL_EXT_texture_object ";
}

// ---------------------------------------------------------------------------
// Enable / Disable
// ---------------------------------------------------------------------------

void RXGL::glEnableDisable(GLenum cap, bool value)
{
    // Try render state first
    if (m_rs.EnableDisable(cap, value))
    {
        MarkDirty();
        // GL_BLEND also needs to notify texture state for alpha blending
        if (cap == GL_BLEND)
            m_tex.GetState().SetMainBlend(m_rs.blend);
        return;
    }

    // Try lighting
    if (m_lighting.EnableDisable(cap, value))
        return;

    // Texture 2D
    if (cap == GL_TEXTURE_2D)
    {
        if (m_tex.GetState().GetTexture2D() != value)
        {
            MarkDirty();
            m_tex.GetState().SetTexture2D(value);
        }
        // Always force stage dirty so the no-texture alpha passthrough re-applies
        m_tex.GetState().ForceStage0Dirty();
        return;
    }

    // Logic ops enable/disable -- D3DRS_LOGICOP=150, NONE=0 disables
    if (cap == GL_COLOR_LOGIC_OP || cap == GL_INDEX_LOGIC_OP)
    {
        if (m_logicOpEnabled != value)
        {
            if (m_vb.GetVertexCount() > 0) FlushVB();
            m_logicOpEnabled = value;
            if (m_dev)
                m_dev->SetRenderState((D3DRENDERSTATETYPE)150,
                    value ? (DWORD)m_logicOp : (DWORD)D3DLOGICOP_NONE);
        }
        return;
    }
    // Known no-ops
    if (cap == GL_DITHER || cap == GL_LINE_SMOOTH ||
        cap == GL_POINT_SMOOTH || cap == GL_POLYGON_SMOOTH)
        return;

    // Texture coordinate generation enable/disable
    if (cap >= GL_TEXTURE_GEN_S && cap <= GL_TEXTURE_GEN_Q)
    {
        int c = (int)cap - (int)GL_TEXTURE_GEN_S;
        m_texGen.enabled[c] = value;
        MarkDirty();
        return;
    }

    // Clip planes -- software clipping path
    if (cap >= GL_CLIP_PLANE0 && cap <= GL_CLIP_PLANE5)
    {
        FlushVB(); // flush pending geometry before changing clip state
        int idx = (int)cap - (int)GL_CLIP_PLANE0;
        g_clipPlaneEnabled[idx] = value;
        return;
    }

    DbgPrint("glEnable/Disable: unhandled cap 0x%x\n", cap);
    SetError(GL_INVALID_ENUM);
}

GLboolean RXGL::glIsEnabled(GLenum cap)
{
    GLboolean r = m_rs.IsEnabled(cap);
    if (r != GL_FALSE) return r;
    r = m_lighting.IsEnabled(cap);
    if (r != GL_FALSE) return r;
    if (cap == GL_TEXTURE_2D) return m_tex.GetState().GetTexture2D() ? GL_TRUE : GL_FALSE;
    return GL_FALSE;
}

// ---------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------

void RXGL::glClear(GLbitfield mask)
{
    FlushVB();
    ApplyAllState();
    DWORD dm = 0;
    if (mask & GL_COLOR_BUFFER_BIT)   dm |= D3DCLEAR_TARGET;
    if (mask & GL_DEPTH_BUFFER_BIT)   dm |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    if (mask & GL_STENCIL_BUFFER_BIT) dm |= D3DCLEAR_STENCIL;
    m_dev->Clear(0, NULL, dm, m_rs.clearColor,
                 (float)m_rs.clearDepth, (DWORD)m_rs.clearStencil);
}

// ---------------------------------------------------------------------------
// Color (DL-aware)
// ---------------------------------------------------------------------------

void RXGL::glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_COLOR3F, 0,0,0,0, r,g,b,0);
        if (m_dl.IsCompileOnly()) return;
    }
    m_lighting.currentColor[0] = r;
    m_lighting.currentColor[1] = g;
    m_lighting.currentColor[2] = b;
    m_lighting.currentColor[3] = 1.0f;
    m_vb.SetColor(D3DRGB(r,g,b));
    if (m_lighting.colorMaterial)
        m_lighting.UpdateMaterialFromColor(r, g, b, 1.0f);
}

void RXGL::glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_COLOR4F, 0,0,0,0, r,g,b,a);
        if (m_dl.IsCompileOnly()) return;
    }
    m_lighting.currentColor[0] = r;
    m_lighting.currentColor[1] = g;
    m_lighting.currentColor[2] = b;
    m_lighting.currentColor[3] = a;
    m_vb.SetColor(D3DRGBA(r,g,b,a));
    if (m_lighting.colorMaterial)
        m_lighting.UpdateMaterialFromColor(r, g, b, a);
}

// ---------------------------------------------------------------------------
// Immediate mode vertex (DL-aware)
// ---------------------------------------------------------------------------

void RXGL::glBegin(GLenum mode)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_BEGIN, mode);
        if (m_dl.IsCompileOnly()) return;
    }

    // GL_INVALID_OPERATION if called inside another glBegin
    if (m_inPrimitive) { SetError(GL_INVALID_OPERATION); return; }

    // GL_INVALID_ENUM if mode is not a valid primitive
    switch (mode)
    {
    case GL_POINTS: case GL_LINES: case GL_LINE_LOOP: case GL_LINE_STRIP:
    case GL_TRIANGLES: case GL_TRIANGLE_STRIP: case GL_TRIANGLE_FAN:
    case GL_QUADS: case GL_QUAD_STRIP: case GL_POLYGON:
        break;
    default:
        SetError(GL_INVALID_ENUM); return;
    }

    m_inPrimitive = true;

    if (m_renderStateDirty || !m_vb.IsMergableMode(mode))
    {
        FlushVB();
        ApplyAllState();

        DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
        // Include normals when lighting needs software Phong,
        // OR when texgen needs them for hardware TCI sphere/normal map
        bool needNormals = m_lighting.lighting ||
                           m_texGen.enabled[0] || m_texGen.enabled[1] ||
                           m_texGen.enabled[2] || m_texGen.enabled[3];
        if (needNormals)
            fvf |= D3DFVF_NORMAL;
        fvf |= (m_tex.GetState().GetMaxStages() << D3DFVF_TEXCOUNT_SHIFT);
        if (fvf != m_vb.GetVertexTypeDesc())
            m_vb.Initialize(m_dev, m_d3d, m_hwTnL, fvf);
        m_vb.Begin(mode);
    }
    else
    {
        m_vb.Append(mode);
    }
}

void RXGL::glEnd()
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_END);
        if (m_dl.IsCompileOnly()) return;
    }

    // GL_INVALID_OPERATION if called outside glBegin
    if (!m_inPrimitive) { SetError(GL_INVALID_OPERATION); return; }
    m_inPrimitive = false;
    // Flush deferred to next glBegin or SwapBuffers
}

void RXGL::glTexCoord2f(GLfloat s, GLfloat t)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_TEXCOORD2F, 0,0,0,0, s,t,0,0);
        if (m_dl.IsCompileOnly()) return;
    }
    // Apply texture matrix in software
    D3DXMATRIX* mx = m_mx.GetTextureStack()->GetTop();
    GLfloat os = s * mx->_11 + t * mx->_21 + mx->_41;
    GLfloat ot = s * mx->_12 + t * mx->_22 + mx->_42;
    // Write to whichever stage is current (set by glClientActiveTextureARB)
    int stage = m_tex.GetState().GetClientStage();
    if (stage == 0)
        m_vb.SetTextureCoord0(os, ot);
    else
        m_vb.SetTextureCoord(stage, os, ot);
}

void RXGL::EmitVertex(GLfloat x, GLfloat y, GLfloat z)
{
    m_vb.SetNormal(m_lighting.currentNormal[0],
                   m_lighting.currentNormal[1],
                   m_lighting.currentNormal[2]);
    m_lighting.ApplyToVertex(m_vb, m_mx.modelView->GetTop(), x, y, z);

    // ---------------------------------------------------------------------------
    // Software texgen.
    //
    // WHY SOFTWARE: Hardware TCI modes (D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR etc)
    // require a split WORLD/VIEW transform so the NV2A knows what "camera space"
    // means. We combine everything into D3DTS_WORLD with D3DTS_VIEW=identity, which
    // confuses the hardware normal transform used by TCI (inverse-transpose of WORLD
    // includes our translation, corrupting the reflection vector at sphere edges).
    //
    // Splitting WORLD/VIEW would be a major refactor of the matrix system. Software
    // texgen is the correct alternative -- it is what many GL implementations do
    // internally anyway, and gives identical results to GL on PC.
    //
    // PERFORMANCE: ~2-3 sqrtf calls per vertex per texgen sphere. Acceptable for
    // the use cases this shim targets. OBJECT_LINEAR is free (no sqrt needed).
    // ---------------------------------------------------------------------------
    bool anyTexGen = m_texGen.enabled[0] || m_texGen.enabled[1] ||
                     m_texGen.enabled[2] || m_texGen.enabled[3];
    if (anyTexGen)
    {
        const D3DXMATRIX* mv = m_mx.modelView->GetTop();
        if (mv)
        {
            // Eye-space position
            GLfloat ex = x*mv->_11 + y*mv->_21 + z*mv->_31 + mv->_41;
            GLfloat ey = x*mv->_12 + y*mv->_22 + z*mv->_32 + mv->_42;
            GLfloat ez = x*mv->_13 + y*mv->_23 + z*mv->_33 + mv->_43;

            // Eye-space normal (no translation row)
            GLfloat nx = m_lighting.currentNormal[0];
            GLfloat ny = m_lighting.currentNormal[1];
            GLfloat nz = m_lighting.currentNormal[2];
            GLfloat enx = nx*mv->_11 + ny*mv->_21 + nz*mv->_31;
            GLfloat eny = nx*mv->_12 + ny*mv->_22 + nz*mv->_32;
            GLfloat enz = nx*mv->_13 + ny*mv->_23 + nz*mv->_33;
            GLfloat nlen = sqrtf(enx*enx + eny*eny + enz*enz);
            if (nlen > 0.00001f) { enx/=nlen; eny/=nlen; enz/=nlen; }

            // Precompute sphere map values (shared for S and T)
            GLfloat smS = 0.5f, smT = 0.5f;
            bool needReflect = (m_texGen.enabled[0] &&
                               (m_texGen.mode[0]==GL_SPHERE_MAP||m_texGen.mode[0]==GL_REFLECTION_MAP)) ||
                               (m_texGen.enabled[1] &&
                               (m_texGen.mode[1]==GL_SPHERE_MAP||m_texGen.mode[1]==GL_REFLECTION_MAP));
            if (needReflect)
            {
                GLfloat elen = sqrtf(ex*ex + ey*ey + ez*ez);
                GLfloat evx=0,evy=0,evz=-1;
                if (elen > 0.00001f) { evx=-ex/elen; evy=-ey/elen; evz=-ez/elen; }
                GLfloat dot2 = 2.0f*(evx*enx + evy*eny + evz*enz);
                GLfloat rrx = dot2*enx - evx;
                GLfloat rry = dot2*eny - evy;
                GLfloat rrz = dot2*enz - evz;
                GLfloat p = sqrtf(rrx*rrx + rry*rry + (rrz+1.0f)*(rrz+1.0f));
                if (p < 0.00001f) p = 0.00001f;
                smS = rrx/(2.0f*p) + 0.5f;
                smT = rry/(2.0f*p) + 0.5f;
            }

            GLfloat genS = 0, genT = 0;
            bool hasS = false, hasT = false;

            if (m_texGen.enabled[0])
            {
                switch (m_texGen.mode[0])
                {
                case GL_SPHERE_MAP: case GL_REFLECTION_MAP:
                    genS=smS; hasS=true; break;
                case GL_EYE_LINEAR: {
                    const GLfloat* p=m_texGen.eyePlane[0];
                    genS=p[0]*ex+p[1]*ey+p[2]*ez+p[3]; hasS=true; break; }
                case GL_OBJECT_LINEAR: {
                    const GLfloat* p=m_texGen.objectPlane[0];
                    genS=p[0]*x+p[1]*y+p[2]*z+p[3]; hasS=true; break; }
                case GL_NORMAL_MAP:
                    genS=enx*0.5f+0.5f; hasS=true; break;
                default: break;
                }
            }
            if (m_texGen.enabled[1])
            {
                switch (m_texGen.mode[1])
                {
                case GL_SPHERE_MAP: case GL_REFLECTION_MAP:
                    genT=smT; hasT=true; break;
                case GL_EYE_LINEAR: {
                    const GLfloat* p=m_texGen.eyePlane[1];
                    genT=p[0]*ex+p[1]*ey+p[2]*ez+p[3]; hasT=true; break; }
                case GL_OBJECT_LINEAR: {
                    const GLfloat* p=m_texGen.objectPlane[1];
                    genT=p[0]*x+p[1]*y+p[2]*z+p[3]; hasT=true; break; }
                case GL_NORMAL_MAP:
                    genT=eny*0.5f+0.5f; hasT=true; break;
                default: break;
                }
            }

            if (hasS || hasT)
                m_vb.SetTextureCoord0(genS, genT);
        }
    }

    m_vb.SetVertex(x, y, z);
}

void RXGL::glVertex2f(GLfloat x, GLfloat y)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_VERTEX2F, 0,0,0,0, x,y,0,0);
        if (m_dl.IsCompileOnly()) return;
    }
    EmitVertex(x, y, 0.f);
}

void RXGL::glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    if (m_dl.IsCompiling() && !m_dl.IsExecuting())
    {
        RecordDLCmd(DL_OP_VERTEX3F, 0,0,0,0, x,y,z,0);
        if (m_dl.IsCompileOnly()) return;
    }
    EmitVertex(x, y, z);
}

// ---------------------------------------------------------------------------
// Vertex arrays
// ---------------------------------------------------------------------------

void RXGL::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    // When lighting is active, DrawArrays must go through the glBegin/glEnd
    // path so software lighting (ApplyToVertex) runs per-vertex.
    // The fast DrawArrays path bypasses EmitVertex and never calls ApplyToVertex,
    // so lit geometry would render with the wrong (un-lit) vertex colour.
    if (m_lighting.lighting)
    {
        glBegin(mode);
        for (GLint i = first; i < first + count; i++)
            glArrayElement(i);
        glEnd();
        return;
    }
    EnsureBeginScene();
    ApplyAllState();
    m_arrays.DrawArrays(mode, first, count, m_vb, m_dev, m_d3d, m_hwTnL,
                        m_tex.GetState().GetMaxStages());
}

void RXGL::glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    // Same lighting fix as glDrawArrays -- route through glBegin/glEnd when lit.
    if (m_lighting.lighting)
    {
        const GLubyte*  idx8  = (const GLubyte*) indices;
        const GLushort* idx16 = (const GLushort*)indices;
        const GLuint*   idx32 = (const GLuint*)  indices;
        glBegin(mode);
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
            glArrayElement(idx);
        }
        glEnd();
        return;
    }
    EnsureBeginScene();
    ApplyAllState();
    m_arrays.DrawElements(mode, count, type, indices, m_vb, m_dev, m_d3d, m_hwTnL,
                          m_tex.GetState().GetMaxStages());
}

// ---------------------------------------------------------------------------
// Display list helper
// ---------------------------------------------------------------------------

void RXGL::RecordDLCmd(int op, GLenum e0, GLenum e1, GLint i0, GLuint u0,
                          GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3)
{
    RXGLDisplayListCmd c;
    memset(&c, 0, sizeof(c));
    c.op=op; c.e0=e0; c.e1=e1; c.i0=i0; c.u0=u0;
    c.f[0]=f0; c.f[1]=f1; c.f[2]=f2; c.f[3]=f3;
    m_dl.Record(c);
}

void RXGL::DLDispatch(const RXGLDisplayListCmd& c, void* ctx)
{
    RXGL* gl = (RXGL*)ctx;
    switch (c.op)
    {
    case DL_OP_BEGIN:        gl->glBegin(c.e0);                           break;
    case DL_OP_END:          gl->glEnd();                                 break;
    case DL_OP_COLOR3F:      gl->glColor3f(c.f[0],c.f[1],c.f[2]);        break;
    case DL_OP_COLOR4F:      gl->glColor4f(c.f[0],c.f[1],c.f[2],c.f[3]); break;
    case DL_OP_COLOR4FV:     gl->glColor4fv(c.f);                         break;
    case DL_OP_COLOR3UB:     gl->glColor3ub(c.ub[0],c.ub[1],c.ub[2]);    break;
    case DL_OP_COLOR4UB:     gl->glColor4ub(c.ub[0],c.ub[1],c.ub[2],c.ub[3]); break;
    case DL_OP_COLOR3UBV:    gl->glColor3ubv(c.ub);                       break;
    case DL_OP_COLOR4UBV:    gl->glColor4ubv(c.ub);                       break;
    case DL_OP_TEXCOORD2F:   gl->glTexCoord2f(c.f[0],c.f[1]);             break;
    case DL_OP_VERTEX2F:     gl->glVertex2f(c.f[0],c.f[1]);               break;
    case DL_OP_VERTEX3F:     gl->glVertex3f(c.f[0],c.f[1],c.f[2]);        break;
    case DL_OP_VERTEX3FV:    gl->glVertex3fv(c.f);                         break;
    case DL_OP_NORMAL3F:     gl->glNormal3f(c.f[0],c.f[1],c.f[2]);        break;
    case DL_OP_LOADIDENTITY: gl->glLoadIdentity();                         break;
    case DL_OP_TRANSLATEF:   gl->glTranslatef(c.f[0],c.f[1],c.f[2]);      break;
    case DL_OP_SCALEF:       gl->glScalef(c.f[0],c.f[1],c.f[2]);          break;
    case DL_OP_ROTATEF:      gl->glRotatef(c.f[0],c.f[1],c.f[2],c.f[3]); break;
    case DL_OP_LOADMATRIXF:  gl->glLoadMatrixf(c.f);                       break;
    case DL_OP_MULTMATRIXF:  gl->glMultMatrixf(c.f);                       break;
    case DL_OP_ORTHO:        gl->glOrtho  (c.f[0],c.f[1],c.f[2],c.f[3],c.f[4],c.f[5]); break;
    case DL_OP_FRUSTUM:      gl->glFrustum(c.f[0],c.f[1],c.f[2],c.f[3],c.f[4],c.f[5]); break;
    case DL_OP_PUSHMATRIX:   gl->glPushMatrix();                           break;
    case DL_OP_POPMATRIX:    gl->glPopMatrix();                            break;
    case DL_OP_MATRIXMODE:   gl->glMatrixMode(c.e0);                       break;
    case DL_OP_ENABLE:       gl->glEnable(c.e0);                           break;
    case DL_OP_DISABLE:      gl->glDisable(c.e0);                          break;
    case DL_OP_BINDTEXTURE:  gl->glBindTexture(c.e0, c.u0);                break;
    case DL_OP_TEXENVI:      gl->glTexEnvi(c.e0, c.e1, c.i0);              break;
    case DL_OP_TEXENVF:      gl->glTexEnvf(c.e0, c.e1, c.f[0]);            break;
    case DL_OP_BLENDFUNC:    gl->glBlendFunc(c.e0, c.e1);                  break;
    case DL_OP_BLENDEQUATION:gl->glBlendEquation(c.e0);                    break;
    case DL_OP_ALPHAFUNC:    gl->glAlphaFunc(c.e0, c.f[0]);                break;
    case DL_OP_FOGF:         gl->glFogf(c.e0, c.f[0]);                     break;
    case DL_OP_FOGI:         gl->glFogi(c.e0, c.i0);                       break;
    case DL_OP_FOGFV:        gl->glFogfv(c.e0, c.f);                       break;
    case DL_OP_MATERIALFV:   gl->glMaterialfv(c.e0, c.e1, c.f);            break;
    case DL_OP_LIGHTFV:      gl->glLightfv(c.e0, c.e1, c.f);               break;
    case DL_OP_DEPTHFUNC:    gl->glDepthFunc(c.e0);                         break;
    case DL_OP_DEPTHMASK:    gl->glDepthMask((GLboolean)c.i0);              break;
    case DL_OP_CULLFACE:     gl->glCullFace(c.e0);                          break;
    case DL_OP_FRONTFACE:    gl->glFrontFace(c.e0);                         break;
    case DL_OP_SHADEMODEL:   gl->glShadeModel(c.e0);                        break;
    case DL_OP_CALLLIST:     gl->m_dl.CallList(c.u0, DLDispatch, gl);       break;
    }
}

// ---------------------------------------------------------------------------
// State queries
// ---------------------------------------------------------------------------

void RXGL::glGetFloatv(GLenum pname, GLfloat* params)
{
    if (!params) return;
    switch (pname)
    {
    // --- Matrix state ---
    case GL_MODELVIEW_MATRIX:
        memcpy(params, m_mx.modelView->GetTop(), sizeof(D3DMATRIX)); break;
    case GL_PROJECTION_MATRIX:
        memcpy(params, m_mx.projection->GetTop(), sizeof(D3DMATRIX)); break;
    case GL_TEXTURE_MATRIX:
        memcpy(params, m_mx.texture->GetTop(), sizeof(D3DMATRIX)); break;

    // --- Viewport / depth ---
    case GL_DEPTH_RANGE:
        params[0]=(GLfloat)m_rs.depthRangeNear;
        params[1]=(GLfloat)m_rs.depthRangeFar; break;
    case GL_VIEWPORT:
        params[0]=(GLfloat)m_rs.viewportX; params[1]=(GLfloat)m_rs.viewportY;
        params[2]=(GLfloat)m_rs.viewportW; params[3]=(GLfloat)m_rs.viewportH; break;

    // --- Clear / colour ---
    case GL_COLOR_CLEAR_VALUE:
        params[0]=(float)((m_rs.clearColor>>16)&0xff)/255.f;
        params[1]=(float)((m_rs.clearColor>>8) &0xff)/255.f;
        params[2]=(float)((m_rs.clearColor)    &0xff)/255.f;
        params[3]=(float)((m_rs.clearColor>>24)&0xff)/255.f; break;
    case GL_CURRENT_COLOR:
        params[0]=m_lighting.currentColor[0]; params[1]=m_lighting.currentColor[1];
        params[2]=m_lighting.currentColor[2]; params[3]=m_lighting.currentColor[3]; break;

    // --- Fog ---
    case GL_FOG_COLOR:
        params[0]=m_rs.fogColor[0]; params[1]=m_rs.fogColor[1];
        params[2]=m_rs.fogColor[2]; params[3]=m_rs.fogColor[3]; break;
    case GL_FOG_START:   params[0]=m_rs.fogStart;   break;
    case GL_FOG_END:     params[0]=m_rs.fogEnd;     break;
    case GL_FOG_DENSITY: params[0]=m_rs.fogDensity; break;
    case GL_FOG_INDEX:   params[0]=0.0f;             break;

    // --- Lighting ---
    case GL_LIGHT_MODEL_AMBIENT:
        params[0]=m_lighting.lightModelAmbient[0]; params[1]=m_lighting.lightModelAmbient[1];
        params[2]=m_lighting.lightModelAmbient[2]; params[3]=m_lighting.lightModelAmbient[3]; break;
    case GL_CURRENT_NORMAL:
        params[0]=m_lighting.currentNormal[0];
        params[1]=m_lighting.currentNormal[1];
        params[2]=m_lighting.currentNormal[2]; break;

    // --- Alpha / blend ---
    case GL_ALPHA_TEST_REF:    params[0]=m_rs.alphaFuncRef;        break;
    case GL_POINT_SIZE:        params[0]=m_rs.pointSize;           break;
    case GL_LINE_WIDTH:        params[0]=m_rs.lineWidth;           break;
    case GL_POLYGON_OFFSET_FACTOR: params[0]=m_rs.polygonOffsetFactor; break;
    case GL_POLYGON_OFFSET_UNITS:  params[0]=m_rs.polygonOffsetUnits;  break;

    default:
        // Fall back to glGetIntegerv for integer-valued params
        { GLint iv=0; glGetIntegerv(pname,&iv); params[0]=(GLfloat)iv; }
        break;
    }
}

void RXGL::glGetIntegerv(GLenum pname, GLint* params)
{
    switch (pname)
    {
    case GL_MAX_TEXTURE_SIZE:           *params = XBOX_MAX_TEXTURE_SIZE; break;
    case GL_MAX_TEXTURE_UNITS:          *params = m_tex.GetState().GetMaxStages(); break;
    case GL_MAX_MODELVIEW_STACK_DEPTH:  *params = 32; break;
    case GL_MAX_PROJECTION_STACK_DEPTH: *params = 4;  break;
    case GL_MAX_TEXTURE_STACK_DEPTH:    *params = 4;  break;
    case GL_MAX_LIGHTS:                 *params = 0;  break;
    case GL_DEPTH_BITS:                 *params = 24; break;
    case GL_STENCIL_BITS:               *params = 8;  break;
    case GL_RED_BITS: case GL_GREEN_BITS: case GL_BLUE_BITS: *params = 8; break;
    case GL_ALPHA_BITS:                 *params = 8;  break;
    case GL_SUBPIXEL_BITS:              *params = 4;  break;
    case GL_VIEWPORT:
        params[0]=m_rs.viewportX; params[1]=m_rs.viewportY;
        params[2]=m_rs.viewportW; params[3]=m_rs.viewportH; break;
    case GL_SCISSOR_BOX:
        params[0]=m_rs.scissorX; params[1]=m_rs.scissorY;
        params[2]=m_rs.scissorW; params[3]=m_rs.scissorH; break;
    case GL_UNPACK_ALIGNMENT:           *params = m_rs.unpackAlignment; break;
    case GL_PACK_ALIGNMENT:             *params = 4; break;
    case GL_FRONT_FACE:                 *params = (GLint)m_rs.frontFaceMode; break;
    case GL_MATRIX_MODE:                *params = (GLint)m_mx.matrixMode; break;
    case GL_SHADE_MODEL:                *params = (GLint)m_rs.shadeModel; break;
    case GL_BLEND_SRC:                  *params = (GLint)m_rs.blendSFactor; break;
    case GL_BLEND_DST:                  *params = (GLint)m_rs.blendDFactor; break;
    case GL_DEPTH_FUNC:                 *params = (GLint)m_rs.depthFunc; break;
    case GL_STENCIL_FUNC:               *params = (GLint)m_rs.stencilFunc; break;
    case GL_STENCIL_REF:                *params = m_rs.stencilRef; break;
    case GL_STENCIL_VALUE_MASK:         *params = (GLint)m_rs.stencilValueMask; break;
    case GL_STENCIL_WRITEMASK:          *params = (GLint)m_rs.stencilWriteMask; break;
    case GL_STENCIL_CLEAR_VALUE:        *params = m_rs.clearStencil; break;
    case GL_TEXTURE_BINDING_2D:         *params = (GLint)m_tex.GetState().GetCurrentTexture(); break;
    case GL_BLEND_EQUATION:             *params = (GLint)m_rs.blendEquation; break;
    case GL_MAX_CLIP_PLANES:            *params = 6; break;
    default:
        DbgPrint("glGetIntegerv: unhandled pname 0x%x\n", pname);
        SetError(GL_INVALID_ENUM);
        *params = 0; break;
    }
}

void RXGL::glGetBooleanv(GLenum pname, GLboolean* params)
{ GLint v=0; glGetIntegerv(pname,&v); *params = v ? GL_TRUE : GL_FALSE; }

const GLubyte* RXGL::glGetString(GLenum name)
{
    EnsureDriverInfo();
    switch (name)
    {
    case GL_VENDOR:     return (const GLubyte*)m_vendor;
    case GL_RENDERER:   return (const GLubyte*)m_renderer;
    case GL_VERSION:    return (const GLubyte*)m_version;
    case GL_EXTENSIONS: return (const GLubyte*)m_extensions;
    default:            return (const GLubyte*)"";
    }
}

// ---------------------------------------------------------------------------
// Pixel readback
// ---------------------------------------------------------------------------

void RXGL::glReadPixels(GLint x, GLint y, GLsizei w, GLsizei h,
                           GLenum format, GLenum type, GLvoid* pixels)
{
    if (!pixels || type != GL_UNSIGNED_BYTE) { LocalDebugBreak(); return; }
    if (format != GL_RGB && format != GL_RGBA) { LocalDebugBreak(); return; }
    if (w <= 0 || h <= 0) return;

    FlushVB();

    IDirect3DSurface8* pBB = NULL;
    m_dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBB);
    if (!pBB) return;

    D3DSURFACE_DESC desc; pBB->GetDesc(&desc);
    IDirect3DSurface8* pStaging = NULL;

    if (SUCCEEDED(m_dev->CreateImageSurface(desc.Width, desc.Height, desc.Format, &pStaging)))
    {
        if (SUCCEEDED(D3DXLoadSurfaceFromSurface(pStaging,NULL,NULL,pBB,NULL,NULL,D3DX_FILTER_NONE,0)))
        {
            D3DLOCKED_RECT lr;
            if (SUCCEEDED(pStaging->LockRect(&lr, NULL, D3DLOCK_READONLY)))
            {
                int outBpp = (format == GL_RGBA) ? 4 : 3;
                for (int row = 0; row < h; row++)
                {
                    int srcY = (int)desc.Height - 1 - (y + row);
                    if (srcY < 0 || srcY >= (int)desc.Height) continue;
                    unsigned char* dst = (unsigned char*)pixels + row * w * outBpp;
                    const unsigned char* src = (const unsigned char*)lr.pBits + srcY * lr.Pitch + x * 4;
                    for (int col = 0; col < w; col++)
                    {
                        dst[col*outBpp+0] = src[col*4+2]; // R
                        dst[col*outBpp+1] = src[col*4+1]; // G
                        dst[col*outBpp+2] = src[col*4+0]; // B
                        if (format == GL_RGBA) dst[col*outBpp+3] = 255;
                    }
                }
                pStaging->UnlockRect();
            }
        }
        pStaging->Release();
    }
    pBB->Release();
}


// ---------------------------------------------------------------------------
// glCopyPixels
// Copies a rectangle of pixels from the framebuffer to the current raster
// position, as if glReadPixels then glDrawPixels were called.
//
// type == GL_COLOR:   fully supported via back-buffer readback + texture draw
// type == GL_DEPTH:   not supported (D3D8 depth surface not CPU-readable)
// type == GL_STENCIL: not supported (D3D8 stencil surface not CPU-readable)
// ---------------------------------------------------------------------------
void RXGL::glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    if (width <= 0 || height <= 0) return;

    if (type == GL_DEPTH || type == GL_STENCIL)
    {
        DbgPrint("glCopyPixels: type %s not supported on this platform (D3D8 depth/stencil not CPU-readable)\n",
            type == GL_DEPTH ? "GL_DEPTH" : "GL_STENCIL");
        return;
    }

    if (type != GL_COLOR)
    {
        DbgPrint("glCopyPixels: unknown type 0x%x\n", (unsigned)type);
        return;
    }

    // GL_COLOR: read the source rectangle then draw at current raster position.
    // Allocate a temporary RGBA buffer.
    int bufSize = width * height * 4;
    unsigned char* buf = new unsigned char[(size_t)bufSize];
    if (!buf) return;

    // Read source region from colour buffer (RGBA, unsigned byte)
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);

    // Draw the pixels at the current raster position via glDrawPixels
    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);

    delete[] buf;
}
// ---------------------------------------------------------------------------
// Swap / gamma
// ---------------------------------------------------------------------------

void RXGL::SwapBuffers()
{
    FlushVB();
    m_dev->EndScene();
    m_needBeginScene = true;
    m_dev->Present(NULL, NULL, NULL, NULL);
}

void RXGL::SetGammaRamp(const unsigned char* table)
{
    D3DGAMMARAMP ramp;
    for (int i = 0; i < 256; i++)
        ramp.red[i] = ramp.green[i] = ramp.blue[i] = table[i];
    m_dev->SetGammaRamp(D3DSGR_NO_CALIBRATION, &ramp);
}

// ===========================================================================
// Global C interface
// ===========================================================================

static RXGL* gRXGL = NULL;

#ifdef __cplusplus
extern "C" {
#endif

HGLRC wglCreateContext()
{
    gRXGL = new RXGL();
    if (!gRXGL) return (HGLRC)0;
    return (HGLRC)1;
}

BOOL wglMakeCurrent()  { return gRXGL ? TRUE : FALSE; }

BOOL wglDeleteContext()
{
    delete gRXGL;
    gRXGL = NULL;
    return TRUE;
}

void FakeSwapBuffers() { if (gRXGL) gRXGL->SwapBuffers(); }

// Extension forwarders
static void __cdecl Ext_BindTextureEXT(GLenum t, GLuint tx)     { gRXGL->glBindTexture(t,tx); }
static void __cdecl Ext_MTexCoord2fSGIS(GLenum t, GLfloat s, GLfloat u) { gRXGL->glMTexCoord2fSGIS(t,s,u); }
static void __cdecl Ext_SelectTextureSGIS(GLenum t)              { gRXGL->glSelectTextureSGIS(t); }
static void __cdecl Ext_ActiveTextureARB(GLenum t)               { gRXGL->glActiveTextureARB(t); }
static void __cdecl Ext_ClientActiveTextureARB(GLenum t)         { gRXGL->glClientActiveTextureARB(t); }
static void __cdecl Ext_MultiTexCoord2fARB(GLenum t, GLfloat s, GLfloat u) { gRXGL->glMultiTexCoord2fARB(t,s,u); }

struct ExtEntry { const char* name; PROC fn; };
static const ExtEntry kExtTable[] =
{
    { "glBindTextureEXT",         (PROC)Ext_BindTextureEXT         },
    { "glMTexCoord2fSGIS",        (PROC)Ext_MTexCoord2fSGIS        },
    { "glSelectTextureSGIS",      (PROC)Ext_SelectTextureSGIS      },
    { "glActiveTextureARB",       (PROC)Ext_ActiveTextureARB       },
    { "glClientActiveTextureARB", (PROC)Ext_ClientActiveTextureARB },
    { "glMultiTexCoord2fARB",     (PROC)Ext_MultiTexCoord2fARB     },
    { NULL, NULL }
};

#pragma warning(push)
#pragma warning(disable: 4191)
PROC wglGetProcAddress(LPCSTR s)
{
    for (int i = 0; kExtTable[i].name; i++)
        if (strcmp(s, kExtTable[i].name) == 0) return kExtTable[i].fn;
    DbgPrint("wglGetProcAddress: unknown '%s'\n", s);
    return NULL;
}
#pragma warning(pop)

// GL C wrappers
void glAlphaFunc(GLenum f,GLclampf r)               { gRXGL->glAlphaFunc(f,r); }
void glBegin(GLenum m)                              { gRXGL->glBegin(m); }
void glBindTexture(GLenum t,GLuint tx)              { gRXGL->glBindTexture(t,tx); }
void glGenerateMipmap(GLenum t)                     { gRXGL->glGenerateMipmap(t); }
void glBlendFunc(GLenum sf,GLenum df)               { gRXGL->glBlendFunc(sf,df); }
void glBlendEquation(GLenum mode)                   { gRXGL->glBlendEquation(mode); }
void glBlendColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a){ gRXGL->glBlendColor(r,g,b,a); }
void glLogicOp(GLenum op)                             { gRXGL->glLogicOp(op); }
void glClear(GLbitfield m)                          { gRXGL->glClear(m); }
void glClearColor(GLclampf r,GLclampf g,GLclampf b,GLclampf a){ gRXGL->glClearColor(r,g,b,a); }
void glClearDepth(GLclampd d)                       { gRXGL->glClearDepth(d); }
void glClearStencil(GLint s)                        { gRXGL->glClearStencil(s); }
void glColor3f(GLfloat r,GLfloat g,GLfloat b)       { gRXGL->glColor3f(r,g,b); }
void glColor3fv(const GLfloat* v)                   { gRXGL->glColor3fv(v); }
void glColor3d(GLdouble r,GLdouble g,GLdouble b)    { gRXGL->glColor3d(r,g,b); }
void glColor3dv(const GLdouble* v)                  { gRXGL->glColor3dv(v); }
void glColor3b(GLbyte r,GLbyte g,GLbyte b)          { gRXGL->glColor3b(r,g,b); }
void glColor3bv(const GLbyte* v)                    { gRXGL->glColor3bv(v); }
void glColor3s(GLshort r,GLshort g,GLshort b)       { gRXGL->glColor3s(r,g,b); }
void glColor3sv(const GLshort* v)                   { gRXGL->glColor3sv(v); }
void glColor3us(GLushort r,GLushort g,GLushort b)   { gRXGL->glColor3us(r,g,b); }
void glColor3usv(const GLushort* v)                 { gRXGL->glColor3usv(v); }
void glColor3ui(GLuint r,GLuint g,GLuint b)         { gRXGL->glColor3ui(r,g,b); }
void glColor3uiv(const GLuint* v)                   { gRXGL->glColor3uiv(v); }
void glColor3i(GLint r,GLint g,GLint b)             { gRXGL->glColor3i(r,g,b); }
void glColor3iv(const GLint* v)                     { gRXGL->glColor3iv(v); }
void glColor3ub(GLubyte r,GLubyte g,GLubyte b)      { gRXGL->glColor3ub(r,g,b); }
void glColor3ubv(const GLubyte* v)                  { gRXGL->glColor3ubv(v); }
void glColor4f(GLfloat r,GLfloat g,GLfloat b,GLfloat a){ gRXGL->glColor4f(r,g,b,a); }
void glColor4fv(const GLfloat* v)                   { gRXGL->glColor4fv(v); }
void glColor4d(GLdouble r,GLdouble g,GLdouble b,GLdouble a){ gRXGL->glColor4d(r,g,b,a); }
void glColor4dv(const GLdouble* v)                  { gRXGL->glColor4dv(v); }
void glColor4b(GLbyte r,GLbyte g,GLbyte b,GLbyte a) { gRXGL->glColor4b(r,g,b,a); }
void glColor4bv(const GLbyte* v)                    { gRXGL->glColor4bv(v); }
void glColor4s(GLshort r,GLshort g,GLshort b,GLshort a){ gRXGL->glColor4s(r,g,b,a); }
void glColor4sv(const GLshort* v)                   { gRXGL->glColor4sv(v); }
void glColor4us(GLushort r,GLushort g,GLushort b,GLushort a){ gRXGL->glColor4us(r,g,b,a); }
void glColor4usv(const GLushort* v)                 { gRXGL->glColor4usv(v); }
void glColor4ui(GLuint r,GLuint g,GLuint b,GLuint a){ gRXGL->glColor4ui(r,g,b,a); }
void glColor4uiv(const GLuint* v)                   { gRXGL->glColor4uiv(v); }
void glColor4i(GLint r,GLint g,GLint b,GLint a)     { gRXGL->glColor4i(r,g,b,a); }
void glColor4iv(const GLint* v)                     { gRXGL->glColor4iv(v); }
void glColor4ub(GLubyte r,GLubyte g,GLubyte b,GLubyte a){ gRXGL->glColor4ub(r,g,b,a); }
void glColor4ubv(const GLubyte* v)                  { gRXGL->glColor4ubv(v); }
void glColorMask(GLboolean r,GLboolean g,GLboolean b,GLboolean a){ gRXGL->glColorMask(r,g,b,a); }
void glCullFace(GLenum m)                           { gRXGL->glCullFace(m); }
void glFrontFace(GLenum m)                          { gRXGL->glFrontFace(m); }
void glPixelStorei(GLenum p,GLint v)                { gRXGL->glPixelStorei(p,v); }
void glDeleteTextures(GLsizei n,const GLuint* t)    { gRXGL->glDeleteTextures(n,t); }
void glDepthFunc(GLenum f)                          { gRXGL->glDepthFunc(f); }
void glDepthMask(GLboolean f)                       { gRXGL->glDepthMask(f); }
void glDepthRange(GLclampd n,GLclampd f)            { gRXGL->glDepthRange(n,f); }
void glDisable(GLenum c)                            { gRXGL->glDisable(c); }
void glDisableClientState(GLenum a)                 { gRXGL->glDisableClientState(a); }
void glArrayElement(GLint i)                        { gRXGL->glArrayElement(i); }
void glDrawArrays(GLenum m,GLint f,GLsizei c)       { gRXGL->glDrawArrays(m,f,c); }
void glDrawBuffer(GLenum m)                         { gRXGL->glDrawBuffer(m); }
void glDrawElements(GLenum m,GLsizei c,GLenum t,const GLvoid* i){ gRXGL->glDrawElements(m,c,t,i); }
void glEnable(GLenum c)                             { gRXGL->glEnable(c); }
void glEnableClientState(GLenum a)                  { gRXGL->glEnableClientState(a); }
void glEnd()                                        { gRXGL->glEnd(); }
void glFinish()                                     { gRXGL->glFinish(); }
void glFlush()                                      { gRXGL->glFlush(); }
void glFogf(GLenum p,GLfloat v)                     { gRXGL->glFogf(p,v); }
void glFogfv(GLenum p,const GLfloat* v)             { gRXGL->glFogfv(p,v); }
void glFogi(GLenum p,GLint v)                       { gRXGL->glFogi(p,v); }
void glFogiv(GLenum p,const GLint* v)               { gRXGL->glFogi(p,v[0]); }

// ---------------------------------------------------------------------------
// glClipPlane
// GL spec: the plane equation is multiplied by the inverse of the current
// modelview matrix and stored in eye space.
// For our software clipper, we store it in eye space so ClipAndFlush can
// transform it correctly to object space at draw time.
//
// eye_plane = MV^{-T} * object_plane
// Equivalently: eye_plane = object_plane * MV^{-1}
// For the common case (orthogonal rotation + translation), MV^{-T} = MV for
// the rotation part, but we need the full inverse for correctness.
//
// Practical approach: store the raw equation AND convert to eye space by
// multiplying the plane (as a row vector) by MV^{-1}.
// Since computing a full 4x4 inverse is complex, we use the fact that for
// rigid-body transforms (R|t), MV^{-1} = [R^T | -R^T*t; 0 0 0 1].
// For general transforms we approximate using the transpose (works for
// pure rotation) with translation correction.
//
// SIMPLEST CORRECT APPROACH: store the plane as-is. Then in ClipAndFlush,
// instead of transforming the plane to object space, transform the vertices
// to eye space for testing but submit them in object space.
// This is handled in ClipAndFlush via the stored MV matrix.
// ---------------------------------------------------------------------------

void glClipPlane(GLenum plane, const GLdouble* equation)
{
    int idx = (int)plane - (int)GL_CLIP_PLANE0;
    if (idx < 0 || idx > 5 || !equation) return;

    // GL spec: eye_plane = MV^{-T} * object_plane (column-vector convention).
    // D3D uses row-vector convention, so D3D_MV = GL_MV^T.
    // Therefore GL_MV^{-T} = D3D_MV^{-1}.
    // eye_plane = D3D_MV^{-1} * equation  (column vector multiply)
    // result[r] = sum_c( inv[r][c] * equation[c] )

    // Always store the raw equation for glGetClipPlane
    g_clipPlaneRaw[idx][0] = equation[0];
    g_clipPlaneRaw[idx][1] = equation[1];
    g_clipPlaneRaw[idx][2] = equation[2];
    g_clipPlaneRaw[idx][3] = equation[3];

    if (!gRXGL) { g_clipPlaneEq[idx][0]=equation[0]; g_clipPlaneEq[idx][1]=equation[1]; g_clipPlaneEq[idx][2]=equation[2]; g_clipPlaneEq[idx][3]=equation[3]; return; }
    const D3DXMATRIX* d3dmv = gRXGL->m_mx.modelView->GetTop();
    if (!d3dmv)   { g_clipPlaneEq[idx][0]=equation[0]; g_clipPlaneEq[idx][1]=equation[1]; g_clipPlaneEq[idx][2]=equation[2]; g_clipPlaneEq[idx][3]=equation[3]; return; }

    D3DXMATRIX inv;
    D3DXMatrixInverse(&inv, NULL, d3dmv);

    double A=equation[0], B=equation[1], C=equation[2], D=equation[3];
    // result[r] = sum_c( inv.m[r][c] * eq[c] )  -- column vector multiply
    g_clipPlaneEq[idx][0] = A*inv.m[0][0] + B*inv.m[0][1] + C*inv.m[0][2] + D*inv.m[0][3];
    g_clipPlaneEq[idx][1] = A*inv.m[1][0] + B*inv.m[1][1] + C*inv.m[1][2] + D*inv.m[1][3];
    g_clipPlaneEq[idx][2] = A*inv.m[2][0] + B*inv.m[2][1] + C*inv.m[2][2] + D*inv.m[2][3];
    g_clipPlaneEq[idx][3] = A*inv.m[3][0] + B*inv.m[3][1] + C*inv.m[3][2] + D*inv.m[3][3];
}

void glGetClipPlane(GLenum plane, GLdouble* equation)
{
    int idx = (int)plane - (int)GL_CLIP_PLANE0;
    if (idx < 0 || idx > 5 || !equation) return;
    // Return the original equation as passed to glClipPlane (GL spec)
    equation[0] = g_clipPlaneRaw[idx][0];
    equation[1] = g_clipPlaneRaw[idx][1];
    equation[2] = g_clipPlaneRaw[idx][2];
    equation[3] = g_clipPlaneRaw[idx][3];
}

void glTexGeni(GLenum c,GLenum p,GLint v)           { gRXGL->glTexGeni(c,p,v); }
void glTexGend(GLenum c,GLenum p,GLdouble v)        { gRXGL->glTexGenf(c,p,(GLfloat)v); }
void glTexGendv(GLenum c,GLenum p,const GLdouble* v){ GLfloat f[4]={(GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]}; gRXGL->glTexGenfv(c,p,f); }
void glTexGenf(GLenum c,GLenum p,GLfloat v)         { gRXGL->glTexGenf(c,p,v); }
void glTexGenfv(GLenum c,GLenum p,const GLfloat* v) { gRXGL->glTexGenfv(c,p,v); }
void glTexGeniv(GLenum c,GLenum p,const GLint* v)   { gRXGL->glTexGeniv(c,p,v); }
void glFrustum(GLdouble l,GLdouble r,GLdouble b,GLdouble t,GLdouble zn,GLdouble zf){ gRXGL->glFrustum(l,r,b,t,zn,zf); }
void glGenTextures(GLsizei n,GLuint* t)             { gRXGL->glGenTextures(n,t); }
void glGetBooleanv(GLenum p,GLboolean* v)           { gRXGL->glGetBooleanv(p,v); }
void glGetFloatv(GLenum p,GLfloat* v)               { gRXGL->glGetFloatv(p,v); }
void glGetIntegerv(GLenum p,GLint* v)               { gRXGL->glGetIntegerv(p,v); }
void glGetTexParameterfv(GLenum tg,GLenum pn,GLfloat* p)
    { switch(pn){case GL_TEXTURE_MIN_FILTER:p[0]=(GLfloat)GL_NEAREST_MIPMAP_LINEAR;break;case GL_TEXTURE_MAG_FILTER:p[0]=(GLfloat)GL_LINEAR;break;case GL_TEXTURE_WRAP_S:case GL_TEXTURE_WRAP_T:p[0]=(GLfloat)GL_REPEAT;break;default:p[0]=0.0f;break;}}
void glGetTexParameteriv(GLenum tg,GLenum pn,GLint* p)
    { GLfloat f=0.0f; glGetTexParameterfv(tg,pn,&f); p[0]=(GLint)f; }
void glGetTexImage(GLenum tg,GLint lv,GLenum fmt,GLenum tp,GLvoid* px)
    { gRXGL->glGetTexImage(tg,lv,fmt,tp,px); }
GLenum glGetError()                                 { return gRXGL->glGetError(); }
const GLubyte* glGetString(GLenum n)                { return gRXGL->glGetString(n); }
void glHint(GLenum t,GLenum m)                      { gRXGL->glHint(t,m); }
GLboolean glIsEnabled(GLenum c)                     { return gRXGL->glIsEnabled(c); }
GLboolean glIsTexture(GLuint t)                     { return gRXGL->glIsTexture(t); }
GLboolean glAreTexturesResident(GLsizei n,const GLuint* t,GLboolean* r){ return gRXGL->glAreTexturesResident(n,t,r); }
void glAccum(GLenum op,GLfloat v)                    { gRXGL->glAccum(op,v); }
void glClearAccum(GLfloat r,GLfloat g,GLfloat b,GLfloat a){ gRXGL->glClearAccum(r,g,b,a); }
void glTexImage1D(GLenum t,GLint lv,GLint fmt,GLsizei w,GLint b,GLenum f,GLenum ty,const GLvoid* px)
    { gRXGL->glTexImage1D(t,lv,fmt,w,b,f,ty,px); }
void glTexSubImage1D(GLenum t,GLint lv,GLint x,GLsizei w,GLenum f,GLenum ty,const GLvoid* px)
    { gRXGL->glTexSubImage1D(t,lv,x,w,f,ty,px); }
void glCopyTexSubImage1D(GLenum t,GLint lv,GLint xo,GLint x,GLint y,GLsizei w)
    { gRXGL->glCopyTexSubImage1D(t,lv,xo,x,y,w); }
void glEdgeFlag(GLboolean f)                         { gRXGL->glEdgeFlag(f); }
void glEdgeFlagv(const GLboolean* f)                 { gRXGL->glEdgeFlagv(f); }
void glEdgeFlagPointer(GLsizei s,const GLvoid* p)    { gRXGL->glEdgeFlagPointer(s,p); }
void glPrioritizeTextures(GLsizei n,const GLuint* t,const GLclampf* p){ gRXGL->glPrioritizeTextures(n,t,p); }
void glGetTexGenfv(GLenum c,GLenum p,GLfloat* v)     { gRXGL->glGetTexGenfv(c,p,v); }
void glGetTexGendv(GLenum c,GLenum p,GLdouble* v)    { gRXGL->glGetTexGendv(c,p,v); }
void glGetTexGeniv(GLenum c,GLenum p,GLint* v)       { gRXGL->glGetTexGeniv(c,p,v); }
void glLoadIdentity()                               { gRXGL->glLoadIdentity(); }
void glLoadMatrixf(const GLfloat* m)                { gRXGL->glLoadMatrixf(m); }
void glMatrixMode(GLenum m)                         { gRXGL->glMatrixMode(m); }
void glMultMatrixf(const GLfloat* m)                { gRXGL->glMultMatrixf(m); }
void glMultMatrixd(const GLdouble* m)               { gRXGL->glMultMatrixd(m); }
void glLoadMatrixd(const GLdouble* m)               { gRXGL->glLoadMatrixd(m); }
void glLoadTransposeMatrixf(const GLfloat* m)        { gRXGL->glLoadTransposeMatrixf(m); }
void glLoadTransposeMatrixd(const GLdouble* m)       { gRXGL->glLoadTransposeMatrixd(m); }
void glMultTransposeMatrixf(const GLfloat* m)        { gRXGL->glMultTransposeMatrixf(m); }
void glMultTransposeMatrixd(const GLdouble* m)       { gRXGL->glMultTransposeMatrixd(m); }
void glRotated(GLdouble a,GLdouble x,GLdouble y,GLdouble z){ gRXGL->glRotated(a,x,y,z); }
void glScaled(GLdouble x,GLdouble y,GLdouble z)     { gRXGL->glScaled(x,y,z); }
void glTranslated(GLdouble x,GLdouble y,GLdouble z) { gRXGL->glTranslated(x,y,z); }
void glNormal3f(GLfloat x,GLfloat y,GLfloat z)      { gRXGL->glNormal3f(x,y,z); }
void glNormal3fv(const GLfloat* v)                  { gRXGL->glNormal3fv(v); }
void glNormal3b(GLbyte x,GLbyte y,GLbyte z)         { gRXGL->glNormal3b(x,y,z); }
void glNormal3bv(const GLbyte* v)                   { gRXGL->glNormal3bv(v); }
void glNormal3s(GLshort x,GLshort y,GLshort z)      { gRXGL->glNormal3s(x,y,z); }
void glNormal3sv(const GLshort* v)                  { gRXGL->glNormal3sv(v); }
void glNormal3i(GLint x,GLint y,GLint z)            { gRXGL->glNormal3i(x,y,z); }
void glNormal3iv(const GLint* v)                    { gRXGL->glNormal3iv(v); }
void glNormal3d(GLdouble x,GLdouble y,GLdouble z)   { gRXGL->glNormal3d(x,y,z); }
void glNormal3dv(const GLdouble* v)                  { gRXGL->glNormal3dv(v); }
void glNormalPointer(GLenum t,GLsizei s,const GLvoid* p){ gRXGL->glNormalPointer(t,s,p); }
void glOrtho(GLdouble l,GLdouble r,GLdouble b,GLdouble t,GLdouble zn,GLdouble zf){ gRXGL->glOrtho(l,r,b,t,zn,zf); }
void glPolygonMode(GLenum f,GLenum m)               { gRXGL->glPolygonMode(f,m); }
void glPolygonOffset(GLfloat f,GLfloat u)           { gRXGL->glPolygonOffset(f,u); }
void glPopMatrix()                                  { gRXGL->glPopMatrix(); }
void glPushMatrix()                                 { gRXGL->glPushMatrix(); }
void glReadBuffer(GLenum m)                         { gRXGL->glReadBuffer(m); }
void glReadPixels(GLint x,GLint y,GLsizei w,GLsizei h,GLenum f,GLenum t,GLvoid* p){ gRXGL->glReadPixels(x,y,w,h,f,t,p); }
void glCopyPixels(GLint x,GLint y,GLsizei w,GLsizei h,GLenum t)        { gRXGL->glCopyPixels(x,y,w,h,t); }


// ---------------------------------------------------------------------------
// Raster position
// ---------------------------------------------------------------------------

// WindowPos sets raster pos directly in window coords (no transform needed)
static void SetWindowPos(GLfloat x, GLfloat y)
{
    gRXGL->m_rasterX = x;
    gRXGL->m_rasterY = y;
    gRXGL->m_rasterValid = true;
}

// RasterPos transforms through modelview+projection then to window coords
static void SetRasterPos(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    // Transform through modelview then projection
    const D3DXMATRIX* mv   = gRXGL->m_mx.modelView->GetTop();
    const D3DXMATRIX* proj = gRXGL->m_mx.projection->GetTop();
    if (!mv || !proj) { gRXGL->m_rasterValid = false; return; }

    // Eye coords
    GLfloat ex = x*mv->_11 + y*mv->_21 + z*mv->_31 + w*mv->_41;
    GLfloat ey = x*mv->_12 + y*mv->_22 + z*mv->_32 + w*mv->_42;
    GLfloat ez = x*mv->_13 + y*mv->_23 + z*mv->_33 + w*mv->_43;
    GLfloat ew = x*mv->_14 + y*mv->_24 + z*mv->_34 + w*mv->_44;

    // Clip coords
    GLfloat cx = ex*proj->_11 + ey*proj->_21 + ez*proj->_31 + ew*proj->_41;
    GLfloat cy = ex*proj->_12 + ey*proj->_22 + ez*proj->_32 + ew*proj->_42;
    GLfloat cw = ex*proj->_14 + ey*proj->_24 + ez*proj->_34 + ew*proj->_44;

    if (cw == 0.0f) { gRXGL->m_rasterValid = false; return; }

    // NDC -> window coords
    GLfloat ndcX = cx / cw;
    GLfloat ndcY = cy / cw;
    gRXGL->m_rasterX = (ndcX * 0.5f + 0.5f) * gRXGL->m_rs.viewportW + gRXGL->m_rs.viewportX;
    gRXGL->m_rasterY = (ndcY * 0.5f + 0.5f) * gRXGL->m_rs.viewportH + gRXGL->m_rs.viewportY;
    gRXGL->m_rasterValid = true;
}

void glRasterPos2f(GLfloat x, GLfloat y)       { SetRasterPos(x, y, 0, 1); }
void glRasterPos2d(GLdouble x,GLdouble y)      { SetRasterPos((GLfloat)x,(GLfloat)y,0,1); }
void glRasterPos2dv(const GLdouble* v)         { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],0,1); }
void glRasterPos2fv(const GLfloat* v)          { SetRasterPos(v[0],v[1],0,1); }
void glRasterPos2iv(const GLint* v)            { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],0,1); }
void glRasterPos2s(GLshort x,GLshort y)        { SetRasterPos((GLfloat)x,(GLfloat)y,0,1); }
void glRasterPos2sv(const GLshort* v)          { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],0,1); }
void glRasterPos2i(GLint x, GLint y)            { SetRasterPos((GLfloat)x, (GLfloat)y, 0, 1); }
void glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)           { SetRasterPos(x, y, z, 1); }
void glRasterPos3d(GLdouble x,GLdouble y,GLdouble z)          { SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,1); }
void glRasterPos3dv(const GLdouble* v)                        { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],1); }
void glRasterPos3fv(const GLfloat* v)                         { SetRasterPos(v[0],v[1],v[2],1); }
void glRasterPos3i(GLint x,GLint y,GLint z)                   { SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,1); }
void glRasterPos3iv(const GLint* v)                           { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],1); }
void glRasterPos3s(GLshort x,GLshort y,GLshort z)             { SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,1); }
void glRasterPos3sv(const GLshort* v)                         { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],1); }
void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w){ SetRasterPos(x, y, z, w); }
void glRasterPos4d(GLdouble x,GLdouble y,GLdouble z,GLdouble w){ SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
void glRasterPos4dv(const GLdouble* v)                        { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glRasterPos4fv(const GLfloat* v)                         { SetRasterPos(v[0],v[1],v[2],v[3]); }
void glRasterPos4i(GLint x,GLint y,GLint z,GLint w)           { SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
void glRasterPos4iv(const GLint* v)                           { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glRasterPos4s(GLshort x,GLshort y,GLshort z,GLshort w)   { SetRasterPos((GLfloat)x,(GLfloat)y,(GLfloat)z,(GLfloat)w); }
void glRasterPos4sv(const GLshort* v)                         { SetRasterPos((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glWindowPos2f(GLfloat x, GLfloat y)        { SetWindowPos(x, y); }
void glWindowPos2i(GLint x, GLint y)            { SetWindowPos((GLfloat)x, (GLfloat)y); }
void glWindowPos2fARB(GLfloat x, GLfloat y)     { SetWindowPos(x, y); }
void glWindowPos2iARB(GLint x, GLint y)         { SetWindowPos((GLfloat)x, (GLfloat)y); }

void glPixelZoom(GLfloat xf, GLfloat yf)
{
    gRXGL->m_pixelZoomX = xf;
    gRXGL->m_pixelZoomY = yf;
}



// ---------------------------------------------------------------------------
// glDrawPixels -- draw pixel rectangle at current raster position
//
// Implementation: upload pixel data as a temporary RGBA texture, then draw
// a screen-aligned quad at (m_rasterX, m_rasterY). This is exactly what
// hardware-accelerated GL drivers do internally.
//
// Supported format/type combos:
//   GL_RGBA  + GL_UNSIGNED_BYTE  (most common)
//   GL_RGB   + GL_UNSIGNED_BYTE
//   GL_LUMINANCE + GL_UNSIGNED_BYTE
//   GL_LUMINANCE_ALPHA + GL_UNSIGNED_BYTE
// ---------------------------------------------------------------------------
void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
    if (!pixels || width <= 0 || height <= 0) return;
    if (!gRXGL->m_rasterValid) return;

    if (type != GL_UNSIGNED_BYTE)
    {
        DbgPrint("glDrawPixels: unsupported type 0x%X -- only GL_UNSIGNED_BYTE implemented\n", type);
        return;
    }

    // Build RGBA pixel data
    int srcStride = gRXGL->m_rs.unpackRowLength > 0 ? gRXGL->m_rs.unpackRowLength : width;
    int srcBpp = 1;
    switch (format)
    {
    case GL_RGBA:             srcBpp = 4; break;
    case GL_RGB:              srcBpp = 3; break;
    case GL_LUMINANCE_ALPHA:  srcBpp = 2; break;
    case GL_LUMINANCE:
    case GL_ALPHA:            srcBpp = 1; break;
    default:
        DbgPrint("glDrawPixels: unsupported format 0x%X\n", format);
        return;
    }

    // Align stride
    int align = gRXGL->m_rs.unpackAlignment;
    if (align < 1) align = 1;
    int rowBytes = srcStride * srcBpp;
    rowBytes = (rowBytes + align - 1) & ~(align - 1);

    // Convert to RGBA
    const unsigned char* src = (const unsigned char*)pixels;
    unsigned char* rgba = new unsigned char[width * height * 4];

    for (int row = 0; row < height; row++)
    {
        // GL pixel rows start from bottom -- flip for D3D texture (top=0)
        const unsigned char* srcRow = src + (height - 1 - row) * rowBytes;
        unsigned char* dstRow = rgba + row * width * 4;

        for (int col = 0; col < width; col++)
        {
            const unsigned char* s = srcRow + col * srcBpp;
            unsigned char* d = dstRow + col * 4;
            switch (format)
            {
            case GL_RGBA:
                d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3]; break;
            case GL_RGB:
                d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=255;   break;
            case GL_LUMINANCE_ALPHA:
                d[0]=s[0]; d[1]=s[0]; d[2]=s[0]; d[3]=s[1];  break;
            case GL_LUMINANCE:
                d[0]=s[0]; d[1]=s[0]; d[2]=s[0]; d[3]=255;   break;
            case GL_ALPHA:
                d[0]=255;  d[1]=255;  d[2]=255;  d[3]=s[0];  break;
            }
        }
    }

    // Upload as temp texture.
    // Xbox NV2A requires power-of-2 texture dimensions -- round up, adjust UVs.
    GLsizei texW = 1; while (texW < width)  texW <<= 1;
    GLsizei texH = 1; while (texH < height) texH <<= 1;
    GLfloat u1 = (GLfloat)width  / (GLfloat)texW;
    GLfloat v1 = (GLfloat)height / (GLfloat)texH;

    unsigned char* uploadBuf = rgba;
    if (texW != width || texH != height)
    {
        uploadBuf = new unsigned char[texW * texH * 4];
        memset(uploadBuf, 0, texW * texH * 4);
        for (int row = 0; row < height; row++)
            memcpy(uploadBuf + row * texW * 4, rgba + row * width * 4, width * 4);
        delete[] rgba;
        rgba = NULL;
    }

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, uploadBuf);
    delete[] uploadBuf;

    GLfloat x0 = gRXGL->m_rasterX;
    GLfloat y0 = gRXGL->m_rasterY;
    GLfloat x1 = x0 + width  * gRXGL->m_pixelZoomX;
    GLfloat y1 = y0 + height * fabsf(gRXGL->m_pixelZoomY);

    // Save relevant state
    GLboolean wasDepth   = glIsEnabled(GL_DEPTH_TEST);
    GLboolean wasBlend   = glIsEnabled(GL_BLEND);
    GLboolean wasLighting = glIsEnabled(GL_LIGHTING);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    if (format == GL_RGBA || format == GL_LUMINANCE_ALPHA || format == GL_ALPHA)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor4f(1,1,1,1);

    // Set up 2D ortho for pixel-exact drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    GLsizei vw = (GLsizei)gRXGL->m_rs.viewportW;
    GLsizei vh = (GLsizei)gRXGL->m_rs.viewportH;
    glOrtho(0, vw, 0, vh, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glTexCoord2f(0,  v1); glVertex2f(x0, y0);
    glTexCoord2f(u1, v1); glVertex2f(x1, y0);
    glTexCoord2f(u1, 0 ); glVertex2f(x1, y1);
    glTexCoord2f(0,  0 ); glVertex2f(x0, y1);
    glEnd();

    // Restore state
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    if (wasDepth)    glEnable(GL_DEPTH_TEST);
    if (wasBlend)    glEnable(GL_BLEND);
    if (wasLighting) glEnable(GL_LIGHTING);

    // Advance raster position by image width
    gRXGL->m_rasterX += width * gRXGL->m_pixelZoomX;

    glDeleteTextures(1, &texId);

    // Restore matrices
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// ---------------------------------------------------------------------------
// glBitmap -- STUBBED
// 1-bit mask drawing is complex and rarely used in modern engines.
// We advance the raster position correctly so layout still works.
// ---------------------------------------------------------------------------
static bool s_bitmapWarned = false;
void glBitmap(GLsizei /*width*/, GLsizei /*height*/,
              GLfloat /*xorig*/, GLfloat /*yorig*/,
              GLfloat xmove, GLfloat ymove, const GLubyte* /*bitmap*/)
{
    if (!s_bitmapWarned)
    {
        s_bitmapWarned = true;
        DbgPrint("glBitmap: STUBBED -- 1-bit mask rendering not implemented. Raster position will advance.\n");
    }
    // Advance raster position so multi-character rendering still spaces correctly
    if (gRXGL->m_rasterValid)
    {
        gRXGL->m_rasterX += xmove;
        gRXGL->m_rasterY += ymove;
    }
}
void glRotatef(GLfloat a,GLfloat x,GLfloat y,GLfloat z){ gRXGL->glRotatef(a,x,y,z); }
void glScalef(GLfloat x,GLfloat y,GLfloat z)        { gRXGL->glScalef(x,y,z); }
void glScissor(GLint x,GLint y,GLsizei w,GLsizei h) { gRXGL->glScissor(x,y,w,h); }
void glShadeModel(GLenum m)                         { gRXGL->glShadeModel(m); }
void glStencilFunc(GLenum f,GLint r,GLuint m)       { gRXGL->glStencilFunc(f,r,m); }
void glStencilMask(GLuint m)                        { gRXGL->glStencilMask(m); }
void glStencilOp(GLenum f,GLenum z,GLenum p)        { gRXGL->glStencilOp(f,z,p); }
void glTexCoord2f(GLfloat s,GLfloat t)              { gRXGL->glTexCoord2f(s,t); }
void glTexCoord2fv(const GLfloat* v)                { gRXGL->glTexCoord2fv(v); }
void glTexCoord2i(GLint s,GLint t)                  { gRXGL->glTexCoord2i(s,t); }
void glTexCoord2s(GLshort s,GLshort t)              { gRXGL->glTexCoord2s(s,t); }
void glTexCoord1f(GLfloat s)                        { gRXGL->glTexCoord1f(s); }
void glTexCoord3f(GLfloat s,GLfloat t,GLfloat r)    { gRXGL->glTexCoord3f(s,t,r); }
void glTexCoord3dv(const GLdouble* v)               { gRXGL->glTexCoord3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
void glTexCoord3s(GLshort s,GLshort t,GLshort r)    { gRXGL->glTexCoord3f((GLfloat)s,(GLfloat)t,(GLfloat)r); }
void glTexCoord3sv(const GLshort* v)                { gRXGL->glTexCoord3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
void glTexCoord3i(GLint s,GLint t,GLint r)          { gRXGL->glTexCoord3f((GLfloat)s,(GLfloat)t,(GLfloat)r); }
void glTexCoord3iv(const GLint* v)                  { gRXGL->glTexCoord3f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2]); }
void glTexCoord4f(GLfloat s,GLfloat t,GLfloat r,GLfloat q){ gRXGL->glTexCoord4f(s,t,r,q); }
void glTexCoord4dv(const GLdouble* v)               { gRXGL->glTexCoord4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glTexCoord4i(GLint s,GLint t,GLint r,GLint q)  { gRXGL->glTexCoord4f((GLfloat)s,(GLfloat)t,(GLfloat)r,(GLfloat)q); }
void glTexCoord4iv(const GLint* v)                  { gRXGL->glTexCoord4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glTexCoord4s(GLshort s,GLshort t,GLshort r,GLshort q){ gRXGL->glTexCoord4f((GLfloat)s,(GLfloat)t,(GLfloat)r,(GLfloat)q); }
void glTexCoord4sv(const GLshort* v)                { gRXGL->glTexCoord4f((GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]); }
void glTexCoord1d(GLdouble s)                        { gRXGL->glTexCoord1d(s); }
void glTexCoord1dv(const GLdouble* v)                { gRXGL->glTexCoord1d(v[0]); }
void glTexCoord1i(GLint s)                           { gRXGL->glTexCoord2f((GLfloat)s,0.0f); }
void glTexCoord1iv(const GLint* v)                   { gRXGL->glTexCoord2f((GLfloat)v[0],0.0f); }
void glTexCoord1s(GLshort s)                         { gRXGL->glTexCoord2f((GLfloat)s,0.0f); }
void glTexCoord1sv(const GLshort* v)                 { gRXGL->glTexCoord2f((GLfloat)v[0],0.0f); }
void glTexCoord2d(GLdouble s,GLdouble t)             { gRXGL->glTexCoord2d(s,t); }
void glTexCoord3d(GLdouble s,GLdouble t,GLdouble r)  { gRXGL->glTexCoord3d(s,t,r); }
void glTexCoord4d(GLdouble s,GLdouble t,GLdouble r,GLdouble q){ gRXGL->glTexCoord4d(s,t,r,q); }
void glTexCoord1fv(const GLfloat* v)                 { gRXGL->glTexCoord1fv(v); }
void glTexCoord3fv(const GLfloat* v)                 { gRXGL->glTexCoord3fv(v); }
void glTexCoord4fv(const GLfloat* v)                 { gRXGL->glTexCoord4fv(v); }
void glTexCoord2iv(const GLint* v)                   { gRXGL->glTexCoord2iv(v); }
void glTexCoord2sv(const GLshort* v)                 { gRXGL->glTexCoord2sv(v); }
void glTexCoord2dv(const GLdouble* v)                { gRXGL->glTexCoord2dv(v); }
void glTexCoordPointer(GLint s,GLenum t,GLsizei st,const GLvoid* p){ gRXGL->glTexCoordPointer(s,t,st,p); }
void glClientActiveTextureARB(GLenum t)                            { gRXGL->glClientActiveTextureARB(t); }
void glTexEnvf(GLenum tg,GLenum p,GLfloat v)        { gRXGL->glTexEnvf(tg,p,v); }
void glTexEnvi(GLenum tg,GLenum p,GLint v)          { gRXGL->glTexEnvi(tg,p,v); }
void glTexEnviv(GLenum tg,GLenum p,const GLint* v)  { gRXGL->glTexEnvi(tg,p,v[0]); }
void glTexEnvfv(GLenum tg,GLenum p,const GLfloat* v){ gRXGL->glTexEnvfv(tg,p,v); }
void glTexImage2D(GLenum tg,GLint lv,GLint ifmt,GLsizei w,GLsizei h,GLint b,GLenum f,GLenum t,const GLvoid* p){ gRXGL->glTexImage2D(tg,lv,ifmt,w,h,b,f,t,p); }
void glTexParameterf(GLenum t,GLenum p,GLfloat v)   { gRXGL->glTexParameterf(t,p,v); }
void glTexParameteri(GLenum t,GLenum p,GLint v)     { gRXGL->glTexParameteri(t,p,v); }
void glTexParameterfv(GLenum t,GLenum p,const GLfloat* v){ gRXGL->glTexParameterf(t,p,v[0]); }
void glTexParameteriv(GLenum t,GLenum p,const GLint* v)  { gRXGL->glTexParameteri(t,p,v[0]); }
void glTexSubImage2D(GLenum tg,GLint lv,GLint xo,GLint yo,GLsizei w,GLsizei h,GLenum f,GLenum t,const GLvoid* p){ gRXGL->glTexSubImage2D(tg,lv,xo,yo,w,h,f,t,p); }
void glCopyTexSubImage2D(GLenum tg,GLint lv,GLint xo,GLint yo,GLint x,GLint y,GLsizei w,GLsizei h){ gRXGL->glCopyTexSubImage2D(tg,lv,xo,yo,x,y,w,h); }
// glCopyTexImage1D: GL_TEXTURE_1D not supported on Xbox NV2A/D3D8 -- stub
void glCopyTexImage1D(GLenum tg,GLint lv,GLenum fmt,GLint x,GLint y,GLsizei w,GLint b)
    { DbgPrint("glCopyTexImage1D: GL_TEXTURE_1D not supported on this platform\n"); (void)tg;(void)lv;(void)fmt;(void)x;(void)y;(void)w;(void)b; }
// glCopyColorTable: colour tables not supported on Xbox NV2A/D3D8 -- stub
void glCopyColorTable(GLenum tg,GLenum fmt,GLint x,GLint y,GLsizei w)
    { DbgPrint("glCopyColorTable: colour tables not supported on this platform\n"); (void)tg;(void)fmt;(void)x;(void)y;(void)w; }
void glCopyTexImage2D(GLenum tg,GLint lv,GLenum fmt,GLint x,GLint y,GLsizei w,GLsizei h,GLint b){ gRXGL->glCopyTexImage2D(tg,lv,fmt,x,y,w,h,b); }
void glTranslatef(GLfloat x,GLfloat y,GLfloat z)    { gRXGL->glTranslatef(x,y,z); }
void glVertex2f(GLfloat x,GLfloat y)                { gRXGL->glVertex2f(x,y); }
void glVertex2fv(const GLfloat* v)                  { gRXGL->glVertex2fv(v); }
void glVertex2i(GLint x,GLint y)                    { gRXGL->glVertex2i(x,y); }
void glVertex2iv(const GLint* v)                    { gRXGL->glVertex2iv(v); }
void glVertex2s(GLshort x,GLshort y)                { gRXGL->glVertex2s(x,y); }
void glVertex2d(GLdouble x,GLdouble y)              { gRXGL->glVertex2d(x,y); }
void glVertex2dv(const GLdouble* v)                 { gRXGL->glVertex2dv(v); }
void glVertex2sv(const GLshort* v)                  { gRXGL->glVertex2sv(v); }
void glVertex3f(GLfloat x,GLfloat y,GLfloat z)      { gRXGL->glVertex3f(x,y,z); }
void glVertex3fv(const GLfloat* v)                  { gRXGL->glVertex3fv(v); }
void glVertex3i(GLint x,GLint y,GLint z)            { gRXGL->glVertex3i(x,y,z); }
void glVertex3iv(const GLint* v)                    { gRXGL->glVertex3iv(v); }
void glVertex3s(GLshort x,GLshort y,GLshort z)      { gRXGL->glVertex3s(x,y,z); }
void glVertex3d(GLdouble x,GLdouble y,GLdouble z)   { gRXGL->glVertex3d(x,y,z); }
void glVertex3dv(const GLdouble* v)                  { gRXGL->glVertex3dv(v); }
void glVertex3sv(const GLshort* v)                   { gRXGL->glVertex3sv(v); }
void glVertex4f(GLfloat x,GLfloat y,GLfloat z,GLfloat w){ gRXGL->glVertex4f(x,y,z,w); }
void glVertex4fv(const GLfloat* v)                  { gRXGL->glVertex4fv(v); }
void glVertex4d(GLdouble x,GLdouble y,GLdouble z,GLdouble w){ gRXGL->glVertex4d(x,y,z,w); }
void glVertex4dv(const GLdouble* v)                  { gRXGL->glVertex4dv(v); }
void glVertex4i(GLint x,GLint y,GLint z,GLint w)     { gRXGL->glVertex4i(x,y,z,w); }
void glVertex4iv(const GLint* v)                     { gRXGL->glVertex4iv(v); }
void glVertex4s(GLshort x,GLshort y,GLshort z,GLshort w){ gRXGL->glVertex4s(x,y,z,w); }
void glVertex4sv(const GLshort* v)                   { gRXGL->glVertex4sv(v); }
void glVertexPointer(GLint s,GLenum t,GLsizei st,const GLvoid* p){ gRXGL->glVertexPointer(s,t,st,p); }
void glColorPointer(GLint s,GLenum t,GLsizei st,const GLvoid* p){ gRXGL->glColorPointer(s,t,st,p); }
void glViewport(GLint x,GLint y,GLsizei w,GLsizei h){ gRXGL->glViewport(x,y,w,h); }
void glLineWidth(GLfloat w)                         { gRXGL->glLineWidth(w); }
void glPointSize(GLfloat s)                         { gRXGL->glPointSize(s); }
void glLightfv(GLenum l,GLenum p,const GLfloat* v)  { gRXGL->glLightfv(l,p,v); }
void glLightf(GLenum l,GLenum p,GLfloat v)          { gRXGL->glLightf(l,p,v); }
void glLighti(GLenum l,GLenum p,GLint v)            { gRXGL->glLightf(l,p,(GLfloat)v); }
void glLightiv(GLenum l,GLenum p,const GLint* v)    { GLfloat f[4]={(GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]}; gRXGL->glLightfv(l,p,f); }
void glLightModelfv(GLenum p,const GLfloat* v)      { gRXGL->glLightModelfv(p,v); }
void glLightModelf(GLenum p,GLfloat v)              { gRXGL->glLightModelf(p,v); }
void glLightModeli(GLenum p, GLint v)               { gRXGL->glLightModeli(p,v); }
void glLightModeliv(GLenum p,const GLint* v)        { GLfloat f[4]={(GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]}; gRXGL->glLightModelfv(p,f); }
void glMaterialfv(GLenum f,GLenum p,const GLfloat* v){ gRXGL->glMaterialfv(f,p,v); }
void glMaterialf(GLenum f,GLenum p,GLfloat v)       { gRXGL->glMaterialf(f,p,v); }
void glMateriali(GLenum f,GLenum p,GLint v)         { gRXGL->glMaterialf(f,p,(GLfloat)v); }
void glMaterialiv(GLenum f,GLenum p,const GLint* v) { GLfloat fl[4]={(GLfloat)v[0],(GLfloat)v[1],(GLfloat)v[2],(GLfloat)v[3]}; gRXGL->glMaterialfv(f,p,fl); }
void glGetLightfv(GLenum l,GLenum p,GLfloat* v)     { gRXGL->glGetLightfv(l,p,v); }
void glGetMaterialfv(GLenum f,GLenum p,GLfloat* v)  { gRXGL->glGetMaterialfv(f,p,v); }
void glGetLightiv(GLenum l,GLenum p,GLint* v)        { gRXGL->glGetLightiv(l,p,v); }
void glGetMaterialiv(GLenum f,GLenum p,GLint* v)     { gRXGL->glGetMaterialiv(f,p,v); }
void glGetDoublev(GLenum p,GLdouble* v)              { gRXGL->glGetDoublev(p,v); }
void glGetTexEnvfv(GLenum tg,GLenum pn,GLfloat* p)   { gRXGL->glGetTexEnvfv(tg,pn,p); }
void glGetTexEnviv(GLenum tg,GLenum pn,GLint* p)     { gRXGL->glGetTexEnviv(tg,pn,p); }
void glGetPointerv(GLenum pn,GLvoid** p)             { gRXGL->glGetPointerv(pn,p); }
GLboolean glIsList(GLuint l)                         { return gRXGL->glIsList(l); }
void glGetTexLevelParameterfv(GLenum tg,GLint lv,GLenum pn,GLfloat* p){ gRXGL->glGetTexLevelParameterfv(tg,lv,pn,p); }
void glGetTexLevelParameteriv(GLenum tg,GLint lv,GLenum pn,GLint* p)  { gRXGL->glGetTexLevelParameteriv(tg,lv,pn,p); }
void glColorMaterial(GLenum f,GLenum m)             { gRXGL->glColorMaterial(f,m); }
void glActiveTextureARB(GLenum t)                   { gRXGL->glActiveTextureARB(t); }
void glMultiTexCoord2fARB(GLenum t,GLfloat s,GLfloat u){ gRXGL->glMultiTexCoord2fARB(t,s,u); }
void glMultiTexCoord3f(GLenum t,GLfloat s,GLfloat u,GLfloat r)      { gRXGL->glMultiTexCoord3f(t,s,u,r); }
void glMultiTexCoord4f(GLenum t,GLfloat s,GLfloat u,GLfloat r,GLfloat q){ gRXGL->glMultiTexCoord4f(t,s,u,r,q); }
GLuint glGenLists(GLsizei r)                        { return gRXGL->glGenLists(r); }
void glNewList(GLuint l,GLenum m)                   { gRXGL->glNewList(l,m); }
void glEndList()                                    { gRXGL->glEndList(); }
void glCallList(GLuint l)                           { gRXGL->glCallList(l); }
void glCallLists(GLsizei n,GLenum t,const GLvoid* l){ gRXGL->glCallLists(n,t,l); }
void glListBase(GLuint b)                           { gRXGL->glListBase(b); }
void glDeleteLists(GLuint l,GLsizei r)              { gRXGL->glDeleteLists(l,r); }
void glPushAttrib(GLbitfield mask)                  { gRXGL->glPushAttrib(mask); }
void glPopAttrib()                                  { gRXGL->glPopAttrib(); }
void glPushClientAttrib(GLbitfield mask)            { gRXGL->glPushClientAttrib(mask); }
void glPopClientAttrib()                            { gRXGL->glPopClientAttrib(); }

// ---------------------------------------------------------------------------
// glRect family -- draw axis-aligned filled rectangle.
// Equivalent to: glBegin(GL_QUADS); 4 vertices with current colour; glEnd().
// The spec says the quad uses corners (x1,y1), (x2,y1), (x2,y2), (x1,y2).
// ---------------------------------------------------------------------------
void glRectf(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2)
{
    glBegin(GL_QUADS);
    glVertex2f(x1,y1); glVertex2f(x2,y1);
    glVertex2f(x2,y2); glVertex2f(x1,y2);
    glEnd();
}
void glRecti(GLint x1,GLint y1,GLint x2,GLint y2)
{
    glRectf((GLfloat)x1,(GLfloat)y1,(GLfloat)x2,(GLfloat)y2);
}
void glRects(GLshort x1,GLshort y1,GLshort x2,GLshort y2)
{
    glRectf((GLfloat)x1,(GLfloat)y1,(GLfloat)x2,(GLfloat)y2);
}
void glRectd(GLdouble x1,GLdouble y1,GLdouble x2,GLdouble y2)
{
    glRectf((GLfloat)x1,(GLfloat)y1,(GLfloat)x2,(GLfloat)y2);
}
void glRectfv(const GLfloat* v1,const GLfloat* v2)  { glRectf(v1[0],v1[1],v2[0],v2[1]); }
void glRectiv(const GLint* v1,const GLint* v2)       { glRecti(v1[0],v1[1],v2[0],v2[1]); }
void glRectsv(const GLshort* v1,const GLshort* v2)   { glRects(v1[0],v1[1],v2[0],v2[1]); }
void glRectdv(const GLdouble* v1,const GLdouble* v2) { glRectd(v1[0],v1[1],v2[0],v2[1]); }

// ---------------------------------------------------------------------------
// glInterleavedArrays
//
// Decomposes a packed interleaved vertex buffer into individual gl*Pointer
// calls plus the appropriate glEnableClientState/glDisableClientState.
// This is a pure client-state setup function -- no drawing happens here.
//
// Layout reference (all sizes in bytes, float=4, ubyte=1):
//
//  GL_V2F              | V2f                      | stride  8
//  GL_V3F              | V3f                      | stride 12
//  GL_C4UB_V2F         | C4ub V2f                 | stride 12
//  GL_C4UB_V3F         | C4ub V3f                 | stride 16
//  GL_C3F_V3F          | C3f V3f                  | stride 24
//  GL_N3F_V3F          | N3f V3f                  | stride 24
//  GL_C4F_N3F_V3F      | C4f N3f V3f              | stride 40
//  GL_T2F_V3F          | T2f V3f                  | stride 20
//  GL_T2F_C4UB_V3F     | T2f C4ub V3f             | stride 24
//  GL_T2F_C3F_V3F      | T2f C3f V3f              | stride 32
//  GL_T2F_N3F_V3F      | T2f N3f V3f              | stride 32
//  GL_T2F_C4F_N3F_V3F  | T2f C4f N3f V3f          | stride 48
// ---------------------------------------------------------------------------

void glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid* pointer)
{
    const GLubyte* p = (const GLubyte*)pointer;

    // We always disable everything first, then re-enable what this format provides.
    // This matches the GL spec: glInterleavedArrays implicitly disables arrays
    // not present in the format.
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Sizes of components
    const int F = sizeof(GLfloat);   // 4
    const int UB = sizeof(GLubyte);  // 1

    // Offsets and strides computed per format.
    // Variables: et=has texcoord, ec=has color, en=has normal
    //            ct=color type, cs=color size (components)
    //            texOff, colOff, normOff, vertOff = byte offsets within stride
    //            vs = vertex size (2 or 3)

    bool et=false, ec=false, en=false;
    GLenum  ct   = GL_FLOAT;
    int     cs   = 4;          // colour components
    int     vs   = 3;          // vertex components
    int     texOff=0, colOff=0, normOff=0, vertOff=0;
    GLsizei autoStride = 0;
    int ts = 2; // tex coord components (2 or 4)

    switch (format)
    {
    case GL_V2F:
        vs=2;
        vertOff=0; autoStride=2*F;
        break;

    case GL_V3F:
        vs=3;
        vertOff=0; autoStride=3*F;
        break;

    case GL_C4UB_V2F:
        ec=true; ct=GL_UNSIGNED_BYTE; cs=4; vs=2;
        colOff=0; vertOff=4*UB; autoStride=4*UB+2*F;
        break;

    case GL_C4UB_V3F:
        ec=true; ct=GL_UNSIGNED_BYTE; cs=4; vs=3;
        colOff=0; vertOff=4*UB; autoStride=4*UB+3*F;
        break;

    case GL_C3F_V3F:
        ec=true; ct=GL_FLOAT; cs=3; vs=3;
        colOff=0; vertOff=3*F; autoStride=6*F;
        break;

    case GL_N3F_V3F:
        en=true; vs=3;
        normOff=0; vertOff=3*F; autoStride=6*F;
        break;

    case GL_C4F_N3F_V3F:
        ec=true; ct=GL_FLOAT; cs=4;
        en=true; vs=3;
        colOff=0; normOff=4*F; vertOff=7*F; autoStride=10*F;
        break;

    case GL_T2F_V3F:
        et=true; vs=3;
        texOff=0; vertOff=2*F; autoStride=5*F;
        break;

    case GL_T2F_C4UB_V3F:
        et=true; ec=true; ct=GL_UNSIGNED_BYTE; cs=4; vs=3;
        texOff=0; colOff=2*F; vertOff=2*F+4*UB; autoStride=2*F+4*UB+3*F;
        break;

    case GL_T2F_C3F_V3F:
        et=true; ec=true; ct=GL_FLOAT; cs=3; vs=3;
        texOff=0; colOff=2*F; vertOff=5*F; autoStride=8*F;
        break;

    case GL_T2F_N3F_V3F:
        et=true; en=true; vs=3;
        texOff=0; normOff=2*F; vertOff=5*F; autoStride=8*F;
        break;

    case GL_T2F_C4F_N3F_V3F:
        et=true; ec=true; ct=GL_FLOAT; cs=4;
        en=true; vs=3;
        texOff=0; colOff=2*F; normOff=6*F; vertOff=9*F; autoStride=12*F;
        break;

    // GL_T4F_V4F: T4f V4f -- stride 32
    // 4D homogeneous texcoords + 4D homogeneous vertex
    case GL_T4F_V4F:
        et=true; vs=4; ts=4;
        texOff=0; vertOff=4*F; autoStride=8*F;
        break;

    // GL_T4F_C4F_N3F_V4F: T4f C4f N3f V4f -- stride 60
    case GL_T4F_C4F_N3F_V4F:
        et=true; ec=true; ct=GL_FLOAT; cs=4;
        en=true; vs=4; ts=4;
        texOff=0; colOff=4*F; normOff=8*F; vertOff=11*F; autoStride=15*F;
        break;

    default:
        DbgPrint("glInterleavedArrays: unknown format 0x%X\n", (unsigned)format);
        return;
    }

    GLsizei s = (stride == 0) ? autoStride : stride;

    if (et) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(ts, GL_FLOAT, s, p + texOff);
    }
    if (ec) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(cs, ct, s, p + colOff);
    }
    if (en) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, s, p + normOff);
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(vs, GL_FLOAT, s, p + vertOff);
}

// Xbox helpers
int   d3dIsResolutionHD()
{
    return ((XGetAVPack() == XC_AV_PACK_HDTV) &&
            (XGetVideoFlags() & (XC_VIDEO_FLAGS_HDTV_720p | XC_VIDEO_FLAGS_HDTV_480p))) ? TRUE : FALSE;
}
void  d3dSetMode(int w,int h,int,int,int vm) { gWidth=(DWORD)w; gHeight=(DWORD)h; gVideoMode=vm; }
void  d3dSetGammaRamp(const unsigned char* t)       { if (gRXGL) gRXGL->SetGammaRamp(t); }
void  d3dInitSetForce16BitTextures(int v)            { g_force16bitTextures=(v!=0); }
void  d3dHint_GenerateMipMaps(int v)                 { if (gRXGL) gRXGL->Hint_GenerateMipMaps(v); }
float d3dGetD3DDriverVersion()                       { return 0.81f; }
void  d3dEvictTextures()                             { if (gRXGL) gRXGL->EvictTextures(); }

#ifdef __cplusplus
} // extern "C"
#endif
