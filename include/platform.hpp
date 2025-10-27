#pragma once

#ifdef _WIN32
    #define WINDOWS_PLATFORM
    #include <windows.h>
#else
    #define UNIX_PLATFORM
    #include <unistd.h>
    #include <sys/wait.h>
    #include <signal.h>
#endif

#include <string>

class Platform {
public:
    static void sleep(int milliseconds);
};