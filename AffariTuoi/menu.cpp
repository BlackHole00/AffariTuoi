#include "menu.h"
#include "timeUtilis.h"

const string VALIGIA_FILE_ROOT = "valigia_frame_";


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
int Menu(HANDLE hConsole, COORD screenSize)
{
	int clock = 0;
	int selected = 0;

	FrameData valigiaAnimata = GetAnimatedFramesFromFiles(VALIGIA_FILE_ROOT, 4);

	HideCursor(hConsole);
	//	Disegnamo il menu per la prima volta.
	DrawMainMenu(hConsole, clock, valigiaAnimata, screenSize);
	while (!(GetAsyncKeyState(VK_RETURN) || (GetAsyncKeyState(VK_SPACE))))
	{
		//	Controlliamo se dobbiamo ridisegnare lo schermo
		if (AggiornaClock(OttieniDelta(), 4, clock))
		{
			_CLS;
			DrawMainMenu(hConsole, clock, valigiaAnimata, screenSize);
		}
	}

	_CLS;
	delete[] valigiaAnimata;

	clock = 0;
	DrawSelectionMenu(hConsole, clock, selected, screenSize);
	while (!((GetAsyncKeyState(VK_RETURN) & KEY_JUST_PRESSED) || ((GetAsyncKeyState(VK_SPACE)) & KEY_JUST_PRESSED)))
	{
		//	Se dobbiamo cambiare colore di qualcosa o è stato ricevuto un input importante ridisegnamo lo schermo
		if (AggiornaClock(OttieniDelta(), 4, clock) || ControllaSelectionMenuKeys(selected))
		{
			_CLS;
			DrawSelectionMenu(hConsole, clock, selected, screenSize);
		}
	}

	return selected;
}

/*	Funzione DrawMainMenu
*
*	Disegna il Menu Principale.Utilizza il parametro clock per sapere di che colore disegnare le cose.
*/
void DrawMainMenu(HANDLE hConsole, int clock, FrameData datiValigia, COORD screenSize)
{
	//	Disegna i bordi
	DrawBorders(hConsole, screenSize);

	//	Disegna valigia animata
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	DrawFrame(hConsole, datiValigia, clock, { 3, 1 });
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	//	Disegna >>>PREMI SPAZIO PER COMINCIARE<<< 
	if (clock % 2)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	DrawStringCentered(hConsole, ">>>PREMI SPAZIO PER COMINCIARE<<<", { screenSize.X / 2, 10 });
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
void DrawSelectionMenu(HANDLE hConsole, int clock, int selection, COORD screenSize)
{
	//	Disegna i bordi
	DrawBorders(hConsole, screenSize);

	//	Disegna la scritta AFFARI TUOI
	if (clock % 2)
		DrawStringInBoxCentered(hConsole, { screenSize.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else
		DrawStringInBoxCentered(hConsole, { screenSize.X / 2, 2 }, "AFFARI TUOI", FOREGROUND_RED | FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	//	Disegna la scrtta MENU PRINCIPALE
	if (clock % 2)
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
	DrawStringCentered(hConsole, "MENU PRINCIPALE", { screenSize.X / 2, 4 });

	//	Disegna gli item del menu a seconda di cosa è selezionato
	//	TODO trova un metodo migliore 
	if (selection)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "NUOVO GIOCO", { screenSize.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "CARICA  GIOCO", { screenSize.X / 2, 8 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
	else
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		DrawStringCentered(hConsole, "NUOVO GIOCO", { screenSize.X / 2, 7 });

		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		DrawStringCentered(hConsole, "CARICA  GIOCO", { screenSize.X / 2, 8 });
	}

	//	Disegna una scatola attorno agli item del menu
	DrawBox(hConsole, { screenSize.X / 2 - 7, 6 }, { screenSize.X / 2 + 7, 9 });

	DrawStringCentered(hConsole, "FRECCE: Seleziona INVIO: Accetta", { screenSize.X / 2 + 1, 11 });
}