#include "raylib.h"
#include "math.h"
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 4

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

#define NOTE_RADIUS 55
#define TARGET_RADIUS (NOTE_RADIUS + 5)  // TARGET_RADIUS is 5 units larger than NOTE_RADIUS
#define TARGET_SPACING (2.8 * NOTE_RADIUS) // TARGET_SPACING is 3 times the NOTE_RADIUS


#define MAX_NOTES 1000

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

int clickCount = 0;
float cps = 0;
double lastTimeUpdated = 0;


void InitializeNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j += (i % 2 == 0 ? 3 : 5)) { // More varied spacing
            notes[i][j] = (Note) { 
                .position = { SCREEN_WIDTH / 2 + TARGET_SPACING * (i - COLUMNS / 2 + 0.5f), -NOTE_RADIUS * (j+1) * 6 }, 
                .speed = 15,
                .active = true, 
                .hitTime = -1 
            };
        }
    }
}

bool GetKeyPress(int column) {
    bool pressed = false;
    switch(column) {
        case 0: pressed = IsKeyPressed(KEY_D); break;
        case 1: pressed = IsKeyPressed(KEY_F); break;
        case 2: pressed = IsKeyPressed(KEY_J); break;
        case 3: pressed = IsKeyPressed(KEY_K); break;
    }
    
    if (pressed){
    clickCount++;
    }

    return pressed;
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

    if (autoPlay) {
    clickCount++;
    }

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


  // Update CPS
    if (GetTime() - lastTimeUpdated >= 1.0) {
        cps = clickCount;
        clickCount = 0;
        lastTimeUpdated = GetTime();
    }

}

// functions to move into separate files


Color Lerp(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    result.a = a.a + (b.a - a.a) * t;

    return result;
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

    // Render CPS counter with dynamic effects
    char cpsText[50];
    sprintf(cpsText, "CPS: %.2f", cps);
    Vector2 position = { SCREEN_WIDTH - MeasureText(cpsText, 24) - 10, 10 };  // Top-right corner with some padding

    Color cpsColor = GREEN;
    float cpsFontSize = 24;
    if (cps > 5 && cps <= 10) {
        cpsColor = Lerp(GREEN, YELLOW, (cps - 5) / 5);
    } else if (cps > 10) {
        cpsColor = Lerp(YELLOW, RED, (cps - 10) / 10);
        position.x += (rand() % 8) - 4;  // Random movement for the "crazy" effect
        position.y += (rand() % 8) - 4;
        cpsFontSize += (cps - 10);  // Increase font size a bit for emphasis
    }

    DrawText(cpsText, position.x, position.y, cpsFontSize, cpsColor);

    EndDrawing();
}

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
