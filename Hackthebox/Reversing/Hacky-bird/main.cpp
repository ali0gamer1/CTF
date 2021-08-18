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
                if (!_stricmp(test, modname))
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


void findHWNDprocid(DWORD dwProcessID, HWND & hwnd)
{
    // find all hWnds (vhWnds) associated with a process id (dwProcessID)
    HWND hCurWnd = NULL;
    do
    {
        hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
        DWORD dwProcID = 0;
        GetWindowThreadProcessId(hCurWnd, &dwProcID);
        if (dwProcID == dwProcessID)
        {
            hwnd = hCurWnd;
            return;
        }
    } while (hCurWnd != NULL);
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

    HWND hCurWnd = NULL;
    
    findHWNDprocid(procid, hCurWnd);


    bool velocity_hack = false;
    bool fly_hack = false;
    bool collision_hack = false;
    while (1)
    {

        if (GetAsyncKeyState(0x56) & 1) //V key (velocity)
        {
            velocity_hack = !velocity_hack;

            if (velocity_hack)
            {
               
                uintptr_t velocity_addr = findDMAaddr(hprocess, dynamicptr, {0x2c});
                static int new_vel  = 0;
                WriteProcessMemory(hprocess, (BYTE*)velocity_addr, &new_vel, sizeof(new_vel), 0);
                patch((BYTE*)(modulebase + 0x2fe0), (BYTE*)"\x90\x90\x90", 3, hprocess);
            }
            else
            {
                patch((BYTE*)(modulebase + 0x2fe0), (BYTE*)"\xff\x46\x2c", 3, hprocess);
            }
        }


        if (GetAsyncKeyState(0x43) & 1) //C key (collision)
        {
            collision_hack = !collision_hack;

            if (collision_hack)
            {
                patch((BYTE*)(modulebase + 0x30BF), (BYTE*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x31\xc0\x90\x90\x90\x90", 61, hprocess);
            }
            else
            {
                patch((BYTE*)(modulebase + 0x30BF), (BYTE*)"\x8d\x45\xcc\x50\x8d\x45\xdc\x89\x55\xcc\x50\x8d\x45\xac\xc7\x45\xd0\x00\x00\x00\x00\x50\x89\x55\xbc\x0f\x11\x45\xac\xff\x15\x80\x21\x41\x00\x85\xc0\x0f\x85\xcd\x00\x00\x00\x8d\x45\xbc\x50\x8d\x45\xdc\x50\x8d\x45\xac\x50\xff\x15\x80\x21\x41\x00", 61, hprocess);
            }
        }

        if (GetAsyncKeyState(VK_F1) & 1)
        {
            static int scooreee = 999;
            static uintptr_t scoreaddr = findDMAaddr(hprocess, dynamicptr, { 0x94 });
            WriteProcessMemory(hprocess, (BYTE*)scoreaddr, &scooreee, sizeof(scooreee), 0);
        }

        if (GetAsyncKeyState(VK_CONTROL) & 1) //fly hack (control with mouse)
            fly_hack = !fly_hack;
        

        if (fly_hack)
        {
            static int birdx, birdy;

            static uintptr_t xpos = findDMAaddr(hprocess, dynamicptr, { 0x18 });
            static uintptr_t ypos = findDMAaddr(hprocess, dynamicptr, { 0x1c });

            if (hCurWnd != NULL)
            {
                POINT p;
                GetCursorPos(&p);
                ScreenToClient(hCurWnd, &p);
                p.y = p.y - 285;
                p.x = p.x - 105;
                WriteProcessMemory(hprocess, (BYTE*)xpos, &p.x, sizeof(p.x), 0);
                WriteProcessMemory(hprocess, (BYTE*)ypos, &p.y, sizeof(p.y), 0);
            }
            

        }
   

        Sleep(10);
    }

    std::cin.get();
    return 0;
}
