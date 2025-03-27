#include <stdio.h>
#include <GL/glut.h>

void display(void);

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Hello, OpenGL");
    glutDisplayFunc(display);
    glutMainLoop();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}