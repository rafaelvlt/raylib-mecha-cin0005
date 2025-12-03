#ifndef SCREEN_LOADOUT_H
#define SCREEN_LOADOUT_H
#include <raylib.h>

struct Systems;

typedef struct {
    //TBD
} LoadoutData;

void InitLoadoutScreen(struct Systems* systems, LoadoutData* data);
void UpdateLoadoutScreen(struct Systems* systems, LoadoutData* data);
void DrawLoadoutScreen(struct Systems* systems, LoadoutData* data);
void DestroyLoadoutScreen(struct Systems* systems, LoadoutData* data);

#endif

