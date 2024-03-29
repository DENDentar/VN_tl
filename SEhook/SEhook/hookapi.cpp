
/*
SEHook: hookapi.cpp
Copyright (C) DENDentar.
*/

#include <windows.h>

#include "hookapi.h"
#include "hooks.h"

// ---- Version.dll ----

// GetFileVersionInfoW
//typedef BOOL (WINAPI *GFVerIFUNC) (LPCWSTR, DWORD, DWORD, LPVOID);
//bool HasGFVerI;
// GetFileVersionInfoSizeW
//typedef DWORD (WINAPI *GFVerISFUNC) (LPCWSTR, LPDWORD);
//bool HasGFVerIS;
// VerQueryValueW
typedef BOOL (WINAPI *VerQVFUNC) (LPCVOID, LPCWSTR, LPVOID*, PUINT);
bool HasVerQV;


//DECLARE_HOOK(GetFileVersionInfoW,		BOOL,		(LPCWSTR, DWORD, DWORD, LPVOID));
//DECLARE_HOOK(GetFileVersionInfoSizeW,	DWORD,		(LPCWSTR, LPDWORD));
DECLARE_HOOK(VerQueryValueW,			BOOL,		(LPCVOID, LPCWSTR, LPVOID*, PUINT));


enum {
//	GETFILEVERSIONINFOW,
//	GETFILEVERSIONINFOSIZEW,
	VERQUERYVALUEW
};

SDLLHook VersionHooks = {
    "VERSION.DLL",
    false, NULL,
    {
//		{ "GetFileVersionInfoW",		(void*) GetFileVersionInfoW_hook		},
//		{ "GetFileVersionInfoSizeW",	(void*) GetFileVersionInfoSizeW_hook	},
		{ "VerQueryValueW",				(void*) VerQueryValueW_hook				},
		{ NULL, NULL }
    }
};

bool __stdcall
HookVersion(void)
{
    bool state = HookAPICalls(&VersionHooks);
    //HMODULE verHandle = GetModuleHandleW(L"VERSION");
//	HasGFVerI = verHandle && GetProcAddress(verHandle, "GetFileVersionInfoW");
//	HasGFVerIS = verHandle && GetProcAddress(verHandle, "GetFileVersionInfoSizeW");
	//HasVerQV = verHandle && GetProcAddress(verHandle, "VerQueryValueW");

	#ifdef APICALLLOG
	APICALLOGCLEAR(L"ApiCallLog\\VersionCallLog.txt");
	#endif

	return state;
}
/*
BOOL HOOK_CALL_STYLE 
GetFileVersionInfoW_hook(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{ // Получает информацию о версии KERNEL32.dll
    GetFileVersionInfoW_t OldFn =
	(GetFileVersionInfoW_t) VersionHooks.Functions[GETFILEVERSIONINFOW].OrigFn;
	#ifdef APICALLLOG
	APICALLOGWRITEVER((ApiLogStrVer, "CALL GetFileVersionInfo. lptstrFilename: \"%s\"\r\n", lptstrFilename));
	#endif
    if (HasGFVerI) {
		HMODULE verHandle = GetModuleHandleW(L"VERSION");
		GFVerIFUNC f = (GFVerIFUNC) GetProcAddress(verHandle, "GetFileVersionInfoW");
		BOOL rv = f(lptstrFilename, dwHandle, dwLen, lpData);
	    return rv;
    }
    return OldFn(lptstrFilename, dwHandle, dwLen, lpData);
}
*/

/*
DWORD HOOK_CALL_STYLE 
GetFileVersionInfoSizeW_hook(LPCSTR lptstrFilename, LPDWORD lpdwHandle)
{ // Получает размер информации о версии KERNEL32.dll
    GetFileVersionInfoSizeW_t OldFn =
	(GetFileVersionInfoSizeW_t) VersionHooks.Functions[GETFILEVERSIONINFOSIZEW].OrigFn;
	#ifdef APICALLLOG
	APICALLOGWRITEVER((ApiLogStrVer, "CALL GetFileVersionInfoSize. lptstrFilename: \"%s\"\r\n", lptstrFilename));
	#endif
    if (HasGFVerIS) {
		HMODULE verHandle = GetModuleHandleW(L"VERSION");
		GFVerISFUNC f = (GFVerISFUNC) GetProcAddress(verHandle, "GetFileVersionInfoSizeW");
		DWORD rv = f(lptstrFilename, lpdwHandle);
	    return rv;
    }
    return OldFn(lptstrFilename, lpdwHandle);
}
*/

BOOL HOOK_CALL_STYLE 
VerQueryValueW_hook(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen)
{ // Извлекает локаль KERNEL32.dll
    VerQueryValueW_t OldFn =
	(VerQueryValueW_t) VersionHooks.Functions[VERQUERYVALUEW].OrigFn;
	#ifdef APICALLLOG
	APICALLOGWRITEVER((ApiLogStrVer, L"CALL VerQueryValue. lpSubBlock: \"%s\"\r\n", lpSubBlock));
	#endif
	// Не думаю что здесь нужна Юникодная функция)
    /*if (HasGFVerI) {
		//wchar_t *ws;
		//ws = new wchar_t[strlen(lptstrFilename)+1];
		JisToUniString(lptstrFilename, ws, strlen(lptstrFilename));
		HMODULE verHandle = GetModuleHandleW(L"VERSION");
		GFVerIFUNC f = (GFVerIFUNC) GetProcAddress(verHandle, "VerQueryValueA");
		BOOL rv = f(pBlock, lpSubBlock, *lplpBuffer, puLen);
		//delete[] ws;
	    return rv;
    }*/
	BOOL rv = OldFn(pBlock, lpSubBlock, lplpBuffer, puLen);
	if (lstrcmpiW(lpSubBlock, L"\\VarFileInfo\\Translation") == 0) {
		//wchar_t text[] = L"\x11\x04\xb0\x04";
		//*lplpBuffer = text;
		//strcpy((char*) *lplpBuffer, "\x11\x04\xb0\x04");
		DWORD *lpBuffer = (DWORD*) *lplpBuffer;
		*lpBuffer = 0x04b00411;
	}
    return rv;
}

// ---------------------

// ----- User32 -----

extern HMODULE SLhookModule;

// SetWindowTextW
typedef BOOL (WINAPI *SWTFUNC)(HWND, LPCWSTR);
SWTFUNC hSetWindowTextW = NULL;
DECLARE_HOOK(SetWindowTextW, BOOL, (HWND, LPCWSTR));

// CreateDialogParamW
typedef HWND (WINAPI *CDPFUNC) (HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
CDPFUNC hCreateDialogParamW = NULL;
DECLARE_HOOK(CreateDialogParamW, HWND, (HINSTANCE,LPCWSTR,HWND,DLGPROC,LPARAM));

// DialogBoxParamW
typedef INT_PTR (WINAPI *DBPFUNC) (HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
DBPFUNC hDialogBoxParamW = NULL;
DECLARE_HOOK(DialogBoxParamW, INT_PTR, (HINSTANCE,LPCWSTR,HWND,DLGPROC,LPARAM));

enum {
	SETWINDOWTEXTW,
    CREATEDIALOGPARAMW,
    DIALOGBOXPARAMW
};
	
SDLLHook UserHooks = {
    "USER32.DLL",
    false, NULL,
    {
		{ "SetWindowTextW",			(void*) SetWindowTextW_hook				},
		{ "CreateDialogParamW",		(void*) CreateDialogParamW_hook			},
		{ "DialogBoxParamW",		(void*) DialogBoxParamW_hook			},
        { NULL, NULL }
    }
};


bool __stdcall
HookUser32(void)
{
    HMODULE usrHandle = GetModuleHandleW(L"USER32");
	if (usrHandle == NULL) return false;

	hSetWindowTextW = (SWTFUNC) GetProcAddress(usrHandle, "SetWindowTextW");
	hCreateDialogParamW = (CDPFUNC) GetProcAddress(usrHandle, "CreateDialogParamW");
	hDialogBoxParamW = (DBPFUNC) GetProcAddress(usrHandle, "DialogBoxParamW");

	if (hSetWindowTextW == NULL || hCreateDialogParamW == NULL || hDialogBoxParamW == NULL) return false;

	bool state = HookAPICalls(&UserHooks);

	#ifdef APICALLLOG
	APICALLOGCLEAR(L"ApiCallLog\\User32CallLog.txt");
	#endif

	return state;
}

BOOL HOOK_CALL_STYLE 
SetWindowTextW_hook(HWND hWnd, LPCWSTR lpString)
{
	//size_t wslen = strlenW(lpString)+1;
	wchar_t *ws = new wchar_t[1024]; ws[0] = NULL;
	//JisToUniString(lpString, ws, wslen);
	#if (SLVER == 0x01015007)
	if (strcmpW(lpString, L"「あいう漢字カナ薔薇」") == 0) strcpyW(ws, L"Фронт загробного мира.");
	#elif (SLVER == 0x0100070A)
	if (strcmpW(lpString, L"「あいう漢字カナ薔薇」") == 0) strcpyW(ws, L"У меня есть сверхспособности!");
	#endif
	else strcpyW(ws, lpString);
    BOOL rv = hSetWindowTextW(hWnd, ws);
    delete[] ws;
	#ifdef APICALLLOG
	APICALLOGWRITEUSR32((ApiLogStr, "CALL SetWindowText. hWnd: %#08x, lpString: \"%s\"\r\n", hWnd, lpString));
	#endif
	return rv;
}

HWND HOOK_CALL_STYLE
CreateDialogParamW_hook(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,
						DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	#ifdef APICALLLOG
	APICALLOGWRITEUSR32((ApiLogStr, "CALL CreateDialogParam. hInstance: %#08x, lpTemplateName: %#08x, hWndParent: %#08x, lpDialogFunc: %#08x, dwInitParam: %#08x\r\n", hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam));
	#endif
	HWND rv;
    if (FindResourceW(SLhookModule, lpTemplateName, MAKEINTRESOURCEW(5))) hInstance = SLhookModule;
	rv = hCreateDialogParamW(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
	//DWORD err = GetLastError();
	//APICALLOGWRITEUSR32((ApiLogStr, "CALL CreateDialogParam. HWND: %#08x, err: %#08x\r\n", rv, err));
	return rv;
}

INT_PTR HOOK_CALL_STYLE
DialogBoxParamW_hook(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,
						DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	#ifdef APICALLLOG
	APICALLOGWRITEUSR32((ApiLogStr, "CALL DialogBoxParam.\r\n"));
	#endif
    if (FindResourceW(SLhookModule, lpTemplateName, MAKEINTRESOURCEW(5))) hInstance = SLhookModule;
	return hDialogBoxParamW(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}


// ---------------------

// ---- Kernel32 ----

// GetACP
//typedef UINT (WINAPI *GACPFUNC) (void);
//GACPFUNC hGetACP = NULL;
DECLARE_HOOK(GetACP, UINT, (void));

// GetLocaleInfoW
typedef int (WINAPI *GLOCIFUNC) (LCID, LCTYPE, LPWSTR, int);
GLOCIFUNC hGetLocaleInfoW = NULL;
DECLARE_HOOK(GetLocaleInfoW, int, (LCID, LCTYPE, LPWSTR, int));

// GetTimeZoneInformation
typedef DWORD (WINAPI *GTZINFFUNC) (PTIME_ZONE_INFORMATION);
GTZINFFUNC hGetTimeZoneInformation = NULL;
DECLARE_HOOK(GetTimeZoneInformation, DWORD, (PTIME_ZONE_INFORMATION));

// MultiByteToWideChar
typedef int (WINAPI *MultiByteToWideChar_func) (UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
MultiByteToWideChar_func hMultiByteToWideChar = NULL;
DECLARE_HOOK(MultiByteToWideChar, int, (UINT, DWORD, LPCSTR, int, LPWSTR, int));

// WideCharToMultiByte
typedef int (WINAPI *WideCharToMultiByte_func) (UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);
WideCharToMultiByte_func hWideCharToMultiByte = NULL;
DECLARE_HOOK(WideCharToMultiByte, int, (UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL));


enum {
	GETACP,
	GETLOCALEINFOW,
	GETTIMEZONEINFORMATION,

	MULTIBYTETOWIDECHAR,
	WIDECHARTOMULTIBYTE
};

SDLLHook KernelHooks = {
    "KERNEL32.DLL",
    false, NULL,
    {
		{ "GetACP",					(void*) GetACP_hook					},
		{ "GetLocaleInfoW",			(void*) GetLocaleInfoW_hook			},
		{ "GetTimeZoneInformation",	(void*) GetTimeZoneInformation_hook	},

		{ "MultiByteToWideChar",	(void*) MultiByteToWideChar_hook	},
		{ "WideCharToMultiByte",	(void*) WideCharToMultiByte_hook	},

        { NULL, NULL }
    }
};

bool __stdcall
HookKernel32(void)
{
	HMODULE kerHandle = GetModuleHandleW(L"KERNEL32");
	if (kerHandle == NULL) return false;


	//hGetACP = (GACPFUNC) GetProcAddress(kerHandle, "GetACP");
	hGetLocaleInfoW = (GLOCIFUNC) GetProcAddress(kerHandle, "GetLocaleInfoW");
	hGetTimeZoneInformation = (GTZINFFUNC) GetProcAddress(kerHandle, "GetTimeZoneInformation");

	hMultiByteToWideChar = (MultiByteToWideChar_func) GetProcAddress(kerHandle, "MultiByteToWideChar");
	hWideCharToMultiByte = (WideCharToMultiByte_func) GetProcAddress(kerHandle, "WideCharToMultiByte");

    bool state = HookAPICalls(&KernelHooks);

	#ifdef APICALLLOG
	APICALLOGCLEAR(L"ApiCallLog\\Kernel32CallLog.txt");
	#endif

	return state;
}

UINT HOOK_CALL_STYLE
GetACP_hook(void)
{
	return 932;
}


int HOOK_CALL_STYLE 
GetLocaleInfoW_hook(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData)
{ // Проверка системной локали в 1.6.x.x. Проверено на RealLiveMax (1.6.5.6).
	#ifdef APICALLLOG
	APICALLOGWRITEKER32((ApiLogStr, L"CALL GetLocaleInfo. Locale: %#08x\r\n", Locale));
	#endif
	int rv;
	rv = hGetLocaleInfoW(Locale, LCType, lpLCData, cchData);
	/*if (Locale == LOCALE_SYSTEM_DEFAULT && (LCType == LOCALE_ILANGUAGE || LCType == LOCALE_SLANGUAGE)) {
		if (LCType == LOCALE_ILANGUAGE) ws = L"0411"; // Good work!
		else if (LCType == LOCALE_SLANGUAGE) ws = L"日本語 (日本国)"; // ???
	}*/
	if (Locale == LOCALE_SYSTEM_DEFAULT && LCType == LOCALE_ILANGUAGE && cchData != 0) {
		//wcscpy(lpLCData, L"0411");
		strcpyW(lpLCData, L"0411");
	}
    return rv;
}


DWORD HOOK_CALL_STYLE
GetTimeZoneInformation_hook(PTIME_ZONE_INFORMATION lpTimeZoneInformation)
{ // Проверка часового пояса в 1.6.x.x. Проверено на RealLiveMax (1.6.5.6).
	DWORD rv = hGetTimeZoneInformation(lpTimeZoneInformation);
	#ifdef APICALLLOG
	UniToJisString(tzi->StandardName, text, 255);
	APICALLOGWRITEKER32((ApiLogStr, "CALL GetTimeZoneInformation. StandardName: \"%s\"\r\n", text));
	#endif
	//wcscpy(tzi->StandardName, L"TOKYO");
	//wcscpy(tzi->DaylightName, L"TOKYO");
	strcpyW(lpTimeZoneInformation->StandardName, L"TOKYO");
	strcpyW(lpTimeZoneInformation->DaylightName, L"TOKYO");
	return rv;
}

int HOOK_CALL_STYLE
MultiByteToWideChar_hook(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
	/*wchar_t text[1024];
	wsprintfW(text, L"MultiByteToWideChar CP %d\n%s", CodePage, lpWideCharStr);
	MessageBoxW(NULL, text, L"Info", MB_OK);*/
	if (CodePage == CP_ACP || CodePage == CP_OEMCP || CodePage == CP_THREAD_ACP) CodePage = 932;
	int rv = hMultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	return rv;
}


int HOOK_CALL_STYLE
WideCharToMultiByte_hook(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	/*wchar_t text[1024];
	wsprintfW(text, L"WideCharToMultiByte CP %d\n%s", CodePage, lpWideCharStr);
	MessageBoxW(NULL, text, L"Info", MB_OK);*/
	if (CodePage == CP_ACP || CodePage == CP_OEMCP || CodePage == CP_THREAD_ACP) CodePage = 932;
	int rv = hWideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	return rv;
}

// --------------------------

// --------- GDI32 ----------

// EnumFontFamiliesW
typedef int (WINAPI *EnumFontFamiliesExW_func)(HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD);
EnumFontFamiliesExW_func hEnumFontFamiliesExW = NULL;
DECLARE_HOOK(EnumFontFamiliesW, int, (HDC, LPCWSTR, FONTENUMPROCW, LPARAM));

enum { 
//	GETGLYPHOUTLINEW, 
	ENUMFONTFAMILIESW//, 
//	CREATEFONTW 
};

SDLLHook GdiHooks = {
    "GDI32.DLL",
    false, NULL,
    {
//		{ "GetGlyphOutlineW",  (void*) GetGlyphOutlineW_hook },
        { "EnumFontFamiliesW", (void*) EnumFontFamiliesW_hook },
//		{ "CreateFontW",       (void*) CreateFontW_hook },
        { NULL, NULL }
    }
};

bool __stdcall
HookGDI32(void)
{
	hEnumFontFamiliesExW = (EnumFontFamiliesExW_func) GetProcAddress(GetModuleHandleW(L"GDI32"), "EnumFontFamiliesExW");
	
	bool state = HookAPICalls(&GdiHooks);

	return state;
}

FONTENUMPROCW EnumFontFamProc;
int GotFonts;

int CALLBACK FakeEnumFontFamProc(ENUMLOGFONTEXW *lpelfe, NEWTEXTMETRICEXW *lpntme, DWORD FontType, LPARAM lParam)
{
	// Exclude vertical variants.
	if (lpelfe->elfLogFont.lfFaceName[0] == L'@') return 1;
	// Record that a font was found.	
	++GotFonts;
	// Tell RealLive it's a Japanese font. No, really. Honest.
	ENUMLOGFONTEXW fakelf = *lpelfe;
	fakelf.elfLogFont.lfCharSet = SHIFTJIS_CHARSET;
	// And pass control over to RealLive's own callback function.
	return EnumFontFamProc((LPLOGFONTW) &fakelf, (LPTEXTMETRICW) lpntme, FontType, lParam);
}

int HOOK_CALL_STYLE 
EnumFontFamiliesW_hook(HDC hdc, LPCWSTR lpszFamily, FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam)
{
	EnumFontFamProc = lpEnumFontFamProc;
	GotFonts = 0;
	LOGFONTW lf;
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfFaceName[0] = 0;
	lf.lfPitchAndFamily = 0;
	DWORD retval = hEnumFontFamiliesExW(hdc, &lf, (FONTENUMPROCW) FakeEnumFontFamProc, lParam, 0);
	if (!GotFonts) {
		MessageBoxW(NULL, L"Fatal error: unable to find a suitable font.",
							L"Siglus error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	return retval;
}

// --------------------------

// ------ Dbghelp.dll -------
#if (SLVER >= 0x01000400 && SLVER <= 0x0100070A)

#include <shlobj.h>

// MakeSureDirectoryPathExists
//typedef int (WINAPI *EnumFontFamiliesExW_func)(HDC, LPLOGFONTW, FONTENUMPROCW, LPARAM, DWORD);
//EnumFontFamiliesExW_func hEnumFontFamiliesExW = NULL;
DECLARE_HOOK(MakeSureDirectoryPathExists, BOOL, (PCSTR));

//enum { 
//	GETGLYPHOUTLINEW, 
//	ENUMFONTFAMILIESW//, 
//	CREATEFONTW 
//};

SDLLHook DbhHooks = {
    "IMAGEHLP.DLL",
    false, NULL,
    {
//		{ "GetGlyphOutlineW",  (void*) GetGlyphOutlineW_hook },
        { "MakeSureDirectoryPathExists", (void*) MakeSureDirectoryPathExists_hook },
//		{ "CreateFontW",       (void*) CreateFontW_hook },
        { NULL, NULL }
    }
};

bool __stdcall
HookDbghelp(void)
{
	//hEnumFontFamiliesExW = (EnumFontFamiliesExW_func) GetProcAddress(GetModuleHandleW(L"GDI32"), "EnumFontFamiliesExW");
	//if (hMultiByteToWideChar == NULL) hMultiByteToWideChar = (MultiByteToWideChar_func) GetProcAddress(GetModuleHandleW(L"KERNEL32"), "MultiByteToWideChar");
	//SHCreateDirectoryEx
	bool state = HookAPICalls(&DbhHooks);

	return state;
}

BOOL HOOK_CALL_STYLE
MakeSureDirectoryPathExists_hook(PCSTR DirPath)
{
	int slenA = strlenA(DirPath);
	char * DirPathA = new char[slenA + 1];
	wchar_t *DirPathW = new wchar_t[slenA + 1];
	strcpyA(DirPathA, DirPath);
	DirPathW[0] = NULL;

	slenA--;
	if (DirPathA[slenA] != '\\' && DirPathA[slenA] != '/')
	{
		while (slenA >= 0 && DirPathA[slenA] != '\\' && DirPathA[slenA] != '/') DirPathA[slenA--] = '\0';
	}
	slenA++;

	MultiByteToWideChar(932, NULL, DirPathA, -1, DirPathW, slenA + 1);

	int rv = SHCreateDirectoryExW(NULL, DirPathW, NULL);

	delete[] DirPathA;
	delete[] DirPathW;

	if (rv == ERROR_SUCCESS || rv == ERROR_ALREADY_EXISTS) return TRUE;
	return FALSE;
}
#endif
// --------------------------