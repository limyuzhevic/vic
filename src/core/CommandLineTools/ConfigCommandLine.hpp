#pragma once

#include <string>
#include <memory>

namespace nlm {

class ConfigHelper;
class ConfigCommandLine {
public:
    ConfigCommandLine();
    ~ConfigCommandLine();
    
    // Disable copying, enable moving
    ConfigCommandLine(const ConfigCommandLine&) = delete;
    ConfigCommandLine& operator=(const ConfigCommandLine&) = delete;
    ConfigCommandLine(ConfigCommandLine&&) noexcept;
    ConfigCommandLine& operator=(ConfigCommandLine&&) noexcept;
    
    // Configuration file path
    std::string configFile_;
    std::string outputFile_;
    std::string format_;
    std::string schemaFile_;
    std::string mergeFile_;
    
    // Command line options
    bool verbose_;
    bool quiet_;
    bool validateOnly_;
    bool createDefaultConfig_;
    bool listKeys_;
    bool getValue_; // Keep for compatibility
    std::string getKey_;
    std::string setKey_;
    std::string setValue_;
    
    // Main processing functions
    bool parseArguments(int argc, char** argv);
    bool loadConfiguration();
    bool processCommands();
    
    // Helper functions
    void listKeys();
    void getValue(const std::string& key);
    void setValue(const std::string& key, const std::string& valueStr);
    void saveConfiguration();
    void createDefaultConfig();
    void validateConfiguration();
    
    // Display functions
    void showHelp();
    void showVersion();
    
    // Configuration processing
    ConfigValue parseValue(const std::string& str);
    std::string getConfigValue(const std::string& key);
    
    // Configuration storage
    std::unique_ptr<ConfigHelper> config_;
};

} // namespace nlm
