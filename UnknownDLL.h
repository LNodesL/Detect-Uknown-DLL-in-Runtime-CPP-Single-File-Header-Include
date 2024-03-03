#ifndef UKNOWNDLL_H
#define UKNOWNDLL_H

#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <vector>
#include <string>

// Function to get the file name of the current executable
std::string GetExecutableFileName() {
    char szFileName[MAX_PATH];
    GetModuleFileNameA(NULL, szFileName, MAX_PATH);
    return szFileName;
}

// Function to enumerate loaded modules and detect unknown DLLs
void DetectUnknownModules() {
    DWORD processId = GetCurrentProcessId();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process." << std::endl;
        return;
    }

    HMODULE hModules[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        int moduleCount = cbNeeded / sizeof(HMODULE);
        std::vector<std::string> knownModules = {
            "ntdll.dll",
            "KERNEL32.DLL",
            "KERNELBASE.dll",
            "apphelp.dll",
            "ucrtbase.dll"
        };

        // Add the current executable to the list of known modules
        knownModules.push_back(GetExecutableFileName());

        for (int i = 0; i < moduleCount; i++) {
            char szModName[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hModules[i], szModName, sizeof(szModName))) {
                std::string moduleName = szModName;
                bool isKnownModule = false;
                for (const auto& knownModule : knownModules) {
                    if (moduleName.find(knownModule) != std::string::npos) {
                        isKnownModule = true;
                        break;
                    }
                }
                if (!isKnownModule) {
                    std::cout << "Unknown DLL loaded: " << moduleName << std::endl;
                    exit(0);
                    // Handle detection of unknown module
                }
            }
        }
    } else {
        std::cerr << "Failed to enumerate process modules." << std::endl;
    }

    CloseHandle(hProcess);
}

#endif // UKNOWNDLL_H
