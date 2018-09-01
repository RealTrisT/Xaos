#pragma once


struct __declspec(dllexport) Xaos_ModuleInfo {
	const char* LoadMessage;
	const char* ModuleInfo;
	const char** ModuleDependencies; //ends in 0						///////////////NOT YET USED, MAYBE WHEN MANUAL MAPPING IS IMPLEMENTED
	bool (*Init)(void* me);
	void(*Term)();
};

Xaos_ModuleInfo* __declspec(dllexport) Xaos_LoadModule(const char* mod);
bool __declspec(dllexport) Xaos_UnloadModule(const char* mod);