#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Configuration validation rules
struct ConfigValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

class ConfigValidator {
public:
    ConfigValidator() {
        // Add validation rules for known configuration keys
        validationRules.push_back({
            "random_seed",
            [](const ConfigValue& val) {
                if (std::holds_alternative<int64_t>(val)) {
                    auto seed = std::get<int64_t>(val);
                    return seed >= 0;
                } else if (std::holds_alternative<int>(val)) {
                    auto seed = std::get<int>(val);
                    return seed >= 0;
                }
                return false;
            },
            "random_seed must be a non-negative integer"
        });
        
        validationRules.push_back({
            "simulation_timestep",
            [](const ConfigValue& val) {
                if (std::holds_alternative<double>(val)) {
                    auto timestep = std::get<double>(val);
                    return timestep > 0.0 && timestep < 1.0;
                }
                return false;
            },
            "simulation_timestep must be a positive double less than 1.0"
        });
        
        validationRules.push_back({
            "neuron_count",
            [](const ConfigValue& val) {
                if (std::holds_alternative<int64_t>(val)) {
                    auto count = std::get<int64_t>(val);
                    return count > 0 && count < 100000;
                }
                return false;
            },
            "neuron_count must be a positive int64_t less than 100000"
        });
        
        validationRules.push_back({
            "region_count",
            [](const ConfigValue& val) {
                if (std::holds_alternative<int64_t>(val)) {
                    auto count = std::get<int64_t>(val);
                    return count > 0 && count < 100;
                }
                return false;
            },
            "region_count must be a positive int64_t less than 100"
        });
        
        validationRules.push_back({
            "connection_probability",
            [](const ConfigValue& val) {
                if (std::holds_alternative<double>(val)) {
                    auto prob = std::get<double>(val);
                    return prob >= 0.0 && prob <= 1.0;
                }
                return false;
            },
            "connection_probability must be a double between 0.0 and 1.0"
        });
    }
    
    std::vector<std::string> validate(const ConfigValue& value, const std::string& key) const {
        std::vector<std::string> errors;
        for (const auto& rule : validationRules) {
            if (rule.key == key && !rule.validator(value)) {
                errors.push_back(rule.errorMessage);
            }
        }
        return errors;
    }
    
private:
    std::vector<ConfigValidationRule> validationRules;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filePath;  // Store file path for save operations
};

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
    
    // Store file path for potential save operations
    pImpl->filePath = filepath;
    
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

// Validate current configuration
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    static const ConfigValidator validator;
    
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> validationErrors = validator.validate(entry.value, entry.key);
        errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
    }
    
    return errors;
}

// Get file path for loading
std::string Config::getFilePath() const {
    return pImpl->filePath;
}
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
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
    // Validate the value if this is a known configuration key
    static const ConfigValidator validator;
    std::vector<std::string> validationErrors = validator.validate(value, key);
    if (!validationErrors.empty()) {
        std::ostringstream errorMsg;
        errorMsg << "Configuration validation failed for key '" << key << "':";
        for (const auto& error : validationErrors) {
            errorMsg << " " << error;
        }
        // Log the error but continue - validation warnings instead of hard failures
        std::cerr << "WARNING: " << errorMsg.str() << std::endl;
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
// Get file path for loading
std::string Config::getFilePath() const {
    return pImpl->filePath;
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
