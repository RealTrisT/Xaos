///Hyperion: God of Watchfulness, Wisdom and the Light
#pragma once

#include <Windows.h>

#include "../Xaos/Xaos.h"

bool InitializeHyperion(void* thismod);
void TerminateHyperion();

Xaos_ModuleInfo HyperionModInfo = {
	"----------------------------- HYPERION LOADED -----------------------------",
	"Hyperion: God of Watchfulness, Wisdom and the Light.\nFather of: Helios (the sun), Selene (the moon) and Eos (dawn)\nThis module manages UI.\n",
	0,
	InitializeHyperion,
	TerminateHyperion
};

__declspec(dllexport) extern "C" const void* ModuleInfo = (void*)&HyperionModInfo;
