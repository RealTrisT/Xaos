#include <string>

#include "Xaos.h"
#include "ModuleManaging.h"


Xaos_ModuleInfo* Xaos_LoadModule(const char * modName, void** modPtr){

	unsigned modNameLen = strlen(modName);
	char* modPath = (char*)malloc(8 + modNameLen + 1);
	memcpy(modPath, (char*)"Modules\\", 8); memcpy(modPath + 8, modName, modNameLen + 1);

	HMODULE loadedMod = 0;											//try and
	if (!(loadedMod = LoadLibraryA(modPath))) { printf("Failed to load %s:%p:0x%X\n", modName, loadedMod, GetLastError()); free(modPath); return 0; }				//load it

	free(modPath);

	Xaos_ModuleInfo* modInfo = 0;																						//try and
	if (!(modInfo = (Xaos_ModuleInfo*)GetProcAddress(loadedMod, "ModuleInfo"))) { FreeLibrary(loadedMod); return 0; }	//get modInfo struct
	puts("found module info");

	Xaos_ModEntry* llc = const_cast<Xaos_ModEntry*>(Xaos_ModLinkedList);			//linked list cursor
	for (; llc->next; llc = llc->next);												//get last element

	//doubly linked list where the first element points to the last, but the last points to zero
	Xaos_ModEntry* loadedModXaosEntry = Xaos_ModEntry::AllocNew(0, llc, modInfo, loadedMod, _strdup(modName));

	llc->next = loadedModXaosEntry;													//adjust previos entry's next
	Xaos_ModLinkedList->prev = loadedModXaosEntry;									//adjust first entry's prev

	if(modInfo->Init)modInfo->Init((unsigned char*)loadedMod);						//run initialization proc			TODO: check return type and act accordingly

	if (modPtr)*modPtr = loadedMod;
	return modInfo;																	//return the infoz
}

bool Xaos_UnloadModule(const char * modName) {
	Xaos_ModEntry* llc = Xaos_ModLinkedList;											//linked list cursor
	bool found = false;
	for (; llc->next && !(found = !strcmp(modName, llc->modName)); llc = llc->next);	//find correct one
	if (!found)return false;
	if (!strcmp(Xaos_ModLinkedList->modName, llc->modName))return false;				//this asshole is trying to free xaos
	HMODULE lib = llc->modBase;															//save lib base cuz we plan on releasing the object
	llc->prev->next = llc->next;														//adjust linked list previos element's next
	if (llc->next) llc->next->prev = llc->prev;											//adjust linked list next element's prev
	llc->Free();																		//release linked list object (this pointer will no longer point to anything)
	FreeLibrary(lib);																	//free da boi
	return true;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------- TEST REALM ---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../Coeus/Coeus.h"
#include "../ChakraCore/ChakraCore.h"
#pragma comment(lib, "../libs/ChakraCore64.lib")

int main() {
	SetConsoleTitle("Xaos");
	/*//															COEUS_TEST
	HMODULE Coeus_Dll = 0;
	Xaos_ModuleInfo* Coeus_DllInfo = Xaos_LoadModule("Coeus.dll", (void**)&Coeus_Dll);
	if (Coeus_DllInfo) {

		bool(*Coeus_RunJs)(wchar_t*, std::wstring*) = (bool(*)(wchar_t*, std::wstring*))GetProcAddress(Coeus_Dll, "Coeus_RunJs");
		void(*Coeus_CollectGarbageJs)() = (void(*)())GetProcAddress(Coeus_Dll, "Coeus_CollectGarbageJs");
		
		std::wstring resultW; std::wstring script;
		do {
			Coeus_CollectGarbageJs();
			resultW = L"FAILED TO RUN";
			printf(">>"); std::getline(std::wcin, script);
			Coeus_RunJs((wchar_t*)script.c_str(), &resultW);
			printf(": %S\n", resultW.c_str());
		} while (wcscmp(resultW.c_str(), L"exit"));

		Xaos_UnloadModule("Coeus.dll");
		puts("Coeus Unloaded");
	} else { printf("coeus not loaded\n"); }*/

	//																HYPERION_TEST
	HMODULE Hyperion_Dll = 0;
	Xaos_ModuleInfo* Hyperion_DllInfo = Xaos_LoadModule("Hyperion.dll", (void**)&Hyperion_Dll);
	if (!Hyperion_DllInfo)printf("fack\n");


	//system("pause");
	getchar();
	return 0;
}