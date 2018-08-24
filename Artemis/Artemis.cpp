#include "Artemis.h"
#include "Process.h"

#include "JsHelper.h"

#ifndef _WIN64			//32
#pragma comment(lib, "../libs/ChakraCore32.lib")
#else					//64
#pragma comment(lib, "../libs/ChakraCore64.lib")
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------Raw Artemis-----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

JsValueRef CALLBACK Artemis_OpenProcess(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short count, void* callbackState) {
	if (count > 1) {
		JsValueType retType;
		JsGetValueType(arguments[1], &retType);

		Process* process = new Process();
		if (retType == JsNumber) {
			unsigned pid;
			JsNumberToInt(arguments[1], (signed*)&pid);
			if (process->Open(pid)) {
				return process->BuildJsObject();
			} else {
				delete process;
				goto Artemis_OpenProcess_failiure;
			}
		} else if (retType == JsString) {
			size_t ProcNameSize = 0;
			JsCopyString(arguments[1], 0, 0, &ProcNameSize);
			if (ProcNameSize > _MAX_FNAME) { delete process; goto Artemis_OpenProcess_failiure; }
			char* nameBuffer = (char*)calloc(ProcNameSize+1, 1);
			JsCopyString(arguments[1], nameBuffer, ProcNameSize + 1, &ProcNameSize);
			if (process->Open(nameBuffer)) { return process->BuildJsObject();}
			else { delete process; goto Artemis_OpenProcess_failiure; }
		} else {
			goto Artemis_OpenProcess_failiure;
		}
	}
Artemis_OpenProcess_failiure:
	JsValueRef returnv; JsGetNullValue(&returnv); return returnv;
}

JsValueRef
	ArtemisObjectVR,
		OpenProcessVR;
JsPropertyIdRef
	ArtemisObjectPI,
		OpenProcessPI;

JsValueRef CALLBACK Artemis_Get(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short count, JsValueRef callbackState) {
	return callbackState;
}



JsValueRef InitializeJs() {
	JsValueRef global;
	JsGetGlobalObject(&global);							//get global object

	ArtemisObjectVR = Js_AppendObject(global, "Artemis", 0, Artemis_Get);
	Js_AppendFunction(ArtemisObjectVR, "OpenProcess", Artemis_OpenProcess);
	
	JsValueRef retstring;
	JsCreateString("\
------------------------------------------------------------\n\
: ----------------ARTEMIS---SUCCESSFULLY---LOADED-------------\n\
: ------------------------------------------------------------\
", 187, &retstring);
	return retstring;
}
