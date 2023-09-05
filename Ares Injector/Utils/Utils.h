#pragma once

#include <iostream>
#include <Windows.h>

class Utils {
public:
	static auto initCustomScheme(void) -> void;
	static auto handleCustomUri(const char*) -> void;
public:
	static auto getRoamPath(void) -> const char*;
	static auto getAresPath(void) -> const char*;
};