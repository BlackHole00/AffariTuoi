#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;

#include "gfx.h"

const COORD GRANDEZZA_VALIGIA = { 3, 2 };
const COORD SCREEN_SIZE = { 43, 12 };
const int NUMERO_VALIGIE = 20;

const string VALIGIA_FILE_ROOT = "valigia_frame_";

int LeggiPremiNulliDaFile(string, int, string[], int&);
inline int NumeroRandomInRange(int, int);
BOOL WINAPI test(DWORD);

void DisegnaValigia(HANDLE, COORD, int);
void DisegnaPartita(HANDLE, bool[], int);

void DisegnaMenu(HANDLE);

float OttieniDelta();
bool AggiornaClock(float, int, int&);

int main() 
{
	//	Inilizzazione caratteri random
	srand(time(NULL));

	bool valigie[20] = {
		true,
		true,
		false,
		true,
		false,
		true,
		true,
		false,
		true,
		true,
		false,
		true,
		false,
		true,
		false,
		true,
		true,
		false,
		true,
		false,
	};

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//SetConsoleScreenBufferSize(hConsole, SCREEN_SIZE);
	SetConsoleCtrlHandler(test, TRUE);

	string temp[25];
	int lung = 0;

	SetConsoleCursorPosition(hConsole, { 3, 6 });

	LeggiPremiNulliDaFile("nulli.txt", 3, temp, lung);

	DisegnaMenu(hConsole);

	DisegnaPartita(hConsole, valigie, 3);

	SetConsoleCursorPosition(hConsole, { 0, 15 });

	return 0;
}


int LeggiPremiNulliDaFile(string filePath, int numPremi, string vectPremi[], int& lungVect)
{
	fstream file;
	file.open(filePath.c_str(), ios::in);

	string temp;

	if (file.is_open())
	{
		lungVect = 0;

		//	TODO trova un metodo per evitare di fare questo ciclo.
		int fileLung = 0;
		while (getline(file, temp))
			fileLung++;

		int* vect = new int[numPremi];
		vect[0] = NumeroRandomInRange(0, fileLung - numPremi);
		for (int i = 1; i < numPremi; i++)
			vect[i] = NumeroRandomInRange(vect[i-1] + 1, fileLung - numPremi + i);


		file.clear();
		file.seekg(0);


		int i = 0;
		lungVect = 0;
		while (lungVect < numPremi)
		{
			getline(file, temp);

			if (i == vect[lungVect])
			{
				vectPremi[lungVect] = temp;
				lungVect++;
			}

			i++;
		}

		delete[] vect;
		file.close();
	}
	else
		return -1;
}

inline int NumeroRandomInRange(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

BOOL WINAPI test(DWORD fdwCtrlType)
{
	system("echo funzia! > test.txt");

	return false;
}



void DisegnaValigia(HANDLE hConsole, COORD coord, int num)
{
	string temp;
	temp = to_string(num);

	if (num < 10)
		temp += " ";

	DrawBox(hConsole, coord, { coord.X + GRANDEZZA_VALIGIA.X, coord.Y + GRANDEZZA_VALIGIA.Y });
	SetConsoleCursorPosition(hConsole, { coord.X + 1, coord.Y + GRANDEZZA_VALIGIA.Y - 1});
	WriteConsoleA(hConsole, temp.c_str(), 2, NULL, NULL);
}



void DisegnaPartita(HANDLE hConsole, bool valigie[], int selected)
{
	DrawBorders(hConsole, SCREEN_SIZE);

	COORD coord = { 2, 1 };

	for (int i = 0; i < NUMERO_VALIGIE; i++)
	{
		if (valigie[i])
		{
			if (selected == i)
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
				DisegnaValigia(hConsole, coord, i + 1);
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			}
			else
			{
				DisegnaValigia(hConsole, coord, i + 1);
			}
		}

		coord.X += GRANDEZZA_VALIGIA.X + 1;

		if (i == 9)
		{
			coord.Y += GRANDEZZA_VALIGIA.Y + 1;
			coord.X = 2;
		}
	}

	DrawLine(hConsole, { 1, 8 }, 42, 0xCD, Orizzontale);
}



void DisegnaMenu(HANDLE hConsole)
{
	DrawBorders(hConsole, SCREEN_SIZE);
	HideCursor(hConsole);

	float delta = 0;
	int clock = 0;

	FrameData valigiaAnimata = GetAnimatedFramesFromFiles(VALIGIA_FILE_ROOT, 4);

	while (1)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		//DisegnaValigiaMenu(hConsole, clock);
		DrawFrame(hConsole, valigiaAnimata, clock, { 3, 1 });
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		
		if (clock % 2)
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		else
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		SetConsoleCursorPosition(hConsole, { 5, 10 });
		WriteConsoleA(hConsole, ">>>PREMI SPAZIO PER COMINCIARE<<<", 35, NULL, NULL);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);


		if (clock % 2)
			DrawStringInBox(hConsole, { 23, 2 }, "AFFARI TUOI", FOREGROUND_RED, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		else
			DrawStringInBox(hConsole, { 23, 2 }, "AFFARI TUOI", FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		

		if (AggiornaClock(OttieniDelta(), 4, clock))
		{
			_CLS;
			DrawBorders(hConsole, SCREEN_SIZE);
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			break;
		}
	}

	_CLS;
	delete[] valigiaAnimata;
}


float OttieniDelta()
{
	static time_t old;
	static time_t current = clock();

	old = current;
	current = clock();
	return current - old;
}


bool AggiornaClock(float delta, int valoreMassimoClock, int& clock)
{
	static float time = 0;

	time += delta;

	if (time >= 1000)
	{
		time = 0;
		
		clock++;
		if (clock == valoreMassimoClock)
			clock = 0;
		
		return true;
	}

	return false;
}