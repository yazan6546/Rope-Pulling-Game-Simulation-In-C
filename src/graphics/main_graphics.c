#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

#define PLAYERS_PER_TEAM 4

float team1_x[PLAYERS_PER_TEAM];
float team2_x[PLAYERS_PER_TEAM];
float rope_center = 0.0;

float energy_team1 = 10.0;
float energy_team2 = 9.0;


void initializePlayers(float team1[], float team2[], int count) {
    float spacing = 0.15;
    float start_left = -0.8;
    float start_right = 0.8;

    for (int i = 0; i < count; i++) {
        team1[i] = start_left + i * spacing;
        team2[i] = start_right - i * spacing;
    }
}

void drawStickman(float x, float y, float scale, float r, float g, float b) {
    glColor3f(r, g, b);


    glLineWidth(2.0);


    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159 / 180;
        glVertex2f(x + cos(angle) * 0.03 * scale, y + 0.07 * scale + sin(angle) * 0.03 * scale);
    }
    glEnd();

    //Body and limbs (lines)
    glBegin(GL_LINES);
    //Body
    glVertex2f(x, y + 0.03 * scale);
    glVertex2f(x, y - 0.04 * scale);
    //Arms
    glVertex2f(x - 0.05 * scale, y);
    glVertex2f(x + 0.05 * scale, y);
    //Legs
    glVertex2f(x, y - 0.04 * scale);
    glVertex2f(x - 0.04 * scale, y - 0.08 * scale);
    glVertex2f(x, y - 0.04 * scale);
    glVertex2f(x + 0.04 * scale, y - 0.08 * scale);
    glEnd();

    // Reset line width to default after drawing stickman
    glLineWidth(1.0);  // Add this line to reset line width
}

//Draw the rope as a rectangle
void drawRope(float x1, float y1, float x2, float y2) {
    glColor3f(0.5, 0.3, 0.1);  // Wooden brown

    // Draw as a thick rectangle (width = 0.02)
    float thickness = 0.02;
    glBegin(GL_QUADS);
    glVertex2f(x1, y1 - thickness);
    glVertex2f(x1, y1 + thickness);
    glVertex2f(x2, y2 + thickness);
    glVertex2f(x2, y2 - thickness);
    glEnd();
}


// Draw a circle (for sun/clouds)
void drawCircle(float cx, float cy, float r, int segments, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

// Draw a cloud
void drawCloud(float cx, float cy, float scale) {
    glColor3f(1.0, 1.0, 1.0);  // White
    drawCircle(cx, cy, 0.07 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx + 0.08 * scale, cy, 0.06 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx - 0.08 * scale, cy, 0.06 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx + 0.05 * scale, cy + 0.05 * scale, 0.05 * scale, 32, 1.0, 1.0, 1.0);
    drawCircle(cx - 0.05 * scale, cy + 0.05 * scale, 0.05 * scale, 32, 1.0, 1.0, 1.0);
}

// Draw the background (sky, ground, sun, clouds)
void drawBackground() {
    // Sky (gradient blue)
    glBegin(GL_QUADS);
    glColor3f(0.53, 0.81, 0.98);  // Light blue (top)
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glColor3f(0.69, 0.93, 1.0);    // Lighter blue (bottom)
    glVertex2f(1.0, -0.2);
    glVertex2f(-1.0, -0.2);
    glEnd();

    // Ground (green)
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.7, 0.0);
    glVertex2f(-1.0, -0.2);
    glVertex2f(1.0, -0.2);
    glVertex2f(1.0, -1.0);
    glVertex2f(-1.0, -1.0);
    glEnd();

    // Sun (yellow)
    drawCircle(0.8, 0.8, 0.1, 32, 1.0, 1.0, 0.0);

    // Clouds
    drawCloud(-0.6, 0.7, 1.0);
    drawCloud(0.3, 0.8, 1.2);
    drawCloud(-0.1, 0.6, 0.8);
}

//Making sure players don't go out of the screen
void updateGame(int value) {
    float energy_diff = energy_team1 - energy_team2;
    rope_center += energy_diff * 0.005;

    //Limiting rope movement
    if (rope_center > 0.6) rope_center = 0.6;
    if (rope_center < -0.6) rope_center = -0.6;

    // Update player positions
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        team1_x[i] = (-0.8 + i * 0.15) + rope_center;
        team2_x[i] = (0.8 - i * 0.15) + rope_center;

        // Ensure players stay on-screen
        if (team1_x[i] > 0.95) team1_x[i] = 0.95;
        if (team1_x[i] < -0.95) team1_x[i] = -0.95;
        if (team2_x[i] > 0.95) team2_x[i] = 0.95;
        if (team2_x[i] < -0.95) team2_x[i] = -0.95;
    }

    glutPostRedisplay();
    glutTimerFunc(50, updateGame, 0);
}

//rope between players
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();

    // Draw all stickmen first
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        drawStickman(team1_x[i], -0.1, 0.7, 1.0, 0.0, 0.0);  // Red team
        drawStickman(team2_x[i], -0.1, 0.7, 0.0, 0.0, 1.0);  // Blue team
    }

    // Calculate rope positions
    float grip_offset = 0.05;  // Distance from player center to rope attachment

    //Red team rope segments
    for (int i = 0; i < PLAYERS_PER_TEAM - 1; i++) {
        drawRopeSegment(
            team1_x[i] + grip_offset, -0.1,
            team1_x[i+1] - grip_offset, -0.1
        );
    }

    // Blue team rope segments
    for (int i = PLAYERS_PER_TEAM - 1; i > 0; i--) {
        drawRopeSegment(
            team2_x[i] + grip_offset, -0.1,     // Right side of leftmost player
            team2_x[i-1] - grip_offset, -0.1    // Left side of player to the right
        );
    }

    // Middle rope segment
    drawRopeSegment(
        team1_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1,  // Right side of rightmost red player
        team2_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1   // Right side of leftmost blue player
    );

    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tug-of-War with Rectangular Rope");
    initializePlayers(team1_x, team2_x, PLAYERS_PER_TEAM);
    glutDisplayFunc(display);
    glutTimerFunc(50, updateGame, 0);
    glutMainLoop();
    return 0;
}