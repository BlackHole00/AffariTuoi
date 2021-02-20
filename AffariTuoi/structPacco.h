#pragma once
#include <iostream>

enum ContenutoPacco
{
	denaro,
	oggetto
};

struct StructPacco
{
	bool chiuso;
	ContenutoPacco contenuto;
	float montePremi;
	string premioScarto;
};