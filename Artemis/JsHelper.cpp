#include "JsHelper.h"

#include <sstream>

JsValueType Js_GetType(JsValueRef r) {
	JsValueType ye;
	JsGetValueType(r, &ye);
	return ye;
}
JsValueRef Js_GetNull() {
	JsValueRef ye = 0;
	JsGetNullValue(&ye);
	return ye;
}
size_t Js_GetStringSize(JsValueRef r) {
	size_t size;
	JsCopyString(r, 0, 0, &size);
	return size;
}

std::string Js_GetString(JsValueRef r) {
	size_t modname_size = Js_GetStringSize(r);
	char* modname = (char*)malloc(modname_size+1);
	JsCopyString(r, modname, modname_size+1, &modname_size);
	modname[modname_size] = 0;
	std::string ye(modname); free(modname);
	return ye;
}

void* Js_GetExternPointer(JsValueRef r) {
	void* p;
	JsGetExternalData(r, &p);
	return p;
}

JsValueRef Js_AppendObjectWithDescriptor(JsValueRef parent, void * callback, void * getter, void * setter){
	JsValueRef getter_r; JsCreateFunction((JsNativeFunction)getter, nullptr, &getter_r);
	JsValueRef descriptor_r; JsCreateObject(&descriptor_r);
	JsValueRef getstring_r;	JsCreateString("get", 3, &getstring_r);

	//JsObjectSetProperty
}

JsValueRef Js_MakeString(const char* b, size_t l) {
	JsValueRef ye = 0;
	JsCreateString(b, l, &ye);
	return ye;
}

JsValueRef Js_AppendFunction(JsValueRef parent, const char* name, void* funcptr) {
	JsValueRef funcRef = 0;
	JsPropertyIdRef funcId = 0;
	JsCreatePropertyId(name, strlen(name), &funcId);
	JsCreateFunction((JsNativeFunction)funcptr, nullptr, &funcRef);
	JsSetProperty(parent, funcId, funcRef, true);
	return funcRef;
}

void Js_AddGetterSetter(JsValueRef object, const char* propertyName, void* getterCallback, void* setterCallback, void *callbackState) {
	if (!getterCallback && !setterCallback)return;
	JsValueRef getter;
	JsValueRef setter;
	JsValueRef descriptor;
	JsValueRef get;
	JsValueRef set;
	JsValueRef key;
	bool result;

	JsCreateObject(&descriptor); // step 2
	//------------------------------------------------------------------------------------------
	if (getterCallback) {
		JsCreateFunction((JsNativeFunction)getterCallback, callbackState, &getter); // step 3

		JsCreateString("get", 3, &get); // step 4
		JsObjectSetProperty(descriptor, get, getter, true); // step 5
	}if (setterCallback) {
		JsCreateFunction((JsNativeFunction)setterCallback, callbackState, &setter); // step 3

		JsCreateString("set", 3, &set); // step 4
		JsObjectSetProperty(descriptor, set, setter, true); // step 5
	}
	//------------------------------------------------------------------------------------------

	JsCreateString(propertyName, strlen(propertyName), &key); // step 6
	JsObjectDefineProperty(object, key, descriptor, &result); // step 7
	if (!result) { puts("didnt work");/* it failed? consider if this is possible in your programme if yes handle it`*/ }
}

JsValueRef Js_AppendObject(JsValueRef parent, const char* name, void* Value, void* getter, void* setter, void* toString, void* valueOf) {
	JsValueRef obj = 0; JsPropertyIdRef propid = 0;
	//--------------------------------------------------create objec stuff
	JsCreatePropertyId(name, strlen(name), &propid);
	JsCreateExternalObject(Value, nullptr, &obj);
	//--------------------------------------------------do str func
	if (toString) Js_AppendFunction(obj, "toString", toString);
	//--------------------------------------------------do valueOf func
	if (valueOf)  Js_AppendFunction(obj, "valueOf", valueOf);
	//--------------------------------------------------set it all onto the parent
	JsSetProperty(parent, propid, obj, true);
	//--------------------------------------------------descriptor stuff
	if (getter || setter) {Js_AddGetterSetter(parent, name, getter, setter, obj);}
	//---------------------------return
	return obj;
}

JsValueRef CALLBACK DWORD_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	DWORD* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	JsIntToNumber(*data, &number);
	return number;
}
JsValueRef CALLBACK DWORD_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	DWORD* data = 0;
	signed argument = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);
	*data = argument;
	JsIntToNumber(*data, &number);
	return number;
}

JsValueRef CALLBACK WORD_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	WORD* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}
JsValueRef CALLBACK WORD_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	WORD* data = 0;
	signed argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);

	*data = argument;

	JsIntToNumber(*data, &number);
	return number;
}

JsValueRef CALLBACK BYTE_valueOf_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	BYTE* data = 0;
	JsGetExternalData(callbackState, (void**)&data);
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}
JsValueRef CALLBACK BYTE_set_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	JsValueRef number = 0;
	BYTE* data = 0;
	signed argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);

	*data = argument;

	JsIntToNumber(*data, &number);
	return number;
}

JsValueRef CALLBACK DWORD_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	DWORD* me = (DWORD*)Js_GetExternPointer(arguments[0]);
	char bffr[11] = { 0 };
	sprintf_s(bffr, "0x%.8X", *me);
	return Js_MakeString(bffr, 10);
}

JsValueRef CALLBACK WORD_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	WORD* me = (WORD*)Js_GetExternPointer(arguments[0]);
	char bffr[7] = { 0 };
	sprintf_s(bffr, "0x%.4X", *me);
	return Js_MakeString(bffr, 6);
}

JsValueRef CALLBACK BYTE_toString_callback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState) {
	BYTE* me = (BYTE*)Js_GetExternPointer(arguments[0]);
	char bffr[5] = { 0 };
	sprintf_s(bffr, "0x%.2X", *me);
	return Js_MakeString(bffr, 4);
}
