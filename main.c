#include "portableMap.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{

	mapData* map;

	loadMap(&map, "D:\\ACE\\_personal_projects\\ImageManipulation\\makimabkg.ppm");
	filterMap(map, 0);
	storeMap(map, "D:\\ACE\\_personal_projects\\ImageManipulation\\out.ppm");
	dropMap(map);

	return 0;
}