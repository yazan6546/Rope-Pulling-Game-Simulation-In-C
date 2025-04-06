#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <sys/mman.h>
#include <signal.h>

#include "game.h"
#include "config.h"

#define PLAYERS_PER_TEAM 4

float team1_x[PLAYERS_PER_TEAM];
float team2_x[PLAYERS_PER_TEAM];
float rope_center = 0.0;

float energy_team1 = 10.0;
float energy_team2 = 9.0;
Game *game;

// NEW: Global flags for animations.
volatile sig_atomic_t readyAnimation = 0;
volatile sig_atomic_t startAnimation = 0;

int timer = 0;

typedef struct {
    int is_falling;          // Flag to indicate if player is falling
    int falling_frames;      // Counter for animation frames
    float vertical_offset;   // Vertical position offset for animation
} PlayerAnimation;

// Animation data for each player
PlayerAnimation team1_animation[PLAYERS_PER_TEAM];
PlayerAnimation team2_animation[PLAYERS_PER_TEAM];

// Add this to your initialization function or add a new function
void initializeAnimations() {
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        team1_animation[i].is_falling = 0;
        team1_animation[i].falling_frames = 0;
        team1_animation[i].vertical_offset = 0.0f;

        team2_animation[i].is_falling = 0;
        team2_animation[i].falling_frames = 0;
        team2_animation[i].vertical_offset = 0.0f;
    }
}

void handle_ready_signal(int signum) {
    readyAnimation = 1;
    glutPostRedisplay();
}

void handle_start_signal(int signum) {
    startAnimation = 1;
    glutPostRedisplay();
}

void handle_alarm(int signum) {
    startAnimation = 0; // Reset the start animation flag
    readyAnimation = 0; // Reset the ready animation flag
    // Update the display
    glutPostRedisplay();
}

// Function to render text in OpenGL
void renderText(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);

    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}

// NEW: Add a helper function to render bigger text using a larger font.
void renderBigText(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
}

// Function to draw the scoreboard
void drawScoreboard() {
    char buffer[100];

    // Draw scoreboard background - pure black
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-0.98, 0.98);
    glVertex2f(0.98, 0.98);
    glVertex2f(0.98, 0.75);
    glVertex2f(-0.98, 0.75);
    glEnd();

    // Left side - Team A (Red)
    sprintf(buffer, "Team A Wins: %d", game->team_wins_A);
    renderText(-0.95, 0.93, buffer, 1.0, 0.0, 0.0);

    sprintf(buffer, "Round Score A: %.1f", game->total_effort_A);
    renderText(-0.95, 0.85, buffer, 1.0, 0.0, 0.0);

    // Middle - Round and timing info
    sprintf(buffer, "Round: %d", game->round_num);
    renderText(-0.15, 0.93, buffer, 1.0, 1.0, 1.0);

    sprintf(buffer, "Total Score: %.1f", game->total_score);
    renderText(-0.15, 0.85, buffer, 1.0, 1.0, 1.0);

    // Time information (stacked)
    sprintf(buffer, "Elapsed Time: %ds", game->elapsed_time);
    renderText(-0.15, 0.77, buffer, 1.0, 1.0, 1.0);

    sprintf(buffer, "Round Time: %ds", game->round_time);
    renderText(-0.15, 0.69, buffer, 1.0, 1.0, 1.0);

    // Right side - Team B (Blue)
    sprintf(buffer, "Team B Wins: %d", game->team_wins_B);
    renderText(0.5, 0.93, buffer, 0.0, 0.0, 1.0);

    sprintf(buffer, "Round Score B: %.1f", game->total_effort_B);
    renderText(0.5, 0.85, buffer, 0.0, 0.0, 1.0);

    // Who's winning indicator with modified text
    const char* winning_text;
    float text_r, text_g, text_b;

    if (game->total_effort_A > game->total_effort_B) {
        winning_text = "Red Team is taking the lead!";
        text_r = 1.0; text_g = 0.0; text_b = 0.0;
    } else if (game->total_effort_B > game->total_effort_A) {
        winning_text = "Blue Team is taking the lead!";
        text_r = 0.0; text_g = 0.0; text_b = 1.0;
    } else {
        winning_text = "Teams are tied - pull harder!";
        text_r = 1.0; text_g = 1.0; text_b = 1.0;
    }

    // Draw winning indicator below scoreboard
    renderText(-0.4, 0.63, winning_text, text_r, text_g, text_b);

  
}

void initializePlayers(float team1[], float team2[], int count) {
    float spacing = 0.15;
    float start_left = -0.8;
    float start_right = 0.8;

    for (int i = 0; i < count; i++) {
        team1[i] = start_left + i * spacing;
        team2[i] = start_right - i * spacing;
    }
}

void drawStickman(float x, float y, float scale, float r, float g, float b, float vertical_offset) {
    // Apply the vertical offset to the y position
    y += vertical_offset;

    glColor3f(r, g, b);
    glLineWidth(2.0);

    // Draw head (circle)
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159 / 180;
        glVertex2f(x + cos(angle) * 0.03 * scale, y + 0.07 * scale + sin(angle) * 0.03 * scale);
    }
    glEnd();

    // Body and limbs (lines)
    glBegin(GL_LINES);
    // Body
    glVertex2f(x, y + 0.03 * scale);
    glVertex2f(x, y - 0.04 * scale);
    // Arms
    glVertex2f(x - 0.05 * scale, y);
    glVertex2f(x + 0.05 * scale, y);
    // Legs
    glVertex2f(x, y - 0.04 * scale);
    glVertex2f(x - 0.04 * scale, y - 0.08 * scale);
    glVertex2f(x, y - 0.04 * scale);
    glVertex2f(x + 0.04 * scale, y - 0.08 * scale);
    glEnd();

    // Reset line width to default after drawing stickman
    glLineWidth(1.0);
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
    float energy_diff = game->total_effort_A - game->total_effort_B;
    rope_center += energy_diff * 0.005;

    // Limiting rope movement
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

        // Check for fallen or exhausted players in Team A
        if (game != NULL && game != MAP_FAILED) {
            // Team A falling animation
            if ((game->players_teamA[i].state == FALLEN || game->players_teamA[i].state == EXHAUSTED) &&
                !team1_animation[i].is_falling) {
                // Start new falling animation
                team1_animation[i].is_falling = 1;
                team1_animation[i].falling_frames = 0;
                team1_animation[i].vertical_offset = 0.0f;
            }

            // Team B falling animation
            if ((game->players_teamB[i].state == FALLEN || game->players_teamB[i].state == EXHAUSTED) &&
                !team2_animation[i].is_falling) {
                // Start new falling animation
                team2_animation[i].is_falling = 1;
                team2_animation[i].falling_frames = 0;
                team2_animation[i].vertical_offset = 0.0f;
            }

            // Update ongoing animations for Team A
            if (team1_animation[i].is_falling) {
                team1_animation[i].falling_frames++;

                // Calculate vertical offset with a smooth curve
                float progress = (float)team1_animation[i].falling_frames / 10.0f; // 10 frames = 0.5 seconds at 50ms timer
                if (progress <= 1.0f) {
                    // Apply a smooth falling curve
                    team1_animation[i].vertical_offset = -0.08f * (progress * progress);
                } else {
                    // Keep fallen on the ground
                    team1_animation[i].vertical_offset = -0.08f;

                    // Reset animation if player is no longer fallen/exhausted
                    if (game->players_teamA[i].state != FALLEN && game->players_teamA[i].state != EXHAUSTED) {
                        team1_animation[i].is_falling = 0;
                        team1_animation[i].vertical_offset = 0.0f;
                    }
                }
            }

            // Update ongoing animations for Team B
            if (team2_animation[i].is_falling) {
                team2_animation[i].falling_frames++;

                // Calculate vertical offset with a smooth curve
                float progress = (float)team2_animation[i].falling_frames / 10.0f; // 10 frames = 0.5 seconds at 50ms timer
                if (progress <= 1.0f) {
                    // Apply a smooth falling curve
                    team2_animation[i].vertical_offset = -0.08f * (progress * progress);
                } else {
                    // Keep fallen on the ground
                    team2_animation[i].vertical_offset = -0.08f;

                    // Reset animation if player is no longer fallen/exhausted
                    if (game->players_teamB[i].state != FALLEN && game->players_teamB[i].state != EXHAUSTED) {
                        team2_animation[i].is_falling = 0;
                        team2_animation[i].vertical_offset = 0.0f;
                    }
                }
            }
        }
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


void drawEnergyBar(float x, float y, float energy, float maxEnergy) {
    // Very basic safety checks
    // if (maxEnergy <= 0.0f) maxEnergy = 200.0f;
    // if (energy < 0.0f) energy = 0.0f;
    // if (energy > maxEnergy) energy = maxEnergy;

    float percentage = energy / maxEnergy;

    // Simple dimensions
    float barWidth = 0.06f;
    float barHeight = 0.01f;
    float barY = y + 0.12f;
    float barLeft = x - barWidth/2.0f;

    // Draw background (grey)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barLeft, barY);
    glVertex2f(barLeft + barWidth, barY);
    glVertex2f(barLeft + barWidth, barY + barHeight);
    glVertex2f(barLeft, barY + barHeight);
    glEnd();

    // Fixed color thresholds (no complex calculations)
    if (percentage > 0.7f) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
    } else if (percentage > 0.3f) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    } else {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
    }

    // Draw filled portion
    glBegin(GL_QUADS);
    glVertex2f(barLeft, barY);
    glVertex2f(barLeft + barWidth * percentage, barY);
    glVertex2f(barLeft + barWidth * percentage, barY + barHeight);
    glVertex2f(barLeft, barY + barHeight);
    glEnd();
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();

    // Draw stickmen and energy bars
    for (int i = 0; i < PLAYERS_PER_TEAM; i++) {
        // Team A (Red)
        drawStickman(team1_x[i], -0.1, 0.7, 1.0, 0.0, 0.0, team1_animation[i].vertical_offset);

        // Team B (Blue)
        drawStickman(team2_x[i], -0.1, 0.7, 0.0, 0.0, 1.0, team2_animation[i].vertical_offset);

        // Draw energy bars using actual game data
        if (game != NULL && game != MAP_FAILED) {
            float energyA = game->players_teamA[i].attributes.energy;
            float energyB = game->players_teamB[i].attributes.energy;

            // Add bounds checking for safety
            if (energyA < 0.0f) energyA = 0.0f;
            if (energyB < 0.0f) energyB = 0.0f;

            // Draw energy bars
            drawEnergyBar(team1_x[i], -0.1 + team1_animation[i].vertical_offset, energyA, game->players_teamA[i].attributes.inital_energy);
            drawEnergyBar(team2_x[i], -0.1 + team2_animation[i].vertical_offset, energyB, game->players_teamB[i].attributes.inital_energy);

            // Draw energy numbers above bars
            char energyTextA[10], energyTextB[10];
            sprintf(energyTextA, "%.2f", energyA);
            sprintf(energyTextB, "%.2f", energyB);

            // Position text above energy bars (y coordinate: -0.1 + 0.14)
            renderText(team1_x[i] - 0.02, 0.04 + team1_animation[i].vertical_offset, energyTextA, 1.0, 0.0, 0.0); // Red team
            renderText(team2_x[i] - 0.02, 0.04 + team2_animation[i].vertical_offset, energyTextB, 0.0, 0.0, 1.0); // Blue team

            // Animation text displays
            if (readyAnimation) {
                renderBigText(-0.1, 0.5, "READY!", 1.0, 1.0, 0.0);
            }

            if (startAnimation) {
                readyAnimation = 0;
                timer++;
                if (timer > 50) {
                    timer = 0;
                    startAnimation = 0;
                }
                renderBigText(-0.1, 0.5, "START!", 0.0, 1.0, 0.0);
            }

            if(game->game_running == 0) {
                renderBigText(-0.1, 0.5, "GAME OVER!", 1.0, 0.0, 0.0);
            }
        }
    }

    // Rest of display function remains the same
    // Draw rope segments
    float grip_offset = 0.05;

    // Red team rope segments
    for (int i = 0; i < PLAYERS_PER_TEAM - 1; i++) {
        drawRopeSegment(
            team1_x[i] + grip_offset, -0.1 + team1_animation[i].vertical_offset,
            team1_x[i+1] - grip_offset, -0.1 + team1_animation[i+1].vertical_offset
        );
    }

    // Blue team rope segments
    for (int i = PLAYERS_PER_TEAM - 1; i > 0; i--) {
        drawRopeSegment(
            team2_x[i] + grip_offset, -0.1 + team2_animation[i].vertical_offset,
            team2_x[i-1] - grip_offset, -0.1 + team2_animation[i-1].vertical_offset
        );
    }

    // Middle rope segment
    drawRopeSegment(
        team1_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1 + team1_animation[PLAYERS_PER_TEAM-1].vertical_offset,
        team2_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1 + team2_animation[PLAYERS_PER_TEAM-1].vertical_offset
    );

    // Draw the scoreboard
    drawScoreboard();

    glFlush();
}


int main(int argc, char** argv) {

    // Check command-line argument

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fd>\n", argv[0]);
        return 1;
    }
    int fd = atoi(argv[1]);
    game = mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (game == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tug-of-War with Rectangular Rope");
    initializePlayers(team1_x, team2_x, PLAYERS_PER_TEAM);
    initializeAnimations(); // Add this line to initialize animations

    // Rest of main function remains the same
    signal(SIGUSR1, handle_ready_signal);
    signal(SIGUSR2, handle_start_signal);
    signal(SIGALRM, handle_alarm);
    glutDisplayFunc(display);
    glutTimerFunc(50, updateGame, 0);
    glutMainLoop();
    return 0;

}