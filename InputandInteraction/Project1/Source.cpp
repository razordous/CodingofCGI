// interaction_demo.cpp
// Simple GLUT demo for Input & Interaction (Chapter 4)
// Covers: display, idle, mouse, keyboard, motion, reshape, double buffer

#include <windows.h>    // On Windows, include this before GL headers
#include <GL/glut.h>
#include <cstdlib>      // rand()
#include <ctime>        // time()

// ----- Global State (used by many callbacks) -----
int gWinWidth = 800;
int gWinHeight = 600;

float gAngle = 0.0f;      // For rotation animation (Idle callback demo)
bool  gPaused = false;     // Toggle animation

// “Player” square controlled by keyboard / mouse
float gPlayerX = 100.0f;
float gPlayerY = 100.0f;

// Store clicked squares
const int MAX_SQUARES = 200;
int   gSquareCount = 0;
float gSquareX[MAX_SQUARES];
float gSquareY[MAX_SQUARES];
float gSquareR[MAX_SQUARES];
float gSquareG[MAX_SQUARES];
float gSquareB[MAX_SQUARES];

bool  gIsDragging = false;  // Left button drag

// ----- Utility: Convert window Y to world Y (OpenGL origin at bottom) -----
float windowToWorldY(int y)
{
    return (float)(gWinHeight - y);
}

// ----- Draw a simple filled square at (x, y) -----
void drawSquareAt(float x, float y, float size)
{
    glBegin(GL_POLYGON);
    glVertex2f(x - size, y - size);
    glVertex2f(x + size, y - size);
    glVertex2f(x + size, y + size);
    glVertex2f(x - size, y + size);
    glEnd();
}

// ----- DISPLAY CALLBACK (Slide: Display & Double Buffer) -----
void myDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 1) Draw rotating square at center (uses gAngle from Idle callback)
    glPushMatrix();
    glTranslatef(gWinWidth * 0.5f, gWinHeight * 0.5f, 0.0f);
    glRotatef(gAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(0.2f, 0.8f, 0.3f);
    drawSquareAt(0.0f, 0.0f, 50.0f);
    glPopMatrix();

    // 2) Draw “player” square (controlled by keyboard / passive mouse)
    glColor3f(0.1f, 0.3f, 1.0f);
    drawSquareAt(gPlayerX, gPlayerY, 20.0f);

    // 3) Draw all clicked squares
    for (int i = 0; i < gSquareCount; ++i)
    {
        glColor3f(gSquareR[i], gSquareG[i], gSquareB[i]);
        drawSquareAt(gSquareX[i], gSquareY[i], 10.0f);
    }

    // Use DOUBLE BUFFERING: draw to back buffer, then show it
    glutSwapBuffers();   // (Slide: double buffer, replaces glFlush)
}

// ----- IDLE CALLBACK (Slide 11) -----
void myIdle()
{
    if (!gPaused)
    {
        gAngle += 0.2f;            // rotate a bit every frame
        if (gAngle >= 360.0f)
            gAngle -= 360.0f;
    }

    // Tell GLUT that display needs to be redrawn
    glutPostRedisplay();
}

// ----- MOUSE CALLBACK (Slide: Mouse & Trigger/Measure) -----
void myMouse(int button, int state, int x, int y)
{
    float wx = (float)x;
    float wy = windowToWorldY(y);

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // Right click = quit program
        exit(0);
    }

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            gIsDragging = true;

            // Add a new square at click location (if there is space)
            if (gSquareCount < MAX_SQUARES)
            {
                gSquareX[gSquareCount] = wx;
                gSquareY[gSquareCount] = wy;

                gSquareR[gSquareCount] = (float)(rand() % 256) / 255.0f;
                gSquareG[gSquareCount] = (float)(rand() % 256) / 255.0f;
                gSquareB[gSquareCount] = (float)(rand() % 256) / 255.0f;

                gSquareCount++;
            }
        }
        else if (state == GLUT_UP)
        {
            gIsDragging = false;
        }
    }
}

// ----- MOTION CALLBACK (drag with button pressed) -----
void myMotion(int x, int y)
{
    if (!gIsDragging) return;

    float wx = (float)x;
    float wy = windowToWorldY(y);

    if (gSquareCount < MAX_SQUARES)
    {
        gSquareX[gSquareCount] = wx;
        gSquareY[gSquareCount] = wy;

        gSquareR[gSquareCount] = (float)(rand() % 256) / 255.0f;
        gSquareG[gSquareCount] = (float)(rand() % 256) / 255.0f;
        gSquareB[gSquareCount] = (float)(rand() % 256) / 255.0f;

        gSquareCount++;
    }

    glutPostRedisplay();
}

// ----- PASSIVE MOTION CALLBACK (move without button) -----
void myPassiveMotion(int x, int y)
{
    // Make player follow the mouse (for demo)
    gPlayerX = (float)x;
    gPlayerY = windowToWorldY(y);

    glutPostRedisplay();
}

// ----- KEYBOARD CALLBACK (normal keys) -----
void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
    case 'Q':
    case 27: // ESC
        exit(0);
        break;

    case 'c':
    case 'C':
        // Clear all clicked squares
        gSquareCount = 0;
        break;

    case 'p':
    case 'P':
        // Pause or resume animation (toggle idle effect)
        gPaused = !gPaused;
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

// ----- SPECIAL KEYS CALLBACK (arrow keys, F1, etc.) -----
void mySpecial(int key, int x, int y)
{
    const float step = 10.0f;

    switch (key)
    {
    case GLUT_KEY_UP:
        gPlayerY += step;
        break;
    case GLUT_KEY_DOWN:
        gPlayerY -= step;
        break;
    case GLUT_KEY_LEFT:
        gPlayerX -= step;
        break;
    case GLUT_KEY_RIGHT:
        gPlayerX += step;
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

// ----- RESHAPE CALLBACK (Slide: Reshape & Viewport) -----
void myReshape(int w, int h)
{
    if (h == 0) h = 1;

    gWinWidth = w;
    gWinHeight = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Simple 2D orthographic projection: world coords same as window pixels
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ----- MAIN FUNCTION (setup + register callbacks) -----
int main(int argc, char** argv)
{
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);

    // Use DOUBLE BUFFER and RGB color
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(gWinWidth, gWinHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Input & Interaction Demo - Chapter 4");

    // Background color (white)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Register callbacks
    glutDisplayFunc(myDisplay);          // Draw scene
    glutIdleFunc(myIdle);                // Animation when idle
    glutMouseFunc(myMouse);              // Mouse button
    glutMotionFunc(myMotion);            // Mouse drag
    glutPassiveMotionFunc(myPassiveMotion); // Mouse move without button
    glutKeyboardFunc(myKeyboard);        // Normal keys
    glutSpecialFunc(mySpecial);          // Arrow / function keys
    glutReshapeFunc(myReshape);          // Window resize

    // Start event loop (Slide: GLUT Event Loop)
    glutMainLoop();

    return 0;
}
