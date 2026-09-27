#include "BrainConfigTool.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace nlm {
namespace cli {

BrainConfigTool::BrainConfigTool() {
    currentConfigPath = "brain_config.json";
}

BrainConfigTool::~BrainConfigTool() {
}

bool BrainConfigTool::LoadConfig(const std::string& configPath) {
    try {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file: " << configPath << std::endl;
            return false;
        }
        
        std::string line;
        std::string currentKey;
        std::string currentValue;
        bool inKey = false;
        
        while (std::getline(file, line)) {
            // Simple key=value parsing
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                currentKey = line.substr(0, pos);
                currentValue = line.substr(pos + 1);
                configData[currentKey] = currentValue;
            } else if (!line.empty() && line[0] != '#') {
                // Add lines without = as separate entries
                configData[line] = "";
            }
        }
        
        currentConfigPath = configPath;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::SaveConfig(const std::string& configPath) const {
    try {
        std::ofstream file(configPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file for writing: " << configPath << std::endl;
            return false;
        }
        
        for (const auto& pair : configData) {
            if (pair.second.empty()) {
                file << pair.first << std::endl;
            } else {
                file << pair.first << "=" << pair.second << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::ValidateConfig() const {
    // Basic validation checks
    if (configData.empty()) {
        std::cerr << "Config is empty" << std::endl;
        return false;
    }
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {"brain.name", "brain.type"};
    for (const auto& key : requiredKeys) {
        if (configData.find(key) == configData.end()) {
            std::cerr << "Missing required config key: " << key << std::endl;
            return false;
        }
    }
    
    return true;
}

bool BrainConfigTool::BackupConfig(const std::string& backupPath) const {
    try {
        return SaveConfig(backupPath);
    } catch (const std::exception& e) {
        std::cerr << "Error backing up config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::RestoreConfig(const std::string& backupPath) {
    return LoadConfig(backupPath);
}

bool BrainConfigTool::CompareConfigs(const std::string& configPath1, const std::string& configPath2) const {
    try {
        // Simple implementation - load both configs and compare
        std::ifstream file1(configPath1);
        std::ifstream file2(configPath2);
        
        if (!file1.is_open() || !file2.is_open()) {
            return false;
        }
        
        // Compare line by line
        std::string line1, line2;
        int lineNum = 1;
        while (std::getline(file1, line1) && std::getline(file2, line2)) {
            if (line1 != line2) {
                std::cout << "Difference at line " << lineNum << ":" << std::endl;
                std::cout << "  File1: " << line1 << std::endl;
                std::cout << "  File2: " << line2 << std::endl;
            }
            lineNum++;
        }
        
        // Check if one file is longer than the other
        if (std::getline(file1, line1)) {
            std::cout << "File1 is longer at line " << lineNum << std::endl;
            return false;
        }
        if (std::getline(file2, line2)) {
            std::cout << "File2 is longer at line " << lineNum << std::endl;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error comparing configs: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::OptimizeConfig() {
    // Simple optimization - remove empty values
    bool changed = false;
    for (auto it = configData.begin(); it != configData.end(); ) {
        if (it->second.empty()) {
            it = configData.erase(it);
            changed = true;
        } else {
            it++;
        }
    }
    return changed;
}

bool BrainConfigTool::UpdateConfig(const std::string& key, const std::string& value) {
    configData[key] = value;
    return true;
}

std::string BrainConfigTool::GetConfigValue(const std::string& key) const {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return it->second;
    }
    return "";
}

bool BrainConfigTool::CreateBrainConfig(const std::string& brainPath) {
    try {
        // Create a basic brain config
        configData.clear();
        configData["brain.name"] = "New Brain";
        configData["brain.type"] = "neural_network";
        configData["brain.version"] = "1.0.0";
        configData["brain.created"] = "2026-01-01";
        
        currentConfigPath = brainPath + "/config.json";
        return SaveConfig(currentConfigPath);
    } catch (const std::exception& e) {
        std::cerr << "Error creating brain config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::InitializeBrainConfig(const std::string& brainPath, const std::string& initData) {
    try {
        // Basic initialization
        std::ifstream initFile(initData);
        if (!initFile.is_open()) {
            std::cerr << "Failed to open initialization data: " << initData << std::endl;
            return false;
        }
        
        std::string line;
        std::vector<std::string> initLines;
        while (std::getline(initFile, line)) {
            if (!line.empty()) {
                initLines.push_back(line);
            }
        }
        
        configData["brain.init_data"] = initLines.size() > 0 ? initLines[0] : "default";
        configData["brain.init_timestamp"] = "2026-01-01T00:00:00Z";
        
        currentConfigPath = brainPath + "/config.json";
        return SaveConfig(currentConfigPath);
    } catch (const std::exception& e) {
        std::cerr << "Error initializing brain config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::ExportConfig(const std::string& outputPath, const std::string& format) const {
    try {
        if (format == "json") {
            // Convert to JSON-like format
            std::ofstream file(outputPath);
            if (!file.is_open()) {
                return false;
            }
            
            file << "{\n";
            bool first = true;
            for (const auto& pair : configData) {
                if (!first) file << ",\n";
                file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
                first = false;
            }
            file << "\n}\n";
        } else if (format == "csv") {
            // Convert to CSV
            std::ofstream file(outputPath);
            if (!file.is_open()) {
                return false;
            }
            
            file << "key,value\n";
            for (const auto& pair : configData) {
                file << pair.first << "," << pair.second << "\n";
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting config: " << e.what() << std::endl;
        return false;
    }
}

bool BrainConfigTool::ImportConfig(const std::string& inputPath, const std::string& format) {
    try {
        if (format == "json") {
            std::ifstream file(inputPath);
            if (!file.is_open()) {
                return false;
            }
            
            // Simple JSON parsing (limited to simple key:value pairs)
            std::string line;
            while (std::getline(file, line)) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(1, colonPos - 2); // Remove "  "
                    std::string value = line.substr(colonPos + 3); // Remove ": "
                    size_t quotePos = value.find('"');
                    if (quotePos != std::string::npos) {
                        value = value.substr(0, quotePos);
                    }
                    configData[key] = value;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing config: " << e.what() << std::endl;
        return false;
    }
}

} // namespace cli
} // namespace nlm