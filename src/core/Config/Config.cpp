#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Use the nlohmann_json provided by CMake (should be in the system include path)
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filepath;  // Track original file for reloading
    bool validateOnLoad = true;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set up default configuration with validation
    pImpl->validateOnLoad = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath;
    
    // Try JSON format first (preferred)
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_INFO("JSON parsing failed, falling back to key=value format: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Validate required structure
        if (!j.is_object()) {
            NLM_LOG_ERROR("Invalid JSON: Root element must be an object");
            return false;
        }
        
        // Clear existing configuration
        clear();
        
        // Parse JSON recursively
        parseJSON(j, ConfigSource::File);
        
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with improved error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key at line " + std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse value based on content
        try {
            if (value.empty()) {
                set(key, "", ConfigSource::File);
            } else if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                set(key, boolVal, ConfigSource::File);
            } else {
                // Try to parse as number (int first, then double)
                try {
                    size_t pos_int;
                    long long intVal = std::stoll(value, &pos_int);
                    if (pos_int == value.size()) {
                        set(key, intVal, ConfigSource::File);
                    } else {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::File);
                    }
                } catch (const std::exception&) {
                    // Not a number, treat as string
                    set(key, value, ConfigSource::File);
                }
            }
            
            // Validate critical configuration parameters
            if (pImpl->validateOnLoad) {
                validateKey(key, getValueByKey(key));
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing config key '" + key + "': " + std::string(e.what()) + 
                         " at line " + std::to_string(lineNumber));
        }
    }
    
    return true;
}

void Config::parseJSON(const json& j, ConfigSource source) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Add description from JSON if available
            std::string description = "";
            if (it.value().contains("_description")) {
                description = it.value()["_description"].get<std::string>();
            }
            
            // Parse nested values
            ConfigValue configValue = jsonToConfigValue(it.value());
            set(key, configValue, source);
            
            if (!description.empty()) {
                auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (entry != pImpl->entries.end()) {
                    entry->description = description;
                }
            }
        }
    } else {
        NLM_LOG_ERROR("JSON root element must be an object");
    }
}

ConfigValue Config::jsonToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to determine array type
        if (j.empty()) {
            return std::vector<int>();
        }
        
        // Check if all elements are numbers
        bool allNumbers = std::all_of(j.begin(), j.end(), 
            [](const json& item) { return item.is_number(); });
        if (allNumbers) {
            std::vector<double> doubleVec;
            doubleVec.reserve(j.size());
            for (const auto& item : j) {
                doubleVec.push_back(item.get<double>());
            }
            return doubleVec;
        }
        
        // Check if all elements are integers
        bool allIntegers = std::all_of(j.begin(), j.end(),
            [](const json& item) { return item.is_number_integer(); });
        if (allIntegers) {
            std::vector<int64_t> intVec;
            intVec.reserve(j.size());
            for (const auto& item : j) {
                intVec.push_back(item.get<int64_t>());
            }
            return intVec;
        }
        
        // Check if all elements are strings
        bool allStrings = std::all_of(j.begin(), j.end(),
            [](const json& item) { return item.is_string(); });
        if (allStrings) {
            std::vector<std::string> stringVec;
            stringVec.reserve(j.size());
            for (const auto& item : j) {
                stringVec.push_back(item.get<std::string>());
            }
            return stringVec;
        }
        
        // Mixed types - store as string representation
        std::vector<std::string> mixedVec;
        mixedVec.reserve(j.size());
        for (const auto& item : j) {
            mixedVec.push_back(item.dump());
        }
        return mixedVec;
    } else if (j.is_null()) {
        return std::string("");
    } else {
        // Unknown type - store as string
        return j.dump();
    }
}

void Config::validateKey(const std::string& key, const ConfigValue& value) {
    // Validate critical configuration parameters
    std::string lowerKey = toLower(key);
    
    try {
        if (lowerKey.find("neuron_count") != std::string::npos) {
            int64_t neurons = std::get<int64_t>(value);
            if (neurons <= 0) {
                NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neurons) + ". Must be positive.");
            } else if (neurons > 100000) {
                NLM_LOG_WARNING("Very large neuron_count: " + std::to_string(neurons) + ". May impact performance.");
            }
        } else if (lowerKey.find("connection_probability") != std::string::npos) {
            double prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(prob) + ". Must be between 0.0 and 1.0.");
            }
        } else if (lowerKey.find("simulation_timestep") != std::string::npos) {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be positive.");
            } else if (timestep > 1.0) {
                NLM_LOG_WARNING("Large simulation_timestep: " + std::to_string(timestep) + ". May impact accuracy.");
            }
        }
        
    } catch (const std::bad_variant_access&) {
        // Type validation not applicable for this value
    }
}

ConfigValue Config::getValueByKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    // Return default empty value
    return std::string("");
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Clear any existing configuration from command line (preserve defaults)
    auto originalKeys = getKeys();
    
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
    try {
        // Save as JSON (preferred format)
        json j;
        
        for (const auto& entry : pImpl->entries) {
            json value;
            
            // Convert config value back to JSON
            std::visit([&value](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    json arr = json::array();
                    for (const auto& item : arg) {
                        arr.push_back(item);
                    }
                    value = arr;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json arr = json::array();
                    for (const auto& item : arg) {
                        arr.push_back(item);
                    }
                    value = arr;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json arr = json::array();
                    for (const auto& item : arg) {
                        arr.push_back(item);
                    }
                    value = arr;
                }
            }, entry.value);
            
            // Add to JSON with key
            j[entry.key] = value;
            
            // Add description as metadata (not part of main value)
            if (!entry.description.empty()) {
                j[entry.key]["_description"] = entry.description;
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Write formatted JSON
        file << j.dump(2);  // Pretty print with indentation
        
        NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file: " + std::string(e.what()));
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
        NLM_LOG_ERROR("Type mismatch for key '" + key + "'. Expected different type.");
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
        
        // Validate new entry if validation is enabled
        if (pImpl->validateOnLoad) {
            validateKey(key, value);
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
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
