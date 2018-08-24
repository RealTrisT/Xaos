#include <Windows.h>
#include <iostream>
#include <string>
#pragma comment(lib, "../libs/ChakraCore64.lib")




int main() {
	SetConsoleTitle("Xaos");

	HMODULE Coeus_Dll = LoadLibrary("Modules/Coeus.dll");
	if (Coeus_Dll) {
		JsValueRef Coeus_Return = 0;

		std::wstring(*Coeus_whateverToStringJs)(JsValueRef) = (std::wstring(*)(JsValueRef))GetProcAddress(Coeus_Dll, "Coeus_whateverToStringJs");
		JsErrorCode(*Coeus_RunJs)(wchar_t*, JsValueRef*) = (JsErrorCode(*)(wchar_t*, JsValueRef*))GetProcAddress(Coeus_Dll, "Coeus_RunJs");
		void(*Coeus_CollectGarbageJs)() = (void(*)())GetProcAddress(Coeus_Dll, "Coeus_CollectGarbageJs");

		((void(*)(HMODULE))GetProcAddress(Coeus_Dll, "Initialize"))(Coeus_Dll);
		Coeus_Return = ((JsValueRef(*)())GetProcAddress(Coeus_Dll, "InitializeJs"))();

		printf(": %S\n", Coeus_whateverToStringJs(Coeus_Return).c_str());

		/*---------------------test-------------------------*/
		//JsValueRef globle; JsGetGlobalObject(&globle);
		//PEH.Build(globle);
		/*---------------------test-------------------------*/
		
		std::wstring resultW; std::wstring script; JsValueRef returnValue;
		do {
			Coeus_CollectGarbageJs();
			resultW = L"";
			printf(">>"); std::getline(std::wcin, script);
			Coeus_RunJs((wchar_t*)script.c_str(), &returnValue);
			printf(": %S\n", (resultW = Coeus_whateverToStringJs(returnValue)).c_str());
		} while (wcscmp(resultW.c_str(), L"exit"));

		((void(*)())GetProcAddress(Coeus_Dll, "Terminate"))();
	}else{puts("coeus not loaded"); }

	system("pause");
	return 0;
}