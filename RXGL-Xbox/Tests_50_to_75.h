/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_50_to_75.h  --  RXGL compatibility test suite, tests 50-75.
 *                       Covers bump-mapped spheres, additional vertex submission
 *                       variants, the glRect family, glPushClientAttrib /
 *                       glPopClientAttrib, GL_MIRRORED_REPEAT, and software clip
 *                       plane tests. Slots 63-75 are reserved for future use.
 *                       Included by Tests_Shared.h.
 *                       Stable -- only edit to fix bugs, do not add new tests here.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

// ------------------------------------------------------------
// Test 50 - Bump-mapped spheres (layout like Test 40)
//
// GL_LIGHTING: smooth normals give sphere-wide gradient (N.L).
// Stage0 DOT3 x Stage1 = bump perturbs that lit colour (per-texel).
//
// Phong spec uses the *geometric* normal only -- it cannot follow the normal map in
// fixed-function GL, so a white highlight stays a perfect circle and reads as "shiny
// texture", not relief. DIFF keeps spec (smooth reference); BLU/GRN/EMIT use zero
// specular so you only see bump in diffuse, not a misleading gloss blob.
//
// Spheres stay fixed; only the directional light orbits (same vector for GL_LIGHT0 + DOT3).
//
// Uses g_bumpNormalTex + g_whiteTex from MakeTextures().
// ------------------------------------------------------------

static void Test50_BumpSpheres()
{
    glClearColor(0.04f, 0.04f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);

    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    GLfloat globAmb[4] = { 0.06f, 0.06f, 0.07f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);

    struct BumpSph {
        float x;
        float phase;
        const char* label;
        bool baseOnly;
        float cr, cg, cb;
    };
    BumpSph spheres[4] = {
        { -2.4f, 0.00f, "DIFF", true,  0.8f, 0.1f, 0.1f },
        { -0.8f, 0.21f, "BLU", false, 0.1f, 0.2f, 0.8f },
        {  0.8f, 0.42f, "GRN", false, 0.1f, 0.6f, 0.1f },
        {  2.4f, 0.63f, "EMIT", false, 0.55f, 0.0f, 0.55f },
    };

    GLfloat zeroEm[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat emitMat[4]   = { 0.35f, 0.0f, 0.35f, 1.0f };
    GLfloat whiteSpec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat blackSpec[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    SetGLPerspective(55, 640.0f / 480.0f, 0.1f, 30.0f);
    glTranslatef(0, 0, -7.0f);

    /* Orbiting directional light (XZ sweep, slight elevation). Same vector for GL_LIGHT0 + DOT3. */
    float lang = g_time * 42.0f * DEG2RAD;
    float lx = cosf_approx(lang);
    float lz = sinf_approx(lang);
    float ly = 0.40f;
    float il = sqrtf_approx(lx * lx + ly * ly + lz * lz);
    lx /= il;
    ly /= il;
    lz /= il;
    GLfloat lightEnc[4] = { lx * 0.5f + 0.5f, ly * 0.5f + 0.5f, lz * 0.5f + 0.5f, 1.0f };

    GLfloat Lpos[4] = { lx, ly, lz, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, Lpos);
    GLfloat La[4] = { 0.07f, 0.07f, 0.08f, 1.0f };
    GLfloat Ld[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat Ls[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, La);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Ls);

    for (int i = 0; i < 4; i++)
    {
        float sc = wrap_tex_scroll(spheres[i].phase);

        GLfloat amb[4] = { spheres[i].cr * 0.14f, spheres[i].cg * 0.14f, spheres[i].cb * 0.14f, 1.0f };
        GLfloat diff[4] = { spheres[i].cr, spheres[i].cg, spheres[i].cb, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
        if (spheres[i].baseOnly)
        {
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpec);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
        }
        else
        {
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blackSpec);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
        }
        if (i == 3)
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emitMat);
        else
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroEm);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);

        glPushMatrix();
        glTranslatef(spheres[i].x, 0, 0);

        if (spheres[i].baseOnly)
        {
            glActiveTextureARB(GL_TEXTURE0_ARB);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, g_whiteTex);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            glActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);

            glActiveTextureARB(GL_TEXTURE0_ARB);
            DrawSphere40(0.9f, 20, 32, sc, 0.0f);
            glDisable(GL_TEXTURE_2D);
        }
        else
        {
            glActiveTextureARB(GL_TEXTURE0_ARB);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, g_bumpNormalTex);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_CONSTANT);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, lightEnc);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);

            glActiveTextureARB(GL_TEXTURE1_ARB);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, g_whiteTex);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);

            glActiveTextureARB(GL_TEXTURE0_ARB);
            DrawSphereBumpMTex(0.9f, 20, 32, sc, 0.0f);

            glActiveTextureARB(GL_TEXTURE1_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glDisable(GL_TEXTURE_2D);

            glActiveTextureARB(GL_TEXTURE0_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glDisable(GL_TEXTURE_2D);
        }

        glPopMatrix();
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    SetGLPerspective(55, 640.0f / 480.0f, 0.1f, 30.0f);
    glTranslatef(0, 0, -7.0f);

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(spheres[i].x, -1.4f, 0);
        glScalef(0.3f, 0.3f, 0.3f);
        glColor3f(0.8f, 0.8f, 0.8f);
        int len = 0;
        while (spheres[i].label[len]) len++;
        for (int c = 0; c < len; c++)
        {
            glPushMatrix();
            glTranslatef((c - (len - 1) * 0.5f) * 0.35f, 0, 0);
            DrawChar(spheres[i].label[c]);
            glPopMatrix();
        }
        glPopMatrix();
    }

    glDisable(GL_NORMALIZE);

// Reset all state that might affect subsequent tests

    GLfloat neutralAmb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat matWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, neutralAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matWhite);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matWhite);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matBlack);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matBlack);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

}

static void Test51_VertexVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // 4 cols x 4 rows. W=130, H=100, gap=10.
    // Total W=550, left=45. Total H=430, bottom=25.
    // Col X: 45,185,325,465   Row bottom Y: 25,135,245,355
    static const GLfloat col[4][3] = {
        {1,0.2f,0.2f}, {0.2f,1,0.2f}, {0.2f,0.2f,1}, {1,1,0.2f}
    };
    const float X[4] = { 45,185,325,465 };
    const float W = 130, H = 100;
    const float RY[4] = { 25,135,245,355 };

    // Row 0 (bottom): glVertex2f
    for (int c = 0;c < 4;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        glBegin(GL_QUADS);
        glVertex2f(X[c], RY[0]);   glVertex2f(X[c] + W, RY[0]);
        glVertex2f(X[c] + W, RY[0] + H); glVertex2f(X[c], RY[0] + H);
        glEnd();
    }
    // Row 1: glVertex2i
    for (int c = 0;c < 4;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        glBegin(GL_QUADS);
        glVertex2i((GLint)X[c], (GLint)RY[1]);
        glVertex2i((GLint)(X[c] + W), (GLint)RY[1]);
        glVertex2i((GLint)(X[c] + W), (GLint)(RY[1] + H));
        glVertex2i((GLint)X[c], (GLint)(RY[1] + H));
        glEnd();
    }
    // Row 2: glVertex2s
    for (int c = 0;c < 4;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        glBegin(GL_QUADS);
        glVertex2s((GLshort)X[c], (GLshort)RY[2]);
        glVertex2s((GLshort)(X[c] + W), (GLshort)RY[2]);
        glVertex2s((GLshort)(X[c] + W), (GLshort)(RY[2] + H));
        glVertex2s((GLshort)X[c], (GLshort)(RY[2] + H));
        glEnd();
    }
    // Row 3 (top): glVertex4f -- left 2 w=1, right 2 w=2 (coords doubled)
    for (int c = 0;c < 2;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        glBegin(GL_QUADS);
        glVertex4f(X[c], RY[3], 0, 1);
        glVertex4f(X[c] + W, RY[3], 0, 1);
        glVertex4f(X[c] + W, RY[3] + H, 0, 1);
        glVertex4f(X[c], RY[3] + H, 0, 1);
        glEnd();
    }
    for (int c = 2;c < 4;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        glBegin(GL_QUADS);
        glVertex4f(X[c] * 2, RY[3] * 2, 0, 2);
        glVertex4f((X[c] + W) * 2, RY[3] * 2, 0, 2);
        glVertex4f((X[c] + W) * 2, (RY[3] + H) * 2, 0, 2);
        glVertex4f(X[c] * 2, (RY[3] + H) * 2, 0, 2);
        glEnd();
    }
}

// ---------------------------------------------------------------------------
// Test52 -- Color type variants
// ---------------------------------------------------------------------------

static void Test52_ColorVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // 4 cols x 4 rows, CW=140,CH=100,gap=10
    // Total W=590, left=25. Total H=430, bottom=25.
    // Col X: 25,175,325,475   Row bottom Y: 25,135,245,355
    const float CW = 140, CH = 100;
    const float X[4] = { 25,175,325,475 };
    const float RY[4] = { 25,135,245,355 };

#define QUAD52(c,r) do { \
        glBegin(GL_QUADS); \
        glVertex2f(X[c],    RY[r]);     glVertex2f(X[c]+CW,RY[r]); \
        glVertex2f(X[c]+CW, RY[r]+CH); glVertex2f(X[c],   RY[r]+CH); \
        glEnd(); } while(0)

    // Row 0 (bottom): RED
    glColor3f(1, 0, 0);                    QUAD52(0, 0);
    glColor3ub(255, 0, 0);                 QUAD52(1, 0);
    glColor3us(65535, 0, 0);               QUAD52(2, 0);
    glColor3d(1.0, 0.0, 0.0);             QUAD52(3, 0);
    // Row 1: GREEN
    glColor3f(0, 1, 0);                    QUAD52(0, 1);
    glColor3ub(0, 255, 0);                 QUAD52(1, 1);
    glColor3us(0, 65535, 0);               QUAD52(2, 1);
    glColor3d(0.0, 1.0, 0.0);             QUAD52(3, 1);
    // Row 2: BLUE
    glColor3f(0, 0, 1);                    QUAD52(0, 2);
    glColor3ub(0, 0, 255);                 QUAD52(1, 2);
    glColor3us(0, 0, 65535);               QUAD52(2, 2);
    glColor3d(0.0, 0.0, 1.0);             QUAD52(3, 2);
    // Row 3 (top): WHITE
    glColor4f(1, 1, 1, 1);                  QUAD52(0, 3);
    { GLubyte v[4] = { 255,255,255,255 };   glColor4ubv(v); }  QUAD52(1, 3);
    glColor4us(65535, 65535, 65535, 65535); QUAD52(2, 3);
    { GLdouble v[4] = { 1,1,1,1 };          glColor4dv(v); }   QUAD52(3, 3);

#undef QUAD52
}

// ---------------------------------------------------------------------------
// Test53 -- Normal variant coverage (4 lit spheres, all must look identical)
// ---------------------------------------------------------------------------

static void DrawLitSphere_NV(int nMode, float cx, float cy, float cz, float r,
    int latSegs, int lonSegs)
{
    float latStep = PI_F / latSegs;
    float lonStep = 2.0f * PI_F / lonSegs;
    for (int lat = 0; lat < latSegs; lat++)
    {
        float phi0 = -PI_F / 2.0f + lat * latStep;
        float phi1 = phi0 + latStep;
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon = 0; lon <= lonSegs; lon++)
        {
            float theta = lon * lonStep;
            float cosT = cosf_approx(theta), sinT = sinf_approx(theta);
            for (int pass = 0; pass < 2; pass++)
            {
                float phi = (pass == 0) ? phi0 : phi1;
                float nx = cosf_approx(phi) * cosT;
                float ny = sinf_approx(phi);
                float nz = cosf_approx(phi) * sinT;
                float cnx = nx < -1.f ? -1.f : (nx > 1.f ? 1.f : nx);
                float cny = ny < -1.f ? -1.f : (ny > 1.f ? 1.f : ny);
                float cnz = nz < -1.f ? -1.f : (nz > 1.f ? 1.f : nz);
                switch (nMode) {
                case 0: glNormal3f(nx, ny, nz); break;
                case 1: glNormal3b((GLbyte)(cnx * 127.0f),
                    (GLbyte)(cny * 127.0f),
                    (GLbyte)(cnz * 127.0f)); break;
                case 2: glNormal3s((GLshort)(cnx * 32767.0f),
                    (GLshort)(cny * 32767.0f),
                    (GLshort)(cnz * 32767.0f)); break;
                case 3: glNormal3i((GLint)(cnx * 2147483520.0f),
                    (GLint)(cny * 2147483520.0f),
                    (GLint)(cnz * 2147483520.0f)); break;
                }
                glVertex3f(cx + r * nx, cy + r * ny, cz + r * nz);
            }
        }
        glEnd();
    }
}

static void Test53_NormalVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);

    SetGLPerspective(55.0f, 640.0f / 480.0f, 0.1f, 50.0f);
    glTranslatef(0, 0, -7.0f);

    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    GLfloat lpos[4] = { 3,4,5,1 }, ldiff[4] = { 1,1,0.9f,1 }, lamb[4] = { 0.08f,0.08f,0.12f,1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
    GLfloat matD[4] = { 0.8f,0.6f,0.3f,1 }, matA[4] = { 0.2f,0.15f,0.08f,1 }, matB[4] = { 0,0,0,1 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matD);
    glMaterialfv(GL_FRONT, GL_AMBIENT, matA);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matB);
    glMaterialf(GL_FRONT, GL_SHININESS, 0);

    const float xs[4] = { -2.7f,-0.9f,0.9f,2.7f };
    for (int i = 0;i < 4;i++) DrawLitSphere_NV(i, xs[i], 0, 0, 0.8f, 14, 20);

    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE); glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST);
}

// ---------------------------------------------------------------------------
// Test54 -- glRect family (6 variants, 4 cols each)
// ---------------------------------------------------------------------------

static void Test54_RectFamily()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // 4 cols x 6 rows, CW=140,CH=68,gap=8
    // Total W=584, left=28. Total H=448, bottom=16.
    const float CW = 140, CH = 68, RH = 76;
    const float X[4] = { 28,176,324,472 }, Y0 = 16;
    static const GLfloat col[4][3] = {
        {1,0.3f,0.3f},{0.3f,1,0.3f},{0.3f,0.3f,1},{1,1,0.3f}
    };
    for (int c = 0;c < 4;c++) {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        GLfloat x1 = X[c], x2 = X[c] + CW;
        glRectf(x1, Y0 + RH * 0, x2, Y0 + RH * 0 + CH);
        glRecti((GLint)x1, (GLint)(Y0 + RH * 1), (GLint)x2, (GLint)(Y0 + RH * 1 + CH));
        glRects((GLshort)x1, (GLshort)(Y0 + RH * 2), (GLshort)x2, (GLshort)(Y0 + RH * 2 + CH));
        glRectd((GLdouble)x1, (GLdouble)(Y0 + RH * 3), (GLdouble)x2, (GLdouble)(Y0 + RH * 3 + CH));
        { GLfloat v1[2] = { x1,Y0 + RH * 4 }, v2[2] = { x2,Y0 + RH * 4 + CH }; glRectfv(v1, v2); }
        {
            GLint v1[2] = { (GLint)x1,(GLint)(Y0 + RH * 5) };
            GLint v2[2] = { (GLint)x2,(GLint)(Y0 + RH * 5 + CH) }; glRectiv(v1, v2);
        }
    }
}

// ---------------------------------------------------------------------------
// Test55 -- Vertex variants 3D (rotating cube)
//
// Uses the EXACT same winding order as the proven Test4 cube (cubeFaces[]).
// ONE glScalef(1.5) applied before all faces. All faces use +-1 coords.
// No per-face matrix changes of any kind.
//
// Vert indices from Test4:
//   v0=(-1,-1,-1) v1=(1,-1,-1) v2=(1,1,-1) v3=(-1,1,-1)
//   v4=(-1,-1, 1) v5=(1,-1, 1) v6=(1,1, 1) v7=(-1,1, 1)
//
// Face windings from cubeFaces[][]:
//   +Z red:     {4,5,6,7} = (-1,-1,1),(1,-1,1),(1,1,1),(-1,1,1)
//   -Z green:   {1,0,3,2} = (1,-1,-1),(-1,-1,-1),(-1,1,-1),(1,1,-1)
//   -X blue:    {0,4,7,3} = (-1,-1,-1),(-1,-1,1),(-1,1,1),(-1,1,-1)
//   +X yellow:  {5,1,2,6} = (1,-1,1),(1,-1,-1),(1,1,-1),(1,1,1)
//   +Y cyan:    {7,6,2,3} = (-1,1,1),(1,1,1),(1,1,-1),(-1,1,-1)
//   -Y magenta: {0,1,5,4} = (-1,-1,-1),(1,-1,-1),(1,-1,1),(-1,-1,1)
// ---------------------------------------------------------------------------

static void Test55_VertexVariants3D()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glDisable(GL_LIGHTING);

    SetGLPerspective(60.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glTranslatef(0, 0, -4.0f);
    glRotatef(g_time * 30.0f, 1, 0, 0);
    glRotatef(g_time * 47.0f, 0, 1, 0);
    // Single scale applied once -- all faces use +-1 coords
    glScalef(1.5f, 1.5f, 1.5f);

    static const GLfloat col[6][3] = {
        {1,0,0},{0,1,0},{0,0,1},{1,1,0},{0,1,1},{1,0,1}
    };

    // Face 0 red (+Z): glVertex3f  -- winding {4,5,6,7}
    glColor3f(col[0][0], col[0][1], col[0][2]);
    glBegin(GL_QUADS);
    glVertex3f(-1, -1, 1); glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1); glVertex3f(-1, 1, 1);
    glEnd();

    // Face 1 green (-Z): glVertex3i  -- winding {1,0,3,2}
    glColor3f(col[1][0], col[1][1], col[1][2]);
    glBegin(GL_QUADS);
    glVertex3i(1, -1, -1); glVertex3i(-1, -1, -1);
    glVertex3i(-1, 1, -1); glVertex3i(1, 1, -1);
    glEnd();

    // Face 2 blue (-X): glVertex3s  -- winding {0,4,7,3}
    glColor3f(col[2][0], col[2][1], col[2][2]);
    glBegin(GL_QUADS);
    glVertex3s(-1, -1, -1); glVertex3s(-1, -1, 1);
    glVertex3s(-1, 1, 1); glVertex3s(-1, 1, -1);
    glEnd();

    // Face 3 yellow (+X): glVertex3fv  -- winding {5,1,2,6}
    glColor3f(col[3][0], col[3][1], col[3][2]);
    {
        GLfloat v0[3] = { 1,-1, 1 }, v1[3] = { 1,-1,-1 };
        GLfloat v2[3] = { 1, 1,-1 }, v3[3] = { 1, 1, 1 };
        glBegin(GL_QUADS);
        glVertex3fv(v0); glVertex3fv(v1); glVertex3fv(v2); glVertex3fv(v3);
        glEnd();
    }

    // Face 4 cyan (+Y): glVertex4f w=1  -- winding {7,6,2,3}
    glColor3f(col[4][0], col[4][1], col[4][2]);
    glBegin(GL_QUADS);
    glVertex4f(-1, 1, 1, 1); glVertex4f(1, 1, 1, 1);
    glVertex4f(1, 1, -1, 1); glVertex4f(-1, 1, -1, 1);
    glEnd();

    // Face 5 magenta (-Y): glVertex4fv w=1  -- winding {0,1,5,4}
    glColor3f(col[5][0], col[5][1], col[5][2]);
    {
        GLfloat v0[4] = { -1,-1,-1,1 }, v1[4] = { 1,-1,-1,1 };
        GLfloat v2[4] = { 1,-1, 1,1 }, v3[4] = { -1,-1, 1,1 };
        glBegin(GL_QUADS);
        glVertex4fv(v0); glVertex4fv(v1); glVertex4fv(v2); glVertex4fv(v3);
        glEnd();
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

// ---------------------------------------------------------------------------
// Test56 -- glInterleavedArrays
//
// Draws the same 4 triangles twice side by side:
//   LEFT  half: explicit gl*Pointer calls (reference)
//   RIGHT half: glInterleavedArrays (must look identical)
//
// Each row tests a different format. Y=0 is bottom.
// Row 0 (bottom): GL_V3F
// Row 1          : GL_C4UB_V3F
// Row 2          : GL_N3F_V3F  (lit, so colour comes from material)
// Row 3          : GL_T2F_C4UB_V3F
// Row 4          : GL_T2F_N3F_V3F  (lit + textured)
// Row 5 (top)   : GL_T2F_C4F_N3F_V3F
//
// A thin vertical white line divides the two halves so mismatches are obvious.
// ---------------------------------------------------------------------------

static void Test56_InterleavedArrays()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // Each row is a 290x72 viewport on left and right, gap 10, divider at x=320
    // Row bottom Y: 8, 88, 168, 248, 328, 408
    // Left  half viewport: x=[10..310]
    // Right half viewport: x=[330..630]

    const float RH = 80.0f; // row height including gap
    const float RY[6] = { 8, 88, 168, 248, 328, 408 };
    const float LX = 10, RX = 330, VW = 290, VH = 72;

    // A simple quad: two triangles forming a rectangle, for each format.
    // We use coordinates in [0,1] and scale via the scissor/viewport trick --
    // actually just bake screen coords directly into the vertex data since
    // SetGL2D sets up a pixel-space ortho.

    // -----------------------------------------------------------------------
    // Helper: draw a pair of coloured triangles in a given screen rect,
    // using explicit pointer calls (reference side).
    // We use GL_TRIANGLES with 6 verts (2 tris) = one quad.
    // -----------------------------------------------------------------------

    // -- Row 0: GL_V3F --
    // Vertices only, use current colour (set before draw)
    {
        // Left: explicit
        static const GLfloat vL[] = {
            // x,y,z -- left half quad
            LX,    RY[0],   0,   LX + VW, RY[0],   0,   LX + VW, RY[0] + VH, 0,
            LX,    RY[0],   0,   LX + VW, RY[0] + VH,0,   LX,    RY[0] + VH, 0
        };
        glColor3f(0.8f, 0.3f, 0.3f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, vL);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Right: glInterleavedArrays GL_V3F
        static const GLfloat vR[] = {
            RX,    RY[0],   0,   RX + VW, RY[0],   0,   RX + VW, RY[0] + VH, 0,
            RX,    RY[0],   0,   RX + VW, RY[0] + VH,0,   RX,    RY[0] + VH, 0
        };
        glColor3f(0.8f, 0.3f, 0.3f);
        glInterleavedArrays(GL_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // -- Row 1: GL_C4UB_V3F --
    // Layout: [r,g,b,a ubyte][x,y,z float]
    {
        // Pack struct manually: 4 ubytes then 3 floats
        // Left reference
        struct C4UB_V3F { GLubyte r, g, b, a; GLfloat x, y, z; };
        static const C4UB_V3F vL[6] = {
            {255,200,0,255, LX,    RY[1],   0},
            {255,200,0,255, LX + VW, RY[1],   0},
            {  0,200,255,255, LX + VW, RY[1] + VH,0},
            {255,200,0,255, LX,    RY[1],   0},
            {  0,200,255,255, LX + VW, RY[1] + VH,0},
            {  0,200,255,255, LX,    RY[1] + VH,0},
        };
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(C4UB_V3F), &vL[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(C4UB_V3F), &vL[0].r);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Right: interleaved
        static const C4UB_V3F vR[6] = {
            {255,200,0,255, RX,    RY[1],   0},
            {255,200,0,255, RX + VW, RY[1],   0},
            {  0,200,255,255, RX + VW, RY[1] + VH,0},
            {255,200,0,255, RX,    RY[1],   0},
            {  0,200,255,255, RX + VW, RY[1] + VH,0},
            {  0,200,255,255, RX,    RY[1] + VH,0},
        };
        glInterleavedArrays(GL_C4UB_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // -- Row 2: GL_T2F_C4UB_V3F --
    // Layout: [s,t float][r,g,b,a ubyte][x,y,z float]
    {
        struct T2F_C4UB_V3F { GLfloat s, t; GLubyte r, g, b, a; GLfloat x, y, z; };

        // Use checker texture (already created by MakeTextures)
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_LIGHTING);

        static const T2F_C4UB_V3F vL[6] = {
            {0,0, 255,255,255,255, LX,    RY[2],   0},
            {1,0, 255,255,255,255, LX + VW, RY[2],   0},
            {1,1, 255,255,255,255, LX + VW, RY[2] + VH,0},
            {0,0, 255,255,255,255, LX,    RY[2],   0},
            {1,1, 255,255,255,255, LX + VW, RY[2] + VH,0},
            {0,1, 255,255,255,255, LX,    RY[2] + VH,0},
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(T2F_C4UB_V3F), &vL[0].s);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(T2F_C4UB_V3F), &vL[0].r);
        glVertexPointer(3, GL_FLOAT, sizeof(T2F_C4UB_V3F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const T2F_C4UB_V3F vR[6] = {
            {0,0, 255,255,255,255, RX,    RY[2],   0},
            {1,0, 255,255,255,255, RX + VW, RY[2],   0},
            {1,1, 255,255,255,255, RX + VW, RY[2] + VH,0},
            {0,0, 255,255,255,255, RX,    RY[2],   0},
            {1,1, 255,255,255,255, RX + VW, RY[2] + VH,0},
            {0,1, 255,255,255,255, RX,    RY[2] + VH,0},
        };
        glInterleavedArrays(GL_T2F_C4UB_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_TEXTURE_2D);
    }

    // -- Row 3: GL_N3F_V3F (lit) --
    // Switch to 3D for this row only, draw in window-space via ortho trick
    {
        // We stay in SetGL2D but enable lighting. The normals point toward
        // the camera (+Z in eye space with no modelview transform) so the
        // dot product with a directional light at (0,0,1) = 1.0 = full diffuse.
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        GLfloat ldir[4] = { 0,0,1,0 };   // directional, toward camera
        GLfloat ldiff[4] = { 1,1,1,1 };
        GLfloat lamb[4] = { 0.1f,0.1f,0.1f,1 };
        glLightfv(GL_LIGHT0, GL_POSITION, ldir);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
        GLfloat matD[4] = { 0.3f,0.8f,0.4f,1 }, matA[4] = { 0.1f,0.2f,0.1f,1 }, matB[4] = { 0,0,0,1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matD);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matA);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matB);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
        glEnable(GL_NORMALIZE);

        struct N3F_V3F { GLfloat nx, ny, nz, x, y, z; };
        static const N3F_V3F vL[6] = {
            {0,0,1, LX,    RY[3],   0},
            {0,0,1, LX + VW, RY[3],   0},
            {0,0,1, LX + VW, RY[3] + VH,0},
            {0,0,1, LX,    RY[3],   0},
            {0,0,1, LX + VW, RY[3] + VH,0},
            {0,0,1, LX,    RY[3] + VH,0},
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(N3F_V3F), &vL[0].nx);
        glVertexPointer(3, GL_FLOAT, sizeof(N3F_V3F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const N3F_V3F vR[6] = {
            {0,0,1, RX,    RY[3],   0},
            {0,0,1, RX + VW, RY[3],   0},
            {0,0,1, RX + VW, RY[3] + VH,0},
            {0,0,1, RX,    RY[3],   0},
            {0,0,1, RX + VW, RY[3] + VH,0},
            {0,0,1, RX,    RY[3] + VH,0},
        };
        glInterleavedArrays(GL_N3F_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
    }

    // -- Row 4: GL_T2F_N3F_V3F (lit + textured) --
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        GLfloat ldir[4] = { 0,0,1,0 };
        GLfloat ldiff[4] = { 1,1,1,1 }, lamb[4] = { 0.15f,0.15f,0.15f,1 };
        glLightfv(GL_LIGHT0, GL_POSITION, ldir);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
        GLfloat matD[4] = { 1,1,1,1 }, matA[4] = { 1,1,1,1 }, matB[4] = { 0,0,0,1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matD);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matA);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matB);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
        glEnable(GL_NORMALIZE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        struct T2F_N3F_V3F { GLfloat s, t, nx, ny, nz, x, y, z; };
        static const T2F_N3F_V3F vL[6] = {
            {0,0, 0,0,1, LX,    RY[4],   0},
            {1,0, 0,0,1, LX + VW, RY[4],   0},
            {1,1, 0,0,1, LX + VW, RY[4] + VH,0},
            {0,0, 0,0,1, LX,    RY[4],   0},
            {1,1, 0,0,1, LX + VW, RY[4] + VH,0},
            {0,1, 0,0,1, LX,    RY[4] + VH,0},
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(T2F_N3F_V3F), &vL[0].s);
        glNormalPointer(GL_FLOAT, sizeof(T2F_N3F_V3F), &vL[0].nx);
        glVertexPointer(3, GL_FLOAT, sizeof(T2F_N3F_V3F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const T2F_N3F_V3F vR[6] = {
            {0,0, 0,0,1, RX,    RY[4],   0},
            {1,0, 0,0,1, RX + VW, RY[4],   0},
            {1,1, 0,0,1, RX + VW, RY[4] + VH,0},
            {0,0, 0,0,1, RX,    RY[4],   0},
            {1,1, 0,0,1, RX + VW, RY[4] + VH,0},
            {0,1, 0,0,1, RX,    RY[4] + VH,0},
        };
        glInterleavedArrays(GL_T2F_N3F_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
        glDisable(GL_TEXTURE_2D);
    }

    // -- Row 5 (top): GL_T2F_C4F_N3F_V3F --
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        GLfloat ldir[4] = { 0,0,1,0 };
        GLfloat ldiff[4] = { 1,1,1,1 }, lamb[4] = { 0.1f,0.1f,0.1f,1 };
        glLightfv(GL_LIGHT0, GL_POSITION, ldir);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, ldiff);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lamb);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_NORMALIZE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        struct T2F_C4F_N3F_V3F { GLfloat s, t, r, g, b, a, nx, ny, nz, x, y, z; };
        static const T2F_C4F_N3F_V3F vL[6] = {
            {0,0, 1,0.5f,0,1, 0,0,1, LX,    RY[5],   0},
            {1,0, 1,0.5f,0,1, 0,0,1, LX + VW, RY[5],   0},
            {1,1, 0,0.5f,1,1, 0,0,1, LX + VW, RY[5] + VH,0},
            {0,0, 1,0.5f,0,1, 0,0,1, LX,    RY[5],   0},
            {1,1, 0,0.5f,1,1, 0,0,1, LX + VW, RY[5] + VH,0},
            {0,1, 0,0.5f,1,1, 0,0,1, LX,    RY[5] + VH,0},
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(T2F_C4F_N3F_V3F), &vL[0].s);
        glColorPointer(4, GL_FLOAT, sizeof(T2F_C4F_N3F_V3F), &vL[0].r);
        glNormalPointer(GL_FLOAT, sizeof(T2F_C4F_N3F_V3F), &vL[0].nx);
        glVertexPointer(3, GL_FLOAT, sizeof(T2F_C4F_N3F_V3F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const T2F_C4F_N3F_V3F vR[6] = {
            {0,0, 1,0.5f,0,1, 0,0,1, RX,    RY[5],   0},
            {1,0, 1,0.5f,0,1, 0,0,1, RX + VW, RY[5],   0},
            {1,1, 0,0.5f,1,1, 0,0,1, RX + VW, RY[5] + VH,0},
            {0,0, 1,0.5f,0,1, 0,0,1, RX,    RY[5],   0},
            {1,1, 0,0.5f,1,1, 0,0,1, RX + VW, RY[5] + VH,0},
            {0,1, 0,0.5f,1,1, 0,0,1, RX,    RY[5] + VH,0},
        };
        glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_TEXTURE_2D);
    }

    // Clean up client state
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Divider line between left (explicit) and right (interleaved)
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(320, 0); glVertex2f(320, 480);
    glEnd();
}

// ---------------------------------------------------------------------------
// Test57 -- Multi-light support (GL_LIGHT0..GL_LIGHT3)
//
// Left half:  3 coloured directional lights on a sphere (no attenuation)
// Right half: same sphere with 3 positional lights + quadratic attenuation
//
// All 4 spheres in the scene use the same white material.
// The point of the test is that only the correct number of lights contribute
// and that the colours add correctly (red+green+blue = white at centre).
//
// Top row (2 spheres): directional lights
//   Light0 = red,   direction upper-left
//   Light1 = green, direction upper-right
//   Light2 = blue,  direction below
//
// Bottom row: spotlight test
//   Light0 = white spotlight pointing at left sphere,  cutoff=25deg
//   Light1 = white spotlight pointing at right sphere, cutoff=25deg
//   Both spheres in scene, only the targeted one is lit.
// ---------------------------------------------------------------------------

static void DrawTestSphere(float cx, float cy, float cz, float r, int lat, int lon)
{
    float latStep = PI_F / lat;
    float lonStep = 2.0f * PI_F / lon;
    for (int i = 0; i < lat; i++)
    {
        float phi0 = -PI_F / 2.0f + i * latStep;
        float phi1 = phi0 + latStep;
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= lon; j++)
        {
            float theta = j * lonStep;
            float ct = cosf_approx(theta), st = sinf_approx(theta);
            for (int p = 0; p < 2; p++)
            {
                float phi = (p == 0) ? phi0 : phi1;
                float nx = cosf_approx(phi) * ct;
                float ny = sinf_approx(phi);
                float nz = cosf_approx(phi) * st;
                glNormal3f(nx, ny, nz);
                glVertex3f(cx + r * nx, cy + r * ny, cz + r * nz);
            }
        }
        glEnd();
    }
}

static void Test57_MultiLight()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    // White material for all spheres
    GLfloat white[4] = { 1,1,1,1 };
    GLfloat black[4] = { 0,0,0,1 };
    GLfloat matAmb[4] = { 0.05f,0.05f,0.05f,1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 0);

    // Dark scene ambient so lights stand out clearly
    GLfloat sceneAmb[4] = { 0.02f,0.02f,0.02f,1 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmb);

    SetGLPerspective(55.0f, 640.0f / 480.0f, 0.1f, 50.0f);

    // -----------------------------------------------------------------------
    // TOP ROW: 3 coloured directional lights, sphere at left and right
    // -----------------------------------------------------------------------
    // Left sphere: all 3 lights on -> should look white at the highlighted point
    // Right sphere: only light0 (red) -> pure red highlight

    // Disable all lights first
    for (int i = 0;i < 4;i++) glDisable((GLenum)(GL_LIGHT0 + i));

    // Light 0: red, upper-left direction
    GLfloat redDiff[4] = { 1,0,0,1 };
    GLfloat greenDiff[4] = { 0,1,0,1 };
    GLfloat blueDiff[4] = { 0,0,1,1 };
    GLfloat noAmb[4] = { 0,0,0,1 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, redDiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, noAmb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, noAmb);
    { GLfloat p[4] = { -1,1,1,0 }; glLightfv(GL_LIGHT0, GL_POSITION, p); }

    glLightfv(GL_LIGHT1, GL_DIFFUSE, greenDiff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, noAmb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, noAmb);
    { GLfloat p[4] = { 1,1,1,0 }; glLightfv(GL_LIGHT1, GL_POSITION, p); }

    glLightfv(GL_LIGHT2, GL_DIFFUSE, blueDiff);
    glLightfv(GL_LIGHT2, GL_AMBIENT, noAmb);
    glLightfv(GL_LIGHT2, GL_SPECULAR, noAmb);
    { GLfloat p[4] = { 0,-1,1,0 }; glLightfv(GL_LIGHT2, GL_POSITION, p); }

    glEnable(GL_LIGHT0); glEnable(GL_LIGHT1); glEnable(GL_LIGHT2);

    // Left sphere: RGB lights -> should show colour blending
    glPushMatrix();
    glTranslatef(-2.0f, 1.2f, -6.0f);
    DrawTestSphere(0, 0, 0, 1.0f, 16, 24);
    glPopMatrix();

    // Right sphere: only red light
    glDisable(GL_LIGHT1); glDisable(GL_LIGHT2);
    glPushMatrix();
    glTranslatef(2.0f, 1.2f, -6.0f);
    DrawTestSphere(0, 0, 0, 1.0f, 16, 24);
    glPopMatrix();

    // -----------------------------------------------------------------------
    // BOTTOM ROW: attenuation test
    // Light0 = white, close to left sphere (strong), right sphere gets less
    // Light1 = white, close to right sphere (strong), left sphere gets less
    // -----------------------------------------------------------------------
    glDisable(GL_LIGHT0); glDisable(GL_LIGHT1); glDisable(GL_LIGHT2);

    GLfloat whiteDiff[4] = { 1,1,1,1 };

    // Left light: bright, positioned at left sphere, quadratic attenuation
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, noAmb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, noAmb);
    { GLfloat p[4] = { -2.0f,-1.2f,-6.0f,1 }; glLightfv(GL_LIGHT0, GL_POSITION, p); }
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.25f);

    // Right light: same but at right sphere
    glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, noAmb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, noAmb);
    { GLfloat p[4] = { 2.0f,-1.2f,-6.0f,1 }; glLightfv(GL_LIGHT1, GL_POSITION, p); }
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.25f);

    glEnable(GL_LIGHT0); glEnable(GL_LIGHT1);

    // Both spheres -- each is lit by its own close light, dimmer from the far one
    glPushMatrix();
    glTranslatef(-2.0f, -1.2f, -6.0f);
    DrawTestSphere(0, 0, 0, 1.0f, 16, 24);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0f, -1.2f, -6.0f);
    DrawTestSphere(0, 0, 0, 1.0f, 16, 24);
    glPopMatrix();

    // Restore state
    for (int i = 0;i < 4;i++) glDisable((GLenum)(GL_LIGHT0 + i));
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Restore attenuation defaults for light0/light1
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
}

static void Test58_PushPopClientAttrib()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // Four triangles arranged in a 2x2 grid.
    // Left half: x=10..305   Right half: x=335..630
    // Top row:   y=245..460  Bottom row: y=20..235
    // Y=0 is bottom.

    // Vertex data for LEFT half -- 4 triangles, 3 verts each = 12 verts
    static const GLfloat leftVerts[12*2] = {
        // Top-left: red triangle
        10,460,   157,245,  305,460,
        // Top-right: green triangle  (same shape, different colour handled via colour array)
        10,235,   157,20,   305,235,
        // Bottom-left: blue triangle
        335,460,  482,245,  630,460,
        // Bottom-right: yellow
        335,235,  482,20,   630,235,
    };
    static const GLfloat leftCols[12*4] = {
        1,0,0,1,  1,0,0,1,  1,0,0,1,   // red
        0,1,0,1,  0,1,0,1,  0,1,0,1,   // green
        0,0,1,1,  0,0,1,1,  0,0,1,1,   // blue
        1,1,0,1,  1,1,0,1,  1,1,0,1,   // yellow
    };

    // Garbage data -- wildly wrong positions
    static const GLfloat garbageVerts[12*2] = {
        9999,9999, 9999,8888, 8888,9999,
        9999,9999, 9999,8888, 8888,9999,
        9999,9999, 9999,8888, 8888,9999,
        9999,9999, 9999,8888, 8888,9999,
    };
    static const GLfloat garbageCols[12*4] = {
        0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,
        0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,
        0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,
        0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,  0.5f,0.5f,0.5f,1,
    };

    // -----------------------------------------------------------------------
    // STEP 1: Set up the CORRECT arrays BEFORE the push
    // -----------------------------------------------------------------------
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, leftVerts);
    glColorPointer(4, GL_FLOAT, 0, leftCols);

    // -----------------------------------------------------------------------
    // STEP 2: Draw LEFT half directly (reference -- no push/pop involved)
    // -----------------------------------------------------------------------
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // -----------------------------------------------------------------------
    // STEP 3: Push -- saves the correct pointers and enables
    // -----------------------------------------------------------------------
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

    // -----------------------------------------------------------------------
    // STEP 4: Corrupt EVERYTHING inside the push
    // -----------------------------------------------------------------------
    glVertexPointer(2, GL_FLOAT, 0, garbageVerts);
    glColorPointer(4, GL_FLOAT, 0, garbageCols);
    glDisableClientState(GL_COLOR_ARRAY);  // also corrupt enables

    // -----------------------------------------------------------------------
    // STEP 5: Pop -- must restore correct pointers AND enables
    // -----------------------------------------------------------------------
    glPopClientAttrib();

    // -----------------------------------------------------------------------
    // STEP 6: Draw RIGHT half -- must look identical to left half
    //         because vertex/colour pointers point at leftVerts/leftCols
    //         which covers BOTH halves of the screen in one array
    // -----------------------------------------------------------------------
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // Cleanup
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // White divider line
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();
}

// ---------------------------------------------------------------------------
// Test59 -- GL_MIRRORED_REPEAT texture wrap mode
//
// Creates a clearly ASYMMETRIC 4x4 texture: bright in top-left corner only,
// dark everywhere else.  This makes mirroring unmistakable:
//
//   GL_REPEAT:          bright corner always top-left of each tile
//   GL_MIRRORED_REPEAT: bright corner flips (TL, TR, BL, BR alternately)
//
// Left half: GL_REPEAT    -- bright spot always same corner every tile
// Right half: GL_MIRRORED -- bright spot alternates corners like a reflection
//
// UV range 0..4 gives 4 full tiles so the pattern repeats clearly.
// ---------------------------------------------------------------------------

static GLuint g_asymTex = 0;

static void MakeAsymTex()
{
    if (g_asymTex) return;

    // 32x32 RGBA texture with a clear left-to-right gradient:
    // Left edge = bright white, right edge = black.
    // Under GL_REPEAT:          each tile goes bright->dark, bright->dark (sawtooth)
    // Under GL_MIRRORED_REPEAT: tiles go bright->dark->bright->dark (smooth bounce)
    // The difference is immediately obvious.
    static GLubyte data[32*32*4];
    for (int y=0; y<32; y++)
    {
        for (int x=0; x<32; x++)
        {
            int idx = (y*32+x)*4;
            GLubyte horiz = (GLubyte)(255 - (x * 255 / 31)); // bright left, dark right
            GLubyte vert  = (GLubyte)(255 - (y * 255 / 31)); // bright bottom, dark top
            // Combine: top-left corner is brightest, bottom-right is darkest
            GLubyte v = (GLubyte)((int)(horiz + vert) / 2);
            data[idx+0] = v;
            data[idx+1] = v;
            data[idx+2] = v;
            data[idx+3] = 255;
        }
    }
    glGenTextures(1, &g_asymTex);
    glBindTexture(GL_TEXTURE_2D, g_asymTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
}

static void Test59_MirroredRepeat()
{
    MakeAsymTex();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_asymTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1,1,1);

    // LEFT half: GL_REPEAT -- bright spot always same corner
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(10,  20);
    glTexCoord2f(4,0); glVertex2f(305, 20);
    glTexCoord2f(4,4); glVertex2f(305, 460);
    glTexCoord2f(0,4); glVertex2f(10,  460);
    glEnd();

    // Divider
    glDisable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_asymTex);

    // RIGHT half: GL_MIRRORED_REPEAT -- bright spot alternates corners
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(335, 20);
    glTexCoord2f(4,0); glVertex2f(630, 20);
    glTexCoord2f(4,4); glVertex2f(630, 460);
    glTexCoord2f(0,4); glVertex2f(335, 460);
    glEnd();

    // Restore defaults
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDisable(GL_TEXTURE_2D);
}

static void Test60_ClipPlane()
{
    GLdouble plane[4] = { 1.0, 0.0, 0.0, -320.0 }; // keep x >= 320 in object/window coords
    GLdouble got[4]   = { 0, 0, 0, 0 };

    glClearColor(0.05f, 0.05f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // Store/query plane first.
    // This should work even if actual clipping is stubbed.
    glClipPlane(GL_CLIP_PLANE0, plane);
    glGetClipPlane(GL_CLIP_PLANE0, got);

    // -----------------------------------------------------------------------
    // LEFT: manual reference.
    // Draw only the right half of the would-be square.
    // This is what the clipped result should look like if clip planes work.
    // -----------------------------------------------------------------------
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.8f, 0.2f);
        glVertex2f(210, 160);
        glVertex2f(300, 160);
        glVertex2f(300, 320);
        glVertex2f(210, 320);
    glEnd();

    // Draw diagonal line showing the intended clipping boundary.
    glBegin(GL_LINES);
        glColor3f(1, 1, 0);
        glVertex2f(210, 160);
        glVertex2f(210, 320);
    glEnd();

    // -----------------------------------------------------------------------
    // MID: actual glClipPlane test.
    // A full red square crosses the plane.
    //
    // If clipping works, only the right half survives.
    // If stubbed, the full square is visible.
    // -----------------------------------------------------------------------
    glPushMatrix();
    glTranslatef(320.0f, 0.0f, 0.0f);

    // Plane is transformed by current modelview at glClipPlane time in real GL.
    // Here we set it after translate so the boundary is local x = 120.
    {
        GLdouble localPlane[4] = { 1.0, 0.0, 0.0, -120.0 };
        glClipPlane(GL_CLIP_PLANE0, localPlane);
    }

    glEnable(GL_CLIP_PLANE0);

    glBegin(GL_QUADS);
        glColor3f(0.9f, 0.1f, 0.1f);
        glVertex2f(40, 160);
        glVertex2f(220, 160);
        glVertex2f(220, 320);
        glVertex2f(40, 320);
    glEnd();

    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();

    // Yellow line at the intended clip boundary for the middle square.
    glBegin(GL_LINES);
        glColor3f(1, 1, 0);
        glVertex2f(440, 160);
        glVertex2f(440, 320);
    glEnd();

    // -----------------------------------------------------------------------
    // RIGHT: disabled clip plane control.
    // Always full blue square.
    // -----------------------------------------------------------------------
    glBegin(GL_QUADS);
        glColor3f(0.1f, 0.3f, 1.0f);
        glVertex2f(510, 160);
        glVertex2f(630, 160);
        glVertex2f(630, 320);
        glVertex2f(510, 320);
    glEnd();

    // Labels
    glColor3f(0.9f, 0.9f, 0.9f);

    glPushMatrix();
    glTranslatef(210, 110, 0);
    DrawStringCentered("REFERENCE HALF", 11.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(440, 110, 0);
    DrawStringCentered("CLIP PLANE", 11.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(570, 110, 0);
    DrawStringCentered("NO CLIP", 11.0f);
    glPopMatrix();

    // Query result visual marker:
    // If glGetClipPlane returned the local plane properly, draw a small green box.
    // Otherwise draw red.
    int queryOK =
        (got[0] == 1.0 && got[1] == 0.0 && got[2] == 0.0 && got[3] == -320.0);

    glBegin(GL_QUADS);
        if (queryOK) glColor3f(0, 1, 0);
        else         glColor3f(1, 0, 0);

        glVertex2f(10, 440);
        glVertex2f(40, 440);
        glVertex2f(40, 470);
        glVertex2f(10, 470);
    glEnd();

    glDisable(GL_CLIP_PLANE0);
}

static void T61_Quad(float x0, float y0, float x1, float y1,
                     float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
    glEnd();
}

// Draw PASS (green) or FAIL (red) bar at the bottom of a cell.
static void T61_Result(float cellX, float cellY, float cellW, bool pass)
{
    float barH = 18.0f;
    if (pass)
        glColor3f(0.0f, 0.9f, 0.1f);
    else
        glColor3f(0.9f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
        glVertex2f(cellX,        cellY);
        glVertex2f(cellX+cellW,  cellY);
        glVertex2f(cellX+cellW,  cellY+barH);
        glVertex2f(cellX,        cellY+barH);
    glEnd();
}

// Sample a pixel from the framebuffer (returns 0..255 per channel).
// Because we're on Xbox/D3D we use glReadPixels.
static void T61_ReadPixel(int x, int y, GLubyte* r, GLubyte* g, GLubyte* b)
{
    GLubyte px[4] = {0,0,0,0};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);
    *r = px[0]; *g = px[1]; *b = px[2];
}

// ---------------------------------------------------------------------------
// Cell geometry (all in 2D pixel space via SetGL2D(640,480))
// ---------------------------------------------------------------------------
//  4 columns x 2 rows, each cell 148x210, 10px gap, 8px border.
//  Row 1 (top):    y = 260..470
//  Row 0 (bottom): y =  20..230
//  Col 0: x=  10..158   Col 1: x= 168..316
//  Col 2: x= 326..474   Col 3: x= 484..632

static const float T61_CX[4] = { 10,  168,  326,  484 };
static const float T61_CY[2] = { 20,  260 };
static const float T61_CW   = 148.0f;
static const float T61_CH   = 210.0f;

// Inner drawing area within a cell (inset 4px, leave 22px at bottom for result bar)
static void T61_CellRect(int col, int row,
                          float* ox, float* oy, float* ow, float* oh)
{
    *ox = T61_CX[col] + 4;
    *oy = T61_CY[row] + 22;   // above the result bar
    *ow = T61_CW - 8;
    *oh = T61_CH - 26;
}

// Fill cell background.
static void T61_ClearCell(int col, int row, float r, float g, float b)
{
    T61_Quad(T61_CX[col], T61_CY[row],
             T61_CX[col]+T61_CW, T61_CY[row]+T61_CH, r, g, b);
}

// ---------------------------------------------------------------------------
// SetupScissor/Viewport for a cell (for perspective sub-tests).
// ---------------------------------------------------------------------------
static void T61_SetCellViewport(int col, int row)
{
    int x  = (int)T61_CX[col];
    int y  = (int)T61_CY[row];
    int w  = (int)T61_CW;
    int h  = (int)T61_CH;
    glScissor(x, y, w, h);
    glViewport(x, y, w, h);
}

// ---------------------------------------------------------------------------
// Sub-test 0: Full clip -- quad entirely outside plane -> cell stays black
// Plane: x >= 9999 (nothing passes). Quad drawn at x=20..80.
// PASS if center pixel is black (no quad drawn).
// ---------------------------------------------------------------------------
static bool T61_Sub0_FullClip(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    GLdouble plane[4] = { 1.0, 0.0, 0.0, -9999.0 }; // keep x >= 9999
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    T61_Quad(ox, oy, ox+ow, oy+oh, 1.0f, 0.0f, 0.0f);
    glFlush();

    glDisable(GL_CLIP_PLANE0);

    if (!doReadback) return true;
    GLubyte r,g,b;
    T61_ReadPixel((int)(ox+ow*0.5f), (int)(oy+oh*0.5f), &r, &g, &b);
    // Should be the dark background, not red
    bool pass = (r < 30 && g < 30 && b < 30);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 1: No clip -- quad entirely inside plane -> full quad visible
// Plane: x >= -9999 (everything passes).
// PASS if center pixel is red (quad fully drawn).
// ---------------------------------------------------------------------------
static bool T61_Sub1_NoClip(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    GLdouble plane[4] = { 1.0, 0.0, 0.0, 9999.0 }; // keep x >= -9999
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    T61_Quad(ox, oy, ox+ow, oy+oh, 0.8f, 0.1f, 0.1f);
    glFlush();

    glDisable(GL_CLIP_PLANE0);

    if (!doReadback) return true;
    GLubyte r,g,b;
    T61_ReadPixel((int)(ox+ow*0.5f), (int)(oy+oh*0.5f), &r, &g, &b);
    bool pass = (r > 150 && g < 60 && b < 60);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 2: Half clip -- plane bisects quad vertically at midpoint.
// Plane: local x >= ox+ow*0.5 (right half kept).
// PASS if left-center is black and right-center is red.
// ---------------------------------------------------------------------------
static bool T61_Sub2_HalfClip(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    float midX = ox + ow * 0.5f;
    // Plane equation in current object space (SetGL2D, MV=identity): x - midX >= 0
    GLdouble plane[4] = { 1.0, 0.0, 0.0, (double)(-midX) };
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    T61_Quad(ox, oy, ox+ow, oy+oh, 0.8f, 0.1f, 0.1f);
    glFlush();

    glDisable(GL_CLIP_PLANE0);

    if (!doReadback) return true;
    GLubyte r0,g0,b0, r1,g1,b1;
    // Left quarter -- should be clipped (dark)
    T61_ReadPixel((int)(ox + ow*0.25f), (int)(oy+oh*0.5f), &r0, &g0, &b0);
    // Right quarter -- should be kept (red)
    T61_ReadPixel((int)(ox + ow*0.75f), (int)(oy+oh*0.5f), &r1, &g1, &b1);

    bool pass = (r0 < 40 && r1 > 150 && g1 < 60);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 3: Eye-space plane -- plane set at identity MV, then object translated.
// The plane must stay FIXED in eye space and NOT follow the object.
//
// Uses a local -1..1 ortho space scoped to the cell via scissor+viewport.
// Plane x >= 0 set at identity. Object translated +1.5 in x.
// Quad drawn at LOCAL x = -1..-0.1 (entirely left of 0 in local/object space).
//   Correct: eye_x = local_x + 1.5 -> 0.5..1.4 -> all >= 0 -> VISIBLE
//   Buggy:   plane follows object, tests local_x < 0 -> CLIPPED
// ---------------------------------------------------------------------------
static bool T61_Sub3_EyeSpacePlane(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    // Set up a local -1..1 ortho viewport scoped to this cell
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)ox, (int)oy, (int)ow, (int)oh);
    glViewport((int)ox, (int)oy, (int)ow, (int)oh);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    // Set clip plane at identity MV: keep eye_x >= 0 (right half of NDC)
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    GLdouble plane[4] = { 1.0, 0.0, 0.0, 0.0 };
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    // Translate object right by 1.5 -- eye_x = local_x + 1.5
    glTranslatef(1.5f, 0.0f, 0.0f);

    // Draw quad at local x = -1..-0.1: entirely LEFT of 0 in local/object space.
    // eye_x range = 0.5..1.4 -> all >= 0 -> VISIBLE (correct eye-space plane).
    // If plane wrongly followed object: local_x < 0 -> fully CLIPPED.
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.8f);
        glVertex2f(-0.1f, -0.8f);
        glVertex2f(-0.1f,  0.8f);
        glVertex2f(-1.0f,  0.8f);
    glEnd();
    glFlush();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_SCISSOR_TEST);

    // Restore full 2D pixel space
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0.0, 640.0, 0.0, 480.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Sample right portion of cell (where eye_x maps to in NDC).
    // eye_x=0.5..1.4 -> right ~75% of cell. Sample at 80% across.
    if (!doReadback) return true;
    GLubyte r,g,b;
    T61_ReadPixel((int)(ox + ow*0.8f), (int)(oy + oh*0.5f), &r,&g,&b);
    bool pass = (r > 150 && g < 60 && b < 60);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 4: Plane after rotate -- plane follows object rotation.
// Set MV = Rotate45 around Z, then set plane x >= 0 (in rotated local space).
// In rotated space, x>=0 corresponds to the diagonal half of the drawn quad.
// PASS: only the rotated-right half of the quad is visible.
// We verify by checking two diagonal sample points.
// ---------------------------------------------------------------------------
static bool T61_Sub4_PlaneAfterRotate(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    float cx = ox + ow * 0.5f;
    float cy = oy + oh * 0.5f;

    // Scissor to cell so the rotated quad cannot bleed outside
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)ox, (int)oy, (int)ow, (int)oh);

    // Rotate 45deg around Z, then set plane x >= 0 in that rotated space.
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glTranslatef(cx, cy, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);

    // In this rotated frame, x>=0 keeps the "right" side relative to the 45deg axis.
    // In screen space that's roughly the upper-right half of the quad.
    GLdouble plane[4] = { 1.0, 0.0, 0.0, 0.0 };
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    // Draw a quad centered on origin in local rotated space (will appear rotated on screen)
    float hs = ow * 0.4f; // half size
    glColor3f(0.8f, 0.5f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-hs, -hs);
        glVertex2f( hs, -hs);
        glVertex2f( hs,  hs);
        glVertex2f(-hs,  hs);
    glEnd();
    glFlush();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_SCISSOR_TEST);
    glLoadIdentity();

    // In rotated local space, x>=0 = right half. After 45deg rotation:
    // local +x maps to screen direction (1/sqrt2, 1/sqrt2).
    // Sample upper-right of cell (should be orange/visible) vs lower-left (clipped/dark).
    if (!doReadback) return true;
    GLubyte rUR,gUR,bUR, rLL,gLL,bLL;
    T61_ReadPixel((int)(cx + ow*0.15f), (int)(cy + oh*0.15f), &rUR,&gUR,&bUR);
    T61_ReadPixel((int)(cx - ow*0.15f), (int)(cy - oh*0.15f), &rLL,&gLL,&bLL);

    bool pass = (rUR > 120 && gUR > 60 && rLL < 40);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 5: Two simultaneous planes -- top-right quadrant only.
// PLANE0: y >= cellMidY (top half)
// PLANE1: x >= cellMidX (right half)
// Only top-right quadrant of the drawn quad should be visible.
// ---------------------------------------------------------------------------
static bool T61_Sub5_TwoPlanes(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    float midX = ox + ow * 0.5f;
    float midY = oy + oh * 0.5f;

    GLdouble plane0[4] = { 0.0, 1.0, 0.0, (double)(-midY) }; // y >= midY
    GLdouble plane1[4] = { 1.0, 0.0, 0.0, (double)(-midX) }; // x >= midX
    glClipPlane(GL_CLIP_PLANE0, plane0);
    glClipPlane(GL_CLIP_PLANE1, plane1);
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);

    T61_Quad(ox, oy, ox+ow, oy+oh, 0.2f, 0.6f, 0.9f);
    glFlush();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);

    // Top-right quadrant -> blue (visible)
    // Bottom-left quadrant -> dark (clipped)
    // Top-left quadrant -> dark (clipped by plane1)
    // Bottom-right quadrant -> dark (clipped by plane0)
    if (!doReadback) return true;
    GLubyte rTR,gTR,bTR, rBL,gBL,bBL, rTL,gTL,bTL, rBR,gBR,bBR;
    T61_ReadPixel((int)(ox+ow*0.75f), (int)(oy+oh*0.75f), &rTR,&gTR,&bTR);
    T61_ReadPixel((int)(ox+ow*0.25f), (int)(oy+oh*0.25f), &rBL,&gBL,&bBL);
    T61_ReadPixel((int)(ox+ow*0.25f), (int)(oy+oh*0.75f), &rTL,&gTL,&bTL);
    T61_ReadPixel((int)(ox+ow*0.75f), (int)(oy+oh*0.25f), &rBR,&gBR,&bBR);

    bool pass = (bTR > 150 && rBL < 40 && bTL < 60 && bBR < 60);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 6: Flush-before-disable pattern (the original deferred-flush bug).
// Draw with clip active, glFlush(), glDisable() -- geometry must be clipped.
// This is the exact pattern from the new Test60.
// Without the FlushVB() fix in glEnable/Disable, this would show a full quad.
// With the fix, the right half is clipped away.
// ---------------------------------------------------------------------------
static bool T61_Sub6_FlushPattern(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    float midX = ox + ow * 0.5f;
    GLdouble plane[4] = { -1.0, 0.0, 0.0, (double)(midX) }; // keep x <= midX (left half)
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    T61_Quad(ox, oy, ox+ow, oy+oh, 0.9f, 0.7f, 0.1f); // yellow

    // Flush BEFORE disable -- this is the critical pattern
    glFlush();
    glDisable(GL_CLIP_PLANE0);

    if (!doReadback) return true;
    GLubyte rL,gL,bL, rR,gR,bR;
    // Left half -- should be yellow (kept)
    T61_ReadPixel((int)(ox+ow*0.25f), (int)(oy+oh*0.5f), &rL,&gL,&bL);
    // Right half -- should be dark (clipped)
    T61_ReadPixel((int)(ox+ow*0.75f), (int)(oy+oh*0.5f), &rR,&gR,&bR);

    bool pass = (rL > 150 && gL > 100 && rR < 40);
    return pass;
}

// ---------------------------------------------------------------------------
// Sub-test 7: Boundary vertex -- vertex exactly on plane (dot==0) must be KEPT.
// Draw a triangle with one vertex exactly on the plane.
// The vertex on the plane must appear; clipped side must not.
// ---------------------------------------------------------------------------
static bool T61_Sub7_BoundaryVertex(int col, int row, bool doReadback)
{
    float ox, oy, ow, oh;
    T61_CellRect(col, row, &ox, &oy, &ow, &oh);

    T61_ClearCell(col, row, 0.05f, 0.05f, 0.05f);

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    float midX = ox + ow * 0.5f;
    // Plane: x >= midX (keep right half)
    GLdouble plane[4] = { 1.0, 0.0, 0.0, (double)(-midX) };
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);

    // Triangle:
    //   V0: exactly on plane (x=midX, y=oy) -- dot=0, must be kept
    //   V1: right side (x=ox+ow, y=oy) -- inside, kept
    //   V2: right side (x=ox+ow, y=oy+oh) -- inside, kept
    // The triangle is entirely in the kept region (all x >= midX).
    // A second triangle entirely outside:
    //   V0: (ox, oy) left  -- dot < 0, clipped
    //   V1: (midX-1, oy)   -- dot < 0, clipped
    //   V2: (ox, oy+oh)    -- dot < 0, clipped
    glColor3f(0.2f, 0.9f, 0.3f);
    glBegin(GL_TRIANGLES);
        // Inside triangle (boundary + 2 inside verts)
        glVertex2f(midX,    oy);
        glVertex2f(ox+ow,   oy);
        glVertex2f(ox+ow,   oy+oh);
        // Outside triangle (all outside)
        glVertex2f(ox,      oy);
        glVertex2f(midX-2,  oy);
        glVertex2f(ox,      oy+oh);
    glEnd();
    glFlush();

    glDisable(GL_CLIP_PLANE0);

    if (!doReadback) return true;
    GLubyte rR,gR,bR, rL,gL,bL;
    // Right side -- should be green (kept triangle visible)
    T61_ReadPixel((int)(ox+ow*0.85f), (int)(oy+oh*0.5f), &rR,&gR,&bR);
    // Left side -- should be dark (outside triangle clipped)
    T61_ReadPixel((int)(ox+ow*0.15f), (int)(oy+oh*0.5f), &rL,&gL,&bL);

    bool pass = (gR > 150 && rL < 40 && gL < 40);
    return pass;
}

// ---------------------------------------------------------------------------
// Test61 entry point
// ---------------------------------------------------------------------------
static void Test61_ClipPlaneExhaustive()
{
    // Make sure all clip planes are off entering this test
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);

    glClearColor(0.15f, 0.15f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_SCISSOR_TEST);

    // All sub-tests run in 2D pixel space
    glViewport(0, 0, 640, 480);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Each sub-test function draws geometry AND reads pixels to evaluate pass/fail.
    // glReadPixels forces a GPU stall on Xbox, so we only do the readback once.
    // Every frame: always call the draw functions.
    // First frame only: capture the pass/fail results from glReadPixels.
    static bool s_evaluated = false;
    static bool results[8] = { false,false,false,false,false,false,false,false };

    bool r0 = T61_Sub0_FullClip        (0, 0, !s_evaluated);
    bool r1 = T61_Sub1_NoClip          (1, 0, !s_evaluated);
    bool r2 = T61_Sub2_HalfClip        (2, 0, !s_evaluated);
    bool r3 = T61_Sub3_EyeSpacePlane   (3, 0, !s_evaluated);
    bool r4 = T61_Sub4_PlaneAfterRotate (0, 1, !s_evaluated);
    bool r5 = T61_Sub5_TwoPlanes        (1, 1, !s_evaluated);
    bool r6 = T61_Sub6_FlushPattern     (2, 1, !s_evaluated);
    bool r7 = T61_Sub7_BoundaryVertex   (3, 1, !s_evaluated);

    if (!s_evaluated)
    {
        s_evaluated = true;
        results[0]=r0; results[1]=r1; results[2]=r2; results[3]=r3;
        results[4]=r4; results[5]=r5; results[6]=r6; results[7]=r7;
    }

    // Reset to clean 2D state for drawing result bars and labels
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glViewport(0, 0, 640, 480);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Draw result bars
    static const char* labels[8] = {
        "FULL CLIP", "NO CLIP", "HALF CLIP", "EYE-SP PLANE",
        "ROT PLANE", "2 PLANES", "FLUSH PAT", "BOUNDARY V"
    };
    int col, row;
    for (int i = 0; i < 8; i++)
    {
        col = i % 4;
        row = i / 4;
        T61_Result(T61_CX[col], T61_CY[row], T61_CW, results[i]);

        // Label
        glColor3f(0.9f, 0.9f, 0.9f);
        glPushMatrix();
        glTranslatef(T61_CX[col] + T61_CW*0.5f, T61_CY[row] + T61_CH - 12.0f, 0.0f);
        DrawStringCentered(labels[i], 9.0f);
        glPopMatrix();
    }

    // Overall PASS/FAIL banner
    bool allPass = true;
    for (int i = 0; i < 8; i++) if (!results[i]) allPass = false;

    float bx = 170.0f, bw = 300.0f, bh = 24.0f, by = 455.0f;
    if (allPass) glColor3f(0.0f, 0.85f, 0.1f);
    else         glColor3f(0.85f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
        glVertex2f(bx,    by);    glVertex2f(bx+bw, by);
        glVertex2f(bx+bw, by+bh); glVertex2f(bx,    by+bh);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(bx + bw*0.5f, by + bh*0.5f - 4.0f, 0.0f);
    DrawStringCentered(allPass ? "ALL PASS" : "SOME FAILED", 11.0f);
    glPopMatrix();
}

// ---------------------------------------------------------------------------
// Test62 -- glClipPlane rotating cube demo
//
// Proves clip planes work by drawing a rotating cube visible through
// different clip plane configurations. Each phase shows a different cut.
//
// The cube is drawn TWICE per frame:
//   LEFT half  -- with clip plane active (should show partial cube)
//   RIGHT half -- no clip plane (full cube for reference)
//
// The trick: draw the RIGHT half first (no clip), then enable clip plane,
// draw the LEFT half, then glFlush() BEFORE disabling the plane. This
// ensures the clipped geometry is submitted while the clip is still active.
//
// Phase 0: y >= 0 in eye space -- top half of cube visible
// Phase 1: x + y >= 0 diagonal cut
// Phase 2: two planes -- y >= 0 AND x >= 0 (top-right quadrant only)
// ---------------------------------------------------------------------------

static void DrawCubeForClip()
{
    // Proven correct winding from Test4
    static const GLfloat cv[8][3] = {
        {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
        {-1,-1, 1},{1,-1, 1},{1,1, 1},{-1,1, 1}
    };
    static const int cf[6][4] = {
        {4,5,6,7},{1,0,3,2},{0,4,7,3},{5,1,2,6},{7,6,2,3},{0,1,5,4}
    };
    static const GLfloat cc[6][3] = {
        {1,0,0},{0,1,0},{0,0,1},{1,1,0},{0,1,1},{1,0,1}
    };
    for (int f=0; f<6; f++) {
        glColor3f(cc[f][0],cc[f][1],cc[f][2]);
        glBegin(GL_QUADS);
        for (int v=0; v<4; v++)
            glVertex3f(cv[cf[f][v]][0], cv[cf[f][v]][1], cv[cf[f][v]][2]);
        glEnd();
    }
}

static void Test62_ClipPlaneCube()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_LIGHTING);

    // Make sure clip planes are off at start of frame
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);

    // -----------------------------------------------------------------------
    // RIGHT HALF: full cube, NO clip plane
    // Draw this FIRST while clip is definitely off
    // -----------------------------------------------------------------------
    glEnable(GL_SCISSOR_TEST);
    glScissor(325, 0, 315, 480);
    glViewport(325, 0, 315, 480);
    SetGLPerspective(60.0f, 315.0f/480.0f, 0.1f, 50.0f);
    glTranslatef(0, 0, -4.0f);
    glRotatef(g_time * 35.0f, 1, 0, 0);
    glRotatef(g_time * 55.0f, 0, 1, 0);
    DrawCubeForClip();

    // Flush right half geometry before changing any state
    glFlush();

    // -----------------------------------------------------------------------
    // Set clip plane -- reset modelview for the left cube
    // -----------------------------------------------------------------------
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    GLdouble eq0[4], eq1[4];
    switch (g_rotPhase % 3)
    {
    case 0:
        // y >= 0 in eye space: shows top half
        eq0[0]=0; eq0[1]=1; eq0[2]=0; eq0[3]=0;
        glClipPlane(GL_CLIP_PLANE0, eq0);
        glEnable(GL_CLIP_PLANE0);
        break;
    case 1:
        // diagonal x+y >= 0
        eq0[0]=0.707; eq0[1]=0.707; eq0[2]=0; eq0[3]=0;
        glClipPlane(GL_CLIP_PLANE0, eq0);
        glEnable(GL_CLIP_PLANE0);
        break;
    case 2:
        // two planes: top-right quadrant only
        eq0[0]=0; eq0[1]=1; eq0[2]=0; eq0[3]=0;
        eq1[0]=1; eq1[1]=0; eq1[2]=0; eq1[3]=0;
        glClipPlane(GL_CLIP_PLANE0, eq0);
        glClipPlane(GL_CLIP_PLANE1, eq1);
        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_CLIP_PLANE1);
        break;
    }

    // -----------------------------------------------------------------------
    // LEFT HALF: same cube, clip plane active
    // -----------------------------------------------------------------------
    glScissor(0, 0, 315, 480);
    glViewport(0, 0, 315, 480);
    SetGLPerspective(60.0f, 315.0f/480.0f, 0.1f, 50.0f);
    glTranslatef(0, 0, -4.0f);
    glRotatef(g_time * 35.0f, 1, 0, 0);
    glRotatef(g_time * 55.0f, 0, 1, 0);
    DrawCubeForClip();

    // CRITICAL: flush clipped geometry BEFORE disabling clip planes
    glFlush();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_SCISSOR_TEST);

    // Restore full viewport and draw divider
    glViewport(0, 0, 640, 480);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0, 640, 0, 480, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();
}

static void Test63_VertexDoubles()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // 4 cols x 3 rows. CW=140, CH=130, gap=10
    // Col X: 10, 160, 310, 460   Row bottom Y: 20, 165, 310
    const float CW = 140.0f, CH = 130.0f;
    const float CX[4] = { 10, 160, 310, 460 };
    const float CY[3] = { 20, 165, 310 };
    static const GLfloat col[4][3] = {
        {1.0f,0.2f,0.2f}, {0.2f,1.0f,0.2f}, {0.2f,0.4f,1.0f}, {1.0f,1.0f,0.2f}
    };

    // --- Row 0: 2D double variants ---
    for (int c = 0; c < 4; c++)
    {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        double x0=(double)CX[c], y0=(double)CY[0];
        double x1=x0+CW,         y1=y0+CH;
        switch (c)
        {
        case 0: // glVertex2d
            glBegin(GL_QUADS);
            glVertex2d(x0,y0); glVertex2d(x1,y0);
            glVertex2d(x1,y1); glVertex2d(x0,y1);
            glEnd(); break;
        case 1: // glVertex2dv
            {
                GLdouble v0[2]={x0,y0}, v1[2]={x1,y0}, v2[2]={x1,y1}, v3[2]={x0,y1};
                glBegin(GL_QUADS);
                glVertex2dv(v0); glVertex2dv(v1); glVertex2dv(v2); glVertex2dv(v3);
                glEnd();
            } break;
        case 2: // glVertex2sv
            {
                GLshort v0[2]={(GLshort)x0,(GLshort)y0}, v1[2]={(GLshort)x1,(GLshort)y0};
                GLshort v2[2]={(GLshort)x1,(GLshort)y1}, v3[2]={(GLshort)x0,(GLshort)y1};
                glBegin(GL_QUADS);
                glVertex2sv(v0); glVertex2sv(v1); glVertex2sv(v2); glVertex2sv(v3);
                glEnd();
            } break;
        case 3: // mix: glVertex2d and glVertex2dv alternating
            {
                GLdouble v1a[2]={x1,y0}, v3a[2]={x0,y1};
                glBegin(GL_QUADS);
                glVertex2d(x0,y0); glVertex2dv(v1a);
                glVertex2d(x1,y1); glVertex2dv(v3a);
                glEnd();
            } break;
        }
    }

    // --- Row 1: 3D double variants ---
    for (int c = 0; c < 4; c++)
    {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        double x0=(double)CX[c], y0=(double)CY[1];
        double x1=x0+CW,         y1=y0+CH;
        switch (c)
        {
        case 0: // glVertex3d
            glBegin(GL_QUADS);
            glVertex3d(x0,y0,0); glVertex3d(x1,y0,0);
            glVertex3d(x1,y1,0); glVertex3d(x0,y1,0);
            glEnd(); break;
        case 1: // glVertex3dv
            {
                GLdouble v0[3]={x0,y0,0}, v1[3]={x1,y0,0};
                GLdouble v2[3]={x1,y1,0}, v3[3]={x0,y1,0};
                glBegin(GL_QUADS);
                glVertex3dv(v0); glVertex3dv(v1); glVertex3dv(v2); glVertex3dv(v3);
                glEnd();
            } break;
        case 2: // glVertex3sv
            {
                GLshort v0[3]={(GLshort)x0,(GLshort)y0,0}, v1[3]={(GLshort)x1,(GLshort)y0,0};
                GLshort v2[3]={(GLshort)x1,(GLshort)y1,0}, v3[3]={(GLshort)x0,(GLshort)y1,0};
                glBegin(GL_QUADS);
                glVertex3sv(v0); glVertex3sv(v1); glVertex3sv(v2); glVertex3sv(v3);
                glEnd();
            } break;
        case 3: // glVertex3d with non-zero z (still 2D ortho so z ignored)
            glBegin(GL_QUADS);
            glVertex3d(x0,y0,0.5); glVertex3d(x1,y0,0.5);
            glVertex3d(x1,y1,0.5); glVertex3d(x0,y1,0.5);
            glEnd(); break;
        }
    }

    // --- Row 2: 4D homogeneous variants ---
    for (int c = 0; c < 4; c++)
    {
        glColor3f(col[c][0], col[c][1], col[c][2]);
        double x0=(double)CX[c], y0=(double)CY[2];
        double x1=x0+CW,         y1=y0+CH;
        switch (c)
        {
        case 0: // glVertex4d w=1
            glBegin(GL_QUADS);
            glVertex4d(x0,y0,0,1); glVertex4d(x1,y0,0,1);
            glVertex4d(x1,y1,0,1); glVertex4d(x0,y1,0,1);
            glEnd(); break;
        case 1: // glVertex4dv w=1
            {
                GLdouble v0[4]={x0,y0,0,1}, v1[4]={x1,y0,0,1};
                GLdouble v2[4]={x1,y1,0,1}, v3[4]={x0,y1,0,1};
                glBegin(GL_QUADS);
                glVertex4dv(v0); glVertex4dv(v1); glVertex4dv(v2); glVertex4dv(v3);
                glEnd();
            } break;
        case 2: // glVertex4i / glVertex4iv
            {
                GLint v0[4]={(GLint)x0,(GLint)y0,0,1}, v1[4]={(GLint)x1,(GLint)y0,0,1};
                GLint v2[4]={(GLint)x1,(GLint)y1,0,1}, v3[4]={(GLint)x0,(GLint)y1,0,1};
                glBegin(GL_QUADS);
                glVertex4i((GLint)x0,(GLint)y0,0,1);
                glVertex4iv(v1);
                glVertex4i((GLint)x1,(GLint)y1,0,1);
                glVertex4iv(v3);
                glEnd();
            } break;
        case 3: // glVertex4s / glVertex4sv
            {
                GLshort v0[4]={(GLshort)x0,(GLshort)y0,0,1}, v1[4]={(GLshort)x1,(GLshort)y0,0,1};
                GLshort v2[4]={(GLshort)x1,(GLshort)y1,0,1}, v3[4]={(GLshort)x0,(GLshort)y1,0,1};
                glBegin(GL_QUADS);
                glVertex4s((GLshort)x0,(GLshort)y0,0,1);
                glVertex4sv(v1);
                glVertex4s((GLshort)x1,(GLshort)y1,0,1);
                glVertex4sv(v3);
                glEnd();
            } break;
        }
    }

    // Row labels
    glColor3f(0.8f, 0.8f, 0.8f);
    static const char* rowLabels[3] = { "2D DOUBLE VARIANTS", "3D DOUBLE VARIANTS", "4D HOMOGENEOUS" };
    for (int r = 0; r < 3; r++)
    {
        glPushMatrix();
        glTranslatef(320.0f, CY[r] + CH + 6.0f, 0.0f);
        DrawStringCentered(rowLabels[r], 9.0f);
        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// Test64 -- glNormal3dv + glTexCoord type variants
//
// LEFT section (x=0..319): Lit sphere drawn with glNormal3dv.
//   Should look identical to a sphere drawn with glNormal3fv.
//   Reference sphere on left, glNormal3dv sphere on right of that section.
//
// RIGHT section (x=320..639): Grid of textured quads, each using a different
//   glTexCoord variant. All should show the checker texture correctly.
//   Row 0: glTexCoord1d, glTexCoord2d, glTexCoord2dv
//   Row 1: glTexCoord1fv, glTexCoord3fv, glTexCoord4fv
//   Row 2: glTexCoord2iv, glTexCoord2sv, glTexCoord2d (repeat sanity)
// ---------------------------------------------------------------------------
static void Test64_NormalAndTexCoordVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    // -----------------------------------------------------------------------
    // LEFT: glNormal3dv sphere comparison
    // Left half: glNormal3fv reference | Right half: glNormal3dv
    // -----------------------------------------------------------------------
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, 320, 480);
    glViewport(0, 0, 320, 480);
    SetGLPerspective(55.0f, 320.0f/480.0f, 0.1f, 50.0f);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    GLfloat lpos[4]={2,3,4,1}, ldiff[4]={1,1,0.9f,1}, lamb[4]={0.1f,0.1f,0.15f,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);
    GLfloat matD[4]={0.7f,0.5f,0.2f,1}, matA[4]={0.15f,0.1f,0.05f,1}, matB[4]={0,0,0,1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  matD);
    glMaterialfv(GL_FRONT, GL_AMBIENT,  matA);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matB);
    glMaterialf (GL_FRONT, GL_SHININESS, 0);

    // Reference sphere (left): glNormal3fv
    glPushMatrix();
    glTranslatef(-1.3f, 0.0f, -5.0f);
    DrawLitSphere_NV(0, 0,0,0, 1.0f, 14, 20); // mode 0 = glNormal3f
    glPopMatrix();

    // Test sphere (right): glNormal3dv -- drawn manually
    glPushMatrix();
    glTranslatef(1.3f, 0.0f, -5.0f);
    {
        float r = 1.0f;
        int latSegs = 14, lonSegs = 20;
        float latStep = PI_F / latSegs;
        float lonStep = 2.0f * PI_F / lonSegs;
        for (int lat = 0; lat < latSegs; lat++)
        {
            float phi0 = -PI_F/2.0f + lat*latStep;
            float phi1 = phi0 + latStep;
            glBegin(GL_TRIANGLE_STRIP);
            for (int lon = 0; lon <= lonSegs; lon++)
            {
                float theta = lon * lonStep;
                float ct = cosf_approx(theta), st = sinf_approx(theta);
                for (int pass = 0; pass < 2; pass++)
                {
                    float phi = (pass==0) ? phi0 : phi1;
                    float nx = cosf_approx(phi)*ct;
                    float ny = sinf_approx(phi);
                    float nz = cosf_approx(phi)*st;
                    GLdouble nv[3] = {(GLdouble)nx,(GLdouble)ny,(GLdouble)nz};
                    glNormal3dv(nv);
                    glVertex3f(nx*r, ny*r, nz*r);
                }
            }
            glEnd();
        }
    }
    glPopMatrix();

    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE); glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST);

    // Label
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, 640, 480);
    SetGL2D(640.0f, 480.0f);
    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(80.0f, 12.0f, 0.0f);
    DrawStringCentered("3FV REF", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(240.0f, 12.0f, 0.0f);
    DrawStringCentered("3DV TEST", 9.0f); glPopMatrix();

    // -----------------------------------------------------------------------
    // RIGHT: glTexCoord variant grid (3 rows x 3 cols within x=330..630)
    // -----------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);

    // CW=90, CH=140, gap=5
    // Col X (within right section): 330, 425, 520
    // Row Y: 25, 175, 325
    const float TCW=90.0f, TCH=140.0f;
    const float TX[3]={330,425,520};
    const float TY[3]={25,175,325};

    // Row 0: glTexCoord1d, glTexCoord2d, glTexCoord2dv
    // glTexCoord1d: use g_rgbaTex (4x4 RGBA) -- at t=0 bottom row is
    // red (s<0.5) and yellow (s>=0.5), proving s is set correctly.
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    {
        float x0=TX[0],y0=TY[0],x1=x0+TCW,y1=y0+TCH;
        glBegin(GL_QUADS);
        glTexCoord1d(0.0); glVertex2f(x0,y0);
        glTexCoord1d(1.0); glVertex2f(x1,y0);
        glTexCoord1d(1.0); glVertex2f(x1,y1);
        glTexCoord1d(0.0); glVertex2f(x0,y1);
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    {
        float x0=TX[1],y0=TY[0],x1=x0+TCW,y1=y0+TCH;
        glBegin(GL_QUADS);
        glTexCoord2d(0.0,0.0); glVertex2f(x0,y0);
        glTexCoord2d(1.0,0.0); glVertex2f(x1,y0);
        glTexCoord2d(1.0,1.0); glVertex2f(x1,y1);
        glTexCoord2d(0.0,1.0); glVertex2f(x0,y1);
        glEnd();
    }
    {
        float x0=TX[2],y0=TY[0],x1=x0+TCW,y1=y0+TCH;
        GLdouble tc00[2]={0.0,0.0}, tc10[2]={1.0,0.0};
        GLdouble tc11[2]={1.0,1.0}, tc01[2]={0.0,1.0};
        glBegin(GL_QUADS);
        glTexCoord2dv(tc00); glVertex2f(x0,y0);
        glTexCoord2dv(tc10); glVertex2f(x1,y0);
        glTexCoord2dv(tc11); glVertex2f(x1,y1);
        glTexCoord2dv(tc01); glVertex2f(x0,y1);
        glEnd();
    }

    // Row 1: glTexCoord1fv, glTexCoord3fv, glTexCoord4fv
    // glTexCoord1fv: same g_rgbaTex approach as 1d above
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    {
        float x0=TX[0],y0=TY[1],x1=x0+TCW,y1=y0+TCH;
        GLfloat t0[1]={0.0f}, t1[1]={1.0f};
        glBegin(GL_QUADS);
        glTexCoord1fv(t0); glVertex2f(x0,y0);
        glTexCoord1fv(t1); glVertex2f(x1,y0);
        glTexCoord1fv(t1); glVertex2f(x1,y1);
        glTexCoord1fv(t0); glVertex2f(x0,y1);
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    {
        float x0=TX[1],y0=TY[1],x1=x0+TCW,y1=y0+TCH;
        // glTexCoord3fv: s,t,r -- r ignored, s and t used
        GLfloat tc00[3]={0,0,0}, tc10[3]={1,0,0};
        GLfloat tc11[3]={1,1,0}, tc01[3]={0,1,0};
        glBegin(GL_QUADS);
        glTexCoord3fv(tc00); glVertex2f(x0,y0);
        glTexCoord3fv(tc10); glVertex2f(x1,y0);
        glTexCoord3fv(tc11); glVertex2f(x1,y1);
        glTexCoord3fv(tc01); glVertex2f(x0,y1);
        glEnd();
    }
    {
        float x0=TX[2],y0=TY[1],x1=x0+TCW,y1=y0+TCH;
        // glTexCoord4fv: s,t,r,q -- r,q ignored
        GLfloat tc00[4]={0,0,0,1}, tc10[4]={1,0,0,1};
        GLfloat tc11[4]={1,1,0,1}, tc01[4]={0,1,0,1};
        glBegin(GL_QUADS);
        glTexCoord4fv(tc00); glVertex2f(x0,y0);
        glTexCoord4fv(tc10); glVertex2f(x1,y0);
        glTexCoord4fv(tc11); glVertex2f(x1,y1);
        glTexCoord4fv(tc01); glVertex2f(x0,y1);
        glEnd();
    }

    // Row 2: glTexCoord2iv, glTexCoord2sv, glTexCoord2d (sanity)
    {
        float x0=TX[0],y0=TY[2],x1=x0+TCW,y1=y0+TCH;
        GLint tc00[2]={0,0}, tc10[2]={1,0};
        GLint tc11[2]={1,1}, tc01[2]={0,1};
        glBegin(GL_QUADS);
        glTexCoord2iv(tc00); glVertex2f(x0,y0);
        glTexCoord2iv(tc10); glVertex2f(x1,y0);
        glTexCoord2iv(tc11); glVertex2f(x1,y1);
        glTexCoord2iv(tc01); glVertex2f(x0,y1);
        glEnd();
    }
    {
        float x0=TX[1],y0=TY[2],x1=x0+TCW,y1=y0+TCH;
        GLshort tc00[2]={0,0}, tc10[2]={1,0};
        GLshort tc11[2]={1,1}, tc01[2]={0,1};
        glBegin(GL_QUADS);
        glTexCoord2sv(tc00); glVertex2f(x0,y0);
        glTexCoord2sv(tc10); glVertex2f(x1,y0);
        glTexCoord2sv(tc11); glVertex2f(x1,y1);
        glTexCoord2sv(tc01); glVertex2f(x0,y1);
        glEnd();
    }
    {
        // Sanity: plain glTexCoord2d, should match all others
        float x0=TX[2],y0=TY[2],x1=x0+TCW,y1=y0+TCH;
        glBegin(GL_QUADS);
        glTexCoord2d(0.0,0.0); glVertex2f(x0,y0);
        glTexCoord2d(1.0,0.0); glVertex2f(x1,y0);
        glTexCoord2d(1.0,1.0); glVertex2f(x1,y1);
        glTexCoord2d(0.0,1.0); glVertex2f(x0,y1);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    // Column labels for texcoord grid
    static const char* colLabels[3] = { "1D/2D", "FV", "IV/SV" };
    static const char* rowLabels[3] = { "1D 2D 2DV", "1FV 3FV 4FV", "2IV 2SV 2D" };
    for (int c = 0; c < 3; c++) {
        glPushMatrix();
        glTranslatef(TX[c]+TCW*0.5f, TY[2]+TCH+8.0f, 0.0f);
        DrawStringCentered(colLabels[c], 8.0f);
        glPopMatrix();
    }
    for (int r = 0; r < 3; r++) {
        glPushMatrix();
        glTranslatef(TX[0]+TCW*1.5f, TY[r]+TCH*0.5f, 0.0f);
        DrawStringCentered(rowLabels[r], 8.0f);
        glPopMatrix();
    }

    // Divider
    glColor3f(0.5f,0.5f,0.5f);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();
}

// ---------------------------------------------------------------------------
// Test65 -- glRectsv and glRectdv
//
// Draws a 3x2 grid of coloured rects, each using a different variant.
// Should look identical to the existing Test54 rect tests.
//
// Row 0: glRectf (reference), glRecti (reference), glRects (reference)
// Row 1: glRectd (reference), glRectsv (NEW),       glRectdv (NEW)
// ---------------------------------------------------------------------------
static void Test65_RectVectors()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    // 3 cols x 2 rows, CW=180, CH=190, gap=10
    // Col X: 20, 220, 420   Row Y: 30, 250
    const float CW=180.0f, CH=190.0f;
    const float CX[3]={20,220,420};
    const float CY[2]={30,250};
    static const GLfloat col[6][3] = {
        {1.0f,0.2f,0.2f}, {0.2f,1.0f,0.2f}, {0.2f,0.4f,1.0f},
        {1.0f,1.0f,0.2f}, {1.0f,0.2f,1.0f}, {0.2f,1.0f,1.0f}
    };
    static const char* labels[6] = {
        "RECTF", "RECTI", "RECTS", "RECTD", "RECTSV", "RECTDV"
    };

    int idx = 0;
    for (int r = 0; r < 2; r++)
    for (int c = 0; c < 3; c++, idx++)
    {
        float x0=CX[c], y0=CY[r], x1=x0+CW, y1=y0+CH;
        glColor3f(col[idx][0], col[idx][1], col[idx][2]);
        switch (idx)
        {
        case 0: glRectf(x0, y0, x1, y1);                           break;
        case 1: glRecti((GLint)x0,(GLint)y0,(GLint)x1,(GLint)y1);  break;
        case 2: glRects((GLshort)x0,(GLshort)y0,(GLshort)x1,(GLshort)y1); break;
        case 3: glRectd((GLdouble)x0,(GLdouble)y0,(GLdouble)x1,(GLdouble)y1); break;
        case 4: // glRectsv -- NEW
            {
                GLshort v1[2]={(GLshort)x0,(GLshort)y0};
                GLshort v2[2]={(GLshort)x1,(GLshort)y1};
                glRectsv(v1, v2);
            } break;
        case 5: // glRectdv -- NEW
            {
                GLdouble v1[2]={(GLdouble)x0,(GLdouble)y0};
                GLdouble v2[2]={(GLdouble)x1,(GLdouble)y1};
                glRectdv(v1, v2);
            } break;
        }

        glColor3f(0.8f, 0.8f, 0.8f);
        glPushMatrix();
        glTranslatef(x0+CW*0.5f, y0+8.0f, 0.0f);
        DrawStringCentered(labels[idx], 9.0f);
        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// Test66 -- glInterleavedArrays: GL_T4F_V4F and GL_T4F_C4F_N3F_V4F
//
// For each format:
//   LEFT half:  explicit gl*Pointer calls (reference)
//   RIGHT half: glInterleavedArrays (must look identical)
//
// Row 0: GL_T4F_V4F  -- 4D tex coords + 4D homogeneous vertex
//   Draws a textured quad. 4D homogeneous: w=1 so same as 3D.
//
// Row 1: GL_T4F_C4F_N3F_V4F -- lit + coloured + textured + 4D vertex
//   Draws a lit textured quad with colour.
//
// A white divider line separates left (explicit) from right (interleaved).
// ---------------------------------------------------------------------------
static void Test66_InterleavedArraysT4F()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    SetGL2D(640.0f, 480.0f);

    const float LX=10, RX=330, VW=290, VH=200;
    const float RY[2]={40, 250};
    const float F = sizeof(GLfloat);

    // -----------------------------------------------------------------------
    // Row 0: GL_T4F_V4F
    // Layout: T4f (s,t,r,q) V4f (x,y,z,w)  -- 8 floats per vertex
    // r and q are ignored by our implementation (same as 3fv/4fv texcoords)
    // -----------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDisable(GL_LIGHTING);

    {
        // T4f V4f: stride = 8 floats = 32 bytes
        // Fields: s t r q  x y z w
        struct T4F_V4F { GLfloat s,t,r,q, x,y,z,w; };

        static const T4F_V4F vL[6] = {
            {0,0,0,1, LX,      RY[0],   0,1},
            {1,0,0,1, LX+VW,   RY[0],   0,1},
            {1,1,0,1, LX+VW,   RY[0]+VH,0,1},
            {0,0,0,1, LX,      RY[0],   0,1},
            {1,1,0,1, LX+VW,   RY[0]+VH,0,1},
            {0,1,0,1, LX,      RY[0]+VH,0,1},
        };
        // Left: explicit
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glTexCoordPointer(4, GL_FLOAT, sizeof(T4F_V4F), &vL[0].s);
        glVertexPointer  (4, GL_FLOAT, sizeof(T4F_V4F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const T4F_V4F vR[6] = {
            {0,0,0,1, RX,      RY[0],   0,1},
            {1,0,0,1, RX+VW,   RY[0],   0,1},
            {1,1,0,1, RX+VW,   RY[0]+VH,0,1},
            {0,0,0,1, RX,      RY[0],   0,1},
            {1,1,0,1, RX+VW,   RY[0]+VH,0,1},
            {0,1,0,1, RX,      RY[0]+VH,0,1},
        };
        // Right: glInterleavedArrays
        glInterleavedArrays(GL_T4F_V4F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // -----------------------------------------------------------------------
    // Row 1: GL_T4F_C4F_N3F_V4F
    // Layout: T4f C4f N3f V4f -- 15 floats = 60 bytes
    // Fields: s t r q  cr cg cb ca  nx ny nz  x y z w
    // -----------------------------------------------------------------------
    {
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
        GLfloat ldir[4]={0,0,1,0}, ldiff[4]={1,1,1,1}, lamb[4]={0.1f,0.1f,0.1f,1};
        glLightfv(GL_LIGHT0, GL_POSITION, ldir);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldiff);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_NORMALIZE);

        struct T4F_C4F_N3F_V4F { GLfloat s,t,r,q, cr,cg,cb,ca, nx,ny,nz, x,y,z,w; };

        static const T4F_C4F_N3F_V4F vL[6] = {
            {0,0,0,1, 1,0.5f,0,1, 0,0,1, LX,      RY[1],   0,1},
            {1,0,0,1, 1,0.5f,0,1, 0,0,1, LX+VW,   RY[1],   0,1},
            {1,1,0,1, 0,0.5f,1,1, 0,0,1, LX+VW,   RY[1]+VH,0,1},
            {0,0,0,1, 1,0.5f,0,1, 0,0,1, LX,      RY[1],   0,1},
            {1,1,0,1, 0,0.5f,1,1, 0,0,1, LX+VW,   RY[1]+VH,0,1},
            {0,1,0,1, 0,0.5f,1,1, 0,0,1, LX,      RY[1]+VH,0,1},
        };
        // Left: explicit
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glTexCoordPointer(4, GL_FLOAT, sizeof(T4F_C4F_N3F_V4F), &vL[0].s);
        glColorPointer   (4, GL_FLOAT, sizeof(T4F_C4F_N3F_V4F), &vL[0].cr);
        glNormalPointer  (   GL_FLOAT, sizeof(T4F_C4F_N3F_V4F), &vL[0].nx);
        glVertexPointer  (4, GL_FLOAT, sizeof(T4F_C4F_N3F_V4F), &vL[0].x);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        static const T4F_C4F_N3F_V4F vR[6] = {
            {0,0,0,1, 1,0.5f,0,1, 0,0,1, RX,      RY[1],   0,1},
            {1,0,0,1, 1,0.5f,0,1, 0,0,1, RX+VW,   RY[1],   0,1},
            {1,1,0,1, 0,0.5f,1,1, 0,0,1, RX+VW,   RY[1]+VH,0,1},
            {0,0,0,1, 1,0.5f,0,1, 0,0,1, RX,      RY[1],   0,1},
            {1,1,0,1, 0,0.5f,1,1, 0,0,1, RX+VW,   RY[1]+VH,0,1},
            {0,1,0,1, 0,0.5f,1,1, 0,0,1, RX,      RY[1]+VH,0,1},
        };
        // Right: glInterleavedArrays
        glInterleavedArrays(GL_T4F_C4F_N3F_V4F, 0, vR);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE); glDisable(GL_COLOR_MATERIAL);
    }

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Row labels
    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(160.0f, 22.0f, 0.0f);
    DrawStringCentered("T4F_V4F", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(160.0f, 232.0f, 0.0f);
    DrawStringCentered("T4F_C4F_N3F_V4F", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(160.0f, 14.0f, 0.0f);
    DrawStringCentered("EXPLICIT (REF)", 8.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(480.0f, 14.0f, 0.0f);
    DrawStringCentered("INTERLEAVED", 8.0f); glPopMatrix();

    // Divider
    glColor3f(1,1,1);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();
}

// ---------------------------------------------------------------------------
// Test67 -- Multi-light: GL_LIGHT4-7 and spotlights
//
// TOP ROW: 4 spheres lit by GL_LIGHT4, 5, 6, 7 respectively (one each).
//   Each light has a distinct colour so it's obvious which is active.
//   LIGHT4=red, LIGHT5=green, LIGHT6=blue, LIGHT7=yellow
//
// BOTTOM ROW: Spotlight test.
//   Two spheres side by side. One white spotlight aimed at each.
//   GL_SPOT_CUTOFF=20deg, GL_SPOT_DIRECTION aimed at target sphere.
//   The targeted sphere should be brightly lit; the other nearly dark.
// ---------------------------------------------------------------------------
static void Test67_MultiLightFull()
{
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    GLfloat white[4]={1,1,1,1}, black[4]={0,0,0,1};
    GLfloat matAmb[4]={0.05f,0.05f,0.05f,1};
    glMaterialfv(GL_FRONT, GL_AMBIENT,  matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf (GL_FRONT, GL_SHININESS, 0);
    GLfloat sceneAmb[4]={0.02f,0.02f,0.02f,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmb);

    // Disable all lights first
    for (int i = 0; i < 8; i++) glDisable((GLenum)(GL_LIGHT0+i));

    SetGLPerspective(55.0f, 640.0f/480.0f, 0.1f, 50.0f);

    // -----------------------------------------------------------------------
    // TOP ROW: GL_LIGHT4-7 each on a separate sphere
    // -----------------------------------------------------------------------
    static const GLfloat lightCols[4][4] = {
        {1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,1,0,1}
    };
    static const float sphereX[4] = {-4.5f,-1.5f,1.5f,4.5f};
    GLfloat noAmb[4]={0,0,0,1};

    for (int i = 0; i < 4; i++)
    {
        GLenum light = (GLenum)(GL_LIGHT4 + i);
        glLightfv(light, GL_DIFFUSE,  lightCols[i]);
        glLightfv(light, GL_AMBIENT,  noAmb);
        glLightfv(light, GL_SPECULAR, noAmb);
        GLfloat lpos[4]={0,3,4,0}; // directional from upper-front
        glLightfv(light, GL_POSITION, lpos);
        glEnable(light);

        glPushMatrix();
        glTranslatef(sphereX[i], 1.4f, -8.0f);
        DrawTestSphere(0,0,0, 0.9f, 14, 20);
        glPopMatrix();

        glDisable(light);
    }

    // -----------------------------------------------------------------------
    // BOTTOM ROW: Spotlights using GL_SPOT_CUTOFF / DIRECTION / EXPONENT
    // Two spheres. Spotlight 0 targets left sphere, Spotlight 1 targets right.
    // -----------------------------------------------------------------------
    float leftX  = -2.0f;
    float rightX =  2.0f;
    float sphereY = -1.5f;
    float sphereZ = -7.0f;

    // Spotlight 0: white, aimed at left sphere
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  white);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  noAmb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, noAmb);
    {
        // Position spotlight above and in front of left sphere
        GLfloat lpos[4] = {leftX, sphereY+3.0f, sphereZ+3.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lpos);
        // Direction: toward the left sphere centre
        GLfloat ldir[3] = {0.0f, -3.0f, -3.0f}; // roughly down and back
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, ldir);
        glLightf (GL_LIGHT0, GL_SPOT_CUTOFF,    20.0f);
        glLightf (GL_LIGHT0, GL_SPOT_EXPONENT,  4.0f);
        glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.0f);
        glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
    glEnable(GL_LIGHT0);

    // Spotlight 1: white, aimed at right sphere
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  white);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  noAmb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, noAmb);
    {
        GLfloat lpos[4] = {rightX, sphereY+3.0f, sphereZ+3.0f, 1.0f};
        glLightfv(GL_LIGHT1, GL_POSITION, lpos);
        GLfloat ldir[3] = {0.0f, -3.0f, -3.0f};
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, ldir);
        glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,    20.0f);
        glLightf (GL_LIGHT1, GL_SPOT_EXPONENT,  4.0f);
        glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.0f);
        glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
    glEnable(GL_LIGHT1);

    glPushMatrix();
    glTranslatef(leftX, sphereY, sphereZ);
    DrawTestSphere(0,0,0, 0.9f, 14, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightX, sphereY, sphereZ);
    DrawTestSphere(0,0,0, 0.9f, 14, 20);
    glPopMatrix();

    // Restore
    for (int i = 0; i < 8; i++) glDisable((GLenum)(GL_LIGHT0+i));
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Reset spotlight state back to defaults
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,   180.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,   0.0f);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,   180.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,   0.0f);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);

    // Labels
    SetGL2D(640.0f, 480.0f);
    glColor3f(0.8f,0.8f,0.8f);
    static const char* lightNames[4] = {"LIGHT4","LIGHT5","LIGHT6","LIGHT7"};
    const float labelX[4] = {80,213,347,480};
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(labelX[i], 260.0f, 0.0f);
        DrawStringCentered(lightNames[i], 9.0f);
        glPopMatrix();
    }
    glPushMatrix(); glTranslatef(160.0f, 10.0f, 0.0f);
    DrawStringCentered("SPOT L0->LEFT", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(480.0f, 10.0f, 0.0f);
    DrawStringCentered("SPOT L1->RIGHT", 9.0f); glPopMatrix();
}

// ---------------------------------------------------------------------------
// Test68 -- Four simultaneous clip planes (GL_CLIP_PLANE2-5)
//
// Draws a large white quad and clips it with 4 planes forming a diamond:
//   PLANE2: x + y >= 0   (keep upper-right)
//   PLANE3: -x + y >= 0  (keep upper-left)
//   PLANE4: x - y >= -H  (keep lower-right, i.e. y <= x + H)
//   PLANE5: -x - y >= -H (keep lower-left,  i.e. y <= -x + H)
//
// The result should be a diamond (rotated square) in the centre of the screen.
// Left half uses PLANE2+3, right half uses PLANE4+5 to show all 4 planes work.
// Centre: all 4 active, showing the full diamond intersection.
// ---------------------------------------------------------------------------
static void Test68_ClipPlaneMulti()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);

    // Make sure all clip planes off to start
    for (int i = 0; i < 6; i++) glDisable((GLenum)(GL_CLIP_PLANE0+i));

    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Work in a coordinate system centred at (320, 240)
    // Diamond half-width H = 180
    float cx = 320.0f, cy = 240.0f, H = 180.0f;

    // Diamond: keep |x-cx| + |y-cy| <= H (Manhattan distance from centre)
    // Each side clips away one corner. Inside = dot >= 0.
    // PLANE2 (upper-right side): -(x+y) + (cx+cy+H) >= 0
    // PLANE3 (upper-left side):  (x-y)  + (-cx+cy+H) >= 0
    // PLANE4 (lower-right side): (-x+y) + (cx-cy+H) >= 0
    // PLANE5 (lower-left side):  (x+y)  - (cx+cy-H) >= 0

    GLdouble plane2[4] = {-1.0,-1.0, 0.0, (double)(cx+cy+H) };
    GLdouble plane3[4] = { 1.0,-1.0, 0.0, (double)(-cx+cy+H) };
    GLdouble plane4[4] = {-1.0, 1.0, 0.0, (double)(cx-cy+H) };
    GLdouble plane5[4] = { 1.0, 1.0, 0.0, (double)(-(cx+cy-H)) };

    // --- Section 1: LEFT THIRD -- PLANE2 + PLANE3 only (top diamond half) ---
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, 213, 480);
    glClipPlane(GL_CLIP_PLANE2, plane2); glEnable(GL_CLIP_PLANE2);
    glClipPlane(GL_CLIP_PLANE3, plane3); glEnable(GL_CLIP_PLANE3);

    glColor3f(0.9f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0,0); glVertex2f(640,0); glVertex2f(640,480); glVertex2f(0,480);
    glEnd();
    glFlush();

    glDisable(GL_CLIP_PLANE2); glDisable(GL_CLIP_PLANE3);

    // --- Section 2: RIGHT THIRD -- PLANE4 + PLANE5 only (bottom diamond half) ---
    glScissor(427, 0, 213, 480);
    glClipPlane(GL_CLIP_PLANE4, plane4); glEnable(GL_CLIP_PLANE4);
    glClipPlane(GL_CLIP_PLANE5, plane5); glEnable(GL_CLIP_PLANE5);

    glColor3f(0.3f, 0.9f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(0,0); glVertex2f(640,0); glVertex2f(640,480); glVertex2f(0,480);
    glEnd();
    glFlush();

    glDisable(GL_CLIP_PLANE4); glDisable(GL_CLIP_PLANE5);

    // --- Section 3: CENTRE THIRD -- all 4 planes (full diamond intersection) ---
    glScissor(213, 0, 214, 480);
    glClipPlane(GL_CLIP_PLANE2, plane2); glEnable(GL_CLIP_PLANE2);
    glClipPlane(GL_CLIP_PLANE3, plane3); glEnable(GL_CLIP_PLANE3);
    glClipPlane(GL_CLIP_PLANE4, plane4); glEnable(GL_CLIP_PLANE4);
    glClipPlane(GL_CLIP_PLANE5, plane5); glEnable(GL_CLIP_PLANE5);

    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0,0); glVertex2f(640,0); glVertex2f(640,480); glVertex2f(0,480);
    glEnd();
    glFlush();

    for (int i = 2; i < 6; i++) glDisable((GLenum)(GL_CLIP_PLANE0+i));
    glDisable(GL_SCISSOR_TEST);

    // Labels
    glViewport(0, 0, 640, 480);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(107.0f, 12.0f, 0.0f);
    DrawStringCentered("PLANE2+3", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(320.0f, 12.0f, 0.0f);
    DrawStringCentered("ALL 4 PLANES", 9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(533.0f, 12.0f, 0.0f);
    DrawStringCentered("PLANE4+5", 9.0f); glPopMatrix();

    // Section dividers
    glColor3f(0.5f,0.5f,0.5f);
    glBegin(GL_LINES);
    glVertex2f(213,0); glVertex2f(213,480);
    glVertex2f(427,0); glVertex2f(427,480);
    glEnd();
}

static void Test69_ColorVariantsFull()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);

    // 4 cols x 4 rows. CW=145, CH=105, gap=10
    // Col X: 10,165,320,475   Row Y: 15,130,245,360
    const float CW=145.0f, CH=105.0f;
    const float CX[4]={10,165,320,475};
    const float CY[4]={15,130,245,360};

    // Target colours per column: red, green, blue, white
    // For signed types (b,s,i): max positive value = full channel
    // For unsigned types: max value = full channel

#define QUAD69(c,r) do { \
    glBegin(GL_QUADS); \
    glVertex2f(CX[c],CY[r]); glVertex2f(CX[c]+CW,CY[r]); \
    glVertex2f(CX[c]+CW,CY[r]+CH); glVertex2f(CX[c],CY[r]+CH); \
    glEnd(); } while(0)

    // Row 0: glColor3b, glColor3s, glColor3i, glColor3ui
    // col0=red, col1=green, col2=blue, col3=white
    glColor3b(127,0,0);                                    QUAD69(0,0);
    glColor3b(0,127,0);                                    QUAD69(1,0);
    glColor3b(0,0,127);                                    QUAD69(2,0);
    glColor3b(127,127,127);                                QUAD69(3,0);

    glColor3s(32767,0,0);                                  QUAD69(0,1);
    glColor3s(0,32767,0);                                  QUAD69(1,1);
    glColor3s(0,0,32767);                                  QUAD69(2,1);
    glColor3s(32767,32767,32767);                          QUAD69(3,1);

    glColor3i(2147483647,0,0);                             QUAD69(0,2);
    glColor3i(0,2147483647,0);                             QUAD69(1,2);
    glColor3i(0,0,2147483647);                             QUAD69(2,2);
    glColor3i(2147483647,2147483647,2147483647);           QUAD69(3,2);

    glColor3ui(4294967295u,0,0);                           QUAD69(0,3);
    glColor3ui(0,4294967295u,0);                           QUAD69(1,3);
    glColor3ui(0,0,4294967295u);                           QUAD69(2,3);
    glColor3ui(4294967295u,4294967295u,4294967295u);       QUAD69(3,3);

#undef QUAD69

    // Labels
    glColor3f(0.85f,0.85f,0.85f);
    static const char* rowL[4] = {"3B/3S/3I/3UI","3FV/3BV/3SV/3UBV","4B/4S/4I/4UI","4FV/4BV/4SV/4D"};
    static const char* colL[4] = {"RED","GREEN","BLUE","WHITE"};
    for (int r=0;r<4;r++) {
        glPushMatrix(); glTranslatef(CX[0]+CW*0.5f, CY[r]+CH-13.0f, 0);
        DrawStringCentered(rowL[r], 8.0f); glPopMatrix();
    }
    for (int c=0;c<4;c++) {
        glPushMatrix(); glTranslatef(CX[c]+CW*0.5f, CY[3]+CH+6.0f, 0);
        DrawStringCentered(colL[c], 9.0f); glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// Test70 -- glNormal3 vector and double variants
//
// Draws 5 lit spheres side by side, each using a different normal variant.
// All should look identical since normals are just (nx,ny,nz) at each vertex.
//
// Sphere 0: glNormal3fv  (reference -- already tested in Test53)
// Sphere 1: glNormal3bv
// Sphere 2: glNormal3sv
// Sphere 3: glNormal3iv
// Sphere 4: glNormal3d  (scalar double)
// ---------------------------------------------------------------------------
static void DrawLitSphere_NormalVariant(int mode, float cx, float cy, float cz, float r)
{
    int latSegs=12, lonSegs=18;
    float latStep = PI_F / latSegs;
    float lonStep = 2.0f * PI_F / lonSegs;
    for (int lat=0; lat<latSegs; lat++)
    {
        float phi0 = -PI_F/2.0f + lat*latStep;
        float phi1 = phi0 + latStep;
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon=0; lon<=lonSegs; lon++)
        {
            float theta = lon * lonStep;
            float ct = cosf_approx(theta), st = sinf_approx(theta);
            for (int pass=0; pass<2; pass++)
            {
                float phi = (pass==0) ? phi0 : phi1;
                float nx = cosf_approx(phi)*ct;
                float ny = sinf_approx(phi);
                float nz = cosf_approx(phi)*st;
                // Clamp to [-1,1]
                float cnx = nx<-1.f?-1.f:(nx>1.f?1.f:nx);
                float cny = ny<-1.f?-1.f:(ny>1.f?1.f:ny);
                float cnz = nz<-1.f?-1.f:(nz>1.f?1.f:nz);
                switch (mode)
                {
                case 0: { GLfloat v[3]={nx,ny,nz}; glNormal3fv(v); } break;
                case 1: { GLbyte  v[3]={(GLbyte)(cnx*127),(GLbyte)(cny*127),(GLbyte)(cnz*127)}; glNormal3bv(v); } break;
                case 2: { GLshort v[3]={(GLshort)(cnx*32767),(GLshort)(cny*32767),(GLshort)(cnz*32767)}; glNormal3sv(v); } break;
                case 3: { GLint   v[3]={(GLint)(cnx*2147483520.0f),(GLint)(cny*2147483520.0f),(GLint)(cnz*2147483520.0f)}; glNormal3iv(v); } break;
                case 4: glNormal3d((GLdouble)nx,(GLdouble)ny,(GLdouble)nz); break;
                }
                glVertex3f(cx+r*nx, cy+r*ny, cz+r*nz);
            }
        }
        glEnd();
    }
}

static void Test70_NormalVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);
    SetGLPerspective(55.0f, 640.0f/480.0f, 0.1f, 50.0f);
    glTranslatef(0, 0, -8.0f);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    GLfloat lpos[4]={2,3,4,1}, ldiff[4]={1,1,0.9f,1}, lamb[4]={0.08f,0.08f,0.1f,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);
    GLfloat matD[4]={0.7f,0.5f,0.2f,1}, matA[4]={0.15f,0.1f,0.05f,1}, matB[4]={0,0,0,1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  matD);
    glMaterialfv(GL_FRONT, GL_AMBIENT,  matA);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matB);
    glMaterialf (GL_FRONT, GL_SHININESS, 0);

    const float xs[5] = {-4.8f,-2.4f,0.0f,2.4f,4.8f};
    for (int i=0; i<5; i++)
        DrawLitSphere_NormalVariant(i, xs[i], 0, 0, 0.85f);

    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE); glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST);

    SetGL2D(640.0f, 480.0f);
    glColor3f(0.8f,0.8f,0.8f);
    static const char* labels[5] = {"3FV\n(REF)","3BV","3SV","3IV","3D"};
    const float lx[5] = {64,192,320,448,576};
    for (int i=0; i<5; i++) {
        glPushMatrix(); glTranslatef(lx[i], 18.0f, 0);
        DrawStringCentered(labels[i], 11.0f); glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// Test71 -- glTexCoord untested variants
//
// 4 textured quads using:
//   glTexCoord1f  -- sets s only (t=0), shown with g_rgbaTex
//   glTexCoord2fv -- 2D float vector
//   glTexCoord3f  -- 3D (r ignored), shown with checker
//   glTexCoord4f  -- 4D (r,q ignored), shown with checker
//
// All should show the correct texture sampling.
// ---------------------------------------------------------------------------
static void Test71_TexCoordVariants()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);

    // 4 cells, 2x2 grid. CW=280, CH=210, gap=10
    // Col X: 10, 330   Row Y: 30, 255
    const float CW=280.0f, CH=210.0f;
    const float CX[2]={10,330};
    const float CY[2]={30,255};

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);

    // Cell (0,0): glTexCoord1f -- s only, use g_rgbaTex
    // red at s=0, green at s=1 (t=0 samples bottom row)
    glBindTexture(GL_TEXTURE_2D, g_rgbaTex);
    {
        float x0=CX[0],y0=CY[0],x1=x0+CW,y1=y0+CH;
        glBegin(GL_QUADS);
        glTexCoord1f(0.0f); glVertex2f(x0,y0);
        glTexCoord1f(1.0f); glVertex2f(x1,y0);
        glTexCoord1f(1.0f); glVertex2f(x1,y1);
        glTexCoord1f(0.0f); glVertex2f(x0,y1);
        glEnd();
    }

    // Cell (1,0): glTexCoord2fv -- standard 2D vector
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    {
        float x0=CX[1],y0=CY[0],x1=x0+CW,y1=y0+CH;
        GLfloat tc00[2]={0,0}, tc10[2]={1,0}, tc11[2]={1,1}, tc01[2]={0,1};
        glBegin(GL_QUADS);
        glTexCoord2fv(tc00); glVertex2f(x0,y0);
        glTexCoord2fv(tc10); glVertex2f(x1,y0);
        glTexCoord2fv(tc11); glVertex2f(x1,y1);
        glTexCoord2fv(tc01); glVertex2f(x0,y1);
        glEnd();
    }

    // Cell (0,1): glTexCoord3f -- r component ignored
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    {
        float x0=CX[0],y0=CY[1],x1=x0+CW,y1=y0+CH;
        glBegin(GL_QUADS);
        glTexCoord3f(0,0,99); glVertex2f(x0,y0);
        glTexCoord3f(1,0,99); glVertex2f(x1,y0);
        glTexCoord3f(1,1,99); glVertex2f(x1,y1);
        glTexCoord3f(0,1,99); glVertex2f(x0,y1);
        glEnd();
    }

    // Cell (1,1): glTexCoord4f -- r,q ignored
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    {
        float x0=CX[1],y0=CY[1],x1=x0+CW,y1=y0+CH;
        glBegin(GL_QUADS);
        glTexCoord4f(0,0,99,1); glVertex2f(x0,y0);
        glTexCoord4f(1,0,99,1); glVertex2f(x1,y0);
        glTexCoord4f(1,1,99,1); glVertex2f(x1,y1);
        glTexCoord4f(0,1,99,1); glVertex2f(x0,y1);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(0.85f,0.85f,0.85f);
    static const char* cellLabels[4] = {
        "TEXCOORD1F (S ONLY)", "TEXCOORD2FV",
        "TEXCOORD3F (R=99 IGNORED)", "TEXCOORD4F (R,Q=99 IGNORED)"
    };
    int idx=0;
    for (int r=0;r<2;r++) for (int c=0;c<2;c++,idx++) {
        glPushMatrix();
        glTranslatef(CX[c]+CW*0.5f, CY[r]+12.0f, 0);
        DrawStringCentered(cellLabels[idx], 10.0f);
        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// Test72 -- glInterleavedArrays: remaining 6 formats
//
// Each row: LEFT = explicit gl*Pointer (reference), RIGHT = glInterleavedArrays
//
// Row 0: GL_V2F           -- 2D vertex only
// Row 1: GL_C4UB_V2F      -- colour + 2D vertex
// Row 2: GL_C3F_V3F       -- float colour + 3D vertex
// Row 3: GL_C4F_N3F_V3F   -- float colour + normal + 3D vertex (lit)
// Row 4: GL_T2F_V3F       -- texcoord + 3D vertex
// Row 5: GL_T2F_C3F_V3F   -- texcoord + float colour + 3D vertex
// ---------------------------------------------------------------------------
static void Test72_InterleavedArraysRemaining()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);

    const float LX=10, RX=330, VW=290, VH=68;
    const float RY[6]={8,84,160,236,312,388};

    // --- Row 0: GL_V2F ---
    {
        struct V2F { GLfloat x,y; };
        glColor3f(0.8f,0.2f,0.2f);
        static const V2F vL[6]={
            {LX,RY[0]},{LX+VW,RY[0]},{LX+VW,RY[0]+VH},
            {LX,RY[0]},{LX+VW,RY[0]+VH},{LX,RY[0]+VH}
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2,GL_FLOAT,0,vL);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const V2F vR[6]={
            {RX,RY[0]},{RX+VW,RY[0]},{RX+VW,RY[0]+VH},
            {RX,RY[0]},{RX+VW,RY[0]+VH},{RX,RY[0]+VH}
        };
        glInterleavedArrays(GL_V2F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);
    }

    // --- Row 1: GL_C4UB_V2F ---
    {
        struct C4UB_V2F { GLubyte r,g,b,a; GLfloat x,y; };
        static const C4UB_V2F vL[6]={
            {255,200,0,255,LX,RY[1]},   {255,200,0,255,LX+VW,RY[1]},
            {0,200,255,255,LX+VW,RY[1]+VH},
            {255,200,0,255,LX,RY[1]},   {0,200,255,255,LX+VW,RY[1]+VH},
            {0,200,255,255,LX,RY[1]+VH}
        };
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2,GL_FLOAT,sizeof(C4UB_V2F),&vL[0].x);
        glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(C4UB_V2F),&vL[0].r);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const C4UB_V2F vR[6]={
            {255,200,0,255,RX,RY[1]},   {255,200,0,255,RX+VW,RY[1]},
            {0,200,255,255,RX+VW,RY[1]+VH},
            {255,200,0,255,RX,RY[1]},   {0,200,255,255,RX+VW,RY[1]+VH},
            {0,200,255,255,RX,RY[1]+VH}
        };
        glInterleavedArrays(GL_C4UB_V2F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);
    }

    // --- Row 2: GL_C3F_V3F ---
    {
        struct C3F_V3F { GLfloat r,g,b,x,y,z; };
        static const C3F_V3F vL[6]={
            {1,0.5f,0, LX,RY[2],0},      {1,0.5f,0, LX+VW,RY[2],0},
            {0,0.5f,1, LX+VW,RY[2]+VH,0},
            {1,0.5f,0, LX,RY[2],0},      {0,0.5f,1, LX+VW,RY[2]+VH,0},
            {0,0.5f,1, LX,RY[2]+VH,0}
        };
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3,GL_FLOAT,sizeof(C3F_V3F),&vL[0].x);
        glColorPointer(3,GL_FLOAT,sizeof(C3F_V3F),&vL[0].r);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const C3F_V3F vR[6]={
            {1,0.5f,0, RX,RY[2],0},      {1,0.5f,0, RX+VW,RY[2],0},
            {0,0.5f,1, RX+VW,RY[2]+VH,0},
            {1,0.5f,0, RX,RY[2],0},      {0,0.5f,1, RX+VW,RY[2]+VH,0},
            {0,0.5f,1, RX,RY[2]+VH,0}
        };
        glInterleavedArrays(GL_C3F_V3F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);
    }

    // --- Row 3: GL_C4F_N3F_V3F (lit) ---
    {
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
        GLfloat ldir[4]={0,0,1,0}, ldiff[4]={1,1,1,1}, lamb[4]={0.15f,0.15f,0.15f,1};
        glLightfv(GL_LIGHT0,GL_POSITION,ldir);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,ldiff);
        glLightfv(GL_LIGHT0,GL_AMBIENT,lamb);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_NORMALIZE);

        struct C4F_N3F_V3F { GLfloat r,g,b,a,nx,ny,nz,x,y,z; };
        static const C4F_N3F_V3F vL[6]={
            {0.2f,0.8f,0.2f,1, 0,0,1, LX,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, LX+VW,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, LX+VW,RY[3]+VH,0},
            {0.2f,0.8f,0.2f,1, 0,0,1, LX,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, LX+VW,RY[3]+VH,0},
            {0.2f,0.8f,0.2f,1, 0,0,1, LX,RY[3]+VH,0},
        };
        glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3,GL_FLOAT,sizeof(C4F_N3F_V3F),&vL[0].x);
        glColorPointer(4,GL_FLOAT,sizeof(C4F_N3F_V3F),&vL[0].r);
        glNormalPointer(GL_FLOAT,sizeof(C4F_N3F_V3F),&vL[0].nx);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const C4F_N3F_V3F vR[6]={
            {0.2f,0.8f,0.2f,1, 0,0,1, RX,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, RX+VW,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, RX+VW,RY[3]+VH,0},
            {0.2f,0.8f,0.2f,1, 0,0,1, RX,RY[3],0},
            {0.2f,0.8f,0.2f,1, 0,0,1, RX+VW,RY[3]+VH,0},
            {0.2f,0.8f,0.2f,1, 0,0,1, RX,RY[3]+VH,0},
        };
        glInterleavedArrays(GL_C4F_N3F_V3F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);

        glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE); glDisable(GL_COLOR_MATERIAL);
    }

    // --- Row 4: GL_T2F_V3F ---
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        glDisable(GL_LIGHTING);
        struct T2F_V3F { GLfloat s,t,x,y,z; };
        static const T2F_V3F vL[6]={
            {0,0,LX,RY[4],0},{1,0,LX+VW,RY[4],0},{1,1,LX+VW,RY[4]+VH,0},
            {0,0,LX,RY[4],0},{1,1,LX+VW,RY[4]+VH,0},{0,1,LX,RY[4]+VH,0}
        };
        glEnableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2,GL_FLOAT,sizeof(T2F_V3F),&vL[0].s);
        glVertexPointer(3,GL_FLOAT,sizeof(T2F_V3F),&vL[0].x);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const T2F_V3F vR[6]={
            {0,0,RX,RY[4],0},{1,0,RX+VW,RY[4],0},{1,1,RX+VW,RY[4]+VH,0},
            {0,0,RX,RY[4],0},{1,1,RX+VW,RY[4]+VH,0},{0,1,RX,RY[4]+VH,0}
        };
        glInterleavedArrays(GL_T2F_V3F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);
        glDisable(GL_TEXTURE_2D);
    }

    // --- Row 5: GL_T2F_C3F_V3F ---
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glDisable(GL_LIGHTING);
        struct T2F_C3F_V3F { GLfloat s,t,r,g,b,x,y,z; };
        static const T2F_C3F_V3F vL[6]={
            {0,0,1,0.5f,0, LX,RY[5],0},    {1,0,1,0.5f,0, LX+VW,RY[5],0},
            {1,1,0,0.5f,1, LX+VW,RY[5]+VH,0},
            {0,0,1,0.5f,0, LX,RY[5],0},    {1,1,0,0.5f,1, LX+VW,RY[5]+VH,0},
            {0,1,0,0.5f,1, LX,RY[5]+VH,0}
        };
        glEnableClientState(GL_VERTEX_ARRAY); glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2,GL_FLOAT,sizeof(T2F_C3F_V3F),&vL[0].s);
        glColorPointer(3,GL_FLOAT,sizeof(T2F_C3F_V3F),&vL[0].r);
        glVertexPointer(3,GL_FLOAT,sizeof(T2F_C3F_V3F),&vL[0].x);
        glDrawArrays(GL_TRIANGLES,0,6);

        static const T2F_C3F_V3F vR[6]={
            {0,0,1,0.5f,0, RX,RY[5],0},    {1,0,1,0.5f,0, RX+VW,RY[5],0},
            {1,1,0,0.5f,1, RX+VW,RY[5]+VH,0},
            {0,0,1,0.5f,0, RX,RY[5],0},    {1,1,0,0.5f,1, RX+VW,RY[5]+VH,0},
            {0,1,0,0.5f,1, RX,RY[5]+VH,0}
        };
        glInterleavedArrays(GL_T2F_C3F_V3F,0,vR);
        glDrawArrays(GL_TRIANGLES,0,6);
        glDisable(GL_TEXTURE_2D);
    }

    glDisableClientState(GL_VERTEX_ARRAY); glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY); glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    static const char* rowLabels[6] = {
        "V2F","C4UB_V2F","C3F_V3F","C4F_N3F_V3F","T2F_V3F","T2F_C3F_V3F"
    };
    glColor3f(0.8f,0.8f,0.8f);
    for (int i=0;i<6;i++) {
        glPushMatrix(); glTranslatef(LX+VW*0.5f, RY[i]+VH-10.0f, 0);
        DrawStringCentered(rowLabels[i], 9.0f); glPopMatrix();
    }
    glColor3f(0.5f,0.5f,0.5f);
    glBegin(GL_LINES); glVertex2f(320,0); glVertex2f(320,480); glEnd();
}

// ---------------------------------------------------------------------------
// Test73 -- GL_SUBTRACT combine op, GL_DOT3_RGBA, glCopyTexImage2D
//
// LEFT third:  GL_SUBTRACT -- stage1 - stage0 (clamped). Shows dark result
//              where checker is white (subtraction makes it dark).
//
// CENTRE third: GL_DOT3_RGBA -- dot product stored in RGBA (same as DOT3_RGB
//              but alpha also gets dot product value). Shows same as DOT3_RGB.
//
// RIGHT third: glCopyTexImage2D -- copy current framebuffer into a new texture,
//              then display that texture. Should show a copy of what was drawn.
// ---------------------------------------------------------------------------
static void Test73_SubtractDot3CopyTex()
{
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    const float W=190.0f, H=380.0f, Y0=50.0f;
    const float X[3]={10.0f, 225.0f, 440.0f};

    // -----------------------------------------------------------------------
    // LEFT: GL_SUBTRACT
    // Stage0 = checker texture, Stage1 = solid white constant
    // Result = white - checker: dark where checker is bright, bright where dark
    // -----------------------------------------------------------------------
    glEnable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,    GL_SUBTRACT);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,    GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,    GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,   GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,   GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,  GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,  GL_TEXTURE);
    GLfloat white[4]={1,1,1,1};
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(X[0],Y0);
    glTexCoord2f(2,0); glVertex2f(X[0]+W,Y0);
    glTexCoord2f(2,2); glVertex2f(X[0]+W,Y0+H);
    glTexCoord2f(0,2); glVertex2f(X[0],Y0+H);
    glEnd();

    // Restore
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

    // -----------------------------------------------------------------------
    // CENTRE: GL_DOT3_RGBA
    // Dot product of normal map with a light direction, result in RGB+A
    // -----------------------------------------------------------------------
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,   GL_DOT3_RGBA);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,   GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,   GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,  GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,  GL_SRC_COLOR);
    GLfloat lightEnc[4]={0.5f,0.5f,1.0f,1.0f}; // encoded light dir (0,0,1)
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, lightEnc);

    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(X[1],Y0);
    glTexCoord2f(2,0); glVertex2f(X[1]+W,Y0);
    glTexCoord2f(2,2); glVertex2f(X[1]+W,Y0+H);
    glTexCoord2f(0,2); glVertex2f(X[1],Y0+H);
    glEnd();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_2D);

    // -----------------------------------------------------------------------
    // RIGHT: glCopyTexImage2D
    // Draw a known 4-colour pattern, copy it into a 128x128 POT texture
    // using glCopyTexImage2D, then display the texture below the original.
    // Both panels must look identical, proving the function works correctly.
    // -----------------------------------------------------------------------
    static GLuint s_copyTex73 = 0;

    // Source pattern: 4 coloured quadrants in 128x128 pixel area
    float srcX = X[2];
    float srcY = Y0 + H * 0.5f;  // top half of right cell

    // Draw reference pattern directly to framebuffer
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3f(1,0,0);
    glVertex2f(srcX,    srcY);    glVertex2f(srcX+64, srcY);
    glVertex2f(srcX+64, srcY+64); glVertex2f(srcX,    srcY+64);
    glColor3f(0,1,0);
    glVertex2f(srcX+64,  srcY);    glVertex2f(srcX+128, srcY);
    glVertex2f(srcX+128, srcY+64); glVertex2f(srcX+64,  srcY+64);
    glColor3f(0,0,1);
    glVertex2f(srcX,    srcY+64);  glVertex2f(srcX+64, srcY+64);
    glVertex2f(srcX+64, srcY+128); glVertex2f(srcX,    srcY+128);
    glColor3f(1,1,0);
    glVertex2f(srcX+64,  srcY+64);  glVertex2f(srcX+128, srcY+64);
    glVertex2f(srcX+128, srcY+128); glVertex2f(srcX+64,  srcY+128);
    glEnd();
    glColor3f(1,1,1);
    glFlush();

    // Create texture once
    if (!s_copyTex73)
    {
        glGenTextures(1, &s_copyTex73);
        glBindTexture(GL_TEXTURE_2D, s_copyTex73);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    // glCopyTexImage2D: allocates storage AND copies 128x128 POT region
    glBindTexture(GL_TEXTURE_2D, s_copyTex73);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        (GLint)srcX, (GLint)srcY, 128, 128, 0);

    // Display captured texture in bottom half of right cell -- must match above
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1,1,1);
    float dstY = Y0;
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(srcX,     dstY);
    glTexCoord2f(1,0); glVertex2f(srcX+128, dstY);
    glTexCoord2f(1,1); glVertex2f(srcX+128, dstY+128);
    glTexCoord2f(0,1); glVertex2f(srcX,     dstY+128);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Labels
    glColor3f(0.85f,0.85f,0.85f);
    glPushMatrix(); glTranslatef(X[0]+W*0.5f, Y0-15.0f, 0);
    DrawStringCentered("GL_SUBTRACT", 11.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(X[1]+W*0.5f, Y0-15.0f, 0);
    DrawStringCentered("GL_DOT3_RGBA", 11.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(X[2]+W*0.5f, Y0-15.0f, 0);
    DrawStringCentered("COPYTEXIMAGE2D", 11.0f); glPopMatrix();
}
// ---------------------------------------------------------------------------
// Test74 -- glCopyPixels
//
// Proves glCopyPixels(GL_COLOR) works correctly.
//
// Layout:
//   LEFT third:  source -- 4 coloured quads drawn directly
//   CENTRE third: glCopyPixels result -- copies LEFT into CENTRE
//   RIGHT third:  reference -- same 4 quads drawn again manually
//
// CENTRE and RIGHT must look identical, proving glCopyPixels copied correctly.
// The LEFT source is drawn first, then glCopyPixels reads it and writes to CENTRE.
//
// glCopyPixels(GL_DEPTH) and glCopyPixels(GL_STENCIL) are called and expected
// to produce DbgPrint warnings but no crash (stubs).
// ---------------------------------------------------------------------------
static void Test74_CopyPixels()
{
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Three sections: left=source, centre=copy result, right=reference
    const float SW = 190.0f;  // section width
    const float SX[3] = { 10.0f, 220.0f, 430.0f };
    const float SY = 60.0f;
    const float SH = 360.0f;
    const float HW = SW * 0.5f;
    const float HH = SH * 0.5f;

    // -----------------------------------------------------------------------
    // STEP 1: Draw source pattern in LEFT section
    // 4 coloured quadrants
    // -----------------------------------------------------------------------
    glBegin(GL_QUADS);
    // Top-left: red
    glColor3f(1,0,0);
    glVertex2f(SX[0],      SY+HH); glVertex2f(SX[0]+HW, SY+HH);
    glVertex2f(SX[0]+HW,   SY+SH); glVertex2f(SX[0],    SY+SH);
    // Top-right: green
    glColor3f(0,1,0);
    glVertex2f(SX[0]+HW,   SY+HH); glVertex2f(SX[0]+SW, SY+HH);
    glVertex2f(SX[0]+SW,   SY+SH); glVertex2f(SX[0]+HW, SY+SH);
    // Bottom-left: blue
    glColor3f(0,0,1);
    glVertex2f(SX[0],      SY);    glVertex2f(SX[0]+HW, SY);
    glVertex2f(SX[0]+HW,   SY+HH); glVertex2f(SX[0],    SY+HH);
    // Bottom-right: yellow
    glColor3f(1,1,0);
    glVertex2f(SX[0]+HW,   SY);    glVertex2f(SX[0]+SW, SY);
    glVertex2f(SX[0]+SW,   SY+HH); glVertex2f(SX[0]+HW, SY+HH);
    glEnd();

    // Flush source to framebuffer before reading
    glFlush();

    // -----------------------------------------------------------------------
    // STEP 2: glCopyPixels(GL_COLOR) -- read LEFT, write to CENTRE
    // Set raster position to bottom-left of CENTRE section
    // glCopyPixels reads (SX[0], SY, SW, SH) and writes at raster pos
    // -----------------------------------------------------------------------
    glRasterPos2f(SX[1], SY);
    glCopyPixels((GLint)SX[0], (GLint)SY, (GLsizei)SW, (GLsizei)SH, GL_COLOR);

    // -----------------------------------------------------------------------
    // STEP 3: Verify stubs don't crash (depth/stencil -- should DbgPrint only)
    // -----------------------------------------------------------------------
    glRasterPos2f(SX[1], SY);
    glCopyPixels((GLint)SX[0], (GLint)SY, 4, 4, GL_DEPTH);   // stub -- no-op
    glCopyPixels((GLint)SX[0], (GLint)SY, 4, 4, GL_STENCIL); // stub -- no-op

    // -----------------------------------------------------------------------
    // STEP 4: Draw reference pattern in RIGHT section (same as LEFT)
    // CENTRE and RIGHT must look identical
    // -----------------------------------------------------------------------
    glBegin(GL_QUADS);
    glColor3f(1,0,0);
    glVertex2f(SX[2],      SY+HH); glVertex2f(SX[2]+HW, SY+HH);
    glVertex2f(SX[2]+HW,   SY+SH); glVertex2f(SX[2],    SY+SH);
    glColor3f(0,1,0);
    glVertex2f(SX[2]+HW,   SY+HH); glVertex2f(SX[2]+SW, SY+HH);
    glVertex2f(SX[2]+SW,   SY+SH); glVertex2f(SX[2]+HW, SY+SH);
    glColor3f(0,0,1);
    glVertex2f(SX[2],      SY);    glVertex2f(SX[2]+HW, SY);
    glVertex2f(SX[2]+HW,   SY+HH); glVertex2f(SX[2],    SY+HH);
    glColor3f(1,1,0);
    glVertex2f(SX[2]+HW,   SY);    glVertex2f(SX[2]+SW, SY);
    glVertex2f(SX[2]+SW,   SY+HH); glVertex2f(SX[2]+HW, SY+HH);
    glEnd();

    // Labels
    glColor3f(0.85f,0.85f,0.85f);
    glPushMatrix(); glTranslatef(SX[0]+SW*0.5f, SY+SH+10.0f, 0);
    DrawStringCentered("SOURCE", 11.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(SX[1]+SW*0.5f, SY+SH+10.0f, 0);
    DrawStringCentered("COPYPIXELS", 11.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(SX[2]+SW*0.5f, SY+SH+10.0f, 0);
    DrawStringCentered("REFERENCE", 11.0f); glPopMatrix();

    glPushMatrix(); glTranslatef(320.0f, SY-20.0f, 0);
    DrawStringCentered("CENTRE=COPYPIXELS(GL_COLOR)  DEPTH+STENCIL=STUBS(NO CRASH)", 9.0f);
    glPopMatrix();

    // Dividers
    glColor3f(0.4f,0.4f,0.4f);
    glBegin(GL_LINES);
    glVertex2f(SX[1],0); glVertex2f(SX[1],480);
    glVertex2f(SX[2],0); glVertex2f(SX[2],480);
    glEnd();
}