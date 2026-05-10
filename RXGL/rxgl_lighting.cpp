/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_lighting.cpp  --  Software fixed-function lighting implementation.
 *                        Computes per-vertex diffuse, ambient, and specular colour
 *                        using Blinn-Phong in eye space. Called by the immediate mode
 *                        batcher and vertex array path when GL_LIGHTING is enabled.
 *                        Handles glLightfv, glMaterialfv, glLightModelfv, attenuation,
 *                        and spotlight cone calculations.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */


#include "rxgl_lighting.h"
#include "rxgl_immediate.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static inline GLfloat Clamp01(GLfloat v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static inline GLfloat Dot3(const GLfloat* a, const GLfloat* b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void Normalize3(GLfloat* v)
{
    GLfloat len = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.00001f) { v[0]/=len; v[1]/=len; v[2]/=len; }
}

// Safe pow -- avoids calling pow(0,0) which is implementation-defined
static inline GLfloat SafePow(GLfloat base, GLfloat exp)
{
    if (exp <= 0.0f) return 1.0f;
    if (base <= 0.0f) return 0.0f;
    return (GLfloat)pow((double)base, (double)exp);
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------

void RXGLLightingState::Init()
{
    lighting      = false;
    normalize     = false;
    colorMaterial = false;

    for (int i = 0; i < MAXLIGHTS; i++)
        lights[i].Init(i);

    currentNormal[0] = 0.0f; currentNormal[1] = 0.0f; currentNormal[2] = 1.0f;
    currentColor[0]  = 1.0f; currentColor[1]  = 1.0f;
    currentColor[2]  = 1.0f; currentColor[3]  = 1.0f;

    materialAmbient[0] = materialAmbient[1] = materialAmbient[2] = 0.2f;
    materialAmbient[3] = 1.0f;
    materialDiffuse[0] = materialDiffuse[1] = materialDiffuse[2] = 0.8f;
    materialDiffuse[3] = 1.0f;
    materialSpecular[0] = materialSpecular[1] = materialSpecular[2] = 0.0f;
    materialSpecular[3] = 1.0f;
    materialEmission[0] = materialEmission[1] = materialEmission[2] = 0.0f;
    materialEmission[3] = 1.0f;
    materialShininess   = 0.0f;

    lightModelAmbient[0] = lightModelAmbient[1] = lightModelAmbient[2] = 0.2f;
    lightModelAmbient[3] = 1.0f;
    lightModelColorControl = GL_SINGLE_COLOR;
}

// ---------------------------------------------------------------------------
// ApplyToVertex -- full GL 1.x lighting equation, all enabled lights summed
// ---------------------------------------------------------------------------

bool RXGLLightingState::ApplyToVertex(OGLPrimitiveVertexBuffer& vb,
                                         const D3DXMATRIX* modelView,
                                         GLfloat vx, GLfloat vy, GLfloat vz)
{
    if (!lighting)
        return false;

    // Check at least one light is on
    bool anyOn = false;
    for (int i = 0; i < MAXLIGHTS; i++)
        if (lights[i].enabled) { anyOn = true; break; }
    if (!anyOn)
        return false;

    // Transform and optionally normalise the normal
    GLfloat nx = currentNormal[0];
    GLfloat ny = currentNormal[1];
    GLfloat nz = currentNormal[2];
    if (modelView)
    {
        GLfloat tnx = nx*modelView->_11 + ny*modelView->_21 + nz*modelView->_31;
        GLfloat tny = nx*modelView->_12 + ny*modelView->_22 + nz*modelView->_32;
        GLfloat tnz = nx*modelView->_13 + ny*modelView->_23 + nz*modelView->_33;
        nx = tnx; ny = tny; nz = tnz;
    }
    if (normalize)
    {
        GLfloat n[3] = {nx, ny, nz}; Normalize3(n);
        nx = n[0]; ny = n[1]; nz = n[2];
    }

    // Accumulate lighting: start with emission + scene ambient * material ambient
    GLfloat r = materialEmission[0] + lightModelAmbient[0] * materialAmbient[0];
    GLfloat g = materialEmission[1] + lightModelAmbient[1] * materialAmbient[1];
    GLfloat b = materialEmission[2] + lightModelAmbient[2] * materialAmbient[2];

    for (int i = 0; i < MAXLIGHTS; i++)
    {
        const RXGLLight& L = lights[i];
        if (!L.enabled) continue;

        // ---- Light direction and attenuation ----
        GLfloat lx, ly, lz;
        GLfloat attenuation = 1.0f;

        if (L.position[3] == 0.0f)
        {
            // Directional light: direction is the normalised position vector
            lx = L.position[0]; ly = L.position[1]; lz = L.position[2];
            GLfloat lv[3] = {lx,ly,lz}; Normalize3(lv);
            lx = lv[0]; ly = lv[1]; lz = lv[2];
            // Directional lights have no attenuation
        }
        else
        {
            // Positional light: direction and distance from vertex to light.
            // Transform vertex to eye space using the modelview matrix so the
            // distance is correct regardless of where the vertex is in the scene.
            GLfloat evx = vx, evy = vy, evz = vz;
            if (modelView)
            {
                evx = vx*modelView->_11 + vy*modelView->_21 + vz*modelView->_31 + modelView->_41;
                evy = vx*modelView->_12 + vy*modelView->_22 + vz*modelView->_32 + modelView->_42;
                evz = vx*modelView->_13 + vy*modelView->_23 + vz*modelView->_33 + modelView->_43;
            }
            // Vector from vertex to light
            lx = L.position[0] - evx;
            ly = L.position[1] - evy;
            lz = L.position[2] - evz;
            GLfloat dist = (GLfloat)sqrt(lx*lx + ly*ly + lz*lz);
            GLfloat lv[3] = {lx,ly,lz}; Normalize3(lv);
            lx = lv[0]; ly = lv[1]; lz = lv[2];

            // Attenuation based on actual vertex-to-light distance
            GLfloat d = dist;
            GLfloat denom = L.constAtten + L.linearAtten*d + L.quadAtten*d*d;
            if (denom > 0.0f) attenuation = 1.0f / denom;
        }

        // ---- Spotlight factor ----
        GLfloat spotFactor = 1.0f;
        if (L.spotCutoff < 180.0f)
        {
            // cos of cutoff angle
            GLfloat cosCutoff = (GLfloat)cos((double)L.spotCutoff * 3.14159265f / 180.0f);
            // Direction from light to vertex = -L direction
            GLfloat sx = -lx, sy = -ly, sz = -lz;
            // Dot with spot direction (already normalised)
            GLfloat cosAngle = sx * L.spotDirection[0]
                             + sy * L.spotDirection[1]
                             + sz * L.spotDirection[2];
            if (cosAngle < cosCutoff)
            {
                spotFactor = 0.0f; // outside cone
            }
            else
            {
                spotFactor = SafePow(cosAngle, L.spotExponent);
            }
        }

        GLfloat effectAtten = attenuation * spotFactor;

        // ---- Ambient contribution from this light ----
        r += effectAtten * materialAmbient[0] * L.ambient[0];
        g += effectAtten * materialAmbient[1] * L.ambient[1];
        b += effectAtten * materialAmbient[2] * L.ambient[2];

        // ---- Diffuse ----
        GLfloat ndotl = nx*lx + ny*ly + nz*lz;
        if (ndotl < 0.0f) ndotl = 0.0f;

        r += effectAtten * ndotl * materialDiffuse[0] * L.diffuse[0];
        g += effectAtten * ndotl * materialDiffuse[1] * L.diffuse[1];
        b += effectAtten * ndotl * materialDiffuse[2] * L.diffuse[2];

        // ---- Specular (Blinn-Phong) ----
        if (materialShininess > 0.0f && ndotl > 0.0f &&
            (materialSpecular[0] > 0.0f || materialSpecular[1] > 0.0f || materialSpecular[2] > 0.0f))
        {
            // Half-vector: L + V, where V = (0,0,1) in eye space
            GLfloat hx = lx, hy = ly, hz = lz + 1.0f;
            GLfloat hv[3] = {hx,hy,hz}; Normalize3(hv);
            GLfloat ndoth = nx*hv[0] + ny*hv[1] + nz*hv[2];
            if (ndoth < 0.0f) ndoth = 0.0f;
            GLfloat spec = SafePow(ndoth, materialShininess);
            r += effectAtten * spec * materialSpecular[0] * L.specular[0];
            g += effectAtten * spec * materialSpecular[1] * L.specular[1];
            b += effectAtten * spec * materialSpecular[2] * L.specular[2];
        }
    }

    vb.SetColor(D3DRGBA(Clamp01(r), Clamp01(g), Clamp01(b), materialDiffuse[3]));
    return true;
}

// ---------------------------------------------------------------------------
// SetNormal
// ---------------------------------------------------------------------------

void RXGLLightingState::SetNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    currentNormal[0] = nx;
    currentNormal[1] = ny;
    currentNormal[2] = nz;
}

// ---------------------------------------------------------------------------
// SetLightfv / SetLightf
// ---------------------------------------------------------------------------

void RXGLLightingState::SetLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
    int idx = LightIndex(light);
    if (idx < 0 || !params) return;
    RXGLLight& L = lights[idx];
    switch (pname)
    {
    case GL_POSITION:
        L.position[0]=params[0]; L.position[1]=params[1];
        L.position[2]=params[2]; L.position[3]=params[3];
        break;
    case GL_AMBIENT:
        L.ambient[0]=params[0]; L.ambient[1]=params[1];
        L.ambient[2]=params[2]; L.ambient[3]=params[3];
        break;
    case GL_DIFFUSE:
        L.diffuse[0]=params[0]; L.diffuse[1]=params[1];
        L.diffuse[2]=params[2]; L.diffuse[3]=params[3];
        break;
    case GL_SPECULAR:
        L.specular[0]=params[0]; L.specular[1]=params[1];
        L.specular[2]=params[2]; L.specular[3]=params[3];
        break;
    case GL_SPOT_DIRECTION:
        L.spotDirection[0]=params[0]; L.spotDirection[1]=params[1];
        L.spotDirection[2]=params[2];
        // Pre-normalise for efficiency
        {
            GLfloat sd[3]={L.spotDirection[0],L.spotDirection[1],L.spotDirection[2]};
            Normalize3(sd);
            L.spotDirection[0]=sd[0]; L.spotDirection[1]=sd[1]; L.spotDirection[2]=sd[2];
        }
        break;
    case GL_SPOT_EXPONENT:
        L.spotExponent = params[0] < 0.0f ? 0.0f : params[0] > 128.0f ? 128.0f : params[0];
        break;
    case GL_SPOT_CUTOFF:
        L.spotCutoff = params[0];
        break;
    case GL_CONSTANT_ATTENUATION:
        L.constAtten = params[0] < 0.0f ? 0.0f : params[0];
        break;
    case GL_LINEAR_ATTENUATION:
        L.linearAtten = params[0] < 0.0f ? 0.0f : params[0];
        break;
    case GL_QUADRATIC_ATTENUATION:
        L.quadAtten = params[0] < 0.0f ? 0.0f : params[0];
        break;
    }
}

void RXGLLightingState::SetLightf(GLenum light, GLenum pname, GLfloat param)
{
    GLfloat v[4] = {param, param, param, param};
    SetLightfv(light, pname, v);
}

// ---------------------------------------------------------------------------
// SetLightModel
// ---------------------------------------------------------------------------

void RXGLLightingState::SetLightModelfv(GLenum pname, const GLfloat* params)
{
    if (!params) return;
    if (pname == GL_LIGHT_MODEL_AMBIENT)
    {
        lightModelAmbient[0]=params[0]; lightModelAmbient[1]=params[1];
        lightModelAmbient[2]=params[2]; lightModelAmbient[3]=params[3];
    }
}

void RXGLLightingState::SetLightModelf(GLenum pname, GLfloat param)
{
    GLfloat v[4] = {param, param, param, param};
    SetLightModelfv(pname, v);
}

void RXGLLightingState::SetLightModeli(GLenum pname, GLint param)
{
    if (pname == GL_LIGHT_MODEL_COLOR_CONTROL)
        lightModelColorControl = param;
}

// ---------------------------------------------------------------------------
// Material
// ---------------------------------------------------------------------------

void RXGLLightingState::SetMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
    if (!params) return;
    if (face != GL_FRONT && face != GL_BACK && face != GL_FRONT_AND_BACK) return;
    switch (pname)
    {
    case GL_AMBIENT:
        materialAmbient[0]=params[0]; materialAmbient[1]=params[1];
        materialAmbient[2]=params[2]; materialAmbient[3]=params[3];
        break;
    case GL_DIFFUSE:
        materialDiffuse[0]=params[0]; materialDiffuse[1]=params[1];
        materialDiffuse[2]=params[2]; materialDiffuse[3]=params[3];
        break;
    case GL_AMBIENT_AND_DIFFUSE:
        materialAmbient[0]=materialDiffuse[0]=params[0];
        materialAmbient[1]=materialDiffuse[1]=params[1];
        materialAmbient[2]=materialDiffuse[2]=params[2];
        materialAmbient[3]=materialDiffuse[3]=params[3];
        break;
    case GL_SPECULAR:
        materialSpecular[0]=params[0]; materialSpecular[1]=params[1];
        materialSpecular[2]=params[2]; materialSpecular[3]=params[3];
        break;
    case GL_EMISSION:
        materialEmission[0]=params[0]; materialEmission[1]=params[1];
        materialEmission[2]=params[2]; materialEmission[3]=params[3];
        break;
    case GL_SHININESS:
        materialShininess = params[0] < 0.0f ? 0.0f : params[0] > 128.0f ? 128.0f : params[0];
        break;
    }
}

void RXGLLightingState::SetMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    if (pname == GL_SHININESS)
    {
        materialShininess = param < 0.0f ? 0.0f : param > 128.0f ? 128.0f : param;
        return;
    }
    GLfloat v[4] = {param, param, param, param};
    SetMaterialfv(face, pname, v);
}

// ---------------------------------------------------------------------------
// ColorMaterial
// ---------------------------------------------------------------------------

void RXGLLightingState::SetColorMaterial(GLenum /*face*/, GLenum /*mode*/)
{
    // Full face/mode tracking would go here; common case is FRONT_AND_BACK
    // AMBIENT_AND_DIFFUSE which UpdateMaterialFromColor already handles.
}

void RXGLLightingState::UpdateMaterialFromColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    materialAmbient[0]=r; materialAmbient[1]=g;
    materialAmbient[2]=b; materialAmbient[3]=a;
    materialDiffuse[0]=r; materialDiffuse[1]=g;
    materialDiffuse[2]=b; materialDiffuse[3]=a;
}

// ---------------------------------------------------------------------------
// Enable / Disable / IsEnabled
// ---------------------------------------------------------------------------

bool RXGLLightingState::EnableDisable(GLenum cap, bool value)
{
    if (cap == GL_LIGHTING)      { lighting      = value; return true; }
    if (cap == GL_NORMALIZE)     { normalize     = value; return true; }
    if (cap == GL_COLOR_MATERIAL){ colorMaterial = value; return true; }

    int idx = LightIndex(cap);
    if (idx >= 0) { lights[idx].enabled = value; return true; }

    return false;
}

GLboolean RXGLLightingState::IsEnabled(GLenum cap) const
{
    if (cap == GL_LIGHTING)      return lighting      ? GL_TRUE : GL_FALSE;
    if (cap == GL_NORMALIZE)     return normalize     ? GL_TRUE : GL_FALSE;
    if (cap == GL_COLOR_MATERIAL)return colorMaterial ? GL_TRUE : GL_FALSE;

    int idx = LightIndex(cap);
    if (idx >= 0) return lights[idx].enabled ? GL_TRUE : GL_FALSE;

    return GL_FALSE;
}
