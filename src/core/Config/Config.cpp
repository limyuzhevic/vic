#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::unique_ptr<ConfigValidator>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Add default validator
    pImpl->validators.push_back(std::make_unique<DefaultValidator>());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Config file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        pImpl->entries.clear();
        
        // Process JSON object
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            const nlohmann::json& value = it.value();
            
            // Create ConfigEntry from JSON
            auto entryOpt = ConfigEntry::fromJson(key, value);
            if (entryOpt) {
                // Validate entry
                bool isValid = true;
                for (const auto& validator : pImpl->validators) {
                    if (!validator->validate(key, entryOpt->value)) {
                        NLM_LOG_WARNING("Config validation failed for key '" + key + "': " + 
                                       validator->getErrorMessage(key, entryOpt->value));
                        isValid = false;
                        break;
                    }
                }
                
                if (isValid) {
                    pImpl->entries.push_back(*entryOpt);
                    NLM_LOG_DEBUG("Loaded config key: " + key);
                }
            }
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + 
                    " configuration entries from JSON file: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_WARNING("JSON parse error, falling back to simple key=value format: " + std::string(e.what()));
        
        // Fallback to simple key=value format
        pImpl->entries.clear();
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
                
                // Create entry and validate
                ConfigEntry entry(key, ConfigValue(value), ConfigSource::File);
                bool isValid = true;
                for (const auto& validator : pImpl->validators) {
                    if (!validator->validate(key, entry.value)) {
                        NLM_LOG_WARNING("Config validation failed for key '" + key + "': " + 
                                       validator->getErrorMessage(key, entry.value));
                        isValid = false;
                        break;
                    }
                }
                
                if (isValid) {
                    pImpl->entries.push_back(entry);
                }
            }
        }
        
        return true;
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
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Create JSON structure
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigEntry to JSON
        jsonConfig[entry.key] = configValueToJson(entry.value);
    }
    
    // Write formatted JSON
    file << std::setw(2) << jsonConfig << std::endl;
    
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + 
                " configuration entries to JSON file: " + filepath);
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
    // Validate before setting
    bool isValid = true;
    for (const auto& validator : pImpl->validators) {
        if (!validator->validate(key, value)) {
            NLM_LOG_WARNING("Config validation failed for key '" + key + "': " + 
                           validator->getErrorMessage(key, value));
            isValid = false;
            break;
        }
    }
    
    if (isValid) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) { 
                    oss << arg[i]; if (i < arg.size() - 1) oss << ", "; 
                } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) { 
                    oss << arg[i]; if (i < arg.size() - 1) oss << ", "; 
                } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) { 
                    oss << "\"" << arg[i] << "\""; if (i < arg.size() - 1) oss << ", "; 
                } oss << "]";
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

// JSON serialization implementations
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    
    return j;
}

std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& j) {
    ConfigValue result;
    
    try {
        if (j.is_number_integer()) {
            int64_t val = j.get<int64_t>();
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                result = static_cast<int>(val);
            } else {
                result = val;
            }
        } else if (j.is_number_float()) {
            result = j.get<double>();
        } else if (j.is_boolean()) {
            result = j.get<bool>();
        } else if (j.is_string()) {
            result = j.get<std::string>();
        } else if (j.is_array()) {
            // Check element types to determine vector type
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& elem : j) {
                if (!elem.is_number_integer()) allInts = false;
                if (!elem.is_number_float()) allDoubles = false;
                if (!elem.is_string()) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int> vec;
                vec.reserve(j.size());
                for (const auto& elem : j) {
                    vec.push_back(elem.get<int>());
                }
                result = vec;
            } else if (allDoubles) {
                std::vector<double> vec;
                vec.reserve(j.size());
                for (const auto& elem : j) {
                    vec.push_back(elem.get<double>());
                }
                result = vec;
            } else if (allStrings) {
                std::vector<std::string> vec;
                vec.reserve(j.size());
                for (const auto& elem : j) {
                    vec.push_back(elem.get<std::string>());
                }
                result = vec;
            } else {
                return std::nullopt;  // Cannot convert to homogeneous vector
            }
        } else {
            return std::nullopt;  // Unsupported type
        }
    } catch (const std::exception& e) {
        return std::nullopt;
    }
    
    return result;
}

// ConfigEntry JSON implementations
nlohmann::json ConfigEntry::toJson() const {
    return configValueToJson(value);
}

std::optional<ConfigEntry> ConfigEntry::fromJson(const std::string& key, const nlohmann::json& j) {
    auto valueOpt = jsonToConfigValue(j);
    if (valueOpt) {
        return ConfigEntry(key, *valueOpt, ConfigSource::File);
    }
    return std::nullopt;
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
