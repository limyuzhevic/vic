#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "Config.hpp"

namespace nlm {

// ConfigCLI class for enhanced command-line interface
class ConfigCLI {
public:
    ConfigCLI();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Execute commands
    bool execute();
    
    // Get help text
    std::string getHelp() const;
    
    // Get version string
    std::string getVersion() const;
    
    // Validate configuration
    bool validateConfig() const;
    
    // Generate configuration
    bool generateConfig(const std::string& filepath) const;
    
    // Set configuration value
    bool setValue(const std::string& key, const std::string& valueStr);
    
    // Set typed configuration value
    template<typename T>
    bool setTypedValue(const std::string& key, const std::string& valueStr);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Parse --help argument
    bool parseHelp(const std::string& arg);
    
    // Parse --version argument
    bool parseVersion(const std::string& arg);
    
    // Parse --validate-config argument
    bool parseValidateConfig(const std::string& arg);
    
    // Parse --generate-config argument
    bool parseGenerateConfig(const std::string& arg);
    
    // Parse --set=key=value argument
    bool parseSet(const std::string& arg);
    
    // Parse --set-type=key:type=value argument
    bool parseSetType(const std::string& arg);
    
    // Display help
    void showHelp() const;
    
    // Display version
    void showVersion() const;
    
    // Display validation errors
    void showValidationErrors(const std::vector<std::string>& errors) const;
};

} // namespace nlm