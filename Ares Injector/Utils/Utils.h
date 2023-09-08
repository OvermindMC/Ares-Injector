#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "urlmon.lib")

#include <filesystem>
#include <TlHelp32.h>
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <codecvt>
#include <ostream>
#include <fstream>
#include <string>
#include <locale>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Utils {
public:
	static auto initCustomScheme(void) -> void;
	static auto handleCustomUri(const char*) -> void;
public:
	static auto getRoamPath(void) -> const char*;
	static auto getAresPath(void) -> const char*;
public:
	static auto storeToken(const char*) -> void;
	static auto getToken(void) -> std::string;
public:
	static auto isAresInjected(void) -> bool;
	static auto injectLatest(void) -> void;
};