#include "portableMap.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

struct mapData
{
	char type[3];
	int height;
	int width;
	int maxWhite;
	int* pixelArrayR;
	int* pixelArrayG;
	int* pixelArrayB;
};

struct HSV
{
	double* Hue;
	double* Saturation;
	double* Value;
};

#define defaultOff 3

static void loadBitMap(struct mapData* map, FILE* sourceptr);
static void loadGrayMap(struct mapData* map, FILE* sourceptr);
static void loadPixMap(struct mapData* map, FILE* sourceptr);
static void storeBitMap(struct mapData* map, FILE* sourceptr);
static void storeGrayMap(struct mapData* map, FILE* sourceptr);
static void storePixMap(struct mapData* map, FILE* sourceptr);
static void resizeArray(int* array, int oldSize, int newSize, int top);
static void resizeArray2(int* array, int oldSize, int newSize, int delta, int top);
static int maxVal(int* array, int size);
static inline void normalize(struct mapData* map);
static int cmpfuncInt(const void* a, const void* b);
static int cmpfuncDouble(const void* a, const void* b);
static void* getOffArray(void* array, int pos, int offSet, int sizeH, int sizeW, int sizeOfElem);
static void filterGrayMap(struct mapData* map, int offSet);
static void filterPixMap(struct mapData* map, int offSet);
static void RGB_HSV(struct mapData* map, struct HSV* hsv);
static void HSV_RGB(struct mapData* map, struct HSV* hsv);
static void* filterArray(void* array, int offSet, int sizeH, int sizeW, int sizeOfElem, void* cmp);
void* loadMap(struct mapData** map, char* source);
char storeMap(mapData* map, char* destination);
void resizeWMap(mapData* map, int width, int top);
void resizeHMap(mapData* map, int height, int top);
void filterMap(struct mapData* map, int offSet);
void dropMap(mapData* map);


static void loadBitMap(struct mapData* map, FILE* sourceptr)
{
	int imgSize = map->height * map->width;
	map->maxWhite = -1;
	map->pixelArrayR = (int*)malloc(sizeof(int) * imgSize);
	map->pixelArrayG = NULL;
	map->pixelArrayB = NULL;

	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fscanf_s(sourceptr, "%d", map->pixelArrayR + bitItr);
	}

}

static void loadGrayMap(struct mapData* map, FILE* sourceptr)
{
	int imgSize = map->height * map->width;
	fscanf_s(sourceptr, "%d", &map->maxWhite);
	map->pixelArrayR = (int*)malloc(sizeof(int) * imgSize);
	map->pixelArrayG = NULL;
	map->pixelArrayB = NULL;

	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fscanf_s(sourceptr, "%d", map->pixelArrayR + bitItr);
	}
}

static void loadPixMap(struct mapData* map, FILE* sourceptr)
{
	int imgSize = map->height * map->width;
	fscanf_s(sourceptr, "%d", &map->maxWhite);
	map->pixelArrayR = (int*)malloc(sizeof(int) * imgSize);
	map->pixelArrayG = (int*)malloc(sizeof(int) * imgSize);
	map->pixelArrayB = (int*)malloc(sizeof(int) * imgSize);

	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fscanf_s(sourceptr, "%d", map->pixelArrayR + bitItr);
		fscanf_s(sourceptr, "%d", map->pixelArrayG + bitItr);
		fscanf_s(sourceptr, "%d", map->pixelArrayB + bitItr);
	}
}

void* loadMap(struct mapData** map, char* source)
{
	struct mapData* loacalMap;
	FILE* sourceptr;
	char type[3] = "00\0";

	fopen_s(&sourceptr, source, "r");

	if (sourceptr == NULL)
	{
		return  NULL;
	}

	fgets(type, 3, sourceptr);

	char fileType = 0;

	fileType |= (!(strcmp(type, "P1") & 1)) << 0;
	fileType |= (!(strcmp(type, "P2") & 1)) << 1;
	fileType |= (!(strcmp(type, "P3") & 1)) << 2;

	if (fileType == 0)
	{
		return NULL;
	}
	else
	{
		loacalMap = (struct mapData*)malloc(sizeof(struct mapData));
		strcpy_s(loacalMap->type, 3, type);
	}

	fscanf_s(sourceptr, "%d", &loacalMap->width);
	fscanf_s(sourceptr, "%d", &loacalMap->height);

	switch (fileType)
	{
	case 1:
		loadBitMap(loacalMap, sourceptr);
		break;
	case 2:
		loadGrayMap(loacalMap, sourceptr);
		break;
	case 4:
		loadPixMap(loacalMap, sourceptr);
		break;
	default:
		return NULL;
		break;
	}

	fclose(sourceptr);

	*map = loacalMap;
	return map;
}

static void storeBitMap(struct mapData* map, FILE* destptr)
{
	int imgSize = map->height * map->width;
	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fprintf_s(destptr, "%d ", *(map->pixelArrayR + bitItr));
	}
}

static void storeGrayMap(struct mapData* map, FILE* destptr)
{
	fprintf_s(destptr, "%d\n", map->maxWhite);
	int imgSize = map->height * map->width;
	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fprintf_s(destptr, "%d ", *(map->pixelArrayR + bitItr));
	}
}

static void storePixMap(struct mapData* map, FILE* destptr)
{
	fprintf_s(destptr, "%d\n", map->maxWhite);
	int imgSize = map->height * map->width;
	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		fprintf_s(destptr, "%d ", *(map->pixelArrayR + bitItr));
		fprintf_s(destptr, "%d ", *(map->pixelArrayG + bitItr));
		fprintf_s(destptr, "%d ", *(map->pixelArrayB + bitItr));
	}
}

char storeMap(struct mapData* map, char* destination)
{
	FILE* destptr;
	fopen_s(&destptr, destination, "w");

	if (destptr == NULL)
	{
		return 1;
	}

	fprintf_s(destptr, "%c%c\n", map->type[0], map->type[1]);
	fprintf_s(destptr, "%d ", map->width);
	fprintf_s(destptr, "%d\n", map->height);

	switch (map->type[0] + map->type[1])
	{
	case 'P' + '1':
		storeBitMap(map, destptr);
		break;
	case 'P' + '2':
		storeGrayMap(map, destptr);
		break;
	case 'P' + '3':
		storePixMap(map, destptr);
		break;
	default:
		return 1;
		break;
	}

	fclose(destptr);

	return 0;

}

static void resizeArray(int* array, int oldSize, int newSize, int top)
{
	if (array == NULL)
	{
		return;
	}

	if (newSize < 1 || newSize > oldSize)
	{
		return;
	}

	int offset = 0;
	if (top == 1)
	{
		offset = oldSize - newSize + 1;
	}

	memcpy(array, array + offset, newSize * sizeof(int));
	array = (int*)realloc(array, newSize * sizeof(int));
}

static void resizeArray2(int* array, int oldSize, int newSize, int delta, int top)
{

	if (array == NULL)
	{
		return;
	}

	if (newSize < 1 || newSize > oldSize)
	{
		return;
	}

	int dataSize = newSize / delta;

	int offset = 0;
	if (top == 1)
	{
		offset = oldSize / delta - dataSize + 1;
	}

	int offItr = 0;
	for (offItr = 0; offItr < delta; offItr++)
	{
		memcpy(array + offItr * dataSize, array + offset + offItr * (oldSize / delta), dataSize * sizeof(int));
	}

	array = (int*)realloc(array, newSize * sizeof(int));
}

static int maxVal(int* array, int size)
{
	if (array == NULL)
	{
		return 0;
	}

	int val = 0;

	for (int itr = 0; itr < size; itr++)
	{
		if (array[itr] > val)
		{
			val = array[itr];
		}
	}

	return val;

}

static inline void normalize(struct mapData* map)
{
	int ret1 = maxVal(map->pixelArrayR, map->width * map->height);
	int ret2 = maxVal(map->pixelArrayG, map->width * map->height);
	int ret3 = maxVal(map->pixelArrayB, map->width * map->height);
	map->maxWhite = max(max(ret1, ret2), ret3);
}

extern void resizeWMap(struct mapData* map, int width, int top)
{
	resizeArray2(map->pixelArrayR, map->height * map->width, width * map->height, map->height, top);
	resizeArray2(map->pixelArrayG, map->height * map->width, width * map->height, map->height, top);
	resizeArray2(map->pixelArrayB, map->height * map->width, width * map->height, map->height, top);
	map->width = width;
	normalize(map);
}

extern void resizeHMap(struct mapData* map, int height, int top)
{
	resizeArray(map->pixelArrayR, map->height * map->width, height * map->width, top);
	resizeArray(map->pixelArrayG, map->height * map->width, height * map->width, top);
	resizeArray(map->pixelArrayB, map->height * map->width, height * map->width, top);
	map->height = height;
	normalize(map);
}

static int cmpfuncInt(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

static int cmpfuncDouble(const void* a, const void* b)
{
	return (*(double*)a - *(double*)b);
}

static void* getOffArray(void* array, int pos, int offSet, int sizeH, int sizeW, int sizeOfElem)
{

	void* newArr = malloc(sizeOfElem * offSet * offSet);	

	int hItr = 0;
	int wItr = 0;

	int offH = pos / sizeH;
	int offW = pos % sizeH;

	int nIdx = 0;

	for (hItr = (offH - offSet / 2); hItr < (offH + offSet / 2) + 1; hItr++)
	{
		for (wItr = (offW - offSet / 2); wItr < (offW + offSet / 2) + 1; wItr++)
		{
			int idx1 = hItr;
			int idx2 = wItr;

			if (hItr < 0)
			{
				idx1 = 0;
			}
			if (hItr >= sizeH)
			{
				idx1 = sizeH - 1;
			}
			if (wItr < 0)
			{
				idx2 = 0;
			}
			if (hItr >= sizeW)
			{
				idx2 = sizeW - 1;
			}
			
			memcpy((char*)newArr + (nIdx * sizeOfElem), (char*)array + ((sizeH * idx1 + idx2) * sizeOfElem), sizeOfElem);
			nIdx += 1;
		}
	}

	return newArr;
}

static void* filterArray(void* array, int offSet, int sizeH, int sizeW, int sizeOfElem, void* cmp)
{
	int size = sizeW * sizeH;
	void* newArr = malloc(sizeOfElem * size);

	int bitItr = 0;
	for (bitItr = 0; bitItr < size; bitItr++)
	{
		void* offArr = getOffArray(array, bitItr, offSet, sizeH, sizeW, sizeOfElem);
		qsort(offArr, offSet * offSet, sizeOfElem, cmp);
		memcpy((char*)newArr + bitItr * sizeOfElem, (char*)offArr + ((offSet * offSet) / 2) * sizeOfElem, sizeOfElem);
		free(offArr);
	}

	return newArr;

}

static void filterGrayMap(struct mapData* map, int offSet)
{
	int* newArr = (int*)filterArray(map->pixelArrayR, offSet, map->height, map->width, sizeof(int),&cmpfuncInt);
	int* old = map->pixelArrayR;
	map->pixelArrayR = newArr;
	free(old);
	normalize(map);
}

static void RGB_HSV(struct mapData* map, struct HSV* hsv)
{
	int imgSize = map->height * map->width;

	int bitItr = 0;

	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		float R = map->pixelArrayR[bitItr] / 255.f;
		float G = map->pixelArrayG[bitItr] / 255.f;
		float B = map->pixelArrayB[bitItr] / 255.f;

		float Cmax = max(R, max(G, B));
		float Cmin = min(R, min(G, B));

		float delta = Cmax - Cmin;

		if (delta == 0)
		{
			hsv->Hue[bitItr] = delta;
		}
		else {
			if (Cmax == R)
			{
				hsv->Hue[bitItr] = fmod((60 * (G - B) / delta) + 360, 360.0);
			}
			if (Cmax == G)
			{
				hsv->Hue[bitItr] = fmod((60 * (B - R) / delta) + 120, 360.0);

			}
			if (Cmax == B)
			{
				hsv->Hue[bitItr] = fmod((60 * (R - G) / delta) + 240, 360.0);
			}

			if (hsv->Hue[bitItr] < 0)
			{
				hsv->Hue[bitItr] += 360;

			}
		}

		hsv->Saturation[bitItr] = Cmax != 0.f ? (float)(delta / Cmax) : Cmax;
		hsv->Value[bitItr] = Cmax;
	}
}

static void HSV_RGB(struct mapData* map, struct HSV* hsv)
{
	int imgSize = map->height * map->width;

	int bitItr = 0;
	for (bitItr = 0; bitItr < imgSize; bitItr++)
	{
		float c = hsv->Saturation[bitItr] * hsv->Value[bitItr];
		float x = c * (1 - abs(fmod(hsv->Hue[bitItr] / 60.f, 2) - 1));
		float m = hsv->Value[bitItr] - c;

		float R = 0;
		float G = 0;
		float B = 0;

		if (hsv->Hue[bitItr] < 60)
		{
			R = c;
			G = x;
			B = 0;
		}
		if (hsv->Hue[bitItr] >= 60 && hsv->Hue[bitItr] < 120)
		{
			R = x;
			G = c;
			B = 0;
		}
		if (hsv->Hue[bitItr] >= 120 && hsv->Hue[bitItr] < 180)
		{
			R = 0;
			G = c;
			B = x;
		}
		if (hsv->Hue[bitItr] >= 180 && hsv->Hue[bitItr] < 240)
		{
			R = 0;
			G = x;
			B = c;
		}
		if (hsv->Hue[bitItr] >= 240 && hsv->Hue[bitItr] < 300)
		{
			R = x;
			G = 0;
			B = c;
		}
		if (hsv->Hue[bitItr] >= 300 && hsv->Hue[bitItr] < 360)
		{
			R = c;
			G = 0;
			B = x;
		}

		map->pixelArrayR[bitItr] = (R + m) * 255;
		map->pixelArrayG[bitItr] = (G + m) * 255;
		map->pixelArrayB[bitItr] = (B + m) * 255;

	}

}

static void filterPixMap(struct mapData* map, int offSet)
{
	struct HSV hsvData;
	int imgSize = map->height * map->width;
	hsvData.Hue = (float*)malloc(sizeof(float) * imgSize);
	hsvData.Saturation = (float*)malloc(sizeof(float) * imgSize);
	hsvData.Value = (float*)malloc(sizeof(float) * imgSize);

	RGB_HSV(map, &hsvData);

	float* newArr;
	float* old;

	newArr = (float*)filterArray(hsvData.Hue, defaultOff, map->height, map->width, sizeof(float), &cmpfuncDouble);
	old = hsvData.Hue;
	hsvData.Hue = newArr;
	free(old);

	newArr = (float*)filterArray(hsvData.Saturation, defaultOff, map->height, map->width, sizeof(float), &cmpfuncDouble);
	old = hsvData.Saturation;
	hsvData.Saturation = newArr;
	free(old);

	newArr = (float*)filterArray(hsvData.Value, defaultOff, map->height, map->width, sizeof(float), &cmpfuncDouble);
	old = hsvData.Value;
	hsvData.Value = newArr;
	free(old);

	HSV_RGB(map, &hsvData);

	free(hsvData.Hue);
	free(hsvData.Saturation);
	free(hsvData.Value);

	normalize(map);
}

void filterMap(struct mapData* map, int offSet)
{

	if (offSet < defaultOff)
	{
		offSet = defaultOff;
	}

	switch (map->type[0] + map->type[1])
	{
	case 'P' + '2':
		filterGrayMap(map, offSet);
		break;
	case 'P' + '3':
		filterPixMap(map, offSet);
		break;
	case 'P' + '1':
	default:
		return;
		break;
	}
}

void dropMap(struct mapData* map)
{
	if (map == NULL)
	{
		return;
	}
	else
	{
		free(map->pixelArrayR);

		if (!strcmp(map->type, "P6"))
		{
			free(map->pixelArrayG);
			free(map->pixelArrayB);
		}

		free(map);
	}
}