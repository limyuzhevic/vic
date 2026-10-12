// ExperimentRegistry - Registry for experiment types
#pragma once

#include "IExperiment.hpp"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <typeindex>

namespace nlm {

class ExperimentRegistry {
public:
    // Type alias for experiment factory function
    using ExperimentFactory = std::function<std::unique_ptr<IExperiment>()>;
    
    // Registration
    template<typename T>
    static bool registerExperiment(const std::string& name,
                                   const std::string& description,
                                   const std::vector<std::string>& tags = {}) {
        std::string normalizedName = normalizeName(name);
        
        if (registry.find(normalizedName) != registry.end()) {
            return false; // Already registered
        }
        
        ExperimentInfo info;
        info.name = name;
        info.description = description;
        info.tags = tags;
        info.typeIndex = typeid(T);
        
        registry[normalizedName] = info;
        return true;
    }
    
    // Lookup
    static std::unique_ptr<IExperiment> createExperiment(const std::string& name);
    static ExperimentInfo* getExperimentInfo(const std::string& name);
    static bool isExperimentRegistered(const std::string& name);
    
    // Discovery
    static std::vector<std::string> getExperimentNames() {
        std::vector<std::string> names;
        for (const auto& pair : registry) {
            names.push_back(pair.second.name);
        }
        return names;
    }
    
    static std::vector<std::string> getExperimentNamesByTag(const std::string& tag) {
        std::vector<std::string> names;
        for (const auto& pair : registry) {
            if (std::find(pair.second.tags.begin(), pair.second.tags.end(), tag) != pair.second.tags.end()) {
                names.push_back(pair.second.name);
            }
        }
        return names;
    }
    
    // Experiment info structure
    struct ExperimentInfo {
        std::string name;
        std::string description;
        std::vector<std::string> tags;
        std::type_index typeIndex;
    };
    
private:
    static std::map<std::string, ExperimentInfo> registry;
    static std::map<std::type_index, ExperimentFactory> factories;
    
    static std::string normalizeName(const std::string& name) {
        std::string result = name;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        // Remove spaces and special characters
        result.erase(std::remove_if(result.begin(), result.end(), 
                                  [](char c) { return !std::isalnum(c); }), result.end());
        return result;
    }
};

} // namespace nlm
