/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * RXGL-PC/Main.cpp  --  Win32/WGL entry point and bootstrap for the PC reference
 *                       build of the RXGL test harness. Creates a Win32 window with
 *                       a native WGL OpenGL context (no shim) and runs the same test
 *                       suite as the Xbox build, allowing tests to be developed and
 *                       verified on desktop before deploying to hardware. Test
 *                       functions are defined in Tests_Shared.h and the
 *                       Tests_NN_to_NN.h files in RXGL-Xbox; this file drives
 *                       navigation and maps PC keyboard input to the equivalent
 *                       Xbox gamepad controls.
 *
 * Controls (global):
 *   Left / Right arrow     = cycle through tests
 *   Up / Down arrow        = step rotation phase (Test 4)
 *
 * Controls (Test 90 - pbgl4 lighting):
 *   L                      = toggle lighting on/off
 *   F                      = cycle texture filter mode (3 modes)
 *
 * Controls (Test 91 - pbgl5 lighting):
 *   L                      = toggle lighting on/off
 *
 * Controls (Test 92 - pbgl6 object move):
 *   W / S (held)           = move object toward / away from camera (Z axis)
 *   A / D (held)           = move object left / right (X axis)
 *   Q / E (held)           = move object down / up (Y axis)
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>
#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#ifndef PFD_DRAW_TO_WINDOW
#define PFD_DRAW_TO_WINDOW  0x00000004
#endif
#ifndef PFD_SUPPORT_OPENGL
#define PFD_SUPPORT_OPENGL  0x00000020
#endif
#ifndef PFD_DOUBLE_BUFFER
#define PFD_DOUBLE_BUFFER   0x00000001
#endif

typedef void (WINAPI* PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (WINAPI* PFNGLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat s, GLfloat t);
typedef void (WINAPI* PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (WINAPI* PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (WINAPI* PFNGLWINDOWPOS2FARBPROC)(GLfloat x, GLfloat y);
typedef void (WINAPI* PFNGLWINDOWPOS2IARBPROC)(GLint x, GLint y);
typedef void (WINAPI* PFNGLBLENDCOLORPROC)(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void (WINAPI* PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (WINAPI* PFNGLLOADTRANSPOSEMATRIXFPROC)(const GLfloat* m);
typedef void (WINAPI* PFNGLLOADTRANSPOSEMATRIXDPROC)(const GLdouble* m);
typedef void (WINAPI* PFNGLMULTTRANSPOSEMATRIXFPROC)(const GLfloat* m);
typedef void (WINAPI* PFNGLMULTTRANSPOSEMATRIXDPROC)(const GLdouble* m);
typedef void (WINAPI* PFNGLMULTITEXCOORD3FPROC)(GLenum t, GLfloat s, GLfloat u, GLfloat r);
typedef void (WINAPI* PFNGLMULTITEXCOORD4FPROC)(GLenum t, GLfloat s, GLfloat u, GLfloat r, GLfloat q);
static PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB = nullptr;
static PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB = nullptr;
static PFNGLBLENDEQUATIONPROC          glBlendEquation = nullptr;
static PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = nullptr;
PFNGLWINDOWPOS2FARBPROC glWindowPos2fARB = nullptr;
PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB = nullptr;
static PFNGLBLENDCOLORPROC            glBlendColor = nullptr;
static PFNGLGENERATEMIPMAPPROC         glGenerateMipmap = nullptr;
static PFNGLLOADTRANSPOSEMATRIXFPROC   glLoadTransposeMatrixf = nullptr;
static PFNGLLOADTRANSPOSEMATRIXDPROC   glLoadTransposeMatrixd = nullptr;
static PFNGLMULTTRANSPOSEMATRIXFPROC   glMultTransposeMatrixf = nullptr;
static PFNGLMULTTRANSPOSEMATRIXDPROC   glMultTransposeMatrixd = nullptr;
static PFNGLMULTITEXCOORD3FPROC        glMultiTexCoord3f = nullptr;
static PFNGLMULTITEXCOORD4FPROC        glMultiTexCoord4f = nullptr;

#include "Tests_Shared.h"

// ------------------------------------------------------------
// Win32 / WGL state
// ------------------------------------------------------------

static HWND  g_hwnd = nullptr;
static HDC   g_hdc = nullptr;
static HGLRC g_hglrc = nullptr;
static bool  g_running = true;

static bool  g_leftWasDown = false;
static bool  g_rightWasDown = false;
static bool  g_upWasDown = false;
static bool  g_downWasDown = false;

// pbgl test key state
static bool  g_lWasDown = false;   // L = toggle lighting (test 90/91)
static bool  g_fWasDown = false;   // F = cycle filter    (test 90)
static bool  g_wWasDown = false;   // W/S = object Z      (test 92)
static bool  g_sWasDown = false;
static bool  g_aWasDown = false;   // A/D = object X      (test 92)
static bool  g_dWasDown = false;
static bool  g_qWasDown = false;   // Q/E = object Y      (test 92)
static bool  g_eWasDown = false;

static void LoadMultitextureEntryPoints()
{
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");
    if (!glBlendEquation)
        glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquationEXT");
    glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
    glWindowPos2fARB = (PFNGLWINDOWPOS2FARBPROC)wglGetProcAddress("glWindowPos2fARB");
    glWindowPos2iARB = (PFNGLWINDOWPOS2IARBPROC)wglGetProcAddress("glWindowPos2iARB");
    glBlendColor = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
    glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)wglGetProcAddress("glLoadTransposeMatrixf");
    glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)wglGetProcAddress("glLoadTransposeMatrixd");
    glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)wglGetProcAddress("glMultTransposeMatrixf");
    glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)wglGetProcAddress("glMultTransposeMatrixd");
    glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)wglGetProcAddress("glMultiTexCoord3f");
    glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)wglGetProcAddress("glMultiTexCoord4f");
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CLOSE) { g_running = false; return 0; }
    if (msg == WM_ERASEBKGND) return 1;
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

static bool CreateAppWindowAndGL(int clientW, int clientH)
{
    static const char* kClass = "GLTestPC";
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc); wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = kClass;
    if (!RegisterClassExA(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT r = { 0, 0, clientW, clientH };
    AdjustWindowRect(&r, style, FALSE);

    g_hwnd = CreateWindowExA(0, kClass,
        "GLTest PC | Left/Right: change test | Up/Down: rotation phase (Test4)",
        style, CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_hwnd) return false;

    g_hdc = GetDC(g_hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd); pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLE_BUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 32; pfd.cDepthBits = 24; pfd.cStencilBits = 8;
    int pf = ChoosePixelFormat(g_hdc, &pfd);
    if (!pf || !SetPixelFormat(g_hdc, pf, &pfd)) return false;

    g_hglrc = wglCreateContext(g_hdc);
    if (!g_hglrc || !wglMakeCurrent(g_hdc, g_hglrc)) return false;

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    return true;
}

static void ShutdownGL()
{
    if (g_hglrc) { wglMakeCurrent(nullptr, nullptr); wglDeleteContext(g_hglrc); g_hglrc = nullptr; }
    if (g_hwnd && g_hdc) { ReleaseDC(g_hwnd, g_hdc); g_hdc = nullptr; }
    if (g_hwnd) { DestroyWindow(g_hwnd); g_hwnd = nullptr; }
}

// ------------------------------------------------------------
// Input
// ------------------------------------------------------------

static void AdvanceTest(int delta)
{
    int n = g_maxTest + 1;
    g_test = ((g_test + delta) % n + n) % n;
    g_readPixelsPrinted = false;
    g_getPrinted = false;
    g_rotPhase = 0;
    g_copyDone = false; g_errorTestDone = false;
    g_time = 0.0f;
    g_pbgl4_light = 1; g_pbgl4_filter = 1; g_pbgl5_light = 1;
    g_pbgl6_objPos[0] = -4.0f; g_pbgl6_objPos[1] = -2.0f; g_pbgl6_objPos[2] = -10.0f;
}

static void PollInput()
{
    bool leftDown = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
    if (leftDown && !g_leftWasDown)  AdvanceTest(-1);
    g_leftWasDown = leftDown;

    bool rightDown = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    if (rightDown && !g_rightWasDown) AdvanceTest(1);
    g_rightWasDown = rightDown;

    bool upDown = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
    if (upDown && !g_upWasDown) { g_rotPhase = (g_rotPhase + 1) % 5; g_time = 0; }
    g_upWasDown = upDown;

    bool downDown = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    if (downDown && !g_downWasDown) { g_rotPhase = (g_rotPhase + 4) % 5; g_time = 0; }
    g_downWasDown = downDown;

    // pbgl test keys -- only fire when we've been on this test for at least
    // one frame, so the arrow key that navigates TO the test doesn't
    // simultaneously trigger the test's own keys.
    static int s_prevTest = -1;
    int testStable = (g_test == s_prevTest);
    s_prevTest = g_test;

    if (testStable && (g_test == 90 || g_test == 91))
    {
        bool lDown = (GetAsyncKeyState('L') & 0x8000) != 0;
        if (lDown && !g_lWasDown)
        {
            if (g_test == 90) g_pbgl4_light = !g_pbgl4_light;
            else              g_pbgl5_light = !g_pbgl5_light;
        }
        g_lWasDown = lDown;
    }
    if (testStable && g_test == 90)
    {
        bool fDown = (GetAsyncKeyState('F') & 0x8000) != 0;
        if (fDown && !g_fWasDown) g_pbgl4_filter = (g_pbgl4_filter + 1) % 3;
        g_fWasDown = fDown;
    }
    if (testStable && g_test == 92)
    {
        bool wDown = (GetAsyncKeyState('W') & 0x8000) != 0;
        bool sDown = (GetAsyncKeyState('S') & 0x8000) != 0;
        bool aDown = (GetAsyncKeyState('A') & 0x8000) != 0;
        bool dDown = (GetAsyncKeyState('D') & 0x8000) != 0;
        bool qDown = (GetAsyncKeyState('Q') & 0x8000) != 0;
        bool eDown = (GetAsyncKeyState('E') & 0x8000) != 0;
        if (wDown) g_pbgl6_objPos[2] -= 0.05f;
        if (sDown) g_pbgl6_objPos[2] += 0.05f;
        if (aDown) g_pbgl6_objPos[0] -= 0.05f;
        if (dDown) g_pbgl6_objPos[0] += 0.05f;
        if (qDown) g_pbgl6_objPos[1] -= 0.05f;
        if (eDown) g_pbgl6_objPos[1] += 0.05f;
        g_wWasDown = wDown; g_sWasDown = sDown;
        g_aWasDown = aDown; g_dWasDown = dDown;
        g_qWasDown = qDown; g_eWasDown = eDown;
    }
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
    glVertex2f(0, 0);   glVertex2f(640, 0);   glVertex2f(640, 5);   glVertex2f(0, 5);
    glVertex2f(0, 475); glVertex2f(640, 475); glVertex2f(640, 480); glVertex2f(0, 480);
    glVertex2f(0, 0);   glVertex2f(5, 0);     glVertex2f(5, 480);   glVertex2f(0, 480);
    glVertex2f(635, 0); glVertex2f(640, 0);   glVertex2f(640, 480); glVertex2f(635, 480);
    glEnd();
}

// ------------------------------------------------------------
// DumpInfo
// ------------------------------------------------------------

static void DumpInfoStr(const char* label, GLenum name)
{
    const char* s = (const char*)glGetString(name);
    OutputDebugStringA(label);
    OutputDebugStringA(s ? s : "(null)");
    OutputDebugStringA("\n");
}

static void DumpInfo()
{
    char buf[256];
    OutputDebugStringA("GLTest PC starting\n");
    DumpInfoStr("GL_VENDOR:   ", GL_VENDOR);
    DumpInfoStr("GL_RENDERER: ", GL_RENDERER);
    DumpInfoStr("GL_VERSION:  ", GL_VERSION);
    GLint maxTex = 0, depthBits = 0, stencilBits = 0, texUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTex);
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texUnits);
    wsprintfA(buf, "Caps: MAX_TEX=%d DEPTH=%d STENCIL=%d UNITS=%d\n", maxTex, depthBits, stencilBits, texUnits);
    OutputDebugStringA(buf);
}

// ------------------------------------------------------------
// WinMain
// ------------------------------------------------------------

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    if (!CreateAppWindowAndGL(640, 480))
    {
        MessageBoxA(nullptr, "Failed to create GL context.", "GLTest PC", MB_OK | MB_ICONERROR);
        return 1;
    }

    LoadMultitextureEntryPoints();
    glViewport(0, 0, 640, 480);
    DumpInfo();
    MakeTextures();

    g_lastTick = GetTickCount();

    while (g_running)
    {
        MSG msg;
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) g_running = false;
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        if (!g_running) break;

        DWORD now = GetTickCount();
        float dt = (float)(now - g_lastTick) * 0.001f;
        g_lastTick = now;
        if (dt > 0.1f) dt = 0.1f;
        g_time += dt;

        PollInput();

        static int s_titleTest = -1;
        if (g_test != s_titleTest)
        {
            s_titleTest = g_test;
            char title[256];
            if (g_test == 90)
                wsprintfA(title, "GLTest PC | Test %d / %d | L: lighting  F: filter", g_test, g_maxTest);
            else if (g_test == 91)
                wsprintfA(title, "GLTest PC | Test %d / %d | L: lighting", g_test, g_maxTest);
            else if (g_test == 92)
                wsprintfA(title, "GLTest PC | Test %d / %d | WASD: move XZ  QE: move Y", g_test, g_maxTest);
            else
                wsprintfA(title, "GLTest PC | Test %d / %d | Left/Right: change test | Up/Down: rotation (Test4)",
                    g_test, g_maxTest);
            SetWindowTextA(g_hwnd, title);
        }

        RunTest(g_test);
        DrawBorder();
        SwapBuffers(g_hdc);
    }

    glDeleteTextures(1, &g_checkerTex);
    glDeleteTextures(1, &g_rgbaTex);
    glDeleteTextures(1, &g_dynamicTex);
    glDeleteTextures(1, &g_alphaTex);
    glDeleteTextures(1, &g_whiteTex);
    glDeleteTextures(1, &g_bumpNormalTex);
    if (g_asymTex) { glDeleteTextures(1, &g_asymTex); g_asymTex = 0; }
    if (g_testDisplayList) glDeleteLists(g_testDisplayList, 1);
    for (int i = 0;i < 6;i++) if (g_dl37[i]) glDeleteLists(g_dl37[i], 1);
    if (g_copyTex) { glDeleteTextures(1, &g_copyTex); g_copyTex = 0; }
    if (g_callListsBase) glDeleteLists(g_callListsBase, 36);

    ShutdownGL();
    return 0;
}