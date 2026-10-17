// NLM Script Configuration
// Implementation of configuration management for script execution

#include "ScriptConfig.hpp"
#include "script/ScriptParser.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct ScriptConfig::Impl {
    std::string lastError;
    bool isLoaded = false;
    std::string sourceFile;
    std::string sourceFormat;
    std::chrono::steady_clock::time_point loadTime;
};

ScriptConfig::ScriptConfig() : pImpl(std::make_unique<Impl>()) {
    pImpl->loadTime = std::chrono::steady_clock::now();
}

ScriptConfig::~ScriptConfig() = default;

bool ScriptConfig::loadFromScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open script file: " + filename;
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    pImpl->sourceFile = filename;
    pImpl->sourceFormat = "auto";
    
    bool result = loadFromString(buffer.str(), "auto");
    if (result) {
        pImpl->isLoaded = true;
        NLM_LOG_INFO("Loaded script configuration from: " + filename);
    }
    
    return result;
}

bool ScriptConfig::loadFromString(const std::string& content, const std::string& format) {
    // Clear previous config
    configEntries.clear();
    environment.variables.clear();
    environment.constants.clear();
    parameterTemplates.clear();
    
    // Parse script content
    ScriptParser parser;
    ParseResult parseResult;
    
    if (format == "auto") {
        parseResult = parser.parseString(content, "auto");
    } else {
        parseResult = parser.parseString(content, format);
    }
    
    if (!parseResult.valid) {
        pImpl->lastError = "Failed to parse script content: " + parseResult.errorMessage;
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
    
    // Extract configuration from parsed AST (simplified implementation)
    // In a real implementation, this would traverse the AST and populate configEntries
    
    NLM_LOG_INFO("Parsed script configuration (" + std::to_string(configEntries.size()) + " entries)");
    return true;
}

void ScriptConfig::addConfigEntry(const ScriptConfigEntry& entry) {
    configEntries.push_back(entry);
    NLM_LOG_INFO("Added config entry: " + entry.key + " = " + entry.value);
}

std::string ScriptConfig::getString(const std::string& key) const {
    for (const auto& entry : configEntries) {
        if (entry.key == key) {
            return entry.value;
        }
    }
    return "";
}

template<typename T>
T ScriptConfig::getValue(const std::string& key, const T& defaultValue) const {
    // Simple implementation - try to convert to target type
    std::string strValue = getString(key);
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(strValue);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(strValue);
        } else if constexpr (std::is_same_v<T, bool>) {
            return strValue == "true" || strValue == "1" || strValue == "yes";
        } else {
            return static_cast<T>(stdValue);  // Fallback
        }
    } catch (...) {
        return defaultValue;
    }
}

bool ScriptConfig::has(const std::string& key) const {
    for (const auto& entry : configEntries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ScriptConfig::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : configEntries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void ScriptConfig::setEnvironmentVariable(const std::string& key, const std::string& value) {
    environment.variables[key] = value;
    NLM_LOG_INFO("Set environment variable: " + key + " = " + value);
}

void ScriptConfig::addParameterTemplate(const ParameterTemplate& paramTemplate) {
    parameterTemplates[paramTemplate.name] = paramTemplate;
    NLM_LOG_INFO("Added parameter template: " + paramTemplate.name);
}

const ParameterTemplate* ScriptConfig::getParameterTemplate(const std::string& name) const {
    auto it = parameterTemplates.find(name);
    return (it != parameterTemplates.end()) ? &it->second : nullptr;
}

std::vector<std::string> ScriptConfig::generateParameterValues(const std::string& templateName) {
    const ParameterTemplate* paramTemplate = getParameterTemplate(templateName);
    if (!paramTemplate) {
        NLM_LOG_ERROR("Parameter template not found: " + templateName);
        return {};
    }
    
    std::vector<std::string> values;
    
    if (paramTemplate->type == "linear") {
        values = generateLinearProgression(paramTemplate->start, paramTemplate->end, paramTemplate->steps);
    } else if (paramTemplate->type == "geometric") {
        values = generateGeometricProgression(paramTemplate->start, paramTemplate->end, paramTemplate->steps);
    } else if (paramTemplate->type == "random") {
        values = generateRandomSampling(paramTemplate->steps, paramTemplate->seed);
    } else if (!paramTemplate->values.empty()) {
        values = paramTemplate->values;
    }
    
    NLM_LOG_INFO("Generated " + std::to_string(values.size()) + " values for template: " + templateName);
    return values;
}

std::string ScriptConfig::resolveTemplate(const std::string& templateStr) {
    std::string result = templateStr;
    
    // Replace parameter templates
    for (const auto& pair : environment.variables) {
        std::string placeholder = "${" + pair.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

// Helper function for linear progression
std::vector<std::string> ScriptConfig::generateLinearProgression(const std::string& startStr, 
                                                                   const std::string& endStr, 
                                                                   size_t steps) {
    try {
        double start = std::stod(startStr);
        double end = std::stod(endStr);
        return ScriptProcessor::generateLinearValues(start, end, steps);
    } catch (...) {
        return {};
    }
}

// Helper function for geometric progression
std::vector<std::string> ScriptConfig::generateGeometricProgression(const std::string& startStr,
                                                                     const std::string& endStr,
                                                                     size_t steps) {
    try {
        double start = std::stod(startStr);
        double end = std::stod(endStr);
        auto values = ScriptProcessor::generateGeometricValues(start, end, steps);
        
        std::vector<std::string> result;
        for (double val : values) {
            result.push_back(std::to_string(val));
        }
        return result;
    } catch (...) {
        return {};
    }
}

// Helper function for random sampling
std::vector<std::string> ScriptConfig::generateRandomSampling(size_t count, const std::string& seed) {
    return ScriptProcessor::generateRandomSampling(count, seed);
}

} // namespace nlm