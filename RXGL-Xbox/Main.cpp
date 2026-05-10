/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * RXGL-Xbox/Main.cpp  --  Original Xbox XDK entry point and bootstrap for the RXGL
 *                         test harness. Initialises the XDK D3D device via the RXGL
 *                         wglCreateContext / wglMakeCurrent wrappers, sets up gamepad
 *                         input using XInputOpen, and runs the test loop. Individual
 *                         test functions are defined in Tests_Shared.h and the
 *                         Tests_NN_to_NN.h files; this file just drives switching
 *                         between them via DPad Left / Right.
 *
 * Controls (global):
 *   DPad Left / Right      = cycle through tests
 *   DPad Up / Down         = step rotation phase (Test 4)
 *
 * Controls (Test 90 - pbgl4 lighting):
 *   A                      = toggle lighting on/off
 *   B                      = cycle texture filter mode (3 modes)
 *
 * Controls (Test 91 - pbgl5 lighting):
 *   A                      = toggle lighting on/off
 *
 * Controls (Test 92 - pbgl6 object move):
 *   A / Y (held)           = move object toward / away from camera (Z axis)
 *   X / B (held)           = move object right / left (X axis)
 *   Black / White (held)   = move object up / down (Y axis)
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

#include <xtl.h>
#include <stdio.h>
#include "rxgl_api.h"
#include "Tests_Shared.h"


// ------------------------------------------------------------
// Input (Xbox XDK)
// ------------------------------------------------------------

static HANDLE       g_padHandle = NULL;
static XINPUT_STATE g_pad;
static XINPUT_STATE g_prevPad;

static void InitInput()
{
    XDEVICE_PREALLOC_TYPE prealloc[1];
    prealloc[0].DeviceType     = XDEVICE_TYPE_GAMEPAD;
    prealloc[0].dwPreallocCount = 4;
    XInitDevices(1, prealloc);

    DWORD mask = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for (DWORD port = 0; port < 4; port++)
    {
        if (mask & (1 << port))
        {
            g_padHandle = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, XDEVICE_NO_SLOT, NULL);
            if (g_padHandle) break;
        }
    }
    ZeroMemory(&g_pad,     sizeof(g_pad));
    ZeroMemory(&g_prevPad, sizeof(g_prevPad));
}

static void PollInput()
{
    DWORD inserted = 0, removed = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &inserted, &removed))
    {
        if (removed && g_padHandle) { XInputClose(g_padHandle); g_padHandle = NULL; }
        if (inserted && !g_padHandle)
        {
            for (DWORD port = 0; port < 4; port++)
            {
                if (inserted & (1 << port))
                {
                    g_padHandle = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, XDEVICE_NO_SLOT, NULL);
                    if (g_padHandle) break;
                }
            }
        }
    }
    if (!g_padHandle) return;

    g_prevPad = g_pad;
    ZeroMemory(&g_pad, sizeof(g_pad));
    XInputGetState(g_padHandle, &g_pad);

    WORD buttons  = g_pad.Gamepad.wButtons;
    WORD previous = g_prevPad.Gamepad.wButtons;
    WORD pressed  = buttons & ~previous;

    if (pressed & XINPUT_GAMEPAD_DPAD_RIGHT)
    {
        g_test = (g_test + 1) % (g_maxTest + 1);
        g_readPixelsPrinted = false; g_getPrinted = false;
        g_rotPhase = 0; g_copyDone = false; g_errorTestDone = false; g_time = 0.0f;
        g_pbgl4_light = 1; g_pbgl4_filter = 1; g_pbgl5_light = 1;
        g_pbgl6_objPos[0] = -4.0f; g_pbgl6_objPos[1] = -2.0f; g_pbgl6_objPos[2] = -10.0f;
    }
    if (pressed & XINPUT_GAMEPAD_DPAD_LEFT)
    {
        g_test = (g_test + g_maxTest) % (g_maxTest + 1);
        g_readPixelsPrinted = false; g_getPrinted = false;
        g_rotPhase = 0; g_copyDone = false; g_errorTestDone = false; g_time = 0.0f;
        g_pbgl4_light = 1; g_pbgl4_filter = 1; g_pbgl5_light = 1;
        g_pbgl6_objPos[0] = -4.0f; g_pbgl6_objPos[1] = -2.0f; g_pbgl6_objPos[2] = -10.0f;
    }
    if (pressed & XINPUT_GAMEPAD_DPAD_UP)   { g_rotPhase = (g_rotPhase+1)%5; g_time=0; }
    if (pressed & XINPUT_GAMEPAD_DPAD_DOWN)  { g_rotPhase = (g_rotPhase+4)%5; g_time=0; }

    // Per-test face button handling for pbgl tests.
    // bAnalogButtons[]: A=0 B=1 X=2 Y=3 Black=4 White=5
    // "pressed" for analog = was 0 last frame, non-zero this frame.
    #define ABTN_PRESSED(i) (g_pad.Gamepad.bAnalogButtons[i] > 0 && g_prevPad.Gamepad.bAnalogButtons[i] == 0)
    #define ABTN_HELD(i)    (g_pad.Gamepad.bAnalogButtons[i] > 0)

    if (g_test == 90)
    {
        if (ABTN_PRESSED(XINPUT_GAMEPAD_A)) g_pbgl4_light  = !g_pbgl4_light;
        if (ABTN_PRESSED(XINPUT_GAMEPAD_B)) g_pbgl4_filter = (g_pbgl4_filter + 1) % 3;
    }
    if (g_test == 91)
    {
        if (ABTN_PRESSED(XINPUT_GAMEPAD_A)) g_pbgl5_light = !g_pbgl5_light;
    }
    if (g_test == 92)
    {
        if (ABTN_HELD(XINPUT_GAMEPAD_Y))     g_pbgl6_objPos[2] -= 0.1f;
        else if (ABTN_HELD(XINPUT_GAMEPAD_A)) g_pbgl6_objPos[2] += 0.1f;
        if (ABTN_HELD(XINPUT_GAMEPAD_B))     g_pbgl6_objPos[0] -= 0.1f;
        else if (ABTN_HELD(XINPUT_GAMEPAD_X)) g_pbgl6_objPos[0] += 0.1f;
        if (ABTN_HELD(XINPUT_GAMEPAD_WHITE)) g_pbgl6_objPos[1] -= 0.1f;
        else if (ABTN_HELD(XINPUT_GAMEPAD_BLACK)) g_pbgl6_objPos[1] += 0.1f;
    }

    #undef ABTN_PRESSED
    #undef ABTN_HELD
}

// ------------------------------------------------------------
// Border
// ------------------------------------------------------------

static void DrawBorder()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    SetGL2D(640, 480);

    float r = (g_test & 1) ? 1.0f : 0.2f;
    float g = (g_test & 2) ? 1.0f : 0.2f;
    float b = (g_test & 4) ? 1.0f : 0.2f;

    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(0,0);   glVertex2f(640,0);   glVertex2f(640,5);   glVertex2f(0,5);
    glVertex2f(0,475); glVertex2f(640,475); glVertex2f(640,480); glVertex2f(0,480);
    glVertex2f(0,0);   glVertex2f(5,0);     glVertex2f(5,480);   glVertex2f(0,480);
    glVertex2f(635,0); glVertex2f(640,0);   glVertex2f(640,480); glVertex2f(635,480);
    glEnd();
}

// ------------------------------------------------------------
// DumpInfo
// ------------------------------------------------------------

static void DumpInfo()
{
    char buf[512];
    OutputDebugString("RXGL compatibility test starting\n");

    wsprintf(buf, "GL_VENDOR:   %s\n", (const char*)glGetString(GL_VENDOR));   OutputDebugString(buf);
    wsprintf(buf, "GL_RENDERER: %s\n", (const char*)glGetString(GL_RENDERER)); OutputDebugString(buf);
    wsprintf(buf, "GL_VERSION:  %s\n", (const char*)glGetString(GL_VERSION));  OutputDebugString(buf);

    GLint maxTex=0, depthBits=0, stencilBits=0, texUnits=0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,  &maxTex);
    glGetIntegerv(GL_DEPTH_BITS,        &depthBits);
    glGetIntegerv(GL_STENCIL_BITS,      &stencilBits);
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texUnits);
    wsprintf(buf, "Caps: MAX_TEX=%d DEPTH=%d STENCIL=%d UNITS=%d\n",
        maxTex, depthBits, stencilBits, texUnits);
    OutputDebugString(buf);
}

// ------------------------------------------------------------
// Entry point
// ------------------------------------------------------------

void __cdecl main()
{
    d3dSetMode(640, 480, 32, 24, 0);
    d3dInitSetForce16BitTextures(0);
    d3dHint_GenerateMipMaps(0);

    InitInput();
    wglCreateContext();
    wglMakeCurrent();

    DumpInfo();
    MakeTextures();

    g_lastTick = GetTickCount();

    while (true)
    {
        DWORD now = GetTickCount();
        float dt  = (float)(now - g_lastTick) * 0.001f;
        g_lastTick = now;
        if (dt > 0.1f) dt = 0.1f;
        g_time += dt;

        PollInput();

        RunTest(g_test);
        DrawBorder();
        FakeSwapBuffers();
    }

    glDeleteTextures(1, &g_checkerTex);
    glDeleteTextures(1, &g_rgbaTex);
    glDeleteTextures(1, &g_dynamicTex);
    glDeleteTextures(1, &g_alphaTex);
    glDeleteTextures(1, &g_whiteTex);
    glDeleteTextures(1, &g_bumpNormalTex);
    if (g_asymTex) { glDeleteTextures(1, &g_asymTex); g_asymTex = 0; }
    if (g_callListsBase) glDeleteLists(g_callListsBase, 36);
    wglDeleteContext();
}