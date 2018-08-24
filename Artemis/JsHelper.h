#pragma once

#include <windows.h>
#include <string>
#include "../ChakraCore/ChakraCore.h"

JsValueType Js_GetType(JsValueRef r);
JsValueRef Js_GetNull();
size_t Js_GetStringSize(JsValueRef r);

std::string Js_GetString(JsValueRef r);

void* Js_GetExternPointer(JsValueRef r);

JsValueRef Js_AppendObjectWithDescriptor(JsValueRef parent, void* callback, void* getter = 0, void* setter = 0);

JsValueRef Js_MakeString(const char* b, size_t l);

JsValueRef Js_AppendFunction(JsValueRef parent, const char* name, void* funcptr);

JsValueRef Js_AppendObject(JsValueRef parent, const char* name, void* Value, void* getter = 0, void* setter = 0, void* toString = 0, void* valueOf = 0);

JsValueRef CALLBACK DWORD_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK DWORD_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK DWORD_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

JsValueRef CALLBACK WORD_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK WORD_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK WORD_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

JsValueRef CALLBACK BYTE_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK BYTE_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
JsValueRef CALLBACK BYTE_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

enum JsPropertyType {
	JS_PROPRTYPE_GETTER,
	JS_PROPRTYPE_SETTER,
	JS_PROPRTYPE_FUNCTION,
	JS_PROPRTYPE_OBJECT,
};