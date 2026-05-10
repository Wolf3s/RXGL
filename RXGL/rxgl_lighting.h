/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_lighting.h  --  Software fixed-function lighting subsystem declarations.
 *                      Defines RXGLLight and RXGLMaterial structs, and
 *                      RXGLLightingState which owns all eight lights and material
 *                      properties. Exposes per-vertex lighting calculation used by
 *                      the immediate mode and array paths when GL_LIGHTING is enabled.
 *                      Supports GL_LIGHT0..GL_LIGHT7, point/directional lights,
 *                      attenuation, spotlights, and Blinn-Phong specular.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#ifndef RXGL_LIGHTING_H
#define RXGL_LIGHTING_H

#include "rxgl_internal.h"

#define MAXLIGHTS 8

struct RXGLLight
{
    bool    enabled;

    GLfloat position[4];        // w=0 directional, w=1 positional
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];

    GLfloat spotDirection[3];   // normalised spot direction (eye space)
    GLfloat spotExponent;       // 0..128
    GLfloat spotCutoff;         // 0..90 or 180 (180 = no spot)

    GLfloat constAtten;         // GL_CONSTANT_ATTENUATION
    GLfloat linearAtten;        // GL_LINEAR_ATTENUATION
    GLfloat quadAtten;          // GL_QUADRATIC_ATTENUATION

    void Init(int index)
    {
        enabled       = false;
        // GL defaults: position (0,0,1,0) for all lights
        position[0]   = 0.0f; position[1] = 0.0f;
        position[2]   = 1.0f; position[3] = 0.0f;
        // GL defaults: diffuse/specular white for light 0, black for rest
        if (index == 0) {
            ambient[0]  = ambient[1]  = ambient[2]  = 0.0f; ambient[3]  = 1.0f;
            diffuse[0]  = diffuse[1]  = diffuse[2]  = 1.0f; diffuse[3]  = 1.0f;
            specular[0] = specular[1] = specular[2] = 1.0f; specular[3] = 1.0f;
        } else {
            ambient[0]  = ambient[1]  = ambient[2]  = 0.0f; ambient[3]  = 1.0f;
            diffuse[0]  = diffuse[1]  = diffuse[2]  = 0.0f; diffuse[3]  = 1.0f;
            specular[0] = specular[1] = specular[2] = 0.0f; specular[3] = 1.0f;
        }
        spotDirection[0] = 0.0f; spotDirection[1] = 0.0f; spotDirection[2] = -1.0f;
        spotExponent  = 0.0f;
        spotCutoff    = 180.0f;
        constAtten    = 1.0f;
        linearAtten   = 0.0f;
        quadAtten     = 0.0f;
    }
};

struct RXGLLightingState
{
    bool    lighting;
    bool    normalize;
    bool    colorMaterial;

    RXGLLight lights[MAXLIGHTS];

    GLfloat currentNormal[3];
    GLfloat currentColor[4];

    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialEmission[4];
    GLfloat materialShininess;

    GLfloat lightModelAmbient[4];
    GLint   lightModelColorControl;

    void Init();

    // Returns true if lighting is active and changed the color.
    // Call this before SetVertex; it calls SetColor on the vertex buffer.
    // vx,vy,vz = object-space vertex position (needed for positional light attenuation).
    bool ApplyToVertex(class OGLPrimitiveVertexBuffer& vb, const D3DXMATRIX* modelView,
                       GLfloat vx, GLfloat vy, GLfloat vz);

    // GL entry points
    void SetNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
    void SetLightfv(GLenum light, GLenum pname, const GLfloat* params);
    void SetLightf(GLenum light, GLenum pname, GLfloat param);
    void SetLightModelfv(GLenum pname, const GLfloat* params);
    void SetLightModelf(GLenum pname, GLfloat param);
    void SetLightModeli(GLenum pname, GLint param);
    void SetMaterialfv(GLenum face, GLenum pname, const GLfloat* params);
    void SetMaterialf(GLenum face, GLenum pname, GLfloat param);
    void SetColorMaterial(GLenum face, GLenum mode);

    void UpdateMaterialFromColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

    bool EnableDisable(GLenum cap, bool value);
    GLboolean IsEnabled(GLenum cap) const;

private:
    // Returns light index from GL_LIGHTi, or -1 if out of range
    static int LightIndex(GLenum light)
    {
        int idx = (int)light - (int)GL_LIGHT0;
        return (idx >= 0 && idx < MAXLIGHTS) ? idx : -1;
    }
};

#endif // RXGL_LIGHTING_H
