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

// Configuration validation and helper methods

namespace nlm {

// Validate configuration key
bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration key cannot be empty");
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            NLM_LOG_ERROR("Configuration key \"" + key + "\" contains invalid characters");
            return false;
        }
    }
    
    // Check for reserved keywords
    static const std::vector<std::string> reservedKeywords = {
        "null", "true", "false", "and", "or", "not", "if", "else", "for", "while"
    };
    
    std::string lowerKey = toLower(key);
    if (std::find(reservedKeywords.begin(), reservedKeywords.end(), lowerKey) != reservedKeywords.end()) {
        NLM_LOG_ERROR("Configuration key \"" + key + "\" is a reserved keyword");
        return false;
    }
    
    return true;
}

// Validate configuration value based on expected type
bool Config::validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) const {
    if (value.index() == 0) { // std::string
        const std::string& strValue = std::get<std::string>(value);
        if (strValue.empty() && key.find("count") != std::string::npos) {
            NLM_LOG_WARNING("Configuration \"" + key + "\" has empty string value (may cause issues)");
        }
    } else if (value.index() == 3) { // bool
        const bool& boolValue = std::get<bool>(value);
        if (!boolValue && key.find("enable") != std::string::npos) {
            NLM_LOG_WARNING("Configuration \"" + key + "\" is disabled (may affect functionality)");
        }
    } else if (value.index() == 2) { // double
        const double& doubleValue = std::get<double>(value);
        if (doubleValue < 0.0 && (key.find("rate") != std::string::npos || key.find("ratio") != std::string::npos)) {
            NLM_LOG_WARNING("Configuration \"" + key + "\" has negative value (may indicate configuration error)");
        }
    }
    
    return true;
}

// Check for required configuration keys
std::vector<std::string> Config::validateRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    std::vector<std::string> missingKeys;
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        std::ostringstream oss;
        oss << "Missing required configuration keys: ";
        for (size_t i = 0; i < missingKeys.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << missingKeys[i];
        }
        NLM_LOG_ERROR(oss.str());
    }
    
    return missingKeys;
}

// Get descriptive error message for configuration key
std::string Config::getKeyDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "Unknown configuration key: " + key;
}

// Export configuration to JSON format (Phase 3 enhancement)
std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            }
        }, entry.value);
    }
    
    oss << "\n}";
    return oss.str();
}

// Import configuration from JSON format (Phase 3 enhancement)
bool Config::importFromJSON(const std::string& json) {
    try {
        // Simple JSON parser for key-value pairs
        // This is a simplified implementation for Phase 3
        // In Phase 4, this would use a proper JSON library
        
        std::istringstream iss(json);
        std::string line;
        
        while (std::getline(iss, line)) {
            // Skip whitespace and comments
            line = trim(line);
            if (line.empty() || line.find("//") == 0 || line.find("#") == 0) {
                continue;
            }
            
            // Parse "key": "value" or "key": value format
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from key
            if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            
            // Remove quotes from value if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value based on type
            bool parsed = false;
            
            // Try boolean
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
                parsed = true;
            }
            // Try integer
            else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
                try {
                    set(key, std::stoll(value), ConfigSource::File);
                    parsed = true;
                } catch (...) {
                    // Fall through to string
                }
            }
            // Try floating point
            else {
                size_t dotPos = value.find('.');
                if (dotPos != std::string::npos && std::all_of(value.begin(), value.begin() + dotPos, ::isdigit) &&
                    std::all_of(value.begin() + dotPos + 1, value.end(), ::isdigit)) {
                    try {
                        set(key, std::stod(value), ConfigSource::File);
                        parsed = true;
                    } catch (...) {
                        // Fall through to string
                    }
                }
            }
            
            if (!parsed) {
                // Fall back to string
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error importing JSON configuration: ") + e.what());
        return false;
    }
}

// Validate configuration consistency
bool Config::validateConsistency() const {
    bool isValid = true;
    
    // Check for common configuration patterns
    if (has("brain.neuron_count") && has("brain.connection_probability")) {
        int64_t neuronCount = getOr<int64_t>("brain.neuron_count", 0);
        double connectionProb = getOr<double>("brain.connection_probability", 0.0);
        
        if (neuronCount <= 0 && connectionProb > 0.0) {
            NLM_LOG_ERROR("brain.neuron_count (" + std::to_string(neuronCount) + ") must be positive when connection_probability is set");
            isValid = false;
        }
    }
    
    if (has("plasticity.stdp.enable") && has("plasticity.stdp.learning_rate")) {
        bool stdpEnabled = getOr<bool>("plasticity.stdp.enable", false);
        double stdpLearningRate = getOr<double>("plasticity.stdp.learning_rate", 0.0);
        
        if (stdpEnabled && stdpLearningRate <= 0.0) {
            NLM_LOG_ERROR("plasticity.stdp.learning_rate must be positive when STDP is enabled");
            isValid = false;
        }
    }
    
    return isValid;
}

} // namespace nlm
