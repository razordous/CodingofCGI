// lighting_demo_glut.cpp
// Simple OpenGL lighting demo using GLUT
// - Toggle GL_LIGHT0 .. GL_LIGHT7 with keys 0-7
// - Change lighting mode: ambient / diffuse / specular / emission / full

#include <windows.h>
#include <GL/glut.h>
#include <cstdio>

enum LightingMode {
    MODE_FULL = 0,
    MODE_AMBIENT_ONLY,
    MODE_DIFFUSE_ONLY,
    MODE_SPECULAR_ONLY,
    MODE_EMISSION_ONLY
};

LightingMode gMode = MODE_FULL;
bool gLightEnabled[8] = { true, false, false, false, false, false, false, false };
float gAngleY = 0.0f;   // rotate object so lighting effect is clearer

// Forward declarations
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void idle();

void printInstructions()
{
    printf("OpenGL Lighting Demo (GLUT)\n");
    printf("---------------------------------\n");
    printf("Number keys 0-7 : Toggle GL_LIGHT0..GL_LIGHT7\n");
    printf("a : Ambient only mode\n");
    printf("d : Diffuse only mode\n");
    printf("s : Specular only mode\n");
    printf("e : Emission only mode\n");
    printf("f : Full lighting (ambient + diffuse + specular)\n");
    printf("r : Reset lights and mode (only LIGHT0 on)\n");
    printf("ESC : Quit\n\n");
}

void initGL()
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);      // make sure normals stay correct after scaling/rotations

    glEnable(GL_LIGHTING);

    // Global ambient light for the whole scene
    GLfloat globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Only light front faces
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

void setupLights()
{
    GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    for (int i = 0; i < 8; ++i)
    {
        GLenum lightId = GL_LIGHT0 + i;

        // For emission-only mode, we turn OFF all lights (object glows by itself)
        if (gMode == MODE_EMISSION_ONLY) {
            glDisable(lightId);
            continue;
        }

        if (!gLightEnabled[i]) {
            glDisable(lightId);
            continue;
        }

        glEnable(lightId);

        // Base position and color for each light
        GLfloat pos[4];
        GLfloat baseColor[4];

        switch (i)
        {
        case 0: // GL_LIGHT0: white directional light (like the sun)
            pos[0] = 1.0f; pos[1] = 1.0f; pos[2] = 1.0f; pos[3] = 0.0f; // w = 0 → directional light
            baseColor[0] = 1.0f; baseColor[1] = 1.0f; baseColor[2] = 1.0f; baseColor[3] = 1.0f;
            break;

        case 1: // GL_LIGHT1: warm point light from the left
            pos[0] = -3.0f; pos[1] = 3.0f; pos[2] = 3.0f; pos[3] = 1.0f; // w = 1 → point light
            baseColor[0] = 1.0f; baseColor[1] = 0.8f; baseColor[2] = 0.6f; baseColor[3] = 1.0f;
            break;

        case 2: // GL_LIGHT2: cool point light from the right
            pos[0] = 3.0f; pos[1] = 3.0f; pos[2] = 0.0f; pos[3] = 1.0f;
            baseColor[0] = 0.6f; baseColor[1] = 0.7f; baseColor[2] = 1.0f; baseColor[3] = 1.0f;
            break;

        case 3: // GL_LIGHT3: spotlight from above
            pos[0] = 0.0f; pos[1] = 6.0f; pos[2] = 0.0f; pos[3] = 1.0f;
            baseColor[0] = 0.8f; baseColor[1] = 0.8f; baseColor[2] = 1.0f; baseColor[3] = 1.0f;
            {
                GLfloat spotDir[] = { 0.0f, -1.0f, 0.0f };
                glLightfv(lightId, GL_SPOT_DIRECTION, spotDir);
                glLightf(lightId, GL_SPOT_CUTOFF, 25.0f); // spotlight cone angle
            }
            break;

        default: // GL_LIGHT4..GL_LIGHT7: extra colored directional lights (for fun)
            pos[0] = (i - 4) - 1.5f; pos[1] = 2.0f; pos[2] = 2.0f; pos[3] = 0.0f; // directional
            baseColor[0] = (i & 1) ? 1.0f : 0.3f;
            baseColor[1] = (i & 2) ? 1.0f : 0.3f;
            baseColor[2] = (i & 4) ? 1.0f : 0.3f;
            baseColor[3] = 1.0f;
            break;
        }

        glLightfv(lightId, GL_POSITION, pos);

        // Simple attenuation example for point/spot lights (w == 1.0)
        if (pos[3] == 1.0f)
        {
            glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightId, GL_LINEAR_ATTENUATION, 0.1f);
            glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.01f);
        }
        else
        {
            // Directional lights: no distance attenuation
            glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightId, GL_LINEAR_ATTENUATION, 0.0f);
            glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.0f);
        }

        // Set how this light contributes based on current mode
        switch (gMode)
        {
        case MODE_AMBIENT_ONLY:
            glLightfv(lightId, GL_AMBIENT, baseColor);
            glLightfv(lightId, GL_DIFFUSE, black);
            glLightfv(lightId, GL_SPECULAR, black);
            break;

        case MODE_DIFFUSE_ONLY:
            glLightfv(lightId, GL_AMBIENT, black);
            glLightfv(lightId, GL_DIFFUSE, baseColor);
            glLightfv(lightId, GL_SPECULAR, black);
            break;

        case MODE_SPECULAR_ONLY:
            glLightfv(lightId, GL_AMBIENT, black);
            glLightfv(lightId, GL_DIFFUSE, black);
            glLightfv(lightId, GL_SPECULAR, baseColor);
            break;

        case MODE_FULL:
        default:
            glLightfv(lightId, GL_AMBIENT, baseColor);
            glLightfv(lightId, GL_DIFFUSE, baseColor);
            glLightfv(lightId, GL_SPECULAR, baseColor);
            break;
        }
    }
}

void setMaterial()
{
    // Base material: shiny red
    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.1f, 0.1f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat shininess = 50.0f;

    if (gMode == MODE_EMISSION_ONLY)
    {
        // In emission-only mode, object glows by itself, no normal lighting
        emission[0] = 0.8f; emission[1] = 0.2f; emission[2] = 0.2f;
        ambient[0] = ambient[1] = ambient[2] = 0.0f;
        diffuse[0] = diffuse[1] = diffuse[2] = 0.0f;
        specular[0] = specular[1] = specular[2] = 0.0f;
        shininess = 0.0f;
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 4.0 / 3.0, 0.1, 50.0);

    // Camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 2.0, 8.0,   // eye position
        0.0, 0.0, 0.0,   // look-at target
        0.0, 1.0, 0.0);  // up vector

    // Rotate object so we can see highlights move
    glRotatef(gAngleY, 0.0f, 1.0f, 0.0f);

    // Setup lights & material for this frame
    setupLights();
    setMaterial();

    // Draw a teapot – nice curved surface for lighting demo
    glutSolidTeapot(1.5);

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    // Projection recalculated in display(), so no need to change here
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC
        exit(0);
        break;

    case 'a': case 'A':
        gMode = MODE_AMBIENT_ONLY;
        printf("Mode: Ambient only\n");
        break;

    case 'd': case 'D':
        gMode = MODE_DIFFUSE_ONLY;
        printf("Mode: Diffuse only\n");
        break;

    case 's': case 'S':
        gMode = MODE_SPECULAR_ONLY;
        printf("Mode: Specular only\n");
        break;

    case 'e': case 'E':
        gMode = MODE_EMISSION_ONLY;
        printf("Mode: Emission only (object glows, lights off)\n");
        break;

    case 'f': case 'F':
        gMode = MODE_FULL;
        printf("Mode: Full lighting (ambient + diffuse + specular)\n");
        break;

    case 'r': case 'R':
        gMode = MODE_FULL;
        for (int i = 0; i < 8; ++i)
            gLightEnabled[i] = (i == 0); // reset: only LIGHT0 on
        printf("Reset: Full mode, only GL_LIGHT0 enabled.\n");
        break;

    default:
        if (key >= '0' && key <= '7')
        {
            int idx = key - '0';
            gLightEnabled[idx] = !gLightEnabled[idx];
            printf("GL_LIGHT%d %s\n", idx, gLightEnabled[idx] ? "ENABLED" : "DISABLED");
        }
        break;
    }

    glutPostRedisplay();
}

void idle()
{
    gAngleY += 0.2f;
    if (gAngleY > 360.0f) gAngleY -= 360.0f;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lighting Demo - GL_LIGHT0 .. GL_LIGHT7");

    initGL();
    printInstructions();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
