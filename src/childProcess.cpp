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
    
    // Открываем файл на запись (только чтобы создать/открыть, но не пишем результаты)
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }
    file.close(); // Закрываем файл, так как по условию результаты идут в stdout

    std::string input;
    while (true) {
        // Читаем из стандартного ввода (из pipe от родителя)
        if (!std::getline(std::cin, input)) {
            break;
        }
        
        if (input.empty()) {
            continue;
        }

        // Обрабатываем строку - удаляем гласные
        std::string processed = StringProcessor::removeVowels(input);
        
        // Пишем результат в стандартный вывод (по условию)
        std::cout << "Child" << processNumber << " processed: '" << input 
                  << "' -> '" << processed << "'" << std::endl;
        std::cout.flush();
    }

    return 0;
}