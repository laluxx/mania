#include "SMparser.h"
#include <stdio.h>
#include <string.h>

bool ParseValue(const char* line, const char* key, char* dest, size_t destSize) {
    size_t keyLen = strlen(key);
    if (strncmp(line, key, keyLen) == 0) {
        strncpy(dest, line + keyLen, destSize - 1);
        dest[destSize - 1] = '\0';  // Ensure null termination
        char* newline = strchr(dest, '\n');
        if (newline) *newline = '\0';  // Replace newline with null terminator
        return true;
    }
    return false;
}

bool ParseSMFile(const char* pathToPack, const char* songName, SMHeader* header, SMNote notes[MAX_NOTES]) {
    char filepath[MAX_FILEPATH];
    snprintf(filepath, sizeof(filepath), "./songs/%s/%s/%s.sm", pathToPack, songName, songName);

    FILE* file = fopen(filepath, "r");
    if (!file) return false;

    char line[MAX_LINE_LENGTH];
    bool isNotesSection = false;
    int noteIndex = 0;

    while (fgets(line, sizeof(line), file)) {
        if (isNotesSection) {
            // Parsing notes (simplified for the sake of brevity)
            if (noteIndex < MAX_NOTES && strlen(line) >= COLUMNS) {
                notes[noteIndex].measure = noteIndex;  // Using noteIndex as measure for this basic example
                for (int i = 0; i < COLUMNS; i++) {
                    notes[noteIndex].type[i] = line[i] - '0';  // Convert char to int
                }
                noteIndex++;
            }
            if (strcmp(line, ";\n") == 0) {
                isNotesSection = false;
            }
            continue;
        }

        // Header parsing
        if (!ParseValue(line, "#TITLE:", header->title, sizeof(header->title))) {
            ParseValue(line, "#SUBTITLE:", header->subtitle, sizeof(header->subtitle));
            ParseValue(line, "#ARTIST:", header->artist, sizeof(header->artist));
            ParseValue(line, "#GENRE:", header->genre, sizeof(header->genre));
            // Continue with other fields...

            if (strncmp(line, "#NOTES:", 7) == 0) {
                isNotesSection = true;
            }
        }
    }

    snprintf(header->banner, sizeof(header->banner), "./songs/%s/%s/%s-bn.png", pathToPack, songName, songName);
    snprintf(header->background, sizeof(header->background), "./songs/%s/%s/%s-bg.png", pathToPack, songName, songName);
    snprintf(header->music, sizeof(header->music), "./songs/%s/%s/%s.ogg", pathToPack, songName, songName);

    fclose(file);
    return true;
}
