/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_25_to_49.h  --  Extended RXGL compatibility test suite, tests 25-49.
 *                       Covers polygon mode, multitexture, vertex arrays,
 *                       interleaved arrays, display lists, and blend equation modes.
 *                       Included by Tests_Shared.h. Add new tests here until test
 *                       49, then create a new Tests_50_to_74.h range file.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

static void Test25_PolygonMode()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    // LEFT: normal filled quad
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.4f, 0.0f);
        glVertex2f(70, 150);
        glVertex2f(230, 150);
        glVertex2f(230, 330);
        glVertex2f(70, 330);
    glEnd();

    // MIDDLE: wireframe quad
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(250, 150);
        glVertex2f(390, 150);
        glVertex2f(390, 330);
        glVertex2f(250, 330);
    glEnd();

    // RIGHT: point polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.8f, 1.0f);
        glVertex2f(430, 150);
        glVertex2f(570, 150);
        glVertex2f(570, 330);
        glVertex2f(430, 330);
    glEnd();

    // Reset for later tests.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void Test26_Viewport()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    struct VP { GLint x, y, w, h; float r, g, b; };

    VP vps[4] =
    {
        {  40, 260, 240, 160, 1, 0, 0 },
        { 360, 260, 240, 160, 0, 1, 0 },
        {  40,  60, 240, 160, 0, 0, 1 },
        { 360,  60, 240, 160, 1, 1, 0 }
    };

    for (int i = 0; i < 4; i++)
    {
        glViewport(vps[i].x, vps[i].y, vps[i].w, vps[i].h);
        SetGL2D(640, 480);

        glBegin(GL_QUADS);
            glColor3f(vps[i].r, vps[i].g, vps[i].b);
            glVertex2f(0, 0);
            glVertex2f(640, 0);
            glVertex2f(640, 480);
            glVertex2f(0, 480);
        glEnd();
    }

    glViewport(0, 0, 640, 480);
}

static void Test27_DepthRange()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    // LEFT:
    // Red writes normal depth range.
    // Green is farther, so it should be hidden.
    glDepthRange(0.0, 1.0);

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f(60, 100, -0.2f);
        glVertex3f(300, 100, -0.2f);
        glVertex3f(300, 380, -0.2f);
        glVertex3f(60, 380, -0.2f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 0);
        glVertex3f(180, 330, -0.6f);
        glVertex3f(90, 150, -0.6f);
        glVertex3f(270, 150, -0.6f);
    glEnd();

    glClear(GL_DEPTH_BUFFER_BIT);

    // RIGHT:
    // Red writes FAR depth range.
    // Cyan is geometrically farther, but mapped to NEAR range,
    // so cyan should appear on top if glDepthRange works.
    glDepthRange(0.75, 1.0);

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f(340, 100, -0.2f);
        glVertex3f(580, 100, -0.2f);
        glVertex3f(580, 380, -0.2f);
        glVertex3f(340, 380, -0.2f);
    glEnd();

    glDepthRange(0.0, 0.25);

    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 1);
        glVertex3f(460, 330, -0.6f);
        glVertex3f(370, 150, -0.6f);
        glVertex3f(550, 150, -0.6f);
    glEnd();

    glDepthRange(0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
}

static void Test28_LineWidthPointSize()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex2f(80, 360);
        glVertex2f(280, 360);
    glEnd();

    glLineWidth(6.0f);
    glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        glVertex2f(80, 260);
        glVertex2f(280, 260);
    glEnd();

    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glColor3f(1, 1, 0);
        glVertex2f(400, 360);
        glVertex2f(440, 360);
        glVertex2f(480, 360);
    glEnd();

    glPointSize(10.0f);
    glBegin(GL_POINTS);
        glColor3f(0, 1, 1);
        glVertex2f(400, 260);
        glVertex2f(440, 260);
        glVertex2f(480, 260);
    glEnd();

    glLineWidth(1.0f);
    glPointSize(1.0f);
}

static void Test29_LightingNormals()
{
    GLfloat lightPos[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    GLfloat white[4]    = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambient[4]  = { 0.45f, 0.45f, 0.45f, 1.0f };

    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

    SetGLPerspective(60, 640.0f / 480.0f, 0.1f, 20.0f);

    glTranslatef(0, 0, -4);
    glRotatef(g_time * 35.0f, 1, 0, 0);
    glRotatef(g_time * 50.0f, 0, 1, 0);

    glBegin(GL_QUADS);

        glNormal3f(0, 0, 1);
        glVertex3f(-1, -1, 1);
        glVertex3f( 1, -1, 1);
        glVertex3f( 1,  1, 1);
        glVertex3f(-1,  1, 1);

        glNormal3f(0, 0, -1);
        glVertex3f( 1, -1, -1);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1,  1, -1);
        glVertex3f( 1,  1, -1);

        glNormal3f(-1, 0, 0);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1,  1);
        glVertex3f(-1,  1,  1);
        glVertex3f(-1,  1, -1);

        glNormal3f(1, 0, 0);
        glVertex3f(1, -1,  1);
        glVertex3f(1, -1, -1);
        glVertex3f(1,  1, -1);
        glVertex3f(1,  1,  1);

        glNormal3f(0, 1, 0);
        glVertex3f(-1, 1,  1);
        glVertex3f( 1, 1,  1);
        glVertex3f( 1, 1, -1);
        glVertex3f(-1, 1, -1);

        glNormal3f(0, -1, 0);
        glVertex3f(-1, -1, -1);
        glVertex3f( 1, -1, -1);
        glVertex3f( 1, -1,  1);
        glVertex3f(-1, -1,  1);

    glEnd();

    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

static void Test30_LightMaterial()
{
    GLfloat lightPos[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    //GLfloat white[4]    = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat white[4] = { 0.35f, 0.35f, 0.35f, 1.0f };
    GLfloat ambient[4]  = { 0.08f, 0.08f, 0.08f, 1.0f };

    GLfloat red[4]   = { 1.0f, 0.1f, 0.1f, 1.0f };
    GLfloat green[4] = { 0.1f, 1.0f, 0.1f, 1.0f };
    GLfloat blue[4]  = { 0.1f, 0.3f, 1.0f, 1.0f };

    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    SetGL2D(640, 480);

    glNormal3f(0, 0, 1);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    glBegin(GL_QUADS);
        glVertex2f(80, 150);
        glVertex2f(220, 150);
        glVertex2f(220, 330);
        glVertex2f(80, 330);
    glEnd();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
    glBegin(GL_QUADS);
        glVertex2f(250, 150);
        glVertex2f(390, 150);
        glVertex2f(390, 330);
        glVertex2f(250, 330);
    glEnd();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
    glBegin(GL_QUADS);
        glVertex2f(420, 150);
        glVertex2f(560, 150);
        glVertex2f(560, 330);
        glVertex2f(420, 330);
    glEnd();

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

static void Test31_ColorMaterial()
{
    GLfloat lightPos[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    GLfloat white[4]    = { 1.0f, 1.0f, 1.0f, 1.0f };

    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

    SetGL2D(640, 480);

    glNormal3f(0, 0, 1);

    glBegin(GL_TRIANGLES);
        glColor3f(1, 0, 0);
        glVertex2f(320, 370);

        glColor3f(0, 1, 0);
        glVertex2f(160, 110);

        glColor3f(0, 0, 1);
        glVertex2f(480, 110);
    glEnd();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

static void Test32_TexEnvAdd()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    DrawTexEnvQuad(70, 150, 210, 330, GL_MODULATE, 0.5f, 0.5f, 0.5f);
    DrawTexEnvQuad(250, 150, 390, 330, GL_REPLACE,  1.0f, 0.0f, 0.0f);
    DrawTexEnvQuad(430, 150, 570, 330, GL_ADD,      0.25f, 0.25f, 0.25f);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);
}

static void DrawFogColumn(float x0, float x1, GLenum mode, float density)
{
    GLfloat fogColor[4] = {0.45f, 0.45f, 0.65f, 1.0f};

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, mode);
    glFogf(GL_FOG_START,   1.0f);
    glFogf(GL_FOG_END,     8.0f);
    glFogf(GL_FOG_DENSITY, density);
    glFogfv(GL_FOG_COLOR,  fogColor);

    for (int row = 0; row < 10; row++)
    {
        float z0 = -(float)row * 0.9f;
        float z1 = z0 - 0.85f;
        int checker = row & 1;

        glBegin(GL_QUADS);
        if (checker)
            glColor3f(0.15f, 0.85f, 0.12f);
        else
            glColor3f(0.08f, 0.40f, 0.06f);

        glVertex3f(x0, 0, z0);
        glVertex3f(x1, 0, z0);
        glVertex3f(x1, 0, z1);
        glVertex3f(x0, 0, z1);
        glEnd();
    }

    glDisable(GL_FOG);
}

static void Test33_FogModes()
{
    GLfloat fogColor[4] = {0.45f, 0.45f, 0.65f, 1.0f};

    glClearColor(fogColor[0], fogColor[1], fogColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // Look down at floor from slightly above, same camera as Test 11
    SetGLPerspective(70, 640.0f / 480.0f, 0.1f, 20.0f);
    glTranslatef(0, -1.0f, -0.5f);
    glRotatef(28.0f, 1, 0, 0);

    // Three side-by-side floor columns, each with a different fog equation.
    // Left:  GL_LINEAR  -- straight-line falloff between FOG_START and FOG_END
    // Mid:   GL_EXP     -- exponential falloff based on density
    // Right: GL_EXP2    -- squared exponential, noticeably sharper than EXP
    DrawFogColumn(-3.1f, -1.1f, GL_LINEAR, 1.0f);
    DrawFogColumn(-0.9f,  0.9f, GL_EXP,   0.20f);
    DrawFogColumn( 1.1f,  3.1f, GL_EXP2,  0.22f);

    glDisable(GL_DEPTH_TEST);
}

// ------------------------------------------------------------
// Test 34 - Blend Modes
// Shows 6 blend equation combos side by side:
// SRC_ALPHA/ONE_MINUS_SRC_ALPHA  (standard alpha blend)
// ONE / ONE                       (additive)
// DST_COLOR / ZERO               (multiply)
// ZERO / SRC_COLOR               (multiply alt)
// ONE / ONE_MINUS_SRC_ALPHA      (premultiplied alpha)
// SRC_ALPHA / ONE                (additive with alpha)
// ------------------------------------------------------------

static void DrawBlendQuad(float x0, float y0, float x1, float y1,
                           GLenum sfactor, GLenum dfactor,
                           float cr, float cg, float cb, float ca)
{
    glBlendFunc(sfactor, dfactor);
    glBegin(GL_QUADS);
    glColor4f(cr, cg, cb, ca);
    glVertex2f(x0, y0);
    glVertex2f(x1, y0);
    glVertex2f(x1, y1);
    glVertex2f(x0, y1);
    glEnd();
}

static void Test34_BlendModes()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    // Column layout: 6 columns, each showing a blend mode.
    // Each column has a coloured background stripe, then a
    // semi-transparent overlay quad drawn with a different blend mode.

    struct BlendCase
    {
        GLenum   src, dst;
        float    or_, og, ob;  // overlay colour
        float    oa;           // overlay alpha
        const char* label;     // not drawn but useful for reference
    };

    BlendCase cases[6] =
    {
        { GL_SRC_ALPHA,     GL_ONE_MINUS_SRC_ALPHA, 1.0f, 0.0f, 0.0f, 0.6f, "STD"  },
        { GL_ONE,           GL_ONE,                 0.0f, 0.5f, 1.0f, 1.0f, "ADD"  },
        { GL_DST_COLOR,     GL_ZERO,                0.8f, 0.8f, 0.0f, 1.0f, "MUL"  },
        { GL_ZERO,          GL_SRC_COLOR,           0.5f, 1.0f, 0.5f, 1.0f, "MLA"  },
        { GL_ONE,           GL_ONE_MINUS_SRC_ALPHA, 0.0f, 1.0f, 0.4f, 0.5f, "PMA"  },
        { GL_SRC_ALPHA,     GL_ONE,                 1.0f, 0.5f, 0.0f, 0.7f, "AAD"  },
    };

    float colW  = 640.0f / 6.0f;
    float bgY0  = 80.0f;
    float bgY1  = 400.0f;
    float ovY0  = 160.0f;
    float ovY1  = 320.0f;

    // Background colours for each column -- a gradient strip so blend
    // results are visible against varying backgrounds.
    float bgColors[6][3] =
    {
        { 0.0f, 0.4f, 0.8f },
        { 0.8f, 0.2f, 0.0f },
        { 0.2f, 0.7f, 0.2f },
        { 0.6f, 0.0f, 0.6f },
        { 0.8f, 0.8f, 0.0f },
        { 0.0f, 0.6f, 0.6f },
    };

    for (int i = 0; i < 6; i++)
    {
        float x0 = i * colW + 4.0f;
        float x1 = x0 + colW - 8.0f;

        // Draw solid background
        glDisable(GL_BLEND);
        glBegin(GL_QUADS);
        glColor3f(bgColors[i][0], bgColors[i][1], bgColors[i][2]);
        glVertex2f(x0, bgY0);
        glVertex2f(x1, bgY0);
        glVertex2f(x1, bgY1);
        glVertex2f(x0, bgY1);
        glEnd();

        // Draw blend overlay
        glEnable(GL_BLEND);
        DrawBlendQuad(x0, ovY0, x1, ovY1,
                      cases[i].src, cases[i].dst,
                      cases[i].or_, cases[i].og, cases[i].ob, cases[i].oa);
    }

    glDisable(GL_BLEND);

    // Draw a white dividing line between columns for clarity
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int i = 1; i < 6; i++)
    {
        float x = i * colW;
        glVertex2f(x, bgY0);
        glVertex2f(x, bgY1);
    }
    glEnd();
}

// ------------------------------------------------------------
// Test 35 - Scissor
// Four quadrants each scissored independently.
// Centre cross drawn without scissor to confirm it bleeds through.
// Inner dynamic scissor box shrinks/grows with time to show
// that scissor state changes correctly mid-frame.
// ------------------------------------------------------------

static void Test35_Scissor()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    // ---- Pass 1: fill each quadrant with a different colour
    // using scissor to confine each quad exactly.
    struct Quad { GLint x, y, w, h; float r, g, b; };

    Quad quads[4] =
    {
        {   0, 240, 320, 240, 0.7f, 0.1f, 0.1f },   // top-left     red
        { 320, 240, 320, 240, 0.1f, 0.7f, 0.1f },   // top-right    green
        {   0,   0, 320, 240, 0.1f, 0.1f, 0.7f },   // bottom-left  blue
        { 320,   0, 320, 240, 0.6f, 0.6f, 0.0f },   // bottom-right yellow
    };

    glEnable(GL_SCISSOR_TEST);

    for (int i = 0; i < 4; i++)
    {
        glScissor(quads[i].x, quads[i].y, quads[i].w, quads[i].h);
        glBegin(GL_QUADS);
        glColor3f(quads[i].r, quads[i].g, quads[i].b);
        glVertex2f(0, 0);
        glVertex2f(640, 0);
        glVertex2f(640, 480);
        glVertex2f(0, 480);
        glEnd();
    }

    // ---- Pass 2: draw a full-screen white cross WITHOUT scissor.
    // It should appear across all four quadrants to confirm
    // that disabling scissor actually lifts the restriction.
    glDisable(GL_SCISSOR_TEST);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    // horizontal bar (6px tall)
    glVertex2f(  0, 237); glVertex2f(640, 237);
    glVertex2f(640, 243); glVertex2f(  0, 243);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    // vertical bar (6px wide)
    glVertex2f(317,   0); glVertex2f(323,   0);
    glVertex2f(323, 480); glVertex2f(317, 480);
    glEnd();

    // ---- Pass 3: animated magenta border drawn within scissor rect.
    // The 8px thick border is drawn just inside the scissor edge so
    // you can see where the scissor clips it. Region pulses with time.
    float pulse  = sinf_approx(g_time * 1.5f) * 0.5f + 0.5f;
    GLint margin = (GLint)(20.0f + pulse * 80.0f);
    GLint sw     = 640 - margin * 2;
    GLint sh     = 480 - margin * 2;

    glEnable(GL_SCISSOR_TEST);
    glScissor(margin, margin, sw, sh);

    float bx0 = (float)margin + 2.0f;
    float by0 = (float)margin + 2.0f;
    float bx1 = 640.0f - (float)margin - 2.0f;
    float by1 = 480.0f - (float)margin - 2.0f;
    float t   = 10.0f;

    // top strip
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(bx0, by1-t); glVertex2f(bx1, by1-t);
    glVertex2f(bx1, by1);   glVertex2f(bx0, by1);
    glEnd();

    // bottom strip
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(bx0,   by0);   glVertex2f(bx1,   by0);
    glVertex2f(bx1,   by0+t); glVertex2f(bx0,   by0+t);
    glEnd();

    // left strip
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(bx0,   by0+t); glVertex2f(bx0+t, by0+t);
    glVertex2f(bx0+t, by1-t); glVertex2f(bx0,   by1-t);
    glEnd();

    // right strip
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2f(bx1-t, by0+t); glVertex2f(bx1,   by0+t);
    glVertex2f(bx1,   by1-t); glVertex2f(bx1-t, by1-t);
    glEnd();

    glDisable(GL_SCISSOR_TEST);}

// ------------------------------------------------------------
// Test 36 - Viewport
// Screen split into 4 viewports. Each shows a different 3D scene
// to confirm that projection, modelview and clipping are all
// correctly scoped to each viewport.
//
// Top-left:    spinning triangle, ortho
// Top-right:   depth-tested overlapping quads, perspective
// Bottom-left: textured quad
// Bottom-right: rotating cube (same as Test4 but small)
// ------------------------------------------------------------

static void DrawMiniCube(float angle)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glTranslatef(0, 0, -3);
    glRotatef(angle * 35.0f, 1, 0, 0);
    glRotatef(angle * 55.0f, 0, 1, 0);

    for (int f = 0; f < 6; f++)
    {
        glBegin(GL_QUADS);
        glColor3f(cubeColors[f][0], cubeColors[f][1], cubeColors[f][2]);
        for (int v = 0; v < 4; v++)
        {
            const GLfloat* p = cubeVerts[cubeFaces[f][v]];
            glVertex3f(p[0], p[1], p[2]);
        }
        glEnd();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

static void Test36_Viewport()
{
    // Clear the full screen first
    glViewport(0, 0, 640, 480);
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    float angle = g_time;

    // ---- Top-left: spinning colour triangle, orthographic ----
    glViewport(0, 240, 320, 240);
    glClear(GL_DEPTH_BUFFER_BIT);

    SetGL2D(320, 240);
    glLoadIdentity();
    glTranslatef(160, 120, 0);
    glRotatef(angle * 60.0f, 0, 0, 1);

    glDisable(GL_DEPTH_TEST);
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0); glVertex2f(0,    80);
    glColor3f(0, 1, 0); glVertex2f(-70, -50);
    glColor3f(0, 0, 1); glVertex2f( 70, -50);
    glEnd();

    // ---- Top-right: perspective depth test ----
    glViewport(320, 240, 320, 240);
    glClear(GL_DEPTH_BUFFER_BIT);

    SetGLPerspective(60, 1.0f, 0.1f, 20.0f);
    glTranslatef(0, 0, -4);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.2f, 0.2f);
    glVertex3f(-1.5f, -1.0f, -1.0f); glVertex3f(1.5f, -1.0f, -1.0f);
    glVertex3f(1.5f,  1.0f, -1.0f);  glVertex3f(-1.5f, 1.0f, -1.0f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.8f, 0.2f);
    glVertex3f(-0.8f, -0.8f, 0.5f); glVertex3f(0.8f, -0.8f, 0.5f);
    glVertex3f(0.8f,  0.8f, 0.5f);  glVertex3f(-0.8f, 0.8f, 0.5f);
    glEnd();

    glDisable(GL_DEPTH_TEST);

    // ---- Bottom-left: textured quad ----
    glViewport(0, 0, 320, 240);
    glClear(GL_DEPTH_BUFFER_BIT);

    SetGL2D(320, 240);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0, 0); glVertex2f(20,  20);
    glTexCoord2f(3, 0); glVertex2f(300, 20);
    glTexCoord2f(3, 3); glVertex2f(300, 220);
    glTexCoord2f(0, 3); glVertex2f(20,  220);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // ---- Bottom-right: rotating mini cube ----
    glViewport(320, 0, 320, 240);
    glClear(GL_DEPTH_BUFFER_BIT);

    SetGLPerspective(60, 1.0f, 0.1f, 20.0f);
    DrawMiniCube(angle);

    // Restore full viewport
    glViewport(0, 0, 640, 480);

    // Draw thin lines separating the four quadrants
    SetGL2D(640, 480);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(320, 0);   glVertex2f(320, 480);  // vertical
    glVertex2f(0, 240);   glVertex2f(640, 240);  // horizontal
    glEnd();
}

// ------------------------------------------------------------
// Test 37 - Display List comprehensive coverage
// Builds several display lists that exercise all the newly
// recorded ops: color variants, normals, scale, matrix ops,
// state changes, nested call, fog, material.
// Each list draws one recognisable shape -- compare PC vs Xbox.
// ------------------------------------------------------------

static GLuint g_dl37[6] = {0,0,0,0,0,0};

static void BuildTest37Lists()
{
    if (g_dl37[0]) return;

    glGenLists(6);
    // Re-generate so we control IDs cleanly
    for (int i = 0; i < 6; i++) g_dl37[i] = glGenLists(1);

    // List 0: Color3ub + vertex3fv -- red triangle
    glNewList(g_dl37[0], GL_COMPILE);
    {
        GLfloat v0[3] = {-0.8f, -0.6f, 0};
        GLfloat v1[3] = { 0.0f,  0.6f, 0};
        GLfloat v2[3] = { 0.8f, -0.6f, 0};
        glBegin(GL_TRIANGLES);
            glColor3ub(220, 40, 40);
            glVertex3fv(v0);
            glColor3ub(255, 120, 60);
            glVertex3fv(v1);
            glColor3ub(180, 20, 20);
            glVertex3fv(v2);
        glEnd();
    }
    glEndList();

    // List 1: glScalef -- yellow quad that should be wide
    glNewList(g_dl37[1], GL_COMPILE);
    {
        glPushMatrix();
        glScalef(1.5f, 0.5f, 1.0f);
        glBegin(GL_QUADS);
            glColor3ub(220, 200, 0);
            glVertex3f(-0.5f, -0.5f, 0);
            glVertex3f( 0.5f, -0.5f, 0);
            glVertex3f( 0.5f,  0.5f, 0);
            glVertex3f(-0.5f,  0.5f, 0);
        glEnd();
        glPopMatrix();
    }
    glEndList();

    // List 2: glColor4ub + alpha blend -- semi-transparent blue over a background
    glNewList(g_dl37[2], GL_COMPILE);
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
            glColor4ub(40, 80, 220, 180);
            glVertex3f(-0.7f, -0.7f, 0);
            glVertex3f( 0.7f, -0.7f, 0);
            glVertex3f( 0.7f,  0.7f, 0);
            glVertex3f(-0.7f,  0.7f, 0);
        glEnd();
        glDisable(GL_BLEND);
    }
    glEndList();

    // List 3: glNormal3f + glMaterialfv -- lit quad (cyan material)
    glNewList(g_dl37[3], GL_COMPILE);
    {
        GLfloat cyan[4]    = {0.0f, 0.8f, 0.8f, 1.0f};
        GLfloat ambient[4] = {0.1f, 0.3f, 0.3f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  cyan);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ambient);
        glNormal3f(0, 0, 1);
        glBegin(GL_QUADS);
            glVertex3f(-0.7f, -0.7f, 0);
            glVertex3f( 0.7f, -0.7f, 0);
            glVertex3f( 0.7f,  0.7f, 0);
            glVertex3f(-0.7f,  0.7f, 0);
        glEnd();
    }
    glEndList();

    // List 4: nested glCallList -- calls list 0 (red triangle) from inside list 4
    glNewList(g_dl37[4], GL_COMPILE);
    {
        glPushMatrix();
        glScalef(0.7f, 0.7f, 1.0f);
        glCallList(g_dl37[0]);   // nested call -- red triangle scaled down
        glPopMatrix();
    }
    glEndList();

    // List 5: glShadeModel flat -- flat-shaded quad, each tri one colour
    glNewList(g_dl37[5], GL_COMPILE);
    {
        glShadeModel(GL_FLAT);
        glBegin(GL_TRIANGLES);
            glColor3ub(200, 50, 200);
            glVertex3f(-0.8f, -0.6f, 0);
            glVertex3f( 0.0f,  0.6f, 0);
            glVertex3f( 0.8f, -0.6f, 0);
        glEnd();
        glShadeModel(GL_SMOOTH);
    }
    glEndList();
}

static void Test37_DisplayListOps()
{
    BuildTest37Lists();

    glClearColor(0.06f, 0.06f, 0.10f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    // Arrange 6 lists in a 3x2 grid.
    // Each cell is ~200x200 px in a 640x480 screen.
    // We use a small ortho projection per cell.

    struct Cell { float cx, cy; const char* label; };
    Cell cells[6] =
    {
        { 107, 360, "color3ub+v3fv" },
        { 320, 360, "scalef"        },
        { 533, 360, "color4ub+blend"},
        { 107, 160, "normal+matfv"  },
        { 320, 160, "nested call"   },
        { 533, 160, "flat shade"    },
    };

    // Enable lighting for cell 3 (lit quad)
    GLfloat lightPos[4] = {0,0,1,0};
    GLfloat white[4]    = {1,1,1,1};

    for (int i = 0; i < 6; i++)
    {
        // Set up a small viewport for this cell
        GLint vx = (GLint)(cells[i].cx - 100);
        GLint vy = (GLint)(cells[i].cy - 100);
        glViewport(vx, vy, 200, 200);

        // Clear depth for this cell
        glClear(GL_DEPTH_BUFFER_BIT);

        // Unit ortho in this viewport
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1, 1, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Cell 3 needs lighting
        if (i == 3)
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  white);
            GLfloat amb[4] = {0.1f,0.1f,0.1f,1.0f};
            glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
        }

        // Draw a dark background rect
        glDisable(GL_BLEND);
        glBegin(GL_QUADS);
        glColor3f(0.15f, 0.15f, 0.20f);
        glVertex2f(-1,-1); glVertex2f(1,-1);
        glVertex2f(1, 1);  glVertex2f(-1, 1);
        glEnd();

        // Call the display list
        glCallList(g_dl37[i]);

        if (i == 3)
        {
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHTING);
        }
    }

    // Restore full viewport and reset state
    glViewport(0, 0, 640, 480);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glShadeModel(GL_SMOOTH);

    // Draw grid lines to separate the cells
    SetGL2D(640, 480);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
    glVertex2f(213, 0);   glVertex2f(213, 480);
    glVertex2f(427, 0);   glVertex2f(427, 480);
    glVertex2f(0,   260); glVertex2f(640, 260);
    glEnd();
}

// ------------------------------------------------------------
// Test 38 - glCopyTexSubImage2D
// LEFT half: source scene (coloured gradient quad + triangle).
// RIGHT half: a quad textured with a copy of the left half's
//             framebuffer rectangle -- should match exactly.
// A green border is drawn around both sides so misalignment
// is immediately obvious.
// ------------------------------------------------------------

static const GLsizei COPY_W = 256;
static const GLsizei COPY_H = 256;  // must be power-of-two for Xbox texture

static void Test38_CopyTexSubImage2D()
{
    // Allocate copy texture once
    if (!g_copyTex)
    {
        glGenTextures(1, &g_copyTex);
        glBindTexture(GL_TEXTURE_2D, g_copyTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, COPY_W, COPY_H, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // ---- Always draw source scene on the LEFT half ----
    glViewport(0, 0, 320, 480);
    SetGL2D(320, 480);

    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.1f, 0.1f); glVertex2f(  0,   0);
    glColor3f(0.1f, 0.1f, 0.8f); glVertex2f(320,   0);
    glColor3f(0.1f, 0.8f, 0.8f); glVertex2f(320, 480);
    glColor3f(0.8f, 0.8f, 0.1f); glVertex2f(  0, 480);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 1); glVertex2f(160, 380);
    glColor3f(1, 1, 0); glVertex2f( 60, 140);
    glColor3f(0, 1, 1); glVertex2f(260, 140);
    glEnd();

    // ---- Do the framebuffer copy on frame 2 ----
    // Frame 1: scene is drawn above but NOT yet on the backbuffer (still in GPU
    // command buffer). We let it swap once so the rendered pixels are committed.
    // Frame 2+: copy the backbuffer into the texture exactly once, then display it.
    GLint srcX = (320 - COPY_W) / 2;
    GLint srcY = (480 - COPY_H) / 2;

    if (!g_copyDone)
    {
        glBindTexture(GL_TEXTURE_2D, g_copyTex);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                            0, 0,
                            srcX, srcY,
                            COPY_W, COPY_H);
        g_copyDone = true;
    }

    // ---- Draw the texture on the RIGHT half ----
    glViewport(320, 0, 320, 480);
    SetGL2D(320, 480);

    // Dark background so we can see the textured quad clearly
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.1f, 0.1f);
    glVertex2f(0,0); glVertex2f(320,0); glVertex2f(320,480); glVertex2f(0,480);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_copyTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Draw the texture centred in the right half at the same size
    GLfloat qx0 = (320.0f - COPY_W) * 0.5f;
    GLfloat qy0 = (480.0f - COPY_H) * 0.5f;
    GLfloat qx1 = qx0 + COPY_W;
    GLfloat qy1 = qy0 + COPY_H;

    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glTexCoord2f(0,0); glVertex2f(qx0, qy0);
    glTexCoord2f(1,0); glVertex2f(qx1, qy0);
    glTexCoord2f(1,1); glVertex2f(qx1, qy1);
    glTexCoord2f(0,1); glVertex2f(qx0, qy1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Restore full viewport
    glViewport(0, 0, 640, 480);
    SetGL2D(640, 480);

    // Draw a centre divider line and source/dest border boxes
    // so misalignment is immediately visible
    glBegin(GL_LINES);
    glColor3f(0.4f, 0.4f, 0.4f);
    glVertex2f(320, 0); glVertex2f(320, 480);
    glEnd();

    // Green border around the source rectangle (left half)
    glBegin(GL_LINE_LOOP);
    glColor3f(0, 1, 0);
    glVertex2f((GLfloat)srcX,          (GLfloat)srcY);
    glVertex2f((GLfloat)(srcX+COPY_W), (GLfloat)srcY);
    glVertex2f((GLfloat)(srcX+COPY_W), (GLfloat)(srcY+COPY_H));
    glVertex2f((GLfloat)srcX,          (GLfloat)(srcY+COPY_H));
    glEnd();

    // Green border around the destination quad (right half -- offset by 320)
    glBegin(GL_LINE_LOOP);
    glColor3f(0, 1, 0);
    glVertex2f(320 + qx0, qy0);
    glVertex2f(320 + qx1, qy0);
    glVertex2f(320 + qx1, qy1);
    glVertex2f(320 + qx0, qy1);
    glEnd();
}

// ------------------------------------------------------------
// Test 39 - glGetFloatv visual verification
// 4x2 grid. Each cell sets a value, reads it back with glGetFloatv,
// then renders using the QUERIED value so you can see if it works.
// ONE clear at the start -- no per-cell clears, no scissor needed.
// ------------------------------------------------------------

static bool FloatEq39(GLfloat a, GLfloat b) { float d=a-b; return d>-0.01f&&d<0.01f; }

static void Test39_GetFloatv()
{
    const int CW = 160;  // 640/4
    const int CH = 240;  // 480/2

    // Single clear for the whole screen -- no per-cell clears needed
    glViewport(0, 0, 640, 480);
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // --- Cell (0,1): GL_FOG_COLOR ---
    // Orange fog. Fogged floor -- sky colour should be orange.
    {
        GLfloat set[4] = {0.8f, 0.4f, 0.05f, 1.0f};
        glFogfv(GL_FOG_COLOR, set);
        GLfloat got[4]; glGetFloatv(GL_FOG_COLOR, got);

        glViewport(0, CH, CW, CH);
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE,  GL_LINEAR);
        glFogf(GL_FOG_START, 1.0f);
        glFogf(GL_FOG_END,   6.0f);
        glFogfv(GL_FOG_COLOR, got);
        SetGLPerspective(65, 1.0f, 0.1f, 20.0f);
        glTranslatef(0,-0.5f,-0.3f); glRotatef(20,1,0,0);
        for (int r=0;r<10;r++) {
            float z0=-(float)r*0.9f, z1=z0-0.85f;
            glBegin(GL_QUADS);
            if (r&1) glColor3f(0.15f,0.85f,0.12f); else glColor3f(0.08f,0.40f,0.06f);
            glVertex3f(-1.5f,0,z0); glVertex3f(1.5f,0,z0);
            glVertex3f(1.5f,0,z1);  glVertex3f(-1.5f,0,z1);
            glEnd();
        }
        glDisable(GL_FOG);
    }

    // --- Cell (1,1): GL_FOG_START / GL_FOG_END ---
    // Grey fog. Nearest strip full colour, farthest fully grey.
    {
        GLfloat grey[4] = {0.55f,0.55f,0.55f,1.0f};
        glFogf(GL_FOG_START, 1.0f); GLfloat gotS; glGetFloatv(GL_FOG_START, &gotS);
        glFogf(GL_FOG_END,   5.0f); GLfloat gotE; glGetFloatv(GL_FOG_END,   &gotE);

        glViewport(CW, CH, CW, CH);
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE,  GL_LINEAR);
        glFogf(GL_FOG_START, gotS);
        glFogf(GL_FOG_END,   gotE);
        glFogfv(GL_FOG_COLOR, grey);
        SetGLPerspective(65, 1.0f, 0.1f, 20.0f);
        glTranslatef(0,-0.5f,-0.3f); glRotatef(20,1,0,0);
        for (int r=0;r<10;r++) {
            float z0=-(float)r*0.9f, z1=z0-0.85f;
            glBegin(GL_QUADS);
            if (r&1) glColor3f(0.15f,0.85f,0.12f); else glColor3f(0.08f,0.40f,0.06f);
            glVertex3f(-1.5f,0,z0); glVertex3f(1.5f,0,z0);
            glVertex3f(1.5f,0,z1);  glVertex3f(-1.5f,0,z1);
            glEnd();
        }
        glDisable(GL_FOG);
    }

    // --- Cell (2,1): GL_FOG_DENSITY ---
    // Blue EXP fog, density 0.4 -- should fade quickly.
    {
        GLfloat blue[4] = {0.1f,0.1f,0.7f,1.0f};
        glFogf(GL_FOG_DENSITY, 0.4f); GLfloat gotD; glGetFloatv(GL_FOG_DENSITY, &gotD);

        glViewport(CW*2, CH, CW, CH);
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE,    GL_EXP);
        glFogf(GL_FOG_DENSITY, gotD);
        glFogfv(GL_FOG_COLOR,  blue);
        SetGLPerspective(65, 1.0f, 0.1f, 20.0f);
        glTranslatef(0,-0.5f,-0.3f); glRotatef(20,1,0,0);
        for (int r=0;r<10;r++) {
            float z0=-(float)r*0.9f, z1=z0-0.85f;
            glBegin(GL_QUADS);
            if (r&1) glColor3f(0.15f,0.85f,0.12f); else glColor3f(0.08f,0.40f,0.06f);
            glVertex3f(-1.5f,0,z0); glVertex3f(1.5f,0,z0);
            glVertex3f(1.5f,0,z1);  glVertex3f(-1.5f,0,z1);
            glEnd();
        }
        glDisable(GL_FOG);
    }

    // --- Cell (3,1): GL_CURRENT_COLOR ---
    // Set magenta, read back, draw a quad that colour.
    {
        glColor4f(0.9f, 0.1f, 0.8f, 1.0f);
        GLfloat got[4]; glGetFloatv(GL_CURRENT_COLOR, got);

        glViewport(CW*3, CH, CW, CH);
        SetGL2D((float)CW, (float)CH);
        glBegin(GL_QUADS);
        glColor4f(got[0],got[1],got[2],got[3]);
        glVertex2f(10,20); glVertex2f((float)CW-10,20);
        glVertex2f((float)CW-10,(float)CH-20); glVertex2f(10,(float)CH-20);
        glEnd();
    }

    // ================================================================
    // BOTTOM ROW (y = 0 to (float)CH = 0 to 240)
    // ================================================================

    // Clear depth for the bottom row so 3D cells aren't affected by top row depth
    glViewport(0, 0, 640, CH);
    glDepthMask(GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    // --- Cell (0,0): GL_LIGHT_MODEL_AMBIENT ---
    // Purple ambient. Rotating white cube -- dark faces glow purple.
    {
        GLfloat lma[4] = {0.5f,0.0f,0.5f,1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lma);
        GLfloat got[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, got);

        glViewport(0, 0, CW, CH);
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);

        GLfloat lpos[4]={1,1,1,0}, white[4]={0.7f,0.7f,0.7f,1}, noamb[4]={0,0,0,1};
        glLightfv(GL_LIGHT0,GL_POSITION,lpos);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
        glLightfv(GL_LIGHT0,GL_AMBIENT,noamb);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, got);  // use queried value
        GLfloat matW[4]={1,1,1,1};
        glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matW);
        glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matW);

        SetGLPerspective(60,1.0f,0.1f,20.0f);
        glTranslatef(0,0,-4);
        glRotatef(g_time*30.0f,1,0,0);
        glRotatef(g_time*45.0f,0,1,0);

        int cf[6][4]={{4,5,6,7},{1,0,3,2},{0,4,7,3},{5,1,2,6},{7,6,2,3},{0,1,5,4}};
        GLfloat cn[6][3]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
        for (int f=0;f<6;f++) {
            glBegin(GL_QUADS);
            glNormal3f(cn[f][0],cn[f][1],cn[f][2]);
            for (int v=0;v<4;v++) glVertex3f(cubeVerts[cf[f][v]][0],cubeVerts[cf[f][v]][1],cubeVerts[cf[f][v]][2]);
            glEnd();
        }
        glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHT0); glDisable(GL_LIGHTING);
        GLfloat defLma[4]={0.2f,0.2f,0.2f,1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, defLma);
    }

    // --- Cell (1,0): GL_DEPTH_RANGE ---
    // Red quad in near range [0,0.5], blue in far range [0.5,1].
    // Same Z -- red should always be on top.
    {
        glViewport(CW, 0, CW, CH);
        glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
        SetGL2D((float)CW, (float)CH);

        glDepthRange(0.5, 1.0);
        GLfloat drB[2]; glGetFloatv(GL_DEPTH_RANGE, drB);
        glDepthRange(drB[0], drB[1]);
        glBegin(GL_QUADS); glColor3f(0.2f,0.2f,0.9f);
        glVertex3f(5,30,-0.5f);   glVertex3f((float)CW-5,30,-0.5f);
        glVertex3f((float)CW-5,(float)CH-30,-0.5f); glVertex3f(5,(float)CH-30,-0.5f);
        glEnd();

        glDepthRange(0.0, 0.5);
        GLfloat drR[2]; glGetFloatv(GL_DEPTH_RANGE, drR);
        glDepthRange(drR[0], drR[1]);
        glBegin(GL_QUADS); glColor3f(0.9f,0.2f,0.2f);
        glVertex3f(25,60,-0.5f);  glVertex3f((float)CW-25,60,-0.5f);
        glVertex3f((float)CW-25,(float)CH-60,-0.5f); glVertex3f(25,(float)CH-60,-0.5f);
        glEnd();

        glDepthRange(0.0,1.0);
        glDisable(GL_DEPTH_TEST);
    }

    // --- Cell (2,0): GL_LINE_WIDTH ---
    // 4 lines, widths 1/3/6/10. Draw using queried width.
    {
        glViewport(CW*2, 0, CW, CH);
        SetGL2D((float)CW, (float)CH);
        float ws[4]={1.0f,3.0f,6.0f,10.0f};
        GLfloat cols[4][3]={{1,0.2f,0.2f},{0.2f,1,0.2f},{0.2f,0.5f,1},{1,1,0.2f}};
        for (int i=0;i<4;i++) {
            glLineWidth(ws[i]);
            GLfloat got; glGetFloatv(GL_LINE_WIDTH,&got);
            glLineWidth(got);
            float ly=30.0f+i*((float)CH-60.0f)/3.0f;
            glBegin(GL_LINES);
            glColor3f(cols[i][0],cols[i][1],cols[i][2]);
            glVertex2f(10,ly); glVertex2f((float)CW-10,ly);
            glEnd();
        }
        glLineWidth(1.0f);
    }

    // --- Cell (3,0): GL_ALPHA_TEST_REF ---
    // 8 vertical strips, alpha 0?1. Ref=0.5: left half clipped.
    {
        glViewport(CW*3, 0, CW, CH);
        SetGL2D((float)CW, (float)CH);
        glAlphaFunc(GL_GREATER, 0.5f);
        GLfloat got; glGetFloatv(GL_ALPHA_TEST_REF, &got);
        glAlphaFunc(GL_GREATER, got);
        glEnable(GL_ALPHA_TEST);
        for (int s=0;s<8;s++) {
            float alpha=(float)s/7.0f;
            float sx0=s*(float)CW/8.0f, sx1=(s+1)*(float)CW/8.0f;
            glBegin(GL_QUADS);
            glColor4f(0.2f,0.85f,0.4f,alpha);
            glVertex2f(sx0,20);    glVertex2f(sx1,20);
            glVertex2f(sx1,(float)CH-20); glVertex2f(sx0,(float)CH-20);
            glEnd();
        }
        glDisable(GL_ALPHA_TEST);
        // Cyan tick at cutoff position
        glBegin(GL_LINES);
        glColor3f(0,1,1);
        glVertex2f(got*(float)CW,5);  glVertex2f(got*(float)CW,18);
        glVertex2f(got*(float)CW,(float)CH-18); glVertex2f(got*(float)CW,(float)CH-5);
        glEnd();
        glAlphaFunc(GL_ALWAYS,0.0f);
    }

    // Restore full viewport and draw grid dividers
    glViewport(0, 0, 640, 480);
    SetGL2D(640, 480);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);

    glBegin(GL_LINES);
    glColor3f(0.4f,0.4f,0.4f);
    glVertex2f((float)CW,0);   glVertex2f((float)CW,480);
    glVertex2f((float)CW*2,0); glVertex2f((float)CW*2,480);
    glVertex2f((float)CW*3,0); glVertex2f((float)CW*3,480);
    glVertex2f(0,(float)CH);   glVertex2f(640,(float)CH);
    glEnd();

    // Restore state
    glFogf(GL_FOG_START,   2.0f);
    glFogf(GL_FOG_END,     12.0f);
    glFogf(GL_FOG_DENSITY, 1.0f);
    glLineWidth(1.0f);
    glDepthRange(0.0,1.0);
}
// ------------------------------------------------------------
// Test 40 - GL_SPECULAR / GL_SHININESS / GL_EMISSION
// Four spheres with different material properties side by side:
//   Left:       Diffuse only (no specular) -- reference
//   Centre-L:   Low shininess (32) -- broad soft highlight
//   Centre-R:   High shininess (96) -- tight sharp highlight
//   Right:      Emission -- glows regardless of lighting
// All spheres are built from a subdivided latitude/longitude mesh
// so there are enough vertices to see the per-vertex highlight.
// ------------------------------------------------------------

// Build a sphere mesh at the given radius with lat/lon bands.
// Normals are the normalised vertex position.
// scrollU/V shift UVs (used by bump sphere test).
static void DrawSphere40(float radius, int stacks, int slices,
                         float scrollU = 0.0f, float scrollV = 0.0f)
{
    for (int i = 0; i < stacks; i++)
    {
        float phi0 = PI_F * (-0.5f + (float)i       / stacks);
        float phi1 = PI_F * (-0.5f + (float)(i + 1) / stacks);
        float cp0 = cosf_approx(phi0), sp0 = sinf_approx(phi0);
        float cp1 = cosf_approx(phi1), sp1 = sinf_approx(phi1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float theta = 2.0f * PI_F * (float)j / slices;
            float ct = cosf_approx(theta), st = sinf_approx(theta);
            float u = (float)j / slices;

            glNormal3f(cp0*ct, sp0, cp0*st);
            glTexCoord2f(u + scrollU, (float)i / stacks + scrollV);
            glVertex3f(radius*cp0*ct, radius*sp0, radius*cp0*st);

            glNormal3f(cp1*ct, sp1, cp1*st);
            glTexCoord2f(u + scrollU, (float)(i+1) / stacks + scrollV);
            glVertex3f(radius*cp1*ct, radius*sp1, radius*cp1*st);
        }
        glEnd();
    }
}

// Same geometry as DrawSphere40; UVs go to both texture units (Test 50 bump).
static void DrawSphereBumpMTex(float radius, int stacks, int slices,
                              float scrollU, float scrollV)
{
    for (int i = 0; i < stacks; i++)
    {
        float phi0 = PI_F * (-0.5f + (float)i       / stacks);
        float phi1 = PI_F * (-0.5f + (float)(i + 1) / stacks);
        float cp0 = cosf_approx(phi0), sp0 = sinf_approx(phi0);
        float cp1 = cosf_approx(phi1), sp1 = sinf_approx(phi1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++)
        {
            float theta = 2.0f * PI_F * (float)j / slices;
            float ct = cosf_approx(theta), st = sinf_approx(theta);
            float u = (float)j / slices;
            float v0 = (float)i / stacks + scrollV;
            float v1 = (float)(i + 1) / stacks + scrollV;
            float uc = u + scrollU;

            glNormal3f(cp0*ct, sp0, cp0*st);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, uc, v0);
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, uc, v0);
            glVertex3f(radius*cp0*ct, radius*sp0, radius*cp0*st);

            glNormal3f(cp1*ct, sp1, cp1*st);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, uc, v1);
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, uc, v1);
            glVertex3f(radius*cp1*ct, radius*sp1, radius*cp1*st);
        }
        glEnd();
    }
}

static void Test40_Specular()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // White light from upper-right
    GLfloat lpos[4]  = { 2.0f, 3.0f, 4.0f, 0.0f };
    GLfloat white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat dim[4]   = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  dim);

    // Sphere positions spaced across screen
    struct SphereDesc {
        float        x;
        GLfloat      diffuse[4];
        GLfloat      specular[4];
        GLfloat      emission[4];
        GLfloat      shininess;
        const char*  label;
    };

    SphereDesc spheres[4] = {
        // Diffuse only -- red, no specular
        { -2.4f,
          {0.8f,0.1f,0.1f,1}, {0,0,0,1}, {0,0,0,1}, 0.0f, "DIFF" },
        // Soft specular -- blue, shininess 32
        { -0.8f,
          {0.1f,0.2f,0.8f,1}, {0.8f,0.8f,0.8f,1}, {0,0,0,1}, 32.0f, "SH32" },
        // Sharp specular -- green, shininess 96
        {  0.8f,
          {0.1f,0.6f,0.1f,1}, {1.0f,1.0f,1.0f,1}, {0,0,0,1}, 96.0f, "SH96" },
        // Emission -- magenta glow regardless of light
        {  2.4f,
          {0.2f,0.0f,0.2f,1}, {0,0,0,1}, {0.6f,0.0f,0.6f,1}, 0.0f, "EMIT" },
    };

    SetGLPerspective(55, 640.0f/480.0f, 0.1f, 30.0f);
    glTranslatef(0, 0, -7.0f);
    glRotatef(g_time * 18.0f, 0, 1, 0);  // slow rotation to show highlight moving

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(spheres[i].x, 0, 0);

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  spheres[i].diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  spheres[i].diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spheres[i].specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, spheres[i].emission);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, spheres[i].shininess);

        DrawSphere40(0.9f, 20, 32);

        glPopMatrix();
    }

    // Draw labels using DrawChar under each sphere
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    SetGLPerspective(55, 640.0f/480.0f, 0.1f, 30.0f);
    glTranslatef(0, 0, -7.0f);

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(spheres[i].x, -1.4f, 0);
        glScalef(0.3f, 0.3f, 0.3f);
        glColor3f(0.8f, 0.8f, 0.8f);
        for (int c = 0; c < 4 && spheres[i].label[c]; c++)
        {
            glPushMatrix();
            glTranslatef((c - 1.5f) * 0.35f, 0, 0);
            DrawChar(spheres[i].label[c]);
            glPopMatrix();
        }
        glPopMatrix();
    }

    // Reset material to defaults
    GLfloat defSpec[4] = {0,0,0,1};
    GLfloat defEmit[4] = {0,0,0,1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defEmit);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

// ------------------------------------------------------------
// Test 41 - glCallLists / glListBase
// Simulates how old engines do font rendering:
//   1. Build one display list per character (A-Z, 0-9)
//   2. Set glListBase to the first list ID
//   3. Call glCallLists with a string of character indices
//      (character - 'A' or character - '0')
//
// Three rows of text, each using a different glCallLists type
// (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_BYTE) to verify all
// type paths work correctly.
// ------------------------------------------------------------

static GLuint g_callListsBase = 0;  // base list ID for A-Z then 0-9

static void BuildCallListsFont()
{
    if (g_callListsBase) return;

    // Reserve 36 lists: 0-25 = A-Z, 26-35 = 0-9
    g_callListsBase = glGenLists(36);

    // Each character list draws the char using DrawChar at scale 60
    // The list ID = g_callListsBase + (char - 'A') for letters
    //             = g_callListsBase + 26 + (char - '0') for digits
    const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[]  = "0123456789";

    for (int i = 0; i < 26; i++)
    {
        glNewList(g_callListsBase + i, GL_COMPILE);
            glPushMatrix();
            DrawChar(letters[i]);
            glTranslatef(0.5f, 0, 0);  // advance cursor one char width
            glPopMatrix();
            glTranslatef(0.5f, 0, 0);  // advance modelview for next char
        glEndList();
    }
    for (int i = 0; i < 10; i++)
    {
        glNewList(g_callListsBase + 26 + i, GL_COMPILE);
            glPushMatrix();
            DrawChar(digits[i]);
            glPopMatrix();
            glTranslatef(0.5f, 0, 0);
        glEndList();
    }
}

static void Test41_CallLists()
{
    BuildCallListsFont();

    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    SetGL2D(640, 480);

    // ---- Row 1: GL_UNSIGNED_BYTE type ----
    // Spell "HELLO XBOX" using unsigned byte indices
    {
        const char* word = "HELLOXBOX";
        GLubyte indices[9];
        for (int i = 0; word[i]; i++)
            indices[i] = (GLubyte)(word[i] - 'A');

        glColor3f(0.2f, 0.9f, 0.2f);  // green
        glLoadIdentity();
        glTranslatef(60.0f, 360.0f, 0.0f);
        glScalef(55.0f, 55.0f, 1.0f);

        glListBase(g_callListsBase);
        glCallLists(9, GL_UNSIGNED_BYTE, indices);
    }

    // ---- Row 2: GL_UNSIGNED_SHORT type ----
    // Spell "CALLLISTS" using unsigned short indices
    {
        const char* word = "CALLLISTS";
        GLushort indices[9];
        for (int i = 0; word[i]; i++)
            indices[i] = (GLushort)(word[i] - 'A');

        glColor3f(0.9f, 0.6f, 0.1f);  // orange
        glLoadIdentity();
        glTranslatef(60.0f, 260.0f, 0.0f);
        glScalef(55.0f, 55.0f, 1.0f);

        glListBase(g_callListsBase);
        glCallLists(9, GL_UNSIGNED_SHORT, indices);
    }

    // ---- Row 3: GL_BYTE with glListBase offset ----
    // Use digits: spell "0123456789" using GL_BYTE with base pointing at digits
    {
        GLbyte indices[10] = {0,1,2,3,4,5,6,7,8,9};

        glColor3f(0.3f, 0.7f, 1.0f);  // cyan
        glLoadIdentity();
        glTranslatef(60.0f, 155.0f, 0.0f);
        glScalef(55.0f, 55.0f, 1.0f);

        // Set base to point at digit lists (offset 26 from start)
        glListBase(g_callListsBase + 26);
        glCallLists(10, GL_BYTE, indices);
    }

    // ---- Row 4: Mixed -- use GL_FLOAT type ----
    // Spell "XBOX" to confirm float type path works
    {
        GLfloat indices[4] = {
            (GLfloat)('X'-'A'), (GLfloat)('B'-'A'),
            (GLfloat)('O'-'A'), (GLfloat)('X'-'A')
        };

        glColor3f(0.9f, 0.2f, 0.5f);  // pink
        glLoadIdentity();
        glTranslatef(60.0f, 55.0f, 0.0f);
        glScalef(55.0f, 55.0f, 1.0f);

        glListBase(g_callListsBase);
        glCallLists(4, GL_FLOAT, indices);
    }

    // Draw row labels at the left
    glLoadIdentity();
    SetGL2D(640, 480);
    glColor3f(0.4f, 0.4f, 0.4f);

    // Reset listBase
    glListBase(0);
}

// ------------------------------------------------------------
// Test 42 - glGetError real tracking
// Deliberately triggers each error type and checks that
// glGetError returns the right code. Draws a row of squares:
//   GREEN = correct error code returned
//   RED   = wrong code (or no error when one was expected)
// All 5 squares should be green.
// ------------------------------------------------------------

struct ETest42 { const char* label; bool pass; };
static ETest42 g_errorTests[6];
static GLenum  g_stickyFirst  = 0;
static GLenum  g_stickySecond = 0;

static void Test42_GetError()
{
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    SetGL2D(640, 480);


    // Only run the error-triggering GL calls once per test visit
    if (!g_errorTestDone)
    {
        g_errorTestDone = true;

        // Flush any pre-existing errors
        while (glGetError() != GL_NO_ERROR) {}

        // 0: Baseline -- no error after flush
        g_errorTests[0].label = "NO ERR";
        g_errorTests[0].pass  = (glGetError() == GL_NO_ERROR);

        // 1: GL_INVALID_ENUM -- bogus pname to glGetIntegerv
        { GLint dummy; glGetIntegerv((GLenum)0xDEAD, &dummy); }
        g_errorTests[1].label = "INV ENUM";
        g_errorTests[1].pass  = (glGetError() == GL_INVALID_ENUM);

        // 2: GL_INVALID_OPERATION -- glEnd without glBegin
        glEnd();
        g_errorTests[2].label = "INV OP";
        g_errorTests[2].pass  = (glGetError() == GL_INVALID_OPERATION);

        // 3: GL_INVALID_VALUE -- negative texture size
        {
            GLuint tmp; glGenTextures(1, &tmp);
            glBindTexture(GL_TEXTURE_2D, tmp);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, -1, -1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glDeleteTextures(1, &tmp);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        g_errorTests[3].label = "INV VAL";
        g_errorTests[3].pass  = (glGetError() == GL_INVALID_VALUE);

        // 4: Sticky -- first error wins, second discarded until read
        { GLint dummy; glGetIntegerv((GLenum)0xDEAD, &dummy); }  // INVALID_ENUM
        glEnd();                                                   // would be INVALID_OPERATION
        {
            GLenum first  = glGetError();
            GLenum second = glGetError();
            g_errorTests[4].label = "STICKY";
            g_errorTests[4].pass  = (first == GL_INVALID_ENUM && second == GL_NO_ERROR);
            g_stickyFirst  = first;
            g_stickySecond = second;
        }

        // 5: Clear -- error clears after one read
        { GLint dummy; glGetIntegerv((GLenum)0xDEAD, &dummy); }
        glGetError();
        g_errorTests[5].label = "CLEAR";
        g_errorTests[5].pass  = (glGetError() == GL_NO_ERROR);

        // Log results with raw hex values -- proves actual error codes returned
        char buf[256];
        OutputDebugString("--- Test42 glGetError results ---\n");
        wsprintf(buf, "  constants: NO_ERR=0x%x INV_ENUM=0x%x INV_OP=0x%x INV_VAL=0x%x\n",
            (UINT)GL_NO_ERROR,(UINT)GL_INVALID_ENUM,(UINT)GL_INVALID_OPERATION,(UINT)GL_INVALID_VALUE);
        OutputDebugString(buf);
        for (int li = 0; li < 6; li++)
        {
            wsprintf(buf, "  [%d] %-10s: %s\n", li,
                g_errorTests[li].label, g_errorTests[li].pass ? "PASS" : "FAIL");
            OutputDebugString(buf);
        }
        wsprintf(buf, "  STICKY: first=0x%x(want 0x500) second=0x%x(want 0x0)\n",
            (UINT)g_stickyFirst, (UINT)g_stickySecond);
        OutputDebugString(buf);
        OutputDebugString("---------------------------------\n");
    }



    // --- Draw 6 cells in a 3x2 grid ---
    // SetGL2D puts y=0 at bottom, y=480 at top.
    // Row 0 = top half (y: 240-480), Row 1 = bottom half (y: 0-240)
    const float CW6 = 640.0f / 3.0f;
    const float CH6 = 480.0f / 2.0f;

    for (int i = 0; i < 6; i++)
    {
        int col =  i % 3;
        int row =  i / 3;   // 0=top row, 1=bottom row

        float x0 = col * CW6 + 8.0f;
        float x1 = x0 + CW6 - 16.0f;
        // Y: row 0 = top half, row 1 = bottom half
        float cellTop    = 480.0f - row * CH6;        // top edge of cell
        float cellBottom = cellTop - CH6;              // bottom edge of cell
        float sqTop      = cellTop    - 40.0f;         // square top (leave room for label)
        float sqBot      = cellBottom + 30.0f;         // square bottom (leave room for marker)
        float cx         = (x0 + x1) * 0.5f;

        // Coloured square
        glBegin(GL_QUADS);
        if (g_errorTests[i].pass) glColor3f(0.1f, 0.8f, 0.1f);
        else               glColor3f(0.8f, 0.1f, 0.1f);
        glVertex2f(x0, sqBot); glVertex2f(x1, sqBot);
        glVertex2f(x1, sqTop); glVertex2f(x0, sqTop);
        glEnd();

        // Label above square
        glColor3f(1, 1, 1);
        glPushMatrix();
        glTranslatef(x0 + 6.0f, sqTop + 10.0f, 0);
        glScalef(18.0f, 18.0f, 1.0f);
        DrawString(g_errorTests[i].label);
        glPopMatrix();

        // +/- marker below square
        glPushMatrix();
        glTranslatef(cx, sqBot - 22.0f, 0);
        glScalef(20.0f, 20.0f, 1.0f);
        if (g_errorTests[i].pass) glColor3f(0, 1, 0); else glColor3f(1, 0, 0);
        DrawChar(g_errorTests[i].pass ? '+' : '-');
        glPopMatrix();
    }
}

// ------------------------------------------------------------
// Test 43 - glBlendEquation
// 5 columns showing each blend mode with two overlapping quads:
//   Background: bright cyan quad
//   Foreground: red quad blended over it
//
//   ADD      = src + dst  (gets brighter, may saturate)
//   SUBTRACT = dst - src  (gets darker)
//   REVSUB   = src - dst  (dark where dst > src)
//   MIN      = min(src,dst) per channel
//   MAX      = max(src,dst) per channel
//
// The visual difference between modes should be obvious.
// Compare PC vs Xbox -- should look identical.
// ------------------------------------------------------------

static void Test43_BlendEquation()
{
    // Each row shows 4 cells:
    //   [BACKGROUND] [FOREGROUND] [BLENDED RESULT] [EXPECTED LABEL]
    //
    // Row 1 - ADD:    Black + Red   = Red       (0+R=R)
    // Row 2 - ADD:    Red   + Cyan  = White     (R+C = saturate white)
    // Row 3 - SUB:    White - Red   = Cyan      (W-R removes red channel)
    // Row 4 - MIN:    Green + Blue  = Black     (min per channel = 0,0,0... actually min(0,1,0),(0,0,1) = 0,0,0 = black)
    // Row 5 - MAX:    Dark  + Bright = Bright   (max always picks brightest)

    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    SetGL2D(640, 480);

    struct Row {
        GLenum   eq;
        const char* eqName;
        float    bgR, bgG, bgB;   // background colour
        float    fgR, fgG, fgB;   // foreground colour
        float    exR, exG, exB;   // expected result colour
        const char* exLabel;
    };

    Row rows[5] = {
        // eq          name      BG (black)          FG (red)            Expected (red)
        { GL_FUNC_ADD,              "ADD",   0,0,0,         1,0,0,      1,0,0,    "RED"    },
        // eq          name      BG (black)          FG (white)          Expected (white = white-black)
        { GL_FUNC_SUBTRACT,         "SUB",   0,0,0,         1,1,1,      1,1,1,    "WHITE"  },
        // eq          name      BG (white)          FG (red)            Expected (cyan = white-red)
        { GL_FUNC_REVERSE_SUBTRACT, "REVSUB", 1,1,1,         1,0,0,      0,1,1,    "CYAN"   },
        // eq          name      BG (green)          FG (blue)           Expected (black = min)
        { GL_MIN,                   "MIN",   0,1,0,         0,0,1,      0,0,0,    "BLACK"  },
        // eq          name      BG (dark grey)      FG (yellow)         Expected (yellow = max)
        { GL_MAX,                   "MAX",   0.2f,0.2f,0,   1,1,0,      1,1,0,    "YELLOW" },
    };

    float rowH   = 480.0f / 5.0f;   // 96px per row
    float cellW  = 640.0f / 5.0f;   // 128px: label | bg | fg | result | expected

    for (int i = 0; i < 5; i++)
    {
        float y0 = i * rowH + 4.0f;
        float y1 = y0 + rowH - 8.0f;
        float mid = (y0 + y1) * 0.5f;

        // Cell 0: equation label
        float lx0 = 4.0f, lx1 = cellW - 4.0f;
        glDisable(GL_BLEND);
        glBegin(GL_QUADS); glColor3f(0.15f,0.15f,0.15f);
        glVertex2f(lx0,y0); glVertex2f(lx1,y0);
        glVertex2f(lx1,y1); glVertex2f(lx0,y1); glEnd();
        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(lx0+4, mid+8, 0); glScalef(16,16,1); DrawString(rows[i].eqName); glPopMatrix();

        // Cell 1: background colour swatch
        float bx0 = cellW + 4, bx1 = cellW*2 - 4;
        glBegin(GL_QUADS); glColor3f(rows[i].bgR, rows[i].bgG, rows[i].bgB);
        glVertex2f(bx0,y0); glVertex2f(bx1,y0);
        glVertex2f(bx1,y1); glVertex2f(bx0,y1); glEnd();
        glColor3f(0.6f,0.6f,0.6f);
        glPushMatrix();
        glTranslatef(bx0+4, y0+6, 0); glScalef(12,12,1); DrawString("BG"); glPopMatrix();

        // Cell 2: foreground colour swatch
        float fx0 = cellW*2 + 4, fx1 = cellW*3 - 4;
        glBegin(GL_QUADS); glColor3f(rows[i].fgR, rows[i].fgG, rows[i].fgB);
        glVertex2f(fx0,y0); glVertex2f(fx1,y0);
        glVertex2f(fx1,y1); glVertex2f(fx0,y1); glEnd();
        glColor3f(0.6f,0.6f,0.6f);
        glPushMatrix();
        glTranslatef(fx0+4, y0+6, 0); glScalef(12,12,1); DrawString("FG"); glPopMatrix();

        // Cell 3: ACTUAL blend result
        float rx0 = cellW*3 + 4, rx1 = cellW*4 - 4;
        // Draw BG
        glDisable(GL_BLEND);
        glBegin(GL_QUADS); glColor3f(rows[i].bgR, rows[i].bgG, rows[i].bgB);
        glVertex2f(rx0,y0); glVertex2f(rx1,y0);
        glVertex2f(rx1,y1); glVertex2f(rx0,y1); glEnd();
        // Blend FG over it
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(rows[i].eq);
        glBegin(GL_QUADS); glColor3f(rows[i].fgR, rows[i].fgG, rows[i].fgB);
        glVertex2f(rx0,y0); glVertex2f(rx1,y0);
        glVertex2f(rx1,y1); glVertex2f(rx0,y1); glEnd();
        glDisable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glColor3f(0.6f,0.6f,0.6f);
        glPushMatrix();
        glTranslatef(rx0+4, y0+6, 0); glScalef(12,12,1); DrawString("RESULT"); glPopMatrix();

        // Cell 4: expected colour swatch + label
        float ex0 = cellW*4 + 4, ex1 = 640.0f - 4;
        glBegin(GL_QUADS); glColor3f(rows[i].exR, rows[i].exG, rows[i].exB);
        glVertex2f(ex0,y0); glVertex2f(ex1,y0);
        glVertex2f(ex1,y1); glVertex2f(ex0,y1); glEnd();
        glColor3f(0.5f,0.5f,0.5f);
        glPushMatrix();
        glTranslatef(ex0+4, y0+6, 0); glScalef(12,12,1); DrawString(rows[i].exLabel); glPopMatrix();

        // Row divider
        glBegin(GL_LINES); glColor3f(0.3f,0.3f,0.3f);
        glVertex2f(0, y1+4); glVertex2f(640, y1+4); glEnd();
    }

    // Column headers at very top of screen -- draw after since ortho y=0 is bottom
    glColor3f(0.5f,0.5f,0.5f);
    // (labels already embedded in cells)
}

// ------------------------------------------------------------
// Test 44 - glPushAttrib / glPopAttrib
// 4 rows showing state is correctly saved and restored:
//
// Row 1 - GL_COLOR_BUFFER_BIT: set blend equation to SUBTRACT,
//         push, change to ADD, pop -- should restore SUBTRACT.
//         Visual: cyan bg + red fg should give black (SUB) not cyan (ADD)
//
// Row 2 - GL_LIGHTING_BIT: push lighting state, change material
//         to red, pop -- should restore original white material.
//         Visual: cube should be white not red after pop.
//
// Row 3 - GL_FOG_BIT: push fog, set thick blue fog, pop --
//         should restore no fog. Visual: floor should be green not blue.
//
// Row 4 - GL_ENABLE_BIT: push, enable blend+fog, pop --
//         should restore both disabled. Visual: solid red quad.
// ------------------------------------------------------------

static void Test44_PushPopAttrib()
{
    // LEFT half  = WITH glPopAttrib (state restored)
    // RIGHT half = WITHOUT glPopAttrib (state stays changed)
    // Each row tests a different attrib group.
    // Every row should look DIFFERENT left vs right if pop is working.
    //
    // Row (bottom): ENABLE BIT  - left=RED(blend off),  right=BLACK(blend zero)
    // Row 2:        LIGHTING    - left=WHITE cube,       right=RED cube
    // Row 3:        FOG         - left=GREEN floor,      right=BLUE floor
    // Row (top):    BLEND EQ    - left=WHITE(ADD),       right=BLACK(SUB)

    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);

    const float H = 120.0f;  // row height
    const float W = 320.0f;  // half width

    // ================================================================
    // ROW 0 (y=0..120, BOTTOM of screen): ENABLE BIT
    // LEFT:  push, enable blend(ZERO,ZERO), pop -> blend off -> RED
    // RIGHT: push, enable blend(ZERO,ZERO), no pop -> black
    // ================================================================
    SetGL2D(640, 480);
    for (int side = 0; side < 2; side++)
    {
        float x0 = side * W;
        float x1 = x0 + W;
        float y0 = 0, y1 = H;

        glDisable(GL_BLEND);
        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND); glBlendFunc(GL_ZERO, GL_ZERO);

        if (side == 0) glPopAttrib();  // LEFT: restores blend=off

        glBegin(GL_QUADS); glColor3f(1, 0.1f, 0.1f);
        glVertex2f(x0+2,y0+2); glVertex2f(x1-2,y0+2);
        glVertex2f(x1-2,y1-2); glVertex2f(x0+2,y1-2); glEnd();

        if (side == 1) glPopAttrib();
        glDisable(GL_BLEND);
    }

    // ================================================================
    // ROW 1 (y=120..240): LIGHTING MATERIAL
    // LEFT:  push, set red material, pop -> white cube
    // RIGHT: push, set red material, no pop -> red cube
    // ================================================================
    {
        GLfloat white[4]={1,1,1,1}, red[4]={1,0,0,1}, nospec[4]={0,0,0,1};
        GLfloat lpos[4]={1,1,2,0}, ld[4]={1,1,1,1}, la[4]={0.15f,0.15f,0.15f,1};

        for (int side = 0; side < 2; side++)
        {
            glViewport((int)(side*W), 120, (int)W, (int)H);

            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  white);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  white);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, nospec);

            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);

            if (side == 0) glPopAttrib();  // LEFT: restores white

            glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
            glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
            glLightfv(GL_LIGHT0,GL_POSITION,lpos);
            glLightfv(GL_LIGHT0,GL_DIFFUSE,ld);
            glLightfv(GL_LIGHT0,GL_AMBIENT,la);

            SetGLPerspective(60,1.0f,0.1f,20.0f);
            glTranslatef(0,0,-4);
            glRotatef(g_time*40,1,0,0); glRotatef(g_time*60,0,1,0);
            for (int f=0;f<6;f++) {
                static const GLfloat cn[6][3]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
                glBegin(GL_QUADS); glNormal3f(cn[f][0],cn[f][1],cn[f][2]);
                for (int v=0;v<4;v++) {
                    const GLfloat* p=cubeVerts[cubeFaces[f][v]];
                    glVertex3f(p[0],p[1],p[2]);
                }
                glEnd();
            }
            if (side == 1) glPopAttrib();
            glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
            glDisable(GL_NORMALIZE); glDisable(GL_DEPTH_TEST);
        }
    }

    // ================================================================
    // ROW 2 (y=240..360): FOG
    // LEFT:  push fog+enable, set thick blue fog, pop -> green floor
    // RIGHT: push fog+enable, set thick blue fog, no pop -> blue floor
    // ================================================================
    {
        GLfloat blueFog[4]={0.1f,0.1f,0.9f,1};
        GLfloat greyFog[4]={0.5f,0.5f,0.5f,1};

        for (int side = 0; side < 2; side++)
        {
            glViewport((int)(side*W), 240, (int)W, (int)H);
            glDisable(GL_FOG);
            glFogfv(GL_FOG_COLOR, greyFog);

            glPushAttrib(GL_FOG_BIT | GL_ENABLE_BIT);
            glFogfv(GL_FOG_COLOR, blueFog);
            glFogf(GL_FOG_START, 0.5f); glFogf(GL_FOG_END, 3.0f);
            glEnable(GL_FOG); glFogi(GL_FOG_MODE, GL_LINEAR);

            if (side == 0) glPopAttrib();  // LEFT: restores no fog

            SetGLPerspective(60,1.0f,0.1f,20.0f);
            glTranslatef(0,-0.5f,-0.3f); glRotatef(20,1,0,0);
            for (int r=0;r<8;r++) {
                float z0=-(float)r*0.9f, z1=z0-0.85f;
                glBegin(GL_QUADS);
                if (r&1) glColor3f(0.15f,0.85f,0.12f); else glColor3f(0.08f,0.40f,0.06f);
                glVertex3f(-1.5f,0,z0); glVertex3f(1.5f,0,z0);
                glVertex3f(1.5f,0,z1);  glVertex3f(-1.5f,0,z1); glEnd();
            }
            if (side == 1) glPopAttrib();
            glDisable(GL_FOG);
        }
    }

    // ================================================================
    // ROW 3 (y=360..480, TOP of screen): BLEND EQUATION
    // LEFT:  push ADD, change to SUB, pop -> ADD -> WHITE (cyan+red=white)
    // RIGHT: push ADD, change to SUB, no pop -> SUB -> BLACK (cyan-red=black)
    // ================================================================
    glViewport(0,0,640,480);
    SetGL2D(640,480);

    // LEFT side: push ADD, change to MIN, pop -> ADD restored -> WHITE
    // (grey + white with ADD = white)
    glBlendEquation(GL_FUNC_ADD);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glBlendEquation(GL_MIN);
    glPopAttrib();  // restores ADD
    glDisable(GL_BLEND);
    glBegin(GL_QUADS); glColor3f(0.5f,0.5f,0.5f);  // grey bg
    glVertex2f(2,362); glVertex2f(W-2,362); glVertex2f(W-2,478); glVertex2f(2,478); glEnd();
    glEnable(GL_BLEND); glBlendFunc(GL_ONE,GL_ONE);
    glBegin(GL_QUADS); glColor3f(1,1,1);  // white fg -- ADD = WHITE
    glVertex2f(2,362); glVertex2f(W-2,362); glVertex2f(W-2,478); glVertex2f(2,478); glEnd();
    glDisable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);

    // RIGHT side: push ADD, change to MIN, no pop -> MIN stays -> GREY
    // (grey + white with MIN = grey -- min of each channel)
    glBlendEquation(GL_FUNC_ADD);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glBlendEquation(GL_MIN);
    // NO POP -- MIN stays
    glDisable(GL_BLEND);
    glBegin(GL_QUADS); glColor3f(0.5f,0.5f,0.5f);  // grey bg
    glVertex2f(W+2,362); glVertex2f(638,362); glVertex2f(638,478); glVertex2f(W+2,478); glEnd();
    glEnable(GL_BLEND); glBlendFunc(GL_ONE,GL_ONE);
    glBegin(GL_QUADS); glColor3f(1,1,1);  // white fg -- MIN = GREY
    glVertex2f(W+2,362); glVertex2f(638,362); glVertex2f(638,478); glVertex2f(W+2,478); glEnd();
    glDisable(GL_BLEND);
    glPopAttrib();
    glBlendEquation(GL_FUNC_ADD);

    // ================================================================
    // Labels -- draw AFTER all rendering
    // ================================================================
    glViewport(0,0,640,480);
    SetGL2D(640,480);
    glDisable(GL_DEPTH_TEST);
    glColor3f(0.9f,0.9f,0.9f);

    struct { float cy; const char* left; const char* right; } rows[4] = {
        { 60,  "WITH POP=RED",   "NO POP=BLACK"  },
        { 180, "WITH POP=WHITE", "NO POP=RED"    },
        { 300, "WITH POP=GREEN", "NO POP=BLUE"   },
        { 420, "WITH POP=WHITE", "NO POP=GREY"   },
    };
    for (int i=0;i<4;i++) {
        glPushMatrix(); glTranslatef(W*0.5f, rows[i].cy, 0);
        DrawStringCentered(rows[i].left, 11.0f); glPopMatrix();
        glPushMatrix(); glTranslatef(W+W*0.5f, rows[i].cy, 0);
        DrawStringCentered(rows[i].right, 11.0f); glPopMatrix();
    }

    // Grid lines
    glBegin(GL_LINES); glColor3f(0.4f,0.4f,0.4f);
    glVertex2f(W,0);   glVertex2f(W,480);
    glVertex2f(0,H);   glVertex2f(640,H);
    glVertex2f(0,H*2); glVertex2f(640,H*2);
    glVertex2f(0,H*3); glVertex2f(640,H*3);
    glEnd();
}

// ------------------------------------------------------------
// Test 45 - glTexGen (texture coordinate generation)
// Three spheres side by side:
//   Left:   manual texcoords (reference -- no texgen)
//   Centre: GL_SPHERE_MAP texgen -- UVs auto-generated from normals
//           The texture should appear to reflect/slide as sphere rotates
//   Right:  GL_EYE_LINEAR -- UVs from eye-space position (plane fixed in eye space)
//           Texture slides based on position in view
//
// The key visual proof: centre sphere's texture moves differently
// from left (fixed UVs) -- proving texgen is generating coordinates.
// ------------------------------------------------------------

static void Test45_TexGen()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);

    // Use checker texture -- makes UV movement obvious
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    const float positions[3] = { -2.2f, 0.0f, 2.2f };
    const char* labels[3]    = { "MANUAL", "SPHERE MAP", "EYE LINEAR" };

    for (int i = 0; i < 3; i++)
    {
        SetGLPerspective(55, 640.0f/480.0f, 0.1f, 30.0f);
        glTranslatef(positions[i], 0, -7.0f);
        glRotatef(g_time * 30.0f, 0, 1, 0);

        // Set up texgen mode for this sphere
        if (i == 0)
        {
            // Manual -- no texgen, standard UV coordinates
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
        }
        else if (i == 1)
        {
            // Sphere map -- UVs from reflection vector
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
        }
        else
        {
            // EYE_LINEAR: UVs from eye-space position dotted with plane.
            // The plane is set BEFORE rotation so it stays fixed in eye space.
            // As the sphere rotates, the checker pattern slides across the surface.
            // glTexGenfv stores plane transformed by inv(MV) at call time per GL spec.
            GLfloat planeS[4] = {1.0f, 0.0f, 0.0f, 0.0f};
            GLfloat planeT[4] = {0.0f, 1.0f, 0.0f, 0.0f};
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGenfv(GL_S, GL_EYE_PLANE, planeS);
            glTexGenfv(GL_T, GL_EYE_PLANE, planeT);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
        }

        // Draw sphere
        DrawSphere40(1.0f, 16, 24);

        // Disable texgen for next sphere
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }

    // Labels
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);

    SetGLPerspective(55, 640.0f/480.0f, 0.1f, 30.0f);
    glTranslatef(0,0,-7.0f);

    glColor3f(0.9f,0.9f,0.9f);
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(positions[i], -1.6f, 0);
        glScalef(0.22f, 0.22f, 1);
        DrawStringCentered(labels[i], 1.0f);
        glPopMatrix();
    }
}

// ------------------------------------------------------------
// Test 46 - Client Array Torture Test
// 5 cells proving correct handling of packed/interleaved data:
//
//  [0] Non-zero stride       - vertex array with explicit stride > element size
//  [1] Interleaved struct    - VCTN packed struct, all arrays point into it
//  [2] DrawArrays first!=0   - skip first 2 verts, draw from index 2
//  [3] DrawElements UBYTE    - GL_UNSIGNED_BYTE index array
//  [4] Multi-texcoord array  - glClientActiveTexture switches coord arrays
//
// Each cell draws a coloured quad. Wrong result = exploded/missing/wrong colour.
// All should show solid coloured quads matching their labels.
// ------------------------------------------------------------

static void Test46_ClientArrays()
{
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    SetGL2D(640, 480);

    // 5 cells arranged in a row
    const int N = 5;
    float cw = 640.0f / N;  // cell width
    float pad = 8.0f;

    // ---- Cell 0: Non-zero stride ----
    // Vertex array has explicit stride of 24 bytes (6 floats) but only first 3 used
    // Extra padding floats between vertices should be skipped correctly
    {
        struct PaddedVert { float x,y,z; float pad0,pad1,pad2; };
        static const PaddedVert verts[4] = {
            {0,0,0, 99,99,99}, {1,0,0, 99,99,99},
            {1,1,0, 99,99,99}, {0,1,0, 99,99,99},
        };
        float x0=cw*0+pad, y0=60+pad, x1=cw*1-pad, y1=420-pad;
        // Scale verts to cell
        struct PaddedVert sv[4] = {
            {x0,y0,0, 0,0,0},{x1,y0,0, 0,0,0},
            {x1,y1,0, 0,0,0},{x0,y1,0, 0,0,0}
        };

        glColor3f(1,0.3f,0.3f);  // red -- color from glColor, not array
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        // stride = sizeof(PaddedVert) = 24 bytes, explicitly non-zero
        glVertexPointer(3, GL_FLOAT, sizeof(PaddedVert), sv);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    // ---- Cell 1: Interleaved struct (V+C+T packed) ----
    // Real engines use this constantly -- one struct, all arrays point into it
    {
        struct VCT {
            float x,y,z;       // vertex
            GLubyte r,g,b,a;   // color
            float u,v;         // texcoord (unused here)
        };
        float x0=cw*1+pad, y0=60+pad, x1=cw*2-pad, y1=420-pad;
        static VCT verts[4];
        verts[0].x=x0; verts[0].y=y0; verts[0].z=0; verts[0].r=0; verts[0].g=255; verts[0].b=100; verts[0].a=255; verts[0].u=0; verts[0].v=0;
        verts[1].x=x1; verts[1].y=y0; verts[1].z=0; verts[1].r=0; verts[1].g=255; verts[1].b=100; verts[1].a=255; verts[1].u=1; verts[1].v=0;
        verts[2].x=x1; verts[2].y=y1; verts[2].z=0; verts[2].r=0; verts[2].g=255; verts[2].b=100; verts[2].a=255; verts[2].u=1; verts[2].v=1;
        verts[3].x=x0; verts[3].y=y1; verts[3].z=0; verts[3].r=0; verts[3].g=255; verts[3].b=100; verts[3].a=255; verts[3].u=0; verts[3].v=1;

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT,         sizeof(VCT), &verts[0].x);
        glColorPointer (4, GL_UNSIGNED_BYTE, sizeof(VCT), &verts[0].r);
        glDrawArrays(GL_QUADS, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // ---- Cell 2: DrawArrays with first != 0 ----
    // Array has 6 verts. first=2 skips the first two (which are offscreen/wrong colour)
    {
        float x0=cw*2+pad, y0=60+pad, x1=cw*3-pad, y1=420-pad;
        // First 2 verts are junk (offscreen, wrong color) -- should be skipped
        static float vx[6*3];
        static GLubyte vc[6*4];
        // Junk verts at index 0,1
        vx[0]=9999; vx[1]=9999; vx[2]=0;   vc[0]=255; vc[1]=0;   vc[2]=0;   vc[3]=255;
        vx[3]=9999; vx[4]=9999; vx[5]=0;   vc[4]=255; vc[5]=0;   vc[6]=0;   vc[7]=255;
        // Real quad at index 2,3,4,5 -- blue
        vx[6]=x0;   vx[7]=y0;   vx[8]=0;   vc[8]=80;  vc[9]=80;  vc[10]=255; vc[11]=255;
        vx[9]=x1;   vx[10]=y0;  vx[11]=0;  vc[12]=80; vc[13]=80; vc[14]=255; vc[15]=255;
        vx[12]=x1;  vx[13]=y1;  vx[14]=0;  vc[16]=80; vc[17]=80; vc[18]=255; vc[19]=255;
        vx[15]=x0;  vx[16]=y1;  vx[17]=0;  vc[20]=80; vc[21]=80; vc[22]=255; vc[23]=255;

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT,         0, vx);
        glColorPointer (4, GL_UNSIGNED_BYTE, 0, vc);
        glDrawArrays(GL_QUADS, 2, 4);  // first=2, skip junk
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // ---- Cell 3: DrawElements with GL_UNSIGNED_BYTE indices ----
    // 6 vertices, indexed with ubyte indices to draw 2 triangles = quad
    {
        float x0=cw*3+pad, y0=60+pad, x1=cw*4-pad, y1=420-pad;
        static float vx[6*3];
        static GLubyte vc[6*4];
        // 4 unique verts for the quad
        vx[0]=x0; vx[1]=y0; vx[2]=0;  vc[0]=255; vc[1]=200; vc[2]=0; vc[3]=255; // yellow
        vx[3]=x1; vx[4]=y0; vx[5]=0;  vc[4]=255; vc[5]=200; vc[6]=0; vc[7]=255;
        vx[6]=x1; vx[7]=y1; vx[8]=0;  vc[8]=255; vc[9]=200; vc[10]=0; vc[11]=255;
        vx[9]=x0; vx[10]=y1; vx[11]=0; vc[12]=255; vc[13]=200; vc[14]=0; vc[15]=255;
        // Plus 2 junk verts at index 4,5 that should never be referenced
        vx[12]=9999; vx[13]=9999; vx[14]=0; vc[16]=255; vc[17]=0; vc[18]=0; vc[19]=255;
        vx[15]=9999; vx[16]=9999; vx[17]=0; vc[20]=255; vc[21]=0; vc[22]=0; vc[23]=255;

        // GL_UNSIGNED_BYTE index array -- two triangles making a quad
        static const GLubyte idx[6] = {0,1,2, 0,2,3};

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT,         0, vx);
        glColorPointer (4, GL_UNSIGNED_BYTE, 0, vc);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, idx);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // ---- Cell 4: Switching client active texture coord arrays ----
    // glClientActiveTexture(TEX0) sets one UV array,
    // glClientActiveTexture(TEX1) sets another.
    // Draw with texture showing correct UVs from TEX0.
    {
        float x0=cw*4+pad, y0=60+pad, x1=cw*5-pad, y1=420-pad;
        static float vx[4*3] = {0};
        static float uv0[4*2]; // correct UVs: full texture
        static float uv1[4*2]; // wrong UVs: degenerate (all 0)
        vx[0]=x0; vx[1]=y0; vx[2]=0;
        vx[3]=x1; vx[4]=y0; vx[5]=0;
        vx[6]=x1; vx[7]=y1; vx[8]=0;
        vx[9]=x0; vx[10]=y1; vx[11]=0;
        // UV set 0: proper 0..1 mapping
        uv0[0]=0; uv0[1]=0; uv0[2]=1; uv0[3]=0;
        uv0[4]=1; uv0[5]=1; uv0[6]=0; uv0[7]=1;
        // UV set 1: all zeros (wrong)
        for (int i=0;i<8;i++) uv1[i]=0;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glColor3f(1,1,1);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // Set TEX1 to wrong UVs first
        glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glTexCoordPointer(2, GL_FLOAT, 0, uv1);

        // Then set TEX0 to correct UVs -- this is what stage 0 should use
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glTexCoordPointer(2, GL_FLOAT, 0, uv0);

        glVertexPointer(3, GL_FLOAT, 0, vx);
        glDrawArrays(GL_QUADS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }

    // ---- Labels ----
    glColor3f(0.9f,0.9f,0.9f);
    const char* labels[N] = { "STRIDE", "INTERLEAVED", "FIRST!=0", "UBYTE IDX", "MULTITEX UV" };
    for (int i = 0; i < N; i++)
    {
        glPushMatrix();
        glTranslatef(cw*i + cw*0.5f, 30, 0);
        DrawStringCentered(labels[i], 11.0f);
        glPopMatrix();
    }

    // Grid lines
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
    for (int i=1;i<N;i++) {
        glVertex2f(cw*i,0); glVertex2f(cw*i,480);
    }
    glEnd();
}

// ------------------------------------------------------------
// Test 47 - Multitexture Edge Cases
// 4 cells:
//
//  [0] Two-stage blend: checker * rgba = tinted checker
//  [1] glActiveTexture switches bind target correctly
//  [2] Per-stage texenv: stage0=REPLACE, stage1=MODULATE
//  [3] Array multitex: two texcoord arrays via ClientActiveTexture
// ------------------------------------------------------------

static void Test47_Multitexture()
{
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    SetGL2D(640, 480);

    const float cw = 160.0f;
    const float pad = 8.0f;

    // ---- Cell 0: Two-stage blend ----
    // Stage 0: checker texture (REPLACE)
    // Stage 1: rgba texture (MODULATE) -- tints the checker
    // Result: tinted checker pattern
    {
        float x0=pad, y0=60+pad, x1=cw-pad, y1=420-pad;

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glBegin(GL_QUADS);
        glColor3f(1,1,1);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0,0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0,0); glVertex2f(x0,y0);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 2,0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1,0); glVertex2f(x1,y0);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 2,2); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1,1); glVertex2f(x1,y1);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0,2); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0,1); glVertex2f(x0,y1);
        glEnd();

        glActiveTextureARB(GL_TEXTURE1_ARB); glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB); glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 1: ActiveTexture bind target ----
    // Bind checker to unit 0, rgba to unit 1
    // Draw with only unit 0 enabled -- should show checker NOT rgba
    {
        float x0=cw+pad, y0=60+pad, x1=cw*2-pad, y1=420-pad;

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);  // unit 1 off -- should not affect result
        glBindTexture(GL_TEXTURE_2D, g_rgbaTex);

        glActiveTextureARB(GL_TEXTURE0_ARB);  // back to unit 0

        glBegin(GL_QUADS);
        glColor3f(1,1,1);
        glTexCoord2f(0,0); glVertex2f(x0,y0);
        glTexCoord2f(2,0); glVertex2f(x1,y0);
        glTexCoord2f(2,2); glVertex2f(x1,y1);
        glTexCoord2f(0,2); glVertex2f(x0,y1);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 2: Per-stage texenv modes ----
    // Stage 0: checker, REPLACE
    // Stage 1: same checker, ADD -- should brighten the result
    {
        float x0=cw*2+pad, y0=60+pad, x1=cw*3-pad, y1=420-pad;

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

        glBegin(GL_QUADS);
        glColor3f(1,1,1);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0,0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0,0); glVertex2f(x0,y0);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 2,0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 2,0); glVertex2f(x1,y0);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 2,2); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 2,2); glVertex2f(x1,y1);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0,2); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0,2); glVertex2f(x0,y1);
        glEnd();

        glActiveTextureARB(GL_TEXTURE1_ARB); glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB); glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 3: Array multitex via ClientActiveTexture ----
    // Two separate UV arrays, bound to stage 0 and stage 1 via ClientActiveTexture
    {
        float x0=cw*3+pad, y0=60+pad, x1=cw*4-pad, y1=420-pad;
        static float vx[4*3], uv0[4*2], uv1[4*2];
        vx[0]=x0; vx[1]=y0; vx[2]=0;
        vx[3]=x1; vx[4]=y0; vx[5]=0;
        vx[6]=x1; vx[7]=y1; vx[8]=0;
        vx[9]=x0; vx[10]=y1; vx[11]=0;
        // Stage 0: normal checker UVs
        uv0[0]=0; uv0[1]=0; uv0[2]=2; uv0[3]=0;
        uv0[4]=2; uv0[5]=2; uv0[6]=0; uv0[7]=2;
        // Stage 1: offset UVs for rgba texture
        uv1[0]=0; uv1[1]=0; uv1[2]=1; uv1[3]=0;
        uv1[4]=1; uv1[5]=1; uv1[6]=0; uv1[7]=1;

        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vx);

        glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, uv0);

        glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, uv1);

        glDrawArrays(GL_QUADS, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTextureARB(GL_TEXTURE1_ARB);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTextureARB(GL_TEXTURE0_ARB);

        glActiveTextureARB(GL_TEXTURE1_ARB); glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB); glDisable(GL_TEXTURE_2D);
    }

    // Labels
    glColor3f(0.9f,0.9f,0.9f);
    const char* labels[4] = { "2STAGE BLEND", "BIND TARGET", "TEXENV ADD", "ARRAY MULTITEX" };
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(cw*i + cw*0.5f, 30, 0);
        DrawStringCentered(labels[i], 11.0f);
        glPopMatrix();
    }

    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
    for (int i=1;i<4;i++) { glVertex2f(cw*i,0); glVertex2f(cw*i,480); }
    glEnd();
}

// ------------------------------------------------------------
// Test 48 - glDrawPixels / glRasterPos / glWindowPos
// 4 cells:
//
//  [0] glWindowPos + glDrawPixels RGBA -- coloured rect at exact screen pos
//  [1] glRasterPos in 2D ortho + glDrawPixels RGB
//  [2] glDrawPixels with GL_LUMINANCE -- greyscale gradient
//  [3] glPixelZoom 2x -- same image drawn twice as large
//
// If working: each cell shows a coloured rectangle at the correct position.
// If broken: black cell, wrong position, or wrong colours.
// ------------------------------------------------------------

static void Test48_DrawPixels()
{
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    const int CW = 160;
    const int CH = 360;
    const int PAD = 8;

    // Build test pixel data
    // 64x64 RGBA gradient (red->green left to right, blue top to bottom)
    static unsigned char pxRGBA[64*64*4];
    static unsigned char pxRGB[32*32*3];
    static unsigned char pxLUM[48*48];
    static bool pxBuilt = false;
    if (!pxBuilt)
    {
        pxBuilt = true;
        for (int y=0;y<64;y++) for (int x=0;x<64;x++)
        {
            unsigned char* p = pxRGBA + (y*64+x)*4;
            p[0] = (unsigned char)(x*4);        // R: left=0 right=255
            p[1] = (unsigned char)(255-x*4);    // G: left=255 right=0
            p[2] = (unsigned char)(y*4);        // B: bottom=0 top=255
            p[3] = 255;
        }
        for (int y=0;y<32;y++) for (int x=0;x<32;x++)
        {
            unsigned char* p = pxRGB + (y*32+x)*3;
            p[0] = 255; p[1] = (unsigned char)(x*8); p[2] = 0;  // orange gradient
        }
        for (int y=0;y<48;y++) for (int x=0;x<48;x++)
            pxLUM[y*48+x] = (unsigned char)((x+y)*255/96);
    }

    // ---- Cell 0: glWindowPos + RGBA ----
    {
        int x0 = PAD;
        int y0 = 60 + PAD;
        // glWindowPos goes directly to window coords -- no transform
        glWindowPos2i(x0, y0);
        glPixelZoom(1,1);
        glDrawPixels(64, 64, GL_RGBA, GL_UNSIGNED_BYTE, pxRGBA);
    }

    // ---- Cell 1: glRasterPos in 2D ortho + RGB ----
    {
        SetGL2D(640, 480);
        int x0 = CW + PAD;
        int y0 = 60 + PAD;
        glRasterPos2i(x0, y0);
        glPixelZoom(1,1);
        glDrawPixels(32, 32, GL_RGB, GL_UNSIGNED_BYTE, pxRGB);
    }

    // ---- Cell 2: GL_LUMINANCE ----
    {
        glWindowPos2i(CW*2 + PAD, 60 + PAD);
        glPixelZoom(1,1);
        glDrawPixels(48, 48, GL_LUMINANCE, GL_UNSIGNED_BYTE, pxLUM);
    }

    // ---- Cell 3: glPixelZoom 2x ----
    {
        glWindowPos2i(CW*3 + PAD, 60 + PAD);
        glPixelZoom(2.0f, 2.0f);
        glDrawPixels(32, 32, GL_RGB, GL_UNSIGNED_BYTE, pxRGB);
        glPixelZoom(1.0f, 1.0f);
    }

    // Labels
    SetGL2D(640, 480);
    glColor3f(0.9f,0.9f,0.9f);
    const char* labels[4] = { "WINDOWPOS RGBA", "RASTERPOS RGB", "LUMINANCE", "PIXELZOOM 2X" };
    for (int i=0;i<4;i++)
    {
        glPushMatrix();
        glTranslatef(CW*i + CW*0.5f, 30, 0);
        DrawStringCentered(labels[i], 11.0f);
        glPopMatrix();
    }

    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
    for (int i=1;i<4;i++) { glVertex2f((float)(CW*i),0); glVertex2f((float)(CW*i),480); }
    glEnd();
}

// ------------------------------------------------------------
// Test 49 - GL_COMBINE texenv
// 4 cells proving the most common GL_COMBINE modes work:
//
//  [0] MODULATE (checker * diffuse colour) -- baseline, same as GL_MODULATE
//  [1] ADD_SIGNED (checker + previous - 0.5) -- contrast boost
//  [2] INTERPOLATE (lerp between tex and colour using tex alpha)
//  [3] DOT3_RGB (fake normal map dot product lighting)
//
// Each cell should look distinct. If GL_COMBINE is broken,
// cells will show wrong colours or all look the same.
// ------------------------------------------------------------

static void Test49_Combine()
{
    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    SetGL2D(640, 480);

    const float cw = 160.0f;
    const float pad = 8.0f;
    const float y0 = 60 + pad, y1 = 420 - pad;

    // ---- Cell 0: GL_COMBINE MODULATE ----
    // combine RGB = MODULATE(TEXTURE, PRIMARY_COLOR)
    // Should look like checker tinted by glColor (red)
    {
        float x0=pad, x1=cw-pad;
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,       GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,       GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,       GL_PRIMARY_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,     GL_REPLACE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,     GL_TEXTURE);
        glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 1.0f);  // cyan tint -- white squares go cyan, black stay black
        glTexCoord2f(0,0); glVertex2f(x0,y0);
        glTexCoord2f(3,0); glVertex2f(x1,y0);
        glTexCoord2f(3,3); glVertex2f(x1,y1);
        glTexCoord2f(0,3); glVertex2f(x0,y1);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 1: GL_COMBINE ADD_SIGNED 2X ----
    // RGB = (TEXTURE + PRIMARY_COLOR - 0.5) * 2
    // High contrast, colours shifted
    {
        float x0=cw+pad, x1=cw*2-pad;
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,       GL_ADD_SIGNED);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,       GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,       GL_PRIMARY_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,      GL_SRC_COLOR);
        glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE,         2.0f);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,     GL_REPLACE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,     GL_TEXTURE);
        glBegin(GL_QUADS);
        glColor3f(0.3f, 1.0f, 0.2f);  // green -- add_signed shifts colours noticeably
        glTexCoord2f(0,0); glVertex2f(x0,y0);
        glTexCoord2f(3,0); glVertex2f(x1,y0);
        glTexCoord2f(3,3); glVertex2f(x1,y1);
        glTexCoord2f(0,3); glVertex2f(x0,y1);
        glEnd();
        glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
        glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 2: GL_COMBINE INTERPOLATE ----
    // Single stage: lerp between TEXTURE and CONSTANT using TEXTURE alpha
    // checker white squares (alpha=1) -> show TEXTURE colour (white)
    // checker black squares (alpha=0) -> show CONSTANT colour (orange)
    // Result: white squares stay white, black squares turn orange
    {
        float x0=cw*2+pad, x1=cw*3-pad;
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,       GL_INTERPOLATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,       GL_CONSTANT);   // orange (when weight=0)
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,       GL_TEXTURE);    // white/black checker (when weight=1)
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB,       GL_TEXTURE);    // use tex luminance as weight
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB,      GL_SRC_COLOR);  // white=show tex, black=show const
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,     GL_REPLACE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,     GL_TEXTURE);
        // Orange constant colour
        GLfloat orange[4] = {1.0f, 0.5f, 0.0f, 1.0f};
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, orange);
        glBegin(GL_QUADS);
        glColor3f(1,1,1);
        glTexCoord2f(0,0); glVertex2f(x0,y0);
        glTexCoord2f(3,0); glVertex2f(x1,y0);
        glTexCoord2f(3,3); glVertex2f(x1,y1);
        glTexCoord2f(0,3); glVertex2f(x0,y1);
        glEnd();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_TEXTURE_2D);
    }

    // ---- Cell 3: GL_COMBINE DOT3_RGB ----
    // Fake per-pixel dot3 lighting: dot(normalmap, lightdir)
    // Uses checker as a rough stand-in normal map, constant light direction
    {
        float x0=cw*3+pad, x1=cw*4-pad;
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,       GL_DOT3_RGB);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,       GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,       GL_CONSTANT);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,      GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,      GL_SRC_COLOR);
        // Light direction encoded as (L+1)/2 in RGBA: (0.5,0.7,1,1) = roughly up-left
        // Encode light dir as (L+1)/2. Using strong blue-purple tint.
        GLfloat lightDir[4] = {0.9f, 0.5f, 0.9f, 1.0f};
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, lightDir);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,     GL_REPLACE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,     GL_TEXTURE);
        glBegin(GL_QUADS);
        glColor3f(1,1,1);
        glTexCoord2f(0,0); glVertex2f(x0,y0);
        glTexCoord2f(3,0); glVertex2f(x1,y0);
        glTexCoord2f(3,3); glVertex2f(x1,y1);
        glTexCoord2f(0,3); glVertex2f(x0,y1);
        glEnd();
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_TEXTURE_2D);
    }

    // Labels
    glColor3f(0.9f,0.9f,0.9f);
    const char* labels[4] = { "MODULATE", "ADD SIGNED 2X", "INTERPOLATE", "DOT3 RGB" };
    for (int i=0;i<4;i++)
    {
        glPushMatrix();
        glTranslatef(cw*i + cw*0.5f, 30, 0);
        DrawStringCentered(labels[i], 11.0f);
        glPopMatrix();
    }
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
    for (int i=1;i<4;i++) { glVertex2f(cw*i,0); glVertex2f(cw*i,480); }
    glEnd();
}
