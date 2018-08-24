///Coeus (roman Polus) - The Celestial Axis
#pragma once

#include <Windows.h>
#include <string>
#include "../ChakraCore/ChakraCore.h"

extern "C" __declspec(dllexport) void Initialize(HMODULE me);
extern "C" __declspec(dllexport) JsValueRef InitializeJs();
extern "C" __declspec(dllexport) void Terminate();

extern "C" __declspec(dllexport) JsErrorCode Coeus_RunJs(wchar_t* script, JsValueRef* result_);
extern "C" __declspec(dllexport) void Coeus_CollectGarbageJs();
extern "C" __declspec(dllexport) std::wstring Coeus_whateverToStringJs(JsValueRef ey);

struct Coeus {
	bool initJS();
	void termJS();
	JsErrorCode runJs(wchar_t* script, JsValueRef* result_);
	std::wstring WhateverToStringJs(JsValueRef r);
	std::wstring stringResultJs();
	HMODULE LoadModule(char* modName, JsValueRef* ModuleMessage = 0);

	unsigned currentSourceContext = 0;
	JsRuntimeHandle jsRuntime;
	JsContextRef jsContext;
	JsValueRef result;
}* coeus;
