#include <iostream>
#include <Windows.h>
using namespace std;

#define _CLS system("cls")

typedef string* FrameData;

enum TipoLinea {
	Orizzontale,
	Verticale
};

inline void DrawChar(HANDLE, char);
inline void DrawCharAtPos(HANDLE, char, COORD);
inline void DrawString(HANDLE, string);
inline void DrawStringAtPos(HANDLE, string, COORD);

void DrawBox(HANDLE, COORD, COORD);
void DrawLine(HANDLE, COORD, int, char, TipoLinea);
void DrawBorders(HANDLE, COORD);
void DrawStringInBox(HANDLE, COORD, string, WORD, WORD);

void HideCursor(HANDLE);
void HideCursor(HANDLE);

FrameData GetAnimatedFramesFromFiles(string, int);
void DrawFrame(HANDLE, FrameData, int, COORD);