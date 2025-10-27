#pragma once

#include <string>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
#else
    #include <unistd.h>
#endif

class PipeManager {
private:
#ifdef WINDOWS_PLATFORM
    HANDLE readHandle;
    HANDLE writeHandle;
#else
    int pipefd[2];
#endif
    bool isReadEndOpen;
    bool isWriteEndOpen;

public:
    PipeManager();
    ~PipeManager();
    
    bool createPipe();
    int getReadFD() const;
    int getWriteFD() const;
    bool writeToPipe(const std::string& data);
    bool readFromPipe(std::string& data);
    void closeReadEnd();
    void closeWriteEnd();
    void closeBoth();
};