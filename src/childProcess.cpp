#include <iostream>
#include <fstream>
#include <string>
#include "stringProcessor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <process_number>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int processNumber = std::stoi(argv[2]);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }
    file.close(); 

    std::string input;
    while (true) {
        if (!std::getline(std::cin, input)) {
            break;
        }
        
        if (input.empty()) {
            continue;
        }

        std::string processed = StringProcessor::removeVowels(input);
        std::cout << "Child" << processNumber << " processed: '" << input 
                  << "' -> '" << processed << "'" << std::endl;
        std::cout.flush();
    }

    return 0;
}