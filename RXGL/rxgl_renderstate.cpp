/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_renderstate.cpp  --  Render state cache implementation.
 *                           Implements glEnable/glDisable for all supported caps,
 *                           glBlendFunc, glBlendEquation, glDepthFunc, glDepthMask,
 *                           glCullFace, glFrontFace, glStencilFunc, glStencilOp,
 *                           glScissor, glFog*, glShadeModel, glPolygonMode,
 *                           glPolygonOffset, glColorMask, and glViewport.
 *                           Flushes dirty state to D3D8 before each draw call.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_renderstate.h"

// Globals (defined in rxgl_core.cpp)
extern DWORD gWidth;
extern DWORD gHeight;

void RXGLRenderState::Init(GLsizei screenW, GLsizei screenH)
{
    alphaTestDirty   = true;
    alphaTest        = false;
    alphaFunc        = GL_ALWAYS;
    alphaFuncRef     = 0.0f;

    blendDirty       = true;
    blend            = false;
    blendSFactor     = GL_ONE;
    blendDFactor     = GL_ZERO;
    blendEquation    = GL_FUNC_ADD;

    cullDirty        = true;
    cullFace         = false;
    cullFaceMode     = GL_BACK;
    frontFaceMode    = GL_CCW;

    depthDirty       = true;
    depthTest        = false;
    depthFunc        = GL_LESS;
    depthMask        = true;
    depthRangeNear   = 0.0;
    depthRangeFar    = 1.0;
    clearDepth       = 1.0;

    stencilDirty     = true;
    stencilTest      = false;
    stencilFunc      = GL_ALWAYS;
    stencilRef       = 0;
    stencilValueMask = 0xFFFFFFFF;
    stencilFail      = GL_KEEP;
    stencilZFail     = GL_KEEP;
    stencilZPass     = GL_KEEP;
    stencilWriteMask = 0xFFFFFFFF;
    clearStencil     = 0;

    scissorDirty     = true;
    scissorTest      = false;
    scissorX         = 0;
    scissorY         = 0;
    scissorW         = screenW;
    scissorH         = screenH;

    fogDirty         = true;
    fog              = false;
    fogMode          = GL_EXP;
    fogDensity       = 1.0f;
    fogStart         = 0.0f;
    fogEnd           = 1.0f;
    fogColor[0] = fogColor[1] = fogColor[2] = fogColor[3] = 0.0f;

    colorMaskDirty   = true;
    colorMaskR = colorMaskG = colorMaskB = colorMaskA = GL_TRUE;

    polygonModeDirty   = true;
    polygonModeFront   = GL_FILL;
    polygonModeBack    = GL_FILL;
    polygonOffsetFill  = false;
    polygonOffsetDirty = true;
    polygonOffsetFactor = 0.0f;
    polygonOffsetUnits  = 0.0f;

    shadeModelDirty  = true;
    shadeModel       = GL_SMOOTH;

    lineWidthDirty   = true;
    lineWidth        = 1.0f;
    pointSizeDirty   = true;
    pointSize        = 1.0f;

    viewportDirty    = true;
    viewportX        = 0;
    viewportY        = 0;
    viewportW        = screenW;
    viewportH        = screenH;

    clearColor = D3DCOLOR_RGBA(0, 0, 0, 255);
    clearR = clearG = clearB = 0.0f; clearA = 1.0f;
    unpackAlignment  = 4;
    unpackRowLength  = 0;
}

void RXGLRenderState::Apply(IDirect3DDevice8* dev, GLsizei vpW, GLsizei vpH)
{
    if (alphaTestDirty)
    {
        alphaTestDirty = false;
        dev->SetRenderState(D3DRS_ALPHATESTENABLE, alphaTest ? TRUE : FALSE);
        dev->SetRenderState(D3DRS_ALPHAFUNC,
            alphaTest ? GLToDXCompare(alphaFunc) : D3DCMP_ALWAYS);
        dev->SetRenderState(D3DRS_ALPHAREF, (DWORD)(255.0f * alphaFuncRef));
    }

    if (blendDirty)
    {
        blendDirty = false;
        dev->SetRenderState(D3DRS_ALPHABLENDENABLE, blend ? TRUE : FALSE);
        dev->SetRenderState(D3DRS_SRCBLEND,
            blend ? GLToDXSBlend(blendSFactor) : D3DBLEND_ONE);
        dev->SetRenderState(D3DRS_DESTBLEND,
            blend ? GLToDXDBlend(blendDFactor) : D3DBLEND_ZERO);

        // Blend equation — always set so it's correct even when blend is off
        D3DBLENDOP dxOp;
        switch (blendEquation)
        {
        case GL_FUNC_SUBTRACT:         dxOp = D3DBLENDOP_SUBTRACT;      break;
        case GL_FUNC_REVERSE_SUBTRACT: dxOp = D3DBLENDOP_REVSUBTRACT;   break;
        case GL_MIN:                   dxOp = D3DBLENDOP_MIN;            break;
        case GL_MAX:                   dxOp = D3DBLENDOP_MAX;            break;
        case GL_FUNC_ADD:
        default:                       dxOp = D3DBLENDOP_ADD;            break;
        }
        dev->SetRenderState(D3DRS_BLENDOP, (DWORD)dxOp);
    }

    if (polygonModeDirty)
    {
        polygonModeDirty = false;

        GLenum mode = polygonModeFront;
        if (polygonModeFront != polygonModeBack)
        {
            if (polygonModeFront == GL_LINE || polygonModeBack == GL_LINE)
                mode = GL_LINE;
            else if (polygonModeFront == GL_POINT || polygonModeBack == GL_POINT)
                mode = GL_POINT;
            else
                mode = GL_FILL;
        }

        D3DFILLMODE dxFill = D3DFILL_SOLID;
        switch (mode)
        {
        case GL_POINT: dxFill = D3DFILL_POINT;     break;
        case GL_LINE:  dxFill = D3DFILL_WIREFRAME;  break;
        default:       dxFill = D3DFILL_SOLID;      break;
        }
        dev->SetRenderState(D3DRS_FILLMODE, dxFill);
    }

    if (polygonOffsetDirty)
    {
        polygonOffsetDirty = false;
        dev->SetRenderState(D3DRS_SOLIDOFFSETENABLE,
            polygonOffsetFill ? TRUE : FALSE);
        dev->SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE,
            *(DWORD*)&polygonOffsetFactor);
        dev->SetRenderState(D3DRS_POLYGONOFFSETZOFFSET,
            *(DWORD*)&polygonOffsetUnits);
    }

    if (cullDirty)
    {
        cullDirty = false;
        D3DCULL cull = D3DCULL_NONE;
        if (cullFace)
        {
            bool frontCCW = (frontFaceMode == GL_CCW);
            switch (cullFaceMode)
            {
            case GL_BACK:
                cull = frontCCW ? D3DCULL_CW : D3DCULL_CCW; break;
            case GL_FRONT:
                cull = frontCCW ? D3DCULL_CCW : D3DCULL_CW; break;
            case GL_FRONT_AND_BACK:
                cull = D3DCULL_NONE; break;
            default:
                cull = frontCCW ? D3DCULL_CW : D3DCULL_CCW; break;
            }
        }
        dev->SetRenderState(D3DRS_CULLMODE, cull);
    }

    if (depthDirty)
    {
        depthDirty = false;
        dev->SetRenderState(D3DRS_ZENABLE,      depthTest ? D3DZB_TRUE : D3DZB_FALSE);
        dev->SetRenderState(D3DRS_ZWRITEENABLE, depthMask ? TRUE : FALSE);
        dev->SetRenderState(D3DRS_ZFUNC,        GLToDXCompare(depthFunc));
    }

    if (stencilDirty)
    {
        stencilDirty = false;
        dev->SetRenderState(D3DRS_STENCILENABLE,    stencilTest ? TRUE : FALSE);
        dev->SetRenderState(D3DRS_STENCILFUNC,      GLToDXCompare(stencilFunc));
        dev->SetRenderState(D3DRS_STENCILREF,       (DWORD)stencilRef);
        dev->SetRenderState(D3DRS_STENCILMASK,      stencilValueMask);
        dev->SetRenderState(D3DRS_STENCILWRITEMASK, stencilWriteMask);
        dev->SetRenderState(D3DRS_STENCILFAIL,      GLToDXStencilOp(stencilFail));
        dev->SetRenderState(D3DRS_STENCILZFAIL,     GLToDXStencilOp(stencilZFail));
        dev->SetRenderState(D3DRS_STENCILPASS,      GLToDXStencilOp(stencilZPass));
    }

    if (scissorDirty)
    {
        scissorDirty = false;
        D3DRECT sr;
        if (scissorTest)
        {
            sr.x1 = scissorX;
            sr.x2 = scissorX + scissorW;
            // OpenGL scissor Y is bottom-left; D3D/Xbox is top-left
            sr.y1 = (LONG)gHeight - (scissorY + scissorH);
            sr.y2 = (LONG)gHeight - scissorY;
        }
        else
        {
            sr.x1 = 0; sr.y1 = 0;
            sr.x2 = vpW; sr.y2 = vpH;
        }
        dev->SetScissors(1, FALSE, &sr);
    }

    if (fogDirty)
    {
        fogDirty = false;
        dev->SetRenderState(D3DRS_FOGENABLE, fog ? TRUE : FALSE);
        if (fog)
        {
            D3DFOGMODE dxMode = D3DFOG_LINEAR;
            switch (fogMode)
            {
            case GL_LINEAR: dxMode = D3DFOG_LINEAR; break;
            case GL_EXP:    dxMode = D3DFOG_EXP;    break;
            case GL_EXP2:   dxMode = D3DFOG_EXP2;   break;
            }
            dev->SetRenderState(D3DRS_FOGTABLEMODE,   dxMode);
            dev->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
            dev->SetRenderState(D3DRS_FOGSTART,   *(DWORD*)&fogStart);
            dev->SetRenderState(D3DRS_FOGEND,     *(DWORD*)&fogEnd);
            dev->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&fogDensity);
            DWORD fc = D3DCOLOR_COLORVALUE(fogColor[0], fogColor[1],
                                           fogColor[2], fogColor[3]);
            dev->SetRenderState(D3DRS_FOGCOLOR, fc);
        }
        else
        {
            dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
        }
    }

    if (colorMaskDirty)
    {
        colorMaskDirty = false;
        DWORD mask = 0;
        if (colorMaskR) mask |= D3DCOLORWRITEENABLE_RED;
        if (colorMaskG) mask |= D3DCOLORWRITEENABLE_GREEN;
        if (colorMaskB) mask |= D3DCOLORWRITEENABLE_BLUE;
        if (colorMaskA) mask |= D3DCOLORWRITEENABLE_ALPHA;
        if (colorMaskR && colorMaskG && colorMaskB && colorMaskA)
            mask = D3DCOLORWRITEENABLE_ALL;
        dev->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
    }

    if (shadeModelDirty)
    {
        shadeModelDirty = false;
        dev->SetRenderState(D3DRS_SHADEMODE,
            shadeModel == GL_SMOOTH ? D3DSHADE_GOURAUD : D3DSHADE_FLAT);
    }

    if (lineWidthDirty)
    {
        lineWidthDirty = false;
        dev->SetRenderState(D3DRS_LINEWIDTH, *(DWORD*)&lineWidth);
    }

    if (pointSizeDirty)
    {
        pointSizeDirty = false;
        dev->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)&pointSize);
    }

    if (viewportDirty)
    {
        viewportDirty = false;
        D3DVIEWPORT8 vp;
        vp.X      = (DWORD)viewportX;
        // OpenGL viewport Y is bottom-left origin; D3D is top-left.
        // Convert: D3D_Y = screenHeight - glY - viewportHeight
        vp.Y      = (DWORD)((int)gHeight - viewportY - viewportH);
        vp.Width  = (DWORD)viewportW;
        vp.Height = (DWORD)viewportH;
        vp.MinZ   = (float)depthRangeNear;
        vp.MaxZ   = (float)depthRangeFar;
        dev->SetViewport(&vp);
    }
}

// ---- GL entry points -------------------------------------------------------

void RXGLRenderState::SetAlphaFunc(GLenum func, GLclampf ref)
{
    if (alphaFunc != func || alphaFuncRef != ref)
    {
        alphaFunc = func; alphaFuncRef = ref;
        alphaTestDirty = true;
    }
}

void RXGLRenderState::SetBlendFunc(GLenum sfactor, GLenum dfactor)
{
    if (blendSFactor != sfactor || blendDFactor != dfactor)
    {
        blendSFactor = sfactor; blendDFactor = dfactor;
        blendDirty = true;
    }
}

void RXGLRenderState::SetBlendEquation(GLenum mode)
{
    if (blendEquation != mode)
    {
        blendEquation = mode;
        blendDirty = true;
    }
}

void RXGLRenderState::SetCullFace(GLenum mode)
{
    if (cullFaceMode != mode) { cullFaceMode = mode; cullDirty = true; }
}

void RXGLRenderState::SetFrontFace(GLenum mode)
{
    if (mode != GL_CW && mode != GL_CCW) return;
    if (frontFaceMode != mode) { frontFaceMode = mode; cullDirty = true; }
}

void RXGLRenderState::SetDepthFunc(GLenum func)
{
    if (depthFunc != func) { depthFunc = func; depthDirty = true; }
}

void RXGLRenderState::SetDepthMask(GLboolean flag)
{
    bool v = (flag != 0);
    if (depthMask != v) { depthMask = v; depthDirty = true; }
}

void RXGLRenderState::SetDepthRange(GLclampd n, GLclampd f)
{
    if (depthRangeNear != n || depthRangeFar != f)
    {
        depthRangeNear = n; depthRangeFar = f;
        viewportDirty = true;
    }
}

void RXGLRenderState::SetClearDepth(GLclampd d) { clearDepth = d; }

void RXGLRenderState::SetStencilFunc(GLenum func, GLint ref, GLuint mask)
{
    stencilFunc = func; stencilRef = ref; stencilValueMask = mask;
    stencilDirty = true;
}

void RXGLRenderState::SetStencilMask(GLuint mask)
{
    if (stencilWriteMask != mask) { stencilWriteMask = mask; stencilDirty = true; }
}

void RXGLRenderState::SetStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
    stencilFail = fail; stencilZFail = zfail; stencilZPass = zpass;
    stencilDirty = true;
}

void RXGLRenderState::SetClearStencil(GLint s) { clearStencil = s; }

void RXGLRenderState::SetScissor(GLint x, GLint y, GLsizei w, GLsizei h)
{
    scissorX = x; scissorY = y; scissorW = w; scissorH = h;
    scissorDirty = true;
}

void RXGLRenderState::SetFogf(GLenum pname, GLfloat param)
{
    fogDirty = true;
    switch (pname)
    {
    case GL_FOG_MODE:    fogMode    = (GLenum)(int)param; break;
    case GL_FOG_DENSITY: fogDensity = param;              break;
    case GL_FOG_START:   fogStart   = param;              break;
    case GL_FOG_END:     fogEnd     = param;              break;
    }
}

void RXGLRenderState::SetFogi(GLenum pname, GLint param)
{
    SetFogf(pname, (GLfloat)param);
}

void RXGLRenderState::SetFogfv(GLenum pname, const GLfloat* params)
{
    if (pname == GL_FOG_COLOR)
    {
        fogColor[0] = params[0]; fogColor[1] = params[1];
        fogColor[2] = params[2]; fogColor[3] = params[3];
        fogDirty = true;
    }
    else SetFogf(pname, params[0]);
}

void RXGLRenderState::SetColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    colorMaskR = r; colorMaskG = g; colorMaskB = b; colorMaskA = a;
    colorMaskDirty = true;
}

void RXGLRenderState::SetShadeModel(GLenum mode)
{
    if (shadeModel != mode) { shadeModel = mode; shadeModelDirty = true; }
}

void RXGLRenderState::SetPolygonMode(GLenum face, GLenum mode)
{
    if (mode != GL_POINT && mode != GL_LINE && mode != GL_FILL) return;
    if (face != GL_FRONT && face != GL_BACK && face != GL_FRONT_AND_BACK) return;
    if (face == GL_FRONT || face == GL_FRONT_AND_BACK) polygonModeFront = mode;
    if (face == GL_BACK  || face == GL_FRONT_AND_BACK) polygonModeBack  = mode;
    polygonModeDirty = true;
}

void RXGLRenderState::SetPolygonOffset(GLfloat factor, GLfloat units)
{
    polygonOffsetFactor = factor;
    polygonOffsetUnits  = units;
    polygonOffsetDirty  = true;
}

void RXGLRenderState::SetClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
    clearR = Clamp(r); clearG = Clamp(g); clearB = Clamp(b); clearA = Clamp(a);
    clearColor = D3DRGBA(clearR, clearG, clearB, clearA);
}

void RXGLRenderState::SetPixelStorei(GLenum pname, GLint param)
{
    if (pname == GL_UNPACK_ALIGNMENT)
    {
        if (param == 1 || param == 2 || param == 4 || param == 8)
            unpackAlignment = param;
    }
    else if (pname == GL_UNPACK_ROW_LENGTH)
    {
        unpackRowLength = (param >= 0) ? param : 0;
    }
    else
    {
        DbgPrint("glPixelStorei: unhandled pname 0x%x param %d\n", pname, param);
    }
}

void RXGLRenderState::SetViewport(GLint x, GLint y, GLsizei w, GLsizei h,
                                    bool& projDirty)
{
    if (w <= 0 || h <= 0) return;
    if (viewportX != x || viewportY != y || viewportW != w || viewportH != h)
    {
        viewportX = x; viewportY = y; viewportW = w; viewportH = h;
        viewportDirty = true;
        projDirty = true; // half-pixel correction depends on viewport size
    }
}

bool RXGLRenderState::EnableDisable(GLenum cap, bool value)
{
    switch (cap)
    {
    case GL_ALPHA_TEST:
        if (alphaTest != value) { alphaTest = value; alphaTestDirty = true; }
        return true;

    case GL_BLEND:
        if (blend != value) { blend = value; blendDirty = true; }
        return true;  // caller also needs to notify texture state

    case GL_CULL_FACE:
        if (cullFace != value) { cullFace = value; cullDirty = true; }
        return true;

    case GL_DEPTH_TEST:
        if (depthTest != value) { depthTest = value; depthDirty = true; }
        return true;

    case GL_STENCIL_TEST:
        if (stencilTest != value) { stencilTest = value; stencilDirty = true; }
        return true;

    case GL_POLYGON_OFFSET_FILL:
        if (polygonOffsetFill != value) { polygonOffsetFill = value; polygonOffsetDirty = true; }
        return true;

    case GL_SCISSOR_TEST:
        if (scissorTest != value) { scissorTest = value; scissorDirty = true; }
        return true;

    case GL_FOG:
        if (fog != value) { fog = value; fogDirty = true; }
        return true;

    default:
        return false; // not handled here
    }
}

GLboolean RXGLRenderState::IsEnabled(GLenum cap) const
{
    switch (cap)
    {
    case GL_ALPHA_TEST:   return alphaTest    ? GL_TRUE : GL_FALSE;
    case GL_BLEND:        return blend        ? GL_TRUE : GL_FALSE;
    case GL_CULL_FACE:    return cullFace     ? GL_TRUE : GL_FALSE;
    case GL_DEPTH_TEST:   return depthTest    ? GL_TRUE : GL_FALSE;
    case GL_STENCIL_TEST: return stencilTest  ? GL_TRUE : GL_FALSE;
    case GL_SCISSOR_TEST: return scissorTest  ? GL_TRUE : GL_FALSE;
    case GL_FOG:          return fog          ? GL_TRUE : GL_FALSE;
    default:              return GL_FALSE;
    }
}

void RXGLRenderState::SetLineWidth(GLfloat w)
{
    if (lineWidth != w) { lineWidth = w; lineWidthDirty = true; }
}

void RXGLRenderState::SetPointSize(GLfloat s)
{
    if (pointSize != s) { pointSize = s; pointSizeDirty = true; }
}
