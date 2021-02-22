#pragma once
#include <Windows.h>

//	Da utilizzare con GetAsyncKeyState()
#define KEY_JUST_PRESSED 0x01

const COORD GRANDEZZA_VALIGIA = { 3, 2 };
const COORD SCREEN_SIZE = { 43, 12 };

//	*NON CAMBIARE PER NESSUN ASSOLUTISSIMO MOTIVO!!!*
const int NUMERO_VALIGIE = 20;

const string DOTTORE_FILE_ROOT = "res/dottore_frame_";
const string VALIGIA_FILE_ROOT = "res/valigia_frame_";

const string FILE_SALVATAGGIO = "res/salvataggio.txt";
const string FILE_PREMI_NULLI = "res/nulli.txt";

const int NUM_PREMI_NULLI = 3;
const int NUM_PREMI_DENARO = NUMERO_VALIGIE - NUM_PREMI_NULLI;

const int MIN_INTERVALLO_DOTTORE = 3;
const int MAX_INTERVALLO_DOTTORE = 5;

const int MENU_PAUSA_NUMERO_VOCI = 4;