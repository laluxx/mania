#ifndef SMPARSER_H
#define SMPARSER_H

#include <stdbool.h>

#define MAX_NOTES 1000
#define COLUMNS 4
#define MAX_FILEPATH 255
#define MAX_LINE_LENGTH 512

typedef struct {
    char title[MAX_LINE_LENGTH];
    char subtitle[MAX_LINE_LENGTH];
    char artist[MAX_LINE_LENGTH];
    char genre[MAX_LINE_LENGTH];
    char banner[MAX_FILEPATH];
    char background[MAX_FILEPATH];
    char cdTitle[MAX_FILEPATH];
    char music[MAX_FILEPATH];
    float offset;
    float sampleStart;
    float sampleLength;
    float bpms;
    bool selectable;
} SMHeader;

typedef struct {
    int measure;
    int type[COLUMNS]; // Assuming 4 columns of notes for now
} SMNote;

bool ParseSMFile(const char* pathToPack, const char* songName, SMHeader* header, SMNote notes[MAX_NOTES]);

#endif // SMPARSER_H
