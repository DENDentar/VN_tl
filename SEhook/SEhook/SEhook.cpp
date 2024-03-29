
/*
SEHook: SEhook.cpp
Copyright (C) DENDentar.
*/

#include "SEhook.h"


//#pragma comment(linker,"/SECTION:.rdata,RW")
//#pragma comment(linker,"/MERGE:.data=.rdata")
//#pragma comment(linker,"/MERGE:.reloc=.rdata")


bool CheckWinVer(DWORD MajorVersion, DWORD MinorVersion, DWORD ServicePackMajor)
{
	//bool OSVerErr = true;

	OSVERSIONINFOEXW osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXW));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	if (GetVersionExW((OSVERSIONINFOW*) &osvi) == FALSE) return false;

	if (osvi.dwPlatformId < VER_PLATFORM_WIN32_NT) return false; //OSVerErr = false;
	if (osvi.dwMajorVersion < MajorVersion) return false;
	if (osvi.dwMajorVersion == MajorVersion && osvi.dwMinorVersion < MinorVersion) return false; //OSVerErr = false;
	if (osvi.dwMajorVersion == MajorVersion && osvi.dwMinorVersion == MinorVersion)
	{
		if (osvi.wServicePackMajor < ServicePackMajor) return false; //OSVerErr = false;
	}
	//return OSVerErr;
	return true;
}

DWORD RlVersion(HMODULE mod)
{
    // Get version number of host RealLive instance.
    //HMODULE mod = GetModuleHandleW(NULL);
    LPVOID lres = LockResource(LoadResource(mod,
					    FindResourceW(mod, (LPCWSTR) 1,
							 /*MAKEINTRESOURCEW(16))));*/RT_VERSION)));
	if (lres == NULL) return 0;
    // With any luck, this offset is constant...
    VS_FIXEDFILEINFO *info = (VS_FIXEDFILEINFO*) ((char*) lres + 0x28);
    return (((info->dwFileVersionMS >> 16) & 0xff) << 24)
       	 | ((info->dwFileVersionMS & 0xff) << 16)
       	 | (((info->dwFileVersionLS >> 16) & 0xff) << 8)
       	 | (info->dwFileVersionLS & 0xff);
}

bool DllLoaded = false;
DWORD DllLoadCount = 0;

void LoadDll(void)
{
	if (DllLoaded) return;

	if (!CheckWinVer(5, 1, 3))
	{
		MessageBoxW(NULL, L"This program support only Windows XP SP3 or higher.", L"SLhook error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	
	initmcrt();

	SLModule = GetModuleHandleW(NULL);

	SLVer = RlVersion(SLModule);

	if (!(HookKernel32() && HookGDI32() && HookVersion() && HookUser32()))
	{
		MessageBoxW(NULL, L"Fatal Error", L"SLhook error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

//	HookVersion();
//	HookUser32();

	#if (SLVER >= 0x01000400 && SLVER <= 0x0100070A)
	HookDbghelp();
	#endif

	if (SLVer == SLVER) HookText();
	

	wchar_t *SysDirPath = new wchar_t[MAX_PATH];
	GetSystemDirectoryW(SysDirPath, MAX_PATH);

	wchar_t *DSOUND_Path = new wchar_t[MAX_PATH + 12];
	wsprintfW(DSOUND_Path, L"%s\\DSOUND.dll", SysDirPath);
	HMODULE hDSOUND = LoadLibraryW(DSOUND_Path);
	/*
	if (hDSOUND == NULL)
	{
		MessageBoxW(NULL, L"Cannot load DSOUND.DLL", L"SLhook error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	*/

	hDirectSoundCreate = GetProcAddress(hDSOUND, (LPCSTR) 1);
	hDirectSoundEnumerateA = GetProcAddress(hDSOUND, (LPCSTR) 2);
	hDirectSoundEnumerateW = GetProcAddress(hDSOUND, (LPCSTR) 3);

	if (hDirectSoundCreate == NULL || hDirectSoundEnumerateA == NULL || hDirectSoundEnumerateW == NULL)
	{
		MessageBoxW(NULL, L"Cannot load DSOUND.DLL", L"SLhook error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	delete[] SysDirPath;
	delete[] DSOUND_Path;

	DllLoaded = true;
	DllLoadCount++;
}

void UnLodaDll(void)
{
	DllLoadCount--;
	
	if (DllLoadCount == 0) deinitmcrt();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SLhookModule = hModule;
		LoadDll();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnLodaDll();
		break;
	}

	return TRUE;
}


// --- DSOUND ---------------
// Прокси для подключения вместо DSOUND.dll

extern "C" void __declspec(naked) DirectSoundCreate(void)
{
	__asm jmp hDirectSoundCreate;
}

extern "C" void __declspec(naked) DirectSoundEnumerateA(void)
{
	__asm jmp hDirectSoundEnumerateA;
}

extern "C" void __declspec(naked) DirectSoundEnumerateW(void)
{
	__asm jmp hDirectSoundEnumerateW;
}
