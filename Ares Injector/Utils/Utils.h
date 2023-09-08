#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

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
};