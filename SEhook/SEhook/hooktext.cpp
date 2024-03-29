
/*
SEHook: hooktext.cpp
Copyright (C) DENDentar.
*/

#include "hooktext.h"
#include "hooktextadr.h"

extern HMODULE SLModule;

/*UINT uChar = NULL;
INT uCharWidth = 0;
INT uCharWidthP = 0;
INT MaxStrWidth = 0;
INT CurStrX = 0;*/
//wchar_t *CurTextStr = (wchar_t*) NULL;
//unsigned long long EPCode_orig;
//unsigned long long EPCode_new;


/*
void __declspec(naked) HookEP(void)
{
	__asm	pushad
	HookApi();
	//memcpy(EPAdr, &EPCode_orig, 8);
	*(unsigned long long*) EPAdr = EPCode_orig;
	__asm
	{
		popad;
		jmp EPAdr;
	}

}
*/

#if (SLVER >= 0x01015000 && SLVER <= 0x01015007)
typedef BOOL (WINAPI *hSLCreateFont) (DWORD, INT);
#elif (SLVER >= 0x01000400 && SLVER <= 0x0100070A)
typedef BOOL (WINAPI *hSLCreateFont) (DWORD, INT);
#endif
hSLCreateFont SLCreateFont_func = NULL;


extern "C" UINT __stdcall GetCharWidthSL(UINT ch, INT fSize)
{
    INT width = 0;
	HDC *hdc = *(HDC**) HdcAdr;
	#if (SLVER >= 0x01015000 && SLVER <= 0x01015007)
	DWORD FontName = (*(DWORD*) FontHameAdr) + 0x110;
	SLCreateFont_func(FontName, fSize);
	#elif (SLVER >= 0x01000400 && SLVER <= 0x0100070A)
	DWORD FontName = (*(DWORD*) FontHameAdr) + 0x84;
	SLCreateFont_func(FontName, fSize);
	#endif
	//MessageBoxW(NULL, FontName, L"test", MB_OK);
	TEXTMETRICW TextMetr;
	if (GetTextMetricsW(*hdc, &TextMetr) == NULL) return 0;
	//wchar_t text[256];
	//wsprintfW(text, L"%d", TextMetr.tmHeight);
	//MessageBoxW(NULL, text, L"log", MB_OK);
	if (TextMetr.tmHeight > fSize) fSize = TextMetr.tmHeight;
    if (ch >= 0x20)
	{
	#ifdef USEGETCHARWIDTH
	
	if (TextMetr.tmPitchAndFamily & TMPF_TRUETYPE)
	{
		ABC lpabc;
		GetCharABCWidthsW(*hdc, ch, ch, &lpabc);
		width = lpabc.abcA + lpabc.abcB + lpabc.abcC;
	}
	else
	{
		GetCharWidth32W(*hdc, ch, ch, &width);
	}

	#else

	USHORT Glyph;
	WORD str[] = { ch, 0 };
	if (GetGlyphIndicesW(*hdc, (LPCWSTR) str, 1, &Glyph, 0)  == GDI_ERROR)
		return 0;
	USHORT hmtx_len, cmetrics[2];
	SHORT adl[3];
	if (GetFontData(*hdc, dwHheaName, 34, &hmtx_len, 2) == GDI_ERROR)
	    return 0;
	hmtx_len = SWAPWORD(hmtx_len);
	if (GetFontData(*hdc, dwHmtxName,
			(Glyph < hmtx_len ? Glyph : hmtx_len - 1) * 4,
			cmetrics, 4) != GDI_ERROR
	    && GetFontData(*hdc, dwHheaName, 4, &adl, 6) != GDI_ERROR) 
	{
		//width = ceil((double) SWAPWORD(cmetrics[0]) * fSize / (USHORT)(SWAPWORD(adl[0]) - SWAPWORD(adl[1]) + SWAPWORD(adl[2])));
		width = SWAPWORD(cmetrics[0]) * fSize / (USHORT)(SWAPWORD(adl[0]) - SWAPWORD(adl[1]) + SWAPWORD(adl[2]));
	}

	#endif
    }
	else
	{
		width = 0;
    }
	//width += 1;
    return width;
}

extern "C" UINT __stdcall WordWrapTWSL(wchar_t *TextStr, INT MojiSize, INT MojiSpace, INT CurX, INT MaxX, UINT TextChar)
{
	if (TextStr == NULL || *TextStr == NULL) return 0;
	if (TextChar == 0x0A || TextChar == 0x0D) return 1;
	/*if (TextChar == 0x0A)
	{
		*(TextStr - 1) = 0x20;
		return 1;
	}*/
	/*if (*(TextStr) == 0x0A)
	{
		*(TextStr) = 0x20;
		return 1;
	}*/
	if (TextChar != 0x20) return 0;

	USHORT uCharTmp;
	//INT StrWidth = CurStrX + uCharWidthP;
	INT StrWidth = CurX + MojiSize + MojiSpace;
	while (*TextStr != 0x0 && *TextStr != 0x20)
	{
		uCharTmp = *TextStr;
		StrWidth += GetCharWidthSL(uCharTmp, MojiSize) + MojiSpace;
		TextStr++;
	}
	if (StrWidth > MaxX) return 1; //return MaxStrWidth;
	//return uCharWidthP;
	return 0;
}


extern "C" INT __stdcall GetStrWidthInPix(wchar_t *TextStr, INT MojiSize, INT MojiSpace)
{
	if (TextStr == NULL || *TextStr == NULL) return 0;

	USHORT uCharTmp;
	INT StrWidth = 0;
	while (*TextStr != 0x0)
	{
		uCharTmp = *TextStr;
		StrWidth += GetCharWidthSL(uCharTmp, MojiSize) + MojiSpace;
		TextStr++;
	}

	return StrWidth;
}

extern "C" UINT __stdcall GetStrWidthInChar(wchar_t *TextStr, INT MojiSize, INT MojiSpace)
{
	if (TextStr == NULL || *TextStr == NULL) return 0;

	/*INT MojiSpace = (MojiSize >> 16) & 0x00FF;
	MojiSize = MojiSize >> 24;

	MojiSize = 0x1c;
	MojiSpace = 0;*/

	INT StrWidth = GetStrWidthInPix(TextStr, MojiSize, MojiSpace);

	return StrWidth / ((MojiSize + MojiSpace) / 2);
}

extern "C" LPCWSTR __stdcall CheckConstStr(LPCWSTR TextStr)
{
	DWORD ModuleAdr = (DWORD) SLModule;

	for (int i = 0; user_str[i].stradr != NULL; i++)
	{
		if ((DWORD) TextStr == user_str[i].stradr + ModuleAdr) TextStr = user_str[i].str;
	}
	
	return TextStr;
}

#if (SLVER >= 0x01015000 && SLVER <= 0x01015007)

void __declspec(naked) GetCharWidthTWSL(void)
{
	__asm
	{
		sub ESP, 4;
		mov [ESP], ECX;
		pushad;
		MOV EAX, DWORD PTR [EBP-0x400];
		test eax, eax;
		jnz m1;
		push [EBX+0x6C]; // MojiSize
		push [EBP-0x418]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		
		push [EBP-0x418]; // Char
		push [EBX+0x0A8]; // MaxX
		push [EBX+0x5C]; // CurX
		push [EBX+0x70]; // MojiSpace
		push [EBX+0x6C]; // MojiSize
		push [EBP-0x408]; // Str
		call WordWrapTWSL;
		test AL, AL;
		jz m1;
		mov EAX, [EBX+0x0A8];
		add EAX, [EBX+0x6C];
		add EAX, [EBX+0x70];
		mov [EBX+0x5C], EAX;
		m1:
		popad;
		mov ECX, [ESP];
		//m2:
		mov DWORD PTR [EBP-0x414], ECX;
		add ESP, 4;
		ret;
	}
}

// not sapport vertical text out
void __declspec(naked) GetCharWidthObjSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP-0x58]; // MojiSize
		push [EBP-0x64]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;

		MOV EBX, DWORD PTR [EBP-0x44];
		cmp DWORD PTR [EBX+0x15C], 0;
		jle m1;

		mov EAX, [EBP-0x58];
		add EAX, [EBX+0x154];
		imul EAX, [EBX+0x15C];

		push [EBP-0x64]; // Char
		push EAX; // MaxX
		push [EBP-0x5c]; // CurX
		push [EBX+0x154]; // MojiSpace
		push [EBP-0x58]; // MojiSize
		push [EBP-0x48]; // Str
		call WordWrapTWSL;
		test AL, AL;
		jz m1;
		MOV EAX, DWORD PTR [EBP-0x44];
		MOV EAX, DWORD PTR [EAX+0x158];
		MOV ECX, DWORD PTR [EBP-0x4C];
		XOR EBX, EBX;
		ADD EAX, DWORD PTR [EBP-0x58];
		ADD ECX, EAX;
		MOV DWORD PTR [EBP-0x5C], EBX;
		MOV DWORD PTR [EBP-0x4C], ECX;
		mov DWORD PTR [EBP-0x64], 0x20;
		jmp m2;
		m1:
		popad;
		MOV EAX, DWORD PTR [EBP-0x44];
		MOV EAX, DWORD PTR [EAX+0x154];
		add EAX, [ESP];
		add EBX, EAX;
		mov [EBP-0x5c], EBX;
		add ESP, 4;
		ret;
		m2:
		popad;
		MOV ECX, DWORD PTR [EBP-0x4C];
		MOV EBX, DWORD PTR [EBP-0x5C];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) GetCharWidthNameWinSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP-0x3D4]; // MojiSize
		push [EBP-0x3D8]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		mov EDX, [EDI+0x38];
		add EDX, [ESP];
		add [EDI+0x2C], EDX;
		LEA ECX, [EDI+0x88];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) GetCharWidthSelObjSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push EBX; // MojiSize
		push [EBP-0x3CC]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		mov EDX, [EBP+0x30];
		add EDX, [ESP];
		mov EAX, EDX;
		mov EDI, [EBP-0x3D0];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) GetCharWidthRubySL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP-0x7A4]; // MojiSize
		push [EBP-0x79C]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		add EAX, [ESP];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) SLSetIndent(void)
{
	__asm
	{
		CMP ESI, 0x0FF08;
		je m0;
		cmp ESI, 0x22;
		je m0;
		cmp ESI, 0x27;
		je m0;
		cmp ESI, 0x28;
		m0:
		ret;
	}
}

void __declspec(naked) SLCheckIndent(void)
{
	__asm
	{
		MOV EAX, 0x0FF08;
		CMP CX, AX;
		jne m1;
		CMP ESI, 0x0FF09;
		jne m1;
		jmp mret;
		m1:
		/*MOV EAX, 0x22;
		CMP CX, AX;
		jne m2;
		CMP ESI, 0x22;
		jne m2;
		jmp mret;
		m2:
		MOV EAX, 0x27;
		CMP CX, AX;
		jne m3;
		CMP ESI, 0x27;
		jne m3;
		jmp mret;
		m3:*/
		MOV EAX, 0x28;
		CMP CX, AX;
		jne m4;
		CMP ESI, 0x29;
		jne m4;
		jmp mret;
		m4:
		mret:
		ret;
	}
}


void __declspec(naked) SLVarStrLenAlt(void)
{
	__asm
	{
		cmp AX, 0xF0;
		jne m10;
		XOR EBX, EBX;
		CMP DWORD PTR [EDI+0x14], 8;
		jb m20;
		MOV ESI, DWORD PTR [EDI];
		jmp m21;
		m20:
		MOV ESI, EDI;
		m21:
		MOV EDX, DWORD PTR [0x4D26A48]; // ??
		LEA ESP, [ESP]; // ??
		sub ESP, 4;
		pushad;
		MOV EAX, DWORD PTR [EBP+0x8];
		push [EAX+8];
		push [EAX+4];
		push ESI;
		call GetStrWidthInChar;
		mov [ESP+32], EAX;
		popad;
		mov EBX, [ESP];
		add ESP, 4;
		MOV ECX, EBX;
		call SLPushStackFuncAdr;
		jmp SLVarStrLenAltJmp1;
		m10:
		cmp AX, 0xF1;
		jne m11;
		XOR EBX, EBX;
		CMP DWORD PTR [EDI+0x14], 8;
		jb m30;
		MOV ESI, DWORD PTR [EDI];
		jmp m31;
		m30:
		MOV ESI, EDI;
		m31:
		MOV EDX, DWORD PTR [0x4D26A48]; // ??
		LEA ESP, [ESP]; // ??
		sub ESP, 4;
		pushad;
		MOV EAX, DWORD PTR [EBP+0x8];
		push [EAX+8];
		push [EAX+4];
		push ESI;
		call GetStrWidthInPix;
		mov [ESP+32], EAX;
		popad;
		mov EBX, [ESP];
		add ESP, 4;
		MOV ECX, EBX;
		call SLPushStackFuncAdr;
		jmp SLVarStrLenAltJmp1;
		m11:
		PUSH 0x009A9A14;
		jmp SLVarStrLenAltJmp2;
	}
}

void __declspec(naked) SLStrConcConstStr(void)
{
	__asm
	{
		MOV ESI, ECX;
		MOV EAX, DWORD PTR [EBP+0x8];
		sub ESP, 4;
		pushad;
		push edx
		call CheckConstStr;
		mov [ESP+32], EAX;
		popad;
		mov EDX, [ESP];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) SLStrConcStrConst(void)
{
	__asm
	{
		MOV ESI, ECX;
		MOV EAX, DWORD PTR [EBP+0x8];
		sub ESP, 4;
		pushad;
		push eax
		call CheckConstStr;
		mov [ESP+32], EAX;
		popad;
		mov EAX, [ESP];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) SLStrCopyConstStr(void)
{
	__asm
	{
		MOV EDX, DWORD PTR [EBP+0x8];
		XOR EAX, EAX;
		sub ESP, 4;
		pushad;
		push edx
		call CheckConstStr;
		mov [ESP+32], EAX;
		popad;
		mov EDX, [ESP];
		add ESP, 4;
		ret;
	}
}

#elif (SLVER >= 0x01000400 && SLVER <= 0x0100070A)

void __declspec(naked) GetCharWidthTWSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP+0x54]; // MojiSize
		push [ESP+0x14+32+8+4]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		
		push [ESP+0x14+32+4+4]; // Char
		push [EBP+0x88]; // MaxX
		push [EBP+0x4C]; // CurX
		push [EBP+0x58]; // MojiSpace
		push [EBP+0x54]; // MojiSize
		push [ESP+0x14+32+8+32]; // Str
		call WordWrapTWSL;
		test AL, AL;
		jz m1;
		mov EAX, [EBP+0x88];
		add EAX, [EBP+0x54];
		add EAX, [EBP+0x58];
		mov [EBP+0x4C], EAX;
		mov [ESP+0x14+32+4+4], 0x20;
		m1:
		popad;
		mov ESI, [ESP];
		MOV EAX, DWORD PTR [EBP+0x88];
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) GetCharWidthNameWinSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP+0x30]; // MojiSize
		push [ESP+40+32+8+4]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		MOV EAX, DWORD PTR [EBP+0x34];
		add EAX, [ESP];
		ADD DWORD PTR [EBP+0x28], EAX;
		add ESP, 4;
		ret;
	}
}

void __declspec(naked) GetCharWidthObjSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [ESP+0x2C+32+8]; //ESI; // MojiSize
		push [ESP+0x24+32+8+4]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		
		mov EBX, [ESP+16];
		cmp DWORD PTR [EBX+0x98], 0;
		jle m1;

		mov EAX, [ESP+0x2C+32+8];
		add EAX, [EBX+0x90];
		imul EAX, [EBX+0x98];

		push [ESP+0x24+32+8]; // Char
		push EAX; // MaxX
		push [ESP+0x14+32+8+4+4]; // CurX
		push [EBX+0x90]; // MojiSpace
		push [ESP+0x2C+32+8+4+4+4+4]; // MojiSize
		push [ESP+0x7C]; // Str
		call WordWrapTWSL;
		test AL, AL;
		jz m1;
		mov ECX, [EBX+0x94];
		add ECX, [ESP+0x2C+32+8];
		add [ESP+0x18+32+8], ECX;
		mov DWORD PTR [ESP+0x1C+32+8], 0;
		mov DWORD PTR [ESP+0x14+32+8], 0;
		mov DWORD PTR [ESP+0x24+32+8], 0x20;
		jmp m2;
		m1:
		popad;
		mov EAX, [ESP];
		ADD EAX, [EBX+0x90];
		ADD [ESP+0x14+8], EAX;
		add ESP, 4;
		LEA ECX, [ESP+0x4C+4];
		ret;
		m2:
		popad;
		mov EAX, 0;
		add ESP, 4;
		LEA ECX, [ESP+0x4C+4];
		ret;
	}
}

void __declspec(naked) GetCharWidthSelObjSL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push EDI; // MojiSize
		push [ESP+20+32+8+4]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		MOV EDX, DWORD PTR [ESP+0x288+8];
		add EDX, [ESP];
		ADD DWORD PTR [ESP+0x1C+8], EDX;
		add ESP, 4;
		ret;
	}
}
/*
void __declspec(naked) GetCharWidthRubySL(void)
{
	__asm
	{
		sub ESP, 4;
		pushad;
		push [EBP-0x7A4]; // MojiSize
		push [EBP-0x79C]; // Char
		call GetCharWidthSL;
		mov [ESP+32], EAX;
		popad;
		add EAX, [ESP];
		add ESP, 4;
		ret;
	}
}
*/
void __declspec(naked) SLSetIndent(void)
{
	__asm
	{
		CMP EBX, 0x0FF08;
		je m0;
		cmp EBX, 0x22;
		je m0;
		cmp EBX, 0x27;
		je m0;
		cmp EBX, 0x28;
		m0:
		ret;
	}
}

void __declspec(naked) SLCheckIndent(void)
{
	__asm
	{
		CMP AX, 0x0FF08;
		jne m1;
		CMP EBX, 0x0FF09;
		jne m1;
		jmp mret;
		m1:
		/*CMP AX, 0x22;
		jne m2;
		CMP EBX, 0x22;
		jne m2;
		jmp mret;
		m2:
		CMP AX, 0x27;
		jne m3;
		CMP EBX, 0x27;
		jne m3;
		jmp mret;
		m3:*/
		CMP AX, 0x28;
		jne m4;
		CMP EBX, 0x29;
		jne m4;
		jmp mret;
		m4:
		mret:
		ret;
	}
}


void __declspec(naked) SLVarStrLenAlt(void)
{
	__asm
	{
		cmp AX, 0xF0;
		jne m10;
		LEA ECX, [ESP+0x18];
		PUSH ECX;
		MOV EDI, EBP;
		//XOR EBX, EBX;
		mov EBX, EAX;
		CALL SLGetLStrFuncAdr;
		sub ESP, 4;
		pushad;
		//push EAX;
		//push EBX;
		//push [ESP+0x18+12+32];
		mov EAX, [ESP+0xA4+12+32-8];
		push [EAX+8];
		push [EAX+4];
		push [ESP+0x18+12+32+4];
		call GetStrWidthInChar;
		mov [ESP+32], EAX;
		popad;
		mov EBX, [ESP];
		add ESP, 4;
		push EBX;
		call SLPushStackFuncAdr;
		add ESP, 4;
		jmp SLVarStrLenAltJmp1;
		m10:
		cmp AX, 0xF1;
		jne m11;
		LEA ECX, [ESP+0x18];
		PUSH ECX;
		MOV EDI, EBP;
		//XOR EBX, EBX;
		mov EBX, EAX;
		CALL SLGetLStrFuncAdr;
		sub ESP, 4;
		pushad;
		//push EAX;
		//push [ESP+0xA4+12+32-8];
		//push [ESP+0xA8+12+32-4];
		mov EAX, [ESP+0xA4+12+32-8];
		push [EAX+8];
		push [EAX+4];
		push [ESP+0x18+12+32+4];
		call GetStrWidthInPix;
		mov [ESP+32], EAX;
		popad;
		mov EBX, [ESP];
		add ESP, 4;
		push EBX;
		call SLPushStackFuncAdr;
		add ESP, 4;
		jmp SLVarStrLenAltJmp1;
		m11:
		MOV EDX, 0x007AA9EC;
		jmp SLVarStrLenAltJmp2;
	}
}

#endif

wchar_t SLScreanSizeHelpStr[] = L"Максимальное разрешение игры - %d * %d.\nПо возможности старайтесь выбирать именно это разрешение или максимально близкое к нему.\n\n Пользователям ЖК-мониторов:\nДаже при выбранном максимальном разрешении изображение может казаться несколько размытым или «грубым». Просим вас понять, что это связано с работой самого монитора.\n\nИногда даже при выбранном максимальном разрешении надписи могут казаться вытянутыми либо по длине, либо по ширине. Эта проблема связана с работой монитора. По возможности старайтесь выбирать разрешение, максимально близкое в пропорциях к разрешению игры.";
wchar_t SLFontTestStringStr[] = L"Тестовая строка.";
//wchar_t SLGameIsRunStr[] = L" уже работает.";

wchar_t SLCfgWndScreenStr[] = L"Экран";
wchar_t SLCfgWndSoundStr[] = L"Звук";
wchar_t SLCfgWndTextStr[] = L"Текст";
wchar_t SLCfgWndColorStr[] = L"Цвет";
wchar_t SLCfgWndVoiceStr[] = L"Голоса";
wchar_t SLCfgWndAutomodeStr[] = L"Авточтение";
wchar_t SLCfgWndVoicespeedStr[] = L"Скорость голоса";
wchar_t SLCfgWndSystemStr[] = L"Система";
wchar_t SLCfgWndTextSecStr[] = L" сек";

wchar_t SLRiched32ErrErrStr[] = L"Ошибка";
wchar_t SLRiched32LoadErrStr[] = L"Не удалось загрузить riched32.dll. Приложение будет закрыто.";

void HookText(void)
{
	DWORD dwOldProtect, dwNewProtect, dwDontCare;
	BOOL bVPError = FALSE;
	dwNewProtect = PAGE_EXECUTE_READWRITE;
	jump_near* lpFunc;

	DWORD ModuleAdr = (DWORD) SLModule;

	/*
	VirtualProtect((LPVOID) EPAdr, 8, dwNewProtect, &dwOldProtect);

	EPCode_orig = *(unsigned long long*) EPAdr;

	lpFunc = (jump_near*) EPAdr;
	//lpFunc->opcode = 0xe8;
	lpFunc->opcode = 0xe9;
	lpFunc->relativeAddress = (DWORD) &HookEP - ((DWORD) lpFunc + 5);

	EPCode_new = *(unsigned long long*) EPAdr;
	*/
	
	FontMonoFlagSetAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) FontMonoFlagSetAdr, 8, dwNewProtect, &dwOldProtect))
	{
		*((char*)FontMonoFlagSetAdr + 6) = 0x01;
		if (!VirtualProtect((LPVOID) FontMonoFlagSetAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}

	CharWidthTWAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthTWAdr, CharWidthTWAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) CharWidthTWAdr, 0x90, CharWidthTWAdrLen);
		lpFunc = (jump_near*) CharWidthTWAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthTWSL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthTWAdr, CharWidthTWAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
	

	CharWidthNameWinAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthNameWinAdr, CharWidthNameWinAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) CharWidthNameWinAdr, 0x90, CharWidthNameWinAdrLen);
		lpFunc = (jump_near*) CharWidthNameWinAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthNameWinSL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthNameWinAdr, CharWidthNameWinAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

#if (SLVER == 0x01015007)
	CharWidthObjVPAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthObjVPAdr, CharWidthObjVPAdrLen, dwNewProtect, &dwOldProtect))
	{
		*(unsigned char*)CharWidthObjVPAdr = 0xE9;
		DWORD tempptr = *(unsigned int*)(CharWidthObjVPAdr + 2);
		*(unsigned int*)(CharWidthObjVPAdr + 1) = tempptr + 1;
		*(unsigned char*)(CharWidthObjVPAdr + 5) = 0x90;
		CharWidthObjAdr += ModuleAdr;
		memset((void*) CharWidthObjAdr, 0x90, CharWidthObjAdrLen);
		lpFunc = (jump_near*) CharWidthObjAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthObjSL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthObjVPAdr, CharWidthObjVPAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
#elif (SLVER == 0x0100070A)
	CharWidthObjVPAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthObjVPAdr, CharWidthObjVPAdrLen, dwNewProtect, &dwOldProtect))
	{
		*(unsigned char*)CharWidthObjVPAdr = 0xEB;
		CharWidthObjAdr += ModuleAdr;
		memset((void*) CharWidthObjAdr, 0x90, CharWidthObjAdrLen);
		lpFunc = (jump_near*) CharWidthObjAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthObjSL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthObjVPAdr, CharWidthObjVPAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
#endif

	CharWidthSelObjAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthSelObjAdr, CharWidthSelObjAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) CharWidthSelObjAdr, 0x90, CharWidthSelObjAdrLen);
		lpFunc = (jump_near*) CharWidthSelObjAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthSelObjSL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthSelObjAdr, CharWidthSelObjAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

#if (SLVER == 0x01015007)
	CharWidthRuby1Adr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthRuby1Adr, CharWidthRuby1AdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) CharWidthRuby1Adr, 0x90, CharWidthRuby1AdrLen);
		lpFunc = (jump_near*) CharWidthRuby1Adr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthRubySL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthRuby1Adr, CharWidthRuby1AdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	CharWidthRuby2Adr += ModuleAdr;
	if (VirtualProtect((LPVOID) CharWidthRuby2Adr, CharWidthRuby2AdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) CharWidthRuby2Adr, 0x90, CharWidthRuby2AdrLen);
		lpFunc = (jump_near*) CharWidthRuby2Adr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &GetCharWidthRubySL - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) CharWidthRuby2Adr, CharWidthRuby2AdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
#endif

	SLSetIndentAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLSetIndentAdr, SLSetIndentAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) SLSetIndentAdr, 0x90, SLSetIndentAdrLen);
		lpFunc = (jump_near*) SLSetIndentAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &SLSetIndent - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLSetIndentAdr, SLSetIndentAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCheckIndentAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCheckIndentAdr, SLCheckIndentAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) SLCheckIndentAdr, 0x90, SLCheckIndentAdrLen);
		lpFunc = (jump_near*) SLCheckIndentAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &SLCheckIndent - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLCheckIndentAdr, SLCheckIndentAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

//#if (SLVER == 0x0100070A)
	SLVarStrLenAltAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLVarStrLenAltAdr, SLVarStrLenAltAdrLen, dwNewProtect, &dwOldProtect))
	{
		memset((void*) SLVarStrLenAltAdr, 0x90, SLVarStrLenAltAdrLen);
		lpFunc = (jump_near*) SLVarStrLenAltAdr;
		lpFunc->opcode = 0xe9;
		lpFunc->relativeAddress = (DWORD) &SLVarStrLenAlt - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLVarStrLenAltAdr, SLVarStrLenAltAdrLen, dwOldProtect, &dwDontCare)) bVPError = TRUE;
		SLPushStackFuncAdr += ModuleAdr;
		SLGetLStrFuncAdr   += ModuleAdr;
		SLVarStrLenAltJmp1 += ModuleAdr;
		SLVarStrLenAltJmp2 += ModuleAdr;
	}
	else bVPError = TRUE;
//#endif
	
	SLScreanSizeHelpStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLScreanSizeHelpStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLScreanSizeHelpStrAdr += 1;
		*(wchar_t**) SLScreanSizeHelpStrAdr = SLScreanSizeHelpStr;
		if (!VirtualProtect((LPVOID) SLScreanSizeHelpStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLFontTestStringStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLFontTestStringStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLFontTestStringStrAdr += 1;
		*(wchar_t**) SLFontTestStringStrAdr = SLFontTestStringStr;
		if (!VirtualProtect((LPVOID) SLFontTestStringStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	/*
	SLGameIsRunStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLGameIsRunStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLGameIsRunStrAdr += 1;
		*(wchar_t**) SLGameIsRunStrAdr = SLGameIsRunStr;
		if (!VirtualProtect((LPVOID) SLGameIsRunStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
	*/

	SLCfgWndScreenStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndScreenStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndScreenStrAdr += 1;
		*(wchar_t**) SLCfgWndScreenStrAdr = SLCfgWndScreenStr;
		if (!VirtualProtect((LPVOID) SLCfgWndScreenStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndTextStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndTextStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndTextStrAdr += 1;
		*(wchar_t**) SLCfgWndTextStrAdr = SLCfgWndTextStr;
		if (!VirtualProtect((LPVOID) SLCfgWndTextStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndSoundStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndSoundStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndSoundStrAdr += 1;
		*(wchar_t**) SLCfgWndSoundStrAdr = SLCfgWndSoundStr;
		if (!VirtualProtect((LPVOID) SLCfgWndSoundStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndColorStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndColorStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndColorStrAdr += 1;
		*(wchar_t**) SLCfgWndColorStrAdr = SLCfgWndColorStr;
		if (!VirtualProtect((LPVOID) SLCfgWndColorStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndVoiceStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndVoiceStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndVoiceStrAdr += 1;
		*(wchar_t**) SLCfgWndVoiceStrAdr = SLCfgWndVoiceStr;
		if (!VirtualProtect((LPVOID) SLCfgWndVoiceStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndAutomodeStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndAutomodeStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndAutomodeStrAdr += 1;
		*(wchar_t**) SLCfgWndAutomodeStrAdr = SLCfgWndAutomodeStr;
		if (!VirtualProtect((LPVOID) SLCfgWndAutomodeStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndVoicespeedStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndVoicespeedStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndVoicespeedStrAdr += 1;
		*(wchar_t**) SLCfgWndVoicespeedStrAdr = SLCfgWndVoicespeedStr;
		if (!VirtualProtect((LPVOID) SLCfgWndVoicespeedStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndSystemStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndSystemStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndSystemStrAdr += 1;
		*(wchar_t**) SLCfgWndSystemStrAdr = SLCfgWndSystemStr;
		if (!VirtualProtect((LPVOID) SLCfgWndSystemStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndTextSecStrAdr1 += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr1, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndTextSecStrAdr1 += 1;
		*(wchar_t**) SLCfgWndTextSecStrAdr1 = SLCfgWndTextSecStr;
		if (!VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr1, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndTextSecStrAdr2 += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr2, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndTextSecStrAdr2 += 1;
		*(wchar_t**) SLCfgWndTextSecStrAdr2 = SLCfgWndTextSecStr;
		if (!VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr2, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLCfgWndTextSecStrAdr3 += ModuleAdr;
	if (VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr3, 8, dwNewProtect, &dwOldProtect))
	{
		SLCfgWndTextSecStrAdr3 += 1;
		*(wchar_t**) SLCfgWndTextSecStrAdr3 = SLCfgWndTextSecStr;
		if (!VirtualProtect((LPVOID) SLCfgWndTextSecStrAdr2, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;


	SLRiched32ErrErrStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLRiched32ErrErrStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLRiched32ErrErrStrAdr += 1;
		*(wchar_t**) SLRiched32ErrErrStrAdr = SLRiched32ErrErrStr;
		if (!VirtualProtect((LPVOID) SLRiched32ErrErrStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLRiched32LoadErrStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLRiched32LoadErrStrAdr, 8, dwNewProtect, &dwOldProtect))
	{
		SLRiched32LoadErrStrAdr += 1;
		*(wchar_t**) SLRiched32LoadErrStrAdr = SLRiched32LoadErrStr;
		if (!VirtualProtect((LPVOID) SLRiched32LoadErrStrAdr, 8, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLStrConcConstStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLStrConcConstStrAdr, 5, dwNewProtect, &dwOldProtect))
	{
		//memset((void*) SLStrConcConstStrAdr, 0x90, 5);
		lpFunc = (jump_near*) SLStrConcConstStrAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &SLStrConcConstStr - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLStrConcConstStrAdr, 5, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	SLStrConcStrConstAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLStrConcStrConstAdr, 5, dwNewProtect, &dwOldProtect))
	{
		//memset((void*) SLStrConcConstStrAdr, 0x90, 5);
		lpFunc = (jump_near*) SLStrConcStrConstAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &SLStrConcStrConst - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLStrConcStrConstAdr, 5, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;
	
	SLStrCopyConstStrAdr += ModuleAdr;
	if (VirtualProtect((LPVOID) SLStrCopyConstStrAdr, 5, dwNewProtect, &dwOldProtect))
	{
		//memset((void*) SLStrCopyConstStrAdr, 0x90, 5);
		lpFunc = (jump_near*) SLStrCopyConstStrAdr;
		lpFunc->opcode = 0xe8;
		lpFunc->relativeAddress = (DWORD) &SLStrCopyConstStr - ((DWORD) lpFunc + 5);
		if (!VirtualProtect((LPVOID) SLStrCopyConstStrAdr, 5, dwOldProtect, &dwDontCare)) bVPError = TRUE;
	}
	else bVPError = TRUE;

	if (bVPError)
	{
		MessageBoxW(NULL, L"Fatal error: VirtualProtect return FALSE.", L"Siglus error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	FontHameAdr = FontHameAdr + ModuleAdr;
	HdcAdr = HdcAdr + ModuleAdr;
	SLCreateFont_func = (hSLCreateFont) (SLCreateFontAdr + ModuleAdr);
}
