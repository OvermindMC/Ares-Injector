#include "Utils.h"

auto Utils::initCustomScheme(void) -> void {

    try
    {
        TCHAR currentExecutablePath[MAX_PATH];
        DWORD pathSize = GetModuleFileName(nullptr, currentExecutablePath, MAX_PATH);

        if (pathSize == 0) {
            throw std::runtime_error("Failed to retrieve executable path.");
        };

        HKEY key;
        if (RegCreateKeyEx(HKEY_CLASSES_ROOT, L"ares", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
            
            throw std::runtime_error("Failed to create registry key.");

        };
        
        if (RegSetValueEx(key, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(L"URL: Ares Protocol"), sizeof(L"URL: Ares Protocol")) != ERROR_SUCCESS) {
            
            throw std::runtime_error("Failed to set default value.");

        };

        if (RegSetValueEx(key, L"URL Protocol", 0, REG_SZ, reinterpret_cast<const BYTE*>(L""), sizeof(L"")) != ERROR_SUCCESS) {
            
            throw std::runtime_error("Failed to set 'URL Protocol' value.");

        };

        HKEY commandKey;
        if (RegCreateKeyEx(key, L"shell\\open\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &commandKey, nullptr) != ERROR_SUCCESS) {
            
            throw std::runtime_error("Failed to create 'command' subkey.");

        };

        std::wstring commandValue = L"\"" + std::wstring(currentExecutablePath) + L"\" \"%1\"";
        if (RegSetValueEx(commandKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(commandValue.c_str()), static_cast<DWORD>(commandValue.size() * sizeof(wchar_t))) != ERROR_SUCCESS) {
            
            throw std::runtime_error("Failed to set 'command' value.");

        };

        RegCloseKey(commandKey);
        RegCloseKey(key);

    } catch (const std::exception& ex) {
        std::cout << "Error registering custom URI scheme: " << ex.what() << std::endl;
    };

};

auto Utils::handleCustomUri(const char* arg) -> void {

    auto argument = std::string(arg);

    if (argument.starts_with("ares://token=")) {

        if (argument.ends_with("/"))
            argument.erase(argument.end()-1);

        std::cout << argument;

    };

};

auto Utils::getRoamPath() -> const char* {

    char* path = nullptr;
    size_t length;

    if (_dupenv_s(&path, &length, "appdata") != 0 || path == nullptr)
        return "";

    const char* roamStatePath = "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState";
    char* finalPath = new char[length + strlen(roamStatePath) + 1];

    strcpy_s(finalPath, length + strlen(roamStatePath) + 1, path);
    strcat_s(finalPath, length + strlen(roamStatePath) + 1, roamStatePath);

    free(path);
    return finalPath;

};

auto Utils::getAresPath(void) -> const char* {

    auto roamPath = Utils::getRoamPath();
    const char* aresPath = "\\Ares";

    size_t totalLength = strlen(roamPath) + strlen(aresPath) + 1;
    char* finalPath = new char[totalLength];

    strcpy_s(finalPath, totalLength, roamPath);
    strcat_s(finalPath, totalLength, aresPath);

    delete[] roamPath;
    return finalPath;

};