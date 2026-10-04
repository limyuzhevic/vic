#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== DEBUG COMMAND LINE PARSING ===" << std::endl;
    
    nlm::Config config;
    
    const int argc = 5;
    const char* argv[] = {
        "test_program",
        "--cli_int=999",
        "--cli_double=2.718",
        "--cli_bool=true",
        "--cli_string=command_line"
    };
    
    std::cout << "Arguments to parse:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "  argv[" << i << "] = \"" << argv[i] << "\"" << std::endl;
    }
    
    std::cout << "\nLoading from args..." << std::endl;
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    std::cout << "\nResulting config keys:" << std::endl;
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::cout << "  Key: " << key << " = ";
        
        if (const auto* val = config.get<int>(key)) {
            std::cout << *val << " (int)" << std::endl;
        } else if (const auto* val = config.get<double>(key)) {
            std::cout << *val << " (double)" << std::endl;
        } else if (const auto* val = config.get<std::string>(key)) {
            std::cout << *val << " (string)" << std::endl;
        } else if (const auto* val = config.get<bool>(key)) {
            std::cout << *val << " (bool)" << std::endl;
        } else {
            std::cout << "(unknown type)" << std::endl;
        }
    }
    
    std::cout << "\nExpected: cli_int=999, cli_double=2.718, cli_bool=true, cli_string=command_line" << std::endl;
    
    return 0;
}
