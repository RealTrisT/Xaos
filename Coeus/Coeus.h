///Coeus (roman Polus) - The Celestial Axis
#pragma once

#include <string>

extern "C" __declspec(dllexport) bool Coeus_RunJs(wchar_t* script, std::wstring* result_ = 0);
extern "C" __declspec(dllexport) void Coeus_CollectGarbageJs();