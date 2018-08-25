#pragma once
#include <stdio.h>

#include "../ChakraCore/ChakraCore.h"

#define EXPORTCLASS __declspec(dllexport)

struct EXPORTCLASS Property {
	Property(const char* Name) : name(Name){}
	virtual bool Build(JsValueRef parent) = 0;
	virtual bool Demolish(JsValueRef parent) = 0;
	const char* name;
};


struct EXPORTCLASS ObjectProperty :Property {
	typedef void* (*GETTERF)(void* parentData, void* oldData);
	bool Build(JsValueRef parent);
	bool Demolish(JsValueRef parent);
	ObjectProperty(const char* Name, void* Data, Property** Properties, void* (*Getter)(void* parentData, void* oldData) = 0) : Property(Name), data(Data), properties(Properties), getter(Getter) {}
	void* data;						//if getter is set, this will be set to it's return
	Property** properties;
	void* (*getter)(void* parentData, void* oldData);	//param data here will be set to the parent's external data, but the return will be what this object's return data is set to. If set to zero, the object will return undefined
	JsValueRef me;
private:
	static JsValueRef DefaultGetter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackstate);
};

struct EXPORTCLASS FunctionProperty :Property {
	bool Build(JsValueRef parent);
	bool Demolish(JsValueRef parent);
	FunctionProperty(const char* Name, JsNativeFunction Func, void* cbState) : Property(Name), function(Func), callbackState(cbState){}
	JsNativeFunction function;
	void* callbackState;
};

//this works with getters and setters

struct EXPORTCLASS BasicProperty : public Property {
	bool Build(JsValueRef parent);
	bool Demolish(JsValueRef parent);
	enum BasicType {BASETYPE_BYTE, BASETYPE_WORD, BASETYPE_DWORD, BASETYPE_FLOAT, BASETYPE_DOUBLE, BASETYPE_STRING, BASETYPE_WSTRING, BASETYPE_STDSTRING, BASETYPE_STDWSTRING, BASETYPE_BOOL};
	BasicProperty(const char* Name, BasicProperty::BasicType tip, unsigned offs, bool hasGetter = true, bool hasSetter = false) : Property(Name), basicType(tip), offsetFromObject(offs), getter(hasGetter), setter(hasSetter){}

	BasicType basicType;
	unsigned offsetFromObject;
	bool getter, setter;
private:
	static JsValueRef JS_BYTE_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_BYTE_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_WORD_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_WORD_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_DWORD_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_DWORD_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_FLOAT_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_FLOAT_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_DOUBLE_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_DOUBLE_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_WSTRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_WSTRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STDSTRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STDSTRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STDWSTRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_STDWSTRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_BOOL_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	static JsValueRef JS_BOOL_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);



	JsNativeFunction gsetters[16] = {
		JS_BYTE_getter, JS_BYTE_setter,
		JS_WORD_getter, JS_WORD_setter,
		JS_DWORD_getter, JS_DWORD_setter,
		JS_FLOAT_getter, JS_FLOAT_setter,
		JS_DOUBLE_getter, JS_DOUBLE_setter,
		JS_STRING_getter, JS_STRING_setter,
		JS_WSTRING_getter, JS_WSTRING_setter,
		JS_BOOL_getter, JS_BOOL_setter,
	};
};


//this class is meant to be inherited by objects of which multiple instances can be spawned by say a function
struct EXPORTCLASS GeneratableObject {
	//this constructor takes the derived class's object properties pointer, and a finalizer
	GeneratableObject(Property** objprops, void* derivedClassPtr, void(*finalizer)(void*) = 0) : ObjectProperties(objprops), DerivedClassPtr(derivedClassPtr), Finalizer(finalizer) {}
	Property** ObjectProperties;
	void(*Finalizer)(void*);
	JsValueRef jsMe;
	//this generates the object and returns a ref to it
	JsValueRef GenerateObject(bool* worked = 0);
private:
	void* DerivedClassPtr;
};