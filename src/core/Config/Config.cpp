#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string fileFormat;  // "text", "json", "yaml"
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->fileFormat = "text";  // Default to text format for backward compatibility
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON format)
    bool loadFromFile(const std::string& filepath) {
        // Determine file format by extension
        std::string extension = "";
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            extension = filepath.substr(dotPos + 1);
        }
        
        // Auto-detect format
        if (extension == "json" || extension == "JSON") {
            return loadFromJSONFile(filepath);
        } else if (extension == "yaml" || extension == "yml") {
            return loadFromYAMLFile(filepath);
        } else {
            // Default to text format for backward compatibility
            return loadFromTextFile(filepath);
        }
    }

    bool loadFromTextFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for reading: " + filepath);
            return false;
        }
        
        pImpl->fileFormat = "text";
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_ERROR("Invalid config line in " + filepath + " at line " + std::to_string(lineNumber) + ": expected '='");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Empty key in config file " + filepath + " at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value based on key
            if (key == "random_seed" || key == "neuron_count" || key == "region_count") {
                // Validate numeric keys
                try {
                    size_t pos2 = value.find('.');
                    if (pos2 != std::string::npos) {
                        // Float value
                        float floatVal = std::stof(value);
                        if (floatVal < 0.0f) {
                            NLM_LOG_ERROR("Negative value for " + key + " in " + filepath + " at line " + std::to_string(lineNumber));
                        }
                    } else {
                        // Integer value
                        int64_t intVal = std::stoll(value);
                        if (intVal < 0) {
                            NLM_LOG_ERROR("Negative value for " + key + " in " + filepath + " at line " + std::to_string(lineNumber));
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Invalid value for " + key + " in " + filepath + " at line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    }

bool Config::loadFromTextFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->fileFormat = "text";
    
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->fileFormat = "json";
    
    try {
        json data;
        file >> data;
        
        // Convert JSON to Config entries
        if (data.is_object()) {
            for (auto it = data.begin(); it != data.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                const auto& value = it.value();
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle array types
                    json arr = value;
                    if (arr.empty()) {
                        // Default to empty string vector
                        configValue = std::vector<std::string>();
                    } else {
                        // Try to infer array type
                        bool allInt = std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_number_integer(); });
                        bool allDouble = std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_number_float(); });
                        bool allString = std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_string(); });
                        
                        if (allInt) {
                            std::vector<int64_t> intArray;
                            for (const auto& v : arr) intArray.push_back(v.get<int64_t>());
                            configValue = intArray;
                        } else if (allDouble) {
                            std::vector<double> doubleArray;
                            for (const auto& v : arr) doubleArray.push_back(v.get<double>());
                            configValue = doubleArray;
                        } else if (allString) {
                            std::vector<std::string> stringArray;
                            for (const auto& v : arr) stringArray.push_back(v.get<std::string>());
                            configValue = stringArray;
                        } else {
                            // Mixed type - use string array
                            std::vector<std::string> stringArray;
                            for (const auto& v : arr) stringArray.push_back(v.dump());
                            configValue = stringArray;
                        }
                    }
                } else {
                    // Fallback to string representation
                    configValue = value.dump();
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // For YAML support, we would need to include a YAML library
    // For now, fallback to text format
    NLM_LOG_WARNING("YAML format not yet implemented, falling back to text format for " + filepath);
    return loadFromTextFile(filepath);
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine output format by extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // Try to save in requested format
    if (extension == "json" || extension == "JSON") {
        return saveToJSONFile(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        // For YAML support, we would need to include a YAML library
        NLM_LOG_WARNING("YAML format not yet implemented, saving to text format for " + filepath);
        return saveToTextFile(filepath);
    } else {
        // Default to text format for backward compatibility
        return saveToTextFile(filepath);
    }
}

bool Config::saveToTextFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::to_string(val);
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::to_string(val[i]);
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << val[i];
                }
                file << "\"]";
            } else {
                file << val;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json data;
        
        // Convert Config entries to JSON
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            json jsonValue;
            std::visit([&jsonValue](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = val;
                } else {
                    jsonValue = std::to_string(val);
                }
            }, entry.value);
            
            data[entry.key] = jsonValue;
        }
        
        // Write JSON with pretty formatting
        file << data.dump(2);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON writing error in " + filepath + ": " + std::string(e.what()));
        return false;
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
    oss << "Format: " << pImpl->fileFormat << "\n";
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
namespace nlm {
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
}

} // namespace nlm