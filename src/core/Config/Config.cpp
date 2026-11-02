#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <numeric>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    mutable std::mutex mutex;
    
    // JSON parsing support
    static nlohmann::json parseJsonFile(const std::string& filepath);
    static bool parseSimpleKeyValue(const std::string& content, std::vector<ConfigEntry>& entries);
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

nlohmann::json Config::Impl::parseJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return nlohmann::json();
        }
        
        nlohmann::json config;
        file >> config;
        return config;
    } catch (const std::exception&) {
        return nlohmann::json();
    }
}

bool Config::Impl::parseSimpleKeyValue(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos || pos == 0) {
            // Log warning for malformed line (optional)
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty() || value.empty()) {
            continue;
        }
        
        // Remove quotes from string values
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
    }
    
    return !entries.empty();
}

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    std::error_code ec;
    bool fileExists = std::filesystem::exists(filepath, ec);
    
    if (!fileExists || ec) {
        return false;
    }
    
    // Check file extension for format detection
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
            auto jsonConfig = Impl::parseJsonFile(filepath);
            if (jsonConfig.empty()) {
                return false;
            }
            
            pImpl->entries.clear();
            pImpl->keyToIndex.clear();
            
            // Convert JSON to ConfigEntry vector
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_unsigned()) {
                    value = it->get<uint64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_array()) {
                    // Convert array to appropriate vector type
                    if (!it->empty() && it->front().is_number_integer()) {
                        std::vector<int> intVec;
                        intVec.reserve(it->size());
                        for (const auto& item : *it) {
                            intVec.push_back(item.get<int>());
                        }
                        value = intVec;
                    } else if (!it->empty() && it->front().is_number_float()) {
                        std::vector<double> doubleVec;
                        doubleVec.reserve(it->size());
                        for (const auto& item : *it) {
                            doubleVec.push_back(item.get<double>());
                        }
                        value = doubleVec;
                    } else {
                        std::vector<std::string> stringVec;
                        stringVec.reserve(it->size());
                        for (const auto& item : *it) {
                            stringVec.push_back(item.get<std::string>());
                        }
                        value = stringVec;
                    }
                } else {
                    // Skip non-serializable types
                    continue;
                }
                
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
            
            // Update key-to-index mapping
            for (size_t i = 0; i < pImpl->entries.size(); ++i) {
                pImpl->keyToIndex[pImpl->entries[i].key] = i;
            }
            
            return !pImpl->entries.empty();
        } else {
            // Handle legacy simple key=value format
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            pImpl->entries.clear();
            pImpl->keyToIndex.clear();
            
            if (!Impl::parseSimpleKeyValue(content, pImpl->entries)) {
                return false;
            }
            
            // Update key-to-index mapping
            for (size_t i = 0; i < pImpl->entries.size(); ++i) {
                pImpl->keyToIndex[pImpl->entries[i].key] = i;
            }
            
            return true;
        }
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 0 || !argv) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.size() >= 3 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos && pos > 2) {
                std::string key = trim(arg.substr(2, pos - 2));
                std::string value = trim(arg.substr(pos + 1));
                
                if (!key.empty() && !value.empty()) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg.size() >= 2 && arg[0] == '-') {
            if (i + 1 < argc && argv[i + 1]) {
                std::string key = trim(arg.substr(1));
                std::string value = trim(argv[i + 1]);
                
                if (!key.empty() && !value.empty()) {
                    set(key, value, ConfigSource::CommandLine);
                }
                ++i; // Skip next argument
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Use JSON format for better compatibility
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonConfig[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonConfig[entry.key] = arg;
                }
            }, entry.value);
        }
        
        file << std::setw(2) << jsonConfig;
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
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
    if (key.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        
        // Remove from vector
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Update keyToIndex for all remaining entries
        pImpl->keyToIndex.clear();
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->keyToIndex[pImpl->entries[i].key] = i;
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
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
                oss << "[int array]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[float array]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[string array]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    if (str.empty()) return str;
    
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) return "";
    
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        return std::nullopt;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[it->second];
    
    // Try direct conversion first
    if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(entry.value)) {
            return std::get<std::string>(entry.value);
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(entry.value)) {
            return std::get<int64_t>(entry.value);
        }
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        if (std::holds_alternative<uint64_t>(entry.value)) {
            return std::get<uint64_t>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            int64_t val = std::get<int64_t>(entry.value);
            if (val >= 0) return static_cast<uint64_t>(val);
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(entry.value)) {
            return std::get<double>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            return static_cast<double>(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<int>(entry.value)) {
            return static_cast<double>(std::get<int>(entry.value));
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(entry.value)) {
            return std::get<bool>(entry.value);
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
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
