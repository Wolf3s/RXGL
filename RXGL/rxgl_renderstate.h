/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_renderstate.h  --  Render state cache subsystem declarations.
 *                         Declares RXGLRenderState, which mirrors the full set of
 *                         GL render states as dirty-flagged fields so that redundant
 *                         D3D SetRenderState calls are skipped. Covers: alpha test,
 *                         blend, cull, depth, stencil, scissor, fog, color mask,
 *                         polygon mode, polygon offset, shade model, and viewport.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_RENDERSTATE_H
#define RXGL_RENDERSTATE_H

#include "rxgl_internal.h"

struct RXGLRenderState
{
    // ---- Alpha test -----------------------------------------------------
    bool     alphaTestDirty;
    bool     alphaTest;
    GLenum   alphaFunc;
    GLclampf alphaFuncRef;

    // ---- Blend ----------------------------------------------------------
    bool     blendDirty;
    bool     blend;
    GLenum   blendSFactor;
    GLenum   blendDFactor;
    GLenum   blendEquation;  // GL_FUNC_ADD etc.

    // ---- Cull -----------------------------------------------------------
    bool     cullDirty;
    bool     cullFace;
    GLenum   cullFaceMode;
    GLenum   frontFaceMode;

    // ---- Depth ----------------------------------------------------------
    bool     depthDirty;
    bool     depthTest;
    GLenum   depthFunc;
    bool     depthMask;
    GLclampd depthRangeNear;
    GLclampd depthRangeFar;
    GLclampd clearDepth;

    // ---- Stencil --------------------------------------------------------
    bool     stencilDirty;
    bool     stencilTest;
    GLenum   stencilFunc;
    GLint    stencilRef;
    GLuint   stencilValueMask;
    GLenum   stencilFail;
    GLenum   stencilZFail;
    GLenum   stencilZPass;
    GLuint   stencilWriteMask;
    GLint    clearStencil;

    // ---- Scissor --------------------------------------------------------
    bool     scissorDirty;
    bool     scissorTest;
    GLint    scissorX;
    GLint    scissorY;
    GLsizei  scissorW;
    GLsizei  scissorH;

    // ---- Fog ------------------------------------------------------------
    bool     fogDirty;
    bool     fog;
    GLenum   fogMode;
    GLfloat  fogDensity;
    GLfloat  fogStart;
    GLfloat  fogEnd;
    GLfloat  fogColor[4];

    // ---- Color mask -----------------------------------------------------
    bool      colorMaskDirty;
    GLboolean colorMaskR;
    GLboolean colorMaskG;
    GLboolean colorMaskB;
    GLboolean colorMaskA;

    // ---- Polygon mode / offset ------------------------------------------
    bool     polygonModeDirty;
    GLenum   polygonModeFront;
    GLenum   polygonModeBack;
    bool     polygonOffsetFill;
    bool     polygonOffsetDirty;
    GLfloat  polygonOffsetFactor;
    GLfloat  polygonOffsetUnits;

    // ---- Shade model ----------------------------------------------------
    bool     shadeModelDirty;
    GLenum   shadeModel;

    // ---- Line width / point size ----------------------------------------
    bool     lineWidthDirty;
    GLfloat  lineWidth;
    bool     pointSizeDirty;
    GLfloat  pointSize;

    // ---- Viewport -------------------------------------------------------
    bool     viewportDirty;
    GLint    viewportX;
    GLint    viewportY;
    GLsizei  viewportW;
    GLsizei  viewportH;

    // ---- Misc -----------------------------------------------------------
    D3DCOLOR clearColor;
    GLclampf clearR, clearG, clearB, clearA;
    GLint    unpackAlignment;
    GLint    unpackRowLength;

    // ---- Init / Apply ---------------------------------------------------
    void Init(GLsizei screenW, GLsizei screenH);
    void Apply(IDirect3DDevice8* dev, GLsizei vpW, GLsizei vpH);

    // ---- GL entry points ------------------------------------------------
    void SetAlphaFunc(GLenum func, GLclampf ref);
    void SetBlendFunc(GLenum sfactor, GLenum dfactor);
    void SetBlendEquation(GLenum mode);
    void SetCullFace(GLenum mode);
    void SetFrontFace(GLenum mode);
    void SetDepthFunc(GLenum func);
    void SetDepthMask(GLboolean flag);
    void SetDepthRange(GLclampd n, GLclampd f);
    void SetClearDepth(GLclampd d);
    void SetStencilFunc(GLenum func, GLint ref, GLuint mask);
    void SetStencilMask(GLuint mask);
    void SetStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
    void SetClearStencil(GLint s);
    void SetScissor(GLint x, GLint y, GLsizei w, GLsizei h);
    void SetFogf(GLenum pname, GLfloat param);
    void SetFogi(GLenum pname, GLint param);
    void SetFogfv(GLenum pname, const GLfloat* params);
    void SetColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
    void SetShadeModel(GLenum mode);
    void SetPolygonMode(GLenum face, GLenum mode);
    void SetPolygonOffset(GLfloat factor, GLfloat units);
    void SetClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
    void SetPixelStorei(GLenum pname, GLint param);
    void SetViewport(GLint x, GLint y, GLsizei w, GLsizei h,
                     bool& projDirty);
    void SetLineWidth(GLfloat width);
    void SetPointSize(GLfloat size);

    // Enable/disable for caps owned by render state
    // Returns true if the cap was handled here, false if it belongs elsewhere
    bool EnableDisable(GLenum cap, bool value);
    GLboolean IsEnabled(GLenum cap) const;
};

#endif // RXGL_RENDERSTATE_H
