/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_texture.cpp  --  Texture subsystem implementation.
 *                       Converts GL pixel formats (RGBA, RGB, LUMINANCE, BGRA, etc.)
 *                       to D3D8 surface formats, uploads via Lock/Unlock, manages
 *                       mipmap chains, and maintains a shadow pixel buffer for
 *                       glGetTexImage readback. Handles the 16-bit texture downgrade
 *                       path (g_force16bitTextures) and anisotropic filtering.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_texture.h"
#include "rxgl_immediate.h"

// Global (defined in rxgl_core.cpp)
extern bool g_force16bitTextures;

// ---------------------------------------------------------------------------
// TextureEntry
// ---------------------------------------------------------------------------

TextureEntry::TextureEntry()
    : m_id(0), m_mipMap(NULL), m_format(D3DFMT_UNKNOWN), m_internalFormat(0),
      m_glTexParameter2DMinFilter(GL_LINEAR_MIPMAP_LINEAR),
      m_glTexParameter2DMagFilter(GL_LINEAR),
      m_glTexParameter2DWrapS(GL_CLAMP),
      m_glTexParameter2DWrapT(GL_CLAMP),
      m_maxAnisotropy(4.0f), m_inUse(false),
      m_shadowPixels(NULL), m_shadowPitch(0),
      m_shadowWidth(0), m_shadowHeight(0), m_shadowBpp(0)
{}

void TextureEntry::Release()
{
    RELEASENULL(m_mipMap);
    delete[] m_shadowPixels;
    m_shadowPixels = NULL;
    m_shadowPitch = m_shadowWidth = m_shadowHeight = m_shadowBpp = 0;
}

// ---------------------------------------------------------------------------
// TextureTable
// ---------------------------------------------------------------------------

TextureTable::TextureTable()
    : m_currentID(0), m_currentTexture(NULL), m_nextGenID(1)
{
    memset(m_hash, 0, sizeof(m_hash));
    BindTexture(0);
}

TextureTable::~TextureTable()
{
    for (DWORD i = 0; i < TASIZE; i++) m_textureArray[i].Release();
    for (int i = 0; i < HASH_SLOTS; i++)
    {
        HashBucket* b = m_hash[i];
        while (b) { b->entry.Release(); HashBucket* next = b->next; delete b; b = next; }
        m_hash[i] = NULL;
    }
}

void TextureTable::GenTextures(GLsizei n, GLuint* ids)
{
    for (GLsizei i = 0; i < n; i++)
    {
        ids[i] = m_nextGenID++;
        BindTexture(ids[i]);
        GetCurrentEntry()->m_inUse = true;
    }
}

void TextureTable::DeleteTextures(GLsizei n, const GLuint* ids)
{
    for (GLsizei i = 0; i < n; i++)
    {
        if (ids[i] == 0) continue;
        TextureEntry* e = GetEntry(ids[i]);
        if (e) { e->Release(); e->m_inUse = false; e->m_id = 0; }
        if (m_currentID == ids[i]) { m_currentID = 0; m_currentTexture = &m_textureArray[0]; }
    }
}

GLboolean TextureTable::IsTexture(GLuint id)
{
    if (id == 0) return GL_FALSE;
    TextureEntry* e = GetEntry(id);
    return (e && e->m_inUse) ? GL_TRUE : GL_FALSE;
}

void TextureTable::BindTexture(GLuint id)
{
    m_currentID = id;
    if (id < TASIZE)
    {
        m_currentTexture = &m_textureArray[id];
        if (m_currentTexture->m_id == id) return;
        m_currentTexture->m_id = id;
        m_currentTexture->m_mipMap = NULL;
    }
    else
    {
        TextureEntry* existing = GetHashEntry(id);
        if (existing) { m_currentTexture = existing; return; }
        int slot = HashSlot(id);
        HashBucket* b = new HashBucket();
        b->entry.m_id = id; b->entry.m_mipMap = NULL;
        b->next = m_hash[slot]; m_hash[slot] = b;
        m_currentTexture = &b->entry;
    }
}

TextureEntry* TextureTable::GetEntry(GLuint id)
{
    if (m_currentID == id && m_currentTexture) return m_currentTexture;
    if (id < TASIZE) return &m_textureArray[id];
    return GetHashEntry(id);
}

IDirect3DTexture8* TextureTable::GetMipMap()
{ return m_currentTexture ? m_currentTexture->m_mipMap : NULL; }

IDirect3DTexture8* TextureTable::GetMipMap(GLuint id)
{ TextureEntry* e = GetEntry(id); return e ? e->m_mipMap : NULL; }

D3DFORMAT TextureTable::GetSurfaceFormat()
{ return m_currentTexture ? m_currentTexture->m_format : D3DFMT_UNKNOWN; }

GLint TextureTable::GetInternalFormat()
{ return m_currentTexture ? m_currentTexture->m_internalFormat : 0; }

void TextureTable::SetTexture(IDirect3DTexture8* mipMap, D3DFORMAT fmt, GLint internalFormat)
{
    if (!m_currentTexture) BindTexture(0);
    RELEASENULL(m_currentTexture->m_mipMap);
    m_currentTexture->m_mipMap         = mipMap;
    m_currentTexture->m_format         = fmt;
    m_currentTexture->m_internalFormat = internalFormat;
}

TextureEntry* TextureTable::GetHashEntry(GLuint id)
{
    int slot = HashSlot(id);
    for (HashBucket* b = m_hash[slot]; b; b = b->next)
        if (b->entry.m_id == id) return &b->entry;
    return NULL;
}

// ---------------------------------------------------------------------------
// TextureStageState
// ---------------------------------------------------------------------------

TextureStageState::TextureStageState()
    : m_currentTexture(0), m_glTextEnvMode(GL_MODULATE),
      m_glTexture2D(false), m_dirty(true),
      m_combineRGB(GL_MODULATE), m_combineAlpha(GL_MODULATE),
      m_rgbScale(1.0f), m_alphaScale(1.0f)
{
    m_envColor[0] = m_envColor[1] = m_envColor[2] = m_envColor[3] = 0.0f;
    // Default sources: src0=TEXTURE, src1=PREVIOUS, src2=CONSTANT
    m_src[0][0] = m_src[0][1] = GL_TEXTURE;
    m_src[1][0] = m_src[1][1] = GL_PREVIOUS;
    m_src[2][0] = m_src[2][1] = GL_CONSTANT;
    // Default operands: RGB=SRC_COLOR, Alpha=SRC_ALPHA
    m_operand[0][0] = m_operand[1][0] = m_operand[2][0] = GL_SRC_COLOR;
    m_operand[0][1] = m_operand[1][1] = m_operand[2][1] = GL_SRC_ALPHA;
}

void TextureStageState::SetCombineEnvi(GLenum pname, GLint param)
{
    m_dirty = true;
    switch (pname)
    {
    case GL_COMBINE_RGB:    m_combineRGB   = param; break;
    case GL_COMBINE_ALPHA:  m_combineAlpha = param; break;
    case GL_SOURCE0_RGB:    m_src[0][0]    = param; break;
    case GL_SOURCE1_RGB:    m_src[1][0]    = param; break;
    case GL_SOURCE2_RGB:    m_src[2][0]    = param; break;
    case GL_SOURCE0_ALPHA:  m_src[0][1]    = param; break;
    case GL_SOURCE1_ALPHA:  m_src[1][1]    = param; break;
    case GL_SOURCE2_ALPHA:  m_src[2][1]    = param; break;
    case GL_OPERAND0_RGB:   m_operand[0][0]= param; break;
    case GL_OPERAND1_RGB:   m_operand[1][0]= param; break;
    case GL_OPERAND2_RGB:   m_operand[2][0]= param; break;
    case GL_OPERAND0_ALPHA: m_operand[0][1]= param; break;
    case GL_OPERAND1_ALPHA: m_operand[1][1]= param; break;
    case GL_OPERAND2_ALPHA: m_operand[2][1]= param; break;
    case GL_RGB_SCALE:      m_rgbScale     = (GLfloat)param; break;
    case GL_ALPHA_SCALE:    m_alphaScale   = (GLfloat)param; break;
    }
}

void TextureStageState::SetCombineEnvf(GLenum pname, GLfloat param)
{
    if (pname == GL_RGB_SCALE)        { m_dirty=true; m_rgbScale   = param; }
    else if (pname == GL_ALPHA_SCALE) { m_dirty=true; m_alphaScale = param; }
    else SetCombineEnvi(pname, (GLint)param);
}

void TextureStageState::DirtyTexture(GLuint id)
{ if (id == m_currentTexture) m_dirty = true; }

void TextureStageState::SetCurrentTexture(GLuint t)
{ m_dirty = true; m_currentTexture = t; }

void TextureStageState::SetTextEnvMode(GLint mode)
{ m_dirty = true; m_glTextEnvMode = mode; }

void TextureStageState::SetTexture2D(bool v)
{ m_dirty = true; m_glTexture2D = v; }

void TextureStageState::SetEnvColor(const GLfloat* c)
{
    m_dirty = true;
    m_envColor[0]=c[0]; m_envColor[1]=c[1]; m_envColor[2]=c[2]; m_envColor[3]=c[3];
}

// ---------------------------------------------------------------------------
// TextureState
// ---------------------------------------------------------------------------

TextureState::TextureState()
    : m_currentStage(0), m_clientStage(0), m_maxStages(1), m_dirty(false), m_mainBlend(false)
{ memset(m_stage, 0, sizeof(m_stage)); }

void TextureState::SetMaxStages(int n)
{
    m_maxStages = (n > MAXSTAGES) ? MAXSTAGES : n;
    for (int i = 0; i < m_maxStages; i++) m_stage[i].SetDirty(true);
    m_dirty = true;
}

void TextureState::SetCurrentStage(int idx)
{ m_currentStage = (idx < m_maxStages) ? idx : m_maxStages - 1; }

void TextureState::DirtyTexture(GLuint id)
{ for (int i = 0; i < m_maxStages; i++) m_stage[i].DirtyTexture(id); m_dirty = true; }

void TextureState::SetMainBlend(bool v)
{ m_mainBlend = v; m_stage[0].SetDirty(true); m_dirty = true; }

GLuint TextureState::GetCurrentTexture() { return Get()->GetCurrentTexture(); }
void   TextureState::SetCurrentTexture(GLuint t) { m_dirty = true; Get()->SetCurrentTexture(t); }
GLint  TextureState::GetTextEnvMode()    { return Get()->GetTextEnvMode(); }
void   TextureState::SetTextEnvMode(GLint mode) { m_dirty = true; Get()->SetTextEnvMode(mode); }
void   TextureState::SetEnvColor(const GLfloat* c) { m_dirty = true; Get()->SetEnvColor(c); }
bool   TextureState::GetTexture2D()     { return Get()->GetTexture2D(); }
void   TextureState::SetTexture2D(bool v) { m_dirty = true; Get()->SetTexture2D(v); }

// ---------------------------------------------------------------------------
// GL_COMBINE helpers -- static so no lambda needed (old MSVC compatibility)
// ---------------------------------------------------------------------------

static DWORD CombineSrcToD3DTA(int src, int operand, bool isAlpha, int stage)
{
    DWORD base;
    switch (src)
    {
    case GL_TEXTURE:       base = D3DTA_TEXTURE;  break;
    case GL_PREVIOUS:      base = (stage==0) ? D3DTA_DIFFUSE : D3DTA_CURRENT; break;
    case GL_PRIMARY_COLOR: base = D3DTA_DIFFUSE;  break;
    case GL_CONSTANT:      base = D3DTA_TFACTOR;  break;
    default:               base = D3DTA_TEXTURE;  break;
    }
    DWORD result = base;
    if (operand == GL_ONE_MINUS_SRC_COLOR || operand == GL_ONE_MINUS_SRC_ALPHA)
        result |= D3DTA_COMPLEMENT;
    if (!isAlpha && (operand == GL_SRC_ALPHA || operand == GL_ONE_MINUS_SRC_ALPHA))
        result |= D3DTA_ALPHAREPLICATE;
    return result;
}

static bool RgbOperandUsed(GLint combineRGB, int opIdx)
{
    switch (combineRGB)
    {
    case GL_REPLACE: return opIdx == 0;
    case GL_MODULATE:
    case GL_ADD:
    case GL_ADD_SIGNED:
    case GL_SUBTRACT: return opIdx <= 1;
    case GL_INTERPOLATE: return opIdx <= 2;
    case GL_DOT3_RGB:
    case GL_DOT3_RGBA: return opIdx <= 1;
    default: return opIdx <= 1;
    }
}

static bool AlphaOperandUsed(GLint combineAlpha, int opIdx)
{
    switch (combineAlpha)
    {
    case GL_REPLACE: return opIdx == 0;
    case GL_MODULATE:
    case GL_ADD:
    case GL_ADD_SIGNED:
    case GL_SUBTRACT: return opIdx <= 1;
    case GL_INTERPOLATE: return opIdx <= 2;
    case GL_DOT3_RGBA: return opIdx <= 1;
    default: return opIdx <= 1;
    }
}

static bool CombineStageUsesConstant(const TextureStageState& s)
{
    for (int op = 0; op < 3; op++)
    {
        if (RgbOperandUsed(s.m_combineRGB, op) && s.m_src[op][0] == GL_CONSTANT)
            return true;
        if (AlphaOperandUsed(s.m_combineAlpha, op) && s.m_src[op][1] == GL_CONSTANT)
            return true;
    }
    return false;
}

static void ApplyCombineStage(IDirect3DDevice8* dev, int i, TextureStageState& s)
{
    // D3D8 has one TEXTUREFACTOR for all stages; only write it when this COMBINE uses
    // GL_CONSTANT on an operand that mode reads (defaults leave SRC2 as CONSTANT for MODULATE).
    // Otherwise stage 1 overwrote DOT3 light encoding on unit 0 (black spheres, speckles).
    if (CombineStageUsesConstant(s))
    {
        const GLfloat* ec = s.GetEnvColor();
        dev->SetRenderState(D3DRS_TEXTUREFACTOR,
            D3DCOLOR_COLORVALUE(ec[0], ec[1], ec[2], ec[3]));
    }

    // RGB args
    dev->SetTextureStageState(i, D3DTSS_COLORARG1, CombineSrcToD3DTA(s.m_src[0][0], s.m_operand[0][0], false, i));
    dev->SetTextureStageState(i, D3DTSS_COLORARG2, CombineSrcToD3DTA(s.m_src[1][0], s.m_operand[1][0], false, i));
    dev->SetTextureStageState(i, D3DTSS_COLORARG0, CombineSrcToD3DTA(s.m_src[2][0], s.m_operand[2][0], false, i));

    DWORD colorOp = D3DTOP_MODULATE;
    switch (s.m_combineRGB)
    {
    case GL_REPLACE:     colorOp = D3DTOP_SELECTARG1; break;
    case GL_MODULATE:    colorOp = (s.m_rgbScale > 3.0f) ? D3DTOP_MODULATE4X :
                                   (s.m_rgbScale > 1.5f) ? D3DTOP_MODULATE2X : D3DTOP_MODULATE; break;
    case GL_ADD:         colorOp = D3DTOP_ADD; break;
    case GL_ADD_SIGNED:  colorOp = (s.m_rgbScale > 1.5f) ? D3DTOP_ADDSIGNED2X : D3DTOP_ADDSIGNED; break;
    case GL_INTERPOLATE: colorOp = D3DTOP_LERP; break;
    case GL_SUBTRACT:    colorOp = D3DTOP_SUBTRACT; break;
    case GL_DOT3_RGB:
    case GL_DOT3_RGBA:   colorOp = D3DTOP_DOTPRODUCT3; break;
    }
    dev->SetTextureStageState(i, D3DTSS_COLOROP, colorOp);

    // Alpha args (strip ALPHAREPLICATE -- not valid for alpha args)
    dev->SetTextureStageState(i, D3DTSS_ALPHAARG1, CombineSrcToD3DTA(s.m_src[0][1], s.m_operand[0][1], true, i) & ~D3DTA_ALPHAREPLICATE);
    dev->SetTextureStageState(i, D3DTSS_ALPHAARG2, CombineSrcToD3DTA(s.m_src[1][1], s.m_operand[1][1], true, i) & ~D3DTA_ALPHAREPLICATE);
    dev->SetTextureStageState(i, D3DTSS_ALPHAARG0, CombineSrcToD3DTA(s.m_src[2][1], s.m_operand[2][1], true, i) & ~D3DTA_ALPHAREPLICATE);

    DWORD alphaOp = D3DTOP_MODULATE;
    switch (s.m_combineAlpha)
    {
    case GL_REPLACE:     alphaOp = D3DTOP_SELECTARG1; break;
    case GL_MODULATE:    alphaOp = (s.m_alphaScale > 3.0f) ? D3DTOP_MODULATE4X :
                                   (s.m_alphaScale > 1.5f) ? D3DTOP_MODULATE2X : D3DTOP_MODULATE; break;
    case GL_ADD:         alphaOp = D3DTOP_ADD; break;
    case GL_ADD_SIGNED:  alphaOp = (s.m_alphaScale > 1.5f) ? D3DTOP_ADDSIGNED2X : D3DTOP_ADDSIGNED; break;
    case GL_INTERPOLATE: alphaOp = D3DTOP_LERP; break;
    case GL_SUBTRACT:    alphaOp = D3DTOP_SUBTRACT; break;
    case GL_DOT3_RGBA:   alphaOp = D3DTOP_DOTPRODUCT3; break;
    default:             alphaOp = D3DTOP_SELECTARG1; break;
    }
    dev->SetTextureStageState(i, D3DTSS_ALPHAOP, alphaOp);
}

void TextureState::SetTextureStageState(IDirect3DDevice8* dev, TextureTable* textures)
{
    if (!m_dirty) return;
    m_dirty = false;

    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        for (int i = 0; i < m_maxStages; i++)
            dev->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
    }

    for (int i = 0; i < m_maxStages; i++)
    {
        if (!m_stage[i].GetDirty()) continue;
        m_stage[i].SetDirty(false);

        if (!m_stage[i].GetTexture2D())
        {
            dev->SetTexture(i, NULL);
            if (i == 0)
            {
                // Stage 0 with no texture: pass vertex diffuse colour and alpha through.
                // Without this, pixel alpha defaults to 1.0 on Xbox NV2A, which breaks
                // alpha testing against vertex alpha.
                dev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
                dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
                dev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
                dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            }
            else
            {
                dev->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
                dev->SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            }
            continue;
        }

        int envMode = (int)m_stage[i].GetTextEnvMode();

        TextureEntry* entry = textures->GetEntry(m_stage[i].GetCurrentTexture());
        if (entry)
        {
            int minF = entry->m_glTexParameter2DMinFilter;
            DWORD dxMin = GLToDXMinFilter(minF);
            DWORD dxMip = GLToDXMipFilter(minF);
            DWORD dxMag = GLToDXMagFilter(entry->m_glTexParameter2DMagFilter);

            if (entry->m_maxAnisotropy != 1.0f)
            {
                if (dxMag == D3DTEXF_LINEAR) dxMag = D3DTEXF_ANISOTROPIC;
                if (dxMin == D3DTEXF_LINEAR) dxMin = D3DTEXF_ANISOTROPIC;
                dev->SetTextureStageState(i, D3DTSS_MAXANISOTROPY, (DWORD)entry->m_maxAnisotropy);
            }

            dev->SetTextureStageState(i, D3DTSS_MINFILTER, dxMin);
            dev->SetTextureStageState(i, D3DTSS_MIPFILTER, dxMip);
            dev->SetTextureStageState(i, D3DTSS_MAGFILTER, dxMag);
            dev->SetTextureStageState(i, D3DTSS_ADDRESSU, GLToDXWrap(entry->m_glTexParameter2DWrapS));
            dev->SetTextureStageState(i, D3DTSS_ADDRESSV, GLToDXWrap(entry->m_glTexParameter2DWrapT));
            if (entry->m_mipMap) dev->SetTexture(i, entry->m_mipMap);
            else LocalDebugBreak();
        }

        DWORD incoming = (i == 0) ? D3DTA_DIFFUSE : D3DTA_CURRENT;

        if (envMode == GL_COMBINE)
        {
            TextureStageState& s = m_stage[i];
            ApplyCombineStage(dev, i, s);
        }
        else if (envMode == GL_BLEND)
        {
            const GLfloat* c = m_stage[i].GetEnvColor();
            DWORD tfactor = D3DCOLOR_COLORVALUE(c[0], c[1], c[2], c[3]);
            dev->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);
            dev->SetTextureStageState(i, D3DTSS_COLORARG0, D3DTA_TEXTURE);
            dev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TFACTOR);
            dev->SetTextureStageState(i, D3DTSS_COLORARG2, incoming);
            dev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_LERP);
            dev->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            dev->SetTextureStageState(i, D3DTSS_ALPHAARG2, incoming);
            dev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
        }
        else if (envMode == GL_DECAL)
        {
            dev->SetTextureStageState(i, D3DTSS_COLORARG0, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);
            dev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            dev->SetTextureStageState(i, D3DTSS_COLORARG2, incoming);
            dev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_LERP);
            dev->SetTextureStageState(i, D3DTSS_ALPHAARG1, incoming);
            dev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
        }
        else
        {
            DWORD colorOp = GLToDXTextEnvMode(envMode);
            dev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            dev->SetTextureStageState(i, D3DTSS_COLORARG2, incoming);
            dev->SetTextureStageState(i, D3DTSS_COLOROP,   colorOp);
            DWORD alphaOp = colorOp;
            if (i == 0 && m_mainBlend) alphaOp = D3DTOP_MODULATE;
            dev->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            dev->SetTextureStageState(i, D3DTSS_ALPHAARG2, incoming);
            dev->SetTextureStageState(i, D3DTSS_ALPHAOP,   alphaOp);
        }
    }
}

// ---------------------------------------------------------------------------
// RXGLTextureSystem
// ---------------------------------------------------------------------------

RXGLTextureSystem::~RXGLTextureSystem()
{
    delete[] m_stickyAlloc;
}

void RXGLTextureSystem::Init(IDirect3DDevice8* dev, IDirect3D8* d3d,
                                D3DSURFACE_DESC& backBufferDesc, bool force16bit)
{
    m_dev           = dev;
    m_d3d           = d3d;
    m_backBufferDesc = backBufferDesc;
    m_force16bit    = force16bit;
}

void RXGLTextureSystem::Apply(IDirect3DDevice8* dev)
{
    m_textureState.SetTextureStageState(dev, &m_textures);
}

void RXGLTextureSystem::InterpretError(HRESULT hr)
{
    char buf[128];
    D3DXGetErrorString(hr, buf, sizeof(buf));
    OutputDebugString(buf);

    // Map D3D error to GL error and fire callback
    if (m_errorFn)
    {
        GLenum glErr = (hr == E_OUTOFMEMORY || hr == D3DERR_OUTOFVIDEOMEMORY)
            ? GL_OUT_OF_MEMORY
            : GL_INVALID_OPERATION;
        m_errorFn(glErr, m_errorCtx);
    }

    LocalDebugBreak();
}

char* RXGLTextureSystem::StickyAlloc(DWORD size)
{
    if (m_stickyAllocSize < size)
    {
        delete[] m_stickyAlloc;
        m_stickyAlloc     = new char[size];
        m_stickyAllocSize = size;
    }
    return m_stickyAlloc;
}

D3DFORMAT RXGLTextureSystem::GLToDXPixelFormat(GLint internalformat, GLenum format)
{
    if (m_force16bit)
    {
        switch (format)
        {
        case GL_RGBA:        return (internalformat == 3) ? D3DFMT_R5G6B5 : D3DFMT_A4R4G4B4;
        case GL_RGB:         return D3DFMT_R5G6B5;
        case GL_COLOR_INDEX: return D3DFMT_L8;
        case GL_LUMINANCE:   return D3DFMT_L8;
        case GL_ALPHA:       return D3DFMT_A8;
        case GL_INTENSITY:   return D3DFMT_L8;
        case GL_RGBA4:       return D3DFMT_A4R4G4B4;
        default: LocalDebugBreak(); return D3DFMT_A4R4G4B4;
        }
    }
    else
    {
        switch (format)
        {
        case GL_RGBA:        return (internalformat == 3) ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;
        case GL_RGB:         return D3DFMT_X8R8G8B8;
        case GL_COLOR_INDEX: return D3DFMT_L8;
        case GL_LUMINANCE:   return D3DFMT_L8;
        case GL_ALPHA:       return D3DFMT_A8;
        case GL_INTENSITY:   return D3DFMT_L8;
        case GL_RGBA4:       return D3DFMT_A4R4G4B4;
        default: LocalDebugBreak(); return D3DFMT_A8R8G8B8;
        }
    }
}

int RXGLTextureSystem::MipMapLevelCount(DWORD w, DWORD h)
{
    int n = 1; DWORD d = (w < h) ? w : h;
    while (d > 1) { d >>= 1; n++; }
    return n;
}

HRESULT RXGLTextureSystem::ConvertToCompatiblePixels(
    GLint internalformat, GLsizei w, GLsizei h,
    GLenum /*format*/, GLenum type, D3DFORMAT dxFmt,
    const GLvoid* src, char** dst, DWORD* pitch)
{
    if (type != GL_UNSIGNED_BYTE) return E_FAIL;
    const unsigned char* sp = (const unsigned char*)src;

    switch (dxFmt)
    {
    case D3DFMT_L8: case D3DFMT_A8: case D3DFMT_P8:
    {
        char* p = StickyAlloc(w * h);
        memcpy(p, src, w * h); *dst = p; *pitch = w;
    } break;

    case D3DFMT_A4R4G4B4:
    {
        unsigned short* p = (unsigned short*)StickyAlloc(2 * w * h);
        *dst = (char*)p; *pitch = 2 * w;
        if (internalformat == 1)
        {
            for (int i = 0; i < w * h; i++)
            { unsigned s = sp[i] >> 4; p[i] = (unsigned short)(s|(s<<4)|(s<<8)|(s<<12)); }
        }
        else
        {
            for (int i = 0; i < w * h; i++)
            {
                const unsigned char* px = sp + i * 4;
                unsigned short v = (0xf & (px[2] >> 4));
                v |= (0xf & (px[1] >> 4)) << 4;
                v |= (0xf & (px[0] >> 4)) << 8;
                v |= (internalformat == 4) ? (0xf & (px[3] >> 4)) << 12 : 0xf000;
                p[i] = v;
            }
        }
    } break;

    case D3DFMT_R5G6B5:
    {
        unsigned short* p = (unsigned short*)StickyAlloc(2 * w * h);
        *dst = (char*)p; *pitch = 2 * w;
        if (internalformat == 1)
        {
            for (int i = 0; i < w * h; i++)
            {
                unsigned y = sp[i];
                p[i] = (unsigned short)((y>>3) | (((y>>2)&0x3f)<<5) | ((y>>3)<<11));
            }
        }
        else
        {
            for (int i = 0; i < w * h; i++)
            {
                const unsigned char* px = sp + i * 4;
                p[i] = (unsigned short)((px[2]>>3) | (((px[1]>>2)&0x3f)<<5) | ((px[0]>>3)<<11));
            }
        }
    } break;

    case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8:
    {
        unsigned char* p = (unsigned char*)StickyAlloc(4 * w * h);
        *dst = (char*)p; *pitch = 4 * w;
        if (internalformat == 1)
        {
            for (int i = 0; i < w * h; i++)
            {
                unsigned char y = sp[i];
                p[i*4+0]=y; p[i*4+1]=y; p[i*4+2]=y;
                p[i*4+3]=(dxFmt==D3DFMT_A8R8G8B8)?y:0xff;
            }
        }
        else
        {
            for (int i = 0; i < w * h; i++)
            {
                p[i*4+0]=sp[i*4+2]; p[i*4+1]=sp[i*4+1]; p[i*4+2]=sp[i*4+0];
                p[i*4+3]=(internalformat==4||internalformat==0x1908)?sp[i*4+3]:0xff; // GL_RGBA=0x1908
            }
        }
    } break;

    default: LocalDebugBreak(); return E_FAIL;
    }
    return S_OK;
}

void RXGLTextureSystem::TexSubImage2D_Imp(IDirect3DTexture8* tex, GLint level,
    GLint xoff, GLint yoff, GLsizei w, GLsizei h,
    GLenum /*fmt*/, GLenum /*type*/, const char* pixels, int srcPitch)
{
    if (!tex || !pixels) return;

    D3DSURFACE_DESC desc;
    tex->GetLevelDesc(level, &desc);

    int bpp;
    switch (desc.Format)
    {
    case D3DFMT_L8: case D3DFMT_A8: case D3DFMT_P8: bpp = 1; break;
    case D3DFMT_R5G6B5: case D3DFMT_A4R4G4B4: case D3DFMT_A8L8: bpp = 2; break;
    default: bpp = 4; break;
    }

    if (xoff < 0 || yoff < 0 || w <= 0 || h <= 0) return;
    if (xoff + w > (GLsizei)desc.Width) return;
    if (yoff + h > (GLsizei)desc.Height) return;

    TextureEntry* entry = m_textures.GetCurrentEntry();
    if (!entry) return;

    DWORD fullPitch = desc.Width * bpp;
    DWORD fullSize  = fullPitch * desc.Height;

    if (!entry->m_shadowPixels ||
        entry->m_shadowWidth  != desc.Width ||
        entry->m_shadowHeight != desc.Height ||
        entry->m_shadowBpp    != (DWORD)bpp)
    {
        delete[] entry->m_shadowPixels;
        entry->m_shadowPixels = new char[fullSize];
        if (!entry->m_shadowPixels) return;
        entry->m_shadowPitch  = fullPitch;
        entry->m_shadowWidth  = desc.Width;
        entry->m_shadowHeight = desc.Height;
        entry->m_shadowBpp    = bpp;

        D3DLOCKED_RECT lr;
        if (SUCCEEDED(tex->LockRect(level, &lr, NULL, D3DLOCK_READONLY)))
        {
            XGUnswizzleRect(lr.pBits, desc.Width, desc.Height, NULL,
                entry->m_shadowPixels, entry->m_shadowPitch, NULL, bpp);
            tex->UnlockRect(level);
        }
    }

    for (int y = 0; y < h; y++)
    {
        const char* srcRow = pixels + y * srcPitch;
        char* dstRow = entry->m_shadowPixels + ((yoff + y) * entry->m_shadowPitch) + (xoff * bpp);
        memcpy(dstRow, srcRow, w * bpp);
    }

    D3DLOCKED_RECT lr;
    if (FAILED(tex->LockRect(level, &lr, NULL, 0))) return;
    XGSwizzleRect(entry->m_shadowPixels, entry->m_shadowPitch, NULL,
        lr.pBits, desc.Width, desc.Height, NULL, bpp);
    tex->UnlockRect(level);

    m_textureState.DirtyTexture(m_textures.GetCurrentID());
}

// ---------------------------------------------------------------------------
// GL entry points
// ---------------------------------------------------------------------------

void RXGLTextureSystem::BindTexture(GLenum target, GLuint texture, bool& rsDirty)
{
    if (target != GL_TEXTURE_2D) { LocalDebugBreak(); return; }
    if (m_textureState.GetCurrentTexture() != texture)
    {
        rsDirty = true;
        m_textureState.SetCurrentTexture(texture);
        m_textures.BindTexture(texture);
    }
}

void RXGLTextureSystem::GenTextures(GLsizei n, GLuint* textures)
{ m_textures.GenTextures(n, textures); }

void RXGLTextureSystem::DeleteTextures(GLsizei n, const GLuint* textures, bool& rsDirty)
{
    rsDirty = true;
    for (GLsizei i = 0; i < n; i++) m_textureState.DirtyTexture(textures[i]);
    m_textures.DeleteTextures(n, textures);
}

GLboolean RXGLTextureSystem::IsTexture(GLuint texture)
{ return m_textures.IsTexture(texture); }

void RXGLTextureSystem::TexParameterf(GLenum target, GLenum pname, GLfloat param, bool& rsDirty)
{ TexParameteri(target, pname, (GLint)param, rsDirty); }

void RXGLTextureSystem::TexParameteri(GLenum target, GLenum pname, GLint param, bool& rsDirty)
{
    if (target != GL_TEXTURE_2D) return;
    rsDirty = true;
    TextureEntry* e = m_textures.GetCurrentEntry();
    m_textureState.DirtyTexture(m_textures.GetCurrentID());
    switch (pname)
    {
    case GL_TEXTURE_MIN_FILTER:     e->m_glTexParameter2DMinFilter = param; break;
    case GL_TEXTURE_MAG_FILTER:     e->m_glTexParameter2DMagFilter = param; break;
    case GL_TEXTURE_WRAP_S:         e->m_glTexParameter2DWrapS     = param; break;
    case GL_TEXTURE_WRAP_T:         e->m_glTexParameter2DWrapT     = param; break;
    case D3D_TEXTURE_MAXANISOTROPY: e->m_maxAnisotropy = (float)param; break;
    default: DbgPrint("glTexParameteri: unhandled pname 0x%x\n", pname); break;
    }
}

void RXGLTextureSystem::TexEnvf(GLenum target, GLenum pname, GLfloat param, bool& rsDirty)
{
    if (target != GL_TEXTURE_ENV) { LocalDebugBreak(); return; }
    rsDirty = true;
    if (pname == GL_TEXTURE_ENV_MODE) m_textureState.SetTextEnvMode((GLint)param);
    else m_textureState.Get()->SetCombineEnvf(pname, param);
}

void RXGLTextureSystem::TexEnvi(GLenum target, GLenum pname, GLint param, bool& rsDirty)
{
    if (target != GL_TEXTURE_ENV) { LocalDebugBreak(); return; }
    rsDirty = true;
    if (pname == GL_TEXTURE_ENV_MODE) m_textureState.SetTextEnvMode(param);
    else m_textureState.Get()->SetCombineEnvi(pname, param);
}

void RXGLTextureSystem::TexEnvfv(GLenum target, GLenum pname, const GLfloat* params, bool& rsDirty)
{
    if (target != GL_TEXTURE_ENV || !params) { LocalDebugBreak(); return; }
    rsDirty = true;
    switch (pname)
    {
    case GL_TEXTURE_ENV_COLOR: m_textureState.SetEnvColor(params); break;
    case GL_TEXTURE_ENV_MODE:  m_textureState.SetTextEnvMode((GLint)params[0]); break;
    default: m_textureState.Get()->SetCombineEnvf(pname, params[0]); break;
    }
}

void RXGLTextureSystem::TexImage2D(GLenum target, GLint level, GLint internalformat,
    GLsizei width, GLsizei height, GLint /*border*/,
    GLenum format, GLenum type, const GLvoid* pixels)
{
    if (target != GL_TEXTURE_2D || type != GL_UNSIGNED_BYTE) { InterpretError(E_FAIL); return; }
    if (width <= 0 || height <= 0) { if (m_errorFn) m_errorFn(GL_INVALID_VALUE, m_errorCtx); return; }

    D3DFORMAT srcFmt  = GLToDXPixelFormat(internalformat, format);
    D3DFORMAT destFmt = srcFmt;

    if (FAILED(m_d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
               m_backBufferDesc.Format, 0, D3DRTYPE_TEXTURE, destFmt)))
        destFmt = m_force16bit ? D3DFMT_A4R4G4B4 : D3DFMT_A8R8G8B8;

    IDirect3DTexture8* tex = m_textures.GetMipMap();
    if (tex)
    {
        if (level == 0) { m_textures.SetTexture(NULL, D3DFMT_UNKNOWN, 0); tex = NULL; }
        else if (level >= (int)tex->GetLevelCount()) return;
    }

    if (!tex)
    {
        int levels = m_hintGenerateMipMaps ? MipMapLevelCount(width, height) : 1;
        HRESULT hr = m_dev->CreateTexture(width, height, levels, 0,
                                          destFmt, D3DPOOL_MANAGED, &tex);
        if (FAILED(hr)) { InterpretError(hr); return; }
        m_textures.SetTexture(tex, destFmt, internalformat);
    }

    if (!pixels) return;

    char* converted = NULL; DWORD pitch = 0;
    if (FAILED(ConvertToCompatiblePixels(internalformat, width, height,
               format, type, destFmt, pixels, &converted, &pitch)))
    { InterpretError(E_FAIL); return; }

    TexSubImage2D_Imp(tex, level, 0, 0, width, height, format, type, converted, pitch);
}

void RXGLTextureSystem::TexSubImage2D(GLenum target, GLint level,
    GLint xoff, GLint yoff, GLsizei width, GLsizei height,
    GLenum format, GLenum type, const GLvoid* pixels,
    OGLPrimitiveVertexBuffer& vb)
{
    if (target != GL_TEXTURE_2D) { LocalDebugBreak(); return; }
    if (width <= 0 || height <= 0 || !pixels) return;
    IDirect3DTexture8* tex = m_textures.GetMipMap();
    if (!tex) return;
    vb.End(); // flush pending geometry before modifying texture
    char* converted = NULL; DWORD pitch = 0;
    if (FAILED(ConvertToCompatiblePixels(m_textures.GetInternalFormat(), width, height,
               format, type, m_textures.GetSurfaceFormat(), pixels, &converted, &pitch)))
    { LocalDebugBreak(); return; }
    TexSubImage2D_Imp(tex, level, xoff, yoff, width, height, format, type, converted, pitch);
}

void RXGLTextureSystem::ActiveTextureARB(GLenum texture)
{
    int stage = (int)(texture - GL_TEXTURE0_ARB);
    m_textureState.SetCurrentStage(stage);
    m_textures.BindTexture(m_textureState.GetCurrentTexture());
}

void RXGLTextureSystem::ClientActiveTextureARB(GLenum texture)
{
    int stage = (int)(texture - GL_TEXTURE0_ARB);
    m_textureState.SetClientStage(stage);
}

void RXGLTextureSystem::SelectTextureSGIS(GLenum target)
{
    int stage = (int)(target - TEXTURE0_SGIS);
    m_textureState.SetCurrentStage(stage);
    m_textures.BindTexture(m_textureState.GetCurrentTexture());
}


// ---------------------------------------------------------------------------
// CopyTexSubImage2D
// Xbox backbuffer is D3DFMT_LIN_X8R8G8B8 (linear, not swizzled).
// We lock it directly ? no staging surface needed, which avoids the
// CreateImageSurface + D3DXLoadSurface overhead that was tanking FPS.
//
// Steps:
//   1. EndScene so the GPU has finished writing to the backbuffer
//   2. GetBackBuffer + LockRect directly (linear format, lock is cheap)
//   3. Read the requested rectangle with Y-flip (GL y=0 = bottom)
//   4. Convert to texture format, patch shadow copy
//   5. XGSwizzleRect -> write swizzled data back into the texture
//   6. Resume rendering (BeginScene)
// ---------------------------------------------------------------------------
void RXGLTextureSystem::CopyTexSubImage2D(
    GLenum target, GLint level,
    GLint xoffset, GLint yoffset,
    GLint x, GLint y, GLsizei width, GLsizei height,
    IDirect3DDevice8* dev, D3DSURFACE_DESC& backBufDesc)
{
    if (target != GL_TEXTURE_2D) { LocalDebugBreak(); return; }
    if (width <= 0 || height <= 0) return;

    IDirect3DTexture8* tex = m_textures.GetMipMap();
    if (!tex) { DbgPrint("glCopyTexSubImage2D: no texture bound\n"); return; }

    D3DSURFACE_DESC texDesc;
    if (FAILED(tex->GetLevelDesc(level, &texDesc))) return;

    // Xbox requires power-of-two textures.
    // XGSwizzleRect produces garbage for non-POT dimensions.
    // --- 1. End scene and block until GPU is truly idle ---
    // On Xbox the GPU command buffer may still have pending draw calls
    // after EndScene. KickPushBuffer flushes the buffer to the GPU and
    // BlockUntilIdle spins until all rendering is complete before we lock.
    // --- 2. Get backbuffer, copy to linear staging surface, then lock ---
    // The Xbox backbuffer is tiled even when the format says LIN_X8R8G8B8.
    // Direct LockRect returns tiled memory which reads as garbage scanlines.
    // CopyRects GPU-detiles into a CreateImageSurface (which IS truly linear),
    // then we lock THAT surface to get correct linear scanlines.
    IDirect3DSurface8* pBB = NULL;
    dev->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBB);
    if (!pBB) return;

    // Create a linear staging surface the same size as the backbuffer
    IDirect3DSurface8* pStage = NULL;
    if (FAILED(dev->CreateImageSurface(backBufDesc.Width, backBufDesc.Height,
                                        D3DFMT_LIN_X8R8G8B8, &pStage)))
    {
        pBB->Release();
        return;
    }

    // Copy backbuffer -> staging (GPU handles detiling)
    dev->CopyRects(pBB, NULL, 0, pStage, NULL);
    // CopyRects is asynchronous ? block until the copy is done
    dev->KickPushBuffer();
    dev->BlockUntilIdle();

    pBB->Release();
    pBB = NULL;

    D3DLOCKED_RECT lr;
    if (FAILED(pStage->LockRect(&lr, NULL, D3DLOCK_READONLY)))
    {
        pStage->Release();
        return;
    }

    const int srcBpp = 4;

    // Determine destination texture bytes-per-pixel
    int dstBpp;
    switch (texDesc.Format)
    {
    case D3DFMT_L8: case D3DFMT_A8: case D3DFMT_P8:
        dstBpp = 1; break;
    case D3DFMT_R5G6B5: case D3DFMT_A4R4G4B4: case D3DFMT_A8L8:
        dstBpp = 2; break;
    default:
        dstBpp = 4; break;
    }

    // --- 3. Ensure shadow copy exists for the destination texture ---
    TextureEntry* entry = m_textures.GetCurrentEntry();
    if (!entry)
    {
        pStage->UnlockRect(); pStage->Release();
        return;
    }

    DWORD fullPitch = texDesc.Width  * dstBpp;
    DWORD fullSize  = fullPitch * texDesc.Height;

    if (!entry->m_shadowPixels ||
        entry->m_shadowWidth  != texDesc.Width  ||
        entry->m_shadowHeight != texDesc.Height ||
        entry->m_shadowBpp    != (DWORD)dstBpp)
    {
        delete[] entry->m_shadowPixels;
        entry->m_shadowPixels = new char[fullSize];
        if (!entry->m_shadowPixels)
        {
            pStage->UnlockRect(); pStage->Release();
            return;
        }
        entry->m_shadowPitch  = fullPitch;
        entry->m_shadowWidth  = texDesc.Width;
        entry->m_shadowHeight = texDesc.Height;
        entry->m_shadowBpp    = dstBpp;

        // Init shadow from existing texture content
        D3DLOCKED_RECT tlr;
        if (SUCCEEDED(tex->LockRect(level, &tlr, NULL, D3DLOCK_READONLY)))
        {
            XGUnswizzleRect(tlr.pBits, texDesc.Width, texDesc.Height, NULL,
                            entry->m_shadowPixels, entry->m_shadowPitch, NULL, dstBpp);
            tex->UnlockRect(level);
        }
        else memset(entry->m_shadowPixels, 0, fullSize);
    }

    // --- 4. Copy pixels from backbuffer into shadow with Y-flip ---
    // GL y=0 is bottom of screen; D3D/Xbox y=0 is top.
    for (int row = 0; row < height; row++)
    {
        int srcY = (int)backBufDesc.Height - 1 - (y + row);
        if (srcY < 0 || srcY >= (int)backBufDesc.Height) continue;

        int dstRow = yoffset + row;
        if (dstRow < 0 || dstRow >= (int)texDesc.Height) continue;

        const unsigned char* srcRow =
            (const unsigned char*)lr.pBits + srcY * lr.Pitch + x * srcBpp;


        unsigned char* dstRowPtr =
            (unsigned char*)entry->m_shadowPixels +
            dstRow * entry->m_shadowPitch +
            xoffset * dstBpp;

        for (int col = 0; col < width; col++)
        {
            if (x + col >= (int)backBufDesc.Width)   break;
            if (xoffset + col >= (int)texDesc.Width)  break;

            // Backbuffer pixel order: B G R X (little-endian X8R8G8B8)
            unsigned char b = srcRow[col * srcBpp + 0];
            unsigned char g = srcRow[col * srcBpp + 1];
            unsigned char r = srcRow[col * srcBpp + 2];
            // srcRow[3] is X (unused alpha)

            switch (texDesc.Format)
            {
            case D3DFMT_A8R8G8B8:
                dstRowPtr[col*4+0]=b; dstRowPtr[col*4+1]=g;
                dstRowPtr[col*4+2]=r; dstRowPtr[col*4+3]=0xff;
                break;
            case D3DFMT_X8R8G8B8:
                dstRowPtr[col*4+0]=b; dstRowPtr[col*4+1]=g;
                dstRowPtr[col*4+2]=r; dstRowPtr[col*4+3]=0xff;
                break;
            case D3DFMT_R5G6B5:
            {
                unsigned short v = (unsigned short)(b>>3)
                                 | ((unsigned short)((g>>2)&0x3f)<<5)
                                 | ((unsigned short)(r>>3)<<11);
                dstRowPtr[col*2+0]=(unsigned char)(v&0xff);
                dstRowPtr[col*2+1]=(unsigned char)(v>>8);
            } break;
            case D3DFMT_A4R4G4B4:
            {
                unsigned short v = (unsigned short)(b>>4)
                                 | ((unsigned short)(g>>4)<<4)
                                 | ((unsigned short)(r>>4)<<8)
                                 | 0xf000;
                dstRowPtr[col*2+0]=(unsigned char)(v&0xff);
                dstRowPtr[col*2+1]=(unsigned char)(v>>8);
            } break;
            case D3DFMT_L8: case D3DFMT_P8:
                dstRowPtr[col] = (unsigned char)(((int)r+g+b)/3);
                break;
            default:
                if (dstBpp==4)
                {
                    dstRowPtr[col*4+0]=b; dstRowPtr[col*4+1]=g;
                    dstRowPtr[col*4+2]=r; dstRowPtr[col*4+3]=0xff;
                }
                break;
            }
        }
    }

    pStage->UnlockRect();
    pStage->Release();

    // --- 5. Swizzle shadow back into the texture ---
    D3DLOCKED_RECT tlr;
    if (SUCCEEDED(tex->LockRect(level, &tlr, NULL, 0)))
    {
        XGSwizzleRect(entry->m_shadowPixels, entry->m_shadowPitch, NULL,
                      tlr.pBits, texDesc.Width, texDesc.Height, NULL, dstBpp);
        tex->UnlockRect(level);
    }

    m_textureState.DirtyTexture(m_textures.GetCurrentID());
}

// ---------------------------------------------------------------------------
// CopyTexImage2D ? allocate a fresh texture level from the framebuffer.
// ---------------------------------------------------------------------------
void RXGLTextureSystem::CopyTexImage2D(
    GLenum target, GLint level, GLenum internalformat,
    GLint x, GLint y, GLsizei width, GLsizei height,
    IDirect3DDevice8* dev, IDirect3D8* d3d,
    D3DSURFACE_DESC& backBufDesc)
{
    if (target != GL_TEXTURE_2D) { LocalDebugBreak(); return; }
    if (width <= 0 || height <= 0) return;

    D3DFORMAT dstFmt = GLToDXPixelFormat(internalformat, GL_RGBA);
    if (FAILED(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
               backBufDesc.Format, 0, D3DRTYPE_TEXTURE, dstFmt)))
        dstFmt = m_force16bit ? D3DFMT_A4R4G4B4 : D3DFMT_A8R8G8B8;

    if (level == 0)
        m_textures.SetTexture(NULL, D3DFMT_UNKNOWN, 0);

    IDirect3DTexture8* tex = m_textures.GetMipMap();
    if (!tex)
    {
        HRESULT hr = dev->CreateTexture(width, height, 1, 0,
                                         dstFmt, D3DPOOL_MANAGED, &tex);
        if (FAILED(hr)) { InterpretError(hr); return; }
        m_textures.SetTexture(tex, dstFmt, internalformat);
    }

    CopyTexSubImage2D(target, level, 0, 0, x, y, width, height, dev, backBufDesc);
}
