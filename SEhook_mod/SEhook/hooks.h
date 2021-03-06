// API hooks

#include "mcrt.h"
#include "apihijack.h"

#define HOOK_CALL_STYLE __stdcall
#define DECLARE_HOOK(fun, ret, vars) typedef ret (HOOK_CALL_STYLE *fun##_t) vars; ret HOOK_CALL_STYLE fun##_hook vars

#ifdef APICALLLOG
#define APICALGLOBLOGWRITE(arg) if (true) \
	{ char *ApiLogStr; \
		ApiLogStr = new wchar_t[512]; \
		wsprintfW arg; \
		ApiCallLogWrite(L"ApiCallLog\\HooksLog.txt", ApiLogStr); \
		delete[] ApiLogStr; \
	} else (void)0
#endif

