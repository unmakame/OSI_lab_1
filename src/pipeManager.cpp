#include "pipeManager.hpp"
#include <iostream>
#include <cstring>

#ifdef WINDOWS_PLATFORM

PipeManager::PipeManager() : readHandle(INVALID_HANDLE_VALUE), writeHandle(INVALID_HANDLE_VALUE), 
                           isReadEndOpen(false), isWriteEndOpen(false) {}

PipeManager::~PipeManager() {
    closeBoth();
}

bool PipeManager::createPipe() {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&readHandle, &writeHandle, &saAttr, 0)) {
        std::cerr << "CreatePipe failed" << std::endl;
        return false;
    }

    isReadEndOpen = true;
    isWriteEndOpen = true;
    return true;
}

bool PipeManager::writeToPipe(const std::string& data) {
    if (!isWriteEndOpen) return false;
    
    DWORD bytesWritten;
    std::string dataWithNewline = data + "\n";
    BOOL success = WriteFile(writeHandle, dataWithNewline.c_str(), dataWithNewline.length(), &bytesWritten, NULL);
    return success && (bytesWritten == dataWithNewline.length());
}

bool PipeManager::readFromPipe(std::string& data) {
    if (!isReadEndOpen) return false;
    
    char buffer[1024];
    DWORD bytesRead;
    BOOL success = ReadFile(readHandle, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
    
    if (success && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        data = std::string(buffer);
        // Убираем символ новой строки если есть
        if (!data.empty() && data.back() == '\n') {
            data.pop_back();
        }
        return true;
    }
    return false;
}

void PipeManager::closeReadEnd() {
    if (isReadEndOpen) {
        CloseHandle(readHandle);
        isReadEndOpen = false;
    }
}

void PipeManager::closeWriteEnd() {
    if (isWriteEndOpen) {
        CloseHandle(writeHandle);
        isWriteEndOpen = false;
    }
}

void PipeManager::closeBoth() {
    closeReadEnd();
    closeWriteEnd();
}

int PipeManager::getReadFD() const { 
    return _open_osfhandle((intptr_t)readHandle, _O_RDONLY);
}

int PipeManager::getWriteFD() const { 
    return _open_osfhandle((intptr_t)writeHandle, _O_WRONLY);
}

#else

PipeManager::PipeManager() : isReadEndOpen(false), isWriteEndOpen(false) {
    pipefd[0] = -1;
    pipefd[1] = -1;
}

PipeManager::~PipeManager() {
    closeBoth();
}

bool PipeManager::createPipe() {
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return false;
    }
    isReadEndOpen = true;
    isWriteEndOpen = true;
    return true;
}

bool PipeManager::writeToPipe(const std::string& data) {
    if (!isWriteEndOpen) return false;
    std::string dataWithNewline = data + "\n";
    ssize_t bytesWritten = write(pipefd[1], dataWithNewline.c_str(), dataWithNewline.length());
    return bytesWritten == static_cast<ssize_t>(dataWithNewline.length());
}

bool PipeManager::readFromPipe(std::string& data) {
    if (!isReadEndOpen) return false;
    
    char buffer[1024];
    ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
    
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        data = std::string(buffer);
        // Убираем символ новой строки если есть
        if (!data.empty() && data.back() == '\n') {
            data.pop_back();
        }
        return true;
    }
    return false;
}

void PipeManager::closeReadEnd() {
    if (isReadEndOpen) {
        close(pipefd[0]);
        isReadEndOpen = false;
    }
}

void PipeManager::closeWriteEnd() {
    if (isWriteEndOpen) {
        close(pipefd[1]);
        isWriteEndOpen = false;
    }
}

void PipeManager::closeBoth() {
    closeReadEnd();
    closeWriteEnd();
}

int PipeManager::getReadFD() const { return pipefd[0]; }
int PipeManager::getWriteFD() const { return pipefd[1]; }

#endif