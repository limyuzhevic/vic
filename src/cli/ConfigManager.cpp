#include "ConfigManager.hpp"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

ConfigManager::ConfigManager() {
    setupDefaultConfig();
}

bool ConfigManager::loadConfig(const std::string& filename) {
    if (filename.empty()) {
        std::cerr << "ERROR: No config file specified" << std::endl;
        return false;
    }
    
    std::filesystem::path path(filename);
    if (!std::filesystem::exists(path)) {
        std::cerr << "ERROR: Config file not found: " << filename << std::endl;
        return false;
    }
    
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == ".json") {
        success = parseJsonFile(filename);
    } else if (extension == ".cfg" || extension == ".txt") {
        success = parseCfgFile(filename);
    } else {
        std::cerr << "ERROR: Unsupported config file format: " << extension << std::endl;
        std::cerr << "Supported formats: .json, .cfg, .txt" << std::endl;
        return false;
    }
    
    if (success) {
        std::cout << "Configuration loaded from: " << filename << std::endl;
    } else {
        std::cerr << "ERROR: Failed to parse config file: " << filename << std::endl;
    }
    
    return success;
}

bool ConfigManager::saveConfig(const std::string& filename) const {
    std::filesystem::path path(filename);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot write to file: " << filename << std::endl;
        return false;
    }
    
    if (extension == ".json") {
        json j;
        // Convert config to JSON (simplified)
        for (const auto& entry : config_.getKeys()) {
            auto value = config_.get<std::string>(entry);
            if (value) {
                j[entry] = *value;
            }
        }
        file << j.dump(2);
    } else {
        // Save as .cfg file format
        for (const auto& key : config_.getKeys()) {
            auto value = config_.get<std::string>(entry);
            if (value) {
                file << key << " = " << *value << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Configuration saved to: " << filename << std::endl;
    return true;
}

bool ConfigManager::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 7) == "--config") {
            std::string value = extractConfigValue(arg.substr(7));
            if (!value.empty()) {
                loadConfig(value);
            }
        } else if (arg.substr(0, 10) == "--output") {
            std::string value = extractConfigValue(arg.substr(10));
            if (!value.empty()) {
                setOutputDir(value);
            }
        } else if (arg.substr(0, 7) == "--param") {
            std::string value = extractConfigValue(arg.substr(7));
            if (!value.empty()) {
                std::string paramFile = value;
                // Load parameter overrides
                std::ifstream paramStream(paramFile);
                if (paramStream.is_open()) {
                    std::string line;
                    while (std::getline(paramStream, line)) {
                        size_t equalsPos = line.find('=');
                        if (equalsPos != std::string::npos) {
                            std::string key = trim(line.substr(0, equalsPos));
                            std::string val = trim(line.substr(equalsPos + 1));
                            paramOverrides_[key] = val;
                        }
                    }
                    paramStream.close();
                }
            }
        } else if (arg.substr(0, 2) == "--") {
            // Parameter override
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos && equalsPos > 2) {
                std::string key = trim(arg.substr(2, equalsPos - 2));
                std::string value = trim(arg.substr(equalsPos + 1));
                paramOverrides_[key] = value;
            }
        }
    }
    
    return true;
}

void ConfigManager::printHelp(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options] <command> [arguments]" << std::endl;
    std::cout << std::endl;
    std::cout << "Global Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --output <dir>          Set output directory" << std::endl;
    std::cout << "  --param <file>          Load parameter overrides" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Commands:" << std::endl;
    std::cout << "  run                     Run a brain simulation experiment" << std::endl;
    std::cout << "  brain                   Manage brain instances and configuration" << std::endl;
    std::cout << "  config                  Load and save configuration files" << std::endl;
    std::cout << "  experiment              Run and manage experiments" << std::endl;
    std::cout << "  demo                    Run Phase 6 integration demo" << std::endl;
    std::cout << "  integration             Test system integration" << std::endl;
    std::cout << "  verify                  Verify all systems are connected" << std::endl;
    std::cout << "  benchmark               Run performance benchmarks" << std::endl;
    std::cout << "  compare                 Compare different brain configurations" << std::endl;
    std::cout << "  analyze                 Analyze brain state and metrics" << std::endl;
    std::cout << std::endl;
}

void ConfigManager::printVersion() const {
    std::cout << "NLM (熙然) - Phase 6.0" << std::endl;
    std::cout << "Neural Learning Machine - Final Integration" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  Phase: 6 - FINAL INTEGRATION" << std::endl;
    std::cout << "  Status: Complete with experimental subsystems" << std::endl;
    std::cout << "  Language: C++20" << std::endl;
    std::cout << "  License: MIT" << std::endl;
}

bool ConfigManager::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config_.set(key, value, ConfigSource::File);
        }
    }
    
    file.close();
    return true;
}

bool ConfigManager::parseJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            std::string value = it.value().dump();
            config_.set(key, value, ConfigSource::File);
        }
        
        file.close();
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::parseCfgFile(const std::string& filename) {
    return parseConfigFile(filename);
}

void ConfigManager::setupDefaultConfig() {
    // Set default values for NLM Phase 6
    config_.set("random_seed", 42, ConfigSource::Default);
    config_.set("simulation_timestep", 0.001, ConfigSource::Default);
    config_.set("neuron_count", 1000, ConfigSource::Default);
    config_.set("region_count", 1, ConfigSource::Default);
    config_.set("connection_probability", 0.1f, ConfigSource::Default);
    config_.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config_.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config_.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Phase 6 specific defaults
    config_.set("enable_checkpointing", true, ConfigSource::Default);
    config_.set("enable_replay", true, ConfigSource::Default);
    config_.set("enable_development", true, ConfigSource::Default);
    config_.set("checkpoint_path", "./checkpoint_test.bin", ConfigSource::Default);
}

std::string ConfigManager::extractConfigValue(const std::string& str) {
    size_t pos = str.find('=');
    if (pos != std::string::npos) {
        return trim(str.substr(pos + 1));
    }
    return "";
}

std::string ConfigManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
