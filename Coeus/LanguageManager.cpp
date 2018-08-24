#include "LanguageManager.h"

bool FunctionProperty::Build(JsValueRef parent){
	JsValueRef funcProcessor_r = 0;
	JsPropertyIdRef funcProcessor_id = 0;
	JsCreatePropertyId(this->name, strlen(this->name), &funcProcessor_id);
	JsCreateFunction(this->function, this->callbackState, &funcProcessor_r);
	JsSetProperty(parent, funcProcessor_id, funcProcessor_r, true);
	return true;
}

bool FunctionProperty::Demolish(JsValueRef parent){
	JsValueRef result = 0;
	JsPropertyIdRef property_id = 0;
	JsCreatePropertyId(name, strlen(name), &property_id);
	JsDeleteProperty(parent, property_id, false, &result);
	return true;
}

bool ObjectProperty::Build(JsValueRef parent){
	JsCreateExternalObject(this->data, nullptr, &this->me);		//TODO: fix finalizer
	if (getter) {
		bool result;
		JsValueRef nameS_r;	JsCreateString(this->name, strlen(this->name), &nameS_r);
		JsValueRef Descriptor; JsCreateObject(&Descriptor);
		JsValueRef getterF_r; JsCreateFunction(ObjectProperty::DefaultGetter, (void*)this, &getterF_r);
		JsValueRef getterS_r; JsCreateString("get", 3, &getterS_r);
		JsObjectSetProperty(Descriptor, getterS_r, getterF_r, true);
		JsObjectDefineProperty(parent, nameS_r, Descriptor, &result);
		if (!result)return false;
		JsAddRef(this->me, nullptr);
	} else {
		JsPropertyIdRef property_id = 0;
		JsCreatePropertyId(this->name, strlen(this->name), &property_id);
		JsSetProperty(parent, property_id, this->me, true);
	}
	bool hasBeenWorking = true;
	for (int i = 0; this->properties[i] && i >= 0;) {
		if (hasBeenWorking) {												//while building properties is successful
			if (hasBeenWorking = this->properties[i]->Build(this->me))i++;	//keep doing it and increase index so next round we do the next one
			else {i--;														//but if it doesn't work, don't worry about it, we don't have to do any shutdown, just dec the index so next round we're demolishing the previous built
				printf("failed to build %s in %s\n", this->properties[i+1]->name, this->name); 
			}														
		}else{																//if it hasn't been working
			this->properties[i--]->Demolish(this->me);						//keep going down the properties list and demolishing them
		}
	}
	if (!hasBeenWorking) {													//if we couldn't get the properties built
		JsValueRef result = 0;
		JsPropertyIdRef property_id = 0;
		JsCreatePropertyId(this->name, strlen(this->name), &property_id);	//just
		JsDeleteProperty(parent, property_id, false, &result);				//delete this shit
		return false;														//everything is fucked
	}
	return true;
}

bool ObjectProperty::Demolish(JsValueRef parent){
	JsValueRef result;
	JsPropertyIdRef property_id = 0;
	JsCreatePropertyId(this->name, strlen(this->name), &property_id);	//delete the
	JsDeleteProperty(parent, property_id, false, &result);				//property
	if(this->getter)JsRelease(this->me, nullptr);						//release this object for the garbage collector to sweep up
	return true;
}

JsValueRef ObjectProperty::DefaultGetter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackstate){
	ObjectProperty* me = (ObjectProperty*)callbackstate;
	void* parentData = 0;
	bool hasData = false;
	JsHasExternalData(arguments[0], &hasData);
	if(hasData)JsGetExternalData(arguments[0], &parentData);
		
	return (me->data = me->getter(parentData, me->data)) ? me->me : JS_INVALID_REFERENCE;
}


bool BasicProperty::Build(JsValueRef parent) {
	if (!this->getter && !this->setter) {  return false; }
	JsNativeFunction* natives = &this->gsetters[this->basicType << 1];
	JsValueRef Descriptor; JsCreateObject(&Descriptor);
	if (this->getter) {
		JsValueRef getterF_r, getterS_r;
		JsCreateFunction(natives[0], (void*)this->offsetFromObject, &getterF_r);
		JsCreateString("get", 3, &getterS_r);
		JsObjectSetProperty(Descriptor, getterS_r, getterF_r, true);
	} if (this->setter) {
		JsValueRef setterF_r, setterS_r;
		JsCreateFunction(natives[1], (void*)this->offsetFromObject, &setterF_r);
		JsCreateString("get", 3, &setterS_r);
		JsObjectSetProperty(Descriptor, setterS_r, setterF_r, true);
	}
	bool result = false;
	JsValueRef nameS_r;
	JsCreateString(this->name, strlen(this->name), &nameS_r);
	JsObjectDefineProperty(parent, nameS_r, Descriptor, &result);
	return result;
}

bool BasicProperty::Demolish(JsValueRef parent) {
	JsValueRef result = 0;
	JsPropertyIdRef property_id = 0;
	JsCreatePropertyId(this->name, strlen(this->name), &property_id);
	JsDeleteProperty(parent, property_id, false, &result);
	return true;
}

JsValueRef BasicProperty::JS_BYTE_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	BYTE* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}

JsValueRef BasicProperty::JS_BYTE_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	BYTE* data = 0;
	signed argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);
	*data = argument;
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}

JsValueRef BasicProperty::JS_WORD_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	WORD* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}

JsValueRef BasicProperty::JS_WORD_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	WORD* data = 0;
	signed argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);
	*data = argument;
	DWORD data_4 = *data;
	JsIntToNumber((signed)data_4, &number);
	return number;
}

JsValueRef BasicProperty::JS_DWORD_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	DWORD* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsIntToNumber((signed)*data, &number);
	return number;

}

JsValueRef BasicProperty::JS_DWORD_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	DWORD* data = 0;
	signed argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToInt(number, &argument);
	*data = argument;
	JsIntToNumber((signed)*data, &number);
	return number;
}

JsValueRef BasicProperty::JS_FLOAT_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	float* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsDoubleToNumber(*data, &number);
	return number;

}

JsValueRef BasicProperty::JS_FLOAT_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	float* data = 0;
	double argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToDouble(number, &argument);
	*data = (float)argument;
	JsDoubleToNumber((double)*data, &number);
	return number;
}

JsValueRef BasicProperty::JS_DOUBLE_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	double* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsDoubleToNumber(*data, &number);
	return number;

}

JsValueRef BasicProperty::JS_DOUBLE_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	double* data = 0;
	double argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToNumber(arguments[1], &number);
	JsNumberToDouble(number, &argument);
	*data = argument;
	JsDoubleToNumber(*data, &number);
	return number;
}

JsValueRef BasicProperty::JS_STRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef string = 0;
	char** data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsCreateString(*data, sizeof(*data), &string);
	return string;
}

JsValueRef BasicProperty::JS_STRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	return JS_INVALID_REFERENCE;
}

JsValueRef BasicProperty::JS_WSTRING_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef string = 0;
	uint16_t** data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsCreateStringUtf16(*data, sizeof(*data), &string);
	return string;

}

JsValueRef BasicProperty::JS_WSTRING_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	return JS_INVALID_REFERENCE;
}

JsValueRef BasicProperty::JS_BOOL_getter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef boolean = 0;
	bool* data = 0;
	JsGetExternalData(arguments[0], (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsBoolToBoolean(*data, &boolean);
	return boolean;
}

JsValueRef BasicProperty::JS_BOOL_setter(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState){
	JsValueRef number = 0;
	bool* data = 0;
	bool argument = 0;
	JsGetExternalData(callbackState, (void**)&data);
	data = (decltype(data))((char*)data + (unsigned)callbackState);
	JsConvertValueToBoolean(arguments[1], &number);
	JsBooleanToBool(number, &argument);
	*data = argument;
	JsBoolToBoolean(*data, &number);
	return number;
}
