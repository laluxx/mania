#include "chart.h"
#include "window.h"
#include "math.h"
#include <stddef.h>

Note notes[COLUMNS][MAX_NOTES];
Sound hitSound;
int lastHit = -1;
double hitFade = 0;
bool autoPlay = false; // initialization of the first state
int clickCount = 0;
float cps = 0;
double lastTimeUpdated = 0;


void InitializeNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j += (i % 2 == 0 ? 3 : 5)) {
            notes[i][j] = (Note) {
                .position = { SCREEN_WIDTH / 2 + TARGET_SPACING * (i - COLUMNS / 2 + 0.5f), -NOTE_RADIUS * (j+1) * 6 },
                .speed = 10,
                .active = true,
                .hitTime = -1
            };
        }
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


// TO SEPARATE INTO input.c
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
