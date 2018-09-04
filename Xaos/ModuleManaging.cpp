#include "ModuleManaging.h"
#include <stdio.h>

ModEntry ModLinkedListOwn = { 0, &ModLinkedListOwn, 0, GetModuleHandleA(0), (char*)"Xaos.dll" }; 
ModEntry*const ModLinkedList = &ModLinkedListOwn;

void ModEntry::Free() {
	if (modInfo && modInfo->Term)modInfo->Term();
	modInfo = 0;	//placebo

	if (modName && (this != &ModLinkedListOwn))free(modName);	//xaos's own ModName is data, not allocated
	modName = 0;	//placebo

	if (this != &ModLinkedListOwn)free(this);					//xaos's own ModEntry is zero
	return;
}

ModEntry * ModEntry::AllocNew(ModEntry * Next, ModEntry * Prev, Xaos_ModuleInfo * ModInfo, HMODULE ModBase, char * ModName) {
	ModEntry* newone = (ModEntry*)malloc(sizeof(ModEntry));
	*newone = { Next, Prev, ModInfo, ModBase, ModName };
	return newone;
}


Xaos_ModuleInfo* LoadModule(const char * modName, void** modPtr) {

	unsigned modNameLen = strlen(modName);
	char* modPath = (char*)malloc(8 + modNameLen + 1);
	memcpy(modPath, (char*)"Modules\\", 8); memcpy(modPath + 8, modName, modNameLen + 1);

	HMODULE loadedMod = 0;											//try and
	if (!(loadedMod = LoadLibraryA(modPath))) { printf("Failed to load %s:%p:0x%X\n", modName, loadedMod, GetLastError()); free(modPath); return 0; }				//load it

	free(modPath);

	Xaos_ModuleInfo* modInfo = 0;																						//try and
	if (!(modInfo = (Xaos_ModuleInfo*)GetProcAddress(loadedMod, "ModuleInfo"))) { FreeLibrary(loadedMod); return 0; }	//get modInfo struct
	puts("found module info");

	ModEntry* llc = ModLinkedList->prev;											//linked list cursor - get last element
																					//doubly linked list where the first element points to the last, but the last points to zero
	ModEntry* loadedModXaosEntry = ModEntry::AllocNew(0, llc, modInfo, loadedMod, _strdup(modName));

	llc->next = loadedModXaosEntry;													//adjust previos entry's next
	ModLinkedList->prev = loadedModXaosEntry;										//adjust first entry's prev

	if (modInfo->Init)modInfo->Init((unsigned char*)loadedMod);						//run initialization proc			TODO: check return type and act accordingly

	if (modPtr)*modPtr = loadedMod;
	return modInfo;																	//return the infoz
}

bool UnloadModule(ModEntry* modE) {
	HMODULE lib = modE->modBase;													//save lib base cuz we plan on releasing the object
	modE->prev->next = modE->next;													//adjust linked list previos element's next
	if (modE->next) modE->next->prev = modE->prev;									//adjust linked list next element's prev
	else ModLinkedList->prev = modE->prev;											//if we're the last, update the first to point to the before us
	modE->Free();																	//release linked list object (this pointer will no longer point to anything)
	FreeLibrary(lib);																//free da boi
	return true;
}
bool UnloadModule(const char * modName) {
	ModEntry* llc = ModLinkedList;													//linked list cursor
	bool found = false;
	for (; llc->next && !(found = !strcmp(modName, llc->modName)); llc = llc->next);//find correct one
	if (!found)return false;
	if (!strcmp(ModLinkedList->modName, llc->modName))return false;
	return UnloadModule(llc);
}

void ShutdownAllModules() {
	ModEntry* llc = ModLinkedList->prev;											//get last element
	for (; llc->prev != llc; ) {													//if we're not the only element left
		llc = llc->prev;															//we are now the previous element (cuz if we release first we can't get it)
		UnloadModule(llc->next);													//release the next one (the one we just were)
	}
}