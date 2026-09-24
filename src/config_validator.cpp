// Fixed version of config_validator.cpp
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace nlm;

class ConfigValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validateFile(const std::string& filename) {
        ValidationResult result;
        result.valid = true;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            result.valid = false;
            result.errors.push_back("Cannot open configuration file: " + filename);
            return result;
        }
        
        // Read and parse the config file
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            ++lineNum;
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Simple parsing: "key = value"
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                result.warnings.push_back("Line " + std::to_string(lineNum) + ": Invalid format - no '=' found");
                continue;
            }
            
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            
            // Validate known keys
            if (!isValidKey(key)) {
                result.warnings.push_back("Line " + std::to_string(lineNum) + ": Unknown configuration key: " + key);
            }
            
            // Basic type validation
            if (!validateValueType(key, value)) {
                result.errors.push_back("Line " + std::to_string(lineNum) + ": Invalid value for key '" + key + "': " + value);
                result.valid = false;
            }
        }
        
        // If file is empty or has no valid entries
        if (lineNum == 0) {
            result.warnings.push_back("Configuration file is empty or contains only comments");
        }
        
        return result;
    }
    
    void printValidationResult(const ValidationResult& result, const std::string& filename) {
        std::cout << "=== Configuration Validation Report ===" << std::endl;
        std::cout << "File: " << filename << std::endl;
        std::cout << "" << std::endl;
        
        if (result.valid) {
            std::cout << "✓ Configuration file is VALID" << std::endl;
        } else {
            std::cout << "✗ Configuration file is INVALID" << std::endl;
        }
        
        std::cout << "" << std::endl;
        
        if (!result.errors.empty()) {
            std::cout << "ERRORS:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  ✗ " << error << std::endl;
            }
            std::cout << "" << std::endl;
        }
        
        if (!result.warnings.empty()) {
            std::cout << "WARNINGS:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "  ! " << warning << std::endl;
            }
            std::cout << "" << std::endl;
        }
        
        std::cout << "============================================" << std::endl;
    }
    
    void listPresets() {
        std::cout << "=== Available Configuration Presets ===" << std::endl;
        std::cout << "" << std::endl;
        
        struct Preset {
            std::string name;
            std::string description;
            std::string usage;
        };
        
        std::vector<Preset> presets = {
            {"quick", "Quick test with minimal neurons", "nlm quick"},
            {"medium", "Standard test with medium neurons", "nlm medium"},
            {"comprehensive", "Comprehensive test with full neurons", "nlm comprehensive"},
            {"debug", "Debug mode with verbose output", "nlm debug"},
            {"basic-connectivity", "Test basic neural connectivity only", "nlm basic-connectivity"},
            {"plasticity-learning", "Test plasticity and learning only", "nlm plasticity-learning"},
            {"stdp-verification", "Test STDP mechanism only", "nlm stdp-verification"}
        };
        
        for (const auto& preset : presets) {
            std::cout << "Preset: " << preset.name << std::endl;
            std::cout << "  Description: " << preset.description << std::endl;
            std::cout << "  Usage:       " << preset.usage << std::endl;
            std::cout << "" << std::endl;
        }
        
        std::cout << "============================================" << std::endl;
    }
    
private:
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }
    
    bool isValidKey(const std::string& key) {
        // Known configuration keys
        static const std::vector<std::string> knownKeys = {
            "random_seed", "simulation_timestep", "neuron_count", "region_count",
            "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
            "synaptogenesis_rate", "pruning_rate", "replay_interval", "consolidation_interval",
            "checkpoint_dir", "log_level", "log_file"
        };
        
        for (const auto& knownKey : knownKeys) {
            if (key == knownKey) {
                return true;
            }
        }
        return false;
    }
    
    bool validateValueType(const std::string& key, const std::string& value) {
        // Based on expected key type
        if (key == "random_seed") {
            return isInteger(value);
        } else if (key == "simulation_timestep" || key == "stdp_tau") {
            return isFloat(value);
        } else if (key == "neuron_count" || key == "region_count" || 
                   key == "replay_interval" || key == "consolidation_interval") {
            return isInteger(value);
        } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                   key == "stdp_ltd_weight" || key == "synaptogenesis_rate" || 
                   key == "pruning_rate") {
            return isFloat(value);
        } else if (key == "log_level") {
            return isLogLevel(value);
        }
        return true; // Unknown keys pass validation
    }
    
    bool isInteger(const std::string& str) {
        if (str.empty()) return false;
        size_t pos;
        try {
            std::stoll(str, pos);
            return pos == str.size();
        } catch (...) {
            return false;
        }
    }
    
    bool isFloat(const std::string& str) {
        if (str.empty()) return false;
        size_t pos;
        try {
            std::stod(str, pos);
            return pos == str.size();
        } catch (...) {
            return false;
        }
    }
    
    bool isLogLevel(const std::string& str) {
        static const std::vector<std::string> validLevels = {"debug", "info", "warning", "error", "fatal"};
        return std::find(validLevels.begin(), validLevels.end(), str) != validLevels.end();
    }
};

int main(int argc, char** argv) {
    std::cout << "=== NLM Configuration Validator ===" << std::endl;
    std::cout << "" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: config_validator <config_file>" << std::endl;
        std::cout << "       config_validator --list-presets" << std::endl;
        return 1;
    }
    
    std::string arg(argv[1]);
    
    if (arg == "--list-presets") {
        ConfigValidator validator;
        validator.listPresets();
        return 0;
    }
    
    ConfigValidator validator;
    auto result = validator.validateFile(arg);
    validator.printValidationResult(result, arg);
    
    return result.valid ? 0 : 1;
}
