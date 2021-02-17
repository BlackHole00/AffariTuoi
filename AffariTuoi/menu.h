#pragma once
#include <Windows.h>
#include "gfx.h"

int Menu(HANDLE, COORD);
void DrawMainMenu(HANDLE, int, FrameData, COORD);
void DrawSelectionMenu(HANDLE, int, int, COORD);
bool ControllaSelectionMenuKeys(int&);