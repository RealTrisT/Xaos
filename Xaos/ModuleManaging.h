#pragma once
#include "Xaos.h"
#include <Windows.h>

struct ModEntry {
	ModEntry* next;
	ModEntry* prev;
	Xaos_ModuleInfo* modInfo;
	HMODULE modBase;
	char* modName;
	void Free();
	static ModEntry* AllocNew(ModEntry* Next, ModEntry* Prev, Xaos_ModuleInfo* ModInfo, HMODULE ModBase, char* ModName);
};

extern ModEntry*const ModLinkedList;

Xaos_ModuleInfo* LoadModule(const char * modName, void** modPtr);
bool UnloadModule(ModEntry * mod);
bool UnloadModule(const char * modName);
void ShutdownAllModules();