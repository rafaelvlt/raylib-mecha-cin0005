#ifndef SCREEN_OPTIONS_H
#define SCREEN_OPTIONS_H
#include <raylib.h>

struct Systems;

typedef struct {
    //TBD
} OptionsData;

void InitOptionsScreen(struct Systems* systems, OptionsData* data);
void UpdateOptionsScreen(struct Systems* systems, OptionsData* data);
void DrawOptionsScreen(struct Systems* systems, OptionsData* data);
void DestroyOptionsScreen(struct Systems* systems, OptionsData* data);

#endif
