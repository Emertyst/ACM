#pragma once

#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <string>
#include <regex>
#include <vector>

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include "resource.h"

#ifdef UNICODE

typedef std::wstring String;
#define System _wsystem
#define toString(x) std::to_wstring(x)
#define cerr std::wcerr
#define cout std::wcout

String convert(const std::string& str) {
	String res;
	for (char c : str)
		res.push_back((TCHAR)c);
	return res;
}

#else

typedef std::string String;
#define System system
#define toString(x) std::to_string(x)
#define cerr std::cerr
#define cout std::cout

String convert(const std::string& str) { return str; }

#endif

typedef long long ll;