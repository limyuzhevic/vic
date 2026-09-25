// Test configuration system
// Configuration for NLM test framework
#pragma once

#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {
namespace test {

// Test configuration structure
struct TestConfig {
    std::string name;
    std::string description;
    TestCategory category;
    int timeoutMilliseconds;
    std::string testCommand;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> dependencies;
    bool enabled;
    double minPerformanceThreshold;
    size_t memoryLimitMB;
    std::chrono::nanoseconds maxExecutionTime;
    
    TestConfig(const std::string& testName, const std::string& testDesc, TestCategory testCat)
        : name(testName), description(testDesc), category(testCat), 
          timeoutMilliseconds(300000), // 5 minutes default
          testCommand(""), enabled(true), minPerformanceThreshold(0.0), 
          memoryLimitMB(1024), maxExecutionTime(std::chrono::seconds(300)) {}
};

// Test parameters system
class TestParameters {
public:
    struct Parameter {
        std::string name;
        std::string type; // "int", "double", "string", "bool"
        std::string value;
        std::string description;
        std::vector<std::string> allowedValues;
        bool required;
    };
    
    std::string testName;
    std::vector<Parameter> parameters;
    std::map<std::string, std::string> overrides;
    
    TestParameters(const std::string& testName) : testName(testName) {}
    
    template<typename T>
    void addParameter(const std::string& name, const T& value, 
                     const std::string& description = "", bool required = false) {
        Parameter param;
        param.name = name;
        param.type = getTypeName<T>();
        param.value = convertToString(value);
        param.description = description;
        param.required = required;
        
        parameters.push_back(param);
    }
    
    template<typename T>
    void addParameterWithOptions(const std::string& name, const T& defaultValue,
                                 const std::vector<std::string>& options,
                                 const std::string& description = "", bool required = false) {
        Parameter param;
        param.name = name;
        param.type = getTypeName<T>();
        param.value = convertToString(defaultValue);
        param.description = description;
        param.required = required;
        param.allowedValues = options;
        
        parameters.push_back(param);
    }
    
    T getParameter<T>(const std::string& name) const {
        auto it = overrides.find(name);
        if (it != overrides.end()) {
            return convertFromString<T>(it->second);
        }
        
        for (const auto& param : parameters) {
            if (param.name == name) {
                return convertFromString<T>(param.value);
            }
        }
        
        throw std::runtime_error("Parameter not found: " + name);
    }
    
    void setParameter(const std::string& name, const std::string& value) {
        overrides[name] = value;
    }
    
    std::string getParameterString(const std::string& name) const {
        auto it = overrides.find(name);
        if (it != overrides.end()) {
            return it->second;
        }
        
        for (const auto& param : parameters) {
            if (param.name == name) {
                return param.value;
            }
        }
        
        throw std::runtime_error("Parameter not found: " + name);
    }
    
    std::string generateCommand() const {
        std::stringstream command;
        command << testCommand;
        
        for (const auto& param : parameters) {
            std::string value = getParameterString(param.name);
            command << " --" << param.name << " " << value;
        }
        
        for (const auto& override : overrides) {
            // Check if parameter is already in parameters list
            bool found = false;
            for (const auto& param : parameters) {
                if (param.name == override.first) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                command << " --" << override.first << " " << override.second;
            }
        }
        
        return command.str();
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Test: " << testName << std::endl;
        ss << "Parameters:" << std::endl;
        
        for (const auto& param : parameters) {
            ss << "  " << param.name << " (" << param.type << ")";
            if (!param.description.empty()) {
                ss << " - " << param.description;
            }
            ss << " = " << param.value << std::endl;
        }
        
        if (!overrides.empty()) {
            ss << "Overrides:" << std::endl;
            for (const auto& override : overrides) {
                ss << "  " << override.first << " = " << override.second << std::endl;
            }
        }
        
        return ss.str();
    }
    
    bool validate() const {
        for (const auto& param : parameters) {
            if (param.required) {
                if (overrides.find(param.name) == overrides.end() && 
                    std::find(overrides.begin(), overrides.end(), param.name) == overrides.end()) {
                    return false;
                }
            }
            
            if (!param.allowedValues.empty()) {
                auto it = overrides.find(param.name);
                std::string value = (it != overrides.end()) ? it->second : param.value;
                if (std::find(param.allowedValues.begin(), param.allowedValues.end(), value) == param.allowedValues.end()) {
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    template<typename T>
    std::string getTypeName() {
        if (std::is_same<T, int>::value) return "int";
        if (std::is_same<T, double>::value) return "double";
        if (std::is_same<T, std::string>::value) return "string";
        if (std::is_same<T, bool>::value) return "bool";
        return "unknown";
    }
    
    template<typename T>
    std::string convertToString(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
    
    template<typename T>
    T convertFromString(const std::string& str) {
        std::istringstream iss(str);
        T value;
        iss >> value;
        return value;
    }
}; 

// Test configuration manager
class TestConfigManager {
public:
    struct ConfigEntry {
        std::string testName;
        TestConfig config;
        std::chrono::system_clock::time_point lastModified;
        int version;
    };
    
private:
    std::map<std::string, ConfigEntry> configs;
    std::string configFile;
    
public:
    TestConfigManager(const std::string& configFile = "test_config.ini") : configFile(configFile) {
        loadConfig();
    }
    
    void addConfig(const TestConfig& config) {
        ConfigEntry entry;
        entry.testName = config.name;
        entry.config = config;
        entry.lastModified = std::chrono::system_clock::now();
        entry.version = 1;
        
        configs[config.name] = entry;
        saveConfig();
    }
    
    bool updateConfig(const TestConfig& config) {
        auto it = configs.find(config.name);
        if (it == configs.end()) {
            return false;
        }
        
        it->second.config = config;
        it->second.lastModified = std::chrono::system_clock::now();
        it->second.version++;
        
        saveConfig();
        return true;
    }
    
    TestConfig getConfig(const std::string& testName) const {
        auto it = configs.find(testName);
        if (it == configs.end()) {
            throw std::runtime_error("Test configuration not found: " + testName);
        }
        return it->second.config;
    }
    
    bool hasConfig(const std::string& testName) const {
        return configs.find(testName) != configs.end();
    }
    
    std::vector<TestConfig> getAllConfigs() const {
        std::vector<TestConfig> result;
        for (const auto& pair : configs) {
            result.push_back(pair.second.config);
        }
        return result;
    }
    
    bool removeConfig(const std::string& testName) {
        auto it = configs.find(testName);
        if (it == configs.end()) {
            return false;
        }
        
        configs.erase(it);
        saveConfig();
        return true;
    }
    
    void loadConfig() {
        std::ifstream file(configFile);
        if (!file) {
            // Create default config if file doesn't exist
            createDefaultConfig();
            return;
        }
        
        std::string line;
        std::string currentSection;
        TestConfig currentConfig("", "", TestCategory::Unit);
        
        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line[0] == '[' && line.back() == ']') {
                // Section header
                if (!currentSection.empty()) {
                    configs[currentSection].config = currentConfig;
                }
                
                currentSection = line.substr(1, line.size() - 2);
                currentConfig = TestConfig("", "", TestCategory::Unit);
            } else if (line.find('=') != std::string::npos) {
                // Key-value pair
                size_t pos = line.find('=');
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                if (key == "name") {
                    currentConfig.name = value;
                } else if (key == "description") {
                    currentConfig.description = value;
                } else if (key == "category") {
                    currentConfig.category = stringToCategory(value);
                } else if (key == "timeout") {
                    currentConfig.timeoutMilliseconds = std::stoi(value);
                } else if (key == "enabled") {
                    currentConfig.enabled = (value == "true" || value == "1" || value == "yes");
                }
            }
        }
        
        if (!currentSection.empty()) {
            configs[currentSection].config = currentConfig;
        }
    }
    
    void saveConfig() {
        std::ofstream file(configFile);
        if (!file) return;
        
        for (const auto& pair : configs) {
            const auto& entry = pair.second;
            const auto& config = entry.config;
            
            file << "[" << config.name << "]" << std::endl;
            file << "name = " << config.name << std::endl;
            file << "description = " << config.description << std::endl;
            file << "category = " << categoryToString(config.category) << std::endl;
            file << "timeout = " << config.timeoutMilliseconds << std::endl;
            file << "enabled = " << (config.enabled ? "true" : "false") << std::endl;
            file << std::endl;
        }
    }
    
    void createDefaultConfig() {
        // Create default test configurations
        TestConfig brainTest("brain_test", "Brain system integration test", TestCategory::Integration);
        brainTest.timeoutMilliseconds = 300000;
        brainTest.enabled = true;
        configs["brain_test"].config = brainTest;
        
        TestConfig performanceTest("performance_test", "Performance benchmark test", TestCategory::Performance);
        performanceTest.timeoutMilliseconds = 600000;
        performanceTest.enabled = true;
        configs["performance_test"].config = performanceTest;
        
        TestConfig memoryTest("memory_test", "Memory leak detection test", TestCategory::Memory);
        memoryTest.timeoutMilliseconds = 900000;
        memoryTest.enabled = true;
        configs["memory_test"].config = memoryTest;
        
        TestConfig stressTest("stress_test", "Stress testing for large networks", TestCategory::Stress);
        stressTest.timeoutMilliseconds = 1200000;
        stressTest.enabled = true;
        configs["stress_test"].config = stressTest;
    }
    
    std::string categoryToString(TestCategory category) {
        switch (category) {
            case TestCategory::Unit: return "unit";
            case TestCategory::Integration: return "integration";
            case TestCategory::Performance: return "performance";
            case TestCategory::Stress: return "stress";
            case TestCategory::Regression: return "regression";
            case TestCategory::Memory: return "memory";
            case TestCategory::Concurrency: return "concurrency";
            case TestCategory::Accuracy: return "accuracy";
            case TestCategory::Scaling: return "scaling";
            default: return "unit";
        }
    }
    
    TestCategory stringToCategory(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "unit") return TestCategory::Unit;
        if (lower == "integration") return TestCategory::Integration;
        if (lower == "performance") return TestCategory::Performance;
        if (lower == "stress") return TestCategory::Stress;
        if (lower == "regression") return TestCategory::Regression;
        if (lower == "memory") return TestCategory::Memory;
        if (lower == "concurrency") return TestCategory::Concurrency;
        if (lower == "accuracy") return TestCategory::Accuracy;
        if (lower == "scaling") return TestCategory::Scaling;
        
        return TestCategory::Unit;
    }
};

}}} // namespace nlm::test
