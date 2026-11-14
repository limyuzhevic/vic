#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON format)
    bool loadFromFile(const std::string& filepath) {
        // Determine file format by extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Default to simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    }
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonData = nlohmann::json::parse(file);
            
            // If it's an object, treat as key-value pairs
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    std::string key = it.key();
                    
                    // Convert JSON value to ConfigValue
                    ConfigValue value;
                    const auto& jsonVal = it.value();
                    
                    if (jsonVal.is_string()) {
                        value = jsonVal.get<std::string>();
                    } else if (jsonVal.is_number_integer()) {
                        value = jsonVal.get<int64_t>();
                    } else if (jsonVal.is_number_unsigned()) {
                        value = jsonVal.get<uint64_t>();
                    } else if (jsonVal.is_number()) {
                        value = jsonVal.get<double>();
                    } else if (jsonVal.is_boolean()) {
                        value = jsonVal.get<bool>();
                    } else if (jsonVal.is_array()) {
                        // Determine array type based on first element
                        if (!jsonVal.empty()) {
                            if (jsonVal[0].is_number_integer()) {
                                value = jsonVal.get<std::vector<int64_t>>();
                            } else if (jsonVal[0].is_number()) {
                                value = jsonVal.get<std::vector<double>>();
                            } else {
                                value = jsonVal.get<std::vector<std::string>>();
                            }
                        } else {
                            value = jsonVal.get<std::vector<std::string>>();
                        }
                    } else {
                        // Unsupported type, skip this entry
                        continue;
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Simple value, convert to string
                set("value", jsonData.dump(), ConfigSource::File);
            }
        } catch (const nlohmann::json::parse_error& e) {
            return false;
        }
        
        return true;
    }
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath) {
        // For now, treat as simple format since YAML parser would require additional dependency
        // TODO: Implement proper YAML parsing when pyyaml or other YAML library is available
        return loadFromSimpleFormat(filepath);
    }
    
    // Load from simple key=value format
    bool loadFromSimpleFormat(const std::string& filepath) {
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
    
    // Load from command line arguments
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

// Save to file (JSON format by default, simple format for backwards compatibility)
    bool saveToFile(const std::string& filepath) const {
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return saveToJSON(filepath);
        } else {
            // Default to simple key=value format for backwards compatibility
            return saveToSimpleFormat(filepath);
        }
    }
    
    // Save to JSON file
    bool saveToJSON(const std::string& filepath) const {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
        return true;
    }
    
    // Save to simple key=value format
    bool saveToSimpleFormat(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert value to string representation
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, int64_t> ||
                                    std::is_same_v<T, uint64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]\n";
                } else {
                    file << entry.key << " = UNKNOWN_TYPE\n";
                }
            }, entry.value);
        }
        
        return true;
    }
    
    // Validate configuration
    bool Config::validate() const {
        std::vector<std::string> errors = getValidationErrors();
        return errors.empty();
    }
    
    // Validate specific key
    bool Config::validateKey(const std::string& key) const {
        std::vector<std::string> errors = getValidationErrors();
        for (const auto& error : errors) {
            if (error.find(key) != std::string::npos) {
                return false;
            }
        }
        return true;
    }
    
    // Get validation errors
    std::vector<std::string> Config::getValidationErrors() const {
        std::vector<std::string> errors;
        
        // Check for required keys
        const std::vector<std::string> requiredKeys = {
            "random_seed",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                errors.push_back("Missing required configuration key: '" + key + "'");
            }
        }
        
        // Validate specific key values
        if (has("neuron_count")) {
            auto neuronCountOpt = get<uint64_t>("neuron_count");
            if (neuronCountOpt && (*neuronCountOpt == 0 || *neuronCountOpt > 100000)) {
                errors.push_back("Invalid neuron_count: must be between 1 and 100000");
            }
        }
        
        if (has("region_count")) {
            auto regionCountOpt = get<uint64_t>("region_count");
            if (regionCountOpt && (*regionCountOpt == 0 || *regionCountOpt > 1000)) {
                errors.push_back("Invalid region_count: must be between 1 and 1000");
            }
        }
        
        if (has("connection_probability")) {
            auto connProbOpt = get<double>("connection_probability");
            if (connProbOpt && (*connProbOpt < 0.0 || *connProbOpt > 1.0)) {
                errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }
    
    // Load from command line arguments
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
