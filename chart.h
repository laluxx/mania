#ifndef CHART_H
#define CHART_H

#include "raylib.h"

#define NOTE_RADIUS 55
#define TARGET_RADIUS (NOTE_RADIUS + 5)
#define TARGET_SPACING (2.8 * NOTE_RADIUS)
#define MAX_NOTES 1000
#define COLUMNS 4  // TODO support keys 5k 6k


typedef struct {
    Vector2 position;
    float speed;
    bool active;
    int hitTime;
} Note;


typedef enum {
    MAP_BASIC,
    MAP_STREAM,
    MAP_COUNT  // should always be last. It will be equal to the number of maps.
} Map;

// Global variable for notes (extern here, definition in .c file)
extern Note notes[COLUMNS][MAX_NOTES];
extern int clickCount;
extern float cps;
extern double lastTimeUpdated;
extern bool autoPlay;
extern double hitFade;
extern int lastHit;
extern Sound hitSound;
extern bool isPaused;






// Function declarations
void InitializeNotes();
void MoveNotes();
void UpdateNotes();
bool GetKeyPress(int column);
Note* GetClosestNote(int column);
void HandleNoteHit(Note* note, float distance);
void ChangeMap(Map newMap);
void NextMap(void);
void PrevMap(void);
void ClearNotes(void);
bool IsKeyHeld(int column);
#endif  // CHART_H
