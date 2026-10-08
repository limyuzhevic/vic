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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
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
    // Try to write as JSON format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        // Write the actual value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJsonString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float> || 
                                std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << escapeJsonString(arg[j]) << "\"";
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << "null";  // Default for unknown types
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        }
    }
    
    file << "\n}\n";
    
    if (file.good()) {
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to write configuration to file: " + filepath);
        return false;
    }
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
    // Basic validation
    if (key.empty()) {
        NLM_LOG_WARNING("Attempt to set empty configuration key");
        return;
    }
    
    // Validate value type based on key
    validateKeyValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Add validation method
void Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Convert key to lowercase for validation
    std::string lowerKey = toLower(key);
    
    // Define validation rules for specific keys
    if (lowerKey == "random_seed" || lowerKey == "neuron_count" || 
        lowerKey == "region_count" || lowerKey == "max_steps") {
        // Should be integer values
        if (!std::holds_alternative<int>(value) && 
            !std::holds_alternative<int64_t>(value)) {
            NLM_LOG_WARNING("Configuration key \"" + key + 
                "\" should be an integer, got " + valueTypeToString(value));
        }
    }
    else if (lowerKey == "connection_probability" || 
             lowerKey == "plasticity_learning_rate" || 
             lowerKey == "stdp_ltp_weight" || 
             lowerKey == "stdp_ltd_weight" || 
             lowerKey == "stdp_time_constant" ||
             lowerKey == "simulation_timestep" ||
             lowerKey == "reward_discount_factor") {
        // Should be floating point values
        if (!std::holds_alternative<double>(value) && 
            !std::holds_alternative<float>(value)) {
            NLM_LOG_WARNING("Configuration key \"" + key + 
                "\" should be a floating point number, got " + valueTypeToString(value));
        }
    }
    else if (lowerKey == "enable_" || lowerKey == "enabled" ||
             lowerKey == "visualization_enabled") {
        // Should be boolean values
        if (!std::holds_alternative<bool>(value)) {
            NLM_LOG_WARNING("Configuration key \"" + key + 
                "\" should be a boolean, got " + valueTypeToString(value));
        }
    }
}

// Helper to convert value type to string
std::string Config::valueTypeToString(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, value);
    return "unknown";
}

// Enhanced has method with case-insensitive search
bool Config::has(const std::string& key) const {
    std::string lowerKey = toLower(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&lowerKey](const ConfigEntry& e) { return toLower(e.key) == lowerKey; });
}

// Enhanced getOr method with better error handling
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    // Log missing key with default
    NLM_LOG_INFO("Using default value for configuration key \"" + key + 
        "\": " + std::to_string(defaultValue));
    
    return defaultValue;
}

// Enhanced summary method with more details
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort entries by key for consistent output
    std::vector<const ConfigEntry*> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.push_back(&entry);
    }
    
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const ConfigEntry* a, const ConfigEntry* b) {
            return a->key < b->key;
        });
    
    for (const auto* entry : sortedEntries) {
        oss << "  " << entry->key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJsonString(arg) << "\"";
            } else {
                oss << arg;
            }
        }, entry->value);
        oss << "] (" << static_cast<int>(entry->source) << ") - " << entry->description << "\n";
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
