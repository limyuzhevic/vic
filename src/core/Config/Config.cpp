#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <stdexcept>

// Try to include nlohmann/json for JSON parsing
// If not available, we'll fall back to the simple parser
#if __has_include(<nlohmann/json.hpp>)
    #include <nlohmann/json.hpp>
    #define NLM_HAS_JSON 1
#else
    #define NLM_HAS_JSON 0
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#if NLM_HAS_JSON
// Helper function to convert JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_null()) {
        return std::string();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Convert array to appropriate vector type based on element type
        const auto& elements = j.get<std::vector<nlohmann::json>>();
        if (!elements.empty()) {
            if (elements[0].is_boolean()) {
                std::vector<bool> vec;
                vec.reserve(elements.size());
                for (const auto& elem : elements) {
                    vec.push_back(elem.get<bool>());
                }
                return vec;
            } else if (elements[0].is_number_integer()) {
                std::vector<int64_t> vec;
                vec.reserve(elements.size());
                for (const auto& elem : elements) {
                    vec.push_back(elem.get<int64_t>());
                }
                return vec;
            } else if (elements[0].is_number_unsigned()) {
                std::vector<uint64_t> vec;
                vec.reserve(elements.size());
                for (const auto& elem : elements) {
                    vec.push_back(elem.get<uint64_t>());
                }
                return vec;
            } else if (elements[0].is_number()) {
                std::vector<double> vec;
                vec.reserve(elements.size());
                for (const auto& elem : elements) {
                    vec.push_back(elem.get<double>());
                }
                return vec;
            } else {
                std::vector<std::string> vec;
                vec.reserve(elements.size());
                for (const auto& elem : elements) {
                    vec.push_back(elem.get<std::string>());
                }
                return vec;
            }
        }
        // Empty array - default to vector<string>
        return std::vector<std::string>();
    }
    
    // Fallback for unknown types
    return j.get<std::string>();
}

// Helper function to convert ConfigValue to JSON value
static nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<uint64_t>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        } else {
            return nlohmann::json(arg);
        }
    }, value);
}
#endif

bool Config::loadFromFile(const std::string& filepath) {
    // Try to use JSON parser if available
#if NLM_HAS_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            // Fall back to simple parser
            return loadFromFileSimple(filepath);
        }
        
        nlohmann::json j;
        file >> j;
        
        if (!j.is_object()) {
            std::cerr << "ERROR: JSON file must contain a top-level object" << std::endl;
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to parse JSON file '" << filepath << "': " << e.what() << std::endl;
        // Fall back to simple parser
        return loadFromFileSimple(filepath);
    }
#else
    // Use simple parser if JSON not available
    return loadFromFileSimple(filepath);
#endif
}

// Simple key=value parser implementation
bool Config::loadFromFileSimple(const std::string& filepath) {
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
    // Try to use JSON output if available
#if NLM_HAS_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        
        // Convert configuration entries to JSON
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = configValueToJson(entry.value);
        }
        
        // Write JSON with pretty formatting
        file << j.dump(2);
        
        NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to write JSON file '" << filepath << "': " << e.what() << std::endl;
        return false;
    }
#else
    // Use simple format if JSON not available
    return saveToFileSimple(filepath);
#endif
}

// Simple save implementation
bool Config::saveToFileSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string based on type
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
        
        file << "\n\n";
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

} // namespace nlm
