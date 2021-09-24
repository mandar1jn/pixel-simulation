#pragma once
#include "raylib.h"
#include "Pixel.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "raymath.h"

static Pixel *pixels = { 0 };

const float gravity = 1.98f;
static int simulationSpeed = 1;

const int width = 800;
const int height = 600;

static Pixel empty();
static Pixel sand();
static Pixel water();

static int GetId(int x, int y);
static void WriteData(int id, Pixel pixel);
static void UpdateSand(int x, int y);
static void UpdateWater(int x, int y);
static bool InFrame(int x, int y);
static bool TryCreate(int x, int y, Pixel pixel);
static bool GetRandomBool();
static void PlaceInSquare(int size, Pixel pixel);
static void UpdateVelocity(int x, int y);
static void ResetPixels();
bool HasPixel(int x, int y);
static bool right = true;

static void Update()
{
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		PlaceInSquare(50, water());
	}

	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
	{
		PlaceInSquare(50, sand());
	}

	if (IsKeyDown(KEY_UP))
	{
		simulationSpeed += 1;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		simulationSpeed = simulationSpeed - 1;
		if (simulationSpeed < 0)
		{
			simulationSpeed = 0;
		}
	}

	if (simulationSpeed > 0)
	{
		for (int i = 0; i < simulationSpeed; i++)
		{
			for (int y = height - 1; y >= 0; y--)
			{
				if (right)
				{
					for (int x = 0; x < width; x++)
					{
						if (HasPixel(x, y)) UpdateVelocity(x, y);
						Pixel pixel = pixels[GetId(x, y)];
						if (pixel.updated) continue;
						switch (pixel.type)
						{
						case SAND:
							UpdateSand(x, y);
							break;
						case WATER:
							UpdateWater(x, y);
							break;
						}
					}
					right = !right;
				}
				else
				{
					for (int x = width - 1; x > 0; x--)
					{
						if (HasPixel(x, y)) UpdateVelocity(x, y);
						Pixel pixel = pixels[GetId(x, y)];
						if (pixel.updated) continue;
						switch (pixel.type)
						{
						case SAND:
							UpdateSand(x, y);
							break;
						case WATER:
							UpdateWater(x, y);
							break;
						}
					}
					right = !right;
				}
			}
			ResetPixels();
		}
	}
}

static bool GetRandomBool()
{
	return rand() % 2;
}

static void ResetPixels()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pixels[GetId(x, y)].updated = false;
		}
	}
}

static bool HasPixel(int x, int y)
{
	return (pixels[GetId(x, y)].type != EMPTY);
}

static bool TryMove(int x, int y, int xOffset, int yOffset)
{
	Pixel pixel = pixels[GetId(x, y)];

	int xMax;
	int yMax;

	bool moved = false;

	int velocity = (int)pixel.velocity;

	for (int i = 1; i < velocity + 1; i++)
	{
			if (InFrame(x + xOffset * i, y + yOffset * i) && !HasPixel(x + xOffset * i, y + yOffset * i))
			{
				xMax = x + xOffset * i;
				yMax = y + yOffset * i;
				moved = true;
			}
	}

	if (moved)
	{
		WriteData(GetId(xMax, yMax), pixels[GetId(x, y)]);
		WriteData(GetId(x, y), empty());
		return moved;
	}
	
	return moved;
}

static bool TryCreate(int x, int y, Pixel pixel)
{
	if (!InFrame(x, y)) return false;
	if (HasPixel(x, y)) return false;
	WriteData(GetId(x, y), pixel);
	return true;
}

int main()
{
	srand((unsigned int)time((time_t)NULL));
	InitWindow(width, height, "Pixel Simulation");
	SetTargetFPS(60);
	pixels = MemAlloc(width * height * sizeof(Pixel));
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			WriteData(GetId(x, y), empty());
		}
	}
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		Update();
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				int id = GetId(x, y);

				Pixel pixel = pixels[id];
				DrawPixel(x, y, pixel.color);
			}
		}
		EndDrawing();
	}
	CloseWindow();
}

static int GetId(int x, int y)
{
	return (y * width + x);
}

static void WriteData(int id, Pixel pixel)
{
	pixel.id = id;
	pixels[id] = pixel;
	return;
}

void UpdateVelocity(int x, int y)
{
	Pixel *pixel = &pixels[GetId(x, y)];
	pixel->velocity += gravity * GetFrameTime();
	pixel->velocity = Clamp(pixel->velocity, -10, 10);
}

static Pixel empty()
{
	Pixel pixel = {0};
	pixel.type = EMPTY;
	pixel.color = BLACK;
	pixel.velocity = 0;
	return pixel;
}

static Pixel sand()
{
	Pixel pixel = { 0 };
	pixel.type = SAND;
	pixel.color = YELLOW;
	pixel.velocity = 1;
	return pixel;
}

static Pixel water()
{
	Pixel pixel = { 0 };
	pixel.type = WATER;
	pixel.color = BLUE;
	pixel.velocity = 1;
	return pixel;
}

static bool InFrame(int x, int y)
{
	if (x < 1 || x > width - 1 || y < 0 || y > height - 1) return false;
	return true;
}

void PlaceInSquare(int size, Pixel pixel)
{
	Vector2 mousePos = GetMousePosition();
	for (int i = (int)(mousePos.x - ((size - 1) / 2)); i < mousePos.x + ((size - 1) / 2); i++)
	{
		for (int j = (int)(mousePos.y - ((size - 1) / 2)); j < mousePos.y + ((size - 1) / 2); j++)
		{
			TryCreate(i, j, pixel);
		}
	}
}

static void UpdateSand(int x, int y)
{
	Pixel *pixel = &pixels[GetId(x, y)];
	pixel->updated = true;

	TryMove(x, y, 0, 1);
	if (TryMove(x, y, -1, 1)) return;
	if (TryMove(x, y, 1, 1)) return;
	pixel->velocity = pixel->velocity *= 0.2f;
	if (pixel->velocity < 1) pixel->velocity = 1;

	return;
}

static void UpdateWater(int x, int y)
{
	Pixel *pixel = &pixels[GetId(x, y)];
	pixel->updated = true;

	TryMove(x, y, 0, 1);
	if (TryMove(x, y, -1, 1)) return;
	if (TryMove(x, y, 1, 1)) return;

	pixel->velocity = pixel->velocity *= 0.2f;
	if (pixel->velocity < 1) pixel->velocity = 1;

	bool right = GetRandomBool();

	if (right)
	{
		if (TryMove(x, y, 1, 0)) return;
		if (TryMove(x, y, -1, 0)) return;
	}
	else
	{
		if (TryMove(x, y, -1, 0)) return;
		if (TryMove(x, y, 1, 0)) return;
	}
	return;
}