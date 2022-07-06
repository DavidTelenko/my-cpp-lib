#pragma once
#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <windows.h>

#define SET_UTF8_CONSOLE_CP() \
    SetConsoleCP(65001);      \
    SetConsoleOutputCP(65001);

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define SET_CONSOLE_VT_MODE()                      \
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); \
    DWORD dwMode = 0;                              \
    GetConsoleMode(hOut, &dwMode);                 \
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;  \
    SetConsoleMode(hOut, dwMode)

