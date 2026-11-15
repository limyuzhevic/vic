#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndexMap; // O(1) lookups for key->index
    
    // For fast existence checking
    std::unordered_set<std::string> keySet; // O(1) lookups for has() checks
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check if file is readable
    if (!file.good()) {
        NLM_LOG_ERROR("Config file is not in good state: " + filepath);
        return false;
    }
    
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
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error parsing config at line ") + std::to_string(lineNumber) +
                             ": " + key + " = " + e.what());
                // Continue parsing other lines
            }
        } else {
            NLM_LOG_WARNING(std::string("Skipping malformed config line ") + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO(std::string("Loaded ") + lineNumber + " lines from config file: " + filepath);
    return true;
}

// Enhanced loadFromArgs with better error handling and validation
bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Error parsing command line argument --") + key + "=" + value +
                                 ": " + e.what());
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Malformed command line argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error parsing command line argument -") + key + " " + value +
                             ": " + e.what());
                success = false;
            }
        }
    }
    
    return success;
}

// New method to load JSON configuration (Phase 2 implementation)
// This would use a proper JSON parser library
bool Config::loadFromJSON(const std::string& filepath) {
    // TODO: Implement proper JSON parsing using nlohmann::json or similar
    // For now, provide a stub that returns false to indicate not implemented
    NLM_LOG_WARNING("JSON configuration loading not yet implemented - fallback to simple format");
    return loadFromFile(filepath);
}

// Helper method to validate configuration values
bool Config::validate() const {
    // Check for required configuration values
    bool isValid = true;
    
    // Required keys for basic operation
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key);
            isValid = false;
        }
    }
    
    // Validate neuron_count
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount.has_value() && *neuronCount <= 0) {
            NLM_LOG_ERROR("neuron_count must be positive, got " + std::to_string(*neuronCount));
            isValid = false;
        }
    }
    
    // Validate region_count
    if (has("region_count")) {
        auto regionCount = get<int64_t>("region_count");
        if (regionCount.has_value() && *regionCount <= 0) {
            NLM_LOG_ERROR("region_count must be positive, got " + std::to_string(*regionCount));
            isValid = false;
        }
    }
    
    // Validate connection_probability
    if (has("connection_probability")) {
        auto prob = get<double>("connection_probability");
        if (prob.has_value()) {
            if (*prob < 0.0 || *prob > 1.0) {
                NLM_LOG_ERROR("connection_probability must be between 0.0 and 1. περισσότερες, got " + std::to_string(*prob));
                isValid = false;
            }
        }
    }
    
    return isValid;
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = pImpl->keyToIndexMap.find(key);
    
    if (it != pImpl->keyToIndexMap.end()) {
        // Key exists, update the entry
        size_t index = it->second;
        pImpl->entries[index].value = value;
        pImpl->entries[index].source = source;
    } else {
        // Key doesn't exist, add new entry
        pImpl->entries.emplace_back(key, value, source);
        size_t newIndex = pImpl->entries.size() - 1;
        pImpl->keyToIndexMap[key] = newIndex;
        pImpl->keySet.insert(key);
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
    return pImpl->keySet.find(key) != pImpl->keySet.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndexMap.find(key);
    if (it != pImpl->keyToIndexMap.end()) {
        size_t index = it->second;
        
    // Update keyToIndexMap for remaining entries
    for (auto& [k, idx] : pImpl->keyToIndexMap) {
        if (idx > index) {
            idx--;
        }
    }
    
    // Remove from keySet
    pImpl->keySet.erase(key);
    
    // Remove from keyToIndexMap
    pImpl->keyToIndexMap.erase(it);
}
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
    pImpl->keyToIndexMap.clear();
    pImpl->keySet.clear();
}

    oss << "\n\n=== Configuration Issues ===\n";
    bool hasIssues = true;
    
    if (!validate()) {
        oss << "ERROR: Configuration validation failed!\n";
        hasIssues = true;
    } else {
        oss << "Configuration is valid.\n";
    }
    
    oss << "\n=== Configuration Summary ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    
    // Count by source
    int defaultCount = 0, fileCount = 0, cmdCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cmdCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "Default values: " << defaultCount << "\n";
    oss << "File loaded: " << fileCount << "\n";
    oss << "Command line: " << cmdCount << "\n";
    oss << "Runtime set: " << runtimeCount << "\n";
    
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
    auto it = pImpl->keyToIndexMap.find(key);
    if (it == pImpl->keyToIndexMap.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    const ConfigEntry& entry = pImpl->entries[index];
    
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
    }, entry.value);
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
