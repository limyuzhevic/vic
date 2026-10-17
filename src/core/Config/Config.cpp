#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first, fall back to key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON parsing if the file appears to be JSON
    if (isJsonContent(content)) {
        return loadFromJson(content, filepath);
    }
    // Try YAML parsing if the file appears to be YAML
    else if (isYamlContent(content)) {
        return loadFromYaml(content, filepath);
    }
    // Fall back to simple key=value format for backward compatibility
    else {
        return loadFromKeyValue(content, filepath);
    }
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Add header comment
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value properly
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Helper method for current timestamp
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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
    // Validate the value before setting
    if (!validate(key, value)) {
        NLM_LOG_WARNING("Validation failed for configuration key: " + key);
        // Still set the value but log the warning
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation for known configuration keys
    if (key == "random_seed") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0) {
                NLM_LOG_WARNING("random_seed should be non-negative: " + std::to_string(seed));
                return false;
            }
        } else if (std::holds_alternative<int>(value)) {
            int seed = std::get<int>(value);
            if (seed < 0) {
                NLM_LOG_WARNING("random_seed should be non-negative: " + std::to_string(seed));
                return false;
            }
        }
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0 || timestep > 1.0) {
                NLM_LOG_WARNING("simulation_timestep should be between 0 and 1.0: " + std::to_string(timestep));
                return false;
            }
        }
    } else if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                NLM_LOG_WARNING("neuron_count should be positive: " + std::to_string(count));
                return false;
            }
        }
    } else if (key == "region_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                NLM_LOG_WARNING("region_count should be positive: " + std::to_string(count));
                return false;
            }
        }
    } else if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_WARNING("connection_probability should be between 0.0 and 1.0: " + std::to_string(prob));
                return false;
            }
        }
    } else if (key == "dopamine_baseline") {
        if (std::holds_alternative<double>(value)) {
            double baseline = std::get<double>(value);
            if (baseline < 0.0 || baseline > 1.0) {
                NLM_LOG_WARNING("dopamine_baseline should be between 0.0 and 1.0: " + std::to_string(baseline));
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            errors.push_back("Invalid configuration value: " + entry.key + " = " + 
                           (entry.description.empty() ? "(no description)" : entry.description));
        }
    }
    
    return errors;
}

void Config::clearValidationErrors() {
    // Currently just a placeholder for future expansion
    // Could clear any cached validation state
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

// Helper methods for format detection
bool Config::isJsonContent(const std::string& content) const {
    // TODO PHASE 3: Implement proper JSON format detection
    return false;
}

bool Config::isYamlContent(const std::string& content) const {
    // TODO PHASE 3: Implement proper YAML format detection
    return false;
}

bool Config::loadFromJson(const std::string& content, const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON parsing using a library like nlohmann::json
    // For now, fall back to key=value format for backward compatibility
    return loadFromKeyValue(content, filepath);
}

bool Config::loadFromYaml(const std::string& content, const std::string& filepath) {
    // TODO PHASE 2: Implement proper YAML parsing using a library like yaml-cpp
    // For now, fall back to key=value format for backward compatibility
    return loadFromKeyValue(content, filepath);
}

bool Config::loadFromKeyValue(const std::string& content, const std::string& filepath) {
    std::stringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + " (key=value format)");
    return true;
}
