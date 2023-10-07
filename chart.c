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



// Define the current map here
Map currentMap = MAP_STREAM; // Set this to whichever you want to initialize

void ClearNotes() {
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < MAX_NOTES; j++) {
            notes[i][j].active = false; // Assuming "active" determines if a note is visible or should be processed.
        }
    }
}


void InitializeNotes() {
    ClearNotes();
    switch (currentMap) {
        case MAP_BASIC:
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
            break;
        case MAP_STREAM:
            int streamSpacing = 1;
            int alternatingPattern[] = {0, 1, 2, 3};
            int patternLength = sizeof(alternatingPattern) / sizeof(alternatingPattern[0]);
            int noteIndex = 0;

            for (int j = 0; j < MAX_NOTES; j += streamSpacing) {
                int col = alternatingPattern[noteIndex % patternLength];
                notes[col][j] = (Note) {
                    .position = { SCREEN_WIDTH / 2 + TARGET_SPACING * (col - COLUMNS / 2 + 0.5f), -NOTE_RADIUS * (j+1) * 6 },
                    .speed = 13,
                    .active = true,
                    .hitTime = -1
                };
                noteIndex++;
            }
            break;
        default:
            // Handle any undefined map logic here or just ignore
            break;
    }
}

// maybe put the in a commons.c file
void ChangeMap(Map newMap) {
    if (newMap < MAP_COUNT) {  // Ensure the map is valid
        currentMap = newMap;
        InitializeNotes();
    } else {
        // Handle an invalid map request here, like setting to a default or raising an error.
    }
}

void NextMap() {
    currentMap = (currentMap + 1) % MAP_COUNT;
    InitializeNotes();
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
