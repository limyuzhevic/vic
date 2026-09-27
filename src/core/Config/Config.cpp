#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <sstream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Hash map for O(1) lookups - provides faster access than linear search
    std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first (preferred modern format)
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON parsing (simplified implementation)
    if (parseJSON(content)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    std::ifstream file2(filepath);
    if (!file2.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file2, line)) {
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
    
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
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
        NLM_LOG_ERROR("Failed to open file for saving configuration: " + filepath);
        return false;
    }
    
    // Save in JSON format (modern, human-readable)
    file << "{\n";
    bool firstEntry = true;
    for (const auto& entry : pImpl->entries) {
        if (!firstEntry) file << ",\n";
        firstEntry = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Format the value based on its type
        std::ostringstream valueStream;
        std::visit([&valueStream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStream << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStream << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStream << ", ";
                    valueStream << arg[i];
                }
                valueStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStream << ", ";
                    valueStream << std::fixed << std::setprecision(6) << arg[i];
                }
                valueStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStream << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStream << ", ";
                    valueStream << "\"" << arg[i] << "\"";
                }
                valueStream << "]";
            } else {
                valueStream << arg;
            }
        }, entry.value);
        
        file << valueStream.str();
        
        // Add comment if description is available
        if (!entry.description.empty()) {
            file << " // " << entry.description;
        }
    }
    file << "\n}\n";
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Use hash map for O(1) lookup instead of O(n) linear search
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Configuration value type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Update hash map for O(1) lookups
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        // Key exists, update it
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        // Key doesn't exist, add it
        size_t newIndex = pImpl->entries.size();
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = newIndex;
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
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        
        // Remove from entries vector
        if (index < pImpl->entries.size()) {
            pImpl->entries.erase(pImpl->entries.begin() + index);
        }
        
        // Update hash map - remove this entry and re-index all following entries
        pImpl->keyToIndex.erase(it);
        
        // Re-index remaining entries
        for (auto& pair : pImpl->keyToIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->keyToIndex.size());
    for (const auto& pair : pImpl->keyToIndex) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::parseJSON(const std::string& jsonContent) {
    // Simple JSON parser implementation
    // In a real implementation, this would use a proper JSON library like nlohmann/json
    
    // For now, just return false to fall back to simple parsing
    // TODO: Implement proper JSON parsing
    return false;
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
