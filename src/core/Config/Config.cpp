#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate the key-value pair before setting
            if (!validateConfigEntry(key, value)) {
                // Log validation error but continue processing
                // TODO: Add proper error logging
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    // Validate configuration after loading
    return validateConfiguration();
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate the key-value pair before setting
                if (!validateConfigEntry(key, value)) {
                    // Log validation error but continue processing
                    // TODO: Add proper error logging
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate the key-value pair before setting
            if (!validateConfigEntry(key, value)) {
                // Log validation error but continue processing
                // TODO: Add proper error logging
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return validateConfiguration();
}

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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
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

    pImpl->entries.clear();
}

// Configuration validation methods
bool Config::validateConfigEntry(const std::string& key, const std::string& value) {
    // Basic validation: key should not be empty
    if (key.empty()) {
        // TODO: Add error logging
        return false;
    }
    
    // Check for valid key characters (alphanumeric, underscores, dots, hyphens)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            // TODO: Add error logging
            return false;
        }
    }
    
    // Check for potentially dangerous values
    if (value.find("..") != std::string::npos || value.find("//") != std::string::npos) {
        // TODO: Add error logging
        return false;
    }
    
    return true;
}

bool Config::validateConfiguration() const {
    // Check for required parameters
    std::vector<std::string> requiredParams = {
        "brain.neuron_count",
        "simulation.timestep",
        "logging.level"
    };
    
    for (const std::string& param : requiredParams) {
        if (!has(param)) {
            // TODO: Add error logging for missing required parameter
            // TODO: Set reasonable defaults for missing parameters
            return false;
        }
    }
    
    // Validate specific parameter ranges and formats
    if (has("brain.neuron_count")) {
        auto neuronCount = get<int64_t>("brain.neuron_count");
        if (neuronCount && (*neuronCount < 1 || *neuronCount > 1000000)) {
            // TODO: Add error logging for invalid neuron count
            return false;
        }
    }
    
    if (has("simulation.timestep")) {
        auto timestep = get<double>("simulation.timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            // TODO: Add error logging for invalid timestep
            return false;
        }
    }
    
    if (has("logging.level")) {
        auto level = get<std::string>("logging.level");
        if (level) {
            std::string lowerLevel = toLower(*level);
            if (lowerLevel != "debug" && lowerLevel != "info" && 
                lowerLevel != "warning" && lowerLevel != "error") {
                // TODO: Add error logging for invalid log level
                return false;
            }
        }
    }
    
    if (has("environment.width") && has("environment.height")) {
        auto width = get<int64_t>("environment.width");
        auto height = get<int64_t>("environment.height");
        if (width && height && (*width < 1 || *height < 1 || *width > 10000 || *height > 10000)) {
            // TODO: Add error logging for invalid world dimensions
            return false;
        }
    }
    
    return true;
}

// Configuration helper functions
std::string Config::getDefaultForKey(const std::string& key) {
    // Return sensible defaults for common configuration keys
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "brain.neuron_count") return "1000";
    if (lowerKey == "brain.synapse_density") return "0.1";
    if (lowerKey == "brain.connection_probability") return "0.05";
    if (lowerKey == "simulation.timestep") return "0.001";
    if (lowerKey == "logging.level") return "INFO";
    if (lowerKey == "logging.to_file") return "false";
    if (lowerKey == "environment.width") return "10";
    if (lowerKey == "environment.height") return "10";
    if (lowerKey == "environment.vision_width") return "8";
    if (lowerKey == "environment.vision_height") return "8";
    
    return "";
}

std::vector<std::string> Config::getValidationErrors() const {
    // Return list of validation error messages
    // TODO: Implement comprehensive error reporting
    return std::vector<std::string>();
}

bool Config::isValid() const {
    // Check if configuration is valid
    return validateConfiguration();
}
