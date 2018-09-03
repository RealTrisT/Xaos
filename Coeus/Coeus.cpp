#include "Coeus.h"
#include <sstream>
#include <vector>

#include "../Xaos/Xaos.h"

#include "../ChakraCore/ChakraCore.h"

#ifndef _WIN64			//32
#pragma comment(lib, "../libs/ChakraCore32.lib")
#else					//64
#pragma comment(lib, "../libs/ChakraCore64.lib")
#endif

struct Coeus {
	bool initJS();
	void termJS();
	JsErrorCode runJs(wchar_t* script, JsValueRef* result_);
	std::wstring WhateverToStringJs(JsValueRef r);
	std::wstring stringResultJs();

	unsigned currentSourceContext = 0;
	JsRuntimeHandle jsRuntime;
	JsContextRef jsContext;
	JsValueRef result;
}*coeus;


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

std::wstring Coeus::WhateverToStringJs(JsValueRef r) {
	bool isNotObject = false;
	bool hasToString = false;
	JsPropertyIdRef toString;

	JsValueType type;
	JsGetValueType(r, &type);
	if (type == JsObject) {
		JsGetPropertyIdFromName(L"toString", &toString);
		JsHasOwnProperty(r, toString, &hasToString);
	}
	else isNotObject = true;
	if (hasToString || isNotObject) {
		JsValueRef resultJSString;
		JsConvertValueToString(result, &resultJSString);

		const wchar_t *resultWC;
		size_t stringLength;
		JsStringToPointer(resultJSString, &resultWC, &stringLength);
		return std::wstring(resultWC);
	}
	return std::wstring(L"::Object does not possess toString method::"); //TODO: decompose object into it's members and print that
}

std::wstring Coeus::stringResultJs() {
	return this->WhateverToStringJs(this->result);
}

//------------------------------------------------------------------------------------------------------------------------




bool InitializeCoeus(unsigned char* me) {
	coeus = new Coeus();
	return coeus->initJS();
}

void TerminateCoeus() {
	coeus->termJS();
	delete coeus;
}

extern "C" __declspec(dllexport) Xaos_ModuleInfo ModuleInfo = {
	"-------------------------------- COEUS LOADED -----------------------------",
	"Coeus: The celestial axis around which the heavens revolve\nThis module contains(/will contain) scripting stuff, as well as console-related stuff, gl.\n",
	0,
	InitializeCoeus,
	TerminateCoeus
};

bool Coeus_RunJs(wchar_t * script, std::wstring* result_){
	JsValueRef scriptResult;
	bool funcResult = (JsNoError == coeus->runJs(script, &scriptResult));
	if(funcResult && result_)*result_ = coeus->WhateverToStringJs(scriptResult);
	return funcResult;
}

void Coeus_CollectGarbageJs(){
	JsCollectGarbage(coeus->jsRuntime);
}