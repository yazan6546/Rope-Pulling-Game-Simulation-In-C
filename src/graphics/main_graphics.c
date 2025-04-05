#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

#define PLAYERS_PER_TEAM 4

float team1_x[PLAYERS_PER_TEAM];
float team2_x[PLAYERS_PER_TEAM];
float rope_center = 0.0;

float energy_team1_players[PLAYERS_PER_TEAM] = {2.5, 2.5, 2.5, 2.5};
float energy_team2_players[PLAYERS_PER_TEAM] = {12.5, 12.5, 12.5, 12.5};

int fallen_team1[PLAYERS_PER_TEAM] = {0};
int fallen_team2[PLAYERS_PER_TEAM] = {0};

void initializePlayers(float team1[], float team2[], int count) {
    float spacing = 0.15;
    float start_left = -0.8;
    float start_right = 0.8;

    for (int i = 0; i < count; i++) {
        team1[i] = start_left + i * spacing;
        team2[i] = start_right - i * spacing;
    }
}

void drawStickman(float x, float y, float scale, float r, float g, float b, int fallen) {
    glColor3f(r, g, b);
    glLineWidth(2.0);

    if (!fallen) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * 3.14159 / 180;
            glVertex2f(x + cos(angle) * 0.03 * scale, y + 0.07 * scale + sin(angle) * 0.03 * scale);
        }
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(x, y + 0.03 * scale);
        glVertex2f(x, y - 0.04 * scale);
        glVertex2f(x - 0.05 * scale, y);
        glVertex2f(x + 0.05 * scale, y);
        glVertex2f(x, y - 0.04 * scale);
        glVertex2f(x - 0.04 * scale, y - 0.08 * scale);
        glVertex2f(x, y - 0.04 * scale);
        glVertex2f(x + 0.04 * scale, y - 0.08 * scale);
        glEnd();
    } else {
        float offset = 0.02 * scale;
        y -= 0.08 * scale;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * 3.14159 / 180;
            glVertex2f(x + 0.07 * scale + cos(angle) * 0.03 * scale,
                       y + sin(angle) * 0.03 * scale);
        }
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(x + 0.07 * scale, y);
        glVertex2f(x - 0.03 * scale, y);

        glVertex2f(x - 0.03 * scale, y);
        glVertex2f(x - 0.07 * scale, y + 0.03 * scale);

        glVertex2f(x - 0.03 * scale, y);
        glVertex2f(x - 0.07 * scale, y - 0.03 * scale);
        glEnd();
    }

    glLineWidth(1.0);
}


void drawCircle(float cx, float cy, float r, int segments, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

void drawCloud(float cx, float cy, float scale) {
    drawCircle(cx, cy, 0.07 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx + 0.08 * scale, cy, 0.06 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx - 0.08 * scale, cy, 0.06 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx + 0.05 * scale, cy + 0.05 * scale, 0.05 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx - 0.05 * scale, cy + 0.05 * scale, 0.05 * scale, 32, 1.0, 1.0, 1.0);
}

void drawBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.53, 0.81, 0.98);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glColor3f(0.69, 0.93, 1.0);
    glVertex2f(1.0, -0.2);
    glVertex2f(-1.0, -0.2);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.7, 0.0);
    glVertex2f(-1.0, -0.2);
    glVertex2f(1.0, -0.2);
    glVertex2f(1.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();

    drawCircle(0.8, 0.8, 0.1, 32, 1.0, 1.0, 0.0);
    drawCloud(-0.6, 0.7, 1.0);
    drawCloud(0.3, 0.8, 1.2);
    drawCloud(-0.1, 0.6, 0.8);
}

void drawRopeSegment(float x1, float y1, float x2, float y2) {
    float thickness = 0.01;
    glColor3f(0.5, 0.3, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1 - thickness);
    glVertex2f(x1, y1 + thickness);
    glVertex2f(x2, y2 + thickness);
    glVertex2f(x2, y2 - thickness);
    glEnd();
}

void updateGame(int value) {
    float energy_team1 = 0.0;
    float energy_team2 = 0.0;

    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        energy_team1 += energy_team1_players[i];
        energy_team2 += energy_team2_players[i];
    }

    float energy_diff = energy_team1 - energy_team2;
    rope_center += energy_diff * 0.005;

    if (rope_center > 0.6) rope_center = 0.6;
    if (rope_center < -0.6) rope_center = -0.6;

    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        team1_x[i] = (-0.8 + i * 0.15) + rope_center;
        team2_x[i] = (0.8 - i * 0.15) + rope_center;

        if (team1_x[i] > 0.95) team1_x[i] = 0.95;
        if (team1_x[i] < -0.95) team1_x[i] = -0.95;
        if (team2_x[i] > 0.95) team2_x[i] = 0.95;
        if (team2_x[i] < -0.95) team2_x[i] = -0.95;
    }


    energy_team1_players[0] -= 0.02;
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        if (energy_team1_players[i] <= 0.0) {
            fallen_team1[i] = 1;
        }
        if (energy_team2_players[i] <= 0.0) {
            fallen_team2[i] = 1;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(50, updateGame, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();

    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        drawStickman(team1_x[i], -0.1, 0.7, 1.0, 0.0, 0.0, fallen_team1[i]);
        drawStickman(team2_x[i], -0.1, 0.7, 0.0, 0.0, 1.0, fallen_team2[i]);
    }

    float grip_offset = 0.05;

    for (int i = 0; i < PLAYERS_PER_TEAM - 1; i++) {
        drawRopeSegment(team1_x[i] + grip_offset, -0.1, team1_x[i + 1] - grip_offset, -0.1);
    }

    for (int i = PLAYERS_PER_TEAM - 1; i > 0; i--) {
        drawRopeSegment(team2_x[i] + grip_offset, -0.1, team2_x[i - 1] - grip_offset, -0.1);
    }

    drawRopeSegment(team1_x[PLAYERS_PER_TEAM - 1] + grip_offset, -0.1,
                    team2_x[PLAYERS_PER_TEAM - 1] + grip_offset, -0.1);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tug-of-War with Falling Players");
    initializePlayers(team1_x, team2_x, PLAYERS_PER_TEAM);
    glutDisplayFunc(display);
    glutTimerFunc(50, updateGame, 0);
    glutMainLoop();
    return 0;
}
