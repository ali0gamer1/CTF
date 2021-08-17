#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <comdef.h>

DWORD getprocID(const char* procname)
{
    DWORD procid = 0;

    HANDLE hsnap = (CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

    if (hsnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procentry;
        procentry.dwSize = sizeof(procentry);

        if (Process32First(hsnap, &procentry))
        {
            do
            {
                char test[260];
                sprintf_s(test, "%ws", procentry.szExeFile);
                if (!_stricmp(test, procname))
                {

                    procid = procentry.th32ProcessID;
                    break;
                }


            } while (Process32Next(hsnap, &procentry));


        }

    }

    CloseHandle(hsnap);
    return procid;
}

uintptr_t get_module_base_addr(DWORD procID, const char* modname)
{
    uintptr_t modbaseadr = 0;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE, procID);

    if (hsnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modentry;
        modentry.dwSize = sizeof(modentry);

        if (Module32First(hsnap, &modentry))
        {
            do
            {
                char test[260];
                sprintf_s(test, "%ws", modentry.szModule);
                if (!strcmp(test, modname))
                {
                    modbaseadr = (uintptr_t)modentry.modBaseAddr;
                    break;
                }


            } while (Module32Next(hsnap, &modentry));

        }

    }

    CloseHandle(hsnap);
    return modbaseadr;

}


uintptr_t findDMAaddr(HANDLE hproc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;

    for (unsigned int i = 0; i < offsets.size(); i++)
    {
        ReadProcessMemory(hproc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;



}


void patch(BYTE* dst, BYTE* src, unsigned int size, HANDLE hprocess)
{
    DWORD protect;
    VirtualProtectEx(hprocess, dst, size, PAGE_EXECUTE_READWRITE, &protect);
    WriteProcessMemory(hprocess, dst, src, size, nullptr);
    VirtualProtectEx(hprocess, dst, size, protect, &protect);
}


int main()
{
    DWORD procid = getprocID("HackyBird.exe");

    uintptr_t modulebase = get_module_base_addr(procid, "HackyBird.exe");

    if (modulebase == 0)
    {
        std::cerr << "Error: game not running" << '\n';
        return 1;
    }


    HANDLE hprocess = 0;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procid);

    uintptr_t dynamicptr = modulebase + 0x1a4b4;

    /*
    std::vector<unsigned int> healthvec;
    healthvec.push_back(0xf8);

    uintptr_t healthaddr = findDMAaddr(hprocess, dynamicptr, healthvec);

    ReadProcessMemory(hprocess, (BYTE*)healthaddr, &temphealth, sizeof(health), 0);
    ReadProcessMemory(hprocess, (BYTE*)ammoaddr, &tempammo, sizeof(ammo), 0);
    WriteProcessMemory(hprocess, (BYTE*)ammoaddr, &newammo, sizeof(newammo), 0);*/

    bool velocity_hack = false;
    while (1)
    {

        if (GetAsyncKeyState(VK_UP) & 1)
        {
            velocity_hack = !velocity_hack;

            if (velocity_hack)
            {
                patch((BYTE*)(modulebase + 0x2fe0), (BYTE*)"\x90\x90\x90", 3, hprocess);
            }
            else
            {
                patch((BYTE*)(modulebase + 0x2fe0), (BYTE*)"\xff\x46\x2c", 3, hprocess);
            }

            
        }

    }

    std::cin.get();
    return 0;
}
