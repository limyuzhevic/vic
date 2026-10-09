#include <sstream>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    
    // Add entry with O(1) index mapping
    size_t addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
        if (keyToIndex.find(key) != keyToIndex.end()) {
            // Key already exists, update it
            for (size_t i = 0; i < entries.size(); ++i) {
                if (entries[i].key == key) {
                    entries[i].value = value;
                    entries[i].source = source;
                    entries[i].description = description;
                    return i;
                }
            }
        }
        
        // Add new entry
        size_t index = entries.size();
        entries.emplace_back(key, value, source, description);
        keyToIndex[key] = index;
        return index;
    }
    
    void removeEntry(const std::string& key) {
        auto it = keyToIndex.find(key);
        if (it != keyToIndex.end()) {
            size_t index = it->second;
            
            // Update keyToIndex for other entries
            for (auto& kv : keyToIndex) {
                if (kv.second > index) {
                    kv.second--;
                }
            }
            
            // Remove from entries
            entries.erase(entries.begin() + index);
            keyToIndex.erase(it);
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#include <sstream>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>
#include <system_error>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    
    // Add entry with O(1) index mapping
    size_t addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "") {
        if (keyToIndex.find(key) != keyToIndex.end()) {
            // Key already exists, update it
            for (size_t i = 0; i < entries.size(); ++i) {
                if (entries[i].key == key) {
                    entries[i].value = value;
                    entries[i].source = source;
                    entries[i].description = description;
                    return i;
                }
            }
        }
        
        // Add new entry
        size_t index = entries.size();
        entries.emplace_back(key, value, source, description);
        keyToIndex[key] = index;
        return index;
    }
    
    void removeEntry(const std::string& key) {
        auto it = keyToIndex.find(key);
        if (it != keyToIndex.end()) {
            size_t index = it->second;
            
            // Update keyToIndex for other entries
            for (auto& kv : keyToIndex) {
                if (kv.second > index) {
                    kv.second--;
                }
            }
            
            // Remove from entries
            entries.erase(entries.begin() + index);
            keyToIndex.erase(it);
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()), errorReportingEnabled_(true) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::validateInput(const std::string& key, const ConfigValue& value) const {
    if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.") == std::string::npos) {
        throw ConfigError("Invalid configuration key: " + key, ConfigErrorType::InvalidKey);
    }
    
    if (key.size() > 256) {
        throw ConfigError("Configuration key too long: " + key, ConfigErrorType::OutOfRange);
    }
}

void Config::logOperation(const std::string& operation, const std::string& key, 
                         bool success, const std::string& details) {
    if (errorReportingEnabled_) {
        if (success) {
            NLM_LOG_INFO("Config " + operation + " for key '" + key + "': " + details);
        } else {
            NLM_LOG_ERROR("Config " + operation + " failed for key '" + key + "': " + details);
        }
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError_ = "Empty file path provided";
        logOperation("loadFromFile", "", false, lastError_);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::error_code ec;
        if (std::filesystem::exists(filepath, ec)) {
            lastError_ = "Permission denied when accessing file: " + filepath;
            logOperation("loadFromFile", filepath, false, lastError_);
            return false;
        } else {
            lastError_ = "File not found: " + filepath;
            logOperation("loadFromFile", filepath, false, lastError_);
            return false;
        }
    }
    
    try {
        json j;
        file >> j;
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Validate key
                if (!validateKey(key)) {
                    lastError_ = "Invalid key in JSON: " + key;
                    logOperation("loadFromFile", key, false, lastError_);
                    continue;
                }
                
                // Parse value based on type
                try {
                    if (it.value().is_number_integer()) {
                        set(key, it.value().get<int>(), ConfigSource::File);
                        logOperation("loadFromFile", key, true, "Loaded integer value");
                    } else if (it.value().is_number_unsigned()) {
                        set(key, it.value().get<int64_t>(), ConfigSource::File);
                        logOperation("loadFromFile", key, true, "Loaded unsigned integer value");
                    } else if (it.value().is_number_float()) {
                        set(key, it.value().get<double>(), ConfigSource::File);
                        logOperation("loadFromFile", key, true, "Loaded floating point value");
                    } else if (it.value().is_boolean()) {
                        set(key, it.value().get<bool>(), ConfigSource::File);
                        logOperation("loadFromFile", key, true, "Loaded boolean value");
                    } else if (it.value().is_string()) {
                        set(key, it.value().get<std::string>(), ConfigSource::File);
                        logOperation("loadFromFile", key, true, "Loaded string value");
                    } else if (it.value().is_array()) {
                        // Handle arrays
                        if (it.value().size() > 0) {
                            if (std::all_of(it.value().begin(), it.value().end(), 
                                         [](const json& v) { return v.is_number_integer(); })) {
                                std::vector<int> intArray;
                                intArray.reserve(it.value().size());
                                for (const auto& item : it.value()) {
                                    intArray.push_back(item.get<int>());
                                }
                                set(key, intArray, ConfigSource::File);
                                logOperation("loadFromFile", key, true, "Loaded integer array");
                            } else if (std::all_of(it.value().begin(), it.value().end(),
                                                 [](const json& v) { return v.is_number_float(); })) {
                                std::vector<double> doubleArray;
                                doubleArray.reserve(it.value().size());
                                for (const auto& item : it.value()) {
                                    doubleArray.push_back(item.get<double>());
                                }
                                set(key, doubleArray, ConfigSource::File);
                                logOperation("loadFromFile", key, true, "Loaded floating point array");
                            } else if (std::all_of(it.value().begin(), it.value().end(),
                                                 [](const json& v) { return v.is_string(); })) {
                                std::vector<std::string> stringArray;
                                stringArray.reserve(it.value().size());
                                for (const auto& item : it.value()) {
                                    stringArray.push_back(item.get<std::string>());
                                }
                                set(key, stringArray, ConfigSource::File);
                                logOperation("loadFromFile", key, true, "Loaded string array");
                            } else {
                                lastError_ = "Unsupported array type for key: " + key;
                                logOperation("loadFromFile", key, false, lastError_);
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    lastError_ = "Error parsing value for key '" + key + "': " + e.what();
                    logOperation("loadFromFile", key, false, lastError_);
                }
            }
        }
    } catch (const json::parse_error& e) {
        lastError_ = "JSON parse error: " + std::string(e.what());
        logOperation("loadFromFile", filepath, false, lastError_);
        return false;
    } catch (const std::exception& e) {
        lastError_ = "Unexpected error: " + std::string(e.what());
        logOperation("loadFromFile", filepath, false, lastError_);
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

bool Config::saveToFile(const std::string& filepath) const {
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError_ = "Empty file path provided";
        logOperation("saveToFile", "", false, lastError_);
        return false;
    }
    
    // Create parent directory if it doesn't exist
    std::filesystem::path path(filepath);
    std::error_code ec;
    if (!std::filesystem::exists(path.parent_path(), ec)) {
        if (!std::filesystem::create_directories(path.parent_path(), ec)) {
            lastError_ = "Cannot create directory " + path.parent_path().string() + ": " + ec.message();
            logOperation("saveToFile", filepath, false, lastError_);
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::error_code ec;
        if (std::filesystem::is_directory(path, ec)) {
            lastError_ = "Path is a directory, not a file: " + filepath;
        } else if (std::filesystem::exists(path.parent_path(), ec) && 
                  !std::filesystem::is_writable(path.parent_path(), ec)) {
            lastError_ = "Permission denied writing to directory: " + path.parent_path().string();
        } else {
            lastError_ = "Cannot create file " + filepath + ": " + ec.message();
        }
        logOperation("saveToFile", filepath, false, lastError_);
        return false;
    }
    
    try {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Handle different value types
            std::visit([&file, &entry](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, int>) {
                    file << value;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << value;
                } else if constexpr (std::is_same_v<T, double>) {
                    // Format double to avoid scientific notation
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(10) << value;
                    file << oss.str();
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << value << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < value.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << value[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < value.size(); ++j) {
                        if (j > 0) file << ", ";
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(10) << value[j];
                        file << oss.str();
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < value.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << "\"" << value[j] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
        
        // Check if write was successful
        if (file.fail()) {
            lastError_ = "Write error while saving to file: " + filepath;
            logOperation("saveToFile", filepath, false, lastError_);
            return false;
        }
        
        logOperation("saveToFile", filepath, true, "Successfully saved " + 
                    std::to_string(pImpl->entries.size()) + " entries");
    } catch (const std::exception& e) {
        lastError_ = "Error saving configuration: " + std::string(e.what());
        logOperation("saveToFile", filepath, false, lastError_);
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    clearLastError();
    
    // Validate key
    if (!validateKey(key)) {
        lastError_ = "Invalid key for get operation: " + key;
        logOperation("get", key, false, lastError_);
        return std::nullopt;
    }
    
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        lastError_ = "Key not found: " + key;
        logOperation("get", key, false, lastError_);
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        lastError_ = "Invalid index for key: " + key;
        logOperation("get", key, false, lastError_);
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    try {
        T value = std::get<T>(entry.value);
        logOperation("get", key, true, "Retrieved " + typeid(T).name() + " value");
        return value;
    } catch (const std::bad_variant_access&) {
        lastError_ = "Type mismatch for key '" + key + "': expected " + 
                    (std::is_same_v<T, int> ? "int" :
                     std::is_same_v<T, int64_t> ? "int64_t" :
                     std::is_same_v<T, double> ? "double" :
                     std::is_same_v<T, bool> ? "bool" :
                     std::is_same_v<T, std::string> ? "string" :
                     std::is_same_v<T, std::vector<int> ? "vector<int>" :
                     std::is_same_v<T, std::vector<double> ? "vector<double>" :
                     std::is_same_v<T, std::vector<std::string> ? "vector<string>" : "unknown");
        logOperation("get", key, false, lastError_);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    clearLastError();
    
    try {
        // Validate inputs
        validateInput(key, value);
        validateValue(key, value);
        
        // Set the value
        size_t index = pImpl->addEntry(key, value, source);
        
        logOperation("set", key, true, "Set value with source " + 
                    (source == ConfigSource::Default ? "Default" :
                     source == ConfigSource::File ? "File" :
                     source == ConfigSource::CommandLine ? "CommandLine" :
                     source == ConfigSource::Runtime ? "Runtime" : "Unknown"));
    } catch (const ConfigError& e) {
        lastError_ = e.what();
        logOperation("set", key, false, lastError_);
        throw;
    } catch (const std::exception& e) {
        lastError_ = "Error setting key '" + key + "': " + e.what();
        logOperation("set", key, false, lastError_);
        throw ConfigError(lastError_, ConfigErrorType::ValidationError);
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
    clearLastError();
    
    if (!validateKey(key)) {
        lastError_ = "Invalid key for has check: " + key;
        logOperation("has", key, false, lastError_);
        return false;
    }
    
    bool result = pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
    if (!result) {
        lastError_ = "Key not found: " + key;
        logOperation("has", key, false, lastError_);
    }
    return result;
}

void Config::remove(const std::string& key) {
    clearLastError();
    
    if (!validateKey(key)) {
        lastError_ = "Invalid key for remove operation: " + key;
        logOperation("remove", key, false, lastError_);
        return;
    }
    
    try {
        pImpl->removeEntry(key);
        logOperation("remove", key, true, "Successfully removed key");
    } catch (const std::exception& e) {
        lastError_ = "Error removing key '" + key + "': " + e.what();
        logOperation("remove", key, false, lastError_);
        throw;
    }
}

std::vector<std::string> Config::getKeys() const {
    clearLastError();
    
    std::vector<std::string> keys;
    try {
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        logOperation("getKeys", "", true, "Retrieved " + std::to_string(keys.size()) + " keys");
    } catch (const std::exception& e) {
        lastError_ = "Error retrieving keys: " + std::string(e.what());
        logOperation("getKeys", "", false, lastError_);
        throw;
    }
    
    return keys;
}

void Config::clear() {
    clearLastError();
    
    try {
        pImpl->entries.clear();
        pImpl->keyToIndex.clear();
        logOperation("clear", "", true, "Successfully cleared all configuration");
    } catch (const std::exception& e) {
        lastError_ = "Error clearing configuration: " + std::string(e.what());
        logOperation("clear", "", false, lastError_);
        throw;
    }
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
