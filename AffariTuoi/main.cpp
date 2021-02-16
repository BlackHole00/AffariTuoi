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

int Menu(HANDLE);
void DrawMainMenu(HANDLE, int, FrameData);
void DrawSelectionMenu(HANDLE, int, int);

bool ControllaSelectionMenuKeys(HANDLE, int&);

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

	LeggiPremiNulliDaFile("nulli.txt", 3, temp, lung);

	int res = Menu(hConsole);
	_CLS;
	if (res)	//	res == 1
	{
		SetConsoleCursorPosition(hConsole, { 0, 0 });
		DrawStringAtPos(hConsole, "DA IMPLEMENTARE!!!\n", {0, 0});
	}
	else
	{
		DisegnaPartita(hConsole, valigie, 3);
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
	SetConsoleCursorPosition(hConsole, { coord.X + 1, coord.Y + GRANDEZZA_VALIGIA.Y - 1});
	WriteConsoleA(hConsole, temp.c_str(), 2, NULL, NULL);
}


/*	Funzione DisegnaPartita
*	
*	Disegna la partita.
*	DESCRIZONE DA AMPLIARE
*/
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
}


/*	Funzione Menu	
*
*	Disegna il menu principale (che è più estetico), poi chiede all'utente se
*		vuole avviare un nuovo gioco o caricare una salvataggio precedente.
*	Ritorna la scelta dell'utente (0 = nuovo gioco; 1 = Carica salvataggio).
* 
*	NOTA:
*		-Aggiorniamo lo schermo solo quando otteniamo un input dall'utente o
*			quando sappiamo che dobbiamo cambiare il colore di qualche scritta
*/
int Menu(HANDLE hConsole)
{
	int clock = 0;
	int selected = 0;

	FrameData valigiaAnimata = GetAnimatedFramesFromFiles(VALIGIA_FILE_ROOT, 4);

	HideCursor(hConsole);
	//	Disegnamo il menu per la prima volta.
	DrawMainMenu(hConsole, clock, valigiaAnimata);
	while (!(GetAsyncKeyState(VK_RETURN) || (GetAsyncKeyState(VK_SPACE))))
	{
		//	Controlliamo se dobbiamo ridisegnare lo schermo
		if (AggiornaClock(OttieniDelta(), 4, clock))
		{
			_CLS;
			DrawMainMenu(hConsole, clock, valigiaAnimata);
		}
	}

	_CLS;
	delete[] valigiaAnimata;

	clock = 0;
	DrawSelectionMenu(hConsole, clock, selected);
	while (!((GetAsyncKeyState(VK_RETURN) & KEY_JUST_PRESSED) || ((GetAsyncKeyState(VK_SPACE)) & KEY_JUST_PRESSED)))
	{
		//	Se dobbiamo cambiare colore di qualcosa è stato ricevuto un input importante ridisegnamo lo schermo
		if (AggiornaClock(OttieniDelta(), 4, clock) || ControllaSelectionMenuKeys(hConsole, selected))
		{
			_CLS;
			DrawSelectionMenu(hConsole, clock, selected);
		}
	}

	return selected;
}

/*	Funzione DrawMainMenu	
*
*	Disegna il Menu Principale.Utilizza il parametro clock per sapere di che colore disegnare le cose.
*/
void DrawMainMenu(HANDLE hConsole, int clock, FrameData datiValigia)
{
	//	Disegna i bordi
	DrawBorders(hConsole, SCREEN_SIZE);

	//	Disegna valigia animata
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	DrawFrame(hConsole, datiValigia, clock, { 3, 1 });
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	//	Disegna >>>PREMI SPAZIO PER COMINCIARE<<< 
	if (clock % 2)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	DrawStringCentered(hConsole, ">>>PREMI SPAZIO PER COMINCIARE<<<", { SCREEN_SIZE.X / 2, 10 });
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	//Disegna affari tuoi dentro una scatola
	if (clock % 2)
		DrawStringInBox(hConsole, { 23, 2 }, "AFFARI TUOI", FOREGROUND_RED, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else
		DrawStringInBox(hConsole, { 23, 2 }, "AFFARI TUOI", FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

/*	Funzione DrawMainMenu
*
*	Disegna il Menu Principale.Utilizza il parametro clock per sapere di che colore disegnare le cose.
*/
void DrawSelectionMenu(HANDLE hConsole, int clock, int selection)
{
	//	Disegna i bordi
	DrawBorders(hConsole, SCREEN_SIZE);
	
	//	Disegna la scritta AFFARI TUOI
	if (clock % 2)
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	//	Disegna la scrtta MENU PRINCIPALE
	if (clock % 2)
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
	DrawStringCentered(hConsole, "MENU PRINCIPALE", { SCREEN_SIZE.X / 2, 4 });

	//	Disegna gli item del menu a seconda di cosa è selezionato
	//	TODO trova un metodo migliore 
	if (selection)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "NUOVO GIOCO", { SCREEN_SIZE.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "CARICA  GIOCO", { SCREEN_SIZE.X / 2, 8 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "NUOVO GIOCO", { SCREEN_SIZE.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "CARICA  GIOCO", { SCREEN_SIZE.X / 2, 8 });
	}

	//	Disegna una scatola attorno agli item del menu
	DrawBox(hConsole, { SCREEN_SIZE.X / 2 - 7, 6 }, { SCREEN_SIZE.X / 2 + 7, 9 });

	DrawStringCentered(hConsole, "FRECCE: Seleziona INVIO: Accetta", { SCREEN_SIZE.X / 2 + 1, 11 });
}


/*	Funzione ControllaSelectionMenuKeys
* 
*	Serve a controllare gli input nel secondo menu della funzione menu.
*	Cambia il valore di selection. Se selection cambia ritorniamo true.
*/
bool ControllaSelectionMenuKeys(HANDLE hConsole, int& selection)
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


/*	Funzione OttieniDelta
*	
*	Serve per trovare la differenza di tempo in millisecondi dall'ultima volta che questa funzione è stata chiamata.
*	La prima volta che viene chiamata ritorna 0 (o quasi).
*/
float OttieniDelta()
{
	static time_t old;
	static time_t current = clock();

	old = current;
	current = clock();
	return current - old;
}


/*	Funzione AggiornaClock
* 
*	Funzione che aggiorna il valore del clock.
*	Utilizzando un contatore interno e la differenza in millisecondi aggiorna clock circa ogni secondo.
*	Ritorna true se clock è stato modificato.
*	Questa funzione è stata pensata per l'utilizzo in combo con le funzioni DrawMainMenu e simili, che
*		hanno bisogno di un valore per sapere di che colore disegnare le cose.
*/
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