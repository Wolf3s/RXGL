/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_pbgl_samples.h  --  Ports of the pbGL sample suite (NeHe-based) adapted
 *                           to RXGL / XDK conventions. The original samples target
 *                           the nxdk/pbGL toolchain; this file translates them to
 *                           drop straight into the RXGL test harness so they can
 *                           be verified on both Xbox hardware and the PC reference
 *                           build. Included by Tests_Shared.h.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

static void PBGLResetState()
{
    // Lighting
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

    // Fog
    glDisable(GL_FOG);

    // Texture
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);

    // Blending / logic
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_LOGIC_OP);
    glBlendFunc(GL_ONE, GL_ZERO);
    glLogicOp(GL_COPY);

    // Depth / stencil
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xFF);

    // Culling / winding
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Polygon
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Scissor / alpha test
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_ALPHA_TEST);

    // Shading
    glShadeModel(GL_SMOOTH);

    // Colour write
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Vertex arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Multitexture -- back to unit 0
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);

    // Viewport and matrices to clean state
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();

    // Vertex colour back to white
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}



// Build a 64x64 RGBA procedural texture that looks similar in complexity to
// the 256x256 stone/wood photo texture used by the original samples.
// Stored statically so it is built once on first use.
static GLuint s_pbglTex = 0;   // the procedural stand-in

static void PBGLEnsureTexture()
{
    if (s_pbglTex) return;

    static GLubyte buf[64 * 64 * 4];
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            GLubyte *p = &buf[(y * 64 + x) * 4];
            // Brick-like pattern: warm reddish blocks separated by thin mortar lines
            int bx = x % 16;
            int by = y % 8;
            int row = y / 8;
            int shifted = (row & 1) ? (x + 8) % 16 : bx;
            int mortar = (shifted == 0 || by == 0);
            if (mortar)
            {
                p[0] = 180; p[1] = 170; p[2] = 160; p[3] = 255;
            }
            else
            {
                // Slight noise via XOR
                GLubyte v = (GLubyte)(((x * 37) ^ (y * 53) ^ 0xA5) & 0x1F);
                p[0] = (GLubyte)(180 + v);
                p[1] = (GLubyte)(80  + v / 2);
                p[2] = (GLubyte)(60  + v / 3);
                p[3] = 255;
            }
        }
    }

    glGenTextures(1, &s_pbglTex);
    glBindTexture(GL_TEXTURE_2D, s_pbglTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
}

// For sample4: three filter-mode variants of the same texture.
// Index 0 = GL_NEAREST, 1 = GL_LINEAR, 2 = GL_LINEAR (mip emulated as LINEAR).
static GLuint s_pbglTexFilters[3] = { 0, 0, 0 };

static void PBGLEnsureFilterTextures()
{
    if (s_pbglTexFilters[0]) return;

    static GLubyte buf[64 * 64 * 4];
    int y, x;
    for (y = 0; y < 64; y++)
        for (x = 0; x < 64; x++)
        {
            GLubyte *p = &buf[(y * 64 + x) * 4];
            int bx = x % 16;
            int by = y % 8;
            int row = y / 8;
            int shifted = (row & 1) ? (x + 8) % 16 : bx;
            int mortar = (shifted == 0 || by == 0);
            if (mortar) { p[0]=180; p[1]=170; p[2]=160; p[3]=255; }
            else {
                GLubyte v=(GLubyte)(((x*37)^(y*53)^0xA5)&0x1F);
                p[0]=(GLubyte)(180+v); p[1]=(GLubyte)(80+v/2);
                p[2]=(GLubyte)(60+v/3); p[3]=255;
            }
        }

    glGenTextures(3, s_pbglTexFilters);

    // texture[0] -- GL_NEAREST
    glBindTexture(GL_TEXTURE_2D, s_pbglTexFilters[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture[1] -- GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, s_pbglTexFilters[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture[2] -- GL_LINEAR_MIPMAP_NEAREST with generated mipmaps
    glBindTexture(GL_TEXTURE_2D, s_pbglTexFilters[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

// Helper: draw a standard unit cube with texcoords and optional per-face normals.
// Used by samples 3, 4, 5.
static void DrawTexCube()
{
    glBegin(GL_QUADS);
        // Front face  (+Z)
        glNormal3f( 0.0f,  0.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Back face   (-Z)
        glNormal3f( 0.0f,  0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Right face  (+X)
        glNormal3f( 1.0f,  0.0f,  0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Left face   (-X)
        glNormal3f(-1.0f,  0.0f,  0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        // Top face    (+Y)
        glNormal3f( 0.0f,  1.0f,  0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        // Bottom face (-Y)
        glNormal3f( 0.0f, -1.0f,  0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glEnd();
}

// ============================================================
// PBGL Sample 1 -- Triangle and Quad (NeHe lesson 2)
// Original: sample1/main.c
//
// Draws a coloured triangle on the left and a solid-colour quad on the right,
// against a black background.  Static scene -- no animation in the original,
// but we add a gentle pulse to the clear colour so the test is easy to spot
// in the carousel.
// ============================================================
static void TestPBGL1_TriangleAndQuad()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    // Left: coloured triangle
    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -6.0f);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 0.0f,  1.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
    glEnd();

    // Right: solid blue quad
    glTranslatef(3.0f, 0.0f, 0.0f);
    glColor3f(0.5f, 0.5f, 1.0f);

    glBegin(GL_QUADS);
        glVertex3f(-1.0f,  1.0f, 0.0f);
        glVertex3f( 1.0f,  1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
}

// ============================================================
// PBGL Sample 2 -- Spinning Pyramid and Coloured Cube (NeHe lesson 3)
// Original: sample2/main.c
//
// The original used a frame counter to drive rotation at fixed increments
// (0.3 deg/frame tri, 0.15 deg/frame quad).  We use g_time instead so the
// speed is frame-rate independent.  At 60 fps the originals moved:
//   tri:  0.3 * 60 = 18 deg/s
//   quad: 0.15 * 60 = 9 deg/s  (counter-clockwise)
// ============================================================
static void TestPBGL2_SpinningPyramidCube()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    float rot_tri  =  g_time * 18.0f;
    float rot_quad = -g_time *  9.0f;

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    // Left: multi-coloured pyramid (four triangle faces)
    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -6.0f);
    glRotatef(rot_tri, 0.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
        // Front
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  1.0f,  0.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Right
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  1.0f,  0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Back
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  1.0f,  0.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        // Left
        glColor3f(1.0f, 0.0f, 0.0f); glVertex3f( 0.0f,  1.0f,  0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();

    // Right: coloured cube
    glLoadIdentity();
    glTranslatef(1.5f, 0.0f, -7.0f);
    glRotatef(rot_quad, 1.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
        // Top
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        // Bottom
        glColor3f(1.0f, 0.5f, 0.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Front
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        // Back
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f,  1.0f, -1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        // Right
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        // Left
        glColor3f(0.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glEnd();
}

// ============================================================
// PBGL Sample 3 -- Textured Spinning Cube (NeHe lesson 5)
// Original: sample3/main.c
//
// Single textured cube rotating on all three axes simultaneously.
// Rotation speeds match the original's per-frame increments at 60 fps:
//   rot_x: 0.3 * 60 = 18 deg/s
//   rot_y: 0.2 * 60 = 12 deg/s
//   rot_z: 0.4 * 60 = 24 deg/s
// ============================================================
static void TestPBGL3_TexturedCube()
{
    PBGLEnsureTexture();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glColor3f(1.0f, 1.0f, 1.0f);

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(g_time * 18.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(g_time * 12.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(g_time * 24.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, s_pbglTex);
    DrawTexCube();

    glDisable(GL_TEXTURE_2D);
}

// ============================================================
// PBGL Sample 4 -- Lit Textured Cube with Filter Modes (NeHe lesson 7)
// Original: sample4/main.c
//
// One cube, three texture filter variants (nearest / linear / mipmap),
// optional lighting, optional additive blend.  The original toggled these
// via gamepad.  Here we cycle automatically:
//   filter mode cycles every 3 seconds.
//   lighting toggles every 5 seconds.
//   blend is off (it disables depth test and is hard to see in a carousel).
// ============================================================
static void TestPBGL4_LitTexturedCube()
{
    PBGLEnsureFilterTextures();

    // Per-test state driven by Main.cpp PollInput() via shared globals.
    // A (pressed) = toggle lighting,  B (pressed) = cycle filter
    int s_filter = g_pbgl4_filter;
    int s_light  = g_pbgl4_light;

    // Light values chosen so that worst-case lit face (ndotl=1.0) does not
    // saturate to white:
    //   scene_ambient(0.2) * mat(0.6) = 0.12
    //   light_ambient(0.2) * mat(0.6) = 0.12
    //   light_diffuse(0.5) * mat(0.6) * ndotl(1.0) = 0.30
    //   total_worst = 0.54  -- leaves texture clearly visible via GL_MODULATE
    static const GLfloat li_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    static const GLfloat li_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
    static const GLfloat li_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };
    static const GLfloat mat_amb[]     = { 0.6f, 0.6f, 0.6f, 1.0f };
    static const GLfloat mat_dif[]     = { 0.6f, 0.6f, 0.6f, 1.0f };
    static const GLfloat mat_spc[]     = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat mat_shn[]     = { 0.0f };

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glDisable(GL_LIGHT0);  // kill PC default white light bleed

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_spc);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shn);

    // Set light position while modelview is identity = camera/eye space,
    // so the light stays fixed in front of the viewer as the cube rotates.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT1, GL_AMBIENT,  li_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  li_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, li_position);

    if (s_light)
    {
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHTING);
    }

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(g_time * 20.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(g_time * 15.0f, 0.0f, 1.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, s_pbglTexFilters[s_filter]);
    DrawTexCube();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHT1);
}

// ============================================================
// PBGL Sample 5 -- Triple Draw Methods (arrays / elements / immediate)
// Original: sample5/main.c
//
// Three cubes side by side, each drawn a different way:
//   Left:   glDrawArrays
//   Centre: glDrawElements
//   Right:  immediate mode glBegin/End
// The original used pbgl_alloc for GPU-side vertex memory; we keep the data
// in a plain static array (the XDK copies it to GPU memory during the draw).
//
// Four coloured directional lights, XOR-pattern texture generated
// procedurally (same as the original's get_texture(128,128)).
// ============================================================

static const GLfloat s_cubeVbuf[] =
{
    // layout per vertex: pos(3) texcoord(2) normal(3)
    // Face +Z
    -0.5f,-0.5f,+0.5f,  0.0f,0.0f,  0.0f,0.0f,+1.0f,
    +0.5f,-0.5f,+0.5f,  1.0f,0.0f,  0.0f,0.0f,+1.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f,  0.0f,0.0f,+1.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f,  0.0f,0.0f,+1.0f,
    -0.5f,+0.5f,+0.5f,  0.0f,1.0f,  0.0f,0.0f,+1.0f,
    -0.5f,-0.5f,+0.5f,  0.0f,0.0f,  0.0f,0.0f,+1.0f,
    // Face -Z
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,  0.0f,0.0f,-1.0f,
    -0.5f,+0.5f,-0.5f,  1.0f,0.0f,  0.0f,0.0f,-1.0f,
    +0.5f,+0.5f,-0.5f,  1.0f,1.0f,  0.0f,0.0f,-1.0f,
    +0.5f,+0.5f,-0.5f,  1.0f,1.0f,  0.0f,0.0f,-1.0f,
    +0.5f,-0.5f,-0.5f,  0.0f,1.0f,  0.0f,0.0f,-1.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,  0.0f,0.0f,-1.0f,
    // Face +X
    +0.5f,-0.5f,-0.5f,  0.0f,0.0f, +1.0f,0.0f,0.0f,
    +0.5f,+0.5f,-0.5f,  1.0f,0.0f, +1.0f,0.0f,0.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f, +1.0f,0.0f,0.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f, +1.0f,0.0f,0.0f,
    +0.5f,-0.5f,+0.5f,  0.0f,1.0f, +1.0f,0.0f,0.0f,
    +0.5f,-0.5f,-0.5f,  0.0f,0.0f, +1.0f,0.0f,0.0f,
    // Face -X
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f, -1.0f,0.0f,0.0f,
    -0.5f,-0.5f,+0.5f,  1.0f,0.0f, -1.0f,0.0f,0.0f,
    -0.5f,+0.5f,+0.5f,  1.0f,1.0f, -1.0f,0.0f,0.0f,
    -0.5f,+0.5f,+0.5f,  1.0f,1.0f, -1.0f,0.0f,0.0f,
    -0.5f,+0.5f,-0.5f,  0.0f,1.0f, -1.0f,0.0f,0.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f, -1.0f,0.0f,0.0f,
    // Face +Y
    -0.5f,+0.5f,-0.5f,  0.0f,0.0f,  0.0f,+1.0f,0.0f,
    -0.5f,+0.5f,+0.5f,  1.0f,0.0f,  0.0f,+1.0f,0.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f,  0.0f,+1.0f,0.0f,
    +0.5f,+0.5f,+0.5f,  1.0f,1.0f,  0.0f,+1.0f,0.0f,
    +0.5f,+0.5f,-0.5f,  0.0f,1.0f,  0.0f,+1.0f,0.0f,
    -0.5f,+0.5f,-0.5f,  0.0f,0.0f,  0.0f,+1.0f,0.0f,
    // Face -Y
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,  0.0f,-1.0f,0.0f,
    +0.5f,-0.5f,-0.5f,  1.0f,0.0f,  0.0f,-1.0f,0.0f,
    +0.5f,-0.5f,+0.5f,  1.0f,1.0f,  0.0f,-1.0f,0.0f,
    +0.5f,-0.5f,+0.5f,  1.0f,1.0f,  0.0f,-1.0f,0.0f,
    -0.5f,-0.5f,+0.5f,  0.0f,1.0f,  0.0f,-1.0f,0.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,  0.0f,-1.0f,0.0f,
};

static const GLushort s_cubeIndices[] =
{
     0, 1, 2, 3, 4, 5,
     6, 7, 8, 9,10,11,
    12,13,14,15,16,17,
    18,19,20,21,22,23,
    24,25,26,27,28,29,
    30,31,32,33,34,35
};

static GLuint s_pbglXorTex = 0;

static void PBGLEnsureXorTexture()
{
    if (s_pbglXorTex) return;

    static GLubyte buf[128 * 128 * 4];
    GLubyte *ptr = buf;
    int y, x;
    for (y = 0; y < 128; y++)
        for (x = 0; x < 128; x++)
        {
            GLubyte v = (GLubyte)(x ^ y);
            *ptr++ = v;
            *ptr++ = v;
            *ptr++ = v;
            *ptr++ = 255;
        }

    glGenTextures(1, &s_pbglXorTex);
    glBindTexture(GL_TEXTURE_2D, s_pbglXorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    glGenerateMipmap(GL_TEXTURE_2D);
}

// Draw the cube using raw immediate-mode, extracting from the interleaved buffer.
static void DrawCubeImmediate()
{
    static const int stride = (3 + 2 + 3); // floats per vertex
    glBegin(GL_TRIANGLES);
    int i;
    for (i = 0; i < 36; i++)
    {
        const GLfloat *v = &s_cubeVbuf[i * stride];
        glTexCoord2f(v[3], v[4]);
        glNormal3f  (v[5], v[6], v[7]);
        glVertex3f  (v[0], v[1], v[2]);
    }
    glEnd();
}

static void TestPBGL5_TripleDrawMethods()
{
    PBGLEnsureXorTexture();

    // A (pressed) = toggle lighting on/off, driven by Main.cpp PollInput()
    int s_light5 = g_pbgl5_light;

    // Four directional coloured lights (w=0 = directional, no attenuation).
    // Directional lights illuminate based solely on normal direction -- with
    // all four active and saturated diffuse colours, each face will be lit by
    // whichever lights face it.  Material is kept below 1.0 so no channel
    // can sum past 1.0 (worst case: two perpendicular diffuse lights hit the
    // same face at ~0.7 each * mat 0.8 = 1.12 -- we use mat 0.6 to be safe).
    static const GLfloat liAmb[][4] = {
        { 0.05f, 0.05f, 0.05f, 1.0f },
        { 0.05f, 0.05f, 0.05f, 1.0f },
        { 0.05f, 0.05f, 0.05f, 1.0f },
        { 0.05f, 0.05f, 0.05f, 1.0f },
    };
    static const GLfloat liDif[][4] = {
        { 1.0f, 0.0f, 0.0f, 1.0f },  // red from left
        { 0.0f, 1.0f, 0.0f, 1.0f },  // green from right
        { 0.0f, 0.0f, 1.0f, 1.0f },  // blue from front
        { 0.0f, 1.0f, 1.0f, 1.0f },  // cyan from back
    };
    static const GLfloat liPos[][4] = {
        { -1.0f,  0.0f,  0.0f, 0.0f },
        { +1.0f,  0.0f,  0.0f, 0.0f },
        {  0.0f,  0.0f, +1.0f, 0.0f },
        {  0.0f,  0.0f, -1.0f, 0.0f },
    };
    static const GLfloat liAmbScene[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    // mat 0.6 keeps worst-case sum below 1.0
    static const GLfloat mat_amb[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    static const GLfloat mat_dif[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    static const GLfloat mat_spc[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static const GLfloat mat_shn[] = { 0.0f };

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glColor3f(1.0f, 1.0f, 1.0f);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_spc);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shn);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, liAmbScene);
    int i;
    // Set light positions while modelview is identity (eye/camera space).
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    for (i = 0; i < 4; i++)
    {
        glLightfv((GLenum)(GL_LIGHT0 + i), GL_AMBIENT,  liAmb[i]);
        glLightfv((GLenum)(GL_LIGHT0 + i), GL_DIFFUSE,  liDif[i]);
        glLightfv((GLenum)(GL_LIGHT0 + i), GL_POSITION, liPos[i]);
    }

    if (s_light5)
    {
        glEnable(GL_LIGHTING);
        for (i = 0; i < 4; i++) glEnable((GLenum)(GL_LIGHT0 + i));
    }
    else
    {
        glDisable(GL_LIGHTING);
        for (i = 0; i < 4; i++) glDisable((GLenum)(GL_LIGHT0 + i));
    }

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    float camDist = -5.0f;
    float camRX   = g_time * 12.0f;
    float camRY   = g_time * 20.0f;

    static const int STRIDE_BYTES = (3 + 2 + 3) * (int)sizeof(GLfloat);

    glBindTexture(GL_TEXTURE_2D, s_pbglXorTex);

    // ------- Left: glDrawArrays -------
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer  (3, GL_FLOAT, STRIDE_BYTES, s_cubeVbuf);
    glTexCoordPointer(2, GL_FLOAT, STRIDE_BYTES, s_cubeVbuf + 3);
    glNormalPointer  (   GL_FLOAT, STRIDE_BYTES, s_cubeVbuf + 5);

    glLoadIdentity();
    glTranslatef(-2.0f, 0.0f, camDist);
    glRotatef(camRX, 1.0f, 0.0f, 0.0f);
    glRotatef(camRY, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ------- Centre: glDrawElements -------
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, camDist);
    glRotatef(camRX, 1.0f, 0.0f, 0.0f);
    glRotatef(camRY, 0.0f, 1.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, s_cubeIndices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // ------- Right: immediate mode -------
    glLoadIdentity();
    glTranslatef(2.0f, 0.0f, camDist);
    glRotatef(camRX, 1.0f, 0.0f, 0.0f);
    glRotatef(camRY, 0.0f, 1.0f, 0.0f);
    DrawCubeImmediate();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    for (i = 0; i < 4; i++) glDisable((GLenum)(GL_LIGHT0 + i));
}

// ============================================================
// PBGL Sample 6 -- Stencil Shadow Volumes (NeHe lesson 27)
// Original: sample6/main.c + object.c
//
// The original loaded a cross-shaped object from a text file and rendered
// it inside a room with a stencil-buffer shadow volume.
//
// Adaptations:
//   * Object data baked in (no file I/O).
//   * SDL gamepad removed; rotation driven by g_time.
//   * gluPerspective replaced with SetGLPerspective.
//   * C-style structs / functions translated to C++ static functions.
//   * object.c logic inlined (globj_read, globj_set_connectivity,
//     globj_calc_plane, globj_draw, globj_cast_shadow).
// ============================================================

#define PBGL6_MAX_POINTS 256
#define PBGL6_MAX_PLANES 128

typedef struct { float x, y, z; }          PBGLPoint;
typedef struct { float a, b, c, d; }       PBGLPlaneEq;
typedef struct {
    unsigned int p[3];
    PBGLPoint    normals[3];
    unsigned int neigh[3];
    PBGLPlaneEq  planeEq;
    GLboolean    visible;
} PBGLPlane;
typedef struct {
    unsigned int nPlanes, nPoints;
    PBGLPoint    points[PBGL6_MAX_POINTS];
    PBGLPlane    planes[PBGL6_MAX_PLANES];
} PBGLObject;

static PBGLObject s_glObj;
static GLboolean  s_glObjReady = GL_FALSE;

// Baked-in data from object.txt.
// The object is a 3-arm cross (+) built from three overlapping rectangular
// boxes.  24 points (1-based), 36 triangular planes.
static void PBGLInitObject(PBGLObject *o)
{
    // -- Points (1-based, matching the original file indices) --
    static const float pts[][3] =
    {
        /* 1*/ {-2.0f, 0.2f,-0.2f}, /* 2*/ { 2.0f, 0.2f,-0.2f},
        /* 3*/ { 2.0f, 0.2f, 0.2f}, /* 4*/ {-2.0f, 0.2f, 0.2f},
        /* 5*/ {-2.0f,-0.2f,-0.2f}, /* 6*/ { 2.0f,-0.2f,-0.2f},
        /* 7*/ { 2.0f,-0.2f, 0.2f}, /* 8*/ {-2.0f,-0.2f, 0.2f},

        /* 9*/ {-0.2f, 2.0f,-0.2f}, /*10*/ { 0.2f, 2.0f,-0.2f},
        /*11*/ { 0.2f, 2.0f, 0.2f}, /*12*/ {-0.2f, 2.0f, 0.2f},
        /*13*/ {-0.2f,-2.0f,-0.2f}, /*14*/ { 0.2f,-2.0f,-0.2f},
        /*15*/ { 0.2f,-2.0f, 0.2f}, /*16*/ {-0.2f,-2.0f, 0.2f},

        /*17*/ {-0.2f, 0.2f,-2.0f}, /*18*/ { 0.2f, 0.2f,-2.0f},
        /*19*/ { 0.2f, 0.2f, 2.0f}, /*20*/ {-0.2f, 0.2f, 2.0f},
        /*21*/ {-0.2f,-0.2f,-2.0f}, /*22*/ { 0.2f,-0.2f,-2.0f},
        /*23*/ { 0.2f,-0.2f, 2.0f}, /*24*/ {-0.2f,-0.2f, 2.0f},
    };
    // Planes: p0,p1,p2 (1-based), then 3x normal xyz
    // (exactly as parsed from object.txt)
    static const float planeData[][12] =
    {
        { 1, 3, 2,  0,1,0, 0,1,0, 0,1,0},
        { 1, 4, 3,  0,1,0, 0,1,0, 0,1,0},
        { 5, 6, 7,  0,-1,0,0,-1,0,0,-1,0},
        { 5, 7, 8,  0,-1,0,0,-1,0,0,-1,0},
        { 5, 4, 1, -1,0,0,-1,0,0,-1,0,0},
        { 5, 8, 4, -1,0,0,-1,0,0,-1,0,0},
        { 3, 6, 2,  1,0,0, 1,0,0, 1,0,0},
        { 3, 7, 6,  1,0,0, 1,0,0, 1,0,0},
        { 5, 1, 2,  0,0,-1,0,0,-1,0,0,-1},
        { 5, 2, 6,  0,0,-1,0,0,-1,0,0,-1},
        { 3, 4, 8,  0,0,1, 0,0,1, 0,0,1},
        { 3, 8, 7,  0,0,1, 0,0,1, 0,0,1},

        { 9,11,10,  0,1,0, 0,1,0, 0,1,0},
        { 9,12,11,  0,1,0, 0,1,0, 0,1,0},
        {13,14,15,  0,-1,0,0,-1,0,0,-1,0},
        {13,15,16,  0,-1,0,0,-1,0,0,-1,0},
        {13,12, 9, -1,0,0,-1,0,0,-1,0,0},
        {13,16,12, -1,0,0,-1,0,0,-1,0,0},
        {11,14,10,  1,0,0, 1,0,0, 1,0,0},
        {11,15,14,  1,0,0, 1,0,0, 1,0,0},
        {13, 9,10,  0,0,-1,0,0,-1,0,0,-1},
        {13,10,14,  0,0,-1,0,0,-1,0,0,-1},
        {11,12,16,  0,0,1, 0,0,1, 0,0,1},
        {11,16,15,  0,0,1, 0,0,1, 0,0,1},

        {17,19,18,  0,1,0, 0,1,0, 0,1,0},
        {17,20,19,  0,1,0, 0,1,0, 0,1,0},
        {21,22,23,  0,-1,0,0,-1,0,0,-1,0},
        {21,23,24,  0,-1,0,0,-1,0,0,-1,0},
        {21,20,17, -1,0,0,-1,0,0,-1,0,0},
        {21,24,20, -1,0,0,-1,0,0,-1,0,0},
        {19,22,18,  1,0,0, 1,0,0, 1,0,0},
        {19,23,22,  1,0,0, 1,0,0, 1,0,0},
        {21,17,18,  0,0,-1,0,0,-1,0,0,-1},
        {21,18,22,  0,0,-1,0,0,-1,0,0,-1},
        {19,20,24,  0,0,1, 0,0,1, 0,0,1},
        {19,24,23,  0,0,1, 0,0,1, 0,0,1},
    };

    unsigned int i;
    o->nPoints = 24;
    for (i = 0; i < 24; i++)
    {
        o->points[i + 1].x = pts[i][0];
        o->points[i + 1].y = pts[i][1];
        o->points[i + 1].z = pts[i][2];
    }
    o->nPlanes = 36;
    for (i = 0; i < 36; i++)
    {
        const float *d = planeData[i];
        o->planes[i].p[0]        = (unsigned int)d[0];
        o->planes[i].p[1]        = (unsigned int)d[1];
        o->planes[i].p[2]        = (unsigned int)d[2];
        o->planes[i].normals[0].x = d[3]; o->planes[i].normals[0].y = d[4];  o->planes[i].normals[0].z = d[5];
        o->planes[i].normals[1].x = d[6]; o->planes[i].normals[1].y = d[7];  o->planes[i].normals[1].z = d[8];
        o->planes[i].normals[2].x = d[9]; o->planes[i].normals[2].y = d[10]; o->planes[i].normals[2].z = d[11];
        o->planes[i].neigh[0] = 0;
        o->planes[i].neigh[1] = 0;
        o->planes[i].neigh[2] = 0;
        o->planes[i].visible  = GL_FALSE;
    }
}

static void PBGLSetConnectivity(PBGLObject *o)
{
    unsigned int i, j, ki, kj;
    for (i = 0; i < o->nPlanes - 1; i++)
    {
        for (j = i + 1; j < o->nPlanes; j++)
        {
            for (ki = 0; ki < 3; ki++)
            {
                if (!o->planes[i].neigh[ki])
                {
                    for (kj = 0; kj < 3; kj++)
                    {
                        unsigned int p1i = o->planes[i].p[ki];
                        unsigned int p2i = o->planes[i].p[(ki + 1) % 3];
                        unsigned int p1j = o->planes[j].p[kj];
                        unsigned int p2j = o->planes[j].p[(kj + 1) % 3];

                        unsigned int P1i = ((p1i + p2i) - (p1i > p2i ? p1i-p2i : p2i-p1i)) / 2;
                        unsigned int P2i = ((p1i + p2i) + (p1i > p2i ? p1i-p2i : p2i-p1i)) / 2;
                        unsigned int P1j = ((p1j + p2j) - (p1j > p2j ? p1j-p2j : p2j-p1j)) / 2;
                        unsigned int P2j = ((p1j + p2j) + (p1j > p2j ? p1j-p2j : p2j-p1j)) / 2;

                        if (P1i == P1j && P2i == P2j)
                        {
                            o->planes[i].neigh[ki] = j + 1;
                            o->planes[j].neigh[kj] = i + 1;
                        }
                    }
                }
            }
        }
    }
}

static void PBGLCalcPlane(PBGLObject *o, PBGLPlane *pl)
{
    PBGLPoint v[4];
    int k;
    for (k = 0; k < 3; k++)
    {
        v[k+1].x = o->points[pl->p[k]].x;
        v[k+1].y = o->points[pl->p[k]].y;
        v[k+1].z = o->points[pl->p[k]].z;
    }
    pl->planeEq.a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
    pl->planeEq.b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
    pl->planeEq.c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
    pl->planeEq.d = -(v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
                      v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
                      v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z));
}

static void PBGLDrawObj(const PBGLObject *o)
{
    unsigned int i, j;
    glBegin(GL_TRIANGLES);
    for (i = 0; i < o->nPlanes; i++)
        for (j = 0; j < 3; j++)
        {
            glNormal3f(o->planes[i].normals[j].x,
                       o->planes[i].normals[j].y,
                       o->planes[i].normals[j].z);
            glVertex3f(o->points[o->planes[i].p[j]].x,
                       o->points[o->planes[i].p[j]].y,
                       o->points[o->planes[i].p[j]].z);
        }
    glEnd();
}

static void PBGLCastShadow(PBGLObject *o, const float lp[4])
{
    unsigned int i, j, k, jj, p1, p2;
    PBGLPoint v1, v2;

    for (i = 0; i < o->nPlanes; i++)
    {
        float side = o->planes[i].planeEq.a * lp[0]
                   + o->planes[i].planeEq.b * lp[1]
                   + o->planes[i].planeEq.c * lp[2]
                   + o->planes[i].planeEq.d * lp[3];
        o->planes[i].visible = (side > 0.0f) ? GL_TRUE : GL_FALSE;
    }

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_STENCIL_TEST);
    glColorMask(0, 0, 0, 0);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    // First pass: increment
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    for (i = 0; i < o->nPlanes; i++)
    {
        if (!o->planes[i].visible) continue;
        for (j = 0; j < 3; j++)
        {
            k = o->planes[i].neigh[j];
            if (!k || !o->planes[k-1].visible)
            {
                p1 = o->planes[i].p[j];
                jj = (j + 1) % 3;
                p2 = o->planes[i].p[jj];
                v1.x = (o->points[p1].x - lp[0]) * 100.0f;
                v1.y = (o->points[p1].y - lp[1]) * 100.0f;
                v1.z = (o->points[p1].z - lp[2]) * 100.0f;
                v2.x = (o->points[p2].x - lp[0]) * 100.0f;
                v2.y = (o->points[p2].y - lp[1]) * 100.0f;
                v2.z = (o->points[p2].z - lp[2]) * 100.0f;
                glBegin(GL_TRIANGLE_STRIP);
                    glVertex3f(o->points[p1].x, o->points[p1].y, o->points[p1].z);
                    glVertex3f(o->points[p1].x + v1.x, o->points[p1].y + v1.y, o->points[p1].z + v1.z);
                    glVertex3f(o->points[p2].x, o->points[p2].y, o->points[p2].z);
                    glVertex3f(o->points[p2].x + v2.x, o->points[p2].y + v2.y, o->points[p2].z + v2.z);
                glEnd();
            }
        }
    }

    // Second pass: decrement
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
    for (i = 0; i < o->nPlanes; i++)
    {
        if (!o->planes[i].visible) continue;
        for (j = 0; j < 3; j++)
        {
            k = o->planes[i].neigh[j];
            if (!k || !o->planes[k-1].visible)
            {
                p1 = o->planes[i].p[j];
                jj = (j + 1) % 3;
                p2 = o->planes[i].p[jj];
                v1.x = (o->points[p1].x - lp[0]) * 100.0f;
                v1.y = (o->points[p1].y - lp[1]) * 100.0f;
                v1.z = (o->points[p1].z - lp[2]) * 100.0f;
                v2.x = (o->points[p2].x - lp[0]) * 100.0f;
                v2.y = (o->points[p2].y - lp[1]) * 100.0f;
                v2.z = (o->points[p2].z - lp[2]) * 100.0f;
                glBegin(GL_TRIANGLE_STRIP);
                    glVertex3f(o->points[p1].x, o->points[p1].y, o->points[p1].z);
                    glVertex3f(o->points[p1].x + v1.x, o->points[p1].y + v1.y, o->points[p1].z + v1.z);
                    glVertex3f(o->points[p2].x, o->points[p2].y, o->points[p2].z);
                    glVertex3f(o->points[p2].x + v2.x, o->points[p2].y + v2.y, o->points[p2].z + v2.z);
                glEnd();
            }
        }
    }

    glFrontFace(GL_CCW);
    glColorMask(1, 1, 1, 1);

    // Full-screen shadow quad
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(-0.1f,  0.1f, -0.10f);
        glVertex3f(-0.1f, -0.1f, -0.10f);
        glVertex3f( 0.1f,  0.1f, -0.10f);
        glVertex3f( 0.1f, -0.1f, -0.10f);
    glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glShadeModel(GL_SMOOTH);
}

static void PBGLDrawRoom()
{
    glBegin(GL_QUADS);
        // Floor
        glNormal3f( 0.0f,  1.0f,  0.0f);
        glVertex3f(-10.0f,-10.0f,-20.0f); glVertex3f(-10.0f,-10.0f, 20.0f);
        glVertex3f( 10.0f,-10.0f, 20.0f); glVertex3f( 10.0f,-10.0f,-20.0f);
        // Ceiling
        glNormal3f( 0.0f, -1.0f,  0.0f);
        glVertex3f(-10.0f, 10.0f, 20.0f); glVertex3f(-10.0f, 10.0f,-20.0f);
        glVertex3f( 10.0f, 10.0f,-20.0f); glVertex3f( 10.0f, 10.0f, 20.0f);
        // Front wall
        glNormal3f( 0.0f,  0.0f,  1.0f);
        glVertex3f(-10.0f, 10.0f,-20.0f); glVertex3f(-10.0f,-10.0f,-20.0f);
        glVertex3f( 10.0f,-10.0f,-20.0f); glVertex3f( 10.0f, 10.0f,-20.0f);
        // Back wall
        glNormal3f( 0.0f,  0.0f, -1.0f);
        glVertex3f( 10.0f, 10.0f, 20.0f); glVertex3f( 10.0f,-10.0f, 20.0f);
        glVertex3f(-10.0f,-10.0f, 20.0f); glVertex3f(-10.0f, 10.0f, 20.0f);
        // Left wall
        glNormal3f( 1.0f,  0.0f,  0.0f);
        glVertex3f(-10.0f, 10.0f, 20.0f); glVertex3f(-10.0f,-10.0f, 20.0f);
        glVertex3f(-10.0f,-10.0f,-20.0f); glVertex3f(-10.0f, 10.0f,-20.0f);
        // Right wall
        glNormal3f(-1.0f,  0.0f,  0.0f);
        glVertex3f( 10.0f, 10.0f,-20.0f); glVertex3f( 10.0f,-10.0f,-20.0f);
        glVertex3f( 10.0f,-10.0f, 20.0f); glVertex3f( 10.0f, 10.0f, 20.0f);
    glEnd();
}

static void TestPBGL6_ShadowVolumes()
{
    // One-time setup
    if (!s_glObjReady)
    {
        PBGLInitObject(&s_glObj);
        PBGLSetConnectivity(&s_glObj);
        unsigned int i;
        for (i = 0; i < s_glObj.nPlanes; i++)
            PBGLCalcPlane(&s_glObj, &s_glObj.planes[i]);
        s_glObjReady = GL_TRUE;
    }

    static const float lightAmb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    static const float lightDif[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    static const float lightSpc[] = {-0.2f,-0.2f,-0.2f, 1.0f };
    static float lightPos[] = { 0.0f, 5.0f, -4.0f, 1.0f };

    // Object position driven by Main.cpp PollInput() via g_pbgl6_objPos[].
    // Y/A = Z axis, B/X = X axis, White/Black = Y axis (held = continuous).
    static float s_rot[2]    = { 0.0f, 0.0f };
    static float s_rotSpd[2] = { 0.1f, -0.1f };

    s_rot[0] += s_rotSpd[0];
    s_rot[1] += s_rotSpd[1];

    float lp[4];
    lp[0] = lightPos[0]; lp[1] = lightPos[1];
    lp[2] = lightPos[2]; lp[3] = lightPos[3];

    glClearColor(0.1f, 1.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    // Material matching original sample6 -- room and object use this.
    {
        static const GLfloat matAmb[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        static const GLfloat matDif[] = { 0.2f, 0.6f, 0.9f, 1.0f };
        static const GLfloat matSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        static const GLfloat matShn[] = { 0.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpc);
        glMaterialfv(GL_FRONT, GL_SHININESS, matShn);
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    SetGLPerspective(45.0f, 640.0f / 480.0f, 0.001f, 100.0f);

    glLightfv(GL_LIGHT1, GL_AMBIENT,  lightAmb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  lightDif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);
    glEnable(GL_LIGHT1);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    PBGLDrawRoom();

    // Draw object
    glTranslatef(g_pbgl6_objPos[0], g_pbgl6_objPos[1], g_pbgl6_objPos[2]);
    glRotatef(s_rot[0], 1.0f, 0.0f, 0.0f);
    glRotatef(s_rot[1], 0.0f, 1.0f, 0.0f);
    glColor3f(0.5f, 0.0f, 1.0f);
    PBGLDrawObj(&s_glObj);

    // Shadow
    PBGLCastShadow(&s_glObj, lp);

    // Light marker (small quad at light position)
    glLoadIdentity();
    glTranslatef(lightPos[0], lightPos[1], lightPos[2] - 10.0f);
    glColor4f(0.7f, 0.4f, 0.0f, 1.0f);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glBegin(GL_QUADS);
        glVertex3f(-0.25f,-0.25f, 0.0f); glVertex3f(-0.25f, 0.25f, 0.0f);
        glVertex3f( 0.25f, 0.25f, 0.0f); glVertex3f( 0.25f,-0.25f, 0.0f);
    glEnd();
    glDepthMask(GL_TRUE);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT1);
    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
}