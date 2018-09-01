#pragma once

struct __declspec(dllexport) Xaos_ModuleInfo {
	const char* LoadMessage;
	const char* ModuleInfo;
	const char** ModuleDependencies; //ends in 0						///////////////NOT YET USED, MAYBE WHEN MANUAL MAPPING IS IMPLEMENTED (in case user's looking to release a module used by others)
	bool (*Init)(void* me);
	void(*Term)();
};

__declspec(dllexport) Xaos_ModuleInfo* Xaos_LoadModule(const char* mod, void** ptr = 0);
__declspec(dllexport) bool Xaos_UnloadModule(const char* mod);