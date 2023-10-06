#include "raylib.h"
#include "math.h"

#define COLUMNS 4
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NOTE_RADIUS 25    // Increased size
#define TARGET_RADIUS 30  // Increased size
#define TARGET_SPACING 80 // Increased spacing
#define MAX_NOTES 100

typedef struct {
    Vector2 position;
    float speed;
    bool active;
    int hitTime;
} Note;

Note notes[COLUMNS][MAX_NOTES]; 
int lastHit = -1;
double hitFade = 0;

void InitializeNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j += (i % 2 == 0 ? 3 : 5)) { // More varied spacing
            notes[i][j] = (Note) { 
                .position = { SCREEN_WIDTH / 2 + TARGET_SPACING * (i - COLUMNS / 2 + 0.5f), -NOTE_RADIUS * (j+1) * 6 }, 
                .speed = 10,
                .active = true, 
                .hitTime = -1 
            };
        }
    }
}

void UpdateNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j++) {
            if (notes[i][j].active) {
                notes[i][j].position.y += notes[i][j].speed;

                if (notes[i][j].position.y > SCREEN_HEIGHT + NOTE_RADIUS) {
                    notes[i][j].active = false;
                }

                if ((IsKeyPressed(KEY_D) && i == 0) || 
                    (IsKeyPressed(KEY_F) && i == 1) || 
                    (IsKeyPressed(KEY_J) && i == 2) || 
                    (IsKeyPressed(KEY_K) && i == 3)) {

                    float distance = fabs(notes[i][j].position.y - (SCREEN_HEIGHT - TARGET_RADIUS));

                    if (distance < 10) {
                        notes[i][j].active = false;
                        notes[i][j].hitTime = GetTime()*1000;
                        lastHit = 1;  // Marvelous
                        hitFade = 0.5;
                    } else if (distance < 20) {
                        notes[i][j].active = false;
                        notes[i][j].hitTime = GetTime()*1000;
                        lastHit = 2;  // Perfect
                        hitFade = 0.5;
                    } else if (distance < 40) {
                        notes[i][j].active = false;
                        notes[i][j].hitTime = GetTime()*1000;
                        lastHit = 3;  // Good
                        hitFade = 0.5;
                    } else {
                        lastHit = 4;  // Bad
                        hitFade = 0.5;
                    }
                }
            }
        }
    }
    if (hitFade > 0) hitFade -= GetFrameTime();
}

void Render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw target circles
    for (int i = 0; i < COLUMNS; i++) {
        DrawCircleLines(SCREEN_WIDTH / 2 + TARGET_SPACING * (i - COLUMNS / 2 + 0.5f), SCREEN_HEIGHT - TARGET_RADIUS, TARGET_RADIUS, DARKGRAY);
    }

    // Draw falling notes
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j++) {
            if (notes[i][j].active) {
                DrawCircleV(notes[i][j].position, NOTE_RADIUS, BLACK);
            }
        }
    }

    // Draw hit feedback
    if (hitFade > 0) {
        Color hitColor;
        const char* hitText;
        switch(lastHit) {
            case 1: hitColor = GREEN; hitText = "Marvelous"; break;
            case 2: hitColor = BLUE; hitText = "Perfect"; break;
            case 3: hitColor = YELLOW; hitText = "Good"; break;
            case 4: hitColor = RED; hitText = "Bad"; break;
            default: hitColor = RAYWHITE; hitText = ""; break;
        }
        DrawText(hitText, SCREEN_WIDTH/2 - MeasureText(hitText, 30)/2, SCREEN_HEIGHT/2 - 15, 30, Fade(hitColor, hitFade));
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Game");
    SetTargetFPS(144);
    
    InitializeNotes(); // Initial setup

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            InitializeNotes();
        }

        UpdateNotes();
        Render();
    }

    CloseWindow();
    return 0;
}
