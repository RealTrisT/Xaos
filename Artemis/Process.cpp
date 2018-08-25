#include "Process.h"
#include <tlhelp32.h>
#include <string>
#include <Psapi.h>
#include <sstream>

#include "JsHelper.h"

char* FilenameFromPath(char* Path) {
	for (unsigned i = 0; Path[i]; i++) if (Path[i] == '\\') { Path += i + 1; i = -1; }
	return Path;
}

char* ToLowerCase(char* str) {
	for (unsigned i = 0; str[i]; i++) if ((unsigned char)((unsigned char)str[i] - 65) < 26) str[i] += 32;
	return str;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------PROCESS FUNCTIONS--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------------------OPEN OVERLOAD 1
bool Process::Open(DWORD ProcID) {
	procID = ProcID;
	procH = OpenProcess(PROCESS_ALL_ACCESS, false, ProcID);
	if (procH)GetModuleFileNameExA(this->procH, nullptr, this->procP, MAX_PATH);
	return !!procH;
}

//------------------------------------------------------------------------------------------------------------------------OPEN OVERLOAD 2
bool Process::Open(char * ProcName) {
	DWORD ProcID = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if ((hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)return false;	//create

	if (!Process32First(hProcessSnap, &pe32)) { CloseHandle(hProcessSnap); return false; }						//get first process

	do if (!strcmp(ProcName, FilenameFromPath(pe32.szExeFile))) { ProcID = pe32.th32ProcessID; break; }			//if filename = same as wanted process -> set ProcID to *the one* and gtfo
	while (Process32Next(hProcessSnap, &pe32));																	//go through the list

	CloseHandle(hProcessSnap);
	return (ProcID) ? Open(ProcID) : false;
}

//------------------------------------------------------------------------------------------------------------------------OPEN_WINDOWTITLE
bool Process::Open_WindowTitle(char * ProcWindowTitle) {
	DWORD ProcID;
	HWND r = FindWindowA(NULL, ProcWindowTitle);
	if (r) GetWindowThreadProcessId(r, &ProcID);
	else return false;
	return Open(ProcID);
}

//------------------------------------------------------------------------------------------------------------------------CLOSE
void Process::Close() {
	if (this->procH) {
		CloseHandle(this->procH);
		this->procH = 0; //zero the handle
		*this->procP = 0;//zero the first byte of path
		this->procID = 0;
	}
}

//------------------------------------------------------------------------------------------------------------------------READMEM OVERLOAD 1
bool Process::ReadMem(BYTE * ForeignAddress, BYTE * Buffer, DWORD Length) {
	SIZE_T read = 0;
	BOOL result = ReadProcessMemory(procH, ForeignAddress, Buffer, Length, &read);
	return result && (read == Length);
}

//------------------------------------------------------------------------------------------------------------------------READMEM OVERLOAD 2
template<typename T>
bool Process::ReadMem(BYTE * ForeignAddress, T & Buffer) {
	return ReadMem(ForeignAddress, (BYTE*)&Buffer, sizeof(Buffer));
}

//------------------------------------------------------------------------------------------------------------------------WRITEMEM OVERLOAD 1
bool Process::WriteMem(BYTE * ForeignAddress, BYTE* Buffer, DWORD Length) {
	SIZE_T written = 0;
	BOOL resulting = WriteProcessMemory(procH, ForeignAddress, Buffer, Length, &written);
	return resulting && (written == Length);
}

//------------------------------------------------------------------------------------------------------------------------WRITEMEM OVERLOAD 2
template<typename T>
inline bool Process::WriteMem(BYTE * ForeignAddress, T Buffer) {
	return WriteMem(ForeignAddress, &Buffer, sizeof(Buffer));
}

//------------------------------------------------------------------------------------------------------------------------GETMODULE
Module Process::GetModule(char * modName) {
	//printf("Getting Module %s\n", modName);
	Module returnboi = {};
	HANDLE ModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);

	ModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->procID);
	if (ModuleSnap == INVALID_HANDLE_VALUE)return returnboi;

	if (!Module32First(ModuleSnap, &me32)) { CloseHandle(ModuleSnap); return returnboi; }

	do {
		if (!strcmp(ToLowerCase(me32.szModule), modName)) {
			returnboi.daddy = this;
			returnboi.base = me32.modBaseAddr;
			returnboi.size = me32.modBaseSize;
			returnboi.name = std::string(me32.szModule);
			break;
		}
	} while (Module32Next(ModuleSnap, &me32));

	return returnboi;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------JAVASCRIPT PORTED FUNCTIONS------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#define getme() Process_OBJREADY* me = (Process_OBJREADY*)Js_GetExternPointer(arguments[0])

void __stdcall KillProcessObject(Process* me) {
	me->Close();
	delete me;
}


JsValueRef Process_OBJREADY::jsClose(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	me->Close();
	return me->jsMe;
}

JsValueRef Process_OBJREADY::jsGetModule(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	if (Js_GetType(arguments[1]) != JsString) { puts("> Provide a string with the name of the module."); return Js_GetNull(); }
	Module_OBJREADY* elmod = new Module_OBJREADY(me->GetModule((char*)Js_GetString(arguments[1]).c_str()));
	if (!elmod->daddy) { delete elmod; puts("> Module not found.");  return Js_GetNull(); }
	return elmod->GenerateObject();
}

JsValueRef Process_OBJREADY::jsToString(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	std::stringstream yeah;
	yeah << "{ path : \"" << me->procP << "\", handle : [0x" << std::uppercase << std::hex << me->procH << "], id : [0x" << me->procID << "|" << std::dec << me->procID << "] }";
	return Js_MakeString(yeah.str().c_str(), yeah.str().length());
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////--------------MODULE---------------////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------MODULE FUNCTIONS------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void Module::UpdatePEH(){
	unsigned char* PEH = this->base;
	unsigned PEH_Offset = 0;
	this->daddy->ReadMem(PEH + 0x3C, PEH_Offset);
	PEH += PEH_Offset;
	this->daddy->ReadMem(PEH, (unsigned char*)&this->PEH, sizeof(this->PEH));
}







//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------JAVASCRIPT PORTED FUNCTIONS------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void KillModuleObject(Module* me) {
	delete me;
}


#define getme() Module_OBJREADY* me = (Module_OBJREADY*)Js_GetExternPointer(arguments[0])
#define getmeget() Module_OBJREADY* me = (Module_OBJREADY*)Js_GetExternPointer(callbackState)

void * Module_OBJREADY::PEH_getter(Module_OBJREADY * daddyData, IMAGE_NT_HEADERS64 * medata) {
	if (!medata->Signature) daddyData->UpdatePEH();
	return &daddyData->PEH;
}

JsValueRef Module_OBJREADY::Module_UpdatePEH(JsValueRef c, bool cc, JsValueRef * a, unsigned short ac, void * cs) { 
	((Module_OBJREADY*)cs)->UpdatePEH();
	return ((Module_OBJREADY*)cs)->Module_objProps[(char)PropArrayIndexes::PEH_i].me;
}

JsValueRef Module_OBJREADY::Module_toString(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	std::stringstream yeah;
	yeah << "{ name : \"" << me->name.c_str() << "\", base : [0x" << std::uppercase << std::hex << (void*)me->base << "], size : [0x" << me->size << "|" << std::dec << me->size << "] }";
	return Js_MakeString(yeah.str().c_str(), yeah.str().length());
}
