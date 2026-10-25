// Config.cpp - Enhanced Configuration System Implementation
// Improved error handling, validation, and robustness
// Fallback to simple format if nlohmann/json is not available

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Try to include nlohmann/json, fallback to simple format if not available
#if __has_include(<nlohmann/json.hpp>)
    #include <nlohmann/json.hpp>
    #define NLM_USE_JSON 1
#else
    #define NLM_USE_JSON 0
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register built-in validators
    registerValidator("neuron_count", [](const ConfigValue& value) {
        if constexpr (std::is_same_v<decltype(value), int64_t>) {
            return value >= 0 && value <= 1000000;
        }
        return false;
    });
    
    registerValidator("synapse_density", [](const ConfigValue& value) {
        if constexpr (std::is_same_v<decltype(value), double>) {
            return value >= 0.0 && value <= 1.0;
        }
        return false;
    });
    
    registerValidator("connection_probability", [](const ConfigValue& value) {
        if constexpr (std::is_same_v<decltype(value), double>) {
            return value >= 0.0 && value <= 1.0;
        }
        return false;
    });
    
    registerValidator("simulation_timestep", [](const ConfigValue& value) {
        if constexpr (std::is_same_v<decltype(value), double>) {
            return value > 0.0 && value <= 1.0;
        }
        return false;
    });
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
#if NLM_USE_JSON
    try {
        auto json = nlohmann::json::parse(file);
        clear();
        
        // Process JSON configuration
        if (json.contains("config")) {
            const auto& config_json = json["config"];
            for (auto it = config_json.begin(); it != config_json.end(); ++it) {
                std::string key = it.key();
                const auto& value = it.value();
                
                ConfigSource source = ConfigSource::File;
                if (json.contains("_source") && json["_source"].is_string()) {
                    std::string source_str = json["_source"].get<std::string>();
                    if (source_str == "defaults") source = ConfigSource::Default;
                    else if (source_str == "command_line") source = ConfigSource::CommandLine;
                    else if (source_str == "runtime") source = ConfigSource::Runtime;
                }
                
                setFromJson(key, value, source);
            }
        } else {
            // Simple key=value format as fallback
            file.clear();
            file.seekg(0);
            return loadFromSimpleFormat(file, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        // JSON parsing failed, try simple format
        file.clear();
        file.seekg(0);
        return loadFromSimpleFormat(file, ConfigSource::File);
    }
#else
    // Use simple format only
    return loadFromSimpleFormat(file, ConfigSource::File);
#endif
}

bool Config::loadFromSimpleFormat(std::ifstream& file, ConfigSource source) {
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(line_number) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, source);
    }
    
    return true;
}

#if NLM_USE_JSON
void Config::setFromJson(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Convert array to vector
        std::vector<double> vec;
        for (const auto& item : value) {
            vec.push_back(item.get<double>());
        }
        set(key, vec, source);
    } else {
        throw std::runtime_error("Unsupported JSON type for key: " + key);
    }
}
#endif

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
                NLM_LOG_INFO("Config from command line: " + key + " = " + value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Config from command line: " + key + " = " + value);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::ctime(&std::time(nullptr));
    file << "\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        file << entry.key << " = ";
        visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to file: " + filepath);
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
        T value = std::get<T>(it->value);
        
        // Run validator if exists
        auto validator_it = pImpl->validators.find(key);
        if (validator_it != pImpl->validators.end()) {
            if (!validator_it->second(value)) {
                NLM_LOG_WARNING("Configuration validation failed for key: " + key + ". Value out of range.");
            }
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch when getting configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value
    auto validator_it = pImpl->validators.find(key);
    if (validator_it != pImpl->validators.end()) {
        if (!validator_it->second(value)) {
            throw std::runtime_error("Configuration validation failed for key: " + key);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    NLM_LOG_DEBUG("Configuration set: " + key + " = " + getValueString(value));
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

void Config::set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << std::left << std::setw(20) << entry.key << " = ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << " (" << static_cast<int>(entry.source) << ")\n";
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

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

void Config::registerValidator(const std::string& key, 
                               std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = std::move(validator);
}

bool Config::validateKey(const std::string& key) const {
    auto validator_it = pImpl->validators.find(key);
    if (validator_it == pImpl->validators.end()) {
        return true;  // No validator, so it's valid
    }
    
    auto entry_it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry_it == pImpl->entries.end()) {
        return true;  // Key doesn't exist yet, so it's valid
    }
    
    return validator_it->second(entry_it->value);
}

std::vector<std::string> Config::getInvalidKeys() const {
    std::vector<std::string> invalid_keys;
    
    for (const auto& validator : pImpl->validators) {
        auto entry_it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&validator](const ConfigEntry& e) { return e.key == validator.first; });
        
        if (entry_it != pImpl->entries.end()) {
            if (!validator.second(entry_it->value)) {
                invalid_keys.push_back(validator.first);
            }
        }
    }
    
    return invalid_keys;
}

bool Config::validateAll() const {
    return getInvalidKeys().empty();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int64_t>> Config::get<std::vector<int64_t>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
