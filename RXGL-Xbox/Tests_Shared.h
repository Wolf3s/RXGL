/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_Shared.h  --  Top-level test harness header, included by both Main.cpp
 *                     (Xbox) and Main.cpp (PC). Selects the correct GL header for
 *                     each platform (rxgl_api.h on Xbox, system GL on PC), then
 *                     pulls in all test range headers in order:
 *                       Tests_00_to_24.h   -- original suite (stable)
 *                       Tests_25_to_49.h   -- extended suite
 *                       Tests_50_to_75.h   -- bump, vertex variants, clip planes
 *                       Tests_76_to_101.h  -- bitmap font, image textures
 *                       Tests_pbgl_samples.h -- pbGL / NeHe sample ports
 *                     Also declares RunTest() and GetTestCount() used by Main.cpp.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

#ifdef _XBOX
#include "rxgl_api.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
// ARB extension constants not in gl.h on Windows SDK
#ifndef GL_MAX_TEXTURE_UNITS
#define GL_MAX_TEXTURE_UNITS        0x84E2
#endif
#ifndef GL_TEXTURE0_ARB
#define GL_TEXTURE0_ARB             0x84C0
#define GL_TEXTURE1_ARB             0x84C1
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE            0x812F
#endif
#ifndef GL_COLOR_LOGIC_OP
#define GL_COLOR_LOGIC_OP         0x0BF2
#endif
#ifndef GL_XOR
#define GL_XOR                    0x1506
#define GL_INVERT                 0x150A
#define GL_COPY                   0x1503
#define GL_CLEAR                  0x1500
#define GL_AND                    0x1501
#define GL_OR                     0x1507
#define GL_NOOP                   0x1505
#define GL_NAND                   0x150E
#define GL_NOR                    0x1508
#define GL_SET                    0x150F
#endif
#ifndef GL_CONSTANT_COLOR
#define GL_CONSTANT_COLOR             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR   0x8002
#define GL_CONSTANT_ALPHA             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA   0x8004
#endif
#ifndef GL_MIRRORED_REPEAT
#define GL_MIRRORED_REPEAT          0x8370
#endif
#ifndef GL_CLIENT_VERTEX_ARRAY_BIT
#define GL_CLIENT_VERTEX_ARRAY_BIT  0x00000002
#define GL_CLIENT_PIXEL_STORE_BIT   0x00000001
#define GL_CLIENT_ALL_ATTRIB_BITS   0xFFFFFFFF
#endif
#ifndef GL_LIGHT1
#define GL_LIGHT1                   0x4001
#define GL_LIGHT2                   0x4002
#define GL_LIGHT3                   0x4003
#define GL_LIGHT4                   0x4004
#define GL_LIGHT5                   0x4005
#define GL_LIGHT6                   0x4006
#define GL_LIGHT7                   0x4007
#endif
#ifndef GL_CONSTANT_ATTENUATION
#define GL_CONSTANT_ATTENUATION     0x1207
#define GL_LINEAR_ATTENUATION       0x1208
#define GL_QUADRATIC_ATTENUATION    0x1209
#define GL_SPOT_DIRECTION           0x1204
#define GL_SPOT_EXPONENT            0x1205
#define GL_SPOT_CUTOFF              0x1206
#endif
#ifndef GL_LIGHT_MODEL_AMBIENT
#define GL_LIGHT_MODEL_AMBIENT      0x0B53
#endif
#ifndef GL_POLYGON_OFFSET_FACTOR
#define GL_POLYGON_OFFSET_FACTOR    0x8038
#define GL_POLYGON_OFFSET_UNITS     0x2A00
#endif
#ifndef GL_FUNC_ADD
#define GL_FUNC_ADD                     0x8006
#define GL_FUNC_SUBTRACT                0x800A
#define GL_FUNC_REVERSE_SUBTRACT        0x800B
#define GL_MIN                          0x8007
#define GL_MAX                          0x8008
#define GL_BLEND_EQUATION               0x8009
#endif
#ifndef GL_TEXTURE_GEN_S
#define GL_TEXTURE_GEN_S                0x0C60
#define GL_TEXTURE_GEN_T                0x0C61
#define GL_TEXTURE_GEN_R                0x0C62
#define GL_TEXTURE_GEN_Q                0x0C63
#define GL_TEXTURE_GEN_MODE             0x2500
#define GL_OBJECT_PLANE                 0x2501
#define GL_EYE_PLANE                    0x2502
#define GL_OBJECT_LINEAR                0x2401
#define GL_EYE_LINEAR                   0x2400
#define GL_SPHERE_MAP                   0x2402
#define GL_NORMAL_MAP                   0x8511
#define GL_REFLECTION_MAP               0x8512
#define GL_S                            0x2000
#define GL_T                            0x2001
#define GL_R                            0x2002
#define GL_Q                            0x2003
#endif
#ifndef GL_2_BYTES
#define GL_2_BYTES                  0x1407
#define GL_3_BYTES                  0x1408
#define GL_4_BYTES                  0x1409
#endif
#ifndef GL_SPECULAR
#define GL_SPECULAR                 0x1202
#define GL_EMISSION                 0x1600
#define GL_SHININESS                0x1601
#endif
#ifndef GL_COMBINE
#define GL_COMBINE                  0x8570
#define GL_COMBINE_RGB              0x8571
#define GL_COMBINE_ALPHA            0x8572
#define GL_SOURCE0_RGB              0x8580
#define GL_SOURCE1_RGB              0x8581
#define GL_SOURCE2_RGB              0x8582
#define GL_SOURCE0_ALPHA            0x8588
#define GL_SOURCE1_ALPHA            0x8589
#define GL_SOURCE2_ALPHA            0x858A
#define GL_OPERAND0_RGB             0x8590
#define GL_OPERAND1_RGB             0x8591
#define GL_OPERAND2_RGB             0x8592
#define GL_OPERAND0_ALPHA           0x8598
#define GL_OPERAND1_ALPHA           0x8599
#define GL_OPERAND2_ALPHA           0x859A
#define GL_RGB_SCALE                0x8573
#define GL_CONSTANT                 0x8576
#define GL_PRIMARY_COLOR            0x8577
#define GL_PREVIOUS                 0x8578
#define GL_ADD_SIGNED               0x8574
#define GL_INTERPOLATE              0x8575
#define GL_SUBTRACT                 0x84E7
#define GL_DOT3_RGB                 0x86AE
#define GL_DOT3_RGBA                0x86AF
#endif
// glWindowPos ARB extension -- loaded via wglGetProcAddress in Main_pc.cpp
typedef void (WINAPI* PFNGLWINDOWPOS2FARBPROC)(GLfloat x, GLfloat y);
typedef void (WINAPI* PFNGLWINDOWPOS2IARBPROC)(GLint x, GLint y);
extern PFNGLWINDOWPOS2FARBPROC glWindowPos2fARB;
extern PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB;
static inline void glWindowPos2f(GLfloat x, GLfloat y) { if (glWindowPos2fARB) glWindowPos2fARB(x,y); }
static inline void glWindowPos2i(GLint x, GLint y)     { if (glWindowPos2iARB) glWindowPos2iARB(x,y); }
// GL 1.3/1.4 extensions -- function pointers defined in Main_pc.cpp
typedef void (WINAPI* PFNGLBLENDCOLORPROC2)(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void (WINAPI* PFNGLGENERATEMIPMAPPROC2)(GLenum target);
typedef void (WINAPI* PFNGLLOADTRANSPOSEMATRIXFPROC2)(const GLfloat* m);
typedef void (WINAPI* PFNGLLOADTRANSPOSEMATRIXDPROC2)(const GLdouble* m);
typedef void (WINAPI* PFNGLMULTTRANSPOSEMATRIXFPROC2)(const GLfloat* m);
typedef void (WINAPI* PFNGLMULTTRANSPOSEMATRIXDPROC2)(const GLdouble* m);
typedef void (WINAPI* PFNGLMULTITEXCOORD3FPROC2)(GLenum t, GLfloat s, GLfloat u, GLfloat r);
typedef void (WINAPI* PFNGLMULTITEXCOORD4FPROC2)(GLenum t, GLfloat s, GLfloat u, GLfloat r, GLfloat q);
extern PFNGLBLENDCOLORPROC2           glBlendColor;
extern PFNGLGENERATEMIPMAPPROC2        glGenerateMipmap;
extern PFNGLLOADTRANSPOSEMATRIXFPROC2  glLoadTransposeMatrixf;
extern PFNGLLOADTRANSPOSEMATRIXDPROC2  glLoadTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC2  glMultTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXDPROC2  glMultTransposeMatrixd;
extern PFNGLMULTITEXCOORD3FPROC2       glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD4FPROC2       glMultiTexCoord4f;
#endif // _XBOX
// GL 1.2 light model (tests + PC gl.h may omit)
#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL    0x81F8
#endif
#ifndef GL_SINGLE_COLOR
#define GL_SINGLE_COLOR                 0x81F9
#endif
#ifndef GL_SEPARATE_SPECULAR_COLOR
#define GL_SEPARATE_SPECULAR_COLOR      0x81FA
#endif
#ifndef PI_F
#define PI_F 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI_F / 180.0f)
#endif

#include <math.h>

// Small repeating UV scroll -- avoids huge texcoords losing fractional precision on PC (breaks DOT3 detail).
static inline float wrap_tex_scroll(float t)
{
    const double period = 16.0;
    double r = fmod((double)t, period);
    if (r < 0.0) r += period;
    return (float)r;
}

static int   g_test = 0;
static int   g_maxTest = 92;
static float g_time = 0.0f;
static DWORD g_lastTick = 0;
static int   g_rotPhase = 0;

// Shared state for pbgl tests driven by Main.cpp input handling
static int   g_pbgl4_light  = 1;    // test 90: 1=lighting on
static int   g_pbgl4_filter = 1;    // test 90: 0=nearest 1=linear 2=mipmap
static int   g_pbgl5_light  = 1;    // test 91: 1=lighting on
static float g_pbgl6_objPos[3] = { -4.0f, -2.0f, -10.0f }; // test 92: object position

static GLuint g_checkerTex = 0;
static GLuint g_rgbaTex = 0;
static GLuint g_dynamicTex = 0;
static GLuint g_alphaTex = 0;

#define TEX_SIZE 64
#define BUMP_TEX_SIZE 128
static GLubyte g_checkerData[TEX_SIZE * TEX_SIZE * 4];
static GLubyte g_rgbaData[4 * 4 * 4];
static GLubyte g_dynamicData[TEX_SIZE * TEX_SIZE * 4];
static GLubyte g_alphaData[TEX_SIZE * TEX_SIZE * 4];
static GLubyte g_subData[16 * 16 * 4];

static int g_subFrame = -1;
static bool g_readPixelsPrinted = false;
static bool g_getPrinted = false;
static bool g_copyDone = false;
static bool g_errorTestDone = false;
static GLuint g_copyTex = 0;
static GLuint g_whiteTex = 0;
static GLuint g_bumpNormalTex = 0;

static float sinf_approx(float x)
{
    while (x > PI_F)  x -= 2.0f * PI_F;
    while (x < -PI_F) x += 2.0f * PI_F;
    float sign = x < 0.0f ? -1.0f : 1.0f;
    float ax = x * sign;
    return sign * (16.0f * ax * (PI_F - ax)) /
        (5.0f * PI_F * PI_F - 4.0f * ax * (PI_F - ax));
}

static float cosf_approx(float x) { return sinf_approx(x + PI_F * 0.5f); }

static float sqrtf_approx(float x)
{
    if (x <= 0.0f) return 0.0f;
    float r = x;
    r = 0.5f * (r + x / r);
    r = 0.5f * (r + x / r);
    r = 0.5f * (r + x / r);
    return r;
}

static void SetGL2D(float w, float h)
{
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0.0f, w, 0.0f, h, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

static void SetGLPerspective(float fovDeg, float aspect, float zn, float zf)
{
    float half = fovDeg * DEG2RAD * 0.5f;
    float top = zn * sinf_approx(half) / cosf_approx(half);
    float right = top * aspect;
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glFrustum(-right, right, -top, top, zn, zf);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

static void FillDynamicBase()
{
    for (int y = 0; y < TEX_SIZE; y++)
        for (int x = 0; x < TEX_SIZE; x++)
        {
            GLubyte* p = &g_dynamicData[(y * TEX_SIZE + x) * 4];
            int border  = (x==0||y==0||x==TEX_SIZE-1||y==TEX_SIZE-1);
            int checker = ((x/8)+(y/8))&1;
            if (border)        { p[0]=255;p[1]=255;p[2]=255;p[3]=255; }
            else if (checker)  { p[0]=40; p[1]=40; p[2]=40; p[3]=255; }
            else               { p[0]=10; p[1]=10; p[2]=10; p[3]=255; }
        }
}

static void MakeTextures()
{
    for (int y=0;y<TEX_SIZE;y++) for (int x=0;x<TEX_SIZE;x++)
    {
        int cell=((x/8)+(y/8))&1; GLubyte* p=&g_checkerData[(y*TEX_SIZE+x)*4];
        if(cell){p[0]=255;p[1]=255;p[2]=255;p[3]=255;}
        else    {p[0]=30; p[1]=30; p[2]=30; p[3]=255;}
    }
    glGenTextures(1,&g_checkerTex); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,4,TEX_SIZE,TEX_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,g_checkerData);

    for(int i=0;i<4*4*4;i++) g_rgbaData[i]=0;
    for(int y=0;y<4;y++) for(int x=0;x<4;x++)
    {
        GLubyte* p=&g_rgbaData[(y*4+x)*4];
        if(x<2&&y<2)        {p[0]=255;p[1]=0;  p[2]=0;  }
        else if(x>=2&&y<2)  {p[0]=0;  p[1]=255;p[2]=0;  }
        else if(x<2&&y>=2)  {p[0]=0;  p[1]=0;  p[2]=255;}
        else                {p[0]=255;p[1]=255;p[2]=0;  }
        p[3]=255;
    }
    glGenTextures(1,&g_rgbaTex); glBindTexture(GL_TEXTURE_2D,g_rgbaTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,4,4,4,0,GL_RGBA,GL_UNSIGNED_BYTE,g_rgbaData);

    FillDynamicBase();
    glGenTextures(1,&g_dynamicTex); glBindTexture(GL_TEXTURE_2D,g_dynamicTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,4,TEX_SIZE,TEX_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,g_dynamicData);

    for(int y=0;y<TEX_SIZE;y++) for(int x=0;x<TEX_SIZE;x++)
    {
        GLubyte* p=&g_alphaData[(y*TEX_SIZE+x)*4];
        int dx=x-TEX_SIZE/2, dy=y-TEX_SIZE/2;
        int dist2=dx*dx+dy*dy;
        p[0]=255;p[1]=255;p[2]=255;
        p[3]=(dist2<18*18)?255:0;
    }
    glGenTextures(1,&g_alphaTex); glBindTexture(GL_TEXTURE_2D,g_alphaTex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,4,TEX_SIZE,TEX_SIZE,0,GL_RGBA,GL_UNSIGNED_BYTE,g_alphaData);

    // Bump-map test: 128^2 dual-frequency height -> normals (Test 50). Vertex colour ?? white tex for diffuse.
    static GLubyte bumpNormal[BUMP_TEX_SIZE * BUMP_TEX_SIZE * 4];
    static float bumpH[BUMP_TEX_SIZE * BUMP_TEX_SIZE];
    // Broad ripples + finer detail layer; blend normals toward +Z so valleys are not clamp-black in DOT3.
    const float bumpFreqLo = PI_F * 22.0f;
    const float bumpFreqHi = PI_F * 56.0f;
    const float detailWeight = 0.26f;
    const float bumpStrength = 0.52f;
    const float ambNormalLift = 0.32f;

    for (int y = 0; y < BUMP_TEX_SIZE; y++)
        for (int x = 0; x < BUMP_TEX_SIZE; x++)
        {
            float u = (float)x / (float)BUMP_TEX_SIZE;
            float v = (float)y / (float)BUMP_TEX_SIZE;
            float lo = sinf_approx(u * bumpFreqLo) * sinf_approx(v * bumpFreqLo);
            float hi = sinf_approx(u * bumpFreqHi) * sinf_approx(v * bumpFreqHi);
            bumpH[y * BUMP_TEX_SIZE + x] = lo + detailWeight * hi;
        }
    for (int y = 0; y < BUMP_TEX_SIZE; y++)
        for (int x = 0; x < BUMP_TEX_SIZE; x++)
        {
            int xm = (x > 0) ? x - 1 : x;
            int xp = (x < BUMP_TEX_SIZE - 1) ? x + 1 : x;
            int ym = (y > 0) ? y - 1 : y;
            int yp = (y < BUMP_TEX_SIZE - 1) ? y + 1 : y;
            float dhds = (bumpH[y * BUMP_TEX_SIZE + xm] - bumpH[y * BUMP_TEX_SIZE + xp]) * 0.5f * (float)BUMP_TEX_SIZE * bumpStrength;
            float dhdt = (bumpH[ym * BUMP_TEX_SIZE + x] - bumpH[yp * BUMP_TEX_SIZE + x]) * 0.5f * (float)BUMP_TEX_SIZE * bumpStrength;
            float nx = -dhds;
            float ny = -dhdt;
            float nz = 1.0f;
            float il = sqrtf_approx(nx * nx + ny * ny + nz * nz);
            nx /= il;
            ny /= il;
            nz /= il;
            float ox = nx, oy = ny, oz = nz;
            nx = ox * (1.0f - ambNormalLift) + 0.0f * ambNormalLift;
            ny = oy * (1.0f - ambNormalLift) + 0.0f * ambNormalLift;
            nz = oz * (1.0f - ambNormalLift) + 1.0f * ambNormalLift;
            il = sqrtf_approx(nx * nx + ny * ny + nz * nz);
            nx /= il;
            ny /= il;
            nz /= il;
            GLubyte* pn = &bumpNormal[(y * BUMP_TEX_SIZE + x) * 4];
            pn[0] = (GLubyte)((nx * 0.5f + 0.5f) * 255.0f);
            pn[1] = (GLubyte)((ny * 0.5f + 0.5f) * 255.0f);
            pn[2] = (GLubyte)((nz * 0.5f + 0.5f) * 255.0f);
            pn[3] = 255;
        }
    static GLubyte whiteRGBA[4] = { 255, 255, 255, 255 };
    glGenTextures(1, &g_whiteTex);
    glBindTexture(GL_TEXTURE_2D, g_whiteTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteRGBA);

    glGenTextures(1, &g_bumpNormalTex);
    glBindTexture(GL_TEXTURE_2D, g_bumpNormalTex);
    // LINEAR at 128^2 avoids chunky NEAREST blocks; detail comes from resolution + dual-frequency height.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, BUMP_TEX_SIZE, BUMP_TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, bumpNormal);
}

#include "Tests_00_to_24.h"
#include "Tests_25_to_49.h"
#include "Tests_50_to_75.h"
#include "Tests_76_to_101.h"
#include "Tests_pbgl_samples.h"

// ---------------------------------------------------------------------------
// ResetGLTestState -- hard reset called before every test.
// Slams all GL state back to known defaults so no test can pollute the next.
// ---------------------------------------------------------------------------
static void ResetGLTestState()
{
    glFlush();

    // Viewport / matrices
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_TEXTURE);    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    // Multitexture -- walk down from unit 3 to 0, clean each
    int tu;
    for (tu = 3; tu >= 0; --tu)
    {
        glActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tu));
        glClientActiveTextureARB((GLenum)(GL_TEXTURE0_ARB + tu));
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);

    // Client arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Clip planes
    {
        int cp;
        for (cp = 0; cp < 6; ++cp)
            glDisable((GLenum)(GL_CLIP_PLANE0 + cp));
    }

    // Main caps
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    glDisable(GL_LIGHT4);
    glDisable(GL_LIGHT5);
    glDisable(GL_LIGHT6);
    glDisable(GL_LIGHT7);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_NORMALIZE);
    glDisable(GL_FOG);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_COLOR_LOGIC_OP);

    // Render state defaults
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonOffset(0.0f, 0.0f);
    glLineWidth(1.0f);
    glPointSize(1.0f);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0xFFFFFFFF);
    glLogicOp(GL_COPY);

    // glBlendEquation is an extension pointer on PC -- guard against null
#ifdef _XBOX
    glBlendEquation(GL_FUNC_ADD);
#else
    if (glBlendEquation) glBlendEquation(GL_FUNC_ADD);
#endif

    // Lighting / material defaults
    {
        GLfloat globalAmb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat matAmb[4]    = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat matDiff[4]   = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat black[4]     = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   matAmb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   matDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  black);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  black);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Reset all light parameters to GL defaults.
    // CRITICAL: glDisable(GL_LIGHTi) only turns the light off -- it does NOT
    // reset the light's colour/position. Prior tests set lights to arbitrary
    // colours (red, green, etc.) and those values persist until explicitly
    // overwritten, causing the next test's lighting to be wrong.
    // GL defaults: LIGHT0 = white diffuse+specular, black ambient, pos (0,0,1,0)
    //              LIGHT1-7 = black diffuse+specular+ambient, pos (0,0,1,0)
    {
        GLfloat black[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat white[4]    = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat defPos[4]   = { 0.0f, 0.0f, 1.0f, 0.0f };
        GLfloat defDir[3]   = { 0.0f, 0.0f, -1.0f };
        int li;
        for (li = 0; li < 8; li++)
        {
            GLenum light = (GLenum)(GL_LIGHT0 + li);
            glLightfv(light, GL_AMBIENT,  black);
            glLightfv(light, GL_DIFFUSE,  (li == 0) ? white : black);
            glLightfv(light, GL_SPECULAR, (li == 0) ? white : black);
            glLightfv(light, GL_POSITION, defPos);
            glLightf (light, GL_SPOT_CUTOFF,    180.0f);
            glLightf (light, GL_SPOT_EXPONENT,    0.0f);
            glLightfv(light, GL_SPOT_DIRECTION, defDir);
            glLightf (light, GL_CONSTANT_ATTENUATION,  1.0f);
            glLightf (light, GL_LINEAR_ATTENUATION,    0.0f);
            glLightf (light, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
    }
}

static void RunTest(int test)
{
    ResetGLTestState();
    switch (test)
    {
        case 0:  Test0_ClearOnly();               break;
        case 1:  Test1_CoordinateOrientation();   break;
        case 2:  Test2_Primitives();              break;
        case 3:  Test3_Depth();                   break;
        case 4:  Test4_CullingCube();             break;
        case 5:  Test5_TextureOrientation();      break;
        case 6:  Test6_BlendTexture();            break;
        case 7:  Test7_VertexArrays();            break;
        case 8:  Test8_ScissorColorMask();        break;
        case 9:  Test9_TexSubImage2D();           break;
        case 10: Test10_TexEnvModes();            break;
        case 11: Test11_Fog();                    break;
        case 12: Test12_Stencil();                break;
        case 13: Test13_AlphaTest();              break;
        case 14: Test14_DepthMask();              break;
        case 15: Test15_TextureWrap();            break;
        case 16: Test16_TextureFilter();          break;
        case 17: Test17_MatrixStack();            break;
        case 18: Test18_TextureMatrix();          break;
        case 19: Test19_TexturedVertexArrays();   break;
        case 20: Test20_Multitexture();           break;
        case 21: Test21_PolygonOffset();          break;
        case 22: Test22_ReadPixels();             break;
        case 23: Test23_GetState();               break;
        case 24: Test24_DisplayListPlaceholder(); break;
        case 25: Test25_PolygonMode();            break;
        case 26: Test26_Viewport();               break;
        case 27: Test27_DepthRange();             break;
        case 28: Test28_LineWidthPointSize();     break;
        case 29: Test29_LightingNormals();        break;
        case 30: Test30_LightMaterial();          break;
        case 31: Test31_ColorMaterial();          break;
        case 32: Test32_TexEnvAdd();              break;
        case 33: Test33_FogModes();               break;
        case 34: Test34_BlendModes();             break;
        case 35: Test35_Scissor();                break;
        case 36: Test36_Viewport();               break;
        case 37: Test37_DisplayListOps();         break;
        case 38: Test38_CopyTexSubImage2D();      break;
        case 39: Test39_GetFloatv();              break;
        case 40: Test40_Specular();               break;
        case 41: Test41_CallLists();              break;
        case 42: Test42_GetError();               break;
        case 43: Test43_BlendEquation();          break;
        case 44: Test44_PushPopAttrib();          break;
        case 45: Test45_TexGen();                 break;
        case 46: Test46_ClientArrays();           break;
        case 47: Test47_Multitexture();           break;
        case 48: Test48_DrawPixels();             break;
        case 49: Test49_Combine();                break;
        case 50: Test50_BumpSpheres();           break;
        case 51: Test51_VertexVariants();        break;
        case 52: Test52_ColorVariants();         break;
        case 53: Test53_NormalVariants();        break;
        case 54: Test54_RectFamily();            break;
        case 55: Test55_VertexVariants3D();      break;
        case 56: Test56_InterleavedArrays();     break;
        case 57: Test57_MultiLight();                          break;
        case 58: Test58_PushPopClientAttrib();   break;
        case 59: Test59_MirroredRepeat();        break;
        case 60: Test60_ClipPlane();             break;
        case 61: Test61_ClipPlaneExhaustive();  break;
        case 62: Test62_ClipPlaneCube();         break;
        case 63: Test63_VertexDoubles();           break;
        case 64: Test64_NormalAndTexCoordVariants(); break;
        case 65: Test65_RectVectors();             break;
        case 66: Test66_InterleavedArraysT4F();    break;
        case 67: Test67_MultiLightFull();          break;
        case 68: Test68_ClipPlaneMulti();          break;
        case 69: Test69_ColorVariantsFull();      break;
        case 70: Test70_NormalVariants();          break;
        case 71: Test71_TexCoordVariants();        break;
        case 72: Test72_InterleavedArraysRemaining(); break;
        case 73: Test73_SubtractDot3CopyTex();    break;
        case 74: Test74_CopyPixels();              break;
        case 75: Test75_BitmapFont();             break;
        case 76: Test76_ImageCube();               break;
        case 77: Test77_NewFunctions();          break;
        case 78: Test78_GetLightMaterial();      break;
        case 79: Test79_TexCoordVariants();       break;
        case 80: Test80_TexGenMaterialTexParam();  break;
        case 81: Test81_RasterPosVariants();       break;
        case 82: Test82_QueryFunctions();          break;
        case 83: Test83_DisplayListCapacity();   break;
        case 84: Test84_TransposeMatrix();          break;
        case 85: Test85_BlendColorMultiTexGenMipmap(); break;
        case 86: Test86_LogicOpGetTexGenResident();    break;
        case 87: TestPBGL1_TriangleAndQuad();          break;
        case 88: TestPBGL2_SpinningPyramidCube();      break;
        case 89: TestPBGL3_TexturedCube();             break;
        case 90: TestPBGL4_LitTexturedCube();          break;
        case 91: TestPBGL5_TripleDrawMethods();        break;
        case 92: TestPBGL6_ShadowVolumes();            break;
        default: Test0_ClearOnly();               break;
    }
}