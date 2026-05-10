/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_texture.h  --  Texture subsystem declarations.
 *                     Declares TextureEntry (per-object state: IDirect3DTexture8,
 *                     filter/wrap params, shadow pixel buffer), TextureTable (the
 *                     glGenTextures/glDeleteTextures ID pool), TextureStageState,
 *                     and TextureState (per-stage env mode + bound texture).
 *                     Owns: glGenTextures, glDeleteTextures, glBindTexture,
 *                           glTexImage2D, glTexSubImage2D, glTexParameteri,
 *                           glTexEnvi, glTexEnvf, glGenerateMipmap.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_TEXTURE_H
#define RXGL_TEXTURE_H

#include "rxgl_internal.h"

// ---------------------------------------------------------------------------
// TextureEntry - per-texture object state
// ---------------------------------------------------------------------------

class TextureEntry
{
public:
    TextureEntry();
    void Release();

    GLuint              m_id;
    IDirect3DTexture8*  m_mipMap;
    D3DFORMAT           m_format;
    GLint               m_internalFormat;
    GLint               m_glTexParameter2DMinFilter;
    GLint               m_glTexParameter2DMagFilter;
    GLint               m_glTexParameter2DWrapS;
    GLint               m_glTexParameter2DWrapT;
    float               m_maxAnisotropy;
    bool                m_inUse;

    // Linear CPU-side shadow copy for correct glTexSubImage2D updates
    char*  m_shadowPixels;
    DWORD  m_shadowPitch;
    DWORD  m_shadowWidth;
    DWORD  m_shadowHeight;
    DWORD  m_shadowBpp;
};

// ---------------------------------------------------------------------------
// TextureTable - O(1) lookup: small array + hash overflow
// ---------------------------------------------------------------------------

#define TASIZE      2000
#define HASH_SLOTS  512

class TextureTable
{
public:
    TextureTable();
    ~TextureTable();

    void       GenTextures(GLsizei n, GLuint* ids);
    void       DeleteTextures(GLsizei n, const GLuint* ids);
    GLboolean  IsTexture(GLuint id);
    void       BindTexture(GLuint id);

    int           GetCurrentID()    const { return m_currentID; }
    TextureEntry* GetCurrentEntry()       { return m_currentTexture; }
    TextureEntry* GetEntry(GLuint id);

    IDirect3DTexture8* GetMipMap();
    IDirect3DTexture8* GetMipMap(GLuint id);
    D3DFORMAT          GetSurfaceFormat();
    GLint              GetInternalFormat();

    void SetTexture(IDirect3DTexture8* mipMap, D3DFORMAT fmt, GLint internalFormat);

private:
    struct HashBucket { TextureEntry entry; HashBucket* next; HashBucket(): next(NULL){} };
    static int HashSlot(GLuint id) { return (int)(id & (HASH_SLOTS - 1)); }
    TextureEntry* GetHashEntry(GLuint id);

    GLuint        m_currentID;
    TextureEntry* m_currentTexture;
    GLuint        m_nextGenID;
    TextureEntry  m_textureArray[TASIZE];
    HashBucket*   m_hash[HASH_SLOTS];
};

// ---------------------------------------------------------------------------
// TextureStageState / TextureState
// ---------------------------------------------------------------------------

class TextureStageState
{
public:
    TextureStageState();
    bool           GetDirty()  const { return m_dirty; }
    void           SetDirty(bool d)  { m_dirty = d; }
    void           DirtyTexture(GLuint id);
    GLuint         GetCurrentTexture() const { return m_currentTexture; }
    void           SetCurrentTexture(GLuint t);
    GLint          GetTextEnvMode() const { return m_glTextEnvMode; }
    void           SetTextEnvMode(GLint mode);
    bool           GetTexture2D() const { return m_glTexture2D; }
    void           SetTexture2D(bool v);
    void           SetEnvColor(const GLfloat* c);
    const GLfloat* GetEnvColor() const { return m_envColor; }

    // GL_COMBINE state
    void           SetCombineEnvi(GLenum pname, GLint param);
    void           SetCombineEnvf(GLenum pname, GLfloat param);

    GLint   m_combineRGB;       // combine function for RGB
    GLint   m_combineAlpha;     // combine function for Alpha
    GLint   m_src[3][2];        // [operand 0-2][0=RGB 1=Alpha]
    GLint   m_operand[3][2];    // [operand 0-2][0=RGB 1=Alpha]
    GLfloat m_rgbScale;
    GLfloat m_alphaScale;

private:
    GLuint  m_currentTexture;
    GLint   m_glTextEnvMode;
    bool    m_glTexture2D;
    bool    m_dirty;
    GLfloat m_envColor[4];
};

class TextureState
{
public:
    TextureState();
    void      SetMaxStages(int n);
    void      SetCurrentStage(int idx);
    int       GetCurrentStage() const { return m_currentStage; }
    void      SetClientStage(int idx) { m_clientStage = (idx >= 0 && idx < m_maxStages) ? idx : 0; }
    int       GetClientStage()  const { return m_clientStage; }
    int       GetMaxStages() const { return m_maxStages; }
    TextureStageState* Get() { return &m_stage[m_currentStage]; }  // public for combine state access
    bool      GetDirty()     const { return m_dirty; }
    void      DirtyTexture(GLuint id);
    void      SetMainBlend(bool v);
    void      ForceStage0Dirty() { m_stage[0].SetDirty(true); m_dirty = true; }
    GLuint    GetCurrentTexture();
    void      SetCurrentTexture(GLuint t);
    GLint     GetTextEnvMode();
    void      SetTextEnvMode(GLint mode);
    void      SetEnvColor(const GLfloat* c);
    bool      GetTexture2D();
    void      SetTexture2D(bool v);
    void      SetTextureStageState(IDirect3DDevice8* dev, TextureTable* textures);

private:
    bool              m_dirty;
    bool              m_mainBlend;
    int               m_maxStages;
    int               m_currentStage;
    int               m_clientStage;   // set by glClientActiveTextureARB
    TextureStageState m_stage[MAXSTAGES];
};

// ---------------------------------------------------------------------------
// RXGLTextureSystem - top-level texture subsystem owned by RXGL
// ---------------------------------------------------------------------------

class RXGLTextureSystem
{
public:
    typedef void (*ErrorFn)(GLenum err, void* ctx);

    RXGLTextureSystem() : m_dev(NULL), m_d3d(NULL),
        m_hintGenerateMipMaps(false), m_stickyAlloc(NULL), m_stickyAllocSize(0),
        m_errorFn(NULL), m_errorCtx(NULL) {}
    ~RXGLTextureSystem();

    void SetErrorCallback(ErrorFn fn, void* ctx) { m_errorFn = fn; m_errorCtx = ctx; }

    void Init(IDirect3DDevice8* dev, IDirect3D8* d3d,
              D3DSURFACE_DESC& backBufferDesc, bool force16bit);
    void SetHintGenerateMipMaps(bool v) { m_hintGenerateMipMaps = v; }

    // Dirty notification — called when caller wants texture re-applied
    void MarkDirty() { m_textureState.DirtyTexture(m_textures.GetCurrentID()); }

    // Apply texture stage state to D3D
    void Apply(IDirect3DDevice8* dev);

    // Accessors used by RXGL for binding / stage management
    TextureState& GetState() { return m_textureState; }
    TextureTable& GetTable() { return m_textures; }

    // GL entry points
    void BindTexture(GLenum target, GLuint texture, bool& renderStateDirty);
    void GenTextures(GLsizei n, GLuint* textures);
    void DeleteTextures(GLsizei n, const GLuint* textures, bool& renderStateDirty);
    GLboolean IsTexture(GLuint texture);
    void TexParameterf(GLenum target, GLenum pname, GLfloat param,
                       bool& renderStateDirty);
    void TexParameteri(GLenum target, GLenum pname, GLint param,
                       bool& renderStateDirty);
    void TexEnvf(GLenum target, GLenum pname, GLfloat param,
                 bool& renderStateDirty);
    void TexEnvi(GLenum target, GLenum pname, GLint param,
                 bool& renderStateDirty);
    void TexEnvfv(GLenum target, GLenum pname, const GLfloat* params,
                  bool& renderStateDirty);
    void TexImage2D(GLenum target, GLint level, GLint internalformat,
                    GLsizei width, GLsizei height, GLint border,
                    GLenum format, GLenum type, const GLvoid* pixels);
    void TexSubImage2D(GLenum target, GLint level, GLint xoff, GLint yoff,
                       GLsizei width, GLsizei height,
                       GLenum format, GLenum type, const GLvoid* pixels,
                       class OGLPrimitiveVertexBuffer& vb);

    // Framebuffer-to-texture copy (render-to-texture workaround)
    // glCopyTexSubImage2D: copy a rectangle from the framebuffer into
    //   an existing texture at (xoffset, yoffset).
    // glCopyTexImage2D: create/replace a texture level from the framebuffer.
    void CopyTexSubImage2D(GLenum target, GLint level,
                           GLint xoffset, GLint yoffset,
                           GLint x, GLint y, GLsizei width, GLsizei height,
                           IDirect3DDevice8* dev, D3DSURFACE_DESC& backBufDesc);
    void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat,
                        GLint x, GLint y, GLsizei width, GLsizei height,
                        IDirect3DDevice8* dev, IDirect3D8* d3d,
                        D3DSURFACE_DESC& backBufDesc);

    // Multitexture
    void ActiveTextureARB(GLenum texture);
    void ClientActiveTextureARB(GLenum texture);
    void SelectTextureSGIS(GLenum target);

private:
    void InterpretError(HRESULT hr);

    D3DFORMAT GLToDXPixelFormat(GLint internalformat, GLenum format);
    static int MipMapLevelCount(DWORD w, DWORD h);
    HRESULT ConvertToCompatiblePixels(GLint internalformat, GLsizei w, GLsizei h,
        GLenum format, GLenum type, D3DFORMAT dxFmt,
        const GLvoid* src, char** dst, DWORD* pitch);
    void TexSubImage2D_Imp(IDirect3DTexture8* tex, GLint level,
        GLint xoff, GLint yoff, GLsizei w, GLsizei h,
        GLenum fmt, GLenum type, const char* pixels, int srcPitch);

    char* StickyAlloc(DWORD size);

    IDirect3DDevice8* m_dev;
    IDirect3D8*       m_d3d;
    D3DSURFACE_DESC   m_backBufferDesc;
    bool              m_force16bit;
    bool              m_hintGenerateMipMaps;
    char*             m_stickyAlloc;
    DWORD             m_stickyAllocSize;
    ErrorFn           m_errorFn;
    void*             m_errorCtx;

    TextureTable      m_textures;
    TextureState      m_textureState;
};

#endif // RXGL_TEXTURE_H
