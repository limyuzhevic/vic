#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/Error.hpp"
#include "../core/InputValidation/InputValidator.hpp"

namespace nlm {
namespace tools {

class NLMConfig {
public:
    NLMConfig() = default;
    ~NLMConfig() = default;
    
    // Main entry point
    static int run(int argc, char* argv[]);
    
    // Configuration loading
    static bool loadConfig(const std::string& filepath, std::map<std::string, std::string>& config);
    
    // Configuration saving
    static bool saveConfig(const std::map<std::string, std::string>& config, const std::string& filepath);
    
    // Validation
    static bool validateConfig(const std::map<std::string, std::string>& config);
    
    // Generation
    static std::map<std::string, std::string> generateConfig(const std::string& templatePath = "");
    
    // Comparison
    static void compareConfigs(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2);
    
    // Analysis
    static void analyzeParameters(const std::map<std::string, std::string>& config);
    
    // Help
    static void printHelp();
    
private:
    // Utility functions
    static std::string trim(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filename);
    static std::string getParameterInfo(const std::string& param, const std::string& value);
    
    // File format detection
    static bool isJSONFile(const std::string& content);
    static bool isYAMLFile(const std::string& content);
    static bool isSimpleFormat(const std::string& content);
};

} // namespace tools
} // namespace nlm

// Entry point for the executable
int main(int argc, char* argv[]) {
    return nlm::tools::NLMConfig::run(argc, argv);
}
