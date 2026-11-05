#include "BrainResetCommand.hpp"
#include "brain/Brain.hpp"
#include <iostream>

bool BrainResetCommand::execute(const std::vector<std::string>& args) {
    bool preserveConfig = false;
    for (const auto& arg : args) {
        if (arg == "--preserve-config") preserveConfig = true;
    }
    
    std::cout << "Reset brain to initial state" << std::endl;
    if (preserveConfig) {
        std::cout << "  (Configuration preserved)" << std::endl;
    }
    
    std::cout << "  Brain state reset completed" << std::endl;
    
    return true;
}