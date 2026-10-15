#include "parameter_parser.hpp"
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

std::map<std::string, std::string> ParameterParser::parseString(const std::string& paramString) const {
    return parseKeyValuePairs(paramString);
}

std::map<std::string, std::string> ParameterParser::parseJSON(const std::string& jsonString) const {
    std::map<std::string, std::string> parameters;
    
    try {
        json j = json::parse(jsonString);
        for (const auto& item : j.items()) {
            parameters[item.key()] = item.value().get<std::string>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    
    return parameters;
}

std::map<std::string, std::string> ParameterParser::parseCommandLine(int argc, char** argv) const {
    std::map<std::string, std::string> parameters;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find('=') != std::string::npos) {
            size_t equalsPos = arg.find('=');
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            parameters[key] = value;
        } else if (arg.substr(0, 2) == "--") {
            parameters[arg] = "true"; // Boolean flag
        } else {
            // Positional argument
            static int posIndex = 0;
            parameters["arg_" + std::to_string(posIndex++)] = arg;
        }
    }
    
    return parameters;
}

std::map<std::string, std::string> ParameterParser::parseFile(const std::string& filepath) const {
    std::map<std::string, std::string> parameters;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file: " << filepath << std::endl;
            return parameters;
        }
        
        // Try JSON format first
        json j;
        file >> j;
        
        for (const auto& item : j.items()) {
            parameters[item.key()] = item.value().get<std::string>();
        }
        
    } catch (...) {
        // If JSON fails, try key=value format
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Skip empty lines
            if (line.empty()) {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                parameters[key] = value;
            }
        }
    }
    
    return parameters;
}

bool ParameterParser::validateParameter(const std::string& key, const std::string& value, const std::string& typeHint) const {
    // Basic validation
    if (key.empty() || value.empty()) {
        return false;
    }
    
    // Type-specific validation
    if (typeHint == "int") {
        try {
            std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (typeHint == "float") {
        try {
            std::stof(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (typeHint == "double") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (typeHint == "bool") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    
    return true; // "auto" or unknown type hints - accept any
}

std::string ParameterParser::getParameterType(const std::string& key, const std::map<std::string, std::string>& parameters) const {
    auto it = parameters.find(key);
    if (it == parameters.end()) {
        return "auto";
    }
    
    const std::string& value = it->second;
    
    // Simple type detection
    if (value == "true" || value == "false") {
        return "bool";
    } else if (value.find('.') != std::string::npos) {
        try {
            std::stod(value);
            return "double";
        } catch (...) {
            return "auto";
        }
    } else {
        try {
            std::stoi(value);
            return "int";
        } catch (...) {
            return "auto";
        }
    }
}

std::map<std::string, std::string> ParameterParser::parseKeyValuePairs(const std::string& input) const {
    std::map<std::string, std::string> parameters;
    std::stringstream ss(input);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        size_t equalsPos = item.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = item.substr(0, equalsPos);
            std::string value = item.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            parameters[key] = value;
        }
    }
    
    return parameters;
}

} // namespace nlm