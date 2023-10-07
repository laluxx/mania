#include "raylib.h"
#include "math.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "SMparser.h"
#include "chart.h"
#include "window.h"

#define COLUMNS 4

// function to move into utils.c
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
    Vector2 position = { 10, 10 };  // Top-left corner with some padding
    /* Vector2 position = { SCREEN_WIDTH - MeasureText(cpsText, 24) - 10, 10 };  // Top-right corner with some padding */

    Color cpsColor = GREEN;
    float cpsFontSize = 54;
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

        if (IsKeyPressed(KEY_N)) {
            NextMap();
        }

        UpdateNotes();
        Render();
    }

    UnloadSound(hitSound);  // Unload the sound
    CloseAudioDevice();  // Close audio device

    CloseWindow();
    return 0;
}
