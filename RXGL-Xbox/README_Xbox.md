# RXGL-Xbox Test Harness

This folder contains the Xbox XDK test harness for RXGL. It compiles against
the RXGL shim (from the `RXGL/` folder) and runs a suite of 93 tests on
Original Xbox hardware to verify correct shim behaviour.

The same test headers are shared with the PC reference build in `RXGL-PC/`.
When a test looks identical on both platforms it confirms the shim is
producing correct output.

---

## Project Files

| File | Description |
|---|---|
| `Main.cpp` | Xbox XDK entry point. Initialises D3D via RXGL, handles gamepad input, and drives the test loop. |
| `RXGLTest.sln` | Visual Studio 2003 solution file. |
| `RXGLTest.vcproj` | Xbox project file. Compiles all shim sources from `../RXGL/` and all test headers from this folder. |

---

## Building

Open `RXGLTest.sln` in Visual Studio 2003 with the Xbox XDK installed.
The project includes all shim `.cpp` files from `../RXGL/` and adds `../RXGL/`
to the include path automatically. Build configurations available:

| Configuration | Use |
|---|---|
| Debug | Full debug info, runtime checks enabled |
| Release | Optimised build |
| Release_LTCG | Link-time code generation |
| Profile | Instrumented for XDK profiler |
| Profile_FastCap | FastCAP profiling |

---

## Controls

| Input | Action |
|---|---|
| DPad Left / Right | Cycle through tests |
| DPad Up / Down | Step rotation phase (Test 4) |
| **Test 90 only** | |
| A | Toggle lighting on/off |
| B | Cycle texture filter mode (3 modes) |
| **Test 91 only** | |
| A | Toggle lighting on/off |
| **Test 92 only** | |
| A (held) | Move object toward camera (Z+) |
| Y (held) | Move object away from camera (Z-) |
| X (held) | Move object right (X+) |
| B (held) | Move object left (X-) |
| Black (held) | Move object up (Y+) |
| White (held) | Move object down (Y-) |

---

## Test Files

| File | Tests | Coverage |
|---|---|---|
| `Tests_00_to_24.h` | 0-24 | Clear, coordinate orientation, primitive types, depth, face culling, texture orientation, alpha blend, vertex arrays, scissor, colour mask, glTexSubImage2D, texenv modes, fog, stencil, alpha test, depth mask, wrap modes, filters, matrix stack, texture matrix, textured vertex arrays, multitexture, polygon offset, glReadPixels, state queries, display lists |
| `Tests_25_to_49.h` | 25-49 | Polygon mode, viewport, depth range, line width, point size, per-vertex lighting, materials, colour material, texenv add, fog modes, blend modes, scissor, display list ops, glCopyTexSubImage2D, glGetFloatv, specular, glCallLists, glGetError, blend equation, push/pop attrib, texgen, client arrays, multitexture stage 2, glDrawPixels, GL_COMBINE |
| `Tests_50_to_75.h` | 50-74 | Bump-mapped spheres, vertex/colour/normal type variants, rect family, all 14 interleaved array formats, 8-light scene, push/pop client attrib, GL_MIRRORED_REPEAT, clip planes (basic / exhaustive / multi), vertex double variants, rect vector forms, full 8-light with spotlights, multi-plane clipping, full colour variants, texcoord variants, remaining interleaved formats, GL_SUBTRACT/DOT3_RGBA, glCopyTexImage2D, glCopyPixels |
| `Tests_76_to_101.h` | 75-86 | Bitmap font via glDrawPixels, RGBA image cube, new function variants, light/material queries, texcoord type variants, texgen/material/texparameter mixed types, raster position variants, query functions, display list capacity (all 8192 slots + recycle), transpose matrix variants, glBlendColor/glMultiTexCoord3f/glGenerateMipmap, logic ops/texgen readback/texture residency |
| `Tests_pbgl_samples.h` | 87-92 | pbGL sample suite ports: triangle and quad, spinning pyramid and cube, textured cube, lit textured cube, triple draw methods, shadow volume object movement |
| `Tests_Shared.h` | all | Pulls in all test range headers, declares RunTest() and the shared global state used across tests. |

---

## Individual Test Reference

| Test | Name | What it verifies |
|---|---|---|
| 0 | ClearOnly | glClear / glClearColor |
| 1 | CoordinateOrientation | NDC and screen-space coordinate handedness |
| 2 | Primitives | All 10 primitive types |
| 3 | Depth | Depth test with GL_LESS / GL_ALWAYS |
| 4 | CullingCube | GL_CULL_FACE, glFrontFace |
| 5 | TextureOrientation | Texture bottom-left origin |
| 6 | BlendTexture | glBlendFunc with textured quads |
| 7 | VertexArrays | glVertexPointer / glDrawArrays |
| 8 | ScissorColorMask | glScissor, glColorMask |
| 9 | TexSubImage2D | glTexSubImage2D partial update |
| 10 | TexEnvModes | GL_REPLACE / GL_MODULATE / GL_DECAL |
| 11 | Fog | GL_LINEAR fog |
| 12 | Stencil | Stencil write and test |
| 13 | AlphaTest | glAlphaFunc |
| 14 | DepthMask | glDepthMask(GL_FALSE) layering |
| 15 | TextureWrap | GL_REPEAT / GL_CLAMP / GL_CLAMP_TO_EDGE |
| 16 | TextureFilter | GL_NEAREST / GL_LINEAR / mipmapped variants |
| 17 | MatrixStack | glPushMatrix / glPopMatrix |
| 18 | TextureMatrix | GL_TEXTURE matrix mode |
| 19 | TexturedVertexArrays | Vertex arrays with texture coordinates |
| 20 | Multitexture | Two-stage glActiveTextureARB |
| 21 | PolygonOffset | glPolygonOffset depth bias |
| 22 | ReadPixels | glReadPixels round-trip |
| 23 | GetState | glGetIntegerv / glGetFloatv |
| 24 | DisplayListPlaceholder | glNewList / glCallList |
| 25 | PolygonMode | GL_FILL / GL_LINE / GL_POINT |
| 26 | Viewport | glViewport sub-region |
| 27 | DepthRange | glDepthRange |
| 28 | LineWidthPointSize | glLineWidth / glPointSize |
| 29 | LightingNormals | Diffuse lighting with normals |
| 30 | LightMaterial | Specular material and light |
| 31 | ColorMaterial | glColorMaterial tracking |
| 32 | TexEnvAdd | GL_ADD texenv |
| 33 | FogModes | GL_EXP / GL_EXP2 |
| 34 | BlendModes | Multiple glBlendFunc pairs |
| 35 | Scissor | Dynamic scissor region |
| 36 | Viewport | Viewport and projection interaction |
| 37 | DisplayListOps | Nested lists, glListBase |
| 38 | CopyTexSubImage2D | Framebuffer-to-texture copy |
| 39 | GetFloatv | Matrix and state float queries |
| 40 | Specular | Blinn-Phong specular highlight |
| 41 | CallLists | glCallLists with glListBase |
| 42 | GetError | glGetError state machine |
| 43 | BlendEquation | GL_FUNC_ADD / SUBTRACT / REVERSE_SUBTRACT |
| 44 | PushPopAttrib | glPushAttrib / glPopAttrib |
| 45 | TexGen | GL_SPHERE_MAP / GL_EYE_LINEAR |
| 46 | ClientArrays | glEnableClientState per-attrib |
| 47 | Multitexture | Three-stage multitexture |
| 48 | DrawPixels | glDrawPixels RGBA |
| 49 | Combine | GL_COMBINE DOT3 |
| 50 | BumpSpheres | Bump-mapped sphere grid |
| 51 | VertexVariants | glVertex2/3/4 type variants |
| 52 | ColorVariants | glColor3/4 type variants |
| 53 | NormalVariants | glNormal3 type variants |
| 54 | RectFamily | glRectf/i/s/d |
| 55 | VertexVariants3D | 3D vertex signed/unsigned variants |
| 56 | InterleavedArrays | All 14 interleaved formats |
| 57 | MultiLight | 8-light scene |
| 58 | PushPopClientAttrib | GL_CLIENT_VERTEX_ARRAY_BIT save/restore |
| 59 | MirroredRepeat | GL_MIRRORED_REPEAT wrap mode |
| 60 | ClipPlane | Single clip plane |
| 61 | ClipPlaneExhaustive | All 6 clip planes simultaneously |
| 62 | ClipPlaneCube | Clipped rotating cube |
| 63 | VertexDoubles | glVertex*d/dv variants |
| 64 | NormalAndTexCoordVariants | All normal and texcoord type forms |
| 65 | RectVectors | glRect*v vector forms |
| 66 | InterleavedArraysT4F | GL_T4F_V4F / GL_T4F_C4F_N3F_V4F |
| 67 | MultiLightFull | 8 lights with spotlights and attenuation |
| 68 | ClipPlaneMulti | Multi-plane dynamic clip |
| 69 | ColorVariantsFull | All 32 colour type variants |
| 70 | NormalVariants | Extended normal type coverage |
| 71 | TexCoordVariants | All texcoord dimension/type combos |
| 72 | InterleavedArraysRemaining | Remaining interleaved formats |
| 73 | SubtractDot3CopyTex | GL_SUBTRACT / DOT3_RGBA / glCopyTexImage2D |
| 74 | CopyPixels | glCopyPixels GL_COLOR |
| 75 | BitmapFont | Bitmap font rendering via glDrawPixels and raster position |
| 76 | ImageCube | RGBA texture upload on a lit rotating cube |
| 77 | NewFunctions | glLoadMatrixd, glMultMatrixd, glArrayElement, gluPerspective |
| 78 | GetLightMaterial | glGetLightfv / glGetMaterialfv -- all 15 parameters |
| 79 | TexCoordVariants | 1/3/4-component d/i/s/iv/sv/dv texcoord forms |
| 80 | TexGenMaterialTexParam | glTexGend/dv, glMateriali/iv, glTexParameterfv/iv |
| 81 | RasterPosVariants | 2/3/4-component d/dv/fv/i/iv/s/sv raster position forms |
| 82 | QueryFunctions | glGetDoublev, glGetLightiv, glGetMaterialiv, glGetTexEnvfv/iv, glGetPointerv, glIsList, glGetTexLevelParameter |
| 83 | DisplayListCapacity | All 8192 list slots allocated; freed IDs recycled by glGenLists |
| 84 | TransposeMatrix | glLoadTransposeMatrixf/d, glMultTransposeMatrixf/d |
| 85 | BlendColorMultiTexGenMipmap | glBlendColor, glMultiTexCoord3f, glGenerateMipmap |
| 86 | LogicOpGetTexGenResident | glLogicOp XOR/INVERT, glGetTexGenfv readback, glAreTexturesResident |
| 87 | PBGL1 TriangleAndQuad | pbGL sample -- basic triangle and quad |
| 88 | PBGL2 SpinningPyramidCube | pbGL sample -- spinning pyramid and cube |
| 89 | PBGL3 TexturedCube | pbGL sample -- textured rotating cube |
| 90 | PBGL4 LitTexturedCube | pbGL sample -- lit textured cube; A=toggle lighting, B=cycle filter |
| 91 | PBGL5 TripleDrawMethods | pbGL sample -- immediate/arrays/display list paths; A=toggle lighting |
| 92 | PBGL6 ShadowVolumes | pbGL sample -- object movement via face buttons and Black/White |
