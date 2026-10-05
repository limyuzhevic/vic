#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <ctime>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationError> validationErrors;
    // Indexing for O(1) lookups
    std::unordered_map<std::string, std::vector<size_t>> keyIndex;
    mutable std::mutex mutex;  // Thread safety
    std::chrono::system_clock::time_point lastModified;
    
    Impl() : lastModified(std::chrono::system_clock::now()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Clear existing data
    clear();
    
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            return parseJSON(filepath);
        } else if (format == "yaml") {
            return parseYAML(filepath);
        } else {
            // Try to read as plain text and parse as key=value
            return parseKeyValue(filepath);
        }
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("Failed to parse configuration file: ") + e.what(), filepath);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    clear();
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON structure
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Store the actual value
        jsonData[entry.key] = convertConfigValueToJSON(entry.value);
    }
    
    // Add metadata
    jsonData["__metadata__"]["source"] = "NLM Configuration";
    jsonData["__metadata__"]["created"] = timePointToString(std::chrono::system_clock::now());
    
    try {
        file << jsonData.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end() || it->second.empty()) {
        return std::nullopt;
    }
    
    // Get the first occurrence (assuming keys are unique)
    size_t index = it->second[0];
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(pImpl->entries[index].value);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update index
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Key exists, update the first occurrence
        size_t index = it->second[0];
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            pImpl->entries[index].timestamp = std::chrono::system_clock::now();
        }
    } else {
        // New key, add to entries and index
        pImpl->entries.emplace_back(key, value, source);
        size_t index = pImpl->entries.size() - 1;
        pImpl->keyIndex[key].push_back(index);
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    return it != pImpl->keyIndex.end() && !it->second.empty();
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Mark entries for removal (we'll rebuild the data structures)
        std::vector<size_t> indicesToRemove = it->second;
        
        // Remove entries
        for (auto index : indicesToRemove) {
            if (index < pImpl->entries.size()) {
                pImpl->entries.erase(pImpl->entries.begin() + index);
                // Adjust subsequent indices
                for (auto& pair : pImpl->keyIndex) {
                    for (auto& idx : pair.second) {
                        if (idx > index) {
                            idx--;
                        }
                    }
                }
            }
        }
        
        pImpl->keyIndex.erase(it);
    }
    
    rebuildIndex();
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (std::find(keys.begin(), keys.end(), entry.key) == keys.end()) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[\"\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\", \"" << arg[i];
                }
                oss << "\"]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

void Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->validationErrors.clear();
    
    // Validate specific known configuration parameters
    auto validateValue = [this](const std::string& key) {
        auto valueOpt = get<double>(key);
        if (!valueOpt) return;
        
        double value = *valueOpt;
        
        if (key == "neuron_count" && value <= 0) {
            pImpl->validationErrors.emplace_back(key, "Neuron count must be greater than 0");
        } else if (key == "region_count" && value <= 0) {
            pImpl->validationErrors.emplace_back(key, "Region count must be greater than 0");
        } else if (key == "simulation_timestep") {
            if (value <= 0.0) {
                pImpl->validationErrors.emplace_back(key, "Simulation timestep must be positive");
            } else if (value > 1.0) {
                pImpl->validationErrors.emplace_back(key, "Simulation timestep too large (max 1.0)");
            }
        } else if (key == "connection_probability") {
            if (value < 0.0 || value > 1.0) {
                pImpl->validationErrors.emplace_back(key, "Connection probability must be between 0.0 and 1.0");
            }
        }
    };
    
    for (const auto& key : {"neuron_count", "region_count", "simulation_timestep", "connection_probability"}) {
        validateValue(key);
    }
}

bool Config::hasErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return !pImpl->validationErrors.empty();
}

const std::vector<ValidationError>& Config::getErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->validationErrors;
}

bool Config::isJSONFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) return false;
    
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    char firstChar;
    file.get(firstChar);
    
    return firstChar == '{';
}

bool Config::isYAMLFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) return false;
    
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    char firstChar;
    file.get(firstChar);
    
    return firstChar == '-';
}

std::string Config::detectFileFormat(const std::string& filepath) {
    if (isJSONFile(filepath)) return "json";
    if (isYAMLFile(filepath)) return "yaml";
    return "keyvalue";
}

bool Config::parseJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileException("Cannot open file for reading", filepath);
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Skip metadata
        if (jsonData.contains("__metadata__")) {
            jsonData.erase("__metadata__");
        }
        
        // Parse each entry
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = parseJSONValue(*it);
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("JSON parsing failed: ") + e.what(), filepath);
    }
}

bool Config::parseYAML(const std::string& filepath) {
    // For now, use key=value format as fallback
    // In a real implementation, you would use a YAML library like yaml-cpp
    return parseKeyValue(filepath);
}

bool Config::parseKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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

ConfigValue Config::parseJSONValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Handle arrays
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        for (const auto& item : jsonValue) {
            if (item.is_string()) {
                stringArray.push_back(item.get<std::string>());
            } else if (item.is_number_integer()) {
                intArray.push_back(item.get<int64_t>());
            } else if (item.is_number_float()) {
                doubleArray.push_back(item.get<double>());
            }
        }
        
        // Determine the appropriate type based on what's filled
        if (!stringArray.empty()) {
            return stringArray;
        } else if (!doubleArray.empty()) {
            return doubleArray;
        } else if (!intArray.empty()) {
            return intArray;
        }
        
        return std::vector<int>(); // Empty array
    }
    
    return std::string(); // Default empty string
}

nlohmann::json Config::convertConfigValueToJSON(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        }
    }, value);
    
    return jsonValue;
}

void Config::rebuildIndex() {
    pImpl->keyIndex.clear();
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        pImpl->keyIndex[pImpl->entries[i].key].push_back(i);
    }
}

std::string Config::timePointToString(const std::chrono::system_clock::time_point& tp) const {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count()
       << " UTC";
    
    return ss.str();
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
