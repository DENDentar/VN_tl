
/*
SEHook: SEhook.cpp
Copyright (C) DENDentar.
*/

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <windows.h>

#include "mcrt.h"
#include "hooks.h"

#include "hookapi.h"
#include "hooktext.h"

HMODULE SLhookModule;
HMODULE SLModule;
DWORD SLVer;


// ---- DSOUND -----
LPVOID hDirectSoundCreate = NULL;
LPVOID hDirectSoundEnumerateA = NULL;
LPVOID hDirectSoundEnumerateW = NULL;
