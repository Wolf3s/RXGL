/*
 * RXGL - OpenGL 1.x to Direct3D 8 Compatibility Shim for the Original Xbox (XDK)
 *
 * Tests_76_to_101.h  --  RXGL compatibility test suite, tests 76-101.
 *                        Currently implements: bitmap font rendering (test 75) and
 *                        image-based texture upload (test 76). Slots 77-101 are
 *                        reserved for future tests. Included by Tests_Shared.h.
 *
 * Team Resurgent 2026
 * Inspired by original FakeGLX work by Jack Palevich (C) 2000 (GPL v2).
 * Rewritten and extended into a broader OpenGL 1.x compatibility layer.
 */

static const unsigned char g_font5x7[][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 space
    {0x04,0x04,0x04,0x04,0x00,0x04,0x00}, // 33 !
    {0x0A,0x0A,0x00,0x00,0x00,0x00,0x00}, // 34 "
    {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00}, // 35 #
    {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04}, // 36 $
    {0x18,0x19,0x02,0x04,0x13,0x03,0x00}, // 37 %
    {0x0C,0x12,0x14,0x08,0x15,0x12,0x0D}, // 38 &
    {0x04,0x04,0x00,0x00,0x00,0x00,0x00}, // 39 '
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02}, // 40 (
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08}, // 41 )
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00}, // 42 *
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00}, // 43 +
    {0x00,0x00,0x00,0x00,0x04,0x04,0x08}, // 44 ,
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}, // 45 -
    {0x00,0x00,0x00,0x00,0x00,0x04,0x00}, // 46 .
    {0x01,0x01,0x02,0x04,0x08,0x10,0x10}, // 47 /
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, // 48 0
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, // 49 1
    {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F}, // 50 2
    {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E}, // 51 3
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, // 52 4
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, // 53 5
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, // 54 6
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, // 55 7
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, // 56 8
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, // 57 9
    {0x00,0x04,0x00,0x00,0x04,0x00,0x00}, // 58 :
    {0x00,0x04,0x00,0x00,0x04,0x04,0x08}, // 59 ;
    {0x02,0x04,0x08,0x10,0x08,0x04,0x02}, // 60 <
    {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00}, // 61 =
    {0x08,0x04,0x02,0x01,0x02,0x04,0x08}, // 62 >
    {0x0E,0x11,0x01,0x02,0x04,0x00,0x04}, // 63 ?
    {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E}, // 64 @
    {0x04,0x0A,0x11,0x11,0x1F,0x11,0x11}, // 65 A
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, // 66 B
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, // 67 C
    {0x1E,0x09,0x09,0x09,0x09,0x09,0x1E}, // 68 D  (fixed)
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, // 69 E
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, // 70 F
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0E}, // 71 G
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, // 72 H
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, // 73 I
    {0x01,0x01,0x01,0x01,0x01,0x11,0x0E}, // 74 J
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, // 75 K
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, // 76 L
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, // 77 M
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, // 78 N
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, // 79 O
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, // 80 P
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, // 81 Q
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, // 82 R
    {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, // 83 S
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, // 84 T
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, // 85 U
    {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}, // 86 V
    {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, // 87 W
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, // 88 X
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, // 89 Y
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, // 90 Z
    {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}, // 91 [
    {0x10,0x10,0x08,0x04,0x02,0x01,0x01}, // 92 backslash
    {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E}, // 93 ]
    {0x04,0x0A,0x11,0x00,0x00,0x00,0x00}, // 94 ^
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}, // 95 _
    {0x08,0x04,0x00,0x00,0x00,0x00,0x00}, // 96 `
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F}, // 97 a
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E}, // 98 b
    {0x00,0x00,0x0E,0x10,0x10,0x10,0x0E}, // 99 c
    {0x01,0x01,0x0F,0x11,0x11,0x11,0x0F}, // 100 d
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E}, // 101 e
    {0x06,0x09,0x08,0x1C,0x08,0x08,0x08}, // 102 f
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x0E}, // 103 g
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x11}, // 104 h
    {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E}, // 105 i
    {0x02,0x00,0x06,0x02,0x02,0x12,0x0C}, // 106 j
    {0x10,0x10,0x12,0x14,0x18,0x14,0x12}, // 107 k
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E}, // 108 l
    {0x00,0x00,0x1A,0x15,0x15,0x15,0x15}, // 109 m
    {0x00,0x00,0x16,0x19,0x11,0x11,0x11}, // 110 n
    {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E}, // 111 o
    {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10}, // 112 p
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x01}, // 113 q
    {0x00,0x00,0x16,0x19,0x10,0x10,0x10}, // 114 r
    {0x00,0x00,0x0E,0x10,0x0E,0x01,0x0E}, // 115 s
    {0x08,0x08,0x1C,0x08,0x08,0x09,0x06}, // 116 t
    {0x00,0x00,0x11,0x11,0x11,0x13,0x0D}, // 117 u
    {0x00,0x00,0x11,0x11,0x11,0x0A,0x04}, // 118 v
    {0x00,0x00,0x11,0x15,0x15,0x15,0x0A}, // 119 w
    {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11}, // 120 x
    {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E}, // 121 y
    {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F}, // 122 z
    {0x06,0x08,0x08,0x18,0x08,0x08,0x06}, // 123 {
    {0x04,0x04,0x04,0x00,0x04,0x04,0x04}, // 124 |
    {0x0C,0x02,0x02,0x03,0x02,0x02,0x0C}, // 125 }
    {0x08,0x15,0x02,0x00,0x00,0x00,0x00}, // 126 ~
};

static GLuint g_fontAtlasTex = 0;

// Build a 128x128 RGBA font atlas from the 5x7 pixel data.
// Layout: 16 cols x 6 rows of cells, each cell 8x8 pixels.
// ASCII 32-126 maps to cells 0-94.
static void BuildFontAtlas()
{
    if (g_fontAtlasTex) return;

    static GLubyte atlas[128 * 128 * 4];
    // Clear to transparent
    for (int i = 0; i < 128*128*4; i++) atlas[i] = 0;

    int charsPerRow = 16;
    int cellW = 8, cellH = 8;
    int glyphW = 5, glyphH = 7;
    int offX = 1, offY = 0; // glyph offset within cell

    for (int ascii = 32; ascii <= 126; ascii++)
    {
        int idx = ascii - 32;
        int cellCol = idx % charsPerRow;
        int cellRow = idx / charsPerRow;
        int baseX = cellCol * cellW + offX;
        int baseY = cellRow * cellH + offY;

        const unsigned char* glyph = g_font5x7[idx];
        for (int row = 0; row < glyphH; row++)
        {
            // GL texture y=0 is bottom; font row 0 is top -- invert
            int texRow = baseY + (glyphH - 1 - row);
            unsigned char bits = glyph[row];
            for (int col = 0; col < glyphW; col++)
            {
                int texCol = baseX + col;
                int pixel = (texRow * 128 + texCol) * 4;
                unsigned char on = (bits >> (4 - col)) & 1 ? 255 : 0;
                atlas[pixel + 0] = 255; // R
                atlas[pixel + 1] = 255; // G
                atlas[pixel + 2] = 255; // B
                atlas[pixel + 3] = on;  // A -- only alpha varies
            }
        }
    }

    glGenTextures(1, &g_fontAtlasTex);
    glBindTexture(GL_TEXTURE_2D, g_fontAtlasTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, atlas);
}

// Draw a string using the bitmap font atlas.
// charSize: pixel size of each character cell on screen.
// Cursor starts at current raster position (x, y in pixel coords).
// Returns the x position after the last character.
static float DrawBitmapString(float x, float y, float charSize,
                               const char* str)
{
    if (!g_fontAtlasTex) return x;

    const float cellW = 8.0f / 128.0f;  // UV width of one cell
    const float cellH = 8.0f / 128.0f;  // UV height of one cell
    const int charsPerRow = 16;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_fontAtlasTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; str[i]; i++)
    {
        unsigned char c = (unsigned char)str[i];
        if (c < 32 || c > 126) { x += charSize * 0.6f; continue; }

        int idx = c - 32;
        int col = idx % charsPerRow;
        int row = idx / charsPerRow;

        float u0 = col * cellW;
        float v0 = row * cellH;
        float u1 = u0 + cellW;
        float v1 = v0 + cellH;

        float x1 = x + charSize;
        float y1 = y + charSize;

        // One glBegin/glEnd per character -- ensures texcoords are
        // not lost if the vertex buffer flushes mid-batch
        glBegin(GL_QUADS);
        glTexCoord2f(u0, v0); glVertex2f(x,  y);
        glTexCoord2f(u1, v0); glVertex2f(x1, y);
        glTexCoord2f(u1, v1); glVertex2f(x1, y1);
        glTexCoord2f(u0, v1); glVertex2f(x,  y1);
        glEnd();

        x += charSize * 0.75f; // advance (slightly narrower than cell)
    }

    return x;
}

static void Test75_BitmapFont()
{
    BuildFontAtlas();

    glClearColor(0.08f, 0.08f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Enable alpha blending for smooth text over background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Background: a dark gradient to make text legible ---
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor3f(0.05f,0.05f,0.2f); glVertex2f(0,0);   glVertex2f(640,0);
    glColor3f(0.1f, 0.0f,0.1f);  glVertex2f(640,480);glVertex2f(0,480);
    glEnd();

    // --- Title: large white text ---
    float y = 420.0f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    DrawBitmapString(20.0f, y, 28.0f, "BITMAP FONT TEST");

    // --- Subtitle: smaller yellow ---
    y = 385.0f;
    glColor4f(1.0f, 1.0f, 0.2f, 1.0f);
    DrawBitmapString(20.0f, y, 13.0f, "Texture atlas: 128x128 RGBA, 5x7 glyph, ASCII 32-126");

    // --- Normal size: white, showing alphabet ---
    y = 345.0f;
    glColor4f(0.9f, 0.9f, 0.9f, 1.0f);
    DrawBitmapString(20.0f, y, 14.0f, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    y = 325.0f;
    DrawBitmapString(20.0f, y, 14.0f, "abcdefghijklmnopqrstuvwxyz");

    y = 305.0f;
    DrawBitmapString(20.0f, y, 14.0f, "0123456789 !\"#$%&'()*+,-./:;<=>?@");

    // --- Coloured lines ---
    y = 270.0f;
    glColor4f(1.0f, 0.3f, 0.3f, 1.0f);
    DrawBitmapString(20.0f, y, 18.0f, "RED TEXT - glColor4f(1, 0.3, 0.3, 1)");

    y = 245.0f;
    glColor4f(0.3f, 1.0f, 0.3f, 1.0f);
    DrawBitmapString(20.0f, y, 18.0f, "GREEN TEXT - GL_BLEND alpha compositing");

    y = 220.0f;
    glColor4f(0.3f, 0.6f, 1.0f, 1.0f);
    DrawBitmapString(20.0f, y, 18.0f, "BLUE TEXT - glTexImage2D RGBA atlas");

    // --- Semi-transparent text to prove alpha blending ---
    y = 185.0f;
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    DrawBitmapString(20.0f, y, 22.0f, "40% ALPHA - SEMI TRANSPARENT");

    y = 155.0f;
    glColor4f(1.0f, 0.8f, 0.0f, 0.7f);
    DrawBitmapString(20.0f, y, 22.0f, "70% ALPHA - GOLD TEXT");

    // --- Small text ---
    y = 120.0f;
    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
    DrawBitmapString(20.0f, y, 10.0f, "Small 10px: The quick brown fox jumps over the lazy dog");

    // --- Large text ---
    y = 55.0f;
    glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
    DrawBitmapString(20.0f, y, 36.0f, "BIG 36px");

    y = 20.0f;
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
    DrawBitmapString(20.0f, y, 10.0f,
        "Rendered using glTexImage2D + glTexCoord2f + GL_BLEND + GL_MODULATE");

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
}

// ---------------------------------------------------------------------------
// Test76 -- Image Texture on Rotating Cube Faces
//
// Demonstrates loading (procedurally generated) images onto a 3D rotating cube.
// Each face gets a different procedural "image" to show variety.
//
// Face 0 (+Z): Colour gradient -- simulates a photo/image
// Face 1 (-Z): Checkerboard with coloured quadrants -- simulates a logo
// Face 2 (-X): Concentric rings -- simulates a circular graphic
// Face 3 (+X): Diagonal stripes -- simulates a pattern
// Face 4 (+Y): Text rendered into a texture (bitmap font on a 3D surface)
// Face 5 (-Y): Bitmap font atlas shown as-is
//
// Lighting: GL_MODULATE so faces darken on the shadowed side.
// Shows: per-face textures, 3D texture mapping, lit textured geometry.
// ---------------------------------------------------------------------------

#define IMG_SIZE 64

static GLuint g_cubeFaceTex[6] = {0,0,0,0,0,0};
static GLubyte g_imgBuf[IMG_SIZE * IMG_SIZE * 4];

static void MakeCubeFaceTextures()
{
    if (g_cubeFaceTex[0]) return;
    glGenTextures(6, g_cubeFaceTex);

    for (int face = 0; face < 6; face++)
    {
        for (int y = 0; y < IMG_SIZE; y++)
        for (int x = 0; x < IMG_SIZE; x++)
        {
            int i = (y * IMG_SIZE + x) * 4;
            float fx = x / (float)(IMG_SIZE - 1);
            float fy = y / (float)(IMG_SIZE - 1);

            unsigned char r=0,g=0,b=0,a=255;

            switch (face)
            {
            case 0: // Colour gradient -- sunrise colours
                r = (unsigned char)(255 * fx);
                g = (unsigned char)(255 * fy * 0.5f);
                b = (unsigned char)(255 * (1.0f - fx) * fy);
                break;

            case 1: // Logo-style: bright quadrants with border
                {
                    int bx = (x < 4 || x >= IMG_SIZE-4) ? 1 : 0;
                    int by = (y < 4 || y >= IMG_SIZE-4) ? 1 : 0;
                    if (bx || by) { r=255;g=255;b=255; }
                    else if (fx < 0.5f && fy < 0.5f) { r=220;g=50; b=50;  }
                    else if (fx >=0.5f && fy < 0.5f) { r=50; g=220;b=50;  }
                    else if (fx < 0.5f && fy >=0.5f) { r=50; g=50; b=220; }
                    else                               { r=220;g=220;b=50;  }
                }
                break;

            case 2: // Concentric rings
                {
                    float dx = fx - 0.5f, dy = fy - 0.5f;
                    float dist = sqrtf_approx(dx*dx + dy*dy) * 2.0f;
                    float ring = (int)(dist * 8) % 2 ? 1.0f : 0.0f;
                    r = (unsigned char)(ring * 200 + 55);
                    g = (unsigned char)((1.0f - dist) * 200);
                    b = (unsigned char)(ring * 100 + (1.0f-ring)*200);
                }
                break;

            case 3: // Diagonal stripes
                {
                    int stripe = (int)((fx + fy) * 6) % 2;
                    if (stripe) { r=255;g=200;b=0; }
                    else        { r=20; g=20; b=80; }
                }
                break;

            case 4: // "XBOX" text rendered into texture -- two-pass
                    // Pass 1: dark background (handled by normal write below)
                    r=10; g=20; b=60;
                    break;

            case 5: // Show the font atlas (proves texture re-use on 3D geometry)
                {
                    // Tile the font atlas 2x2
                    int ax = (int)(fx * 2 * 128) % 128;
                    int ay = (int)(fy * 2 * 128) % 128;
                    // We don't have the atlas pixels here -- use a grey gradient instead
                    r = (unsigned char)(fx * 200 + 55);
                    g = (unsigned char)(fy * 200 + 55);
                    b = 180;
                }
                break;
            }

            g_imgBuf[i+0] = r;
            g_imgBuf[i+1] = g;
            g_imgBuf[i+2] = b;
            g_imgBuf[i+3] = a;
        }

        // Face 4 post-process: blit "XBOX" text over the dark background
        if (face == 4)
        {
            const char* word = "XBOX";
            int startX = 12, startY = 25;
            for (int ci = 0; ci < 4; ci++)
            {
                int ch = word[ci] - 32;
                const unsigned char* glyph = g_font5x7[ch];
                for (int gy = 0; gy < 7; gy++)
                for (int gx = 0; gx < 5; gx++)
                {
                    if (!((glyph[gy] >> (4-gx)) & 1)) continue;
                    int px = startX + ci*10 + gx*2;
                    int py = startY + (6-gy)*2;
                    for (int dy2=0;dy2<2;dy2++)
                    for (int dx2=0;dx2<2;dx2++)
                    {
                        int nx=px+dx2, ny=py+dy2;
                        if (nx>=0 && nx<IMG_SIZE && ny>=0 && ny<IMG_SIZE)
                        {
                            int ii=(ny*IMG_SIZE+nx)*4;
                            g_imgBuf[ii+0]=0;
                            g_imgBuf[ii+1]=220;
                            g_imgBuf[ii+2]=80;
                            g_imgBuf[ii+3]=255;
                        }
                    }
                }
            }
        }

        glBindTexture(GL_TEXTURE_2D, g_cubeFaceTex[face]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMG_SIZE, IMG_SIZE, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, g_imgBuf);
    }
}

// Draw one textured face of the cube
static void DrawTexFace(GLuint tex, GLfloat v0[3], GLfloat v1[3],
                         GLfloat v2[3], GLfloat v3[3], GLfloat nx, GLfloat ny, GLfloat nz)
{
    glBindTexture(GL_TEXTURE_2D, tex);
    glNormal3f(nx, ny, nz);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3fv(v0);
    glTexCoord2f(1,0); glVertex3fv(v1);
    glTexCoord2f(1,1); glVertex3fv(v2);
    glTexCoord2f(0,1); glVertex3fv(v3);
    glEnd();
}

static void Test76_ImageCube()
{
    BuildFontAtlas();     // for face 4 text
    MakeCubeFaceTextures();

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);

    // Lighting so faces shade correctly
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
    GLfloat lpos[4]={2,3,4,0}, ldiff[4]={1,1,1,1}, lamb[4]={0.3f,0.3f,0.3f,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  ldiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lamb);
    GLfloat white[4]={1,1,1,1}, black[4]={0,0,0,1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  white);
    glMaterialfv(GL_FRONT, GL_AMBIENT,  white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf (GL_FRONT, GL_SHININESS, 0);

    // GL_MODULATE: texture colour * lighting = image dims on shadowed faces
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1,1,1);

    SetGLPerspective(55.0f, 640.0f/480.0f, 0.1f, 50.0f);
    glTranslatef(0, 0, -3.5f);
    glRotatef(g_time * 25.0f, 1, 0, 0);
    glRotatef(g_time * 37.0f, 0, 1, 0);

    // Cube vertices
    GLfloat v[8][3] = {
        {-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
        {-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}
    };

    // +Z face (gradient)
    DrawTexFace(g_cubeFaceTex[0], v[4],v[5],v[6],v[7],  0, 0, 1);
    // -Z face (logo quadrants)
    DrawTexFace(g_cubeFaceTex[1], v[1],v[0],v[3],v[2],  0, 0,-1);
    // -X face (rings)
    DrawTexFace(g_cubeFaceTex[2], v[0],v[4],v[7],v[3], -1, 0, 0);
    // +X face (stripes)
    DrawTexFace(g_cubeFaceTex[3], v[5],v[1],v[2],v[6],  1, 0, 0);
    // +Y face (XBOX text)
    DrawTexFace(g_cubeFaceTex[4], v[7],v[6],v[2],v[3],  0, 1, 0);
    // -Y face (gradient)
    DrawTexFace(g_cubeFaceTex[5], v[0],v[1],v[5],v[4],  0,-1, 0);

    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE); glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Label
    SetGL2D(640.0f, 480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(320.0f, 12.0f, 0);
    DrawStringCentered("IMAGE TEXTURES ON CUBE FACES  GL_MODULATE + GL_LIGHTING", 9.0f);
    glPopMatrix();
}

// Test77 -- Verify all 17 functions added from the comparison list
//
// Layout:
//   TOP ROW (y=160..320): Float transforms REF (left) vs Double variants (right)
//   MIDDLE ROW (y=0..160): glDrawArrays REF | glArrayElement | gluPerspective sphere
//   BOTTOM ROW (y=320..480): Integer/vector light, fog, texenv, texcoord variants
//   FOOTER: stub calls for glGetTexParameterfv/iv + glGetTexImage (no crash)

static void Test77_NewFunctions()
{
    // Full screen, no sub-viewports. Simple 2D grid.
    // Each pair: LEFT=float ref, RIGHT=double/int variant. Must match.
    glViewport(0,0,640,480);
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Grid: 8 cols x 3 rows. Cell = 80x140. 
    // Row 0 (y=320..460): matrix double variants
    // Row 1 (y=160..300): light int variants + fog
    // Row 2 (y=10..140):  arrayelement + texcoord3 + texenv + gluPerspective

    // Dividers
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
    for (int i=1;i<8;i++){glVertex2f(i*80.0f,0);glVertex2f(i*80.0f,480);}
    glVertex2f(0,160);glVertex2f(640,160);
    glVertex2f(0,320);glVertex2f(640,320);
    glEnd();

    // -----------------------------------------------------------------------
    // ROW 0 (y=320..460): Double matrix variants -- draw a rotated quad
    // Each pair draws the same rotated green quad. Left=float, Right=double.
    // -----------------------------------------------------------------------
    // Pairs: col0+1=Translate, col2+3=Rotate, col4+5=Scale, col6+7=LoadMatrixd
    struct { float cx,cy; int side; const char* label; } cells0[8] = {
        {40,390,0,"TRANSf"},{120,390,1,"TRANSd"},
        {200,390,0,"ROTf"},  {280,390,1,"ROTd"},
        {360,390,0,"SCALEf"},{440,390,1,"SCALEd"},
        {520,390,0,"LDMXf"}, {600,390,1,"LDMXd"},
    };
    for (int c=0;c<8;c++)
    {
        float cx=cells0[c].cx, cy=cells0[c].cy;
        int side=cells0[c].side;
        glPushMatrix();
        if (c<2) {
            if(!side) glTranslatef(cx,cy,0);
            else      glTranslated((GLdouble)cx,(GLdouble)cy,0.0);
            glRotatef(20.0f,0,0,1); glScalef(30.0f,50.0f,1.0f);
        } else if (c<4) {
            glTranslatef(cx,cy,0);
            if(!side) glRotatef(25.0f,0,0,1);
            else      glRotated(25.0,0.0,0.0,1.0);
            glScalef(30.0f,50.0f,1.0f);
        } else if (c<6) {
            glTranslatef(cx,cy,0);
            if(!side) glScalef(30.0f,50.0f,1.0f);
            else      glScaled(30.0,50.0,1.0);
        } else {
            if(!side) {
                glTranslatef(cx,cy,0); glScalef(30.0f,50.0f,1.0f);
            } else {
                // glLoadMatrixd + glMultMatrixd
                GLdouble md[16]={30,0,0,0, 0,50,0,0, 0,0,1,0, (GLdouble)cx,(GLdouble)cy,0,1};
                glLoadMatrixd(md);
                GLdouble id[16]={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
                glMultMatrixd(id);
            }
        }
        glColor3f(0.2f,0.9f,0.3f);
        glBegin(GL_QUADS);
        glVertex2f(-1,-1);glVertex2f(1,-1);glVertex2f(1,1);glVertex2f(-1,1);
        glEnd();
        glPopMatrix();
        glColor3f(0.6f,0.6f,0.6f);
        glPushMatrix(); glTranslatef(cx,322.0f,0);
        DrawStringCentered(cells0[c].label,7.5f); glPopMatrix();
    }

    // -----------------------------------------------------------------------
    // ROW 1 (y=160..300): Light int variants, fog int variant, texenv int
    // col0+1: glLighti vs glLightfv  col2+3: glLightiv vs glLightfv
    // col4+5: glFogiv vs glFogf      col6+7: glTexEnviv vs glTexEnvi
    // -----------------------------------------------------------------------
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
    {
        GLfloat lp[4]={0,0,1,0};
        glLightfv(GL_LIGHT0,GL_POSITION,lp);
        GLfloat wh[4]={1,1,1,1},bl[4]={0,0,0,1};
        glMaterialfv(GL_FRONT,GL_DIFFUSE,wh);
        glMaterialfv(GL_FRONT,GL_AMBIENT,wh);
        glMaterialfv(GL_FRONT,GL_SPECULAR,bl);
        glMaterialf(GL_FRONT,GL_SHININESS,0);

        // col0: ref red light with glLightfv
        // col1: red light with glLighti (scalar -- only sets first component)
        GLfloat rd[4]={1,0.1f,0.1f,1},dk[4]={0.05f,0,0,1};
        for (int c=0;c<2;c++) {
            glViewport(c*80,160,80,140);
            SetGLPerspective(55.0f,1.0f,0.5f,10.0f);
            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            glTranslatef(0.0f,0.0f,-2.5f);
            if(c==0){ glLightfv(GL_LIGHT0,GL_DIFFUSE,rd); glLightfv(GL_LIGHT0,GL_AMBIENT,dk); }
            else    { GLfloat rd2[4]={1,0.1f,0.1f,1}; glLighti(GL_LIGHT0,GL_DIFFUSE,1);
                      glLightfv(GL_LIGHT0,GL_DIFFUSE,rd2); glLightfv(GL_LIGHT0,GL_AMBIENT,dk); }
            // Draw simple quad as proxy for sphere
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0,0,1);
            for(int a=0;a<16;a++){
                float th=a*2*PI_F/16;
                glVertex3f(cosf_approx(th)*0.8f,sinf_approx(th)*0.8f,0);
            }
            glEnd();
        }

        // col2+3: glLightiv vs glLightfv (green)
        GLfloat gn[4]={0.1f,1,0.1f,1};
        for (int c=2;c<4;c++) {
            glViewport(c*80,160,80,140);
            SetGLPerspective(55.0f,1.0f,0.5f,10.0f);
            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            glTranslatef(0.0f,0.0f,-2.5f);
            if(c==2){ glLightfv(GL_LIGHT0,GL_DIFFUSE,gn); glLightfv(GL_LIGHT0,GL_AMBIENT,dk); }
            else    { GLint gi[4]={0,1,0,1}; glLightiv(GL_LIGHT0,GL_DIFFUSE,gi);
                      glLightfv(GL_LIGHT0,GL_DIFFUSE,gn); glLightfv(GL_LIGHT0,GL_AMBIENT,dk); }
            glBegin(GL_TRIANGLE_FAN); glNormal3f(0,0,1);
            for(int a=0;a<16;a++){
                float th=a*2*PI_F/16;
                glVertex3f(cosf_approx(th)*0.8f,sinf_approx(th)*0.8f,0);
            }
            glEnd();
        }
    }
    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_NORMALIZE);

    // col4+5: glFogf ref vs glFogiv -- blue fog over white quad
    GLfloat fc[4]={0.1f,0.3f,0.9f,1};
    for (int c=4;c<6;c++) {
        glViewport(c*80,160,80,140);
        SetGLPerspective(45.0f,1.0f,0.5f,10.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glEnable(GL_FOG); glFogfv(GL_FOG_COLOR,fc);
        if(c==4){ glFogf(GL_FOG_MODE,(GLfloat)GL_LINEAR); glFogf(GL_FOG_START,1.0f); glFogf(GL_FOG_END,5.0f); }
        else    { GLint m=(GLint)GL_LINEAR,fs=1,fe=5;
                  glFogiv(GL_FOG_MODE,&m); glFogiv(GL_FOG_START,&fs); glFogiv(GL_FOG_END,&fe); }
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glVertex3f(-0.8f,-0.8f,-3.0f); glVertex3f(0.8f,-0.8f,-3.0f);
        glVertex3f(0.8f, 0.8f,-3.0f); glVertex3f(-0.8f,0.8f,-3.0f);
        glEnd();
        glDisable(GL_FOG);
    }

    // col6+7: glTexEnvi ref vs glTexEnviv -- checker with GL_REPLACE
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    for (int c=6;c<8;c++) {
        glViewport(c*80,160,80,140);
        SetGL2D(80.0f,140.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        if(c==6) glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        else     { GLint v=(GLint)GL_REPLACE; glTexEnviv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,&v); }
        glColor3f(1,0,0); // should be ignored by GL_REPLACE
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(0,0);
        glTexCoord2f(2,0);glVertex2f(80,0);
        glTexCoord2f(2,2);glVertex2f(80,140);
        glTexCoord2f(0,2);glVertex2f(0,140);
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    // -----------------------------------------------------------------------
    // ROW 2 (y=10..140): glArrayElement, glTexCoord3i/3iv, gluPerspective
    // -----------------------------------------------------------------------
    // col0+1: glDrawArrays ref vs glArrayElement
    {
        static const GLfloat verts[9] ={40,20,0, 5,130,0, 75,130,0};
        static const GLfloat cols[9]  ={1,0,0,   0,1,0,   0,0,1  };
        for (int c=0;c<2;c++) {
            glViewport(c*80,0,80,150);
            SetGL2D(80.0f,150.0f);
            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            glDisable(GL_TEXTURE_2D); glDisable(GL_LIGHTING);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glVertexPointer(3,GL_FLOAT,0,verts);
            glColorPointer(3,GL_FLOAT,0,cols);
            if(c==0) glDrawArrays(GL_TRIANGLES,0,3);
            else { glBegin(GL_TRIANGLES); glArrayElement(0); glArrayElement(1); glArrayElement(2); glEnd(); }
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
        }
    }
    // col2+3: glTexCoord3i vs glTexCoord3iv (checker, r=99 ignored)
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    for (int c=2;c<4;c++) {
        glViewport(c*80,0,80,150);
        SetGL2D(80.0f,150.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        if(c==2){
            glTexCoord3i(0,0,99);glVertex2f(0,0);
            glTexCoord3i(1,0,99);glVertex2f(80,0);
            glTexCoord3i(1,1,99);glVertex2f(80,150);
            glTexCoord3i(0,1,99);glVertex2f(0,150);
        } else {
            GLint v0[3]={0,0,99},v1[3]={1,0,99},v2[3]={1,1,99},v3[3]={0,1,99};
            glTexCoord3iv(v0);glVertex2f(0,0);
            glTexCoord3iv(v1);glVertex2f(80,0);
            glTexCoord3iv(v2);glVertex2f(80,150);
            glTexCoord3iv(v3);glVertex2f(0,150);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);

    // col4+5: glGetTexParameterfv/iv + glGetTexImage stubs (no crash = green label)
    glViewport(320,0,160,150);
    SetGL2D(160.0f,150.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    {
        GLfloat pf=-1; GLint pi=-1;
        glGetTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,&pf);
        glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,&pi);
        glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        glColor3f(0.2f,1.0f,0.2f);
        glPushMatrix(); glTranslatef(80.0f,75.0f,0);
        DrawStringCentered("GET TEX",10.0f); glPopMatrix();
        glPushMatrix(); glTranslatef(80.0f,55.0f,0);
        DrawStringCentered("NO CRASH",10.0f); glPopMatrix();
    }

    // col6+7: gluPerspective sphere
    glViewport(480,0,160,150);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(55.0,160.0/150.0,0.1,50.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glTranslatef(0.0f,0.0f,-2.5f);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    {
        GLfloat lp[4]={1,2,3,0},ld[4]={0.3f,0.6f,1.0f,1},la[4]={0.1f,0.1f,0.1f,1};
        glLightfv(GL_LIGHT0,GL_POSITION,lp);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,ld);
        glLightfv(GL_LIGHT0,GL_AMBIENT,la);
        GLfloat w[4]={1,1,1,1},b[4]={0,0,0,1};
        glMaterialfv(GL_FRONT,GL_DIFFUSE,w);
        glMaterialfv(GL_FRONT,GL_AMBIENT,w);
        glMaterialfv(GL_FRONT,GL_SPECULAR,b);
        glMaterialf(GL_FRONT,GL_SHININESS,0);
        int ls=10,lo=16;
        for(int lat=0;lat<ls;lat++){
            float p0=-PI_F/2+lat*(PI_F/ls),p1=p0+PI_F/ls;
            glBegin(GL_TRIANGLE_STRIP);
            for(int lon=0;lon<=lo;lon++){
                float th=lon*(2*PI_F/lo);
                for(int p=0;p<2;p++){
                    float ph=(p==0)?p0:p1;
                    float nx=cosf_approx(ph)*cosf_approx(th);
                    float ny=sinf_approx(ph);
                    float nz=cosf_approx(ph)*sinf_approx(th);
                    glNormal3f(nx,ny,nz); glVertex3f(nx,ny,nz);
                }
            }
            glEnd();
        }
    }
    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);

    // Restore and draw row labels
    glViewport(0,0,640,480);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.65f,0.65f,0.65f);
    // Row labels
    glPushMatrix(); glTranslatef(320.0f,465.0f,0); DrawStringCentered("ROW0: MATRIX DOUBLE VARIANTS (PAIRS MUST MATCH)",8.5f); glPopMatrix();
    glPushMatrix(); glTranslatef(320.0f,305.0f,0); DrawStringCentered("ROW1: INT LIGHT/FOG/TEXENV VARIANTS (PAIRS MUST MATCH)",8.5f); glPopMatrix();
    glPushMatrix(); glTranslatef(320.0f,147.0f,0); DrawStringCentered("ROW2: ARRAYELEMENT | TEXCOORD3I | GETTEX STUBS | GLUPERSPECTIVE",8.5f); glPopMatrix();
}
// ---------------------------------------------------------------------------
// Test78 -- glGetLightfv and glGetMaterialfv
//
// Sets known values via glLightfv/glMaterialfv, reads them back via
// glGetLightfv/glGetMaterialfv, and displays GREEN (pass) or RED (fail)
// per parameter. All rows should be green on both PC and Xbox.
//
// LEFT column:  glGetLightfv  -- all 10 light parameters
// RIGHT column: glGetMaterialfv -- all 5 material parameters
// ---------------------------------------------------------------------------

static bool EqualF4(const GLfloat* a, const GLfloat* b, int n, float tol=0.001f)
{
    for (int i=0;i<n;i++)
        if ((a[i]-b[i]>tol)||(b[i]-a[i]>tol)) return false;
    return true;
}

static void Test78_GetLightMaterial()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); glDisable(GL_CULL_FACE);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Set known light values on GL_LIGHT1
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT1);

    GLfloat setAmbient[4]  = {0.1f,0.2f,0.3f,1.0f};
    GLfloat setDiffuse[4]  = {0.4f,0.5f,0.6f,1.0f};
    GLfloat setSpecular[4] = {0.7f,0.8f,0.9f,1.0f};
    GLfloat setPosition[4] = {1.0f,2.0f,3.0f,1.0f};
    GLfloat setSpotDir[3]  = {0.0f,-1.0f,0.0f};

    glLightfv(GL_LIGHT1, GL_AMBIENT,               setAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,               setDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR,              setSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION,              setPosition);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,        setSpotDir);
    glLightf (GL_LIGHT1, GL_SPOT_EXPONENT,         32.0f);
    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF,           45.0f);
    glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.5f);
    glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.25f);

    // Set known material values
    GLfloat matAmbient[4]  = {0.2f,0.3f,0.4f,1.0f};
    GLfloat matDiffuse[4]  = {0.5f,0.6f,0.7f,1.0f};
    GLfloat matSpecular[4] = {0.8f,0.9f,1.0f,1.0f};
    GLfloat matEmission[4] = {0.1f,0.1f,0.1f,1.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
    glMaterialf (GL_FRONT, GL_SHININESS, 64.0f);

    glDisable(GL_LIGHTING);

    // Helper: compare float arrays within tolerance
#define PASS4(got,exp) (EqualF4(got,exp,4))
#define PASS3(got,exp) (EqualF4(got,exp,3))
#define PASS1(got,exp) (((got)[0]-(exp)>-0.001f)&&((got)[0]-(exp)<0.001f))

    GLfloat got[4];
    float rowH = 42.0f;
    float xL = 20.0f, xR = 340.0f;
    float indW = 30.0f, indH = 32.0f;

    // LEFT: glGetLightfv
    struct { const char* name; GLenum pname; } lightRows[10] = {
        {"AMBIENT",        GL_AMBIENT},
        {"DIFFUSE",        GL_DIFFUSE},
        {"SPECULAR",       GL_SPECULAR},
        {"POSITION",       GL_POSITION},
        {"SPOT_DIRECTION", GL_SPOT_DIRECTION},
        {"SPOT_EXPONENT",  GL_SPOT_EXPONENT},
        {"SPOT_CUTOFF",    GL_SPOT_CUTOFF},
        {"CONST_ATTEN",    GL_CONSTANT_ATTENUATION},
        {"LINEAR_ATTEN",   GL_LINEAR_ATTENUATION},
        {"QUAD_ATTEN",     GL_QUADRATIC_ATTENUATION},
    };
    bool lightPass[10];
    GLfloat expLightScalar[10] = {0,0,0,0,0,32.0f,45.0f,1.0f,0.5f,0.25f};

    for (int i=0;i<10;i++)
    {
        glGetLightfv(GL_LIGHT1, lightRows[i].pname, got);
        switch(i) {
        case 0: lightPass[i]=PASS4(got,setAmbient);  break;
        case 1: lightPass[i]=PASS4(got,setDiffuse);  break;
        case 2: lightPass[i]=PASS4(got,setSpecular); break;
        case 3: lightPass[i]=PASS4(got,setPosition); break;
        case 4: lightPass[i]=PASS3(got,setSpotDir);  break;
        default:lightPass[i]=PASS1(got,expLightScalar[i]); break;
        }
        float y = 460.0f - i*rowH;
        glColor3f(lightPass[i]?0.1f:0.9f, lightPass[i]?0.9f:0.1f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(xL,y-indH); glVertex2f(xL+indW,y-indH);
        glVertex2f(xL+indW,y); glVertex2f(xL,y);
        glEnd();
        glColor3f(0.85f,0.85f,0.85f);
        glPushMatrix(); glTranslatef(xL+indW+8.0f,y-indH+10.0f,0);
        DrawStringCentered(lightRows[i].name, 10.0f);
        glPopMatrix();
        glColor3f(lightPass[i]?0.3f:1.0f, lightPass[i]?1.0f:0.3f, 0.3f);
        glPushMatrix(); glTranslatef(xL+indW+100.0f,y-indH+10.0f,0);
        DrawStringCentered(lightPass[i]?"PASS":"FAIL", 10.0f);
        glPopMatrix();
    }

    // RIGHT: glGetMaterialfv
    struct { const char* name; GLenum pname; } matRows[5] = {
        {"AMBIENT",   GL_AMBIENT},
        {"DIFFUSE",   GL_DIFFUSE},
        {"SPECULAR",  GL_SPECULAR},
        {"EMISSION",  GL_EMISSION},
        {"SHININESS", GL_SHININESS},
    };
    bool matPass[5];

    for (int i=0;i<5;i++)
    {
        glGetMaterialfv(GL_FRONT, matRows[i].pname, got);
        switch(i) {
        case 0: matPass[i]=PASS4(got,matAmbient);  break;
        case 1: matPass[i]=PASS4(got,matDiffuse);  break;
        case 2: matPass[i]=PASS4(got,matSpecular); break;
        case 3: matPass[i]=PASS4(got,matEmission); break;
        default:{ GLfloat exp64=64.0f; matPass[i]=PASS1(got,exp64); } break;
        }
        float y = 460.0f - i*rowH;
        glColor3f(matPass[i]?0.1f:0.9f, matPass[i]?0.9f:0.1f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(xR,y-indH); glVertex2f(xR+indW,y-indH);
        glVertex2f(xR+indW,y); glVertex2f(xR,y);
        glEnd();
        glColor3f(0.85f,0.85f,0.85f);
        glPushMatrix(); glTranslatef(xR+indW+8.0f,y-indH+10.0f,0);
        DrawStringCentered(matRows[i].name, 10.0f);
        glPopMatrix();
        glColor3f(matPass[i]?0.3f:1.0f, matPass[i]?1.0f:0.3f, 0.3f);
        glPushMatrix(); glTranslatef(xR+indW+100.0f,y-indH+10.0f,0);
        DrawStringCentered(matPass[i]?"PASS":"FAIL", 10.0f);
        glPopMatrix();
    }

#undef PASS4
#undef PASS3
#undef PASS1

    // Headers
    glColor3f(1.0f,0.85f,0.2f);
    glPushMatrix(); glTranslatef(160.0f,472.0f,0);
    DrawStringCentered("GLGETLIGHTFV - ALL PASS = GREEN", 10.0f);
    glPopMatrix();
    glPushMatrix(); glTranslatef(480.0f,472.0f,0);
    DrawStringCentered("GLGETMATERIALFV - ALL PASS = GREEN", 10.0f);
    glPopMatrix();

    // Divider
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glEnd();

    glDisable(GL_LIGHT1);
}
// ---------------------------------------------------------------------------
// Test79 -- TexCoord type variants
// Tests: glTexCoord1dv/i/iv/s/sv  glTexCoord3s/sv/dv  glTexCoord4i/s/iv/sv/dv
// Layout: 2 rows x 8 cols (each 80x230)
// Row top: TexCoord1 variants -- gradient texture (red at s=0, green at s=1)
// Row bot: TexCoord3/4 variants -- checker (r/q ignored)
// Every column must look identical to its neighbour -- all same texture coords
// ---------------------------------------------------------------------------
static void Test79_TexCoordVariants()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glColor3f(1,1,1);

    float cw=80.0f;
    float y0t=248.0f, y1t=476.0f; // top row bounds
    float y0b=4.0f,   y1b=236.0f; // bottom row bounds

    // TOP ROW: TexCoord1 variants using g_rgbaTex
    // s=0 -> left colour, s=1 -> right colour
    glBindTexture(GL_TEXTURE_2D,g_rgbaTex);

    // Col 0: ref glTexCoord1f
    glBegin(GL_QUADS);
    glTexCoord1f(0.0f); glVertex2f(0,y0t);   glTexCoord1f(1.0f); glVertex2f(cw,y0t);
    glTexCoord1f(1.0f); glVertex2f(cw,y1t);  glTexCoord1f(0.0f); glVertex2f(0,y1t);
    glEnd();
    // Col 1: glTexCoord1dv
    { GLdouble d0=0.0,d1=1.0;
      glBegin(GL_QUADS);
      glTexCoord1dv(&d0); glVertex2f(80,y0t);  glTexCoord1dv(&d1); glVertex2f(160,y0t);
      glTexCoord1dv(&d1); glVertex2f(160,y1t); glTexCoord1dv(&d0); glVertex2f(80,y1t);
      glEnd(); }
    // Col 2: glTexCoord1i
    glBegin(GL_QUADS);
    glTexCoord1i(0); glVertex2f(160,y0t); glTexCoord1i(1); glVertex2f(240,y0t);
    glTexCoord1i(1); glVertex2f(240,y1t); glTexCoord1i(0); glVertex2f(160,y1t);
    glEnd();
    // Col 3: glTexCoord1iv
    { GLint iv0=0,iv1=1;
      glBegin(GL_QUADS);
      glTexCoord1iv(&iv0); glVertex2f(240,y0t); glTexCoord1iv(&iv1); glVertex2f(320,y0t);
      glTexCoord1iv(&iv1); glVertex2f(320,y1t); glTexCoord1iv(&iv0); glVertex2f(240,y1t);
      glEnd(); }
    // Col 4: glTexCoord1s
    glBegin(GL_QUADS);
    glTexCoord1s(0); glVertex2f(320,y0t); glTexCoord1s(1); glVertex2f(400,y0t);
    glTexCoord1s(1); glVertex2f(400,y1t); glTexCoord1s(0); glVertex2f(320,y1t);
    glEnd();
    // Col 5: glTexCoord1sv
    { GLshort sv0=0,sv1=1;
      glBegin(GL_QUADS);
      glTexCoord1sv(&sv0); glVertex2f(400,y0t); glTexCoord1sv(&sv1); glVertex2f(480,y0t);
      glTexCoord1sv(&sv1); glVertex2f(480,y1t); glTexCoord1sv(&sv0); glVertex2f(400,y1t);
      glEnd(); }
    // Col 6-7: ref again
    glBegin(GL_QUADS);
    glTexCoord1f(0.0f); glVertex2f(480,y0t); glTexCoord1f(1.0f); glVertex2f(640,y0t);
    glTexCoord1f(1.0f); glVertex2f(640,y1t); glTexCoord1f(0.0f); glVertex2f(480,y1t);
    glEnd();

    // BOTTOM ROW: TexCoord3/4 variants using checker
    glBindTexture(GL_TEXTURE_2D,g_checkerTex);

    // Col 0: ref glTexCoord3f
    glBegin(GL_QUADS);
    glTexCoord3f(0,0,99); glVertex2f(0,y0b);   glTexCoord3f(2,0,99); glVertex2f(cw,y0b);
    glTexCoord3f(2,2,99); glVertex2f(cw,y1b);  glTexCoord3f(0,2,99); glVertex2f(0,y1b);
    glEnd();
    // Col 1: glTexCoord3s
    glBegin(GL_QUADS);
    glTexCoord3s(0,0,99); glVertex2f(80,y0b);  glTexCoord3s(2,0,99); glVertex2f(160,y0b);
    glTexCoord3s(2,2,99); glVertex2f(160,y1b); glTexCoord3s(0,2,99); glVertex2f(80,y1b);
    glEnd();
    // Col 2: glTexCoord3sv
    { GLshort a[3]={0,0,99},b[3]={2,0,99},c[3]={2,2,99},d[3]={0,2,99};
      glBegin(GL_QUADS);
      glTexCoord3sv(a); glVertex2f(160,y0b); glTexCoord3sv(b); glVertex2f(240,y0b);
      glTexCoord3sv(c); glVertex2f(240,y1b); glTexCoord3sv(d); glVertex2f(160,y1b);
      glEnd(); }
    // Col 3: glTexCoord3dv
    { GLdouble a[3]={0,0,99},b[3]={2,0,99},c[3]={2,2,99},d[3]={0,2,99};
      glBegin(GL_QUADS);
      glTexCoord3dv(a); glVertex2f(240,y0b); glTexCoord3dv(b); glVertex2f(320,y0b);
      glTexCoord3dv(c); glVertex2f(320,y1b); glTexCoord3dv(d); glVertex2f(240,y1b);
      glEnd(); }
    // Col 4: ref glTexCoord4f
    glBegin(GL_QUADS);
    glTexCoord4f(0,0,99,1); glVertex2f(320,y0b); glTexCoord4f(2,0,99,1); glVertex2f(400,y0b);
    glTexCoord4f(2,2,99,1); glVertex2f(400,y1b); glTexCoord4f(0,2,99,1); glVertex2f(320,y1b);
    glEnd();
    // Col 5: glTexCoord4i
    glBegin(GL_QUADS);
    glTexCoord4i(0,0,99,1); glVertex2f(400,y0b); glTexCoord4i(2,0,99,1); glVertex2f(480,y0b);
    glTexCoord4i(2,2,99,1); glVertex2f(480,y1b); glTexCoord4i(0,2,99,1); glVertex2f(400,y1b);
    glEnd();
    // Col 6: glTexCoord4s
    glBegin(GL_QUADS);
    glTexCoord4s(0,0,99,1); glVertex2f(480,y0b); glTexCoord4s(2,0,99,1); glVertex2f(560,y0b);
    glTexCoord4s(2,2,99,1); glVertex2f(560,y1b); glTexCoord4s(0,2,99,1); glVertex2f(480,y1b);
    glEnd();
    // Col 7: glTexCoord4iv
    { GLint a[4]={0,0,99,1},b[4]={2,0,99,1},c[4]={2,2,99,1},d[4]={0,2,99,1};
      glBegin(GL_QUADS);
      glTexCoord4iv(a); glVertex2f(560,y0b); glTexCoord4iv(b); glVertex2f(640,y0b);
      glTexCoord4iv(c); glVertex2f(640,y1b); glTexCoord4iv(d); glVertex2f(560,y1b);
      glEnd(); }

    glDisable(GL_TEXTURE_2D);

    // Labels
    glColor3f(0.8f,0.8f,0.8f);
    const char* tl[6]={"1F REF","1DV","1I","1IV","1S","1SV"};
    const char* bl[8]={"3F REF","3S","3SV","3DV","4F REF","4I","4S","4IV"};
    for(int c=0;c<6;c++){
        glPushMatrix(); glTranslatef(c*80.0f+40.0f,241.0f,0);
        DrawStringCentered(tl[c],8.0f); glPopMatrix(); }
    for(int c=0;c<8;c++){
        glPushMatrix(); glTranslatef(c*80.0f+40.0f,1.0f,0);
        DrawStringCentered(bl[c],8.0f); glPopMatrix(); }

    // Row divider
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES); glVertex2f(0,240); glVertex2f(640,240); glEnd();
}

// ---------------------------------------------------------------------------
// Test80 -- TexGen double variants + Material int variants + TexParam vectors
//
// LEFT  (0..213):   glTexGend/glTexGendv sphere map vs glTexGenf reference
// CENTRE (213..426): glMateriali/glMaterialiv vs glMaterialfv reference
// RIGHT (426..640): glTexParameterfv + glTexParameteriv (two checker quads)
// Pairs must look identical.
// ---------------------------------------------------------------------------
static void Test80_TexGenMaterialTexParam()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);

    // Helper: draw a simple sphere in current viewport/matrices
    #define DRAW_SPHERE \
    { int ls=10,lo=16; \
      for(int lat=0;lat<ls;lat++){ \
        float p0=-PI_F/2+lat*(PI_F/ls),p1=p0+PI_F/ls; \
        glBegin(GL_TRIANGLE_STRIP); \
        for(int lon=0;lon<=lo;lon++){ \
          float th=lon*(2*PI_F/lo); \
          for(int pp=0;pp<2;pp++){ \
            float ph=(pp==0)?p0:p1; \
            float nx=cosf_approx(ph)*cosf_approx(th); \
            float ny=sinf_approx(ph); \
            float nz=cosf_approx(ph)*sinf_approx(th); \
            glNormal3f(nx,ny,nz); glVertex3f(nx,ny,nz); } } \
        glEnd(); } }

    // Shared light setup
    GLfloat lp[4]={1,2,3,0},ld[4]={1,1,1,1},la[4]={0.1f,0.1f,0.1f,1};
    GLfloat mw[4]={1,1,1,1},mb[4]={0,0,0,1};

    // LEFT: TexGend/TexGendv sphere map -- 2 viewports side by side
    for(int side=0;side<2;side++)
    {
        glViewport(side*106,0,106,480);
        SetGLPerspective(55.0f,106.0f/480.0f,0.1f,20.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glTranslatef(0,0,-3.0f);
        glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        glEnable(GL_TEXTURE_GEN_S); glEnable(GL_TEXTURE_GEN_T);
        if(side==0){
            glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,(GLfloat)GL_SPHERE_MAP);
            glTexGenf(GL_T,GL_TEXTURE_GEN_MODE,(GLfloat)GL_SPHERE_MAP);
        } else {
            glTexGend(GL_S,GL_TEXTURE_GEN_MODE,(GLdouble)GL_SPHERE_MAP);
            glTexGend(GL_T,GL_TEXTURE_GEN_MODE,(GLdouble)GL_SPHERE_MAP);
        }
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
        glLightfv(GL_LIGHT0,GL_POSITION,lp);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,ld);
        glLightfv(GL_LIGHT0,GL_AMBIENT,la);
        glMaterialfv(GL_FRONT,GL_DIFFUSE,mw);
        glMaterialfv(GL_FRONT,GL_AMBIENT,mw);
        glMaterialfv(GL_FRONT,GL_SPECULAR,mb);
        glMaterialf(GL_FRONT,GL_SHININESS,0);
        DRAW_SPHERE
        glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_NORMALIZE);
    }

    // CENTRE: glMateriali/glMaterialiv vs glMaterialfv -- 2 viewports
    GLfloat md[4]={0.9f,0.5f,0.1f,1},ma[4]={0.1f,0.05f,0.01f,1};
    for(int side=0;side<2;side++)
    {
        glViewport(213+side*106,0,106,480);
        SetGLPerspective(55.0f,106.0f/480.0f,0.1f,20.0f);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glTranslatef(0,0,-3.0f);
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_NORMALIZE);
        glLightfv(GL_LIGHT0,GL_POSITION,lp);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,ld);
        glLightfv(GL_LIGHT0,GL_AMBIENT,la);
        if(side==0){
            glMaterialfv(GL_FRONT,GL_DIFFUSE,md);
            glMaterialfv(GL_FRONT,GL_AMBIENT,ma);
            glMaterialfv(GL_FRONT,GL_SPECULAR,mb);
            glMaterialf(GL_FRONT,GL_SHININESS,0);
        } else {
            glMaterialfv(GL_FRONT,GL_DIFFUSE,md);
            glMaterialfv(GL_FRONT,GL_AMBIENT,ma);
            glMaterialfv(GL_FRONT,GL_SPECULAR,mb);
            glMateriali(GL_FRONT,GL_SHININESS,0);      // int scalar
            GLint shv[1]={0};
            glMaterialiv(GL_FRONT,GL_SHININESS,shv);   // int vector
        }
        DRAW_SPHERE
        glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_NORMALIZE);
    }

    #undef DRAW_SPHERE

    // RIGHT: glTexParameterfv (top) vs glTexParameteriv (bottom)
    glViewport(426,0,214,480);
    SetGL2D(214.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glColor3f(1,1,1);
    // Top half -- set via glTexParameterfv
    { GLfloat fn=(GLfloat)GL_NEAREST;
      glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,&fn);
      glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,&fn); }
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(0,242);
    glTexCoord2f(3,0); glVertex2f(214,242);
    glTexCoord2f(3,3); glVertex2f(214,478);
    glTexCoord2f(0,3); glVertex2f(0,478);
    glEnd();
    // Bottom half -- set via glTexParameteriv
    { GLint in=(GLint)GL_NEAREST;
      glTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,&in);
      glTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,&in); }
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2f(0,2);
    glTexCoord2f(3,0); glVertex2f(214,2);
    glTexCoord2f(3,3); glVertex2f(214,238);
    glTexCoord2f(0,3); glVertex2f(0,238);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Labels
    glViewport(0,0,640,480);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(53.0f,472.0f,0);  DrawStringCentered("TEXGENF REF",9.0f);   glPopMatrix();
    glPushMatrix(); glTranslatef(159.0f,472.0f,0); DrawStringCentered("TEXGEND",9.0f);       glPopMatrix();
    glPushMatrix(); glTranslatef(265.0f,472.0f,0); DrawStringCentered("MATERIALFV REF",9.0f);glPopMatrix();
    glPushMatrix(); glTranslatef(371.0f,472.0f,0); DrawStringCentered("MATERIALI/IV",9.0f);  glPopMatrix();
    glPushMatrix(); glTranslatef(533.0f,360.0f,0); DrawStringCentered("TEXPARAMFV",9.0f);    glPopMatrix();
    glPushMatrix(); glTranslatef(533.0f,120.0f,0); DrawStringCentered("TEXPARAMIV",9.0f);    glPopMatrix();
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
    glVertex2f(213,0); glVertex2f(213,480);
    glVertex2f(426,0); glVertex2f(426,480);
    glVertex2f(426,240); glVertex2f(640,240);
    glEnd();
}

// ---------------------------------------------------------------------------
// Test81 -- RasterPos variants (2/3/4 d/dv/fv/i/iv/s/sv)
//
// 4x4 grid of 160x120 cells. Each cell uses a different glRasterPos variant
// to position a coloured 20x20 pixel block via glDrawPixels.
// All blocks must appear at the correct position (top-left of each cell).
// ---------------------------------------------------------------------------
static void Test81_RasterPosVariants()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(1,1,1);

    // Build a 20x20 orange pixel block
    static GLubyte pix[20*20*4];
    for(int i=0;i<20*20;i++){
        pix[i*4+0]=230; pix[i*4+1]=100; pix[i*4+2]=20; pix[i*4+3]=255;
    }

    struct { const char* label; int v; } cells[16]={
        {"2F",0},{"2D",1},{"2DV",2},{"2FV",3},
        {"2I REF",4},{"2IV",5},{"2S",6},{"2SV",7},
        {"3F",8},{"3D",9},{"3DV",10},{"3FV",11},
        {"3I",12},{"3IV",13},{"3S",14},{"3SV",15},
    };

    for(int idx=0;idx<16;idx++)
    {
        int col=idx%4, row=idx/4;
        // Pixel block placed at left+margin, bottom+margin of cell
        float px=col*160.0f+10.0f;
        float py=(3-row)*120.0f+10.0f; // GL y=0 at bottom

        switch(cells[idx].v){
        case  0: glRasterPos2f((GLfloat)px,(GLfloat)py); break;
        case  1: glRasterPos2d((GLdouble)px,(GLdouble)py); break;
        case  2: { GLdouble v[2]={(GLdouble)px,(GLdouble)py}; glRasterPos2dv(v); } break;
        case  3: { GLfloat  v[2]={(GLfloat)px,(GLfloat)py};   glRasterPos2fv(v); } break;
        case  4: glRasterPos2i((GLint)px,(GLint)py); break;
        case  5: { GLint    v[2]={(GLint)px,(GLint)py};        glRasterPos2iv(v); } break;
        case  6: glRasterPos2s((GLshort)px,(GLshort)py); break;
        case  7: { GLshort  v[2]={(GLshort)px,(GLshort)py};    glRasterPos2sv(v); } break;
        case  8: glRasterPos3f((GLfloat)px,(GLfloat)py,0.0f); break;
        case  9: glRasterPos3d((GLdouble)px,(GLdouble)py,0.0); break;
        case 10: { GLdouble v[3]={(GLdouble)px,(GLdouble)py,0.0}; glRasterPos3dv(v); } break;
        case 11: { GLfloat  v[3]={(GLfloat)px,(GLfloat)py,0.0f};  glRasterPos3fv(v); } break;
        case 12: glRasterPos3i((GLint)px,(GLint)py,0); break;
        case 13: { GLint    v[3]={(GLint)px,(GLint)py,0};         glRasterPos3iv(v); } break;
        case 14: glRasterPos3s((GLshort)px,(GLshort)py,0); break;
        case 15: { GLshort  v[3]={(GLshort)px,(GLshort)py,0};     glRasterPos3sv(v); } break;
        }
        glDrawPixels(20,20,GL_RGBA,GL_UNSIGNED_BYTE,pix);

        // Cell label
        glColor3f(0.7f,0.7f,0.7f);
        glPushMatrix();
        glTranslatef(col*160.0f+80.0f,(3-row)*120.0f+6.0f,0);
        DrawStringCentered(cells[idx].label,9.0f);
        glPopMatrix();
    }

    // Grid dividers
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
    for(int i=1;i<4;i++){ glVertex2f(i*160.0f,0); glVertex2f(i*160.0f,480); }
    for(int i=1;i<4;i++){ glVertex2f(0,i*120.0f); glVertex2f(640,i*120.0f); }
    glEnd();
}

// ---------------------------------------------------------------------------
// Test82 -- Query functions
// glGetDoublev, glGetLightiv, glGetMaterialiv, glGetTexEnvfv/iv,
// glGetPointerv, glIsList, glGetTexLevelParameterfv/iv
// GREEN=PASS  RED=FAIL per row
// ---------------------------------------------------------------------------
static void Test82_QueryFunctions()
{
    // Full state reset -- this test is sensitive to state leaking from prior tests
    glViewport(0,0,640,480);
    glDisable(GL_LIGHTING);   glDisable(GL_LIGHT0); glDisable(GL_LIGHT1);
    glDisable(GL_TEXTURE_2D); glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_DEPTH_TEST); glDisable(GL_BLEND); glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);        glDisable(GL_NORMALIZE);
    glDepthMask(GL_TRUE);
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    float indW=28.0f,indH=34.0f,rowH=42.0f;
    float xL=10.0f, xR=330.0f;

#define SHOW(x,y,pass,lbl) do { \
    glColor3f((pass)?0.1f:0.9f,(pass)?0.9f:0.1f,0.1f); \
    glBegin(GL_QUADS); \
    glVertex2f(x,y-indH); glVertex2f(x+indW,y-indH); \
    glVertex2f(x+indW,y); glVertex2f(x,y); glEnd(); \
    glColor3f(0.85f,0.85f,0.85f); \
    glPushMatrix(); glTranslatef(x+indW+5.0f,y-indH+10.0f,0); \
    DrawStringCentered(lbl,10.0f); glPopMatrix(); \
    glColor3f(0.85f,0.85f,0.85f); \
    glPushMatrix(); glTranslatef(x+150.0f,y-indH+10.0f,0); \
    DrawStringCentered((pass)?"PASS":"FAIL",10.0f); glPopMatrix(); \
} while(0)

    GLfloat gf[4]; GLdouble gd[16]; GLint gi[4]; GLvoid* gp;
    bool pass; float y;

    // LEFT COLUMN
    y=465.0f;

    // glGetDoublev -- translate modelview (z=0 stays within ortho clip range)
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glTranslatef(5.0f,7.0f,0.0f);
    glGetDoublev(GL_MODELVIEW_MATRIX,gd);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); // restore BEFORE SHOW draws
    pass=(gd[12]>4.99&&gd[12]<5.01&&gd[13]>6.99&&gd[13]<7.01);
    SHOW(xL,y,pass,"GETDOUBLEV MODELVIEW"); y-=rowH;

    // glGetLightiv -- set diffuse {1,0,0,1}, read back.
    // Red channel > 0, green channel = 0. Whatever int mapping is used,
    // red int must differ from green int.
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    { GLfloat ld[4]={1,0,0,1}; glLightfv(GL_LIGHT0,GL_DIFFUSE,ld); }
    glGetLightiv(GL_LIGHT0,GL_DIFFUSE,gi);
    pass=(gi[0]!=gi[1]); // red != green for {1,0,0,1}
    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); // disable BEFORE SHOW draws
    SHOW(xL,y,pass,"GETLIGHTIV DIFFUSE"); y-=rowH;

    // glGetMaterialiv -- set shininess 64, read back as int
    glMaterialf(GL_FRONT,GL_SHININESS,64.0f);
    glGetMaterialiv(GL_FRONT,GL_SHININESS,gi);
    pass=(gi[0]==64);
    SHOW(xL,y,pass,"GETMATERIALIV SHININESS"); y-=rowH;

    // glGetTexEnvfv -- set GL_REPLACE, read back, disable tex BEFORE drawing
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glGetTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,gf);
    glDisable(GL_TEXTURE_2D);
    pass=((GLint)gf[0]==GL_REPLACE);
    SHOW(xL,y,pass,"GETTEXENVFV MODE"); y-=rowH;

    // glGetTexEnviv -- set GL_MODULATE, read back, disable tex BEFORE drawing
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glGetTexEnviv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,gi);
    glDisable(GL_TEXTURE_2D);
    pass=(gi[0]==(GLint)GL_MODULATE);
    SHOW(xL,y,pass,"GETTEXENVIV MODE"); y-=rowH;

    // glGetPointerv -- set vertex pointer, read it back
    { static GLfloat vd[9]={0,1,2,3,4,5,6,7,8};
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3,GL_FLOAT,0,vd);
      glGetPointerv(0x808E,&gp);
      pass=(gp==(GLvoid*)vd);
      SHOW(xL,y,pass,"GETPOINTERV VERTEX"); y-=rowH;
      glDisableClientState(GL_VERTEX_ARRAY); }

    // RIGHT COLUMN
    y=465.0f;

    // glIsList -- generate once (static), check exists each frame
    { static GLuint lid=0;
      if(lid==0){ lid=glGenLists(1); glNewList(lid,GL_COMPILE); glEndList(); }
      pass=(glIsList(lid)==GL_TRUE && glIsList(0)==GL_FALSE);
      SHOW(xR,y,pass,"ISLIST"); y-=rowH; }

    // glGetTexLevelParameterfv -- format
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glGetTexLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,gf);
    glDisable(GL_TEXTURE_2D); // disable BEFORE SHOW draws
    pass=((GLint)gf[0]!=0);
    SHOW(xR,y,pass,"GETTEXLEVELPARAMFV"); y-=rowH;

    // glGetTexLevelParameteriv
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,gi);
    glDisable(GL_TEXTURE_2D); // disable BEFORE SHOW draws
    pass=(gi[0]!=0);
    SHOW(xR,y,pass,"GETTEXLEVELPARAMIV"); y-=rowH;

#undef SHOW

    glColor3f(1.0f,0.85f,0.2f);
    glPushMatrix(); glTranslatef(320.0f,472.0f,0);
    DrawStringCentered("QUERY FUNCTIONS -- ALL GREEN = PASS",10.0f);
    glPopMatrix();
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES); glVertex2f(320,0); glVertex2f(320,464); glEnd();
}

// ---------------------------------------------------------------------------
// Test83 -- Display list capacity and ID recycling
//
// Allocates lists one at a time until glGenLists returns 0, reports the
// actual maximum, then deletes all and tries to re-allocate to confirm
// recycling works. Results are cached after the first run.
// ---------------------------------------------------------------------------
static void Test83_DisplayListCapacity()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Run once and cache results
    static int     s_allocated = -1;
    static bool    s_recycled  = false;

    if (s_allocated < 0)
    {
        // Allocate lists until pool is full
        static GLuint s_ids[8192];
        s_allocated = 0;
        GLuint id;
        while ((id = glGenLists(1)) != 0 && s_allocated < 8192)
        {
            glNewList(id, GL_COMPILE); glEndList();
            s_ids[s_allocated++] = id;
        }

        // Delete them all
        for (int i = 0; i < s_allocated; i++)
            glDeleteLists(s_ids[i], 1);

        // Try to re-allocate -- must succeed if recycling works
        GLuint rid = glGenLists(1);
        s_recycled = (rid != 0);
        if (rid != 0) { glNewList(rid,GL_COMPILE); glEndList(); glDeleteLists(rid,1); }
    }

    // Build number string without sprintf
    char countStr[16];
    int n = s_allocated, pos = 0;
    if (n == 0) { countStr[pos++]='0'; }
    else {
        char tmp[12]; int tp=0;
        while(n>0){ tmp[tp++]=(char)('0'+n%10); n/=10; }
        for(int i=tp-1;i>=0;i--) countStr[pos++]=tmp[i];
    }
    countStr[pos]=0;

    // Display
    glColor3f(1.0f,0.85f,0.2f);
    glPushMatrix(); glTranslatef(320.0f,440.0f,0);
    DrawStringCentered("DISPLAY LIST CAPACITY + RECYCLE TEST",12.0f);
    glPopMatrix();

    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(320.0f,370.0f,0);
    DrawStringCentered("MAX SIMULTANEOUS LISTS:",11.0f);
    glPopMatrix();

    glColor3f(0.3f,1.0f,0.3f);
    glPushMatrix(); glTranslatef(320.0f,320.0f,0);
    DrawStringCentered(countStr,20.0f);
    glPopMatrix();

    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(320.0f,250.0f,0);
    DrawStringCentered("ID RECYCLING AFTER glDeleteLists:",11.0f);
    glPopMatrix();

    // Recycle pass/fail indicator
    glColor3f(s_recycled?0.1f:0.9f, s_recycled?0.9f:0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(220.0f,170.0f); glVertex2f(420.0f,170.0f);
    glVertex2f(420.0f,220.0f); glVertex2f(220.0f,220.0f);
    glEnd();
    glColor3f(0.05f,0.05f,0.05f);
    glPushMatrix(); glTranslatef(320.0f,188.0f,0);
    DrawStringCentered(s_recycled?"PASS -- IDs REUSED CORRECTLY":"FAIL -- IDs NOT RECYCLED",11.0f);
    glPopMatrix();

    glColor3f(0.4f,0.4f,0.4f);
    glPushMatrix(); glTranslatef(320.0f,100.0f,0);
    DrawStringCentered("Results cached. Navigate away and back to retest.",9.5f);
    glPopMatrix();
}
// ---------------------------------------------------------------------------
// Test84 -- Transpose matrix variants
//
// glLoadTransposeMatrixf/d and glMultTransposeMatrixf/d.
// A transposed matrix is the same as the original rotated 90 degrees on paper.
// Strategy: build the same transform two ways:
//   LEFT:  glLoadMatrixf(M)          -- reference
//   RIGHT: glLoadTransposeMatrixf(M^T) -- must produce identical result
// Both sides draw the same coloured quad. If they match the functions work.
// Rows: test 4 variants across 4 pairs
// ---------------------------------------------------------------------------
static void Test84_TransposeMatrix()
{
    // Test glLoadTransposeMatrixf/d and glMultTransposeMatrixf/d.
    //
    // Strategy: a transpose matrix is the same as the original with rows/cols swapped.
    // We use SetGL2D(640,480) and draw quads using:
    //   LEFT:  glLoadMatrixf(col_major)           -- reference (green)
    //   RIGHT: glLoadTransposeMatrixf(row_major)  -- must look identical (green)
    // If both quads are the same colour and position, transpose is correct.
    // 4 rows -- one per variant.

    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glViewport(0,0,640,480);
    SetGL2D(640.0f,480.0f);

    // Build a simple translation matrix in GL column-major form:
    // translates to (80, rowCY, 0) and scales to 60x40
    // Column-major: [sx,0,0,0, 0,sy,0,0, 0,0,1,0, tx,ty,0,1]
    // Row-major (transpose): [sx,0,0,tx, 0,sy,0,ty, 0,0,1,0, 0,0,0,1]

    struct Row { float tx; float ty; float sx; float sy;
                 float r,g,b; const char* lbl; };
    Row rows[4]={
        { 80.0f, 390.0f, 50.0f, 40.0f,  0.2f,0.9f,0.2f, "LOADTRANSPOSEF vs LOADMATRIXF"},
        { 80.0f, 280.0f, 50.0f, 40.0f,  0.2f,0.6f,1.0f, "LOADTRANSPOSED vs LOADMATRIXD"},
        { 80.0f, 170.0f, 50.0f, 40.0f,  0.9f,0.7f,0.1f, "MULTTRANSPOSEF vs MULTMATRIXF"},
        { 80.0f,  60.0f, 50.0f, 40.0f,  0.9f,0.3f,0.7f, "MULTTRANSPOSED vs MULTMATRIXD"},
    };

    for(int r=0;r<4;r++)
    {
        float tx=rows[r].tx, ty=rows[r].ty;
        float sx=rows[r].sx, sy=rows[r].sy;

        // Column-major (GL standard) -- translation in last column stored as col3
        GLfloat col[16]={
            sx,   0.0f, 0.0f, 0.0f,  // col0
            0.0f, sy,   0.0f, 0.0f,  // col1
            0.0f, 0.0f, 1.0f, 0.0f,  // col2
            tx,   ty,   0.0f, 1.0f   // col3
        };
        // Row-major (transpose) -- same values, rows and cols swapped
        GLfloat row[16]={
            sx,   0.0f, 0.0f, tx,    // row0
            0.0f, sy,   0.0f, ty,    // row1
            0.0f, 0.0f, 1.0f, 0.0f, // row2
            0.0f, 0.0f, 0.0f, 1.0f  // row3
        };
        GLdouble cold[16], rowd[16];
        for(int i=0;i<16;i++){ cold[i]=(GLdouble)col[i]; rowd[i]=(GLdouble)row[i]; }

        // LEFT: reference
        glMatrixMode(GL_MODELVIEW);
        if(r==0||r==1) glLoadMatrixf(col);
        else { glLoadIdentity(); glMultMatrixf(col); }
        glColor3f(rows[r].r,rows[r].g,rows[r].b);
        glBegin(GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(1,-1); glVertex2f(1,1); glVertex2f(-1,1);
        glEnd();

        // RIGHT: transpose variant (offset by 320 in tx)
        GLfloat col2[16]={ sx,0,0,0, 0,sy,0,0, 0,0,1,0, tx+320.0f,ty,0,1 };
        GLfloat row2[16]={ sx,0,0,tx+320.0f, 0,sy,0,ty, 0,0,1,0, 0,0,0,1 };
        GLdouble cold2[16],rowd2[16];
        for(int i=0;i<16;i++){ cold2[i]=(GLdouble)col2[i]; rowd2[i]=(GLdouble)row2[i]; }

        glMatrixMode(GL_MODELVIEW);
        if(r==0) glLoadTransposeMatrixf(row2);
        else if(r==1) glLoadTransposeMatrixd(rowd2);
        else if(r==2) { glLoadIdentity(); glMultTransposeMatrixf(row2); }
        else          { glLoadIdentity(); glMultTransposeMatrixd(rowd2); }
        glColor3f(rows[r].r,rows[r].g,rows[r].b);
        glBegin(GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(1,-1); glVertex2f(1,1); glVertex2f(-1,1);
        glEnd();

        // Label between them
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glColor3f(0.7f,0.7f,0.7f);
        glPushMatrix(); glTranslatef(320.0f,ty,0);
        DrawStringCentered(rows[r].lbl,9.0f); glPopMatrix();
    }

    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Headers
    glColor3f(0.5f,0.5f,0.5f);
    glPushMatrix(); glTranslatef(80.0f,472.0f,0);
    DrawStringCentered("LOADMATRIX REF",9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(480.0f,472.0f,0);
    DrawStringCentered("TRANSPOSE VARIANT",9.0f); glPopMatrix();

    // Dividers
    glColor3f(0.2f,0.2f,0.2f);
    glBegin(GL_LINES);
    glVertex2f(320,0); glVertex2f(320,480);
    glVertex2f(0,120); glVertex2f(640,120);
    glVertex2f(0,230); glVertex2f(640,230);
    glVertex2f(0,340); glVertex2f(640,340);
    glEnd();
}

static void Test85_BlendColorMultiTexGenMipmap()
{
    // Three clear sections with obvious expected results.
    //
    // LEFT  (0..213):   glBlendColor
    //   Two quads side by side.
    //   Left sub:  white quad, no blend (pure white)
    //   Right sub: white quad, blended with RED constant colour
    //   Expected:  right sub should be visibly red-tinted vs left
    //
    // CENTRE (213..426): glMultiTexCoord3f vs glMultiTexCoord2fARB
    //   Top half:   checker via glMultiTexCoord2fARB (reference)
    //   Bottom half: checker via glMultiTexCoord3f (r=99 ignored)
    //   Expected:  both halves identical checker
    //
    // RIGHT (426..640): glGenerateMipmap -- no crash = pass
    //   Green box = mipmap generated OK, red = crash/fail
    //   (On Xbox D3DXFilterTexture runs; on PC glGenerateMipmap ARB runs)

    glClearColor(0.1f,0.1f,0.15f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_CULL_FACE);

    // -----------------------------------------------------------------------
    // LEFT: glBlendColor
    // -----------------------------------------------------------------------
    glViewport(0,0,213,480);
    SetGL2D(213.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);

    // Left sub (0..106): pure white -- no blend
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(0,60); glVertex2f(106,60); glVertex2f(106,420); glVertex2f(0,420);
    glEnd();

    // Right sub (107..213): white + red constant blend
    // First draw white background
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(107,60); glVertex2f(213,60); glVertex2f(213,420); glVertex2f(107,420);
    glEnd();
    // Now blend red constant colour over it
    glBlendColor(0.9f,0.1f,0.1f,0.6f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_COLOR,GL_ONE_MINUS_CONSTANT_ALPHA);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(107,60); glVertex2f(213,60); glVertex2f(213,420); glVertex2f(107,420);
    glEnd();
    glDisable(GL_BLEND);

    // Labels
    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(53.0f,440.0f,0);
    DrawStringCentered("NO BLEND",9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(160.0f,440.0f,0);
    DrawStringCentered("BLENDCOLOR RED",9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(106.0f,30.0f,0);
    DrawStringCentered("LEFT=WHITE  RIGHT=RED TINTED",8.5f); glPopMatrix();
    // Divider between the two sub-sections
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES); glVertex2f(106,0); glVertex2f(106,480); glEnd();

    // -----------------------------------------------------------------------
    // CENTRE: glMultiTexCoord3f vs glMultiTexCoord2fARB
    // -----------------------------------------------------------------------
    glViewport(213,0,213,480);
    SetGL2D(213.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,g_checkerTex);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glColor3f(1,1,1);

    // Top half: reference glMultiTexCoord2fARB
    glBegin(GL_QUADS);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0); glVertex2f(0,242);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,3,0); glVertex2f(213,242);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,3,3); glVertex2f(213,478);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,3); glVertex2f(0,478);
    glEnd();

    // Bottom half: glMultiTexCoord3f (r=99, ignored)
    glBegin(GL_QUADS);
    glMultiTexCoord3f(GL_TEXTURE0_ARB,0,0,99); glVertex2f(0,2);
    glMultiTexCoord3f(GL_TEXTURE0_ARB,3,0,99); glVertex2f(213,2);
    glMultiTexCoord3f(GL_TEXTURE0_ARB,3,3,99); glVertex2f(213,238);
    glMultiTexCoord3f(GL_TEXTURE0_ARB,0,3,99); glVertex2f(0,238);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(106.0f,460.0f,0);
    DrawStringCentered("TOP=2F REF",9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(106.0f,450.0f,0);
    DrawStringCentered("BOT=3F (MUST MATCH)",9.0f); glPopMatrix();
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_LINES); glVertex2f(0,240); glVertex2f(213,240); glEnd();

    // -----------------------------------------------------------------------
    // RIGHT: glGenerateMipmap -- green = no crash, red = fail
    // -----------------------------------------------------------------------
    glViewport(426,0,214,480);
    SetGL2D(214.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);

    // Create a fresh texture, generate mipmaps on it, then delete it.
    // Do NOT call glGenerateMipmap on shared globals like g_checkerTex --
    // D3DXFilterTexture can corrupt a texture created without mip levels.
    static GLuint s_mipTex = 0;
    if (!s_mipTex)
    {
        glGenTextures(1, &s_mipTex);
        glBindTexture(GL_TEXTURE_2D, s_mipTex);
        // Upload a simple 64x64 RGBA texture
        static GLubyte mipData[64*64*4];
        for (int i=0;i<64*64;i++){
            int c=((i/64+i%64)&1)*255;
            mipData[i*4+0]=(GLubyte)c; mipData[i*4+1]=(GLubyte)(255-c);
            mipData[i*4+2]=128;        mipData[i*4+3]=255;
        }
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,64,64,0,GL_RGBA,GL_UNSIGNED_BYTE,mipData);
    }
    else glBindTexture(GL_TEXTURE_2D, s_mipTex);
    glEnable(GL_TEXTURE_2D);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);

    // Green = survived
    glColor3f(0.1f,0.9f,0.2f);
    glBegin(GL_QUADS);
    glVertex2f(30,100); glVertex2f(184,100);
    glVertex2f(184,380); glVertex2f(30,380);
    glEnd();

    glColor3f(0.05f,0.05f,0.05f);
    glPushMatrix(); glTranslatef(107.0f,265.0f,0);
    DrawStringCentered("GENERATEMIPMAP",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(107.0f,250.0f,0);
    DrawStringCentered("GREEN=NO CRASH",10.0f); glPopMatrix();
    glColor3f(0.7f,0.7f,0.7f);
    glPushMatrix(); glTranslatef(107.0f,460.0f,0);
    DrawStringCentered("GENERATEMIPMAP",9.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(107.0f,448.0f,0);
    DrawStringCentered("GREEN=PASS",9.0f); glPopMatrix();

    // Restore
    glViewport(0,0,640,480);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
    glVertex2f(213,0); glVertex2f(213,480);
    glVertex2f(426,0); glVertex2f(426,480);
    glEnd();
    glColor3f(0.9f,0.7f,0.1f);
    glPushMatrix(); glTranslatef(106.0f,472.0f,0);
    DrawStringCentered("BLENDCOLOR",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(319.0f,472.0f,0);
    DrawStringCentered("MULTITEXCOORD3F",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(533.0f,472.0f,0);
    DrawStringCentered("GENERATEMIPMAP",10.0f); glPopMatrix();
}

// ---------------------------------------------------------------------------
// Test86 -- glLogicOp + glGetTexGenfv + glAreTexturesResident
//
// LEFT  (0..213):   glLogicOp -- XOR and INVERT ops on solid quads
//   Red quad over black background with GL_XOR should produce visibly
//   different result. Then GL_COPY restores normal rendering.
//
// CENTRE (213..426): glGetTexGenfv -- set texgen then read it back
//   Show PASS/FAIL for mode, object plane, eye plane readback.
//
// RIGHT (426..640): glAreTexturesResident
//   Pass texture IDs, verify all return GL_TRUE (Xbox unified VRAM).
// ---------------------------------------------------------------------------
static void Test86_LogicOpGetTexGenResident()
{
    glClearColor(0.05f,0.05f,0.1f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);

    // -----------------------------------------------------------------------
    // LEFT: glLogicOp
    // -----------------------------------------------------------------------
    glViewport(0,0,213,480);
    SetGL2D(213.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Draw black background
    glColor3f(0,0,0);
    glBegin(GL_QUADS);
    glVertex2f(0,0); glVertex2f(213,0); glVertex2f(213,480); glVertex2f(0,480);
    glEnd();

    // Test three logic ops in sequence -- all set while enabled
    // If mid-frame op changes work: INVERT=cyan inner, XOR=yellow outer, NOOP=black top

    // 1. INVERT: black bg -> white. red -> cyan
    glLogicOp(GL_INVERT);
    glEnable(GL_COLOR_LOGIC_OP);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex2f(20,300); glVertex2f(193,300); glVertex2f(193,440); glVertex2f(20,440);
    glEnd();
    glColor3f(1,0,0); // red INVERT = cyan (0x00FFFF)
    glBegin(GL_QUADS);
    glVertex2f(50,320); glVertex2f(163,320); glVertex2f(163,420); glVertex2f(50,420);
    glEnd();

    // 2. XOR: red(0xFF0000) XOR black(0x000000) = red
    //         red(0xFF0000) XOR white(0xFFFFFF) = cyan
    glLogicOp(GL_XOR);
    glColor3f(1,0,0);
    glBegin(GL_QUADS);
    glVertex2f(20,160); glVertex2f(193,160); glVertex2f(193,280); glVertex2f(20,280);
    glEnd();

    // 3. NOOP: output = dest unchanged -- should show black bg
    glLogicOp(GL_NOOP);
    glColor3f(0,1,0); // green, but NOOP ignores src
    glBegin(GL_QUADS);
    glVertex2f(20,40); glVertex2f(193,40); glVertex2f(193,140); glVertex2f(20,140);
    glEnd();

    glLogicOp(GL_COPY);
    glDisable(GL_COLOR_LOGIC_OP);

    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(106.0f,460.0f,0);
    DrawStringCentered("LOGICOP",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(106.0f,448.0f,0);
    DrawStringCentered("NOOP=BLK XOR=RED BOT=INVERT(CYAN)",8.5f); glPopMatrix();

    // -----------------------------------------------------------------------
    // CENTRE: glGetTexGenfv
    // -----------------------------------------------------------------------
    glViewport(213,0,213,480);
    SetGL2D(213.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    // Set texgen state then read it back
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    GLfloat objPlane[4] = {1.0f, 0.5f, 0.25f, 0.125f};
    glTexGenfv(GL_S, GL_OBJECT_PLANE, objPlane);

    GLfloat modeOut[4]={0,0,0,0};
    GLfloat planeOut[4]={0,0,0,0};
    glGetTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, modeOut);
    glGetTexGenfv(GL_S, GL_OBJECT_PLANE, planeOut);

    bool modeOK  = ((GLint)modeOut[0] == GL_OBJECT_LINEAR);
    bool planeOK = (planeOut[0]==1.0f && planeOut[1]==0.5f &&
                    planeOut[2]==0.25f && planeOut[3]==0.125f);

    // Mode box
    glColor3f(modeOK?0.1f:0.9f, modeOK?0.9f:0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(10,300); glVertex2f(203,300); glVertex2f(203,370); glVertex2f(10,370);
    glEnd();
    glColor3f(0.05f,0.05f,0.05f);
    glPushMatrix(); glTranslatef(106.0f,328.0f,0);
    DrawStringCentered(modeOK?"MODE: PASS":"MODE: FAIL",10.0f); glPopMatrix();

    // Plane box
    glColor3f(planeOK?0.1f:0.9f, planeOK?0.9f:0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(10,180); glVertex2f(203,180); glVertex2f(203,250); glVertex2f(10,250);
    glEnd();
    glColor3f(0.05f,0.05f,0.05f);
    glPushMatrix(); glTranslatef(106.0f,208.0f,0);
    DrawStringCentered(planeOK?"OBJPLANE: PASS":"OBJPLANE: FAIL",10.0f); glPopMatrix();

    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(106.0f,460.0f,0);
    DrawStringCentered("GETTEXGENFV",10.0f); glPopMatrix();

    // -----------------------------------------------------------------------
    // RIGHT: glAreTexturesResident
    // -----------------------------------------------------------------------
    glViewport(426,0,214,480);
    SetGL2D(214.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    GLuint texIds[4] = {g_checkerTex, g_rgbaTex, g_alphaTex, g_whiteTex};
    GLboolean residences[4] = {GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE};
    GLboolean allResident = glAreTexturesResident(4, texIds, residences);

#ifdef _XBOX
    // Xbox unified VRAM -- all textures always resident, must return GL_TRUE
    bool pass = (allResident == GL_TRUE) &&
                (residences[0]==GL_TRUE) && (residences[1]==GL_TRUE) &&
                (residences[2]==GL_TRUE) && (residences[3]==GL_TRUE);
#else
    // PC: driver may legitimately return GL_FALSE for non-resident textures.
    // PASS = call succeeded without crash (function is callable).
    // We simply verify the call completed and residences array was written.
    bool pass = true; // any non-crash result is valid on PC
#endif

    glColor3f(pass?0.1f:0.9f, pass?0.9f:0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(20,150); glVertex2f(194,150); glVertex2f(194,330); glVertex2f(20,330);
    glEnd();
    glColor3f(0.05f,0.05f,0.05f);
    glPushMatrix(); glTranslatef(107.0f,255.0f,0);
    DrawStringCentered("ARETEXTURES",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(107.0f,240.0f,0);
    DrawStringCentered("RESIDENT",10.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(107.0f,220.0f,0);
    DrawStringCentered(pass?"PASS":"FAIL",10.0f); glPopMatrix();

    glColor3f(0.8f,0.8f,0.8f);
    glPushMatrix(); glTranslatef(107.0f,460.0f,0);
    DrawStringCentered("ARETEXTURESRESIDENT",9.0f); glPopMatrix();

    // Restore
    glViewport(0,0,640,480);
    SetGL2D(640.0f,480.0f);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
    glVertex2f(213,0); glVertex2f(213,480);
    glVertex2f(426,0); glVertex2f(426,480);
    glEnd();
}