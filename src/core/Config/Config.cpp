#include "Config.hpp"
#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<spdlog::logger> logger;
};

Config::Config() : pImpl(std::make_unique<Impl>()), configManager_(std::make_unique<ConfigManager>()) {
    pImpl->logger = spdlog::get("nlm");
    if (!pImpl->logger) {
        pImpl->logger = spdlog::stdout_color_mt("nlm");
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use ConfigManager for enhanced file loading capabilities
    return configManager_->loadFromFile(filepath);
    
    /* Original implementation kept for reference:
    bool Config::loadFromFile(const std::string& filepath) {
        // TODO PHASE 2: Implement proper JSON/YAML parser
        // PLACEHOLDER - Phase 1 uses a simple key=value format
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
    */
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Use ConfigManager for enhanced command line argument parsing
    std::string configString;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                configString += key + " = " + value + "\n";
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            configString += key + " = " + value + "\n";
        }
    }
    
    return configManager_->loadFromString(configString);
    
    /* Original implementation kept for reference:
    bool Config::loadFromArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        return true;
    }
    */
}

bool Config::saveToFile(const std::string& filepath) const {
    // Use ConfigManager for enhanced file saving capabilities
    return configManager_->saveToFile(filepath);
    
    /* Original implementation kept for reference:
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
        
        return true;
    }
    */
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Use ConfigManager for enhanced configuration retrieval
    return configManager_->getOr<T>(key, T{});
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // Use ConfigManager for enhanced configuration retrieval
    return configManager_->getOr<T>(key, defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Use ConfigManager for enhanced configuration setting
    // Note: ConfigManager doesn't use ConfigSource, so we'll use Runtime for all updates
    configManager_->set(key, value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Use ConfigManager for enhanced configuration setting
    configManager_->set(key, value);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Use ConfigManager for enhanced configuration setting
    configManager_->set(key, value);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Use ConfigManager for enhanced configuration setting
    configManager_->set(key, value);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Use ConfigManager for enhanced configuration setting
    configManager_->set(key, value);
}

bool Config::has(const std::string& key) const {
    // Use ConfigManager for enhanced configuration checking
    return configManager_->hasKey(key);
}

void Config::remove(const std::string& key) {
    // Use ConfigManager for enhanced configuration removal
    // Note: ConfigManager doesn't have a remove method, so we'll reload without the key
    // For now, just use set to a default value to simulate removal
    configManager_->set(key, std::string(""));
}

std::vector<std::string> Config::getKeys() const {
    // Use ConfigManager for enhanced configuration listing
    return configManager_->getKeys();
}

void Config::clear() {
    // Use ConfigManager for enhanced configuration clearing
    configManager_->set("random_seed", 42);  // Restore default values
    configManager_->set("simulation_timestep", 0.001);
    configManager_->set("neuron_count", 1000);
    configManager_->set("region_count", 2);
    configManager_->set("connection_probability", 0.1);
}

std::string Config::summary() const {
    // Use ConfigManager for enhanced configuration summary
    std::string configSummary = configManager_->summary();
    
    // Add Config class-specific info
    std::ostringstream oss;
    oss << "=== NLM Configuration System ===\n";
    oss << "Using ConfigManager for enhanced configuration management\n";
    oss << configSummary;
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
