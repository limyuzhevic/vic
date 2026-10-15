#include "ConfigurationVisualizer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ConfigurationVisualizer::Impl {
    std::shared_ptr<Config> config;
    
    // Configuration data
    std::vector<ConfigEntryData> configEntries;
    std::string currentGroup;
    
    // History tracking for comparison
    std::deque<std::vector<ConfigEntryData>> configHistory;
    size_t maxHistorySize = 10;
    
    // Visualization options
    bool showSource = true;
    bool showDescription = true;
    bool highlightChanges = true;
    bool showValidOnly = false;
    
    Impl() {}
};

ConfigurationVisualizer::ConfigurationVisualizer() : pImpl(new Impl) {}

ConfigurationVisualizer::~ConfigurationVisualizer() = default;

bool ConfigurationVisualizer::initialize() {
    return true;
}

void ConfigurationVisualizer::update(double deltaTime) {
    if (pImpl->config) {
        // Update from actual config if available
        pImpl->configEntries.clear();
        
        std::vector<std::string> keys = pImpl->config->getKeys();
        for (const auto& key : keys) {
            ConfigSource source = ConfigSource::Runtime; // Simplified - would need actual source tracking
            std::string valueStr;
            
            // Convert value to string
            auto valueOpt = pImpl->config->get<std::string>(key);
            if (valueOpt) {
                valueStr = *valueOpt;
            } else {
                auto intOpt = pImpl->config->get<int>(key);
                if (intOpt) {
                    valueStr = std::to_string(*intOpt);
                } else {
                    auto doubleOpt = pImpl->config->get<double>(key);
                    if (doubleOpt) {
                        valueStr = std::to_string(*doubleOpt);
                    } else {
                        valueStr = "[complex]";
                    }
                }
            }
            
            ConfigEntryData entry(key, valueStr, source, "", false, true);
            pImpl->configEntries.push_back(entry);
        }
    }
}

void ConfigurationVisualizer::render() {
    std::cout << "=== Configuration Visualization ===" << std::endl;
    
    std::cout << getConfigSummary() << std::endl;
    
    if (pImpl->configEntries.empty()) {
        std::cout << "  No configuration entries to display" << std::endl;
        return;
    }
    
    std::cout << std::endl << "Configuration Tree:" << std::endl;
    std::string configTree = generateConfigTree();
    std::cout << configTree;
}

void ConfigurationVisualizer::close() {
    pImpl->configEntries.clear();
    pImpl->configHistory.clear();
}

void ConfigurationVisualizer::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

void ConfigurationVisualizer::setConfigGroup(const std::string& groupName) {
    pImpl->currentGroup = groupName;
}

void ConfigurationVisualizer::addConfigEntry(const ConfigEntryData& entry) {
    pImpl->configEntries.push_back(entry);
}

void ConfigurationVisualizer::updateConfigEntry(const std::string& key, const std::string& value, ConfigSource source) {
    for (auto& entry : pImpl->configEntries) {
        if (entry.key == key) {
            entry.valueStr = value;
            entry.source = source;
            entry.isModified = true;
            break;
        }
    }
}

const std::vector<ConfigEntryData>& ConfigurationVisualizer::getConfigEntries() const {
    return pImpl->configEntries;
}

std::string ConfigurationVisualizer::generateConfigTree() const {
    std::stringstream ss;
    
    // Group entries by prefix or category (simple grouping by first word)
    std::unordered_map<std::string, std::vector<ConfigEntryData>> grouped;
    
    for (const auto& entry : pImpl->configEntries) {
        // Simple grouping by first part before underscore
        std::string group = "general";
        size_t underscorePos = entry.key.find('_');
        if (underscorePos != std::string::npos) {
            group = entry.key.substr(0, underscorePos);
        }
        
        grouped[group].push_back(entry);
    }
    
    for (const auto& pair : grouped) {
        ss << "  " << pair.first << ":" << std::endl;
        
        for (const auto& entry : pair.second) {
            std::string prefix = "    ";
            std::string sourceStr = "";
            
            if (pImpl->showSource) {
                sourceStr = " [" + std::to_string(static_cast<int>(entry.source)) + "]";
            }
            
            if (pImpl->highlightChanges && entry.isModified) {
                prefix = "    >>> ";
            }
            
            if (!pImpl->showValidOnly || entry.isValid) {
                ss << prefix << entry.key << ": " << entry.valueStr << sourceStr;
                
                if (pImpl->showDescription && !entry.description.empty()) {
                    ss << " - " << entry.description;
                }
                
                ss << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::string ConfigurationVisualizer::generateConfigComparison(const std::vector<ConfigEntryData>& before,
                                                             const std::vector<ConfigEntryData>& after) const {
    std::stringstream ss;
    
    ss << "Configuration Comparison:" << std::endl;
    
    // Build maps for easy lookup
    std::unordered_map<std::string, const ConfigEntryData*> beforeMap;
    for (const auto& entry : before) {
        beforeMap[entry.key] = &entry;
    }
    
    std::unordered_map<std::string, const ConfigEntryData*> afterMap;
    for (const auto& entry : after) {
        afterMap[entry.key] = &entry;
    }
    
    // Check all keys from both sets
    std::vector<std::string> allKeys;
    for (const auto& pair : beforeMap) allKeys.push_back(pair.first);
    for (const auto& pair : afterMap) {
        if (beforeMap.find(pair.first) == beforeMap.end()) {
            allKeys.push_back(pair.first);
        }
    }
    
    for (const auto& key : allKeys) {
        auto beforeIt = beforeMap.find(key);
        auto afterIt = afterMap.find(key);
        
        if (beforeIt == beforeMap.end()) {
            ss << "  NEW: " << key << " = " << afterIt->second->valueStr << std::endl;
        } else if (afterIt == afterMap.end()) {
            ss << "  REMOVED: " << key << " = " << beforeIt->second->valueStr << std::endl;
        } else {
            if (beforeIt->second->valueStr != afterIt->second->valueStr) {
                ss << "  CHANGED: " << key << std::endl;
                ss << "    Before: " << beforeIt->second->valueStr << std::endl;
                ss << "    After:  " << afterIt->second->valueStr << std::endl;
            } else {
                ss << "  UNCHANGED: " << key << " = " << beforeIt->second->valueStr << std::endl;
            }
        }
    }
    
    return ss.str();
}

std::string ConfigurationVisualizer::getConfigSummary() const {
    std::stringstream ss;
    
    ss << "Configuration Summary:" << std::endl;
    ss << "  Total Entries: " << pImpl->configEntries.size() << std::endl;
    
    if (pImpl->configEntries.empty()) return ss.str();
    
    // Count by source
    std::unordered_map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->configEntries) {
        sourceCount[entry.source]++;
    }
    
    ss << "  Entries by Source:" << std::endl;
    for (const auto& pair : sourceCount) {
        ss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Count modified
    size_t modifiedCount = 0;
    for (const auto& entry : pImpl->configEntries) {
        if (entry.isModified) modifiedCount++;
    }
    
    ss << "  Modified Entries: " << modifiedCount << std::endl;
    ss << "  Valid Entries: " << std::count_if(pImpl->configEntries.begin(), pImpl->configEntries.end(),
                                             [](const ConfigEntryData& e) { return e.isValid; }) << std::endl;
    
    // Save current state to history
    if (pImpl->highlightChanges) {
        pImpl->configHistory.push_back(pImpl->configEntries);
        if (pImpl->configHistory.size() > pImpl->maxHistorySize) {
            pImpl->configHistory.pop_front();
        }
    }
    
    return ss.str();
}

} // namespace nlm
