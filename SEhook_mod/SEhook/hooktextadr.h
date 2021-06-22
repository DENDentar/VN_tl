
/*
SEHook: hooktextadr.h
Copyright (C) DENDentar.
*/

#if (SLVER == 0x01015000)
// v1.1.80.0 (AngelBeats! -1st beat- Trial 1.00)
// Siglus Text Window
DWORD CharWidthTWAdr = 0x1CAF40 + 0x1000; // v1.1.80.0
DWORD CharWidthTWAdrNext = 6;

// Siglus Text Obj
DWORD CharWidthObjAdr = 0x1E12F2 + 0x1000; // v1.1.80.0
DWORD CharWidthObjAdrNext = 5;

// Siglus Name Window
DWORD CharWidthNameWinAdr = 0x1CDDA9; // v1.1.80.0
DWORD CharWidthNameWinAdrNext = 9;

// Siglus Select Obj
DWORD CharWidthSelObjAdr = 0x1942E8; // v1.1.80.0
DWORD CharWidthSelObjAdrNext = 6;

// Siglus Create Font function adress
DWORD SLCreateFontAdr = 0x280D30; // v1.1.80.0

// Pointer to Font Name adress
DWORD FontHameAdr = 0x605DC0; // v1.1.80.0
// Pointer to HDC
DWORD HdcAdr = 0x0605E90; // v1.1.80.0

#elif (SLVER == 0x01015007)

// v1.1.80.7 (AngelBeats! -1st beat-)
// Siglus Text Window
DWORD CharWidthTWAdr = 0x1CBC80;
DWORD CharWidthTWAdrLen = 6;

// Siglus Name Window
DWORD CharWidthNameWinAdr = 0x1CDAE9;
DWORD CharWidthNameWinAdrLen = 9;

// Siglus Text Obj
DWORD CharWidthObjVPAdr = 0x1E1E55;
DWORD CharWidthObjVPAdrLen = 0x1F7;
DWORD CharWidthObjAdr = 0x1E2032;
DWORD CharWidthObjAdrLen = 5;

// Siglus Select Obj
DWORD CharWidthSelObjAdr = 0x193F38;
DWORD CharWidthSelObjAdrLen = 6;

// Siglus Ruby
DWORD CharWidthRuby1Adr = 0x1CC912;
DWORD CharWidthRuby1AdrLen = 6;
DWORD CharWidthRuby2Adr = 0x1CCBB3;
DWORD CharWidthRuby2AdrLen = 6;

// Siglus Set Indent
DWORD SLSetIndentAdr = 0x1CBF28;
DWORD SLSetIndentAdrLen = 6;
DWORD SLCheckIndentAdr = 0x1CBFA4;
DWORD SLCheckIndentAdrLen = 16;

//
DWORD FontMonoFlagSetAdr = 0x143B1B;
// Siglus Create Font function adress
DWORD SLCreateFontAdr = 0x280B40;
// Pointer to Font Name adress
DWORD FontHameAdr = 0x605DC4;
// Pointer to HDC
DWORD HdcAdr = 0x605E94;

DWORD SLVarStrLenAltAdr = 0x245FCE; //0x1575C3;
DWORD SLVarStrLenAltAdrLen = 5;
DWORD SLVarStrLenAltJmp1 = 0x245FFB; //0x1575F8;
DWORD SLVarStrLenAltJmp2 = 0x245FD3; //0x1575C8;
DWORD SLPushStackFuncAdr = 0x18D070; //0xA17B0;
DWORD SLGetLStrFuncAdr   = 0; //0xA8E50;

DWORD SLScreanSizeHelpStrAdr = 0x1F137A; // Screan size help string
DWORD SLFontTestStringStrAdr = 0x1EF187; // Font test string
DWORD SLGameIsRunStrAdr = 0x1082CB; // Game is run

DWORD SLCfgWndScreenStrAdr = 0x1E5A13; // Config window Screan
DWORD SLCfgWndSoundStrAdr = 0x1E5B1D; // Config window Sound
DWORD SLCfgWndTextStrAdr = 0x1E5B76; // Config window Text
DWORD SLCfgWndTextSecStrAdr1 = 0x1E6D97; // Config window sec
DWORD SLCfgWndTextSecStrAdr2 = 0x1E6EDF; // Config window sec
DWORD SLCfgWndTextSecStrAdr3 = 0x1E7110; // Config window sec
DWORD SLCfgWndColorStrAdr = 0x1E5BE3; // Config window color
DWORD SLCfgWndVoiceStrAdr = 0x1E5D39; // Config window voice
DWORD SLCfgWndAutomodeStrAdr = 0x1E5D92; // Config window automode
DWORD SLCfgWndVoicespeedStrAdr = 0x1E5DFF; // Config window voice speed
DWORD SLCfgWndSystemStrAdr = 0x1E5EE8; // Config window system

DWORD SLRiched32ErrErrStrAdr = 0x108599; // riched32.dll load error string
DWORD SLRiched32LoadErrStrAdr = 0x10857B; // riched32.dll load error string

#elif (SLVER == 0x0100070A)

// v1.0.7.10 (Rewrite)

/*
Address   Hex dump          Command                                  Comments
00488310  /$  83EC 08       SUB ESP,8                                ; SiglusEngine.00488310(guessed Arg1,Arg2,Arg3,Arg4,Arg5)

CALS
4A87F2 - Siglus Select Obj
4DE3A4 - Siglus Text Window
4DFCB9 - Siglus Name Window
4E1F22
4EF03F - Siglus Text Obj
*/

// Siglus Text Window
DWORD CharWidthTWAdr = 0xDE3F7;
DWORD CharWidthTWAdrLen = 6;

// Siglus Name Window
DWORD CharWidthNameWinAdr = 0xDFD72;
DWORD CharWidthNameWinAdrLen = 37;

// Siglus Text Obj
DWORD CharWidthObjVPAdr = 0xEF052;
DWORD CharWidthObjVPAdrLen = 329;
DWORD CharWidthObjAdr = 0xEF182;
DWORD CharWidthObjAdrLen = 8;

// Siglus Select Obj
DWORD CharWidthSelObjAdr = 0xA888D;
DWORD CharWidthSelObjAdrLen = 44;

// Siglus Ruby?
DWORD CharWidthRuby1Adr = 0x1CC912;
DWORD CharWidthRuby1AdrLen = 6;
DWORD CharWidthRuby2Adr = 0x1CCBB3;
DWORD CharWidthRuby2AdrLen = 6;

// Siglus Set Indent?
DWORD SLSetIndentAdr = 0xDE521;
DWORD SLSetIndentAdrLen = 6;
DWORD SLCheckIndentAdr = 0xDE59E;
DWORD SLCheckIndentAdrLen = 12;

//
DWORD FontMonoFlagSetAdr = 0x42155;
// Siglus Create Font function adress
DWORD SLCreateFontAdr = 0x173F80;
// Pointer to Font Name adress
DWORD FontHameAdr = 0x3F73B0;
// Pointer to HDC
DWORD HdcAdr = 0x3F7390;

// 
DWORD SLVarStrLenAltAdr = 0x1575C3;
DWORD SLVarStrLenAltAdrLen = 5;
DWORD SLVarStrLenAltJmp1 = 0x1575F8;
DWORD SLVarStrLenAltJmp2 = 0x1575C8;
DWORD SLPushStackFuncAdr = 0xA17B0;
DWORD SLGetLStrFuncAdr   = 0xA8E50;


DWORD SLScreanSizeHelpStrAdr = 0xFF742;


#endif
