#pragma once

#include "PlasticityCLI.hpp"
#include "PlasticityRuleFactory.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>

namespace nlm {

    bool parseCommandLine(const std::string& line, std::string& command, 
                         std::string& typeName, std::map<std::string, float>& params) {
        std::istringstream iss(line);
        iss >> command;
        
        if (command == "create") {
            iss >> typeName;
            std::string param;
            while (iss >> param) {
                size_t equalPos = param.find('=');
                if (equalPos != std::string::npos && equalPos + 1 < param.length()) {
                    std::string key = param.substr(0, equalPos);
                    std::string valueStr = param.substr(equalPos + 1);
                    try {
                        float value = std::stof(valueStr);
                        params[key] = value;
                    } catch (...) {
                        std::cerr << "Warning: Invalid parameter value '" << valueStr 
                                  << "' for key '" << key << "'" << std::endl;
                    }
                }
            }
            return true;
        } else if (command == "list-rules" || command == "help") {
            return true;
        }
        
        return false;
    }

    void runCLI() {
        std::cout << "NLM Plasticity Command Line Interface" << std::endl;
        std::cout << "Type 'help' for commands" << std::endl << std::endl;
        
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            std::string command, typeName;
            std::map<std::string, float> params;
            
            if (parseCommandLine(line, command, typeName, params)) {
                if (command == "list-rules") {
                    printAvailablePlasticityRules();
                } else if (command == "create") {
                    if (typeName == "STDP") {
                        configureStdpFromParams(params);
                    } else if (typeName == "Hebbian") {
                        configureHebbianFromParams(params);
                    } else if (typeName == "StructuralPlasticity") {
                        configureStructuralPlasticityFromParams(params);
                    } else {
                        createPlasticityRule(typeName, params);
                    }
                } else if (command == "help") {
                    printHelp();
                }
            } else {
                std::cerr << "Unknown command: '" << command << "'" << std::endl;
                std::cerr << "Type 'help' for available commands." << std::endl;
            }
            
            std::cout << std::endl;
        }
    }

} // namespace nlm