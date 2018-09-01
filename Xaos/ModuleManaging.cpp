#include "ModuleManaging.h"

Xaos_ModEntry Xaos_ModLinkedListOwn = { 0, 0, 0, GetModuleHandleA(0), (char*)"Xaos.dll" }; 
extern const Xaos_ModEntry* Xaos_ModLinkedList = &Xaos_ModLinkedListOwn;

void Xaos_ModEntry::Free() {
	if (modInfo && modInfo->Term)modInfo->Term();
	modInfo = 0;	//placebo

	if (modName && (this != &Xaos_ModLinkedListOwn))free(modName);	//xaos's own ModName is data, not allocated
	modName = 0;	//placebo

	if (this != &Xaos_ModLinkedListOwn)free(this);					//xaos's own ModEntry is zero
	return;
}

Xaos_ModEntry * Xaos_ModEntry::AllocNew(Xaos_ModEntry * Next, Xaos_ModEntry * Prev, Xaos_ModuleInfo * ModInfo, HMODULE ModBase, char * ModName) {
	Xaos_ModEntry* newone = (Xaos_ModEntry*)malloc(sizeof(Xaos_ModEntry));
	*newone = { Next, Prev, ModInfo, ModBase, ModName };
	return newone;
}