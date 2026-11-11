// Advanced Features Header
// Includes scripting, experimental modes, and advanced user controls

#pragma once

#include <string>
#include <vector>
#include <any>
#include <functional>
#include <unordered_map>

namespace nlm {

// Script execution context
struct ScriptContext {
    std::shared_ptr<class Brain> brain;
    std::shared_ptr<class Config> config;
    std::unordered_map<std::string, std::any> variables;
    size_t currentStep;
    float currentTime;
    
    ScriptContext(std::shared_ptr<class Brain> b, std::shared_ptr<class Config> c) 
        : brain(b), config(c), currentStep(0), currentTime(0.0f) {}
};

// Experimental mode configurations
struct ExperimentalConfig {
    std::string name;
    std::string description;
    std::function<bool(ScriptContext&)> condition;
    std::function<void(ScriptContext&)> setup;
    std::function<void(ScriptContext&)> execute;
    std::function<void(ScriptContext&)> cleanup;
    bool enabled;
};

class AdvancedFeatures {
private:
    std::vector<ExperimentalConfig> experiments;
    std::unordered_map<std::string, std::function<void(ScriptContext&)>> scripts;
    bool scriptingEnabled;
    bool experimentalMode;
    std::string currentExperiment;
    
public:
    AdvancedFeatures();
    
    void enableScripting(bool enable);
    void enableExperimentalMode(bool enable);
    void addExperiment(const ExperimentalConfig& experiment);
    void addScript(const std::string& name, std::function<void(ScriptContext&)> script);
    bool runExperiment(const std::string& name);
    bool executeScript(const std::string& name);
    void listExperiments() const;
    void listScripts() const;
    
private:
    ScriptContext createScriptContext();
    void initializeBuiltInExperiments();
    void initializeBuiltInScripts();
};

} // namespace nlm
