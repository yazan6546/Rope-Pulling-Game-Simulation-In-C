#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <sys/mman.h>
#include <signal.h>

#include "game.h"
#include "config.h"

#define PLAYERS_PER_TEAM 4

Config config;

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
int display_timer = 0;

int movement_timer = 0;

int display_team_win = 0;

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
void drawScoreboard() {
    char buffer[100];

    // Gradient background for scoreboard
    glBegin(GL_QUADS);
    glColor3f(0.12, 0.12, 0.18);  // Dark top
    glVertex2f(-0.98, 0.98);
    glVertex2f(0.98, 0.98);
    glColor3f(0.18, 0.18, 0.25);  // Slightly lighter bottom
    glVertex2f(0.98, 0.75);
    glVertex2f(-0.98, 0.75);
    glEnd();

    // Team A panel (left side)
    glColor3f(0.6, 0.1, 0.1);  // Dark red
    glBegin(GL_QUADS);
    glVertex2f(-0.96, 0.96);
    glVertex2f(-0.51, 0.96);
    glVertex2f(-0.51, 0.78);
    glVertex2f(-0.96, 0.78);
    glEnd();

    // Team B panel (right side)
    glColor3f(0.1, 0.1, 0.6);  // Dark blue
    glBegin(GL_QUADS);
    glVertex2f(0.51, 0.96);
    glVertex2f(0.96, 0.96);
    glVertex2f(0.96, 0.78);
    glVertex2f(0.51, 0.78);
    glEnd();

    // Center info panel
    glColor3f(0.2, 0.2, 0.2);  // Dark gray
    glBegin(GL_QUADS);
    glVertex2f(-0.45, 0.96);
    glVertex2f(0.45, 0.96);
    glVertex2f(0.45, 0.78);
    glVertex2f(-0.45, 0.78);
    glEnd();

    // Highlighted header bars for teams
    glColor3f(0.8, 0.2, 0.2);  // Brighter red
    glBegin(GL_QUADS);
    glVertex2f(-0.96, 0.96);
    glVertex2f(-0.51, 0.96);
    glVertex2f(-0.51, 0.92);
    glVertex2f(-0.96, 0.92);
    glEnd();

    glColor3f(0.2, 0.2, 0.8);  // Brighter blue
    glBegin(GL_QUADS);
    glVertex2f(0.51, 0.96);
    glVertex2f(0.96, 0.96);
    glVertex2f(0.96, 0.92);
    glVertex2f(0.51, 0.92);
    glEnd();

    glColor3f(0.3, 0.3, 0.3);  // Accent for round
    glBegin(GL_QUADS);
    glVertex2f(-0.25, 0.96);
    glVertex2f(0.25, 0.96);
    glVertex2f(0.25, 0.92);
    glVertex2f(-0.25, 0.92);
    glEnd();

    // Team A info - centered
    renderBigText(-0.735, 0.94, "TEAM A", 1.0, 1.0, 1.0);
    sprintf(buffer, "Wins: %d", game->team_wins_A);
    renderText(-0.93, 0.88, buffer, 1.0, 1.0, 1.0);
    sprintf(buffer, "Score: %.1f", game->total_effort_A);
    renderText(-0.93, 0.82, buffer, 1.0, 1.0, 1.0);

    // Center game info - centered and prominent
    sprintf(buffer, "ROUND %d", game->round_num);
    renderBigText(-0.09, 0.94, buffer, 1.0, 1.0, 0.7);
    sprintf(buffer, "Total: %.1f", game->total_score);
    renderText(-0.08, 0.88, buffer, 0.9, 0.9, 0.9);

    // Divider line
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
    glVertex2f(-0.1, 0.86);
    glVertex2f(0.1, 0.86);
    glEnd();

    // Time display
    sprintf(buffer, "Game: %ds", game->elapsed_time);
    renderText(-0.35, 0.82, buffer, 0.8, 0.8, 0.8);
    sprintf(buffer, "Round: %ds", game->round_time);
    renderText(0.1, 0.82, buffer, 0.8, 0.8, 0.8);

    // Team B info - centered
    renderBigText(0.735, 0.94, "TEAM B", 1.0, 1.0, 1.0);
    sprintf(buffer, "Wins: %d", game->team_wins_B);
    renderText(0.55, 0.88, buffer, 1.0, 1.0, 1.0);
    sprintf(buffer, "Score: %.1f", game->total_effort_B);
    renderText(0.55, 0.82, buffer, 1.0, 1.0, 1.0);

    // Progress bar background
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex2f(-0.6, 0.73);
    glVertex2f(0.6, 0.73);
    glVertex2f(0.6, 0.7);
    glVertex2f(-0.6, 0.7);
    glEnd();

    // Calculate team strength ratio
    float total_effort = game->total_effort_A + game->total_effort_B;
    float ratio_A = 0.5;  // Default to 50/50

    if (total_effort > 0) {
        ratio_A = game->total_effort_A / total_effort;
        if (ratio_A > 1.0) ratio_A = 1.0;
        if (ratio_A < 0.0) ratio_A = 0.0;
    }

    // Team A's portion of progress bar
    glColor3f(0.8, 0.1, 0.1);  // Bright red
    glBegin(GL_QUADS);
    glVertex2f(-0.6, 0.73);
    glVertex2f(-0.6 + 1.2 * ratio_A, 0.73);
    glVertex2f(-0.6 + 1.2 * ratio_A, 0.7);
    glVertex2f(-0.6, 0.7);
    glEnd();

    // Determine leading team text
    const char* winning_text;
    float text_r, text_g, text_b;

    if (game->total_effort_A > game->total_effort_B) {
        winning_text = "Red Team Leading";
        text_r = 1.0; text_g = 0.6; text_b = 0.6;
    } else if (game->total_effort_B > game->total_effort_A) {
        winning_text = "Blue Team Leading";
        text_r = 0.6; text_g = 0.6; text_b = 1.0;
    } else {
        winning_text = "Teams Tied";
        text_r = 1.0; text_g = 1.0; text_b = 1.0;
    }

    // Display winning team indicator
    renderText(-0.12, 0.65, winning_text, text_r, text_g, text_b);
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
    if (game == NULL || game == MAP_FAILED || game->game_running == 0) {
        glutPostRedisplay();
        glutTimerFunc(50, updateGame, 0);
        return;
    }

    float energy_diff = game->total_effort_A - game->total_effort_B;
    float target = -energy_diff/config.WINNING_THRESHOLD * 0.15;

    if(movement_timer == 0) {
        rope_center = 0.0;
    } else if(movement_timer < 20) {
        rope_center += target/20.0;
    } else if(movement_timer == 20)
        movement_timer = 0;

    movement_timer++;

    // Limiting rope movement
    if (rope_center > 0.15) rope_center = 0.15;
    if (rope_center < -0.15) rope_center = -0.15;

    // Update player positions
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        team1_x[i] = (-0.8 + i * 0.15) + rope_center;
        team2_x[i] = (0.8 - i * 0.15) + rope_center;

        // Ensure players stay on-screen
        if (team1_x[i] > 0.95) team1_x[i] = 0.95;
        if (team1_x[i] < -0.95) team1_x[i] = -0.95;
        if (team2_x[i] > 0.95) team2_x[i] = 0.95;
        if (team2_x[i] < -0.95) team2_x[i] = -0.95;

        // Team A animation updates
        if (game->players_teamA[i].state == FALLEN || game->players_teamA[i].state == EXHAUSTED) {
            if (!team1_animation[i].is_falling) {
                // Start new falling animation
                team1_animation[i].is_falling = 1;
                team1_animation[i].falling_frames = 0;
            }
            // Update ongoing animation
            if (team1_animation[i].is_falling && team1_animation[i].falling_frames < 10) {
                team1_animation[i].falling_frames++;
                float progress = (float)team1_animation[i].falling_frames / 10.0f;
                team1_animation[i].vertical_offset = -0.08f * (progress * progress);
            }
        } else {
            // Reset if player is no longer fallen
            team1_animation[i].is_falling = 0;
            team1_animation[i].vertical_offset = 0.0f;
        }

        // Team B animation updates (same logic as Team A)
        if (game->players_teamB[i].state == FALLEN || game->players_teamB[i].state == EXHAUSTED) {
            if (!team2_animation[i].is_falling) {
                team2_animation[i].is_falling = 1;
                team2_animation[i].falling_frames = 0;
            }
            if (team2_animation[i].is_falling && team2_animation[i].falling_frames < 10) {
                team2_animation[i].falling_frames++;
                float progress = (float)team2_animation[i].falling_frames / 10.0f;
                team2_animation[i].vertical_offset = -0.08f * (progress * progress);
            }
        } else {
            team2_animation[i].is_falling = 0;
            team2_animation[i].vertical_offset = 0.0f;
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
                rope_center = 0.0;
                // reset player animation structs
                for (int j = 0; j < PLAYERS_PER_TEAM; j++) {
                    team1_animation[j].is_falling = 0;
                    team2_animation[j].is_falling = 0;
                }

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

            // game over
            if(game->game_running == 0) {
                renderBigText(-0.1, 0.5, "GAME OVER!", 1.0, 0.0, 0.0);
            }

            if(game->round_running == 0) {
                printf("Round finished\n");
                display_team_win = 1;
            }

            if(display_team_win) {
                display_timer++;
               
                // Display the winning team
                if(game->last_winner == TEAM_A) {
                    renderBigText(-0.1, 0.5, "RED TEAM WINS!", 1.0, 0.0, 0.0);
                } else if(game->last_winner == TEAM_B) {
                    renderBigText(-0.1, 0.5, "BLUE TEAM WINS!", 0.0, 0.0, 1.0);
                } else {
                    renderBigText(-0.1, 0.5, "DRAW!", 1.0, 1.0, 1.0);
                }

                if(display_timer > 50) {
                    display_team_win = 0;
                    display_timer = 0;
                }
            }
        }
    }

    // Rest of display function remains the same
    // Draw rope segments
    float grip_offset = 0.05;

    // Red team rope segments
    for (int i = 0; i < PLAYERS_PER_TEAM - 1; i++) {
        drawRopeSegment(
            team1_x[i] + grip_offset, -0.1,
            team1_x[i+1] - grip_offset, -0.1
        );
    }

    // Blue team rope segments
    for (int i = PLAYERS_PER_TEAM - 1; i > 0; i--) {
        drawRopeSegment(
            team2_x[i] + grip_offset, -0.1,
            team2_x[i-1] - grip_offset, -0.1
        );
    }

    // Middle rope segment
    drawRopeSegment(
        team1_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1,
        team2_x[PLAYERS_PER_TEAM-1] + grip_offset, -0.1
    );

    // Draw the scoreboard
    drawScoreboard();

    glFlush();
}


int main(int argc, char** argv) {

    // Check command-line argument

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <config buffer> <fd>\n",argv[0]);
        return 1;
    }
    int fd = atoi(argv[2]);

    // Deserialize the config
    deserialize_config(&config, argv[1]);

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