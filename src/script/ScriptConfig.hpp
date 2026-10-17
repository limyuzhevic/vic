// NLM Script Configuration
// Provides configuration management for script execution

#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <chrono>
#include "script/ScriptProcessor.hpp"

namespace nlm {

// Script configuration entry
struct ScriptConfigEntry {
    std::string key;
    std::string value;
    std::string description;
    bool required = false;
    std::string defaultValue;
    std::string type;  // string, int, double, bool
    
    ScriptConfigEntry() {}
    ScriptConfigEntry(const std::string& k, const std::string& v) : key(k), value(v) {}
    ScriptConfigEntry(const std::string& k, const std::string& v, const std::string& desc) 
        : key(k), value(v), description(desc) {}
};

// Script execution environment
struct ScriptEnvironment {
    std::string name;
    std::map<std::string, std::string> variables;
    std::map<std::string, std::string> constants;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double totalExecutionTime = 0.0;
    std::string workingDirectory;
    std::map<std::string, std::string> systemInfo;
    
    ScriptEnvironment(const std::string& n = "default") : name(n) {
        startTime = std::chrono::steady_clock::now();
    }
    
    // Get variable value
    std::string getVariable(const std::string& key, const std::string& defaultValue = "") {
        auto it = variables.find(key);
        return (it != variables.end()) ? it->second : defaultValue;
    }
    
    // Set variable
    void setVariable(const std::string& key, const std::string& value) {
        variables[key] = value;
    }
    
    // Get constant
    std::string getConstant(const std::string& key) {
        auto it = constants.find(key);
        return (it != constants.end()) ? it->second : "";
    }
    
    // End timing
    void endTiming() {
        endTime = std::chrono::steady_clock::now();
        totalExecutionTime = std::chrono::duration<double>(endTime - startTime).count();
    }
};

// Parameter template definition
struct ParameterTemplate {
    std::string name;
    std::string type;  // linear, geometric, random, fixed
    std::string start;
    std::string end;
    size_t steps;
    std::string distribution;  // uniform, normal, exponential
    double mean = 0.0;
    double stddev = 1.0;
    std::string seed;
    std::vector<std::string> values;  // predefined values
    
    ParameterTemplate() {}
    ParameterTemplate(const std::string& n, const std::string& t) : name(n), type(t) {}
};

// Script-based configuration for experiments
class ScriptConfig {
public:
    ScriptConfig();
    ~ScriptConfig();
    
    // Load configuration from script file
    bool loadFromScript(const std::string& filename);
    
    // Load configuration from string content
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Add configuration entry
    void addConfigEntry(const ScriptConfigEntry& entry);
    
    // Get configuration value as string
    std::string getString(const std::string& key) const;
    
    // Get configuration value with type conversion
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all configuration entries
    const std::vector<ScriptConfigEntry>& getAllEntries() const { return configEntries; }
    
    // Set environment variable
    void setEnvironmentVariable(const std::string& key, const std::string& value);
    
    // Get environment
    const ScriptEnvironment& getEnvironment() const { return environment; }
    
    // Update environment end time
    void endEnvironmentTiming() { environment.endTiming(); }
    
    // Add parameter template
    void addParameterTemplate(const ParameterTemplate& paramTemplate);
    
    // Get parameter template
    const ParameterTemplate* getParameterTemplate(const std::string& name) const;
    
    // Generate parameter values
    std::vector<std::string> generateParameterValues(const std::string& templateName);
    
    // Resolve template in string
    std::string resolveTemplate(const std::string& templateStr);
    
    // Get script processor instance
    static ScriptConfig& getInstance() {
        static ScriptConfig instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ScriptConfigEntry> configEntries;
    ScriptEnvironment environment;
    std::map<std::string, ParameterTemplate> parameterTemplates;
};

} // namespace nlm