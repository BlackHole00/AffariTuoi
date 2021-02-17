#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;

#include "gfx.h"
#include "menu.h"
#include "timeUtilis.h"

const COORD GRANDEZZA_VALIGIA = { 3, 2 };
const COORD SCREEN_SIZE = { 43, 12 };
const int NUMERO_VALIGIE = 20;

const string DOTTORE_FILE_ROOT = "dottore_frame_";

int LeggiPremiNulliDaFile(string, int, string[], int&);
inline int NumeroRandomInRange(int, int);
BOOL WINAPI test(DWORD);

void DisegnaValigia(HANDLE, COORD, int);
void DisegnaPartita(HANDLE, bool[], int, int, string);
void DisegnaDottore(HANDLE, int, FrameData, string, int);

float Gioco(HANDLE);
float SchermataDottore(HANDLE);

int ControllaInputGioco(int&, bool[]);

int main() 
{
	//	Inilizzazione caratteri random
	srand(time(NULL));

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCtrlHandler(test, TRUE);

	string temp[25];
	int lung = 0;

	LeggiPremiNulliDaFile("nulli.txt", 3, temp, lung);

	int res = Menu(hConsole, SCREEN_SIZE);
	_CLS;
	if (res)	//	res == 1
	{
		SetConsoleCursorPosition(hConsole, { 0, 0 });
		DrawStringAtPos(hConsole, "DA IMPLEMENTARE!!!\n", {0, 0});
	}
	else
	{
		Gioco(hConsole);
	}

	SetConsoleCursorPosition(hConsole, { 0, SCREEN_SIZE.Y + 1 });

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


/*	Funzione DisegnaValigia
* 
*	Serve per disegnare una valigia con un numero all'interno
*/
void DisegnaValigia(HANDLE hConsole, COORD coord, int num)
{
	string temp;
	temp = to_string(num);

	//	Aggiungiamo un carattere nel caso stiamo utilizzando un colore custom per il background
	if (num < 10)
		temp += " ";

	DrawBox(hConsole, coord, { coord.X + GRANDEZZA_VALIGIA.X, coord.Y + GRANDEZZA_VALIGIA.Y });
	DrawStringAtPos(hConsole, temp, { coord.X + 1, coord.Y + GRANDEZZA_VALIGIA.Y - 1 });
}


float Gioco(HANDLE hConsole)
{
	int clock = 0;
	int selected = 0;
	int valigiaGiocatore = -1;
	int valigieRimanenti = 20;
	string messaggio = "Seleziona un pacco!!!";
	int res = 0;

	bool valigie[20] = {
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
	};

	DisegnaPartita(hConsole, valigie, valigiaGiocatore, selected, messaggio);

	do
	{
		res = (ControllaInputGioco(selected, valigie));

		if (AggiornaClock(OttieniDelta(), 4, clock) || res == 1)
			DisegnaPartita(hConsole, valigie, valigiaGiocatore, selected, messaggio);

		if (res == 2)
		{
			valigie[selected] = false;
			valigiaGiocatore = selected;
			valigieRimanenti--;

			messaggio = "Hai selezionato il pacco " + to_string(selected + 1) + (string)"!";

			selected++;
			if (selected > 19)
				selected = 0;

			DisegnaPartita(hConsole, valigie, valigiaGiocatore, selected, messaggio);
		}
	} while (res != 2);

	while (valigieRimanenti > 0)
	{
		int res = (ControllaInputGioco(selected, valigie));

		if (AggiornaClock(OttieniDelta(), 4, clock) || res == 1)
			DisegnaPartita(hConsole, valigie, valigiaGiocatore, selected, messaggio);

		if (res == 2)
		{
			valigie[selected] = false;
			valigieRimanenti--;

			messaggio = "Hai aperto il pacco " + to_string(selected + 1) + (string)"!";

			if (valigieRimanenti > 0)
			{
				do {
					selected++;
					if (selected > 19)
						selected = 0;
				} while (!valigie[selected]);
			}

			SchermataDottore(hConsole);

			DisegnaPartita(hConsole, valigie, valigiaGiocatore, selected, messaggio);
		}
	}

	return 0;
}


float SchermataDottore(HANDLE hConsole)
{
	FrameData dottore = GetAnimatedFramesFromFiles(DOTTORE_FILE_ROOT, 2);
	int clock = 0;

	DisegnaDottore(hConsole, clock, dottore, "aaaa", 0);
	while (1)
	{
		if (AggiornaClock(OttieniDelta(), 2, clock))
		{
			DisegnaDottore(hConsole, clock, dottore, "aaaa", 0);
		}
	}

	delete[] dottore;
	return 0;
}


void DisegnaDottore(HANDLE hConsole, int clock, FrameData dottore, string domanda, int selezione)
{
	_CLS;
	DrawBorders(hConsole, SCREEN_SIZE);
	DrawFrame(hConsole, dottore, clock, { SCREEN_SIZE.X - 8, SCREEN_SIZE.Y - 8 });
	DrawFrame(hConsole, dottore, !clock, { 2, SCREEN_SIZE.Y - 8 });

	if (clock)
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "DOTTORE", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	else
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "DOTTORE", FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	DrawStringCentered(hConsole, domanda, { SCREEN_SIZE.X / 2, 5 });

	if (selezione)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "ACCETTA LA PROPOSTA", { SCREEN_SIZE.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "IGNORA LA PROPOSTA", { SCREEN_SIZE.X / 2, 8 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "ACCETTA LA PROPOSTA", { SCREEN_SIZE.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "IGNORA LA PROPOSTA", { SCREEN_SIZE.X / 2, 8 });
	}

}


/*	Funzione DisegnaPartita
*	
*	Disegna la partita.
*	DESCRIZONE DA AMPLIARE
*/
void DisegnaPartita(HANDLE hConsole, bool valigie[], int valigaGiocatore, int selected, string messaggio)
{
	_CLS;

	DrawBorders(hConsole, SCREEN_SIZE);

	COORD coord = { 2, 1 };

	for (int i = 0; i < NUMERO_VALIGIE; i++)
	{
		if (valigie[i])
		{
			if (selected == i)
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				DisegnaValigia(hConsole, coord, i + 1);
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			}
			else
				DisegnaValigia(hConsole, coord, i + 1);
		}

		coord.X += GRANDEZZA_VALIGIA.X + 1;

		if (i == 9)
		{
			coord.Y += GRANDEZZA_VALIGIA.Y + 1;
			coord.X = 2;
		}
	}

	DrawLine(hConsole, { 1, 8 }, 42, 0xCD, Orizzontale);
	
	if (valigaGiocatore != -1)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		DisegnaValigia(hConsole, { SCREEN_SIZE.X - 5, SCREEN_SIZE.Y - 3 }, valigaGiocatore + 1);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	DrawStringAtPos(hConsole, messaggio, { 2, SCREEN_SIZE.Y - 2 });
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

int ControllaInputGioco(int& selection, bool valigie[])
{
	if (GetAsyncKeyState(VK_UP) & KEY_JUST_PRESSED || (GetAsyncKeyState(VK_DOWN) & KEY_JUST_PRESSED))
	{
		int target = selection;
		target += 10;
		target %= 20;

		if (valigie[target])
			selection = target;

		return 1;
	}
	if (GetAsyncKeyState(VK_LEFT) & KEY_JUST_PRESSED)
	{
		do
		{
			selection--;
			if (selection < 0)
				selection = 19;
		} while (!valigie[selection]);
		return 1;
	}
	if (GetAsyncKeyState(VK_RIGHT) & KEY_JUST_PRESSED)
	{
		do
		{
			selection++;
			if (selection > 19)
				selection = 0;
		} while (!valigie[selection]);
		return 1;
	}
	if ((GetAsyncKeyState(VK_SPACE) & KEY_JUST_PRESSED) || (GetAsyncKeyState(VK_RETURN) & KEY_JUST_PRESSED))
		return 2;
	if (GetAsyncKeyState(VK_ESCAPE) & KEY_JUST_PRESSED)
		return 3;

	return 0;
}


/*	Funzione ControllaSelectionMenuKeys
* 
*	Serve a controllare gli input nel secondo menu della funzione menu.
*	Cambia il valore di selection. Se selection cambia ritorniamo true.
*/
bool ControllaSelectionMenuKeys(int& selection)
{
	if ((GetAsyncKeyState(VK_UP) & KEY_JUST_PRESSED) || (GetAsyncKeyState(VK_DOWN) & KEY_JUST_PRESSED))
	{
		selection = !selection;
		return true;
	}
	if (GetAsyncKeyState(0x31)) //	1
	{
		selection = 0;
		return true;
	}
	if (GetAsyncKeyState(0x32))	//	2
	{
		selection = 1;
		return true;
	}

	return false;
}