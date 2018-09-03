#pragma once
#include "Xaos.h"
#include <Windows.h>

struct Xaos_ModEntry {
	Xaos_ModEntry* next;
	Xaos_ModEntry* prev;
	Xaos_ModuleInfo* modInfo;
	HMODULE modBase;
	char* modName;
	void Free();
	static Xaos_ModEntry* AllocNew(Xaos_ModEntry* Next, Xaos_ModEntry* Prev, Xaos_ModuleInfo* ModInfo, HMODULE ModBase, char* ModName);
};

extern Xaos_ModEntry*const Xaos_ModLinkedList;