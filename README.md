# RXGL — OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox

RXGL is a software compatibility layer that translates OpenGL 1.x API calls to
Direct3D 8 for the Original Xbox (NV2A GPU, XDK). It allows engines and
applications written against OpenGL 1.x to run on Xbox hardware with
significantly less porting effort than a full rewrite.

Developed by **Team Resurgent, 2026**.  
Inspired by the original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).

---

## Repository Layout

```
RXGL/           The shim itself -- all source files described in this document
RXGL-Xbox/      Xbox XDK test harness and all test headers (see RXGL-Xbox/README.md)
RXGL-PC/        PC reference build entry point (see RXGL-PC/README.md)
```

---

## How to Integrate

The shim is not a drop-in replacement for `<GL/gl.h>` without some effort.
Here is what is actually required:

**1. Replace the GL header**

In every source file that currently includes `<GL/gl.h>`, replace it with:
```cpp
#include "rxgl_api.h"
```

`rxgl_api.h` defines all GL types, constants, and function declarations.
Do not include any system GL headers alongside it on Xbox.

**2. Add the shim source files to your project**

All `.cpp` files from the `RXGL/` folder must be compiled and linked:
```
rxgl_core.cpp
rxgl_renderstate.cpp
rxgl_matrix.cpp
rxgl_texture.cpp
rxgl_lighting.cpp
rxgl_immediate.cpp
rxgl_arrays.cpp
rxgl_displaylist.cpp
```

**3. Add RXGL/ to your include path**

The shim headers reference each other by filename. Add the `RXGL/` folder
to your compiler's additional include directories.

**4. Link the required XDK libraries**

The shim uses D3D8, D3DX8, and XGraphics internally. Your project must link:
```
d3d8.lib      d3dx8.lib     xgraphics.lib
xapilib.lib   xboxkrnl.lib
```

Debug and profile builds use the suffixed variants (`d3d8d.lib`, `d3dx8d.lib`,
`xgraphicsd.lib`, etc.) as normal for XDK projects.

**5. Replace WGL calls**

If your codebase calls WGL functions to set up an OpenGL context, replace them
with the RXGL equivalents. The shim handles D3D device creation internally:

| WGL call | RXGL equivalent |
|---|---|
| `wglCreateContext(hdc)` | `wglCreateContext()` — no argument, D3D device created internally |
| `wglMakeCurrent(hdc, hglrc)` | `wglMakeCurrent()` — no arguments |
| `wglDeleteContext(hglrc)` | `wglDeleteContext()` — no argument |
| `SwapBuffers(hdc)` | `FakeSwapBuffers()` |
| `wglGetProcAddress(name)` | `wglGetProcAddress(name)` — same signature, returns RXGL extension pointers |

**6. Set video mode before creating the context**

Call `d3dSetMode` before `wglCreateContext` to configure the D3D present
parameters. The shim reads these values when creating the D3D device:
```cpp
d3dSetMode(640, 480, 32, 24, 0);   // width, height, colour depth, Z depth, video mode
d3dInitSetForce16BitTextures(0);    // 0 = 32-bit textures, 1 = force 16-bit
wglCreateContext();
wglMakeCurrent();
```

**7. What you do not need to change**

All `gl*` function calls that correspond to a supported function (see the
Supported GL Functions section below) can remain as-is. The shim provides
C-linkage wrappers with the same signatures as the standard OpenGL 1.x API.
Calls to unsupported functions will produce linker errors, which is the
intended signal that those features need to be stubbed out or worked around
in your codebase.

---

## Architecture

| File | Description |
|---|---|
| `rxgl_api.h` | Public API. All GL type definitions, constants, and function declarations. The only file user code should include. |
| `rxgl_internal.h` | Internal shared definitions, macros, and GL-to-D3D conversion helpers. Not for user code. |
| `rxgl_core.cpp` | Top-level coordinator. Owns the `RXGL` class, D3D device initialisation, all C-callable GL wrapper functions, and the GL-to-D3D enum conversion tables. |
| `rxgl_renderstate.h/.cpp` | Render state cache. Owns alpha test, blend, cull, depth, stencil, scissor, fog, colour mask, polygon mode, polygon offset, shade model, and viewport. Dirty flags prevent redundant `SetRenderState` calls. |
| `rxgl_matrix.h/.cpp` | Matrix stack subsystem. Wraps three `ID3DXMatrixStack` instances (modelview, projection, texture). Dirty flags batch `SetTransform` calls. |
| `rxgl_texture.h/.cpp` | Texture subsystem. Manages `IDirect3DTexture8` objects, GL texture object IDs, format conversion, mipmap generation, stage state, and the `GL_COMBINE` pipeline. |
| `rxgl_lighting.h/.cpp` | Software per-vertex Phong lighting. Supports all 8 GL lights including positional, directional, spotlights, and quadratic attenuation. |
| `rxgl_immediate.h/.cpp` | Immediate mode geometry batcher. Accumulates vertices from `glBegin`/`glEnd` blocks and submits via `DrawPrimitiveUP`. Owns the software Sutherland-Hodgman clip plane implementation. |
| `rxgl_arrays.h/.cpp` | Vertex array subsystem. Handles `glDrawArrays` and `glDrawElements` with full typed pointer support. Routes through the immediate path when lighting is active. |
| `rxgl_displaylist.h/.cpp` | Display list record and playback. Dynamic command array with ID recycling. |

---

## Platform

| Property | Detail |
|---|---|
| Target hardware | Original Xbox -- NVIDIA NV2A GPU |
| Graphics API | Direct3D 8 via XDK |
| Compiler | Visual Studio 2003 (MSVC 7.1), C++03 throughout |
| Max texture size | 2048 x 2048 (NV2A hardware limit) |
| Max texture units | 4 (NV2A hardware limit) |
| Coordinate system | OpenGL convention -- Y-up, right-handed, NDC -1..1 |
| Texture origin | Bottom-left = UV (0,0) matching OpenGL |
| Double precision | All GLdouble variants cast to GLfloat at the call site; D3DXMATRIX is float-only |

---

## Supported GL Functions

### Context and Swap

```
wglCreateContext    wglMakeCurrent    wglDeleteContext
FakeSwapBuffers     wglGetProcAddress
```

### Primitives

```
glBegin    glEnd    glFlush    glFinish
```

Primitive types: `GL_POINTS`, `GL_LINES`, `GL_LINE_STRIP`, `GL_LINE_LOOP`,
`GL_TRIANGLES`, `GL_TRIANGLE_STRIP`, `GL_TRIANGLE_FAN`, `GL_QUADS`,
`GL_QUAD_STRIP`, `GL_POLYGON`.

Quads and polygons are decomposed to triangles before submission to D3D.

### Vertex (Immediate Mode)

All signed/unsigned integer, float, and double variants for 2, 3, and 4 components:

```
glVertex2f/fv/i/iv/s/sv/d/dv
glVertex3f/fv/i/iv/s/sv/d/dv
glVertex4f/fv/i/iv/s/sv/d/dv
```

### Colour

All type variants (b/bv/s/sv/i/iv/ui/uiv/us/usv/ub/ubv/f/fv/d/dv) for
both 3-component and 4-component:

```
glColor3*    glColor4*    glColorMask    glColorMaterial
```

Signed byte/short/int variants map [0, MAX] to [0.0, 1.0]; negatives clamp
to 0.0 per the GL specification.

### Normals

```
glNormal3f/fv/b/bv/s/sv/i/iv/d/dv
```

### Texture Coordinates

All dimension (1/2/3/4) and type (f/fv/d/dv/i/iv/s/sv) variants:

```
glTexCoord1*    glTexCoord2*    glTexCoord3*    glTexCoord4*
glMultiTexCoord2fARB    glMultiTexCoord3f    glMultiTexCoord4f
```

### Matrix Operations

```
glMatrixMode             glLoadIdentity
glPushMatrix             glPopMatrix
glLoadMatrixf            glLoadMatrixd
glMultMatrixf            glMultMatrixd
glLoadTransposeMatrixf   glLoadTransposeMatrixd
glMultTransposeMatrixf   glMultTransposeMatrixd
glTranslatef             glTranslated
glRotatef                glRotated
glScalef                 glScaled
glOrtho                  glFrustum
glViewport               glDepthRange
```

Stack depths: modelview 32, projection 4, texture 4. All double variants
cast to float internally.

### Rectangles

```
glRectf/fv    glRecti/iv    glRects/sv    glRectd/dv
```

Expands to a GL_QUADS begin/end block internally.

### Vertex Arrays

```
glVertexPointer          glNormalPointer
glColorPointer           glTexCoordPointer
glEnableClientState      glDisableClientState
glClientActiveTextureARB
glArrayElement
glDrawArrays             glDrawElements    (GL_UNSIGNED_BYTE / SHORT / INT)
glInterleavedArrays
glPushClientAttrib       glPopClientAttrib
```

Non-zero stride and interleaved structs fully supported. When GL_LIGHTING
is active, glDrawArrays and glDrawElements automatically route through the
immediate path so software lighting runs correctly per vertex.

All 14 glInterleavedArrays formats:

```
GL_V2F              GL_V3F
GL_C4UB_V2F         GL_C4UB_V3F
GL_C3F_V3F          GL_N3F_V3F          GL_C4F_N3F_V3F
GL_T2F_V3F          GL_T4F_V4F
GL_T2F_C4UB_V3F     GL_T2F_C3F_V3F
GL_T2F_N3F_V3F      GL_T2F_C4F_N3F_V3F  GL_T4F_C4F_N3F_V4F
```

### Textures

```
glGenTextures            glDeleteTextures
glBindTexture            glIsTexture
glAreTexturesResident    (always GL_TRUE -- Xbox has unified VRAM)
glPrioritizeTextures     (accepted, no-op on unified VRAM)
glTexImage2D             glTexSubImage2D
glCopyTexImage2D         glCopyTexSubImage2D
glTexImage1D             glTexSubImage1D     (stub -- no 1D texture type in D3D8)
glCopyTexImage1D         glCopyTexSubImage1D (stub)
glGenerateMipmap         (via D3DXFilterTexture; guarded against 1-level textures)
glTexParameterf/fv/i/iv
glTexEnvf/fv/i/iv
glGetTexParameterfv/iv
glGetTexLevelParameterfv/iv
glGetTexImage            (stub -- NV2A texture readback not available via D3D8)
```

**Pixel formats:** `GL_RGBA`, `GL_RGB`, `GL_BGRA`, `GL_BGR`, `GL_LUMINANCE`,
`GL_ALPHA`, `GL_LUMINANCE_ALPHA`

**Internal formats:** `GL_RGBA4`, `GL_RGB5_A1` trigger 16-bit D3D formats;
all others use 32-bit unless `g_force16bitTextures` is set.

**Wrap modes:** `GL_REPEAT`, `GL_CLAMP`, `GL_CLAMP_TO_EDGE`, `GL_MIRRORED_REPEAT`

**Filters:** `GL_NEAREST`, `GL_LINEAR`, `GL_NEAREST_MIPMAP_NEAREST`,
`GL_LINEAR_MIPMAP_NEAREST`, `GL_NEAREST_MIPMAP_LINEAR`, `GL_LINEAR_MIPMAP_LINEAR`

**Env modes:** `GL_REPLACE`, `GL_MODULATE`, `GL_DECAL`, `GL_ADD`, `GL_BLEND`, `GL_COMBINE`

Non-power-of-two textures are automatically padded to the next POT size and UV
coordinates are adjusted. NV2A requires POT dimensions.

### GL_COMBINE Texture Environment

Full GL_ARB_texture_env_combine pipeline:

**RGB/Alpha operations:** `GL_REPLACE`, `GL_MODULATE` (x1/x2/x4),
`GL_ADD`, `GL_ADD_SIGNED` (x1/x2), `GL_SUBTRACT`, `GL_INTERPOLATE`,
`GL_DOT3_RGB`, `GL_DOT3_RGBA`

**Sources:** `GL_TEXTURE`, `GL_CONSTANT`, `GL_PRIMARY_COLOR`, `GL_PREVIOUS`

**Operands:** `GL_SRC_COLOR`, `GL_ONE_MINUS_SRC_COLOR`,
`GL_SRC_ALPHA`, `GL_ONE_MINUS_SRC_ALPHA`

### Texture Coordinate Generation

```
glTexGeni/f/fv/d/dv/iv
glEnable / glDisable:  GL_TEXTURE_GEN_S/T/R/Q
glGetTexGenfv / glGetTexGendv / glGetTexGeniv
```

Modes: `GL_SPHERE_MAP`, `GL_REFLECTION_MAP`, `GL_EYE_LINEAR`,
`GL_OBJECT_LINEAR`, `GL_NORMAL_MAP`. All computed in software with correct
eye-space formulation.

### Multitexture

```
glActiveTextureARB         glClientActiveTextureARB
glMultiTexCoord2fARB       glMultiTexCoord3f       glMultiTexCoord4f
```

Up to 4 simultaneous texture units (NV2A hardware limit).
Also exposed via `wglGetProcAddress` for engines that probe at runtime.

### Lighting

Software per-vertex Phong lighting:

```
glEnable / glDisable:
  GL_LIGHTING    GL_LIGHT0..GL_LIGHT7    GL_NORMALIZE    GL_COLOR_MATERIAL

glLightf/fv       glLighti/iv
glLightModelf/fv  glLightModeli/iv
glMaterialf/fv    glMateriali/iv
glShadeModel      glColorMaterial
glGetLightfv/iv   glGetMaterialfv/iv
```

**Light parameters:** `POSITION`, `DIFFUSE`, `SPECULAR`, `AMBIENT`,
`CONSTANT_ATTENUATION`, `LINEAR_ATTENUATION`, `QUADRATIC_ATTENUATION`,
`SPOT_DIRECTION`, `SPOT_CUTOFF`, `SPOT_EXPONENT`

**Material parameters:** `AMBIENT`, `DIFFUSE`, `SPECULAR`, `EMISSION`,
`SHININESS`, `AMBIENT_AND_DIFFUSE`

All 8 lights (GL_LIGHT0 through GL_LIGHT7) fully supported including positional
lights, directional lights, spotlights, and quadratic attenuation.
`GL_LIGHT_MODEL_AMBIENT`, `GL_LIGHT_MODEL_TWO_SIDE`, and
`GL_LIGHT_MODEL_COLOR_CONTROL` are supported.

### Blending

```
glBlendFunc
```

All standard source and destination factors, including `GL_CONSTANT_COLOR`,
`GL_ONE_MINUS_CONSTANT_COLOR`, `GL_CONSTANT_ALPHA`,
`GL_ONE_MINUS_CONSTANT_ALPHA` (Xbox NV2A D3D8 extension).

```
glBlendEquation    GL_FUNC_ADD / GL_FUNC_SUBTRACT / GL_FUNC_REVERSE_SUBTRACT / GL_MIN / GL_MAX
glBlendColor       sets D3DRS_BLENDCOLOR -- Xbox NV2A extension
```

### Logic Operations

```
glLogicOp
glEnable / glDisable:  GL_COLOR_LOGIC_OP
```

All 16 operations: `CLEAR`, `AND`, `AND_REVERSE`, `COPY`, `AND_INVERTED`,
`NOOP`, `XOR`, `OR`, `NOR`, `EQUIV`, `INVERT`, `OR_REVERSE`,
`COPY_INVERTED`, `OR_INVERTED`, `NAND`, `SET`.

Implemented via `D3DRS_LOGICOP` -- an Xbox NV2A extension. GL opcode enum
values match `D3DLOGICOP_*` values directly. Pending geometry is flushed
before any op change to guarantee correct draw ordering.

### Depth and Stencil

```
glDepthFunc    glDepthMask    glDepthRange    glClearDepth
glStencilFunc  glStencilOp    glStencilMask   glClearStencil
```

### Alpha Test

```
glAlphaFunc
glEnable / glDisable:  GL_ALPHA_TEST
```

### Fog

```
glFogf    glFogi    glFogfv    glFogiv
glEnable / glDisable:  GL_FOG
```

Modes: `GL_LINEAR`, `GL_EXP`, `GL_EXP2`. Parameters: `GL_FOG_COLOR`,
`GL_FOG_DENSITY`, `GL_FOG_START`, `GL_FOG_END`, `GL_FOG_MODE`.

### Render State

```
glEnable / glDisable:
  GL_DEPTH_TEST          GL_BLEND             GL_CULL_FACE
  GL_SCISSOR_TEST        GL_TEXTURE_2D        GL_FOG
  GL_LIGHTING            GL_NORMALIZE         GL_ALPHA_TEST
  GL_STENCIL_TEST        GL_POLYGON_OFFSET_FILL
  GL_COLOR_MATERIAL      GL_COLOR_LOGIC_OP
  GL_DITHER              (accepted, no-op)
  GL_LINE_SMOOTH         (accepted, no-op)
  GL_POINT_SMOOTH        (accepted, no-op)
  GL_POLYGON_SMOOTH      (accepted, no-op)

glCullFace       glFrontFace      glShadeModel
glScissor        glViewport       glColorMask
glPolygonMode    (GL_FILL / GL_LINE / GL_POINT)
glPolygonOffset
glLineWidth      glPointSize
glAlphaFunc
glHint           (accepted, ignored)
```

### Clip Planes

```
glClipPlane      glGetClipPlane
glEnable / glDisable:  GL_CLIP_PLANE0..GL_CLIP_PLANE5
```

Software Sutherland-Hodgman clipping. Up to 6 simultaneous planes. NV2A has
no hardware clip planes accessible via D3D8.

### Attrib Stack

```
glPushAttrib / glPopAttrib
```

Bits saved: `GL_ENABLE_BIT`, `GL_COLOR_BUFFER_BIT`, `GL_DEPTH_BUFFER_BIT`,
`GL_STENCIL_BUFFER_BIT`, `GL_FOG_BIT`, `GL_LIGHTING_BIT`, `GL_CURRENT_BIT`,
`GL_POLYGON_BIT`, `GL_LINE_BIT`, `GL_SCISSOR_BIT`. Stack depth: 16.

```
glPushClientAttrib / glPopClientAttrib    (GL_CLIENT_VERTEX_ARRAY_BIT)
```

Stack depth: 16.

### Framebuffer

```
glClear          glClearColor     glClearDepth     glClearStencil
glColorMask      glDepthMask      glStencilMask
glReadBuffer     glDrawBuffer     (accepted, no-op -- single-buffer model)
```

### Raster and Pixel Operations

```
glRasterPos2/3/4 f/fv/d/dv/i/iv/s/sv
glWindowPos2f    glWindowPos2fARB
glWindowPos2i    glWindowPos2iARB
glPixelZoom
glPixelStorei    (GL_UNPACK_ALIGNMENT, GL_UNPACK_ROW_LENGTH)
glPixelStoref    (accepted, ignored)

glDrawPixels     GL_RGBA/RGB/LUMINANCE/LUMINANCE_ALPHA/ALPHA + GL_UNSIGNED_BYTE
                 Rendered as a textured quad at the current raster position.
                 Pixel zoom applied.

glReadPixels     GL_RGBA/RGB + GL_UNSIGNED_BYTE, reads from D3D back buffer.

glCopyPixels     GL_COLOR: reads back buffer and draws at raster position.
                 GL_DEPTH / GL_STENCIL: stub (NV2A surfaces not CPU-readable via D3D8).

glBitmap         Stub -- raster position advances by (xmove, ymove);
                 no pixels drawn. Logs a one-time debug warning.
```

### Copy Texture

```
glCopyTexImage2D      allocates storage and copies a framebuffer region into a texture
glCopyTexSubImage2D   updates a sub-region of an existing texture from the framebuffer
glCopyTexImage1D      stub -- GL_TEXTURE_1D has no D3D8 equivalent on NV2A
glCopyColorTable      stub -- colour tables not in D3D8
```

### Display Lists

```
glNewList / glEndList    (GL_COMPILE, GL_COMPILE_AND_EXECUTE)
glCallList    glCallLists    glListBase
glGenLists    glDeleteLists
glIsList
```

Commands recordable in a display list: `glBegin/End`, `glVertex2/3f/fv`,
`glColor3/4f/fv/ub/ubv/4ubv`, `glNormal3f`, `glTexCoord2f`, `glLoadIdentity`,
`glTranslatef`, `glRotatef`, `glScalef`, `glMultMatrixf`, `glLoadMatrixf`,
`glPushMatrix/PopMatrix`, `glMatrixMode`, `glOrtho`, `glFrustum`,
`glEnable/Disable`, `glBindTexture`, `glTexEnvf/i`, `glBlendFunc`,
`glBlendEquation`, `glAlphaFunc`, `glDepthFunc`, `glDepthMask`,
`glCullFace`, `glFrontFace`, `glShadeModel`, `glFogf/i/fv`,
`glMaterialfv`, `glLightfv`, `glCallList`.

Maximum 8192 simultaneous list IDs. Freed IDs are recycled by subsequent
`glGenLists` calls.

### State Queries

```
glGetIntegerv    glGetFloatv    glGetDoublev    glGetBooleanv
glGetClipPlane   glGetString    glGetError
glGetLightfv/iv  glGetMaterialfv/iv
glGetTexEnvfv/iv
glGetTexGenfv/dv/iv
glGetTexParameterfv/iv
glGetTexLevelParameterfv/iv
glGetTexImage          (stub -- NV2A readback not available via D3D8)
glGetPointerv          (vertex/normal/colour/texcoord array pointers)
glIsEnabled            glIsTexture    glIsList
glAreTexturesResident  (always GL_TRUE -- Xbox has unified VRAM)
```

`glGetIntegerv` / `glGetFloatv` support: `GL_MODELVIEW_MATRIX`,
`GL_PROJECTION_MATRIX`, `GL_TEXTURE_MATRIX`, `GL_VIEWPORT`, `GL_DEPTH_RANGE`,
`GL_COLOR_CLEAR_VALUE`, `GL_CURRENT_COLOR`, `GL_CURRENT_NORMAL`, `GL_FOG_*`,
`GL_LIGHT_MODEL_AMBIENT`, `GL_ALPHA_TEST_REF`, `GL_POINT_SIZE`, `GL_LINE_WIDTH`,
`GL_POLYGON_OFFSET_FACTOR`, `GL_POLYGON_OFFSET_UNITS`, `GL_BLEND_SRC`,
`GL_BLEND_DST`, `GL_BLEND_EQUATION`, `GL_DEPTH_FUNC`, `GL_SHADE_MODEL`,
`GL_FRONT_FACE`, `GL_MATRIX_MODE`, `GL_STENCIL_*`, `GL_TEXTURE_BINDING_2D`,
`GL_MAX_TEXTURE_SIZE`, `GL_MAX_TEXTURE_UNITS`, `GL_MAX_LIGHTS`,
`GL_MAX_MODELVIEW_STACK_DEPTH`, `GL_MAX_PROJECTION_STACK_DEPTH`,
`GL_MAX_TEXTURE_STACK_DEPTH`, `GL_DEPTH_BITS`, `GL_STENCIL_BITS`,
`GL_RED_BITS`, `GL_GREEN_BITS`, `GL_BLUE_BITS`, `GL_ALPHA_BITS`,
`GL_UNPACK_ALIGNMENT`, `GL_MAX_CLIP_PLANES`

### GLU

```
gluPerspective    (static inline in rxgl_api.h -- no glu32.lib dependency on Xbox)
```

### Xbox-Specific Helpers

RXGL extensions exposed to application code for Xbox-specific setup:

```
d3dSetMode                   set resolution, bit depth, Z depth, and video mode
d3dInitSetForce16BitTextures  force all textures to 16-bit D3D formats
d3dHint_GenerateMipMaps       enable/disable automatic mipmap generation on upload
d3dSetGammaRamp               set the hardware gamma ramp table
d3dGetD3DDriverVersion        query the NV2A driver version string
d3dEvictTextures              evict all managed textures from VRAM
d3dIsResolutionHD             returns non-zero if current mode is 720p or higher
```

### Accepted No-ops

The following functions are accepted and silently ignored:

```
glHint              glFinish              glFlush
glReadBuffer        glDrawBuffer
glAccum             glClearAccum          (no NV2A accumulation buffer)
glEdgeFlag          glEdgeFlagv           glEdgeFlagPointer
glPrioritizeTextures
glPixelStoref
GL_DITHER           GL_LINE_SMOOTH        GL_POINT_SMOOTH      GL_POLYGON_SMOOTH
```

---

## Architecture Notes

### Software Lighting

Lighting is computed per-vertex in C++ using the full Phong model (ambient +
diffuse + specular + emission) before vertices are submitted to D3D. When
`GL_LIGHTING` is enabled, `glDrawArrays` and `glDrawElements` automatically
route through `glBegin` / `glArrayElement` / `glEnd` so lighting runs
correctly per-vertex. The fast direct path is used when lighting is off.

### Software Clip Planes

NV2A does not expose hardware clip planes via D3D8. The implementation:

1. `glClipPlane` stores the plane equation transformed to eye space,
   correctly accounting for the D3D row-vector vs GL column-vector convention.
2. At draw time, Sutherland-Hodgman clipping runs against all active planes
   (up to 6 simultaneously).
3. Clipped triangles are re-submitted as `D3DPT_TRIANGLELIST`.
4. `glEnable/Disable(GL_CLIP_PLANE*)` flushes pending geometry first to
   prevent deferred-flush ordering issues.

### Matrix Architecture

The full GL modelview matrix is submitted as `D3DTS_WORLD` with `D3DTS_VIEW`
set to identity. D3D hardware texture coordinate generation modes require a
split WORLD/VIEW matrix and cannot be used -- all texgen modes are computed in
software. A half-pixel sub-pixel offset is applied to the projection matrix to
align GL rasterisation rules with D3D8 and prevent 1-pixel texture crawl on
screen-aligned quads.

### Texture Dimensions

NV2A requires power-of-two texture dimensions. Non-POT textures passed to
`glTexImage2D` or `glDrawPixels` are automatically padded to the next POT
size and UV coordinates are adjusted accordingly.

### Logic Operations

`D3DRS_LOGICOP` is an Xbox NV2A extension. The `D3DLOGICOP_*` enum values
match GL opcode values (0x1500 through 0x150F) exactly, so the mapping is a
direct cast. Logic ops and alpha blending are mutually exclusive on NV2A --
enabling logic ops implies alpha blending is inactive for that draw.

### 16-bit Texture Mode

`d3dInitSetForce16BitTextures(1)` causes all textures (except those with an
explicitly 16-bit internal format) to be stored in `D3DFMT_R5G6B5` or
`D3DFMT_A4R4G4B4`. This reduces VRAM usage at the cost of colour precision
and is recommended for titles that do not require 32-bit colour.

---

## Known Limitations

| Feature | Status | Notes |
|---|---|---|
| `GL_TEXTURE_1D / 3D` | Stub | No 1D/3D texture type in D3D8 on NV2A |
| Accumulation buffer | No-op | NV2A has no accumulation buffer hardware |
| `glBitmap` | Partial | Raster position advances correctly; no pixels drawn |
| `glDrawPixels` | Partial | GL_UNSIGNED_BYTE only; no pixel-transfer ops |
| `glCopyPixels GL_DEPTH/STENCIL` | Stub | NV2A depth surface not CPU-readable via D3D8 |
| `glGetTexImage` | Stub | NV2A texture readback not available via D3D8 |
| Edge flags | No-op | D3D8 has no per-edge wireframe selection |
| Evaluators | Not implemented | glMap*, glEval* |
| Selection / feedback | Not implemented | glSelectBuffer, glFeedbackBuffer |
| Paletted textures | Not implemented | GL_COLOR_INDEX -- no D3D8 equivalent |
| Imaging pipeline | Not implemented | glConvolutionFilter, glHistogram, etc. |
| Polygon / line stipple | Not implemented | No D3D8 equivalent |
| Pixel transfer ops | Not implemented | glPixelTransfer*, glPixelMap* |
| Double precision | Float internally | D3DXMATRIX is float-only; all doubles cast at the call site |
| Texgen | Software only | Correct but costs approximately 2-3 sqrtf per vertex for sphere/reflection maps |
| Display list IDs | Max 8192 | Freed IDs recycled by glGenLists |
| Logic ops | Xbox only | D3DRS_LOGICOP is an NV2A extension; not available on PC |
| `glGenerateMipmap` | Guarded | No-op on textures created with only 1 mip level to prevent VRAM overrun on NV2A |
