#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <GL/glut.h>
void drawPlayer(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x - size, y - size);
    glVertex2f(x + size, y - size);
    glVertex2f(x + size, y + size);
    glVertex2f(x - size, y + size);
    glEnd();
}

void drawRope(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);
    drawPlayer(-0.5, 0.0, 0.1);
    glColor3f(0.0, 0.0, 1.0);
    drawPlayer(0.5, 0.0, 0.1);
    glColor3f(0.0, 1.0, 0.0);
    drawRope(-0.4, 0.0, 0.4, 0.0);
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Rope Pulling Game");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}