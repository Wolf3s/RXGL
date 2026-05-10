# RXGL-PC Reference Build

This folder contains the PC reference build of the RXGL test harness.
It uses native Win32/WGL OpenGL -- no shim is involved -- and runs the exact
same test suite as the Xbox build from the `RXGL-Xbox/` folder.

The purpose is to provide a ground-truth reference: if a test looks identical
on both the PC build (real OpenGL) and the Xbox build (RXGL shim), the shim
is producing correct output for that feature.

---

## Project Files

| File | Description |
|---|---|
| `Main.cpp` | Win32/WGL entry point. Creates a window and OpenGL context, loads extension function pointers, and drives the test loop via keyboard input. |
| `RXGLTest.sln` | Visual Studio 2017+ solution file. |
| `RXGLTest.vcxproj` | PC project file (x64). Compiles only `Main.cpp`. Test headers are referenced from `../RXGL-Xbox/` via the include path. |
| `RXGLTest.vcxproj.filters` | IDE filter file grouping test headers and source. |

---

## Building

Open `RXGLTest.sln` in Visual Studio 2017 or later. The project targets x64
and links `opengl32.lib` and `glu32.lib` from the Windows SDK. No XDK or
additional dependencies are required.

The test headers are not copied here -- the project references them directly
from `../RXGL-Xbox/` via `AdditionalIncludeDirectories`. This means editing
a test header in `RXGL-Xbox/` updates both builds automatically.

---

## What Is and Is Not Included

The PC build compiles **only** `Main.cpp`. It does not compile any RXGL shim
source files. All OpenGL calls go directly to the system `opengl32.dll` via
native WGL. Extension functions (`glActiveTextureARB`, `glBlendEquation`,
`glGenerateMipmap`, etc.) are loaded at startup via `wglGetProcAddress`.

Features that are Xbox NV2A extensions have no equivalent in desktop OpenGL
and will not behave identically on PC:

| Feature | Xbox (RXGL) | PC reference |
|---|---|---|
| Logic ops (`glLogicOp`) | D3DRS_LOGICOP NV2A extension | Native OpenGL GL_COLOR_LOGIC_OP |
| `glBlendColor` | D3DRS_BLENDCOLOR NV2A extension | GL 1.4 / ARB_imaging extension |
| Clip planes | Software Sutherland-Hodgman | Hardware clip planes via OpenGL |
| Texgen | Software per-vertex | Hardware TCI via OpenGL |
| Lighting | Software per-vertex | Hardware fixed-function pipeline |
| `glGenerateMipmap` | D3DXFilterTexture | GL_ARB_framebuffer_object extension |

These differences are expected. The tests are designed so that correct output
looks the same on both platforms despite taking different code paths.

---

## Controls

| Key | Action |
|---|---|
| Left / Right arrow | Cycle through tests |
| Up / Down arrow | Step rotation phase (Test 4) |
| **Test 90 only** | |
| L | Toggle lighting on/off |
| F | Cycle texture filter mode (3 modes) |
| **Test 91 only** | |
| L | Toggle lighting on/off |
| **Test 92 only** | |
| W / S (held) | Move object toward / away from camera (Z axis) |
| A / D (held) | Move object left / right (X axis) |
| Q / E (held) | Move object down / up (Y axis) |

The window title bar updates to show the current test number and available
controls for that test.

---

## Test Reference

All 93 tests (indices 0 through 92) are defined in the headers in
`RXGL-Xbox/`. See `RXGL-Xbox/README.md` for the full test reference table.
