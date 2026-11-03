// Configuration Loader
#pragma once

#include "IConfiguration.hpp"
#include "ConfigurationTemplate.hpp"
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace nlm {

class ConfigurationLoader {
public:
    ConfigurationLoader() = default;
    ~ConfigurationLoader() = default;
    
    // Disable copying, enable moving
    ConfigurationLoader(const ConfigurationLoader&) = delete;
    ConfigurationLoader& operator=(const ConfigurationLoader&) = delete;
    ConfigurationLoader(ConfigurationLoader&&) noexcept = default;
    ConfigurationLoader& operator=(ConfigurationLoader&&) noexcept = default;
    
    // Loading from file formats
    static std::shared_ptr<IConfiguration> loadFromJSON(const std::string& filepath);
    static std::shared_ptr<IConfiguration> loadFromINI(const std::string& filepath);
    static std::shared_ptr<IConfiguration> loadFromYAML(const std::string& filepath);
    static std::shared_ptr<IConfiguration> loadFromProperties(const std::string& filepath);
    
    // Loading from strings
    static std::shared_ptr<IConfiguration> loadFromJSONString(const std::string& data);
    static std::shared_ptr<IConfiguration> loadFromINIString(const std::string& data);
    
    // Loading from command line
    static std::shared_ptr<IConfiguration> loadFromCommandLine(int argc, char** argv);
    
    // Loading from environment variables
    static std::shared_ptr<IConfiguration> loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Loading from multiple sources with precedence
    static std::shared_ptr<IConfiguration> loadFromMultipleSources(
        const std::vector<std::string>& jsonFiles,
        const std::vector<std::string>& iniFiles,
        const std::string& envPrefix = "",
        int argc = 0, char** argv = nullptr);
    
    // Configuration validation during loading
    static bool validateLoadedConfig(const std::shared_ptr<IConfiguration>& config);
    
    // Error handling
    struct LoadError {
        std::string message;
        std::string source;
        size_t line = 0;
        size_t column = 0;
        
        LoadError(const std::string& msg, const std::string& src) : message(msg), source(src) {}
        LoadError(const std::string& msg, const std::string& src, size_t line, size_t column) 
            : message(msg), source(src), line(line), column(column) {}
    };
    
    using LoadErrorCallback = std::function<void(const LoadError&)>;
    
    // Statistics for loaded configurations
    struct LoadStatistics {
        size_t totalFilesLoaded = 0;
        size_t totalKeysLoaded = 0;
        size_t successfulLoads = 0;
        size_t failedLoads = 0;
        double totalLoadTime = 0.0;
        std::vector<LoadError> errors;
    };
    
    static LoadStatistics getLastLoadStatistics() { return last_stats_; }
    
protected:
    // Internal parsing methods
    static void parseJSON(const std::string& data, ConfigurationTemplate& config);
    static void parseINI(const std::string& data, ConfigurationTemplate& config);
    static void parseYAML(const std::string& data, ConfigurationTemplate& config);
    static void parseProperties(const std::string& data, ConfigurationTemplate& config);
    
    // String processing utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string extractValue(const std::string& line);
    static std::string extractKey(const std::string& line);
    
    // Command line processing
    static void parseCommandLine(int argc, char** argv, ConfigurationTemplate& config);
    static void parseEnvironment(const std::unordered_map<std::string, std::string>& envVars, 
                                ConfigurationTemplate& config, const std::string& prefix = "");
    
    static LoadStatistics last_stats_;
};

} // namespace nlm
