#include "SMparser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atof

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
    char buffer[MAX_LINE_LENGTH]; // Temp buffer for parsing
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
        if (ParseValue(line, "#OFFSET:", buffer, sizeof(buffer))) {
            header->offset = atof(buffer);
        } else if (ParseValue(line, "#SAMPLESTART:", buffer, sizeof(buffer))) {
            header->sampleStart = atof(buffer);
        } else if (ParseValue(line, "#SAMPLELENGTH:", buffer, sizeof(buffer))) {
            header->sampleLength = atof(buffer);
        } else if (ParseValue(line, "#BPMS:", buffer, sizeof(buffer))) {
            char* equals = strchr(buffer, '=');
            if (equals) {
                header->bpms = atof(equals + 1);
            }
        } else if (ParseValue(line, "#SELECTABLE:", buffer, sizeof(buffer))) {
            header->selectable = (strcmp(buffer, "YES") == 0);
        } else {
            ParseValue(line, "#TITLE:", header->title, sizeof(header->title));
            ParseValue(line, "#SUBTITLE:", header->subtitle, sizeof(header->subtitle));
            ParseValue(line, "#ARTIST:", header->artist, sizeof(header->artist));
            ParseValue(line, "#GENRE:", header->genre, sizeof(header->genre));
            // You can continue parsing other fields in a similar fashion...

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
