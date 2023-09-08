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

        argument = argument.substr(13);

        if (argument.ends_with("/"))
            argument.erase(argument.end()-1);

        Utils::storeToken(argument.c_str());

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

auto Utils::storeToken(const char* token) -> void {

    auto path = std::string(Utils::getAresPath());
    auto file = path + "\\Token.txt";

    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);

    try {
        
        std::ofstream ofStream(file, std::ios::out | std::ios::trunc);

        if (!ofStream.is_open()) {
            
            std::cout << "Error opening Token file for writing." << std::endl;
            return;

        };

        ofStream << token;
        ofStream.close();

    } catch (const std::exception& e) {
        std::cout << "Error writing Token to file: " << e.what() << std::endl;
    };

};

auto Utils::getToken(void) -> std::string {

    auto path = std::string(Utils::getAresPath());
    auto file = path + "\\Token.txt";

    try {
        std::ifstream ifStream(file);

        if (!ifStream.is_open())
            return "";

        std::string token;
        ifStream >> token;

        ifStream.close();
        return token;

    }
    catch (const std::exception& e) {
        return "";
    };

    return "";

};

auto Utils::isAresInjected(void) -> bool {

    auto wstrLower = [] (const std::wstring& str) -> const std::wstring& {

        std::wstring lower = str.c_str();
        
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;

    };

    auto multiByteToWide = [](const std::string& narrowStr) -> std::wstring {
        int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), -1, NULL, 0);
        if (wideStrLen == 0)
            return L"";

        std::wstring wideStr(wideStrLen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), -1, &wideStr[0], wideStrLen);

        return wideStr;
    };

    auto procName = "Minecraft.Windows.exe";

    DWORD processId = 0;
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE) {

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if(wcscmp(pe32.szExeFile, multiByteToWide(procName).c_str()) == 0) {
                    processId = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        };
        
        CloseHandle(hSnapshot);

    };

    if (processId == 0)
        return false;

    HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hModuleSnapshot == INVALID_HANDLE_VALUE)
        return false;

    MODULEENTRY32W me32 = { sizeof(MODULEENTRY32W) };
    auto result = false;
    if (Module32FirstW(hModuleSnapshot, &me32)) {
        do {
            auto moduleName = std::wstring(me32.szModule);
            if (moduleName.find(L"Ares") != std::wstring::npos) {
                result = true;
                break;
            }
        } while (Module32NextW(hModuleSnapshot, &me32));
    };

    CloseHandle(hModuleSnapshot);
    return result;

};

auto Utils::injectLatest(void) -> void {
    
    std::wstring url = L"https://github.com/OvermindMC/Ares-Releases/releases/latest/download/Ares.dll";
    std::string path = Utils::getAresPath();
    std::wstring file = std::wstring(path.begin(), path.end()) + L"\\Ares.dll";
    const wchar_t* dllPath = file.c_str();

    HRESULT downloadResult = URLDownloadToFileW(NULL, url.c_str(), file.c_str(), 0, NULL);
    if (downloadResult != S_OK) {
        std::wcout << L"Failed to fetch latest!" << std::endl;
        return;
    };

    const wchar_t* procName = L"Minecraft.Windows.exe";

    DWORD processId = 0;
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (wcscmp(pe32.szExeFile, procName) == 0) {
                    processId = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
    }

    if (processId == 0) {
        std::wcout << L"Minecraft needs to be open!" << std::endl;
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    if (hProcess) {
        LPVOID pRemoteBuf = VirtualAllocEx(hProcess, NULL, (wcslen(dllPath) + 1) * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
        WriteProcessMemory(hProcess, pRemoteBuf, dllPath, (wcslen(dllPath) + 1) * sizeof(wchar_t), NULL);

        // Load the DLL in the target process
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, pRemoteBuf, 0, NULL);
        WaitForSingleObject(hThread, INFINITE);

        // Clean up
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
    }
};