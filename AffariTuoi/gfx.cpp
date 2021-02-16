#include "gfx.h"
#include <fstream>
#include <string>
#include <sstream>

inline void DrawChar(HANDLE hConsole, char character)
{
	WriteConsoleA(hConsole, (char*)&character, 1, NULL, NULL);
}


inline void DrawCharAtPos(HANDLE hConsole, char character, COORD pos)
{
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsoleA(hConsole, (char*)&character, 1, NULL, NULL);
}


inline void DrawString(HANDLE hConsole, string text)
{
	WriteConsoleA(hConsole, text.c_str(), text.length(), NULL, NULL);
}


inline void DrawStringAtPos(HANDLE hConsole, string text, COORD pos)
{
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsoleA(hConsole, text.c_str(), text.length(), NULL, NULL);
}


void DrawBox(HANDLE hConsole, COORD startPos, COORD endPos)
{
	DrawCharAtPos(hConsole, 0xC9, startPos);
	for (int i = startPos.X + 1; i < endPos.X; i++)
		DrawChar(hConsole, 0xCD);
	DrawChar(hConsole, 0xBB);


	for (short int i = 1; i < endPos.Y - startPos.Y; i++)
	{
		DrawCharAtPos(hConsole, 0xBA, { startPos.X, startPos.Y + i });
		DrawCharAtPos(hConsole, 0xBA, { endPos.X, startPos.Y + i });
	}


	DrawCharAtPos(hConsole, 0xC8, { startPos.X, endPos.Y });
	for (int i = startPos.X + 1; i < endPos.X; i++)
		DrawChar(hConsole, 0xCD);
	DrawChar(hConsole, 0xBC);
}


void DrawLine(HANDLE hConsole, COORD coord, int offset, char carattere, TipoLinea tipo)
{
	SetConsoleCursorPosition(hConsole, coord);

	if (tipo == Orizzontale)
	{
		for (int i = 0; i < offset; i++)
			DrawChar(hConsole, carattere);
	}
	else
	{
		for (short int i = 0; i < offset; i++)
		{
			DrawChar(hConsole, carattere);
			SetConsoleCursorPosition(hConsole, { coord.X, coord.Y + i + 1 });
		}
	}
}


void DrawBorders(HANDLE hConsole, COORD windowSize)
{
	DrawBox(hConsole, { 0, 0 }, windowSize);
}


void DrawStringInBox(HANDLE hConsole, COORD coord, string parola, WORD attributiParola, WORD attributiBox)
{
	SetConsoleTextAttribute(hConsole, attributiBox);

	DrawBox(hConsole, coord, { coord.X + (short int)parola.length() + 1, coord.Y + 2 });
	SetConsoleTextAttribute(hConsole, attributiParola);
	DrawStringAtPos(hConsole, parola, { coord.X + 1, coord.Y + 1 });

	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


void HideCursor(HANDLE hConsole)
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &info);
}


void ShowCursor(HANDLE hConsole)
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hConsole, &info);
}


FrameData GetAnimatedFramesFromFiles(string fileRoot, int spriteNumber)
{
	FrameData frames = new string[spriteNumber];
	
	fstream file;
	string filePath, temp;

	for (int i = 0; i < spriteNumber; i++)
	{
		filePath = fileRoot + to_string(i) + ".txt";
		file.open(filePath);

		if (!file.is_open())
			return NULL;

		frames[i] = "";

		while (getline(file, temp))
			frames[i] += temp + "\n";

		file.close();
	}

	return frames;
}


void DrawFrame(HANDLE hConsole, FrameData frames, int frame, COORD pos)
{
	stringstream ss;
	string temp;

	ss << frames[frame];

	short int i = 0;
	while (getline(ss, temp))
	{
		DrawStringAtPos(hConsole, temp, { pos.X, pos.Y + i });
		i++;
	}
}