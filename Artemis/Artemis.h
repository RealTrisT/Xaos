///Artemis (roman Diana) - Godess of the hunt
#pragma once
#include "../Xaos/Xaos.h"
#include "../Coeus/Coeus.h"

__declspec(dllexport) JsValueRef InitializeJs();


bool InitializeArtemis(void* thismod);
void TerminateArtemis();

Xaos_ModuleInfo ArtemisModInfo = {
	"------------------------------ ARTEMIS LOADED -----------------------------",
	"Artemis: Goddess of the hunt, also the moon.\nIdk what the purpose of this module as a whole is yet.\n",
	0,
	InitializeArtemis,
	TerminateArtemis
};

__declspec(dllexport) extern "C" const void* ModuleInfo = (void*)&ArtemisModInfo;
