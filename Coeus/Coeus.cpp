#include "Coeus.h"
#include <sstream>
#include <vector>

#ifndef _WIN64			//32
#pragma comment(lib, "../libs/ChakraCore32.lib")
#else					//64
#pragma comment(lib, "../libs/ChakraCore64.lib")
#endif




struct Module{
	HMODULE m;
	std::string name;
};
std::vector<Module> LoadedModules;





bool Coeus::initJS() {
	// Create a jsRuntime. 
	if (JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &jsRuntime) != JsNoError)return false;

	// Create an execution jsContext. 
	if (JsCreateContext(jsRuntime, &jsContext) != JsNoError) {
		JsDisposeRuntime(jsRuntime);
		return false;
	}

	// Now set the current execution jsContext.
	if (JsSetCurrentContext(jsContext) != JsNoError) {
		JsDisposeRuntime(jsRuntime);
		return false;
	};
	return true;
}

void Coeus::termJS() {
	JsSetCurrentContext(JS_INVALID_REFERENCE);
	JsDisposeRuntime(jsRuntime);
}

JsErrorCode Coeus::runJs(wchar_t* script, JsValueRef* result_) {
	JsErrorCode retval = JsRunScript(script, currentSourceContext++, L"", &result);
	if (result_)*result_ = result;
	return retval;
}

std::wstring Coeus::WhateverToStringJs(JsValueRef r){
	bool isNotObject = false;
	bool hasToString = false;
	JsPropertyIdRef toString;

	JsValueType type;
	JsGetValueType(r, &type);
	if (type == JsObject) {
		JsGetPropertyIdFromName(L"toString", &toString);
		JsHasOwnProperty(r, toString, &hasToString);
	} else isNotObject = true;
	if (hasToString || isNotObject) {
		JsValueRef resultJSString;
		JsConvertValueToString(result, &resultJSString);

		const wchar_t *resultWC;
		size_t stringLength;
		JsStringToPointer(resultJSString, &resultWC, &stringLength);
		//puts("> Coeus - > Return1");
		return std::wstring(resultWC);
	}
	//puts("> Coeus - > Return2");
	return std::wstring(L"::Object does not possess toString method::");
}

std::wstring Coeus::stringResultJs() {
	return this->WhateverToStringJs(this->result);
}




HMODULE Coeus::LoadModule(char * modName, JsValueRef* ModuleMessage){
	HMODULE loaded = 0;
	std::string modPath("");
	modPath.append("Modules/").append(modName).append(".dll");
	loaded = LoadLibraryA(modPath.c_str());
	if (loaded) {
		LoadedModules.insert(LoadedModules.begin(), { loaded, std::string(modName) });
		void(*Call)(HMODULE) = (void(*)(HMODULE))GetProcAddress(loaded, "Initialize");
		if (Call)Call(loaded);
		JsValueRef(*CallJs)() = (JsValueRef(*)())GetProcAddress(loaded, "InitializeJs");
		if (CallJs) {
			if(ModuleMessage)*ModuleMessage = CallJs();
			else CallJs();
		} else if (ModuleMessage)*ModuleMessage = JS_INVALID_REFERENCE;
		return loaded;
	} else {
		puts("> Could Not Load Module");
		return 0;
	}
}








// a native function for logging
JsValueRef CALLBACK Coeus_LoadModuleJsCallback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState){
	if (argumentCount <= 1) goto LoadCallback_Fail;

	JsValueType argtype;
	JsGetValueType(arguments[1], &argtype);
	if (argtype != JsString) goto LoadCallback_Fail;

	JsValueRef ModuleMessage;
	char modName[_MAX_FNAME];
	memset(modName, 0, _MAX_FNAME);
	size_t written;
	JsCopyString(arguments[1], modName, _MAX_FNAME, &written);
	if(!coeus->LoadModule(modName, &ModuleMessage))goto LoadCallback_Fail;

	return ModuleMessage;
	
LoadCallback_Fail:
	JsValueRef null;
	JsGetNullValue(&null);
	return null;

}



void Initialize(HMODULE me){
	coeus = new Coeus();
	coeus->initJS();
	LoadedModules.insert(LoadedModules.begin(), { me, std::string("Coeus") });
}

JsValueRef InitializeJs(){
	JsValueRef Coeus_r, loadFunc_r, global;
	JsPropertyIdRef Coeus_id, loadFunc_id;
	// create console object, log function, and set log function as property of console
	JsCreateObject(&Coeus_r);
	JsCreateFunction(Coeus_LoadModuleJsCallback, nullptr, &loadFunc_r);
	JsCreatePropertyId("LoadModule", 10, &loadFunc_id);
	JsSetProperty(Coeus_r, loadFunc_id, loadFunc_r, true);
	// set console as property of global object
	JsGetGlobalObject(&global);
	JsCreatePropertyId("Coeus", 5, &Coeus_id);
	JsSetProperty(global, Coeus_id, Coeus_r, true);

	JsValueRef retstring;
	JsCreateString("\
------------------------------------------------------------\n\
: ------------------COEUS---SUCCESSFULLY---LOADED-------------\n\
: ------------------------------------------------------------\
", 187, &retstring);
	return (coeus->result = retstring);
}

void Terminate(){
	coeus->termJS();
	delete coeus;
}

JsErrorCode Coeus_RunJs(wchar_t * script, JsValueRef * result_){
	return coeus->runJs(script, result_);
}

void Coeus_CollectGarbageJs(){
	JsCollectGarbage(coeus->jsRuntime);
}

std::wstring Coeus_whateverToStringJs(JsValueRef ey){
	return coeus->WhateverToStringJs(ey);
}