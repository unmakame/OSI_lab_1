
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "pipeManager.hpp"
#include "stringProcessor.hpp"
#include "platform.hpp"

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
#endif

class ParentProcess {
private:
    PipeManager pipe1; // Для коротких строк (≤10 символов)
    PipeManager pipe2; // Для длинных строк (>10 символов)
    std::string filename1, filename2;
    
#ifdef WINDOWS_PLATFORM
    std::vector<PROCESS_INFORMATION> childProcesses;
#else
    std::vector<pid_t> childPids;
#endif

    bool createChildProcess(const std::string& executable, const std::string& filename, 
                          const std::string& processNumber, PipeManager& inputPipe);

public:
    ParentProcess(const std::string& file1, const std::string& file2) 
        : filename1(file1), filename2(file2) {}
    ~ParentProcess();

    bool initializePipes();
    bool createChildProcesses();
    void run();
    void cleanup();
};