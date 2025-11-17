/////////////////////////////////////////////
///Wrapping and Filtering Texture 17 Nov 2025 v1.0
/////////////////////////////////////////////

// chapter5_best_demo.cpp
// The BEST version for Viewing + Texture demo (GLUT only)
// Tekan W = Wrapping mode toggle (REPEAT/CLAMP)
// Tekan F = Filtering mode toggle (NEAREST/LINEAR/MIPMAP)

#include <windows.h>
#include <GL/glut.h>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Globals
GLuint gTexture = 0;
float gAngle = 0;

int wrapMode = 0;   // 0=REPEAT, 1=CLAMP
int filterMode = 0; // 0=NEAREST, 1=LINEAR, 2=MIPMAP

// Camera controls
float camYaw = 45.0f;   // putar kiri/kanan (degree)
float camPitch = 25.0f;   // pandang atas/bawah (degree)
float camDist = 30.0f;   // jarak kamera dari pusat

// Mouse control
int  lastMouseX = 0;
int  lastMouseY = 0;
bool isDragging = false;

// -------------------------------------
// Load Texture using GLU Mipmaps
// -------------------------------------
GLuint LoadTextureJPEG(const char* file)
{
    int w, h, ch;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(file, &w, &h, &ch, 0);

    if (!data) {
        printf("Failed to load %s\n", file);
        return 0;
    }

    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Build mipmap
    gluBuild2DMipmaps(GL_TEXTURE_2D, format, w, h, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return tex;
}


// -------------------------------------
// Apply wrapping & filtering settings
// -------------------------------------
void ApplyTextureSettings()
{
    glBindTexture(GL_TEXTURE_2D, gTexture);

    // WRAPPING
    if (wrapMode == 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        printf("WRAP = GL_REPEAT\n");
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        printf("WRAP = GL_CLAMP\n");
    }

    // FILTERING
    if (filterMode == 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        printf("FILTER = NEAREST\n");
    }
    else if (filterMode == 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        printf("FILTER = LINEAR\n");
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        printf("FILTER = MIPMAP (GL_LINEAR_MIPMAP_LINEAR)\n");
    }
}


// -------------------------------------
void InitGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.15f, 0.15f, 0.2f, 1);

    gTexture = LoadTextureJPEG("mytexture.jpg");
    ApplyTextureSettings();
}


// -------------------------------------
void Reshape(int w, int h)
{
    if (h == 0) h = 1;
    float aspect = (float)w / h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspect, 1, 200);

    glMatrixMode(GL_MODELVIEW);
}


// -------------------------------------
// Draw Floor (best to show wrapping)
// -------------------------------------
void DrawFloorPlane()
{
    glBindTexture(GL_TEXTURE_2D, gTexture);

    glBegin(GL_QUADS);

    // Increase texcoord (0→12) to show REPEAT
    glTexCoord2f(0, 0);  glVertex3f(-20, -5, -20);
    glTexCoord2f(12, 0); glVertex3f(20, -5, -20);
    glTexCoord2f(12, 12); glVertex3f(20, -5, 20);
    glTexCoord2f(0, 12); glVertex3f(-20, -5, 20);

    glEnd();
}


// -------------------------------------
// Cube
// -------------------------------------
void DrawCube(float s)
{
    float h = s * 0.5f;

    glBindTexture(GL_TEXTURE_2D, gTexture);

    glBegin(GL_QUADS);

    // Front
    glTexCoord2f(0, 0); glVertex3f(-h, -h, h);
    glTexCoord2f(1, 0); glVertex3f(h, -h, h);
    glTexCoord2f(1, 1); glVertex3f(h, h, h);
    glTexCoord2f(0, 1); glVertex3f(-h, h, h);

    // Back
    glTexCoord2f(1, 0); glVertex3f(-h, -h, -h);
    glTexCoord2f(1, 1); glVertex3f(-h, h, -h);
    glTexCoord2f(0, 1); glVertex3f(h, h, -h);
    glTexCoord2f(0, 0); glVertex3f(h, -h, -h);

    // Left
    glTexCoord2f(0, 0); glVertex3f(-h, -h, -h);
    glTexCoord2f(1, 0); glVertex3f(-h, -h, h);
    glTexCoord2f(1, 1); glVertex3f(-h, h, h);
    glTexCoord2f(0, 1); glVertex3f(-h, h, -h);

    // Right
    glTexCoord2f(0, 0); glVertex3f(h, -h, -h);
    glTexCoord2f(1, 0); glVertex3f(h, -h, h);
    glTexCoord2f(1, 1); glVertex3f(h, h, h);
    glTexCoord2f(0, 1); glVertex3f(h, h, -h);

    // Top
    glTexCoord2f(0, 1); glVertex3f(-h, h, h);
    glTexCoord2f(0, 0); glVertex3f(h, h, h);
    glTexCoord2f(1, 0); glVertex3f(h, h, -h);
    glTexCoord2f(1, 1); glVertex3f(-h, h, -h);

    // Bottom
    glTexCoord2f(1, 1); glVertex3f(-h, -h, h);
    glTexCoord2f(0, 1); glVertex3f(h, -h, h);
    glTexCoord2f(0, 0); glVertex3f(h, -h, -h);
    glTexCoord2f(1, 0); glVertex3f(-h, -h, -h);

    glEnd();
}


// -------------------------------------
void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // === Camera using yaw, pitch, distance ===
    float radYaw = camYaw * (M_PI / 180.0f);
    float radPitch = camPitch * (M_PI / 180.0f);

    float eyeX = camDist * cos(radPitch) * sin(radYaw);
    float eyeY = camDist * sin(radPitch);
    float eyeZ = camDist * cos(radPitch) * cos(radYaw);

    gluLookAt(eyeX, eyeY, eyeZ,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

    // Floor
    DrawFloorPlane();

    // Rotating cube
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glRotatef(gAngle, 0, 1, 0);
    glRotatef(gAngle * 0.6f, 1, 0, 0);
    DrawCube(4.0f);
    glPopMatrix();

    glutSwapBuffers();
}



// -------------------------------------
void Idle()
{
    gAngle += 0.2f;
    if (gAngle > 360) gAngle -= 360;
    glutPostRedisplay();
}


// -------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w': case 'W':
        wrapMode = 1 - wrapMode;
        ApplyTextureSettings();
        break;

    case 'f': case 'F':
        filterMode = (filterMode + 1) % 3;
        ApplyTextureSettings();
        break;

    case '+':
    case '=': // key atas '=' biasanya share dengan '+'
        camDist -= 1.0f;
        if (camDist < 5.0f) camDist = 5.0f; // clamp dekat sangat
        break;

    case '-':
    case '_':
        camDist += 1.0f;
        if (camDist > 100.0f) camDist = 100.0f; // clamp jauh sangat
        break;

    case 27: // ESC
        exit(0);
    }
}

void SpecialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        camYaw -= 3.0f;
        break;
    case GLUT_KEY_RIGHT:
        camYaw += 3.0f;
        break;
    case GLUT_KEY_UP:
        camPitch += 2.0f;
        if (camPitch > 89.0f) camPitch = 89.0f;   // elak terbalik
        break;
    case GLUT_KEY_DOWN:
        camPitch -= 2.0f;
        if (camPitch < -10.0f) camPitch = -10.0f;
        break;
    }
}

void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        }
        else if (state == GLUT_UP)
        {
            isDragging = false;
        }
    }
}
void MouseMotion(int x, int y)
{
    if (!isDragging) return;

    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    camYaw += dx * 0.4f;    // rotate left/right
    camPitch -= dy * 0.3f;    // rotate up/down (negative sebab koordinat y screen terbalik)

    if (camPitch > 89.0f)  camPitch = 89.0f;
    if (camPitch < -10.0f) camPitch = -10.0f;

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}


// -------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("BEST CHAPTER 5 DEMO — Wrapping & Filtering Toggle");

    InitGL();

    glutReshapeFunc(Reshape);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(MouseMotion);
    glutSpecialFunc(SpecialKeys);

    glutMainLoop();
    return 0;
}
