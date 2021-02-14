#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;

bool done = false;

const COORD GRANDEZZA_VALIGIA = { 3, 2 };
const int NUMERO_VALIGIE = 20;

int LeggiPremiNulliDaFile(string, int, string[], int&);
inline int NumeroRandomInRange(int, int);
BOOL WINAPI test(DWORD);

void DrawBox(HANDLE, COORD, COORD);
void DisegnaValigia(HANDLE, COORD, int);
void DisegnaPartita(HANDLE, bool[]);

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

	SetConsoleCtrlHandler(test, TRUE);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	string temp[25];
	int lung = 0;

	SetConsoleCursorPosition(hConsole, { 3, 6 });

	LeggiPremiNulliDaFile("nulli.txt", 3, temp, lung);

	DisegnaPartita(hConsole, valigie);

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

void DrawBox(HANDLE hConsole, COORD startPos, COORD endPos)
{
	SetConsoleCursorPosition(hConsole, startPos);

	char temp = 0xC9;

	WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	for (int i = startPos.X + 1; i < endPos.X; i++) 
	{
		temp = 0xCD;
		WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	}
	temp = 0xBB;
	WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	
	temp = 0xBA;
	for (short int i = 1; i < endPos.Y - startPos.Y; i++) 
	{
		SetConsoleCursorPosition(hConsole, {startPos.X, startPos.Y + i});
		WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);

		SetConsoleCursorPosition(hConsole, { endPos.X, startPos.Y + i });
		WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	}

	SetConsoleCursorPosition(hConsole, { startPos.X, endPos.Y });
	temp = 0xC8;
	WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	for (int i = startPos.X + 1; i < endPos.X; i++) 
	{
		temp = 0xCD;
		WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL);
	}
	temp = 0xBC;
	WriteConsoleA(hConsole, (char*)&temp, 1, NULL, NULL); 
}


void DisegnaValigia(HANDLE hConsole, COORD coord, int num)
{
	string temp;
	temp = to_string(num);

	DrawBox(hConsole, coord, { coord.X + GRANDEZZA_VALIGIA.X, coord.Y + GRANDEZZA_VALIGIA.Y });
	SetConsoleCursorPosition(hConsole, { coord.X + 1, coord.Y + GRANDEZZA_VALIGIA.Y - 1});
	WriteConsoleA(hConsole, temp.c_str(), temp.length(), NULL, NULL);
}

void DisegnaPartita(HANDLE hConsole, bool valigie[])
{
	COORD coord = { 1, 1 };

	for (int i = 0; i < NUMERO_VALIGIE; i++)
	{
		if (valigie[i]) 
			DisegnaValigia(hConsole, coord, i + 1);

		coord.X += GRANDEZZA_VALIGIA.X + 1;

		if (i == 9)
		{
			coord.Y += GRANDEZZA_VALIGIA.Y + 1;
			coord.X = 1;
		}
	}
}