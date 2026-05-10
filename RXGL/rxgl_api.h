/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * rxgl_api.h  --  Public API header. This is the ONLY file user/test code should include.
 *                 Declares all GL 1.x types, constants, and function prototypes exposed
 *                 by the shim, plus the Xbox-specific d3d* helpers and inline GLU stubs.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 *
 * What this covers:
 *   - Full GL 1.x render state (blend, depth, cull, alpha, scissor, fog, stencil)
 *   - Texture management with proper glGenTextures/glDeleteTextures ID tracking
 *   - Matrix stack (modelview, projection, texture) incl. glMultMatrixf
 *   - Vertex submission via Begin/End AND vertex arrays (DrawArrays/Elements)
 *   - ARB + SGIS multitexture extensions via wglGetProcAddress
 *   - State queries: glGetIntegerv, glGetFloatv, glIsEnabled
 *   - Xbox-specific helpers: resolution, gamma, video mode
 */

#ifndef H_RXGL_API
#define H_RXGL_API

#include <xtl.h>

// ---------------------------------------------------------------------------
// OpenGL type definitions
// ---------------------------------------------------------------------------

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef float           GLfloat;
typedef float           GLclampf;
typedef double          GLdouble;
typedef double          GLclampd;
typedef void            GLvoid;

// ---------------------------------------------------------------------------
// OpenGL constants
// (De-duplicated -- one authoritative copy of every define)
// ---------------------------------------------------------------------------

// Boolean
#define GL_FALSE                            0
#define GL_TRUE                             1

// Primitive types
#define GL_POINTS                           0x0000
#define GL_LINES                            0x0001
#define GL_LINE_LOOP                        0x0002
#define GL_LINE_STRIP                       0x0003
#define GL_TRIANGLES                        0x0004
#define GL_TRIANGLE_STRIP                   0x0005
#define GL_TRIANGLE_FAN                     0x0006
#define GL_QUADS                            0x0007
#define GL_QUAD_STRIP                       0x0008
#define GL_POLYGON                          0x0009

// Blending
#define GL_ZERO                             0
#define GL_ONE                              1
#define GL_SRC_COLOR                        0x0300
#define GL_ONE_MINUS_SRC_COLOR              0x0301
#define GL_SRC_ALPHA                        0x0302
#define GL_ONE_MINUS_SRC_ALPHA              0x0303
#define GL_DST_ALPHA                        0x0304
#define GL_ONE_MINUS_DST_ALPHA              0x0305
#define GL_DST_COLOR                        0x0306
#define GL_ONE_MINUS_DST_COLOR              0x0307
#define GL_SRC_ALPHA_SATURATE               0x0308

// Draw buffer modes
#define GL_NONE                             0
#define GL_FRONT_LEFT                       0x0400
#define GL_FRONT_RIGHT                      0x0401
#define GL_BACK_LEFT                        0x0402
#define GL_BACK_RIGHT                       0x0403
#define GL_FRONT                            0x0404
#define GL_BACK                             0x0405
#define GL_LEFT                             0x0406
#define GL_RIGHT                            0x0407
#define GL_FRONT_AND_BACK                   0x0408
#define GL_AUX0                             0x0409
#define GL_AUX1                             0x040A
#define GL_AUX2                             0x040B
#define GL_AUX3                             0x040C

// Test functions
#define GL_NEVER                            0x0200
#define GL_LESS                             0x0201
#define GL_EQUAL                            0x0202
#define GL_LEQUAL                           0x0203
#define GL_GREATER                          0x0204
#define GL_NOTEQUAL                         0x0205
#define GL_GEQUAL                           0x0206
#define GL_ALWAYS                           0x0207

// Stencil ops
#define GL_KEEP                             0x1E00
#define GL_REPLACE                          0x1E01
#define GL_INCR                             0x1E02
#define GL_DECR                             0x1E03
#define GL_CLEAR                            0x1500
#define GL_AND                              0x1501
#define GL_AND_REVERSE                      0x1502
#define GL_COPY                             0x1503
#define GL_AND_INVERTED                     0x1504
#define GL_NOOP                             0x1505
#define GL_XOR                              0x1506
#define GL_OR                               0x1507
#define GL_NOR                              0x1508
#define GL_EQUIV                            0x1509
#define GL_INVERT                           0x150A
#define GL_OR_REVERSE                       0x150B
#define GL_COPY_INVERTED                    0x150C
#define GL_OR_INVERTED                      0x150D
#define GL_NAND                             0x150E
#define GL_SET                              0x150F

// Enable/disable caps
#define GL_FOG                              0x0B60
#define GL_LIGHTING                         0x0B50
#define GL_TEXTURE_1D                       0x0DE0
#define GL_TEXTURE_2D                       0x0DE1
#define GL_CULL_FACE                        0x0B44
#define GL_ALPHA_TEST                       0x0BC0
#define GL_BLEND                            0x0BE2
#define GL_INDEX_LOGIC_OP                   0x0BF1
#define GL_COLOR_LOGIC_OP                   0x0BF2
#define GL_DITHER                           0x0BD0
#define GL_STENCIL_TEST                     0x0B90
#define GL_DEPTH_TEST                       0x0B71
#define GL_SCISSOR_TEST                     0x0C11
#define GL_AUTO_NORMAL                      0x0D80
#define GL_NORMALIZE                        0x0BA1
#define GL_POLYGON_STIPPLE                  0x0B42
#define GL_LINE_STIPPLE                     0x0B24
#define GL_LINE_SMOOTH                      0x0B20
#define GL_POINT_SMOOTH                     0x0B10
#define GL_POLYGON_SMOOTH                   0x0B41
#define GL_COLOR_MATERIAL                   0x0B57

// Fog
#define GL_FOG_DENSITY                      0x0B62
#define GL_FOG_START                        0x0B63
#define GL_FOG_END                          0x0B64
#define GL_FOG_MODE                         0x0B65
#define GL_FOG_COLOR                        0x0B66
#define GL_FOG_INDEX                        0x0B61
#define GL_LINEAR                           0x2601
#define GL_EXP                              0x0800
#define GL_EXP2                             0x0801

// Texture parameters
#define GL_TEXTURE_WIDTH                    0x1000
#define GL_TEXTURE_HEIGHT                   0x1001
#define GL_TEXTURE_INTERNAL_FORMAT          0x1003
#define GL_TEXTURE_BORDER_COLOR             0x1004
#define GL_TEXTURE_BORDER                   0x1005
#define GL_TEXTURE_RED_SIZE                 0x805C
#define GL_TEXTURE_GREEN_SIZE               0x805D
#define GL_TEXTURE_BLUE_SIZE                0x805E
#define GL_TEXTURE_ALPHA_SIZE               0x805F
#define GL_TEXTURE_LUMINANCE_SIZE           0x8060
#define GL_TEXTURE_INTENSITY_SIZE           0x8061
#define GL_TEXTURE_PRIORITY                 0x8066
#define GL_TEXTURE_RESIDENT                 0x8067
#define GL_TEXTURE_BINDING_1D               0x8068
#define GL_TEXTURE_BINDING_2D               0x8069
#define GL_TEXTURE_MIN_FILTER               0x2801
#define GL_TEXTURE_MAG_FILTER               0x2800
#define GL_TEXTURE_WRAP_S                   0x2802
#define GL_TEXTURE_WRAP_T                   0x2803
#define GL_NEAREST                          0x2600
#define GL_NEAREST_MIPMAP_NEAREST           0x2700
#define GL_LINEAR_MIPMAP_NEAREST            0x2701
#define GL_NEAREST_MIPMAP_LINEAR            0x2702
#define GL_LINEAR_MIPMAP_LINEAR             0x2703
#define GL_CLAMP                            0x2900
#define GL_REPEAT                           0x2901
#define GL_CLAMP_TO_EDGE                    0x812F
#define GL_MIRRORED_REPEAT                  0x8370

// Texture env
#define GL_TEXTURE_ENV                      0x2300
#define GL_TEXTURE_ENV_MODE                 0x2200
#define GL_TEXTURE_ENV_COLOR                0x2201
#define GL_MODULATE                         0x2100
#define GL_DECAL                            0x2101

// Matrix modes
#define GL_MATRIX_MODE                      0x0BA0
#define GL_MODELVIEW                        0x1700
#define GL_PROJECTION                       0x1701
#define GL_TEXTURE                          0x1702

// Shade model
#define GL_SHADE_MODEL                      0x0B54
#define GL_FLAT                             0x1D00
#define GL_SMOOTH                           0x1D01

// Polygon modes
#define GL_POINT                            0x1B00
#define GL_LINE                             0x1B01
#define GL_FILL                             0x1B02
#define GL_CULL_FACE_MODE                   0x0B45
#define GL_FRONT_FACE                       0x0B46
#define GL_CW                               0x0900
#define GL_CCW                              0x0901

// Pixel formats
#define GL_BITMAP                           0x1A00
#define GL_COLOR                            0x1800
#define GL_DEPTH                            0x1801
#define GL_STENCIL                          0x1802
#define GL_BGRA                             0x80E1
#define GL_BGR                              0x80E0
#define GL_RGBA                             0x1908
#define GL_RGB                              0x1907
#define GL_ALPHA                            0x1906
#define GL_LUMINANCE                        0x1909
#define GL_LUMINANCE_ALPHA                  0x190A
#define GL_INTENSITY                        0x8049
#define GL_COLOR_INDEX                      0x1900
#define GL_RGBA4                            0x8056
#define GL_RGB5_A1                          0x8057

// Pixel types
#define GL_BYTE                             0x1400
#define GL_UNSIGNED_BYTE                    0x1401
#define GL_SHORT                            0x1402
#define GL_UNSIGNED_SHORT                   0x1403
#define GL_INT                              0x1404
#define GL_UNSIGNED_INT                     0x1405
#define GL_FLOAT                            0x1406
#define GL_UNSIGNED_BYTE_3_3_2              0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4           0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1           0x8034
#define GL_UNSIGNED_INT_8_8_8_8             0x8035
#define GL_UNSIGNED_INT_10_10_10_2          0x8036

// Clear mask bits
#define GL_COLOR_BUFFER_BIT                 0x00004000
#define GL_DEPTH_BUFFER_BIT                 0x00000100
#define GL_STENCIL_BUFFER_BIT               0x00000400
#define GL_ACCUM_BUFFER_BIT                 0x00000200

// GetString names
#define GL_VENDOR                           0x1F00
#define GL_RENDERER                         0x1F01
#define GL_VERSION                          0x1F02
#define GL_EXTENSIONS                       0x1F03

// glGet tokens (subset most useful for Xbox ports)
#define GL_CURRENT_COLOR                    0x0B00
#define GL_CURRENT_NORMAL                   0x0B02
#define GL_CURRENT_TEXTURE_COORDS           0x0B03
#define GL_POINT_SIZE                       0x0B11
#define GL_POINT_SIZE_RANGE                 0x0B12
#define GL_LINE_WIDTH                       0x0B21
#define GL_LINE_WIDTH_RANGE                 0x0B22
#define GL_DEPTH_RANGE                      0x0B70
#define GL_DEPTH_WRITEMASK                  0x0B72
#define GL_DEPTH_CLEAR_VALUE                0x0B73
#define GL_DEPTH_FUNC                       0x0B74
#define GL_STENCIL_CLEAR_VALUE              0x0B91
#define GL_STENCIL_FUNC                     0x0B92
#define GL_STENCIL_VALUE_MASK               0x0B93
#define GL_STENCIL_FAIL                     0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL          0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS          0x0B96
#define GL_STENCIL_REF                      0x0B97
#define GL_STENCIL_WRITEMASK                0x0B98
#define GL_VIEWPORT                         0x0BA2
#define GL_MODELVIEW_STACK_DEPTH            0x0BA3
#define GL_PROJECTION_STACK_DEPTH           0x0BA4
#define GL_MODELVIEW_MATRIX                 0x0BA6
#define GL_PROJECTION_MATRIX                0x0BA7
#define GL_TEXTURE_MATRIX                   0x0BA8
#define GL_ALPHA_TEST_FUNC                  0x0BC1
#define GL_ALPHA_TEST_REF                   0x0BC2
#define GL_BLEND_DST                        0x0BE0
#define GL_BLEND_SRC                        0x0BE1
#define GL_SCISSOR_BOX                      0x0C10
#define GL_COLOR_CLEAR_VALUE                0x0C22
#define GL_COLOR_WRITEMASK                  0x0C23
#define GL_DOUBLEBUFFER                     0x0C32
#define GL_PERSPECTIVE_CORRECTION_HINT      0x0C50
#define GL_UNPACK_SWAP_BYTES                0x0CF0
#define GL_UNPACK_LSB_FIRST                 0x0CF1
#define GL_UNPACK_ROW_LENGTH                0x0CF2
#define GL_UNPACK_SKIP_ROWS                 0x0CF3
#define GL_UNPACK_SKIP_PIXELS               0x0CF4
#define GL_UNPACK_ALIGNMENT                 0x0CF5
#define GL_PACK_SWAP_BYTES                  0x0D00
#define GL_PACK_LSB_FIRST                   0x0D01
#define GL_PACK_ROW_LENGTH                  0x0D02
#define GL_PACK_SKIP_ROWS                   0x0D03
#define GL_PACK_SKIP_PIXELS                 0x0D04
#define GL_PACK_ALIGNMENT                   0x0D05
#define GL_MAX_LIGHTS                       0x0D31
#define GL_MAX_CLIP_PLANES                  0x0D32
#define GL_MAX_TEXTURE_SIZE                 0x0D33
#define GL_MAX_MODELVIEW_STACK_DEPTH        0x0D36
#define GL_MAX_PROJECTION_STACK_DEPTH       0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH          0x0D39
#define GL_MAX_VIEWPORT_DIMS                0x0D3A
#define GL_MAX_TEXTURE_UNITS                0x84E2
#define GL_SUBPIXEL_BITS                    0x0D50
#define GL_RED_BITS                         0x0D52
#define GL_GREEN_BITS                       0x0D53
#define GL_BLUE_BITS                        0x0D54
#define GL_ALPHA_BITS                       0x0D55
#define GL_DEPTH_BITS                       0x0D56
#define GL_STENCIL_BITS                     0x0D57

// Hint modes
#define GL_DONT_CARE                        0x1100
#define GL_FASTEST                          0x1101
#define GL_NICEST                           0x1102

// Texture env add mode
#define GL_ADD                              0x0104

// Polygon offset
#define GL_POLYGON_OFFSET_FILL              0x8037
#define GL_POLYGON_OFFSET_LINE              0x2A02
#define GL_POLYGON_OFFSET_POINT             0x2A01
#define GL_POLYGON_OFFSET_FACTOR            0x8038
#define GL_POLYGON_OFFSET_UNITS             0x2A00

// Display list modes
#ifndef GL_COMPILE
#define GL_COMPILE                          0x1300
#endif
#ifndef GL_COMPILE_AND_EXECUTE
#define GL_COMPILE_AND_EXECUTE              0x1301
#endif

// Lighting
#define GL_LIST_BASE                        0x0B32
#define GL_LIST_INDEX                       0x0B33
#define GL_2_BYTES                          0x1407
#define GL_3_BYTES                          0x1408
#define GL_4_BYTES                          0x1409
#define GL_LIGHT0                           0x4000
#define GL_LIGHT1                           0x4001
#define GL_LIGHT2                           0x4002
#define GL_LIGHT3                           0x4003
#define GL_LIGHT4                           0x4004
#define GL_LIGHT5                           0x4005
#define GL_LIGHT6                           0x4006
#define GL_LIGHT7                           0x4007
#define GL_SPOT_EXPONENT                    0x1205
#define GL_SPOT_CUTOFF                      0x1206
#define GL_CONSTANT_ATTENUATION             0x1207
#define GL_LINEAR_ATTENUATION               0x1208
#define GL_QUADRATIC_ATTENUATION            0x1209
#define GL_SPOT_DIRECTION                   0x1204
#define GL_LIGHT_MODEL_AMBIENT              0x0B53
#define GL_LIGHT_MODEL_LOCAL_VIEWER         0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE             0x0B52
#define GL_LIGHT_MODEL_COLOR_CONTROL        0x81F8
#define GL_SINGLE_COLOR                     0x81F9
#define GL_SEPARATE_SPECULAR_COLOR          0x81FA
#define GL_AMBIENT                          0x1200
#define GL_DIFFUSE                          0x1201
#define GL_SPECULAR                         0x1202
#define GL_POSITION                         0x1203
#define GL_SHININESS                        0x1601
#define GL_EMISSION                         0x1600
#define GL_AMBIENT_AND_DIFFUSE              0x1602

// Vertex array types (for DrawArrays/Elements support)
#define GL_VERTEX_ARRAY                     0x8074
#define GL_NORMAL_ARRAY                     0x8075
#define GL_COLOR_ARRAY                      0x8076
#define GL_TEXTURE_COORD_ARRAY              0x8078
#define GL_VERTEX_ARRAY_SIZE                0x807A
#define GL_VERTEX_ARRAY_TYPE                0x807B
#define GL_VERTEX_ARRAY_STRIDE              0x807C
#define GL_COLOR_ARRAY_SIZE                 0x8081
#define GL_COLOR_ARRAY_TYPE                 0x8082
#define GL_COLOR_ARRAY_STRIDE               0x8083
#define GL_TEXTURE_COORD_ARRAY_SIZE         0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE         0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE       0x808A

// Multitexture (ARB - what modern engines actually probe for)
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1

// Multitexture (SGIS - the old extension used by D3DQuake)
#define GL_TEXTURE0_SGIS                    0x835E
#define GL_TEXTURE1_SGIS                    0x835F

// Xbox-specific D3D passthrough token
#define D3D_TEXTURE_MAXANISOTROPY           0xf70001

// ---------------------------------------------------------------------------
// Function declarations
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

// WGL context management
HGLRC   wglCreateContext(void);
BOOL    wglMakeCurrent(void);
BOOL    wglDeleteContext(void);
void    FakeSwapBuffers(void);
PROC    wglGetProcAddress(LPCSTR s);

// Rasterization
void    glAlphaFunc(GLenum func, GLclampf ref);

// Attrib stack
#define GL_CURRENT_BIT                      0x00000001
#define GL_POINT_BIT                        0x00000002
#define GL_LINE_BIT                         0x00000004
#define GL_POLYGON_BIT                      0x00000008
#define GL_POLYGON_STIPPLE_BIT              0x00000010
#define GL_PIXEL_MODE_BIT                   0x00000020
#define GL_LIGHTING_BIT                     0x00000040
#define GL_FOG_BIT                          0x00000080
// GL_DEPTH_BUFFER_BIT    0x00000100  -- already defined above for glClear
// GL_ACCUM_BUFFER_BIT    0x00000200  -- already defined above
// GL_STENCIL_BUFFER_BIT  0x00000400  -- already defined above
#define GL_VIEWPORT_BIT                     0x00000800
#define GL_TRANSFORM_BIT                    0x00001000
#define GL_ENABLE_BIT                       0x00002000
// GL_COLOR_BUFFER_BIT    0x00004000  -- already defined above for glClear
#define GL_HINT_BIT                         0x00008000
#define GL_EVAL_BIT                         0x00010000
#define GL_LIST_BIT                         0x00020000
#define GL_TEXTURE_BIT                      0x00040000
#define GL_SCISSOR_BIT                      0x00080000
#define GL_ALL_ATTRIB_BITS                  0xFFFFFFFF

void    glPushAttrib(GLbitfield mask);
void    glPopAttrib(void);

// Client attrib stack -- saves/restores vertex array bindings and enable bits
#define GL_CLIENT_PIXEL_STORE_BIT           0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT          0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS           0xFFFFFFFF

void    glPushClientAttrib(GLbitfield mask);
void    glPopClientAttrib(void);
void    glBegin(GLenum mode);
void    glBlendFunc(GLenum sfactor, GLenum dfactor);
void    glBlendEquation(GLenum mode);
void    glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void    glLogicOp(GLenum opcode);

// Blend equation modes
#define GL_FUNC_ADD                         0x8006
#define GL_FUNC_SUBTRACT                    0x800A
#define GL_FUNC_REVERSE_SUBTRACT            0x800B
#define GL_MIN                              0x8007
#define GL_MAX                              0x8008
#define GL_BLEND_EQUATION                   0x8009
void    glClear(GLbitfield mask);
void    glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void    glAccum(GLenum op, GLfloat value);     // stub: no NV2A accumulation buffer
void    glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void    glClearDepth(GLclampd depth);
void    glClearStencil(GLint s);
void    glColor3f(GLfloat red, GLfloat green, GLfloat blue);
void    glColor3fv(const GLfloat *v);
void    glColor3ub(GLubyte red, GLubyte green, GLubyte blue);
void    glColor3ubv(const GLubyte *v);
void    glColor3b(GLbyte red, GLbyte green, GLbyte blue);
void    glColor3bv(const GLbyte *v);
void    glColor3us(GLushort red, GLushort green, GLushort blue);
void    glColor3usv(const GLushort *v);
void    glColor3ui(GLuint red, GLuint green, GLuint blue);
void    glColor3uiv(const GLuint *v);
void    glColor3i(GLint r, GLint g, GLint b);
void    glColor3iv(const GLint *v);
void    glColor3s(GLshort red, GLshort green, GLshort blue);
void    glColor3sv(const GLshort *v);
void    glColor3d(GLdouble red, GLdouble green, GLdouble blue);
void    glColor3dv(const GLdouble *v);
void    glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void    glColor4fv(const GLfloat *v);
void    glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void    glColor4ubv(const GLubyte *v);
void    glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
void    glColor4bv(const GLbyte *v);
void    glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha);
void    glColor4usv(const GLushort *v);
void    glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha);
void    glColor4uiv(const GLuint *v);
void    glColor4i(GLint r, GLint g, GLint b, GLint a);
void    glColor4iv(const GLint *v);
void    glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha);
void    glColor4sv(const GLshort *v);
void    glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
void    glColor4dv(const GLdouble *v);
void    glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void    glCullFace(GLenum mode);
void    glDepthFunc(GLenum func);
void    glDepthMask(GLboolean flag);
void    glDepthRange(GLclampd zNear, GLclampd zFar);
void    glDisable(GLenum cap);
void    glDrawBuffer(GLenum mode);
void    glEnable(GLenum cap);
void    glEnd(void);
void    glFinish(void);
void    glFlush(void);
void    glFogf(GLenum pname, GLfloat param);

// glClipPlane -- Software Sutherland-Hodgman clipping.
// Clip plane equation is stored in eye space (as passed by the app).
// When any plane is enabled, FlushVB() transforms vertices to eye space
// and clips the triangle list before submitting to D3D.
// Supports all 6 clip planes simultaneously.
#define GL_CLIP_PLANE0                      0x3000
#define GL_CLIP_PLANE1                      0x3001
#define GL_CLIP_PLANE2                      0x3002
#define GL_CLIP_PLANE3                      0x3003
#define GL_CLIP_PLANE4                      0x3004
#define GL_CLIP_PLANE5                      0x3005

void    glClipPlane(GLenum plane, const GLdouble* equation);
void    glGetClipPlane(GLenum plane, GLdouble* equation);

// GL_COMBINE texture environment (ARB_texture_env_combine / OpenGL 1.3)
#define GL_COMBINE                          0x8570
#define GL_COMBINE_RGB                      0x8571
#define GL_COMBINE_ALPHA                    0x8572
#define GL_SOURCE0_RGB                      0x8580
#define GL_SOURCE1_RGB                      0x8581
#define GL_SOURCE2_RGB                      0x8582
#define GL_SOURCE0_ALPHA                    0x8588
#define GL_SOURCE1_ALPHA                    0x8589
#define GL_SOURCE2_ALPHA                    0x858A
#define GL_OPERAND0_RGB                     0x8590
#define GL_OPERAND1_RGB                     0x8591
#define GL_OPERAND2_RGB                     0x8592
#define GL_OPERAND0_ALPHA                   0x8598
#define GL_OPERAND1_ALPHA                   0x8599
#define GL_OPERAND2_ALPHA                   0x859A
#define GL_RGB_SCALE                        0x8573
#define GL_ALPHA_SCALE                      0x0D1C  // reuses existing
// Combine sources
#define GL_TEXTURE                          0x1702  // already defined below if not here
#define GL_CONSTANT                         0x8576
#define GL_PRIMARY_COLOR                    0x8577
#define GL_PREVIOUS                         0x8578
// Combine operations (most already defined as blend factors)
// GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA reused
// Combine functions
// GL_REPLACE, GL_MODULATE, GL_ADD already defined
#define GL_ADD_SIGNED                       0x8574
#define GL_INTERPOLATE                      0x8575
#define GL_SUBTRACT                         0x84E7
#define GL_DOT3_RGB                         0x86AE
#define GL_DOT3_RGBA                        0x86AF
#define GL_S                                0x2000
#define GL_T                                0x2001
#define GL_R                                0x2002
#define GL_Q                                0x2003

#define GL_TEXTURE_GEN_S                    0x0C60
#define GL_TEXTURE_GEN_T                    0x0C61
#define GL_TEXTURE_GEN_R                    0x0C62
#define GL_TEXTURE_GEN_Q                    0x0C63

#define GL_TEXTURE_GEN_MODE                 0x2500
#define GL_OBJECT_PLANE                     0x2501
#define GL_EYE_PLANE                        0x2502

#define GL_OBJECT_LINEAR                    0x2401
#define GL_EYE_LINEAR                       0x2400
#define GL_SPHERE_MAP                       0x2402
#define GL_NORMAL_MAP                       0x8511
#define GL_REFLECTION_MAP                   0x8512

void    glTexGeni(GLenum coord, GLenum pname, GLint param);
void    glTexGend(GLenum coord, GLenum pname, GLdouble param);
void    glTexGendv(GLenum coord, GLenum pname, const GLdouble *params);
void    glTexGenf(GLenum coord, GLenum pname, GLfloat param);
void    glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params);
void    glTexGeniv(GLenum coord, GLenum pname, const GLint* params);
void    glFogfv(GLenum pname, const GLfloat *params);
void    glFogi(GLenum pname, GLint param);
void    glFogiv(GLenum pname, const GLint *params);
void    glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void    glHint(GLenum target, GLenum mode);
void    glPolygonMode(GLenum face, GLenum mode);
void    glPolygonOffset(GLfloat factor, GLfloat units);
void    glReadBuffer(GLenum mode);
void    glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

// Raster position and pixel drawing
#define GL_CURRENT_RASTER_POSITION          0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID    0x0B08
#define GL_ZOOM_X                           0x0D16
#define GL_ZOOM_Y                           0x0D17

void    glRasterPos2f(GLfloat x, GLfloat y);
void    glRasterPos2d(GLdouble x, GLdouble y);
void    glRasterPos2dv(const GLdouble *v);
void    glRasterPos2fv(const GLfloat *v);
void    glRasterPos2iv(const GLint *v);
void    glRasterPos2s(GLshort x, GLshort y);
void    glRasterPos2sv(const GLshort *v);
void    glRasterPos2i(GLint x, GLint y);
void    glRasterPos3f(GLfloat x, GLfloat y, GLfloat z);
void    glRasterPos3d(GLdouble x, GLdouble y, GLdouble z);
void    glRasterPos3dv(const GLdouble *v);
void    glRasterPos3fv(const GLfloat *v);
void    glRasterPos3i(GLint x, GLint y, GLint z);
void    glRasterPos3iv(const GLint *v);
void    glRasterPos3s(GLshort x, GLshort y, GLshort z);
void    glRasterPos3sv(const GLshort *v);
void    glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void    glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void    glRasterPos4dv(const GLdouble *v);
void    glRasterPos4fv(const GLfloat *v);
void    glRasterPos4i(GLint x, GLint y, GLint z, GLint w);
void    glRasterPos4iv(const GLint *v);
void    glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w);
void    glRasterPos4sv(const GLshort *v);
void    glWindowPos2f(GLfloat x, GLfloat y);
void    glWindowPos2i(GLint x, GLint y);
void    glWindowPos2fARB(GLfloat x, GLfloat y);
void    glWindowPos2iARB(GLint x, GLint y);
void    glPixelZoom(GLfloat xfactor, GLfloat yfactor);
void    glPixelStorei(GLenum pname, GLint param);
void    glPixelStoref(GLenum pname, GLfloat param);

// glDrawPixels: draws pixel rectangle at current raster position.
// Implemented as a textured quad via a temporary texture.
// Supports: GL_RGBA/GL_RGB/GL_LUMINANCE/GL_LUMINANCE_ALPHA with GL_UNSIGNED_BYTE.
// Other format/type combos are stubbed with a DbgPrint warning.
void    glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);

// glBitmap: STUBBED -- 1-bit mask drawing is complex and rarely needed.
// Logs a one-time warning. Advances raster position by (xmove, ymove).
void    glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig,
                 GLfloat xmove, GLfloat ymove, const GLubyte* bitmap);
void    glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void    glShadeModel(GLenum mode);
void    glStencilFunc(GLenum func, GLint ref, GLuint mask);
void    glStencilMask(GLuint mask);
void    glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
void    glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

// Matrix stack
void    glLoadIdentity(void);
void    glLoadMatrixf(const GLfloat *m);
void    glLoadMatrixd(const GLdouble *m);
void    glMatrixMode(GLenum mode);
void    glMultMatrixf(const GLfloat *m);
void    glMultMatrixd(const GLdouble *m);
void    glLoadTransposeMatrixf(const GLfloat *m);
void    glLoadTransposeMatrixd(const GLdouble *m);
void    glMultTransposeMatrixf(const GLfloat *m);
void    glMultTransposeMatrixd(const GLdouble *m);
void    glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void    glPopMatrix(void);
void    glPushMatrix(void);
void    glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void    glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void    glScalef(GLfloat x, GLfloat y, GLfloat z);
void    glScaled(GLdouble x, GLdouble y, GLdouble z);
void    glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void    glTranslated(GLdouble x, GLdouble y, GLdouble z);

// Vertex (immediate mode)
void    glTexCoord2f(GLfloat s, GLfloat t);
void    glTexCoord2fv(const GLfloat *v);
void    glTexCoord2i(GLint s, GLint t);
void    glTexCoord2s(GLshort s, GLshort t);
void    glTexCoord1f(GLfloat s);
void    glTexCoord3f(GLfloat s, GLfloat t, GLfloat r);
void    glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void    glTexCoord1d(GLdouble s);
void    glTexCoord1dv(const GLdouble *v);
void    glTexCoord1i(GLint s);
void    glTexCoord1iv(const GLint *v);
void    glTexCoord1s(GLshort s);
void    glTexCoord1sv(const GLshort *v);
void    glTexCoord2d(GLdouble s, GLdouble t);
void    glTexCoord3d(GLdouble s, GLdouble t, GLdouble r);
void    glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
void    glTexCoord1fv(const GLfloat *v);
void    glTexCoord3fv(const GLfloat *v);
void    glTexCoord3dv(const GLdouble *v);
void    glTexCoord3s(GLshort s, GLshort t, GLshort r);
void    glTexCoord3sv(const GLshort *v);
void    glTexCoord3i(GLint s, GLint t, GLint r);
void    glTexCoord3iv(const GLint *v);
void    glTexCoord4fv(const GLfloat *v);
void    glTexCoord4dv(const GLdouble *v);
void    glTexCoord4i(GLint s, GLint t, GLint r, GLint q);
void    glTexCoord4iv(const GLint *v);
void    glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q);
void    glTexCoord4sv(const GLshort *v);
void    glTexCoord2iv(const GLint *v);
void    glTexCoord2sv(const GLshort *v);
void    glTexCoord2dv(const GLdouble *v);
void    glVertex2f(GLfloat x, GLfloat y);
void    glEdgeFlag(GLboolean flag);   // stub: no D3D8 per-edge wireframe selection
void    glEdgeFlagv(const GLboolean *flag);
void    glEdgeFlagPointer(GLsizei stride, const GLvoid *pointer);
void    glVertex2fv(const GLfloat *v);
void    glVertex2i(GLint x, GLint y);
void    glVertex2iv(const GLint *v);
void    glVertex2s(GLshort x, GLshort y);
void    glVertex2d(GLdouble x, GLdouble y);
void    glVertex2dv(const GLdouble *v);
void    glVertex2sv(const GLshort *v);
void    glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void    glVertex3fv(const GLfloat *v);
void    glVertex3i(GLint x, GLint y, GLint z);
void    glVertex3iv(const GLint *v);
void    glVertex3s(GLshort x, GLshort y, GLshort z);
void    glVertex3d(GLdouble x, GLdouble y, GLdouble z);
void    glVertex3dv(const GLdouble *v);
void    glVertex3sv(const GLshort *v);
void    glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void    glVertex4fv(const GLfloat *v);
void    glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void    glVertex4dv(const GLdouble *v);
void    glVertex4i(GLint x, GLint y, GLint z, GLint w);
void    glVertex4iv(const GLint *v);
void    glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w);
void    glVertex4sv(const GLshort *v);
void    glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
void    glNormal3fv(const GLfloat *v);
void    glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz);
void    glNormal3bv(const GLbyte *v);
void    glNormal3i(GLint nx, GLint ny, GLint nz);
void    glNormal3iv(const GLint *v);
void    glNormal3s(GLshort nx, GLshort ny, GLshort nz);
void    glNormal3sv(const GLshort *v);
void    glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
void    glNormal3dv(const GLdouble *v);

// Vertex arrays (non-immediate path -- required for most apps)
void    glEnableClientState(GLenum array);
void    glDisableClientState(GLenum array);
void    glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void    glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
void    glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void    glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void    glDrawArrays(GLenum mode, GLint first, GLsizei count);
void    glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void    glArrayElement(GLint i);

// glInterleavedArrays -- decompose a packed interleaved vertex buffer into
// individual gl*Pointer + glEnableClientState / glDisableClientState calls.
// Format tokens (subset that actually appears in real GL 1.x code):
#define GL_V2F                              0x2A20
#define GL_V3F                              0x2A21
#define GL_C4UB_V2F                         0x2A22
#define GL_C4UB_V3F                         0x2A23
#define GL_C3F_V3F                          0x2A24
#define GL_N3F_V3F                          0x2A25
#define GL_C4F_N3F_V3F                      0x2A26
#define GL_T2F_V3F                          0x2A27
#define GL_T4F_V4F                          0x2A28
#define GL_T2F_C4UB_V3F                     0x2A29
#define GL_T2F_C3F_V3F                      0x2A2A
#define GL_T2F_N3F_V3F                      0x2A2B
#define GL_T2F_C4F_N3F_V3F                  0x2A2C
#define GL_T4F_C4F_N3F_V4F                  0x2A2D

void    glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer);

// glRect -- draw axis-aligned rectangle; trivially expands to glBegin/glEnd
void    glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void    glRecti(GLint x1, GLint y1, GLint x2, GLint y2);
void    glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
void    glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
void    glRectfv(const GLfloat *v1, const GLfloat *v2);
void    glRectiv(const GLint *v1, const GLint *v2);
void    glRectsv(const GLshort *v1, const GLshort *v2);
void    glRectdv(const GLdouble *v1, const GLdouble *v2);

// Textures
void    glBindTexture(GLenum target, GLuint texture);
void    glGenerateMipmap(GLenum target);
GLboolean glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences);
void    glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities);
void    glDeleteTextures(GLsizei n, const GLuint *textures);
void    glGenTextures(GLsizei n, GLuint *textures);
GLboolean glIsTexture(GLuint texture);
GLboolean glIsList(GLuint list);
void    glTexEnvf(GLenum target, GLenum pname, GLfloat param);
void    glTexEnvi(GLenum target, GLenum pname, GLint param);
void    glTexEnviv(GLenum target, GLenum pname, const GLint *params);
void    glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
void    glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void    glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels); // stub: no D3D8 1D texture
void    glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
void    glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void    glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void    glTexParameteri(GLenum target, GLenum pname, GLint param);
void    glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params);
void    glTexParameteriv(GLenum target, GLenum pname, const GLint *params);
void    glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void    glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
// glCopyTexImage1D: GL_TEXTURE_1D not supported on Xbox NV2A -- stub only
void    glCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
// glCopyColorTable: colour tables not supported on Xbox NV2A -- stub only
void    glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
void    glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
void    glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

// State queries
void            glGetFloatv(GLenum pname, GLfloat *params);
void            glGetIntegerv(GLenum pname, GLint *params);
void    glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
void    glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params);
void    glGetLightfv(GLenum light, GLenum pname, GLfloat *params);
void    glGetLightiv(GLenum light, GLenum pname, GLint *params);
void    glGetDoublev(GLenum pname, GLdouble *params);
void    glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params);
void    glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params);
void    glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params);
void    glGetTexGeniv(GLenum coord, GLenum pname, GLint *params);
void    glGetTexEnviv(GLenum target, GLenum pname, GLint *params);
void    glGetPointerv(GLenum pname, GLvoid **params);
void    glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);
void    glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
void    glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params);
void    glGetMaterialiv(GLenum face, GLenum pname, GLint *params);
void    glGetTexParameteriv(GLenum target, GLenum pname, GLint *params);
void            glGetBooleanv(GLenum pname, GLboolean *params);
const GLubyte*  glGetString(GLenum name);
GLboolean       glIsEnabled(GLenum cap);
// Constant colour blending (GL 1.4 / Xbox D3DRS_BLENDCOLOR)
#ifndef GL_CONSTANT_COLOR
#define GL_CONSTANT_COLOR             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR   0x8002
#define GL_CONSTANT_ALPHA             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA   0x8004
#endif

// Error codes
#define GL_NO_ERROR                         0
#define GL_INVALID_ENUM                     0x0500
#define GL_INVALID_VALUE                    0x0501
#define GL_INVALID_OPERATION                0x0502
#define GL_STACK_OVERFLOW                   0x0503
#define GL_STACK_UNDERFLOW                  0x0504
#define GL_OUT_OF_MEMORY                    0x0505

GLenum          glGetError(void);

// Xbox / D3D8 specific helpers exposed to app code
int     d3dIsResolutionHD(void);
void    d3dSetMode(int width, int height, int bpp, int zbpp, int vmode);
void    d3dSetGammaRamp(const unsigned char* gammaTable);
void    d3dInitSetForce16BitTextures(int force16bitTextures);
void    d3dHint_GenerateMipMaps(int value);
float   d3dGetD3DDriverVersion(void);
void    d3dEvictTextures(void);

void    glFrontFace(GLenum mode);
void    glPixelStorei(GLenum pname, GLint param);

// Display lists
GLuint  glGenLists(GLsizei range);
void    glNewList(GLuint list, GLenum mode);
void    glEndList(void);
void    glCallList(GLuint list);
void    glCallLists(GLsizei n, GLenum type, const GLvoid* lists);
void    glListBase(GLuint base);
void    glDeleteLists(GLuint list, GLsizei range);

// Fixed-function lighting / misc
void    glLineWidth(GLfloat width);
void    glPointSize(GLfloat size);
void    glLightfv(GLenum light, GLenum pname, const GLfloat* params);
void    glLighti(GLenum light, GLenum pname, GLint param);
void    glLightiv(GLenum light, GLenum pname, const GLint *params);
void    glLightf(GLenum light, GLenum pname, GLfloat param);
void    glLightModelfv(GLenum pname, const GLfloat* params);
void    glLightModelf(GLenum pname, GLfloat param);
void    glLightModeli(GLenum pname, GLint param);
void    glLightModeliv(GLenum pname, const GLint *params);
void    glMaterialfv(GLenum face, GLenum pname, const GLfloat* params);
void    glMaterialf(GLenum face, GLenum pname, GLfloat param);
void    glMateriali(GLenum face, GLenum pname, GLint param);
void    glMaterialiv(GLenum face, GLenum pname, const GLint *params);
void    glColorMaterial(GLenum face, GLenum mode);

// ARB multitexture (also accessible via wglGetProcAddress)
void    glActiveTextureARB(GLenum texture);
void    glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
void    glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r);
void    glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void    glClientActiveTextureARB(GLenum texture);

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------
// GLU stubs (inline -- no separate link dependency)
// ---------------------------------------------------------------------------

// gluPerspective -- implemented as static inline to avoid linkage issues
static inline void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble znear, GLdouble zfar)
{
    double f  = 1.0/tan((fovy*3.14159265358979323846/180.0)*0.5);
    double nf = 1.0/(znear-zfar);
    GLfloat m[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    m[0]=(GLfloat)(f/aspect); m[5]=(GLfloat)f;
    m[10]=(GLfloat)((zfar+znear)*nf); m[11]=-1.0f;
    m[14]=(GLfloat)(2.0*zfar*znear*nf);
    glMultMatrixf(m);
}

#endif // H_RXGL_API
