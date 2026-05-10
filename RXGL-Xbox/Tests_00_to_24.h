/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_00_to_24.h  --  Original RXGL compatibility test suite, tests 0-24.
 *                       Covers core GL 1.x features: clear, basic primitives,
 *                       colour, depth, alpha blending, texturing, matrix transforms,
 *                       fog, lighting, and stencil. Included by Tests_Shared.h.
 *                       Stable -- only edit to fix bugs, do not add new tests here.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

static void Test0_ClearOnly()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);

    float pulse = sinf_approx(g_time * 2.0f) * 0.5f + 0.5f;
    glClearColor(0.05f, 0.05f + pulse * 0.25f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void Test1_CoordinateOrientation()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glBegin(GL_QUADS);
    glColor3f(1, 0, 0); glVertex2f(0, 0); glVertex2f(120, 0); glVertex2f(120, 120); glVertex2f(0, 120);
    glColor3f(0, 1, 0); glVertex2f(0, 360); glVertex2f(120, 360); glVertex2f(120, 480); glVertex2f(0, 480);
    glColor3f(0, 0, 1); glVertex2f(520, 0); glVertex2f(640, 0); glVertex2f(640, 120); glVertex2f(520, 120);
    glColor3f(1, 1, 0); glVertex2f(520, 360); glVertex2f(640, 360); glVertex2f(640, 480); glVertex2f(520, 480);
    glEnd();

    glLoadIdentity();
    glTranslatef(320, 240, 0);
    glRotatef(g_time * 60.0f, 0, 0, 1);

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 1); glVertex2f(0, 100);
    glColor3f(0, 1, 1); glVertex2f(-100, -80);
    glColor3f(1, 1, 1); glVertex2f(100, -80);
    glEnd();
}

static void Test2_Primitives()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0); glVertex2f(80, 80);
    glColor3f(0, 1, 0); glVertex2f(220, 80);
    glColor3f(0, 0, 1); glVertex2f(150, 220);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1, 1, 0);
    glVertex2f(300, 80);
    glVertex2f(520, 80);
    glVertex2f(520, 220);
    glVertex2f(300, 220);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1, 0, 1); glVertex2f(80, 300);
    glColor3f(0, 1, 1); glVertex2f(80, 420);
    glColor3f(1, 1, 1); glVertex2f(200, 300);
    glColor3f(1, 0.5f, 0); glVertex2f(200, 420);
    glColor3f(0.5f, 1, 0); glVertex2f(320, 300);
    glColor3f(0, 0.5f, 1); glVertex2f(320, 420);
    glEnd();
}

static void Test3_Depth()
{
    glClearColor(0.05f, 0.05f, 0.12f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGLPerspective(60, 640.0f / 480.0f, 0.1f, 100.0f);

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex3f(-1.8f, -1.0f, -4.0f);
    glVertex3f(0.8f, -1.0f, -4.0f);
    glVertex3f(-0.5f, 1.3f, -4.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0, 1, 0);
    glVertex3f(-0.3f, -0.5f, -2.0f);
    glVertex3f(1.1f, -0.5f, -2.0f);
    glVertex3f(0.4f, 0.8f, -2.0f);
    glEnd();

    glDisable(GL_DEPTH_TEST);
}

static const GLfloat cubeVerts[8][3] =
{
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}
};

static const int cubeFaces[6][4] =
{
    {4, 5, 6, 7},
    {1, 0, 3, 2},
    {0, 4, 7, 3},
    {5, 1, 2, 6},
    {7, 6, 2, 3},
    {0, 1, 5, 4}
};

static const GLfloat cubeColors[6][3] =
{
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
    {1, 1, 0}, {0, 1, 1}, {1, 0, 1}
};

// Face label names for Test4 (drawn with GL_LINES in object space)
static const char* cubeFaceNames[6] = { "+Z", "-Z", "-X", "+X", "+Y", "-Y" };

// Draw a single digit/char using line segments.
// Coords are in a local [-0.5,0.5] space; caller has already
// translated/scaled into face-local position.
static void DrawChar(char c)
{
    // 7-segment style line-art glyphs. All drawn in [-0.12, 0.12] x [-0.22, 0.22].
    // Segments: T=top, M=mid, B=bot, TL=top-left, TR=top-right, BL=bot-left, BR=bot-right
    #define T  glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.12f, 0.22f,0);
    #define M  glVertex3f(-0.12f, 0.00f,0); glVertex3f( 0.12f, 0.00f,0);
    #define B  glVertex3f(-0.12f,-0.22f,0); glVertex3f( 0.12f,-0.22f,0);
    #define TL glVertex3f(-0.12f, 0.22f,0); glVertex3f(-0.12f, 0.00f,0);
    #define TR glVertex3f( 0.12f, 0.22f,0); glVertex3f( 0.12f, 0.00f,0);
    #define BL glVertex3f(-0.12f, 0.00f,0); glVertex3f(-0.12f,-0.22f,0);
    #define BR glVertex3f( 0.12f, 0.00f,0); glVertex3f( 0.12f,-0.22f,0);
    #define VL glVertex3f(-0.12f, 0.22f,0); glVertex3f(-0.12f,-0.22f,0);
    #define VR glVertex3f( 0.12f, 0.22f,0); glVertex3f( 0.12f,-0.22f,0);
    // Diagonals for X,K,V,W,Y,Z
    #define DL glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.12f,-0.22f,0);
    #define DR glVertex3f( 0.12f, 0.22f,0); glVertex3f(-0.12f,-0.22f,0);

    glBegin(GL_LINES);
    switch (c)
    {
    case '0': T TL TR BL BR B break;
    case '1': TR BR break;
    case '2': T TR M BL B break;
    case '3': T TR M BR B break;
    case '4': TL TR M BR break;
    case '5': T TL M BR B break;
    case '6': T TL M BL BR B break;
    case '7': T TR BR break;
    case '8': T TL TR M BL BR B break;
    case '9': T TL TR M BR B break;
    case 'A': T TL TR M BL BR break;
    case 'B': // like 8 but flat left side
        glVertex3f(-0.12f, 0.22f,0); glVertex3f(-0.12f,-0.22f,0);
        glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.08f, 0.22f,0);
        glVertex3f( 0.08f, 0.22f,0); glVertex3f( 0.12f, 0.18f,0);
        glVertex3f( 0.12f, 0.18f,0); glVertex3f( 0.12f, 0.04f,0);
        glVertex3f( 0.12f, 0.04f,0); glVertex3f(-0.12f, 0.00f,0);
        glVertex3f(-0.12f, 0.00f,0); glVertex3f( 0.12f,-0.04f,0);
        glVertex3f( 0.12f,-0.04f,0); glVertex3f( 0.12f,-0.18f,0);
        glVertex3f( 0.12f,-0.18f,0); glVertex3f(-0.12f,-0.22f,0);
        break;
    case 'C': T TL BL B break;
    case 'D':
        glVertex3f(-0.12f, 0.22f,0); glVertex3f(-0.12f,-0.22f,0);
        glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.06f, 0.22f,0);
        glVertex3f( 0.06f, 0.22f,0); glVertex3f( 0.12f, 0.14f,0);
        glVertex3f( 0.12f, 0.14f,0); glVertex3f( 0.12f,-0.14f,0);
        glVertex3f( 0.12f,-0.14f,0); glVertex3f( 0.06f,-0.22f,0);
        glVertex3f( 0.06f,-0.22f,0); glVertex3f(-0.12f,-0.22f,0);
        break;
    case 'E': T TL M BL B break;
    case 'F': T TL M BL break;
    case 'G': T TL BL B BR M
        glVertex3f( 0.12f, 0.00f,0); glVertex3f( 0.12f, 0.00f,0); break; // just M on right
    case 'H': VL VR M break;
    case 'I': T B
        glVertex3f( 0.00f, 0.22f,0); glVertex3f( 0.00f,-0.22f,0); break;
    case 'J': TR BR B
        glVertex3f(-0.08f,-0.22f,0); glVertex3f( 0.12f,-0.22f,0); break;
    case 'K': VL
        glVertex3f(-0.12f, 0.00f,0); glVertex3f( 0.12f, 0.22f,0);
        glVertex3f(-0.12f, 0.00f,0); glVertex3f( 0.12f,-0.22f,0); break;
    case 'L': VL B break;
    case 'M': VL VR
        glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.00f, 0.00f,0);
        glVertex3f( 0.12f, 0.22f,0); glVertex3f( 0.00f, 0.00f,0); break;
    case 'N': VL VR
        glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.12f,-0.22f,0); break;
    case 'O': T TL TR BL BR B break;
    case 'P': T TL TR M BL break;
    case 'Q': T TL TR BL B
        glVertex3f( 0.00f,-0.00f,0); glVertex3f( 0.12f,-0.22f,0); break;
    case 'R': T TL TR M BL
        glVertex3f( 0.00f, 0.00f,0); glVertex3f( 0.12f,-0.22f,0); break;
    case 'S': T TL M BR B break;
    case 'T': T
        glVertex3f( 0.00f, 0.22f,0); glVertex3f( 0.00f,-0.22f,0); break;
    case 'U': VL VR B break;
    case 'V': glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.00f,-0.22f,0);
              glVertex3f( 0.12f, 0.22f,0); glVertex3f( 0.00f,-0.22f,0); break;
    case 'W': VL VR
        glVertex3f(-0.12f,-0.22f,0); glVertex3f( 0.00f, 0.00f,0);
        glVertex3f( 0.12f,-0.22f,0); glVertex3f( 0.00f, 0.00f,0); break;
    case 'X': DL DR break;
    case 'Y': glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.00f, 0.00f,0);
              glVertex3f( 0.12f, 0.22f,0); glVertex3f( 0.00f, 0.00f,0);
              glVertex3f( 0.00f, 0.00f,0); glVertex3f( 0.00f,-0.22f,0); break;
    case 'Z': T DR B break;
    case '+': M glVertex3f( 0.00f, 0.11f,0); glVertex3f( 0.00f,-0.11f,0); break;
    case '-': M break;
    case '.': glVertex3f(-0.03f,-0.20f,0); glVertex3f( 0.03f,-0.20f,0); break;
    case '/': DR break;
    case ':': glVertex3f(-0.03f, 0.10f,0); glVertex3f( 0.03f, 0.10f,0);
              glVertex3f(-0.03f,-0.10f,0); glVertex3f( 0.03f,-0.10f,0); break;
    case ' ': break;
    default:  glVertex3f(-0.12f, 0.22f,0); glVertex3f( 0.12f,-0.22f,0); break; // ? mark diagonal
    }
    glEnd();

    #undef T
    #undef M
    #undef B
    #undef TL
    #undef TR
    #undef BL
    #undef BR
    #undef VL
    #undef VR
    #undef DL
    #undef DR
}


// DrawString — renders a string of characters at the current matrix position.
// Each character is 0.3 units wide in local space. Call after glTranslatef/glScalef.
// Example:
//   glTranslatef(x, y, 0); glScalef(20, 20, 1);
//   DrawString("HELLO XBOX");
static void DrawString(const char* s)
{
    if (!s) return;
    glPushMatrix();
    for (int i = 0; s[i]; i++)
    {
        DrawChar(s[i]);
        glTranslatef(0.36f, 0.0f, 0.0f);
    }
    glPopMatrix();
}

// DrawStringCentered — same but centers the string around the current position.
static void DrawStringCentered(const char* s, float scale)
{
    if (!s) return;
    int len = 0;
    for (int i = 0; s[i]; i++) len++;
    glPushMatrix();
    glTranslatef(-(len - 1) * 0.36f * 0.5f * scale, 0, 0);
    glScalef(scale, scale, 1);
    DrawString(s);
    glPopMatrix();
}

// Draw a two-character label (e.g. "+Z") centered on a face.
// The face is in object space; we draw slightly in front of it (bias).
static void DrawFaceLabel(const char* label, float bias)
{
    glPushMatrix();
    glTranslatef(-0.18f, 0.0f, bias);  // left char position
    DrawChar(label[0]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 0.18f, 0.0f, bias);  // right char position
    DrawChar(label[1]);
    glPopMatrix();
}

// Face centres and orientations in object space.
// For each face we need a translation + rotation to bring the
// label into face-local space.
static void DrawCubeFaceLabels()
{
    glDisable(GL_DEPTH_TEST);  // always draw labels on top
    glColor3f(0.0f, 0.0f, 0.0f);  // black label on coloured face

    // +Z face (face 0) — front, normal (0,0,1)
    glPushMatrix();
    glTranslatef(0, 0, 1.01f);
    DrawFaceLabel(cubeFaceNames[0], 0.0f);
    glPopMatrix();

    // -Z face (face 1) — back, normal (0,0,-1), flip X so it reads forward
    glPushMatrix();
    glTranslatef(0, 0, -1.01f);
    glRotatef(180, 0, 1, 0);
    DrawFaceLabel(cubeFaceNames[1], 0.0f);
    glPopMatrix();

    // -X face (face 2) — left, normal (-1,0,0)
    glPushMatrix();
    glTranslatef(-1.01f, 0, 0);
    glRotatef(-90, 0, 1, 0);
    DrawFaceLabel(cubeFaceNames[2], 0.0f);
    glPopMatrix();

    // +X face (face 3) — right, normal (1,0,0)
    glPushMatrix();
    glTranslatef(1.01f, 0, 0);
    glRotatef(90, 0, 1, 0);
    DrawFaceLabel(cubeFaceNames[3], 0.0f);
    glPopMatrix();

    // +Y face (face 4) — top, normal (0,1,0)
    glPushMatrix();
    glTranslatef(0, 1.01f, 0);
    glRotatef(-90, 1, 0, 0);
    DrawFaceLabel(cubeFaceNames[4], 0.0f);
    glPopMatrix();

    // -Y face (face 5) — bottom, normal (0,-1,0)
    glPushMatrix();
    glTranslatef(0, -1.01f, 0);
    glRotatef(90, 1, 0, 0);
    DrawFaceLabel(cubeFaceNames[5], 0.0f);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
}

static void Test4_CullingCube()
{
    glClearColor(0.03f, 0.03f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    SetGLPerspective(60, 640.0f / 480.0f, 0.1f, 50.0f);

    glTranslatef(0, 0, -5);

    // Each phase rotates one axis at a time so PC and Xbox can be
    // compared directly. DPad Up/Down cycles phases. Time resets
    // each phase change so both platforms start from the same angle.
    switch (g_rotPhase)
    {
    case 0: // No rotation — fixed reference pose
        break;
    case 1: // Y axis only
        glRotatef(g_time * 50.0f, 0, 1, 0);
        break;
    case 2: // X axis only
        glRotatef(g_time * 35.0f, 1, 0, 0);
        break;
    case 3: // Z axis only
        glRotatef(g_time * 40.0f, 0, 0, 1);
        break;
    case 4: // X + Y combined (original behaviour)
        glRotatef(g_time * 35.0f, 1, 0, 0);
        glRotatef(g_time * 55.0f, 0, 1, 0);
        break;
    }

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

    DrawCubeFaceLabels();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Draw rotation phase label in 2D so it's readable on screen
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    SetGL2D(640, 480);

    static const char* phaseLabels[5] =
    {
        "PHASE 0: NO ROTATION (reference)",
        "PHASE 1: Y-AXIS ONLY",
        "PHASE 2: X-AXIS ONLY",
        "PHASE 3: Z-AXIS ONLY",
        "PHASE 4: X+Y COMBINED"
    };

    // Draw the phase name as a simple line-art banner at the bottom
    // using coloured lines so it's visible on both platforms
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    // Simple underline to indicate which phase
    float barW = 40.0f + g_rotPhase * 120.0f;
    glVertex2f(20.0f, 30.0f); glVertex2f(barW, 30.0f);
    glEnd();

    // Draw phase number as a large digit at bottom-left
    glPushMatrix();
    glTranslatef(40.0f, 50.0f, 0.0f);
    glScalef(80.0f, 80.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    DrawChar('0' + g_rotPhase);
    glPopMatrix();

    // Draw axis indicator chars next to it
    glPushMatrix();
    glTranslatef(120.0f, 50.0f, 0.0f);
    glScalef(80.0f, 80.0f, 1.0f);
    const char axisChars[5][2] = {{'_','_'},{'Y','_'},{'X','_'},{'Z','_'},{'X','Y'}};
    glColor3f(0.8f, 0.8f, 0.8f);
    DrawChar(axisChars[g_rotPhase][0]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(200.0f, 50.0f, 0.0f);
    glScalef(80.0f, 80.0f, 1.0f);
    glColor3f(0.8f, 0.8f, 0.8f);
    DrawChar(axisChars[g_rotPhase][1]);
    glPopMatrix();
}

static void Test5_TextureOrientation()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    SetGL2D(640, 480);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0, 0); glVertex2f(160, 120);
    glTexCoord2f(1, 0); glVertex2f(480, 120);
    glTexCoord2f(1, 1); glVertex2f(480, 360);
    glTexCoord2f(0, 1); glVertex2f(160, 360);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

static void Test6_BlendTexture()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    SetGL2D(640, 480);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glBegin(GL_QUADS);
    glColor3f(0, 0, 1);
    glVertex2f(80, 80);
    glVertex2f(560, 80);
    glColor3f(1, 0, 0);
    glVertex2f(560, 400);
    glVertex2f(80, 400);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLoadIdentity();
    glTranslatef(320, 240, 0);
    glRotatef(g_time * 30.0f, 0, 0, 1);

    glBegin(GL_QUADS);
    glColor4f(1, 1, 1, 0.5f);
    glTexCoord2f(0, 0); glVertex2f(-220, -140);
    glTexCoord2f(4, 0); glVertex2f(220, -140);
    glTexCoord2f(4, 3); glVertex2f(220, 140);
    glTexCoord2f(0, 3); glVertex2f(-220, 140);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

static const GLfloat vaVerts[] =
{
    -1, -1, 0,
     1, -1, 0,
     1,  1, 0,
    -1,  1, 0
};

static const GLubyte vaColors[] =
{
    255, 0, 0, 255,
    0, 255, 0, 255,
    0, 0, 255, 255,
    255, 255, 0, 255
};

static const GLfloat vaTex[] =
{
    0, 0,
    1, 0,
    1, 1,
    0, 1
};

static const GLushort vaIndices[] =
{
    0, 1, 2,
    0, 2, 3
};

static void Test7_VertexArrays()
{
    glClearColor(0.08f, 0.02f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    SetGLPerspective(60, 640.0f / 480.0f, 0.1f, 20.0f);

    glTranslatef(0, 0, -3);
    glRotatef(g_time * 80.0f, 0, 1, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vaVerts);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, vaColors);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, vaIndices);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_DEPTH_TEST);
}

static void Test8_ScissorColorMask()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    struct Q
    {
        GLint x, y, w, h;
        GLboolean r, g, b, a;
    };

    Q q[4] =
    {
        {0,   240, 320, 240, GL_TRUE,  GL_TRUE,  GL_TRUE,  GL_TRUE},
        {320, 240, 320, 240, GL_FALSE, GL_TRUE,  GL_TRUE,  GL_TRUE},
        {0,     0, 320, 240, GL_TRUE,  GL_FALSE, GL_TRUE,  GL_TRUE},
        {320,   0, 320, 240, GL_TRUE,  GL_TRUE,  GL_FALSE, GL_TRUE}
    };

    for (int i = 0; i < 4; i++)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(q[i].x, q[i].y, q[i].w, q[i].h);
        glColorMask(q[i].r, q[i].g, q[i].b, q[i].a);

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);
        glVertex2f((float)q[i].x, (float)q[i].y);
        glVertex2f((float)(q[i].x + q[i].w), (float)q[i].y);
        glVertex2f((float)(q[i].x + q[i].w), (float)(q[i].y + q[i].h));
        glVertex2f((float)q[i].x, (float)(q[i].y + q[i].h));
        glEnd();
    }

    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

static void UpdateDynamicSubTexture()
{
    int frame = (int)(g_time * 8.0f);
    if (frame == g_subFrame)
        return;

    g_subFrame = frame;

    FillDynamicBase();

    glBindTexture(GL_TEXTURE_2D, g_dynamicTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, g_dynamicData);

    int cell = frame & 15;
    int sx = (cell & 3) * 16;
    int sy = ((cell >> 2) & 3) * 16;

    GLubyte r = (GLubyte)((frame & 1) ? 255 : 0);
    GLubyte g = (GLubyte)((frame & 2) ? 255 : 0);
    GLubyte b = (GLubyte)((frame & 4) ? 255 : 0);

    if (r == 0 && g == 0 && b == 0)
    {
        r = 255;
        g = 255;
        b = 255;
    }

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            GLubyte* p = &g_subData[(y * 16 + x) * 4];
            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = 255;
        }
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, sx, sy, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, g_subData);
}

static void Test9_TexSubImage2D()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    UpdateDynamicSubTexture();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_dynamicTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    SetGL2D(640, 480);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0, 0); glVertex2f(160, 80);
    glTexCoord2f(1, 0); glVertex2f(480, 80);
    glTexCoord2f(1, 1); glVertex2f(480, 400);
    glTexCoord2f(0, 1); glVertex2f(160, 400);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

static void MakeDecalAlphaTexture()
{
    for (int y = 0; y < TEX_SIZE; y++)
    {
        for (int x = 0; x < TEX_SIZE; x++)
        {
            GLubyte* p = &g_dynamicData[(y * TEX_SIZE + x) * 4];

            int checker = ((x / 8) + (y / 8)) & 1;
            if (checker)
            {
                p[0] = 255; p[1] = 255; p[2] = 255;
            }
            else
            {
                p[0] = 0; p[1] = 255; p[2] = 255;
            }

            p[3] = (GLubyte)((x * 255) / (TEX_SIZE - 1));
        }
    }

    glBindTexture(GL_TEXTURE_2D, g_dynamicTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, g_dynamicData);
}

static void DrawTexEnvQuad(float x0, float y0, float x1, float y1, GLint mode, float cr, float cg, float cb)
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);

    float u0 = 0.5f / 4.0f;
    float v0 = 0.5f / 4.0f;
    float u1 = 3.5f / 4.0f;
    float v1 = 3.5f / 4.0f;

    glBegin(GL_QUADS);
    glColor4f(cr, cg, cb, 1.0f);
    glTexCoord2f(u0, v0); glVertex2f(x0, y0);
    glTexCoord2f(u1, v0); glVertex2f(x1, y0);
    glTexCoord2f(u1, v1); glVertex2f(x1, y1);
    glTexCoord2f(u0, v1); glVertex2f(x0, y1);
    glEnd();
}

static void DrawFullTexEnvQuad(float x0, float y0, float x1, float y1, GLint mode, float cr, float cg, float cb)
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);

    glBegin(GL_QUADS);
    glColor4f(cr, cg, cb, 1.0f);
    glTexCoord2f(0, 0); glVertex2f(x0, y0);
    glTexCoord2f(1, 0); glVertex2f(x1, y0);
    glTexCoord2f(1, 1); glVertex2f(x1, y1);
    glTexCoord2f(0, 1); glVertex2f(x0, y1);
    glEnd();
}

static void Test10_TexEnvModes()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    SetGL2D(640, 480);

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3f(0.12f, 0.12f, 0.12f);
    glVertex2f(50, 40);
    glVertex2f(590, 40);
    glVertex2f(590, 430);
    glVertex2f(50, 430);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    DrawTexEnvQuad(96, 280, 224, 408, GL_MODULATE, 0.5f, 1.0f, 1.0f);
    DrawTexEnvQuad(416, 280, 544, 408, GL_REPLACE, 0.0f, 1.0f, 0.0f);
    DrawTexEnvQuad(96, 72, 224, 200, GL_DECAL, 1.0f, 0.0f, 0.0f);

    GLfloat envColor[4] = {1.0f, 0.0f, 1.0f, 1.0f};
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envColor);
    DrawTexEnvQuad(416, 72, 544, 200, GL_BLEND, 0.2f, 0.8f, 1.0f);

    MakeDecalAlphaTexture();

    glBindTexture(GL_TEXTURE_2D, g_dynamicTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    DrawFullTexEnvQuad(256, 176, 384, 304, GL_DECAL, 1.0f, 0.0f, 0.0f);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);
}

static void Test11_Fog()
{
    GLfloat fogColor[4] = {0.45f, 0.45f, 0.65f, 1.0f};

    glClearColor(fogColor[0], fogColor[1], fogColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 2.0f);
    glFogf(GL_FOG_END, 12.0f);
    glFogfv(GL_FOG_COLOR, fogColor);

    SetGLPerspective(70, 640.0f / 480.0f, 0.1f, 30.0f);

    glTranslatef(0, -1.2f, -2.0f);
    glRotatef(22.0f, 1, 0, 0);

    for (int row = 0; row < 12; row++)
    {
        for (int col = -5; col < 5; col++)
        {
            float z0 = -(float)row;
            float z1 = -(float)(row + 1);
            float x0 = (float)col * 0.9f;
            float x1 = (float)(col + 1) * 0.9f;

            int checker = (row + col) & 1;

            glBegin(GL_QUADS);
            if (checker)
                glColor3f(0.2f, 0.8f, 0.15f);
            else
                glColor3f(0.1f, 0.35f, 0.08f);

            glVertex3f(x0, 0, z0);
            glVertex3f(x1, 0, z0);
            glVertex3f(x1, 0, z1);
            glVertex3f(x0, 0, z1);
            glEnd();
        }
    }

    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
}

// ------------------------------------------------------------
// Tests 12-24
// ------------------------------------------------------------

static void Test12_Stencil()
{
    glClearColor(0.04f, 0.04f, 0.04f, 1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    SetGL2D(640, 480);

    // ----------------------------------------------------
    // PASS 1:
    // Write stencil = 1 into LEFT rectangle
    // ----------------------------------------------------

    glEnable(GL_STENCIL_TEST);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glBegin(GL_QUADS);
        glVertex2f(80, 120);
        glVertex2f(280, 120);
        glVertex2f(280, 360);
        glVertex2f(80, 360);
    glEnd();

    // ----------------------------------------------------
    // PASS 2:
    // Write stencil = 2 into RIGHT rectangle
    // ----------------------------------------------------

    glStencilFunc(GL_ALWAYS, 2, 0xFF);

    glBegin(GL_QUADS);
        glVertex2f(360, 120);
        glVertex2f(560, 120);
        glVertex2f(560, 360);
        glVertex2f(360, 360);
    glEnd();

    // ----------------------------------------------------
    // PASS 3:
    // Draw RED where stencil == 1
    // ----------------------------------------------------

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0);

        glVertex2f(40, 80);
        glVertex2f(600, 80);
        glVertex2f(600, 400);
        glVertex2f(40, 400);
    glEnd();

    // ----------------------------------------------------
    // PASS 4:
    // Draw CYAN where stencil == 2
    // ----------------------------------------------------

    glStencilFunc(GL_EQUAL, 2, 0xFF);

    glBegin(GL_QUADS);
        glColor3f(0, 0.8f, 1);

        glVertex2f(40, 80);
        glVertex2f(600, 80);
        glVertex2f(600, 400);
        glVertex2f(40, 400);
    glEnd();

    // ----------------------------------------------------
    // PASS 5:
    // Draw GREEN where stencil != 0
    // Border overlay test
    // ----------------------------------------------------

// ----------------------------------------------------
// PASS 5:
// Draw GREEN inset outlines where stencil != 0
// Inset avoids line pixels falling outside stencil edge.
// ----------------------------------------------------

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glBegin(GL_LINES);

	glColor3f(0, 1, 0);

	// left rect, inset by 1 pixel
	glVertex2f(81, 121);  glVertex2f(279, 121);
	glVertex2f(279, 121); glVertex2f(279, 359);
	glVertex2f(279, 359); glVertex2f(81, 359);
	glVertex2f(81, 359);  glVertex2f(81, 121);

	// right rect, inset by 1 pixel
	glVertex2f(361, 121); glVertex2f(559, 121);
	glVertex2f(559, 121); glVertex2f(559, 359);
	glVertex2f(559, 359); glVertex2f(361, 359);
	glVertex2f(361, 359); glVertex2f(361, 121);

	glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_STENCIL_TEST);
}

static void Test13_AlphaTest()
{
    glClearColor(0.12f, 0.12f, 0.12f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.3f, 0.8f);
    glVertex2f(120, 80);
    glVertex2f(520, 80);
    glVertex2f(520, 400);
    glVertex2f(120, 400);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_alphaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0, 0); glVertex2f(220, 140);
    glTexCoord2f(1, 0); glVertex2f(420, 140);
    glTexCoord2f(1, 1); glVertex2f(420, 340);
    glTexCoord2f(0, 1); glVertex2f(220, 340);
    glEnd();

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

static void Test14_DepthMask()
{
    glClearColor(0.05f, 0.05f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    // LEFT: depth writes OFF.
    // Green triangle is farther away but should still draw.
    glDepthMask(GL_FALSE);

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

    // RIGHT: depth writes ON.
    // Cyan triangle is farther away and should be hidden.
    glClear(GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_TRUE);

    glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f(340, 100, -0.2f);
        glVertex3f(580, 100, -0.2f);
        glVertex3f(580, 380, -0.2f);
        glVertex3f(340, 380, -0.2f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(0, 1, 1);
        glVertex3f(460, 330, -0.6f);
        glVertex3f(370, 150, -0.6f);
        glVertex3f(550, 150, -0.6f);
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_DEPTH_TEST);
}

static void DrawWrappedQuad(float x0, float y0, float x1, float y1)
{
    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(-1, -1); glVertex2f(x0, y0);
    glTexCoord2f(3, -1);  glVertex2f(x1, y0);
    glTexCoord2f(3, 3);   glVertex2f(x1, y1);
    glTexCoord2f(-1, 3);  glVertex2f(x0, y1);
    glEnd();
}

static void Test15_TextureWrap()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    DrawWrappedQuad(80, 120, 280, 360);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    DrawWrappedQuad(360, 120, 560, 360);

    glDisable(GL_TEXTURE_2D);
}

static void Test16_TextureFilter()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    DrawFullTexEnvQuad(80, 120, 280, 360, GL_REPLACE, 1, 1, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    DrawFullTexEnvQuad(360, 120, 560, 360, GL_REPLACE, 1, 1, 1);

    glDisable(GL_TEXTURE_2D);
}

static void Test17_MatrixStack()
{
    glClearColor(0.03f, 0.03f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glLoadIdentity();
    glTranslatef(320, 240, 0);

    for (int i = 0; i < 6; i++)
    {
        glPushMatrix();
        glRotatef(g_time * 40.0f + i * 60.0f, 0, 0, 1);
        glTranslatef(140, 0, 0);

        glBegin(GL_QUADS);
        glColor3f((i & 1) ? 1.0f : 0.2f, (i & 2) ? 1.0f : 0.2f, (i & 4) ? 1.0f : 0.2f);
        glVertex2f(-25, -25);
        glVertex2f(25, -25);
        glVertex2f(25, 25);
        glVertex2f(-25, 25);
        glEnd();

        glPopMatrix();
    }
}

static void Test18_TextureMatrix()
{
    glClearColor(0.02f, 0.02f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(g_time * 0.25f, 0.0f, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);
    glTexCoord2f(0, 0); glVertex2f(120, 100);
    glTexCoord2f(4, 0); glVertex2f(520, 100);
    glTexCoord2f(4, 4); glVertex2f(520, 380);
    glTexCoord2f(0, 4); glVertex2f(120, 380);
    glEnd();

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
}

static void Test19_TexturedVertexArrays()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    static const GLfloat verts2D[] =
    {
        160, 120, 0,
        480, 120, 0,
        480, 360, 0,
        160, 360, 0
    };

    static const GLfloat tex2D[] =
    {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verts2D);
    glTexCoordPointer(2, GL_FLOAT, 0, tex2D);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, vaIndices);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

static void Test20_Multitexture()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGL2D(640, 480);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    glColor3f(1, 1, 1);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 0);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0);
    glVertex2f(120, 100);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 4, 0);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0);
    glVertex2f(520, 100);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 4, 4);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1);
    glVertex2f(520, 380);

    glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0, 4);
    glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1);
    glVertex2f(120, 380);

    glEnd();

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);
}

static void Test21_PolygonOffset()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    SetGLPerspective(60, 640.0f / 480.0f, 0.1f, 20.0f);

    glTranslatef(0, 0, -4.0f);
    glRotatef(25.0f, 1, 0, 0);

    // Base polygon.
    glBegin(GL_QUADS);
        glColor3f(0.2f, 0.2f, 0.8f);
        glVertex3f(-2.0f, -1.0f, 0.0f);
        glVertex3f( 2.0f, -1.0f, 0.0f);
        glVertex3f( 2.0f,  1.0f, 0.0f);
        glVertex3f(-2.0f,  1.0f, 0.0f);
    glEnd();

    // Coplanar decal polygon.
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    glBegin(GL_QUADS);
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-1.5f, -0.6f, 0.0f);
        glVertex3f( 1.5f, -0.6f, 0.0f);
        glVertex3f( 1.5f,  0.6f, 0.0f);
        glVertex3f(-1.5f,  0.6f, 0.0f);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.0f, 0.0f);

    glDisable(GL_DEPTH_TEST);
}

static void Test22_ReadPixels()
{
    glClearColor(0.25f, 0.0f, 0.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    SetGL2D(640, 480);

    glBegin(GL_QUADS);
    glColor3f(0, 1, 0);
    glVertex2f(250, 190);
    glVertex2f(390, 190);
    glVertex2f(390, 290);
    glVertex2f(250, 290);
    glEnd();

    if (!g_readPixelsPrinted)
    {
        g_readPixelsPrinted = true;

        GLubyte px[4];
        px[0] = px[1] = px[2] = px[3] = 0;

        glReadPixels(320, 240, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);

        char buf[256];
        wsprintf(buf, "Test22 glReadPixels center RGBA = %u %u %u %u\n",
            (UINT)px[0], (UINT)px[1], (UINT)px[2], (UINT)px[3]);
        OutputDebugString(buf);
    }
}

static void Test23_GetState()
{
    glClearColor(0.0f, 0.10f, 0.10f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    if (!g_getPrinted)
    {
        g_getPrinted = true;

        GLint maxTex = 0;
        GLint depthBits = 0;
        GLint stencilBits = 0;
        GLint texUnits = 0;

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTex);
        glGetIntegerv(GL_DEPTH_BITS, &depthBits);
        glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texUnits);

        GLboolean depthBefore = glIsEnabled(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_TEST);
        GLboolean depthAfter = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);

        char buf[512];
        wsprintf(buf, "Test23: maxTex=%d depth=%d stencil=%d units=%d depthBefore=%d depthAfter=%d err=%d\n",
            maxTex, depthBits, stencilBits, texUnits,
            (int)depthBefore, (int)depthAfter, (int)glGetError());
        OutputDebugString(buf);

        wsprintf(buf, "Test23: glIsTexture checker=%d fake999=%d\n",
            (int)glIsTexture(g_checkerTex), (int)glIsTexture(999));
        OutputDebugString(buf);
    }

    SetGL2D(640, 480);

    glBegin(GL_TRIANGLES);
    glColor3f(0, 1, 1);
    glVertex2f(320, 360);
    glColor3f(1, 0, 1);
    glVertex2f(180, 120);
    glColor3f(1, 1, 0);
    glVertex2f(460, 120);
    glEnd();
}

static GLuint g_testDisplayList = 0;

static void BuildTest24DisplayList()
{
    if (g_testDisplayList != 0)
        return;

    g_testDisplayList = glGenLists(1);

    glNewList(g_testDisplayList, GL_COMPILE);

        glBegin(GL_QUADS);

            glColor3f(1.0f, 0.5f, 0.0f);
            glVertex2f(-140, -90);
            glVertex2f( 140, -90);
            glVertex2f( 140,  90);
            glVertex2f(-140,  90);

        glEnd();

    glEndList();
}

static void Test24_DisplayListPlaceholder()
{
    glClearColor(0.12f, 0.08f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    SetGL2D(640, 480);

    BuildTest24DisplayList();

    glLoadIdentity();
    glTranslatef(320, 240, 0);
    glRotatef(g_time * 45.0f, 0, 0, 1);

    glCallList(g_testDisplayList);
}