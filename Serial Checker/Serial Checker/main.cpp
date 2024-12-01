#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#include <consoleapi.h>
#include <iostream>
#include <WinBase.h>

void setConsoleTextColor(int r, int g, int b) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwMode;
    GetConsoleMode(hConsole, &dwMode);
    SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
}

void resetConsoleTextColor() {
    std::cout << "\033[0m";
}

void printWithGradient(const std::wstring& text, int startR, int startG, int startB, int endR, int endG, int endB) {
    int len = static_cast<int>(text.size());
    for (int i = 0; i < len; ++i) {
        int r = startR + (endR - startR) * i / (len - 1);
        int g = startG + (endG - startG) * i / (len - 1);
        int b = startB + (endB - startB) * i / (len - 1);
        setConsoleTextColor(r, g, b);
        std::wcout << text[i];
    }
    resetConsoleTextColor();
    std::wcout << std::endl;
}

void initializeCOM() {
    HRESULT hres;

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) {
        std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        exit(1);
    }

    hres = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_DEFAULT, 
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr, 
        EOAC_NONE,
        nullptr
    );

    if (FAILED(hres)) {
        std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        CoUninitialize();
        exit(1);
    }
}

IWbemServices* connectWMI() {
    HRESULT hres;
    IWbemLocator* pLoc = nullptr;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        reinterpret_cast<LPVOID*>(&pLoc)
    );

    if (FAILED(hres)) {
        std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        CoUninitialize();
        exit(1);
    }

    IWbemServices* pSvc = nullptr;

    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        nullptr,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        &pSvc
    );

    pLoc->Release();

    if (FAILED(hres)) {
        std::cerr << "Could not connect to WMI. Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        CoUninitialize();
        exit(1);
    }

    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE, 
        nullptr,
        EOAC_NONE
    );

    if (FAILED(hres)) {
        std::cerr << "Could not set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        pSvc->Release();
        CoUninitialize();
        exit(1);
    }

    return pSvc;
}

void queryWMI(IWbemServices* pSvc, const std::wstring& query, const std::wstring& propertyName) {
    HRESULT hres;
    IEnumWbemClassObject* pEnumerator = nullptr;

    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(query.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator
    );

    if (FAILED(hres)) {
        std::wcerr << L"Query failed: " << query << L" Error code = 0x" << std::hex << hres << std::endl;
        Sleep(2000);
        return;
    }

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    // Iterate through the results
    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn) {
            break;
        }

        VARIANT vtProp;
        hr = pclsObj->Get(propertyName.c_str(), 0, &vtProp, 0, 0);

        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
            // Gradient color for the serial number
            std::wstring serialNumber = vtProp.bstrVal;
            printWithGradient(serialNumber, 173, 216, 230, 128, 0, 128); // Light blue to purple
        }
        else {
            std::wcout << propertyName << L" not available." << std::endl;
        }

        VariantClear(&vtProp);
        pclsObj->Release();
    }

    pEnumerator->Release();
}

int main() {
    initializeCOM();
    IWbemServices* pSvc = connectWMI();

    int startR = 173, startG = 216, startB = 230; // Light Blue (RGB: 173, 216, 230)
    int endR = 128, endG = 0, endB = 128;        // Purple (RGB: 128, 0, 128)

    std::wcout << L"\n";
    const std::wstring asciiArt =
        L"  _____           _       _   _____ _               _             \n"
        L" /  ___|         (_)     | | /  __ \\ |             | |            \n"
        L" \\ `--.  ___ _ __ _  __ _| | | /  \\/ |__   ___  ___| | _____ _ __ \n"
        L"  `--. \\/ _ \\ '__| |/ _` | | | |   | '_ \\ / _ \\/ __| |/ / _ \\ '__|\n"
        L" /\\__/ /  __/ |  | | (_| | | | \\__/\\ | | |  __/ (__|   <  __/ |   \n"
        L" \\____/ \\___|_|  |_|\\__,_|_|  \\____/_| |_|\\___|\\___|_|\\_\\___|_|   \n"
        L"                                                                  \n"
        L"                                                                  ";

    printWithGradient(asciiArt, startR, startG, startB, endR, endG, endB);

    std::wcout << L"\n";
    printWithGradient(L"--- Baseboard Serial ---", startR, startG, startB, endR, endG, endB);
    queryWMI(pSvc, L"SELECT SerialNumber FROM Win32_BaseBoard", L"SerialNumber");

    std::wcout << L"\n";
    printWithGradient(L"--- Disk Serial ---", startR, startG, startB, endR, endG, endB);
    queryWMI(pSvc, L"SELECT SerialNumber FROM Win32_DiskDrive", L"SerialNumber");

    std::wcout << L"\n";
    printWithGradient(L"--- System UUID ---", startR, startG, startB, endR, endG, endB);
    queryWMI(pSvc, L"SELECT UUID FROM Win32_ComputerSystemProduct", L"UUID");

    std::wcout << L"\n";
    pSvc->Release();
    CoUninitialize();

    system("pause");
    return 0;
}
