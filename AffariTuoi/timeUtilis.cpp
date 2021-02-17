#include "timeUtilis.h"
#include <iostream>

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