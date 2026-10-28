#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

// JSON library (included from CMakeLists.txt)
#include "nlohmann/json.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load configuration from file (supports JSON and YAML formats)
    // Automatically detects format based on file extension
    bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Cannot open config file: " + filepath);
            return false;
        }
        
        // Determine file format based on extension
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        try {
            if (ext == ".json" || ext == ".cfg" || ext == "" || ext == ".txt") {
                return loadFromJson(file);
            } else if (ext == ".yaml" || ext == ".yml") {
                return loadFromYaml(file);
            } else {
                NLM_LOG_WARNING("Unsupported config file format: " + ext + ". Using simple format.");
                return loadFromSimple(file);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from JSON file
    bool Config::loadFromJson(std::ifstream& file) {
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            
            clear();
            
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    std::string key = it.key();
                    ConfigValue value = jsonToConfigValue(it.value());
                    set(key, value, ConfigSource::File);
                    
                    // Set description if available
                    auto descIt = it->find("_description");
                    if (descIt != it->end() && descIt->is_string()) {
                        getEntry(key).description = descIt->get<std::string>();
                    }
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from YAML file
    bool Config::loadFromYaml(std::ifstream& file) {
        try {
            // TODO PHASE 2: Implement YAML parsing with yaml-cpp
            // For now, fall back to simple format
            return loadFromSimple(file);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from simple key=value file (backward compatibility)
    bool Config::loadFromSimple(std::ifstream& file) {
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
        NLM_LOG_WARNING("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (JSON format)\n";
    file << "# Generated by NLM on " << getCurrentTimestamp() << "\n\n";
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as a comment in JSON
        if (!entry.description.empty()) {
            jsonData[entry.key] = entry.value;
            // Add description as metadata (optional)
            jsonData[entry.key]["_description"] = entry.description;
        } else {
            jsonData[entry.key] = entry.value;
        }
    }
    
    // Write JSON to file with pretty formatting
    file << jsonData.dump(2);
    file << "\n";
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch() % std::chrono::seconds(1)
    );
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count() << " GMT";
    return ss.str();
}

ConfigEntry& Config::getEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Return a dummy entry if not found (should not happen in normal use)
    static ConfigEntry dummy;
    return dummy;
}

const ConfigEntry& Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Return a dummy entry if not found (should not happen in normal use)
    static const ConfigEntry dummy;
    return dummy;
}

ConfigEntry& Config::getEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Return a dummy entry if not found (should not happen in normal use)
    static ConfigEntry dummy;
    return dummy;
}

const ConfigEntry& Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    // Return a dummy entry if not found (should not happen in normal use)
    static const ConfigEntry dummy;
    return dummy;
}

// Helper function to convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    
    return jsonValue;
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

// Helper function to convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    
    return jsonValue;
}

// Helper function to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        if (jsonValue.empty()) {
            return std::vector<std::string>{};
        }
        
        // Try to infer array type
        bool allInts = true;
        bool allFloats = true;
        bool allStrings = true;
        
        for (const auto& item : jsonValue) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number_float()) allFloats = false;
            if (!item.is_string()) allStrings = false;
        }
        
        if (allInts) {
            std::vector<int> intArray;
            for (const auto& item : jsonValue) {
                intArray.push_back(item.get<int>());
            }
            return intArray;
        } else if (allFloats) {
            std::vector<double> floatArray;
            for (const auto& item : jsonValue) {
                floatArray.push_back(item.get<double>());
            }
            return floatArray;
        } else if (allStrings) {
            std::vector<std::string> stringArray;
            for (const auto& item : jsonValue) {
                stringArray.push_back(item.get<std::string>());
            }
            return stringArray;
        } else {
            // Mixed types - treat as strings
            std::vector<std::string> stringArray;
            for (const auto& item : jsonValue) {
                stringArray.push_back(item.get<std::string>());
            }
            return stringArray;
        }
    }
    
    // Fallback - empty string
    return std::string();
}
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
