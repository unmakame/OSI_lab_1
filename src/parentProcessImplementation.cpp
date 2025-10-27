#include "parentProcess.cpp"
#include <iostream>
#include <cstring>

#ifdef WINDOWS_PLATFORM

bool ParentProcess::createChildProcess(const std::string& executable, const std::string& filename, 
                                     const std::string& processNumber, PipeManager& inputPipe) {
    std::string commandLine = executable + " " + filename + " " + processNumber;
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    
    si.hStdInput = (HANDLE)_get_osfhandle(inputPipe.getReadFD());
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;
    
    char* cmdLine = new char[commandLine.length() + 1];
    strcpy(cmdLine, commandLine.c_str());
    
    BOOL success = CreateProcessA(
        NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi
    );
    
    delete[] cmdLine;
    
    if (success) {
        childProcesses.push_back(pi);
        return true;
    }
    
    std::cerr << "CreateProcess failed (" << GetLastError() << ")" << std::endl;
    return false;
}

#else

bool ParentProcess::createChildProcess(const std::string& executable, const std::string& filename, 
                                     const std::string& processNumber, PipeManager& inputPipe) {
    pid_t pid = fork();
    
    if (pid == 0) {
        dup2(inputPipe.getReadFD(), STDIN_FILENO);
        
        
        pipe1.closeReadEnd();
        pipe1.closeWriteEnd();
        pipe2.closeReadEnd();
        pipe2.closeWriteEnd();
        
        execl(executable.c_str(), executable.c_str(), filename.c_str(), processNumber.c_str(), nullptr);
        perror("execl failed");
        exit(1);
    } else if (pid > 0) {
        childPids.push_back(pid);
        return true;
    }
    
    perror("fork failed");
    return false;
}

#endif

bool ParentProcess::initializePipes() {
    return pipe1.createPipe() && pipe2.createPipe();
}

bool ParentProcess::createChildProcesses() {
    
    bool success1 = createChildProcess("./childProcess", filename1, "1", pipe1);
    Platform::sleep(100);
    
    
    bool success2 = createChildProcess("./childProcess", filename2, "2", pipe2);
    Platform::sleep(100);
    
    
    pipe1.closeReadEnd();
    pipe2.closeReadEnd();
    
    return success1 && success2;
}

void ParentProcess::run() {
    std::cout << "Parent process started. Enter strings (empty line to exit):" << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            break;
        }

        if (StringProcessor::shouldGoToPipe2(input)) {
            std::cout << "Routing to PIPE2 (long string >10): " << input << std::endl;
            if (!pipe2.writeToPipe(input)) {
                std::cerr << "Failed to write to pipe2" << std::endl;
            }
        } else {
            std::cout << "Routing to PIPE1 (short string ≤10): " << input << std::endl;
            if (!pipe1.writeToPipe(input)) {
                std::cerr << "Failed to write to pipe1" << std::endl;
            }
        }
    }

    
    pipe1.closeWriteEnd();
    pipe2.closeWriteEnd();
    
    std::cout << "Waiting for child processes to finish..." << std::endl;
    cleanup();
    std::cout << "Parent process finished." << std::endl;
}

void ParentProcess::cleanup() {
#ifdef WINDOWS_PLATFORM
    for (auto& pi : childProcesses) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    childProcesses.clear();
#else
    for (auto pid : childPids) {
        waitpid(pid, nullptr, 0);
    }
    childPids.clear();
#endif
}

ParentProcess::~ParentProcess() {
    cleanup();
}