/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_matrix.h  --  Matrix stack subsystem declarations.
 *                    Declares RXGLMatrixState, which wraps three ID3DXMatrixStack
 *                    instances (modelview, projection, texture) and mirrors GL matrix
 *                    mode semantics. Dirty flags trigger D3D SetTransform calls only
 *                    when a stack actually changes.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_MATRIX_H
#define RXGL_MATRIX_H

#include "rxgl_internal.h"

struct RXGLMatrixState
{
    ID3DXMatrixStack* modelView;
    ID3DXMatrixStack* projection;
    ID3DXMatrixStack* texture;
    ID3DXMatrixStack* current;       // points to one of the above

    D3DXMATRIX        viewMatrix;    // always identity for this shim

    bool              modelViewDirty;
    bool              projectionDirty;
    bool              textureDirty;
    bool*             currentDirty;  // points to one of the above

    bool              viewDirty;

    GLenum            matrixMode;

    void Init();
    void Destroy();

    // Apply dirty matrices to D3D device
    void Apply(IDirect3DDevice8* dev, GLsizei vpW, GLsizei vpH);

    // GL entry points
    void SetMatrixMode(GLenum mode);
    void LoadIdentity();
    void PushMatrix();
    void PopMatrix();
    void LoadMatrixf(const GLfloat* m);
    void MultMatrixf(const GLfloat* m);
    void MultMatrixd(const GLdouble* m);
    void Translatef(GLfloat x, GLfloat y, GLfloat z);
    void Scalef(GLfloat x, GLfloat y, GLfloat z);
    void Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void Ortho(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble zn, GLdouble zf);
    void Frustum(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble zn, GLdouble zf);

    // Accessor used by immediate mode for software texture matrix transform
    ID3DXMatrixStack* GetTextureStack() { return texture; }
};

#endif // RXGL_MATRIX_H
