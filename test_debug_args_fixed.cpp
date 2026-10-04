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
        
        auto intVal = config.get<int>(key);
        if (intVal.has_value()) {
            std::cout << *intVal << " (int)" << std::endl;
            continue;
        }
        
        auto doubleVal = config.get<double>(key);
        if (doubleVal.has_value()) {
            std::cout << *doubleVal << " (double)" << std::endl;
            continue;
        }
        
        auto stringVal = config.get<std::string>(key);
        if (stringVal.has_value()) {
            std::cout << *stringVal << " (string)" << std::endl;
            continue;
        }
        
        auto boolVal = config.get<bool>(key);
        if (boolVal.has_value()) {
            std::cout << *boolVal << " (bool)" << std::endl;
            continue;
        }
        
        std::cout << "(unknown type)" << std::endl;
    }
    
    std::cout << "\nChecking expected values:" << std::endl;
    
    auto cliInt = config.get<int>("cli_int");
    std::cout << "cli_int: " << (cliInt.has_value() ? "FOUND = " + std::to_string(*cliInt) : "NOT FOUND") << std::endl;
    
    auto cliDouble = config.get<double>("cli_double");
    std::cout << "cli_double: " << (cliDouble.has_value() ? "FOUND = " + std::to_string(*cliDouble) : "NOT FOUND") << std::endl;
    
    auto cliBool = config.get<bool>("cli_bool");
    std::cout << "cli_bool: " << (cliBool.has_value() ? "FOUND = " + std::to_string(*cliBool) : "NOT FOUND") << std::endl;
    
    auto cliString = config.get<std::string>("cli_string");
    std::cout << "cli_string: " << (cliString.has_value() ? "FOUND = " + *cliString : "NOT FOUND") << std::endl;
    
    std::cout << "\nExpected: cli_int=999, cli_double=2.718, cli_bool=true, cli_string=command_line" << std::endl;
    
    return 0;
}
