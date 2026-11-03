#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigValue> defaults;
    std::map<std::string, std::string> descriptions;
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

// Set values
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description, const std::string& category) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description;
        it->category = category;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, category);
    }
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

// Helper functions for category and key extraction
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

bool Config::isValidKey(const std::string& key) {
    return !key.empty() && key.find('=') == std::string::npos;
}

bool Config::isValidCategory(const std::string& category) {
    return !category.empty();
}

std::string Config::extractCategory(const std::string& key) {
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        return key.substr(0, dotPos);
    }
    return "general";
}

std::string Config::extractKey(const std::string& fullKey) {
    size_t dotPos = fullKey.find('.');
    if (dotPos != std::string::npos) {
        return fullKey.substr(dotPos + 1);
    }
    return fullKey;
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

// Implementation of advanced configuration methods
bool Config::validate(const std::string& category) const {
    bool isValid = true;
    for (const auto& entry : pImpl->entries) {
        if (category.empty() || entry.category == category) {
            if (entry.key.empty()) {
                NLM_LOG_ERROR("Config validation error: Empty key found");
                isValid = false;
            }
            if (entry.value.index() == static_cast<size_t>(-1)) { // Invalid variant index
                NLM_LOG_ERROR("Config validation error: Invalid value type for key " + entry.key);
                isValid = false;
            }
        }
    }
    return isValid;
}

std::vector<std::string> Config::getCategories() const {
    std::vector<std::string> categories;
    std::set<std::string> uniqueCategories;
    
    for (const auto& entry : pImpl->entries) {
        if (uniqueCategories.find(entry.category) == uniqueCategories.end()) {
            uniqueCategories.insert(entry.category);
            categories.push_back(entry.category);
        }
    }
    
    return categories;
}

std::vector<std::string> Config::getKeysInCategory(const std::string& category) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::setDefault(const std::string& key, const ConfigValue& value) {
    pImpl->defaults[key] = value;
    NLM_LOG_INFO("Config default set for key: " + key);
}

ConfigValue Config::getDefault(const std::string& key) const {
    auto it = pImpl->defaults.find(key);
    if (it != pImpl->defaults.end()) {
        return it->second;
    }
    
    // Fallback to hardcoded defaults for common keys
    if (key == "neuron_count") return ConfigValue(static_cast<int64_t>(1000));
    if (key == "region_count") return ConfigValue(static_cast<int64_t>(1));
    if (key == "connection_probability") return ConfigValue(0.1f);
    if (key == "random_seed") return ConfigValue(static_cast<int64_t>(42));
    if (key == "simulation_timestep") return ConfigValue(0.001);
    if (key == "replay_interval") return ConfigValue(static_cast<int64_t>(100));
    if (key == "consolidation_interval") return ConfigValue(static_cast<int64_t>(1000));
    
    return ConfigValue();
}

void Config::setDefault(const std::string& key, const ConfigValue& value) {
    // Store default values in a separate map for now
    // This could be extended to use a defaults map stored in Impl
    // For now, we'll just log the default setting
    NLM_LOG_INFO("Config default set for key: " + key);
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

std::map<std::string, std::string> Config::getAllDescriptions() const {
    std::map<std::string, std::string> descriptions;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            descriptions[entry.key] = entry.description;
        }
    }
    
    return descriptions;
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

// Helper functions for category and key extraction
bool Config::isValidKey(const std::string& key) {
    return !key.empty() && key.find('=') == std::string::npos;
}

bool Config::isValidCategory(const std::string& category) {
    return !category.empty();
}

std::string Config::extractCategory(const std::string& key) {
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        return key.substr(0, dotPos);
    }
    return "general";
}

std::string Config::extractKey(const std::string& fullKey) {
    size_t dotPos = fullKey.find('.');
    if (dotPos != std::string::npos) {
        return fullKey.substr(dotPos + 1);
    }
    return fullKey;
}

} // namespace nlm
