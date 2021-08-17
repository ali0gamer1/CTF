#include <windows.h>
#include <limits.h>
#include <iostream>


struct
{
    uintptr_t *left;
    uintptr_t *top;
    uintptr_t *right;
    uintptr_t *bottom;


}bird;

DWORD WINAPI mainthread(HMODULE hm)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    if (f == 0)
    {
        FreeLibraryAndExitThread(hm, 0);
    }
    std::cout << "COCONUT\n";

    unsigned int t = 0;
    bool flag = false;

    int step = 230;
    while (1)
    {   
        uintptr_t classptr = *(uintptr_t*)(0x41a4b4);


        Sleep(10);

        if (GetAsyncKeyState(VK_UP) & 1)
        {
            step += 7;
        }

        if (GetAsyncKeyState(VK_DOWN) & 1)
        {
            step -= 7;
        }


        *(unsigned int*)(classptr + 0x1c) = step; //pos
        /*
        std::cout << "--------player-------\n";
        std::cout << *(unsigned int*)(classptr + 0x1c) << '\n';
        std::cout << "---------------------\n";

        std::cout << "---------------------\n";
        std::cout<<*(unsigned int *)(classptr + 0x38) - 0x3c<<'\n';
        std::cout << "---------------------\n";


        std::cout << "---------------------\n";
        std::cout<<*(unsigned int *)(classptr + 0x38) + 0x3c<<'\n';
        std::cout << "---------------------\n";*/

       

    }
    
    fclose(f);
    FreeLibraryAndExitThread(hm, 0);
    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainthread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

