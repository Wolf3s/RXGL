/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_matrix.cpp  --  Matrix stack subsystem implementation.
 *                      Implements glMatrixMode, glLoadIdentity, glLoadMatrixf/d,
 *                      glMultMatrixf/d, glPushMatrix, glPopMatrix, glRotatef/d,
 *                      glScalef/d, glTranslatef/d, glOrtho, glFrustum, and the
 *                      transpose variants. Forwards dirty matrices to D3D SetTransform.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_matrix.h"

void RXGLMatrixState::Init()
{
    D3DXCreateMatrixStack(0, &modelView);
    D3DXCreateMatrixStack(0, &projection);
    D3DXCreateMatrixStack(0, &texture);

    current      = modelView;
    matrixMode   = GL_MODELVIEW;

    modelViewDirty  = true;
    projectionDirty = true;
    textureDirty    = true;
    viewDirty       = true;
    currentDirty    = &modelViewDirty;

    modelView->LoadIdentity();
    projection->LoadIdentity();
    texture->LoadIdentity();

    D3DXMatrixIdentity(&viewMatrix);
}

void RXGLMatrixState::Destroy()
{
    if (modelView)  { modelView->Release();  modelView  = NULL; }
    if (projection) { projection->Release(); projection = NULL; }
    if (texture)    { texture->Release();    texture    = NULL; }
}

void RXGLMatrixState::Apply(IDirect3DDevice8* dev, GLsizei vpW, GLsizei vpH)
{
    if (modelViewDirty)
    {
        modelViewDirty = false;
        dev->SetTransform(D3DTS_WORLD, modelView->GetTop());
    }

    if (viewDirty)
    {
        viewDirty = false;
        dev->SetTransform(D3DTS_VIEW, &viewMatrix);
    }

    if (projectionDirty)
    {
        projectionDirty = false;

        D3DXMATRIX proj = *projection->GetTop();

        // D3D8/Xbox half-pixel correction.
        // OpenGL-style rasterization expects texel/pixel centers to line up differently.
        // This fixes 1-pixel "steps" on magnified textures/quads.
        D3DXMATRIX halfPixelFix;
        D3DXMatrixIdentity(&halfPixelFix);
        halfPixelFix._41 = -1.0f / (float)vpW;
        halfPixelFix._42 =  1.0f / (float)vpH;
        D3DXMatrixMultiply(&proj, &proj, &halfPixelFix);

        dev->SetTransform(D3DTS_PROJECTION, &proj);
    }

    if (textureDirty)
    {
        textureDirty = false;
        // Texture matrix is applied in software in glTexCoord2f().
        // Keep D3D texture transform disabled.
        dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }
}

void RXGLMatrixState::SetMatrixMode(GLenum mode)
{
    matrixMode = mode;
    switch (mode)
    {
    case GL_MODELVIEW:
        current = modelView;  currentDirty = &modelViewDirty;  break;
    case GL_PROJECTION:
        current = projection; currentDirty = &projectionDirty; break;
    case GL_TEXTURE:
        current = texture;    currentDirty = &textureDirty;    break;
    default:
        LocalDebugBreak();
        break;
    }
}

void RXGLMatrixState::LoadIdentity()
{
    current->LoadIdentity();
    *currentDirty = true;
}

void RXGLMatrixState::PushMatrix()
{
    current->Push();
}

void RXGLMatrixState::PopMatrix()
{
    current->Pop();
    *currentDirty = true;
}

void RXGLMatrixState::LoadMatrixf(const GLfloat* m)
{
    current->LoadMatrix((D3DXMATRIX*)m);
    *currentDirty = true;
}

void RXGLMatrixState::MultMatrixf(const GLfloat* m)
{
    current->MultMatrixLocal((D3DXMATRIX*)m);
    *currentDirty = true;
}

void RXGLMatrixState::MultMatrixd(const GLdouble* m)
{
    D3DXMATRIX fm;
    for (int i = 0; i < 16; i++) fm.m[i/4][i%4] = (float)m[i];
    current->MultMatrixLocal(&fm);
    *currentDirty = true;
}

void RXGLMatrixState::Translatef(GLfloat x, GLfloat y, GLfloat z)
{
    D3DXMATRIX m; D3DXMatrixTranslation(&m, x, y, z);
    current->MultMatrixLocal(&m);
    *currentDirty = true;
}

void RXGLMatrixState::Scalef(GLfloat x, GLfloat y, GLfloat z)
{
    D3DXMATRIX m; D3DXMatrixScaling(&m, x, y, z);
    current->MultMatrixLocal(&m);
    *currentDirty = true;
}

void RXGLMatrixState::Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    D3DXVECTOR3 v(x, y, z);
    float rad = angle * (3.14159265358979f / 180.f);
    current->RotateAxisLocal(&v, rad);
    *currentDirty = true;
}

void RXGLMatrixState::Ortho(GLdouble l, GLdouble r, GLdouble b, GLdouble t,
                               GLdouble zn, GLdouble zf)
{
    D3DXMATRIX m;
    D3DXMatrixOrthoOffCenterRH(&m,
        (float)l, (float)r, (float)b, (float)t, (float)zn, (float)zf);
    current->MultMatrixLocal(&m);
    *currentDirty = true;
}

void RXGLMatrixState::Frustum(GLdouble l, GLdouble r, GLdouble b, GLdouble t,
                                 GLdouble zn, GLdouble zf)
{
    D3DXMATRIX m;
    D3DXMatrixPerspectiveOffCenterRH(&m,
        (float)l, (float)r, (float)b, (float)t, (float)zn, (float)zf);
    current->MultMatrixLocal(&m);
    *currentDirty = true;
}
