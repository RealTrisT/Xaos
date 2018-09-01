#pragma once
#include <Windows.h>
#include <string>
//#include "../ChakraCore/ChakraCore.h"
//#include "../Coeus/Coeus.h"
#include "../Coeus//LanguageManager.h"

struct Module; //forward declaration

struct IMAGE_NT_HEADERS_OBJREADY : public IMAGE_NT_HEADERS {
	/*---------------------------------------------------------------FILE_HEADER--------------------------------------------------------*/
	static JsValueRef fileheader_LogShit(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
		IMAGE_FILE_HEADER* dayta = 0;
		JsGetExternalData(arguments[0], (void**)&dayta);
		printf("Machine: %.4X\nNumberOfSections: %.4X\nTimeDateStamp: %.8X\nPointerToSymbolTable: %.8X\nNumberOfSymbols: %.8X\nSizeOfOptionalHeader: %.4X\nCharacteristics: %.4X\n",
			dayta->Machine, dayta->NumberOfSections, dayta->TimeDateStamp, dayta->PointerToSymbolTable, dayta->NumberOfSymbols, dayta->SizeOfOptionalHeader, dayta->Characteristics);
		return JS_INVALID_REFERENCE;
	}
	FunctionProperty fileheader_funcProps[1] = {
		{ "LogProps", fileheader_LogShit, 0 }
	};
	BasicProperty fileheader_basicProps[7] = {
		{ "Machine",				BasicProperty::BasicType::BASETYPE_WORD,  offsetof(_IMAGE_FILE_HEADER, Machine) },
		{ "NumberOfSections",		BasicProperty::BasicType::BASETYPE_WORD,  offsetof(_IMAGE_FILE_HEADER, NumberOfSections) },
		{ "TimeDateStamp",			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(_IMAGE_FILE_HEADER, TimeDateStamp) },
		{ "PointerToSymbolTable",	BasicProperty::BasicType::BASETYPE_DWORD, offsetof(_IMAGE_FILE_HEADER, PointerToSymbolTable) },
		{ "NumberOfSymbols",		BasicProperty::BasicType::BASETYPE_DWORD, offsetof(_IMAGE_FILE_HEADER, NumberOfSymbols) },
		{ "SizeOfOptionalHeader",	BasicProperty::BasicType::BASETYPE_WORD,  offsetof(_IMAGE_FILE_HEADER, SizeOfOptionalHeader) },
		{ "Characteristics",		BasicProperty::BasicType::BASETYPE_WORD,  offsetof(_IMAGE_FILE_HEADER, Characteristics) },
	};
	Property* fileheader_arre[9] = {
		&fileheader_basicProps[0], &fileheader_basicProps[1], &fileheader_basicProps[2], &fileheader_basicProps[3], &fileheader_basicProps[4],
		&fileheader_basicProps[5], &fileheader_basicProps[6], &fileheader_funcProps[0], 0
	};
	/*---------------------------------------------------------------FILE_HEADER/--------------------------------------------------------*/
	/*-------------------------------------------------------------OPTIONAL_HEADER-------------------------------------------------------*/
	BasicProperty optheader_basicProps[29] = {
		{ "Magic", 						BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, Magic) },
		{ "MajorLinkerVersion", 		BasicProperty::BasicType::BASETYPE_BYTE,  offsetof(IMAGE_OPTIONAL_HEADER, MajorLinkerVersion) },
		{ "MinorLinkerVersion", 		BasicProperty::BasicType::BASETYPE_BYTE,  offsetof(IMAGE_OPTIONAL_HEADER, MinorLinkerVersion) },
		{ "SizeOfCode", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfCode) },
		{ "SizeOfInitializedData", 		BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfInitializedData) },
		{ "SizeOfUninitializedData", 	BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfUninitializedData) },
		{ "AddressOfEntryPoint", 		BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, AddressOfEntryPoint) },
		{ "BaseOfCode", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, BaseOfCode) },
		//{ "BaseOfData", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, BaseOfData) },
		{ "ImageBase", 					BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, ImageBase) },
		{ "SectionAlignment", 			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SectionAlignment) },
		{ "FileAlignment", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, FileAlignment) },
		{ "MajorOperatingSystemVersion",BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MajorOperatingSystemVersion) },
		{ "MinorOperatingSystemVersion",BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MinorOperatingSystemVersion) },
		{ "MajorImageVersion", 			BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MajorImageVersion) },
		{ "MinorImageVersion", 			BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MinorImageVersion) },
		{ "MajorSubsystemVersion", 		BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MajorSubsystemVersion) },
		{ "MinorSubsystemVersion", 		BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, MinorSubsystemVersion) },
		{ "Win32VersionValue", 			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, Win32VersionValue) },
		{ "SizeOfImage", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfImage) },
		{ "SizeOfHeaders", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeaders) },
		{ "CheckSum", 					BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, CheckSum) },
		{ "Subsystem", 					BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, Subsystem) },
		{ "DllCharacteristics", 		BasicProperty::BasicType::BASETYPE_WORD,  offsetof(IMAGE_OPTIONAL_HEADER, DllCharacteristics) },
		{ "SizeOfStackReserve", 		BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfStackReserve) },
		{ "SizeOfStackCommit", 			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfStackCommit) },
		{ "SizeOfHeapReserve", 			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeapReserve) },
		{ "SizeOfHeapCommit", 			BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, SizeOfHeapCommit) },
		{ "LoaderFlags", 				BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, LoaderFlags) },
		{ "NumberOfRvaAndSizes", 		BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_OPTIONAL_HEADER, NumberOfRvaAndSizes) }
	};
	Property* optheader_arre[30] = {
		&optheader_basicProps[0], &optheader_basicProps[1], &optheader_basicProps[2], &optheader_basicProps[3], &optheader_basicProps[4], &optheader_basicProps[5], &optheader_basicProps[6],
		&optheader_basicProps[7], &optheader_basicProps[8], &optheader_basicProps[9], &optheader_basicProps[10], &optheader_basicProps[11], &optheader_basicProps[12], &optheader_basicProps[13],
		&optheader_basicProps[14], &optheader_basicProps[15], &optheader_basicProps[16], &optheader_basicProps[17], &optheader_basicProps[18], &optheader_basicProps[19], &optheader_basicProps[20],
		&optheader_basicProps[21], &optheader_basicProps[22], &optheader_basicProps[23], &optheader_basicProps[24], &optheader_basicProps[25], &optheader_basicProps[26], &optheader_basicProps[27],
		&optheader_basicProps[28], 0
	};
	/*-------------------------------------------------------------OPTIONAL_HEADER/-----------------------------------------------------*/
	/*----------------------------------------------------------------PE_HEADER---------------------------------------------------------*/
	BasicProperty peh_basicProps[1] = {
		{ "Signature", BasicProperty::BasicType::BASETYPE_DWORD, offsetof(IMAGE_NT_HEADERS, Signature) },
	};
	ObjectProperty peh_objProps[2] = {
		{ "FileHeader", &FileHeader, fileheader_arre },
		{ "OptHeader",  &OptionalHeader, optheader_arre }
	};
	Property* peh_arre[4] = { &peh_basicProps[0], &peh_objProps[0], &peh_objProps[1], 0 };
	/*----------------------------------------------------------------PE_HEADER/--------------------------------------------------------*/
};

struct Process {
	bool Open(DWORD ProcID);
	bool Open(char* ProcName);
	bool Open_WindowTitle(char* ProcWindowTitle);
	void Close();
	HANDLE procH; DWORD procID;
	char procP[MAX_PATH] = { 0 };
	bool ReadMem(BYTE* ForeignAddress, BYTE* Buffer, DWORD Length);
	template<typename T>bool ReadMem(BYTE* ForeignAddress, T& Buffer);
	bool WriteMem(BYTE* ForeignAddress, BYTE* Buffer, DWORD Length);
	template<typename T>bool WriteMem(BYTE* ForeignAddress, T Buffer);
	Module GetModule(char* modName);
};

struct Module {
	Module() {}
	Module(const Module &cop) : daddy(cop.daddy), base(cop.base), size(cop.size), name(cop.name) {}

	Process* daddy;
	unsigned char* base;
	unsigned size;
	std::string name;
	IMAGE_NT_HEADERS_OBJREADY PEH = {};
	IMAGE_SECTION_HEADER* Sections = 0;
	IMAGE_DATA_DIRECTORY DataDirectory[16];

	void UpdatePEH();
};

struct Module_OBJREADY : public Module, public GeneratableObject {
	Module_OBJREADY() : GeneratableObject(Module_props, this) {};
	Module_OBJREADY(Module mod) : Module(mod), GeneratableObject(Module_props, this) {};

	static void* PEH_getter(Module_OBJREADY* daddyData, IMAGE_NT_HEADERS64* medata);
	static JsValueRef Module_UpdatePEH(JsValueRef c, bool cc, JsValueRef * a, unsigned short ac, void * cs);
	static JsValueRef Module_toString(JsValueRef, bool, JsValueRef*, unsigned short, void*);

	FunctionProperty Module_fncProps[2] = {
		{ "UpdatePEH", Module_OBJREADY::Module_UpdatePEH, this },
		{ "toString", Module_OBJREADY::Module_toString, this }
	};
	ObjectProperty   Module_objProps[1] = {
		{ "PEH", &PEH, PEH.peh_arre, (ObjectProperty::GETTERF)PEH_getter }
	};
	BasicProperty    Module_bscProps[4] = {
		{ "Name",     BasicProperty::BasicType::BASETYPE_STDSTRING, offsetof(Module, name) },
		{ "BaseLow",  BasicProperty::BasicType::BASETYPE_DWORD,     offsetof(Module, base) },
		{ "BaseHigh", BasicProperty::BasicType::BASETYPE_DWORD,     offsetof(Module, base) + 4 },
		{ "Size",     BasicProperty::BasicType::BASETYPE_DWORD,     offsetof(Module, size) }
	};
		
	enum struct PropArrayIndexes:unsigned char {
		UpdatePEH_i = 0,	PEH_i      = 0,	Name_i = 0,		
		BaseLow_i   = 1,	BaseHigh_i = 2,	Size_i = 3,
	};

	Property*        Module_props[8] = {
		&Module_fncProps[0], &Module_fncProps[1], &Module_objProps[0], &Module_bscProps[0],
		&Module_bscProps[1], &Module_bscProps[2], &Module_bscProps[3], 0
	};
};

struct Process_OBJREADY : public Process, public GeneratableObject {
	Process_OBJREADY() : GeneratableObject(Process_props, this) {}

	JsValueRef jsProcessObject;
	static JsValueRef CALLBACK jsToString(JsValueRef, bool, JsValueRef*, unsigned short, void*);
	static JsValueRef CALLBACK jsClose(JsValueRef, bool, JsValueRef*, unsigned short, void*);
	static JsValueRef CALLBACK jsGetModule(JsValueRef, bool, JsValueRef*, unsigned short, void*);
	FunctionProperty Process_fncProps[3] = {
		{ "toString", Process_OBJREADY::jsToString, this },
		{ "Close",    Process_OBJREADY::jsClose, this },
		{ "GetModule", Process_OBJREADY::jsGetModule, this }
	};
	Property* Process_props[4]{
		&Process_fncProps[0], &Process_fncProps[1], &Process_fncProps[2], 0
	};

};