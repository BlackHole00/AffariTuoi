#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;

#include "gfx.h"
#include "menu.h"
#include "timeUtilis.h"

enum TipoPacco
{
	denaro,
	oggetto
};

struct StructPacco
{
	bool chiuso = true;
	TipoPacco contenuto;
	float monte_premi;
	string premio_scarto;
};

const COORD GRANDEZZA_VALIGIA = { 3, 2 };
const COORD SCREEN_SIZE = { 43, 12 };
const int NUMERO_VALIGIE = 20;

const string DOTTORE_FILE_ROOT = "dottore_frame_";

void RiempiVett(StructPacco[], string[]);

int LeggiPremiNulliDaFile(string, int, string[], int&);
inline int NumeroRandomInRange(int, int);
BOOL WINAPI test(DWORD);

void DisegnaValigia(HANDLE, COORD, int);
void DisegnaPartita(HANDLE, StructPacco[], int, int, string, string);
void DisegnaDottore(HANDLE, int, FrameData, string, string, int);
void DisegnaSchermataFinale(HANDLE, int, string);

float Gioco(HANDLE, StructPacco[]);
float SchermataDottore(HANDLE, int&, StructPacco[]);

int ControllaInputGioco(int&, StructPacco[]);
bool ControllaInputDottore(int&);

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
		srand(time(NULL)); //generazione seme per il random

		string temp[25]; //vettore di string di 25 locazioni 
		int lung = 0;
		StructPacco vettPacchi[20];

		LeggiPremiNulliDaFile("nulli.txt", 3, temp, lung);
		RiempiVett(vettPacchi, temp);

		Gioco(hConsole, vettPacchi);
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


float Gioco(HANDLE hConsole, StructPacco vettPacchi[])
{
	int clock = 0;
	int selected = 0;
	int valigiaGiocatore = -1;
	string messaggio = "Seleziona un pacco!!!";
	string messaggio2 = "";
	int res = 0;

	StructPacco pacco;
	int numPacchi = 20, scelta, turniChiamata = 4, indPacco, eliminato, x;
	float soldiPalio;
	bool offertaDottore = false;

	bool dottore = false;

	DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selected, messaggio, messaggio2);

	do
	{
		res = (ControllaInputGioco(selected, vettPacchi));

		if (AggiornaClock(OttieniDelta(), 4, clock) || res == 1)
			DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selected, messaggio, messaggio2);

		if (res == 2)
		{
			vettPacchi[selected].chiuso = false;
			valigiaGiocatore = selected;
			numPacchi--;

			messaggio = "Hai selezionato il pacco " + to_string(selected + 1) + (string)"!";

			selected++;
			if (selected > 19)
				selected = 0;

			DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selected, messaggio, messaggio2);
		}
	} while (res != 2);

	turniChiamata = NumeroRandomInRange(2, 4);
	while (numPacchi > 0 && !offertaDottore)
	{
		int res = (ControllaInputGioco(selected, vettPacchi));

		if (AggiornaClock(OttieniDelta(), 4, clock) || res == 1)
			DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selected, messaggio, messaggio2);

		if (res != 0 && dottore)
		{
			dottore = false;
			turniChiamata = NumeroRandomInRange(2, 4);

			soldiPalio = SchermataDottore(hConsole, valigiaGiocatore, vettPacchi);
			if (soldiPalio != 0.0)
				offertaDottore = true;
		}
		else if (res == 2)
		{
			vettPacchi[selected].chiuso = false;
			numPacchi--;

			messaggio = "Hai aperto il pacco " + to_string(selected + 1) + (string)"!";
			messaggio2 = "Il pacco conteneva ";
			if (vettPacchi[selected].contenuto == denaro)
				messaggio2 += to_string(vettPacchi[selected].monte_premi);
			else
				messaggio2 += vettPacchi[selected].premio_scarto;
			messaggio2 += "!!!";

			if (numPacchi > 0)
			{
				do {
					selected++;
					if (selected > 19)
						selected = 0;
				} while (!vettPacchi[selected].chiuso);
			}

			DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selected, messaggio, messaggio2);

			turniChiamata--;
			if (turniChiamata == 0)
				dottore = true;
		}
	}

	string temp;
	if (!offertaDottore)
	{
		temp = "Hai vinto ";
		if (vettPacchi[valigiaGiocatore].contenuto == denaro)
			temp += to_string(vettPacchi[valigiaGiocatore].monte_premi);
		else
			temp += vettPacchi[valigiaGiocatore].premio_scarto;
		temp += "!!!";
	}
	else
		temp = "Hai vinto " + to_string(soldiPalio) + "!!!";

	DisegnaSchermataFinale(hConsole, clock, temp);
	while (!((GetAsyncKeyState(VK_SPACE) & KEY_JUST_PRESSED) || (GetAsyncKeyState(VK_RETURN) & KEY_JUST_PRESSED)))
	{
		if (AggiornaClock(OttieniDelta(), 2, clock))
			DisegnaSchermataFinale(hConsole, clock, temp);
	}
			

	return 0;
}


float SchermataDottore(HANDLE hConsole, int& valigiaGiocatore, StructPacco vettPacchi[])
{
	FrameData dottore = GetAnimatedFramesFromFiles(DOTTORE_FILE_ROOT, 2);
	int clock = 0;

	int proposta;
	proposta = NumeroRandomInRange(0, 10);
	int soldi = NumeroRandomInRange(5000, 10000);
	string messaggio = (proposta > 5) ? "Vuoi Scambiare" : "il dottore ti offre";
	string messaggio2 = (proposta > 5) ? "il tuo pacco?" : (to_string(soldi) + " euro");

	int selezione = 0;

	DisegnaDottore(hConsole, clock, dottore, messaggio, messaggio2, selezione);
	while (!((GetAsyncKeyState(VK_SPACE) & KEY_JUST_PRESSED) || (GetAsyncKeyState(VK_RETURN) & KEY_JUST_PRESSED)))
	{
		if (AggiornaClock(OttieniDelta(), 2, clock) || ControllaInputDottore(selezione))
		{
			DisegnaDottore(hConsole, clock, dottore, messaggio, messaggio2, selezione);
		}
	}

	if (!selezione) //	selezione == 0
	{
		if (proposta > 5)
		{
			int	selez = 0;
			while (!vettPacchi[selez].chiuso)
				selez++;

			int res;
			DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selez, "Seleziona un pacco", "");
			do
			{
				res = (ControllaInputGioco(selez, vettPacchi));

				if (AggiornaClock(OttieniDelta(), 4, clock) || res == 1)
					DisegnaPartita(hConsole, vettPacchi, valigiaGiocatore, selez, "Seleziona un pacco", "");

			} while (res != 2);

			vettPacchi[valigiaGiocatore].chiuso = true;
			valigiaGiocatore = selez;
			vettPacchi[valigiaGiocatore].chiuso = false;
			return 0.0;
		}
		else
			return soldi;
	}

	delete[] dottore;
	return 0;
}


void DisegnaDottore(HANDLE hConsole, int clock, FrameData dottore, string domanda, string domanda2, int selezione)
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
	DrawStringCentered(hConsole, domanda2, { SCREEN_SIZE.X / 2, 6 });

	if (selezione)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "ACCETTA LA PROPOSTA", { SCREEN_SIZE.X / 2, 8 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "IGNORA LA PROPOSTA", { SCREEN_SIZE.X / 2, 9 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "ACCETTA LA PROPOSTA", { SCREEN_SIZE.X / 2, 8 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "IGNORA LA PROPOSTA", { SCREEN_SIZE.X / 2, 9 });
	}

}


/*	Funzione DisegnaPartita
*	
*	Disegna la partita.
*	DESCRIZONE DA AMPLIARE
*/
void DisegnaPartita(HANDLE hConsole, StructPacco valigie[], int valigaGiocatore, int selected, string messaggio, string messaggio2)
{
	_CLS;

	DrawBorders(hConsole, SCREEN_SIZE);

	COORD coord = { 2, 1 };

	for (int i = 0; i < NUMERO_VALIGIE; i++)
	{
		if (valigie[i].chiuso)
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
	DrawStringAtPos(hConsole, messaggio2, { 2, SCREEN_SIZE.Y - 1 });
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


void DisegnaSchermataFinale(HANDLE hConsole, int clock, string messaggio)
{
	_CLS;

	DrawBorders(hConsole, SCREEN_SIZE);


	
	if (clock)
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);


	if (clock)
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, SCREEN_SIZE.Y / 2 }, messaggio, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else
		DrawStringInBoxCentered(hConsole, { SCREEN_SIZE.X / 2, SCREEN_SIZE.Y / 2 }, messaggio, FOREGROUND_RED | FOREGROUND_GREEN, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


int ControllaInputGioco(int& selection, StructPacco valigie[])
{
	if (GetAsyncKeyState(VK_UP) & KEY_JUST_PRESSED || (GetAsyncKeyState(VK_DOWN) & KEY_JUST_PRESSED))
	{
		int target = selection;
		target += 10;
		target %= 20;

		if (valigie[target].chiuso)
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
		} while (!valigie[selection].chiuso);
		return 1;
	}
	if (GetAsyncKeyState(VK_RIGHT) & KEY_JUST_PRESSED)
	{
		do
		{
			selection++;
			if (selection > 19)
				selection = 0;
		} while (!valigie[selection].chiuso);
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


bool ControllaInputDottore(int& selection)
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


void RiempiVett(StructPacco vettPacchi[], string temp[])
{
	for (int i = 0; i < 3; i++)
	{
		vettPacchi[i].contenuto = oggetto;
		vettPacchi[i].premio_scarto = temp[i];
	}
	for (int y = 3; y < 20; y++)
	{
		vettPacchi[y].contenuto = denaro;
		vettPacchi[y].monte_premi = NumeroRandomInRange(200, 50000);
	}

	for (int i = 0; i < NumeroRandomInRange(10, 100); i++)
	{
		int ind1 = NumeroRandomInRange(0, 19);
		int ind2 = NumeroRandomInRange(0, 19);

		StructPacco p = vettPacchi[ind1];
		vettPacchi[ind1] = vettPacchi[ind2];
		vettPacchi[ind2] = p;
	}
}