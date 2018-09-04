#include <string>

#include "Xaos.h"
#include "ModuleManaging.h"

Xaos_ModuleInfo * Xaos_LoadModule(const char * mod, void ** ptr) {
	return LoadModule(mod, ptr);
}

bool Xaos_UnloadModule(const char * mod) {
	return UnloadModule(mod);
}

void Xaos_Shutdown() {
	ShutdownAllModules();
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------- TEST REALM ---------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include "../Coeus/Coeus.h"
#include "../Hyperion/Hyperion.h"
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

	auto Hyp_GetScreenDimensions = (decltype(Hyperion_GetScreenDimensions)*)GetProcAddress(Hyperion_Dll, "Hyperion_GetScreenDimensions");
	auto Hyp_UpdateScreen = (decltype(Hyperion_UpdateScreen)*)GetProcAddress(Hyperion_Dll, "Hyperion_UpdateScreen");
	auto HypScreen = *(decltype(HyperionScreen)*)GetProcAddress(Hyperion_Dll, "HyperionScreen");

	unsigned scrDim[2] = { 0 };
	Hyp_GetScreenDimensions(scrDim);

	for (size_t x = 0; x < scrDim[0]; x++) {
		for (size_t y = 0; y < scrDim[1]; y++) {
			HypScreen[y*scrDim[0] + x].r = (unsigned char)(x / float(scrDim[0]) * 255);
			HypScreen[y*scrDim[0] + x].g = (unsigned char)(y / float(scrDim[1]) * 255);
			HypScreen[y*scrDim[0] + x].b = 0;
			HypScreen[y*scrDim[0] + x].a = 128;
		}
	}
	Hyp_UpdateScreen();



	//system("pause");
	getchar();
	Xaos_Shutdown();
	getchar();
	return 0;
}

