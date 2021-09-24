#pragma once

typedef enum PixelType
{
	EMPTY = 0x00,
	SAND,
	WATER
} PixelType;

typedef struct Pixel
{
	int id;
	PixelType type;
	Color color;
	float velocity;
	bool updated;
} Pixel;