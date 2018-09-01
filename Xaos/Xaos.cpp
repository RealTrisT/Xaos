#include <string>

#include "Xaos.h"
#include "ModuleManaging.h"


Xaos_ModuleInfo* Xaos_LoadModule(const char * modName, void** modPtr){
	unsigned modName_size = strlen(modName);										//make
	char* modPath = (char*)malloc(8 + modName_size + 1);							//relative
	memcpy(modPath, "Modules/", 8); memcpy(modPath + 8, modName, modName_size+1);	//path

	HMODULE loadedMod = 0;															//try and
	if (!(loadedMod = LoadLibraryA(modPath))) {free(modPath);return false;}			//load it

	free(modPath);																	//no longer need relative path

	Xaos_ModuleInfo* modInfo = 0;																							//try and
	if (!(modInfo = (Xaos_ModuleInfo*)GetProcAddress(loadedMod, "ModuleInfo"))) { FreeLibrary(loadedMod); return false; }	//get modInfo struct

	Xaos_ModEntry* llc = const_cast<Xaos_ModEntry*>(Xaos_ModLinkedList);										//linked list cursor
	for (; llc->next; llc = llc->next);												//get last element

	//doubly linked list where the first element points to the last, but the last points to zero
	Xaos_ModEntry* loadedModXaosEntry = Xaos_ModEntry::AllocNew(0, llc, modInfo, loadedMod, _strdup(modName));

	llc->next = loadedModXaosEntry;													//adjust previos entry's next
	Xaos_ModLinkedList->prev = loadedModXaosEntry;									//adjust first entry's prev

	if(modInfo->Init)modInfo->Init(loadedMod);										//run initialization proc

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
//----------------------------------------------------------------------NOT TEST REALM----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../Coeus/Coeus.h"
#pragma comment(lib, "../libs/ChakraCore64.lib")

int main() {
	SetConsoleTitle("Xaos");

	HMODULE Coeus_Dll = LoadLibrary("Modules/Coeus.dll");
	if (Coeus_Dll) {
		JsValueRef Coeus_Return = 0;

		std::wstring(*Coeus_whateverToStringJs)(JsValueRef) = (std::wstring(*)(JsValueRef))GetProcAddress(Coeus_Dll, "Coeus_whateverToStringJs");
		JsErrorCode(*Coeus_RunJs)(wchar_t*, JsValueRef*) = (JsErrorCode(*)(wchar_t*, JsValueRef*))GetProcAddress(Coeus_Dll, "Coeus_RunJs");
		void(*Coeus_CollectGarbageJs)() = (void(*)())GetProcAddress(Coeus_Dll, "Coeus_CollectGarbageJs");

		((void(*)(HMODULE))GetProcAddress(Coeus_Dll, "Initialize"))(Coeus_Dll);
		Coeus_Return = ((JsValueRef(*)())GetProcAddress(Coeus_Dll, "InitializeJs"))();

		printf(": %S\n", Coeus_whateverToStringJs(Coeus_Return).c_str());

		/*---------------------test-------------------------*/
		//JsValueRef globle; JsGetGlobalObject(&globle);
		//PEH.Build(globle);
		/*---------------------test-------------------------*/

		std::wstring resultW; std::wstring script; JsValueRef returnValue;
		do {
			Coeus_CollectGarbageJs();
			resultW = L"";
			printf(">>"); std::getline(std::wcin, script);
			Coeus_RunJs((wchar_t*)script.c_str(), &returnValue);
			printf(": %S\n", (resultW = Coeus_whateverToStringJs(returnValue)).c_str());
		} while (wcscmp(resultW.c_str(), L"exit"));

		((void(*)())GetProcAddress(Coeus_Dll, "Terminate"))();
	}
	else { puts("coeus not loaded"); }

	system("pause");
	return 0;
}