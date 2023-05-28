#ifndef _PORTABLE_MAP_H
#define _PORTABLE_MAP_H

typedef struct mapData mapData;

extern void* loadMap(mapData** map, char* source);
extern char storeMap(mapData* map, char* destination);
extern void resizeWMap(mapData* map, int width, int top);
extern void resizeHMap(mapData* map, int hight, int top);
extern void filterMap(struct mapData* map, int offSet);
extern void dropMap(mapData* map);

#endif // !_PORTABLE_MAP_H

