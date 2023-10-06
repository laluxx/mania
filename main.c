#include "raylib.h"
#include "math.h"
#include <stddef.h>

#define COLUMNS 4
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define NOTE_RADIUS 55
#define TARGET_RADIUS (NOTE_RADIUS + 5)  // TARGET_RADIUS is 5 units larger than NOTE_RADIUS
#define TARGET_SPACING (2.8 * NOTE_RADIUS) // TARGET_SPACING is 3 times the NOTE_RADIUS


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

// added
bool autoPlay = false;
Sound hitSound;




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


bool GetKeyPress(int column) {
    switch(column) {
        case 0: return IsKeyPressed(KEY_D);
        case 1: return IsKeyPressed(KEY_F);
        case 2: return IsKeyPressed(KEY_J);
        case 3: return IsKeyPressed(KEY_K);
        default: return false;
    }
}

Note* GetClosestNote(int column) {
    Note* closestNote = NULL;
    float minDistance = SCREEN_HEIGHT;  // An arbitrary large value

    for (int j = 0; j < MAX_NOTES; j++) {
        if (notes[column][j].active) {
            float distance = fabs(notes[column][j].position.y - (SCREEN_HEIGHT - TARGET_RADIUS));
            if (distance < minDistance) {
                minDistance = distance;
                closestNote = &notes[column][j];
            }
        }
    }
    return closestNote;
}

void HandleNoteHit(Note* note, float distance) {
    if (distance < 30) {
        lastHit = 1;  // Marvelous
    } else if (distance < 50) {
        lastHit = 2;  // Perfect
    } else if (distance < 80) {
        lastHit = 3;  // Good
    } else {
        lastHit = 4;  // Bad
    }

    note->active = false;
    note->hitTime = GetTime() * 1000;
    hitFade = 0.5;
    
    // Play the sound every time the function is called, regardless of autoplay or manual hit
    PlaySound(hitSound);
}

void MoveNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j++) {
            if (notes[i][j].active) {
                notes[i][j].position.y += notes[i][j].speed;
                if (notes[i][j].position.y > SCREEN_HEIGHT + NOTE_RADIUS) {
                    notes[i][j].active = false;
                }
            }
        }
    }
}

void UpdateNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        bool keyPressed = GetKeyPress(i);
        Note* closestNote = GetClosestNote(i);
        float distance = closestNote ? fabs(closestNote->position.y - (SCREEN_HEIGHT - TARGET_RADIUS)) : SCREEN_HEIGHT;

        if (autoPlay && closestNote && distance < 10) {
            HandleNoteHit(closestNote, distance);
        } else if (keyPressed && closestNote) {
            HandleNoteHit(closestNote, distance);
        }
    }

    MoveNotes();

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

// int main(void) {
//     InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Game");
//     SetTargetFPS(144);
//     
//     InitializeNotes(); // Initial setup
//
//     while (!WindowShouldClose()) {
//
//         if (IsKeyPressed(KEY_A)) {
//             autoPlay = !autoPlay; // Toggle autoplay mode
//         }
//
//
//         if (IsKeyPressed(KEY_R)) {
//             InitializeNotes();
//         }
//
//
//
//         UpdateNotes();
//         Render();
//     }
//
//     CloseWindow();
//     return 0;
// }
//


int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Game");
    SetTargetFPS(144);

    InitAudioDevice();  // Initialize audio device
    hitSound = LoadSound("./samples/hitsound.ogg");
    
    InitializeNotes(); // Initial setup

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_A)) {
            autoPlay = !autoPlay; // Toggle autoplay mode
        }

        if (IsKeyPressed(KEY_R)) {
            InitializeNotes();
        }

        UpdateNotes();
        Render();
    }

    UnloadSound(hitSound);  // Unload the sound
    CloseAudioDevice();  // Close audio device

    CloseWindow();
    return 0;
}
