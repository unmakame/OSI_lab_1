cmake_minimum_required(VERSION 3.10)
project(ProcessCommunication)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-DWINDOWS_PLATFORM)
else()
    add_definitions(-DUNIX_PLATFORM)
endif()

add_executable(parentProcess 
    src/main.cpp 
    src/parentProcessImplementation.cpp
    src/pipeManager.cpp 
    src/stringProcessor.cpp
    src/platform.cpp
)

add_executable(childProcess 
    src/childProcess.cpp 
    src/stringProcessor.cpp
)

target_include_directories(parentProcess PRIVATE include)
target_include_directories(childProcess PRIVATE include)

if(WIN32)
    target_link_libraries(parentProcess Kernel32.lib)
endif()