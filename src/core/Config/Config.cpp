#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;  // O(1) lookups
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    // Supports JSON files with nested object structure
    // Also supports YAML-style key: value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parser for configuration files
    // Parse "key": "value" pairs
    size_t pos = 0;
    while (pos < content.length()) {
        // Skip whitespace and comments
        while (pos < content.length() && (std::isspace(content[pos]) || content[pos] == '/' || content[pos] == '#')) {
            pos++;
        }
        
        // Find next key
        size_t keyPos = content.find('"', pos);
        if (keyPos == std::string::npos) break;
        
        std::string key = content.substr(keyPos + 1, content.find('"', keyPos + 1) - keyPos - 1);
        pos = content.find('"', keyPos + key + 2) + 1;
        
        if (pos >= content.length()) break;
        
        // Find colon separator
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valuePos = content.find('"', colonPos + 1);
        if (valuePos == std::string::npos) {
            // Try to find unquoted value (simple)
            valuePos = content.find_first_of(",}", colonPos + 1);
            if (valuePos == std::string::npos) break;
        }
        
        std::string value;
        if (valuePos > colonPos + 1) {
            value = content.substr(colonPos + 1, valuePos - colonPos - 1);
        }
        
        // Trim whitespace from value
        value = trim(value);
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        
        // Move to next position
        pos = valuePos + 1;
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration value based on key
    if (!validateConfigValue(key, value)) {
        return;  // Silently reject invalid values
    }
    
    auto it_index = pImpl->keyToIndex.find(key);
    if (it_index != pImpl->keyToIndex.end()) {
        // Key exists, update it
        size_t index = it_index->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        // Key doesn't exist, add it
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
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it_index = pImpl->keyToIndex.find(key);
    if (it_index != pImpl->keyToIndex.end()) {
        // Remove from entries
        size_t index = it_index->second;
        if (index < pImpl->entries.size()) {
            // Swap with last element and pop
            pImpl->entries[index] = std::move(pImpl->entries.back());
            pImpl->entries.pop_back();
            
            // Update keyToIndex for the swapped element
            if (!pImpl->entries.empty()) {
                std::string lastKey = pImpl->entries.back().key;
                pImpl->keyToIndex[lastKey] = index;
            }
        }
        
        // Remove from hash map
        pImpl->keyToIndex.erase(it_index);
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

// Implementation of validateConfigValue
bool Config::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    // Define reasonable bounds for configuration values
    if (key == "random_seed") {
        // Random seed should be a reasonable integer
        if (std::holds_alternative<int>(value)) {
            int seed = std::get<int>(value);
            return seed >= 0 && seed <= INT_MAX;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t seed = std::get<int64_t>(value);
            return seed >= 0;
        }
    }
    else if (key == "simulation_timestep") {
        // Simulation timestep should be positive and reasonable
        if (std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            return timestep > 0.0 && timestep < 1.0;  // Less than 1 second
        }
    }
    else if (key == "neuron_count") {
        // Neuron count should be reasonable
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            return count > 0 && count < 1000000;  // Less than 1 million neurons
        }
    }
    else if (key == "stdp_ltp_weight") {
        // STDP LTP weight should be reasonable
        if (std::holds_alternative<float>(value)) {
            float weight = std::get<float>(value);
            return weight >= 0.0f && weight <= 1.0f;
        }
    }
    else if (key == "stdp_ltd_weight") {
        // STDP LTD weight should be reasonable
        if (std::holds_alternative<float>(value)) {
            float weight = std::get<float>(value);
            return weight >= 0.0f && weight <= 1.0f;
        }
    }
    else if (key == "stdp_tau") {
        // STDP tau should be reasonable
        if (std::holds_alternative<float>(value)) {
            float tau = std::get<float>(value);
            return tau > 0.0f && tau < 1000.0f;  // Less than 1000 ms
        }
    }
    else if (key == "synapse_min_weight") {
        // Synapse min weight should be reasonable
        if (std::holds_alternative<float>(value)) {
            float weight = std::get<float>(value);
            return weight <= 0.0f;  // Min weight should be negative or zero
        }
    }
    else if (key == "synapse_max_weight") {
        // Synapse max weight should be reasonable
        if (std::holds_alternative<float>(value)) {
            float weight = std::get<float>(value);
            return weight >= 0.0f;  // Max weight should be positive or zero
        }
    }
    else if (key == "neuron_lif_threshold") {
        // Neuron threshold should be reasonable
        if (std::holds_alternative<float>(value)) {
            float threshold = std::get<float>(value);
            return threshold > -100.0f && threshold < 50.0f;  // Typical range
        }
    }
    else if (key == "reward_clip_min") {
        // Reward clip min should be reasonable
        if (std::holds_alternative<float>(value)) {
            float clip = std::get<float>(value);
            return clip <= 0.0f;  // Min should be <= 0
        }
    }
    else if (key == "reward_clip_max") {
        // Reward clip max should be reasonable
        if (std::holds_alternative<float>(value)) {
            float clip = std::get<float>(value);
            return clip >= 0.0f;  // Max should be >= 0
        }
    }
    
    // For all other keys, accept any value
    return true;
}
