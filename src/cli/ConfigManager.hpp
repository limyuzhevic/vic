#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class ConfigManager {
public:
    ConfigManager();
    
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename) const;
    bool loadFromArgs(int argc, char** argv);
    
    const Config& getConfig() const { return config_; }
    Config& getConfig() { return config_; }
    
    std::string getOutputDir() const { return outputDir_; }
    void setOutputDir(const std::string& dir) { outputDir_ = dir; }
    
    void printHelp(const std::string& programName) const;
    void printVersion() const;
    
private:
    Config config_;
    std::string outputDir_;
    std::map<std::string, std::string> paramOverrides_;
    
    bool parseConfigFile(const std::string& filename);
    bool parseJsonFile(const std::string& filename);
    bool parseCfgFile(const std::string& filename);
    void setupDefaultConfig();
    std::string extractConfigValue(const std::string& line);
    std::string trim(const std::string& str);
};

} // namespace nlm