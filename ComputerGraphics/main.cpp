#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>

// GLEW VERSION IS 4.0
void renderScene(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.3, 0.3, 1.0);

    glutSwapBuffers();
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(500, 500);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL First Window");

    glewInit();
    if (glewIsSupported("GL_VERSION_4_0"))
    {
        std::cout << " GLEW Version is 4.0\n ";
    }
    else
    {
        std::cout << "GLEW 4.0 not supported\n ";
    }

    glEnable(GL_DEPTH_TEST);
    // register callbacks
    glutDisplayFunc(renderScene);

    glutMainLoop();

    return 0;
}