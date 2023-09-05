#pragma once

#include <ostream>
#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <filesystem>

class Utils {
public:
	static auto initCustomScheme(void) -> void;
	static auto handleCustomUri(const char*) -> void;
public:
	static auto getRoamPath(void) -> const char*;
	static auto getAresPath(void) -> const char*;
public:
	static auto storeToken(const char*) -> void;
};