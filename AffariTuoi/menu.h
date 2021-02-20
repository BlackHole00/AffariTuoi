#pragma once
#include <Windows.h>

#include "gfx/base.h"

int Menu(HANDLE);
void DrawMainMenu(HANDLE, int, FrameData);
void DrawSelectionMenu(HANDLE, int, int);
bool ControllaSelectionMenuKeys(int&);