// Configuration file support for NLM
// Advanced configuration management for better customization

#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Configuration file parser and manager
class ConfigFileManager {
public:
    struct ConfigFileSection {
        std::string name;
        std::map<std::string, std::string> values;
        std::vector<ConfigFileSection> subsections;
        
        ConfigFileSection() {}
        ConfigFileSection(const std::string& sectionName) : name(sectionName) {}
    };
    
    ConfigFileManager() : defaultFormat_("nml") {}
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename, std::shared_ptr<Config> config) {
        if (!config) return false;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse file content
        std::vector<ConfigFileSection> sections = parseFileContent(file);
        applyConfiguration(config, sections);
        
        return true;
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filename, std::shared_ptr<const Config> config) {
        if (!config) return false;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration in appropriate format
        writeConfiguration(file, config);
        
        return true;
    }
    
    // Merge configuration files (file2 overrides file1)
    bool mergeConfigFiles(const std::string& primaryFile, const std::string& secondaryFile, 
                        std::shared_ptr<Config> config) {
        // Load primary configuration
        if (!loadFromFile(primaryFile, config)) {
            return false;
        }
        
        // Load secondary configuration and merge
        std::vector<ConfigFileSection> secondarySections;
        {
            std::ifstream file(secondaryFile);
            if (!file.is_open()) {
                return true;  // Primary only is OK
            }
            secondarySections = parseFileContent(file);
        }
        
        applyConfiguration(config, secondarySections, true);  // Override
        return true;
    }
    
    // Validate configuration file
    std::vector<std::string> validateConfigFile(const std::string& filename) {
        std::vector<std::string> errors;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            errors.push_back("File not found: " + filename);
            return errors;
        }
        
        std::vector<ConfigFileSection> sections = parseFileContent(file);
        
        // Validate section names
        for (const auto& section : sections) {
            if (section.name.empty()) {
                errors.push_back("Empty section name found");
            }
            
            // Validate key-value pairs
            for (const auto& pair : section.values) {
                if (pair.first.empty()) {
                    errors.push_back("Empty key found in section: " + section.name);
                }
                if (pair.second.empty()) {
                    errors.push_back("Empty value for key: " + pair.first + " in section: " + section.name);
                }
            }
        }
        
        return errors;
    }
    
    // Get configuration file format
    std::string getFormat() const {
        return defaultFormat_;
    }
    
    // Set configuration file format
    void setFormat(const std::string& format) {
        defaultFormat_ = format;
    }
    
    // Get supported file extensions
    std::vector<std::string> getSupportedExtensions() const {
        return {
            "nml",   // Native NLM format
            "json",  // JSON format
            "cfg",   // Configuration file format
            "ini",   // Initialization file format
            "toml"   // TOML format
        };
    }
    
    // Check if file format is supported
    bool isFormatSupported(const std::string& filename) const {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) {
            return false;
        }
        
        std::string extension = filename.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        return std::find(getSupportedExtensions().begin(), 
                        getSupportedExtensions().end(), extension) != getSupportedExtensions().end();
    }
    
private:
    void applyConfiguration(std::shared_ptr<Config> config, const std::vector<ConfigFileSection>& sections, bool override = false) {
        for (const auto& section : sections) {
            for (const auto& pair : section.values) {
                try {
                    if (override || !config->has(pair.first)) {
                        // Try to parse as float first
                        try {
                            float value = std::stof(pair.second);
                            config->set(pair.first, value);
                        } catch (...) {
                            // Try to parse as int
                            try {
                                size_t value = std::stoul(pair.second);
                                config->set(pair.first, static_cast<int64_t>(value));
                            } catch (...) {
                                // Try to parse as string
                                config->set(pair.first, pair.second);
                            }
                        }
                    }
                } catch (...) {
                    // Skip invalid configuration entries
                    continue;
                }
            }
            
            // Apply subsection configuration
            if (!section.subsections.empty()) {
                applyConfiguration(config, section.subsections, override);
            }
        }
    }
    
    void writeConfiguration(std::ofstream& file, std::shared_ptr<const Config> config) {
        // Write configuration in native NLM format
        file << "# NLM Configuration File\n";
        file << "# Generated automatically\n\n";
        
        // Write configuration sections
        file << "[GENERAL]\n";
        
        // Try to get all configuration values
        std::vector<std::pair<std::string, std::string>> configValues;
        
        // Common configuration parameters
        std::vector<std::string> commonParams = {
            "neuron_count", "region_count", "connection_probability",
            "random_seed", "simulation_timestep",
            "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
            "synaptogenesis_rate", "pruning_rate"
        };
        
        for (const auto& param : commonParams) {
            if (config->has(param)) {
                if (auto intValue = config->get<int64_t>(param)) {
                    configValues.emplace_back(param, std::to_string(*intValue));
                } else if (auto floatValue = config->get<float>(param)) {
                    configValues.emplace_back(param, std::to_string(*floatValue));
                } else if (auto stringValue = config->get<std::string>(param)) {
                    configValues.emplace_back(param, *stringValue);
                }
            }
        }
        
        // Write configuration values
        for (const auto& pair : configValues) {
            file << pair.first << " = " << pair.second << "\n";
        }
        
        file << "\n[ADVANCED]\n";
        file << "# Advanced configuration options\n";
        
        // Write any remaining configuration options
        file << "# Additional options can be added here\n";
    }
    
    std::vector<ConfigFileSection> parseFileContent(std::ifstream& file) {
        std::vector<ConfigFileSection> sections;
        ConfigFileSection currentSection;
        
        std::string line;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for section headers
            if (line[0] == '[' && line.back() == ']') {
                // Save current section
                if (!currentSection.name.empty()) {
                    sections.push_back(currentSection);
                }
                
                // Start new section
                currentSection = ConfigFileSection(line.substr(1, line.size() - 2));
                continue;
            }
            
            // Parse key-value pairs
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos && equalsPos > 0) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim whitespace from key and value
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                if (!key.empty()) {
                    currentSection.values[key] = value;
                }
            }
        }
        
        // Add the last section
        if (!currentSection.name.empty()) {
            sections.push_back(currentSection);
        }
        
        return sections;
    }
    
    std::string defaultFormat_;
};

} // namespace nlm