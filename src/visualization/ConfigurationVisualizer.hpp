#pragma once

#include "VisualizationInterface.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

// Configuration entry visualization data
struct ConfigEntryData {
    std::string key;
    std::string valueStr;
    ConfigSource source;
    std::string description;
    bool isModified;
    bool isValid;
    
    ConfigEntryData() : source(ConfigSource::Default), isModified(false), isValid(true) {}
    ConfigEntryData(const std::string& k, const std::string& v, ConfigSource s, 
                   const std::string& desc = "", bool modified = false, bool valid = true)
        : key(k), valueStr(v), source(s), description(desc), isModified(modified), isValid(valid) {}
};

// Configuration visualization
class ConfigurationVisualizer : public VisualizationInterface {
public:
    ConfigurationVisualizer();
    ~ConfigurationVisualizer() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Set configuration reference
    void setConfig(std::shared_ptr<Config> config);
    
    // Set configuration group for focused view
    void setConfigGroup(const std::string& groupName);
    
    // Add configuration entry
    void addConfigEntry(const ConfigEntryData& entry);
    
    // Update configuration entry
    void updateConfigEntry(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration entry data
    const std::vector<ConfigEntryData>& getConfigEntries() const;
    
    // Generate configuration tree visualization
    std::string generateConfigTree() const;
    
    // Generate configuration comparison (before/after)
    std::string generateConfigComparison(const std::vector<ConfigEntryData>& before, 
                                        const std::vector<ConfigEntryData>& after) const;
    
    // Get configuration summary
    std::string getConfigSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
