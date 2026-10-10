#pragma once

#include "Config.hpp"
#include <string>
#include <memory>

namespace nlm {

class ConfigJSON {
public:
    ConfigJSON();
    ~ConfigJSON();
    
    // Disable copying, enable moving
    ConfigJSON(const ConfigJSON&) = delete;
    ConfigJSON& operator=(const ConfigJSON&) = delete;
    ConfigJSON(ConfigJSON&&) noexcept;
    ConfigJSON& operator=(ConfigJSON&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON content)
    bool loadFromString(const std::string& jsonString);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get file path
    std::string getFilepath() const;
    void setFilepath(const std::string& filepath);
    
    // Formatting options
    bool setPrettyPrint(bool enable);
    bool setValidateSchema(bool enable, const std::string& schemaFile = "");
    
    // Get JSON representation
    std::string toString() const;
    
    // Access underlying Config
    Config& getConfig() { return config_; }
    const Config& getConfig() const { return config_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Config config_;
    
    // Helper methods
    std::string generateJSON() const;
    bool parseJSON(const std::string& json);
    void storeParsedValue(const std::string& key, const std::string& valueStr);
    std::string escapeJSON(const std::string& str);
    
    ConfigValue getValue(const std::string& key) const;
    std::vector<std::string> getKeys() const { return config_.getKeys(); }
    
    friend class Config; // Allow Config to access private methods
};

} // namespace nlm
