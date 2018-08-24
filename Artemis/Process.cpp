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
Process::Module Process::GetModule(char * modName) {
	//printf("Getting Module %s\n", modName);
	Module returnboi = { 0 };
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


#define getme() Process* me = (Process*)Js_GetExternPointer(arguments[0])

void __stdcall KillProcessObject(Process* me) {
	me->Close();
	delete me;
}

JsValueRef Process::BuildJsObject(){
	JsCreateExternalObject(this, (JsFinalizeCallback)KillProcessObject, &this->jsProcessObject);

	Js_AppendFunction(this->jsProcessObject, "toString",    Process::jsToString);
	Js_AppendFunction(this->jsProcessObject, "Close",       Process::jsClose);
	Js_AppendFunction(this->jsProcessObject, "GetModule",   Process::jsGetModule);
	//Js_AppendFunction(this->jsProcessObject, "ListModules", Process::jsListModules);
	return this->jsProcessObject;
}

JsValueRef Process::jsClose(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	me->Close();
	return me->jsProcessObject;
}

JsValueRef Process::jsGetModule(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	if (Js_GetType(arguments[1]) != JsString) { puts("> Provide a string with the name of the module."); return Js_GetNull(); }
	Module* elmod = new Module(me->GetModule((char*)Js_GetString(arguments[1]).c_str()));
	if (!elmod->daddy) { delete elmod; puts("> Module not found.");  return Js_GetNull(); }
	return elmod->BuildJsObject();
}

JsValueRef Process::jsToString(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
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



void Process::Module::UpdatePEH(){
	unsigned char* PEH = this->base;
	unsigned PEH_Offset = 0;
	this->daddy->ReadMem(PEH + 0x3C, PEH_Offset);
	PEH += PEH_Offset;
	this->daddy->ReadMem(PEH, (unsigned char*)&this->PEH, sizeof(this->PEH));
}







//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------JAVASCRIPT PORTED FUNCTIONS------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void KillModuleObject(Process::Module* me) {
	delete me;
}

JsValueRef Process::Module::BuildJsObject() {
	JsValueRef ElModule = 0;
	JsCreateExternalObject(this, (JsFinalizeCallback)KillModuleObject, &ElModule);

	Js_AppendFunction(ElModule, "UpdatePEH", Process::Module::jsUpdatePEH);
	Js_AppendFunction(ElModule, "toString", Process::Module::jsToString);
	
	jsPEH_R = Js_AppendObject(ElModule, "PEH", this, Process::Module::jsGetPEH);
		Js_AppendObject(jsPEH_R, "Signature", &PEH.Signature, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
		jsFileHeader_r = Js_AppendObject(jsPEH_R, "FileHeader", this, Process::Module::jsGetFileHeader);
			Js_AppendObject(jsFileHeader_r, "Machine", &PEH.FileHeader.Machine, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "NumberOfSections", &PEH.FileHeader.NumberOfSections, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "TimeDateStamp", &PEH.FileHeader.TimeDateStamp, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "PointerToSymbolTable", &PEH.FileHeader.PointerToSymbolTable, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "NumberOfSymbols", &PEH.FileHeader.NumberOfSymbols, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "SizeOfOptionalHeader", &PEH.FileHeader.SizeOfOptionalHeader, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsFileHeader_r, "Characteristics", &PEH.FileHeader.Characteristics, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
		jsOptionalHeader_r = Js_AppendObject(jsPEH_R, "OptionalHeader", this, Process::Module::jsGetFileHeader);
			Js_AppendObject(jsOptionalHeader_r, "Magic", &PEH.OptionalHeader.Magic, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MajorLinkerVersion", &PEH.OptionalHeader.MajorLinkerVersion, 0, 0, BYTE_toString_callback, BYTE_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MinorLinkerVersion", &PEH.OptionalHeader.MinorLinkerVersion, 0, 0, BYTE_toString_callback, BYTE_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfCode", &PEH.OptionalHeader.SizeOfCode, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfInitializedData", &PEH.OptionalHeader.SizeOfInitializedData, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfUninitializedData", &PEH.OptionalHeader.SizeOfUninitializedData, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "AddressOfEntryPoint", &PEH.OptionalHeader.AddressOfEntryPoint, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "BaseOfCode", &PEH.OptionalHeader.BaseOfCode, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			//Js_AppendObject(jsOptionalHeader_r, "BaseOfData", &PEH.OptionalHeader.BaseOfData, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "ImageBase", &PEH.OptionalHeader.ImageBase, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SectionAlignment", &PEH.OptionalHeader.SectionAlignment, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "FileAlignment", &PEH.OptionalHeader.FileAlignment, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MajorOperatingSystemVersion", &PEH.OptionalHeader.MajorOperatingSystemVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MinorOperatingSystemVersion", &PEH.OptionalHeader.MinorOperatingSystemVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MajorImageVersion", &PEH.OptionalHeader.MajorImageVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MinorImageVersion", &PEH.OptionalHeader.MinorImageVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MajorSubsystemVersion", &PEH.OptionalHeader.MajorSubsystemVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "MinorSubsystemVersion", &PEH.OptionalHeader.MinorSubsystemVersion, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "Win32VersionValue", &PEH.OptionalHeader.Win32VersionValue, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfImage", &PEH.OptionalHeader.SizeOfImage, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfHeaders", &PEH.OptionalHeader.SizeOfHeaders, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "CheckSum", &PEH.OptionalHeader.CheckSum, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "Subsystem", &PEH.OptionalHeader.Subsystem, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "DllCharacteristics", &PEH.OptionalHeader.DllCharacteristics, 0, 0, WORD_toString_callback, WORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfStackReserve", &PEH.OptionalHeader.SizeOfStackReserve, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfStackCommit", &PEH.OptionalHeader.SizeOfStackCommit, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfHeapReserve", &PEH.OptionalHeader.SizeOfHeapReserve, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "SizeOfHeapCommit", &PEH.OptionalHeader.SizeOfHeapCommit, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "LoaderFlags", &PEH.OptionalHeader.LoaderFlags, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
			Js_AppendObject(jsOptionalHeader_r, "NumberOfRvaAndSizes", &PEH.OptionalHeader.NumberOfRvaAndSizes, 0, 0, DWORD_toString_callback, DWORD_valueOf_callback);
	return ElModule;
}


#define getme() Module* me = (Module*)Js_GetExternPointer(arguments[0])
#define getmeget() Module* me = (Module*)Js_GetExternPointer(callbackState)


JsValueRef Process::Module::jsUpdatePEH(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	//puts("updating");
	getme();
	me->UpdatePEH();
	return me->jsPEH_R;
}

JsValueRef Process::Module::jsToString(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	getme();
	std::stringstream yeah;
	yeah << "{ name : \"" << me->name << "\", base : [0x" << std::uppercase << std::hex << (void*)me->base << "], size : [0x" << me->size << "|" << std::dec << me->size << "] }";
	return Js_MakeString(yeah.str().c_str(), yeah.str().length());
}

JsValueRef Process::Module::jsGetPEH(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	//puts("> getting PEH");
	getmeget();
	if (!me->PEH.Signature) {
		me->UpdatePEH();
	}
	return me->jsPEH_R;
}

JsValueRef Process::Module::jsGetFileHeader(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	getmeget();
	return me->jsFileHeader_r;
}

JsValueRef Process::Module::jsGetOptionalHeader(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	getmeget();
	return me->jsOptionalHeader_r;
}
