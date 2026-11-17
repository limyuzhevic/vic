#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
            
            // Validate configuration values
            validateConfigValue(key, value, filepath, lineNum);
            
            set(key, value, ConfigSource::File);
        }
    }
    
    // Post-validation after all values are loaded
    validateAllConfigValues();
    
    return true;
}

// Helper method to validate individual configuration values
void Config::validateConfigValue(const std::string& key, const std::string& value, const std::string& filepath, int lineNum) {
    // Validate dopamine_baseline range
    if (key == "dopamine_baseline") {
        try {
            double val = std::stod(value);
            if (val < 0.0 || val > 1.0) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": dopamine_baseline must be between 0.0 and 1.0, got " << val << std::endl;
                // Continue anyway but log the error
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for dopamine_baseline: " << value << std::endl;
        }
    }
    
    // Validate connection_probability range
    else if (key == "connection_probability") {
        try {
            double val = std::stod(value);
            if (val < 0.0 || val > 1.0) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": connection_probability must be between 0.0 and 1.0, got " << val << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for connection_probability: " << value << std::endl;
        }
    }
    
    // Validate neuron_count range
    else if (key == "neuron_count") {
        try {
            long long val = std::stoll(value);
            if (val < 10 || val > 100000) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": neuron_count must be between 10 and 100000, got " << val << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for neuron_count: " << value << std::endl;
        }
    }
    
    // Validate region_count range
    else if (key == "region_count") {
        try {
            long long val = std::stoll(value);
            if (val < 1 || val > 100) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": region_count must be between olamento_value(val, val << "; // Fallback
}
// Add the new method after set
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

// Add validation methods
void Config::validateConfigValue(const std::string& key, const std::string& value, const std::string& filepath, int lineNum) {
    // Validate dopamine_baseline range
    if (key == "dopamine_baseline") {
        try {
            double val = std::stod(value);
            if (val < 0.0 || val > 1.0) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": dopamine_baseline must be between 0.0 and 1.0, got " << val << std::endl;
                // Continue anyway but log the error
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for dopamine_baseline: " << value << std::endl;
        }
    }
    
    // Validate connection_probability range
    else if (key == "connection_probability") {
        try {
            double val = std::stod(value);
            if (val < 0.0 || val > 1.0) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": connection_probability must be between 0.0 and 1.0, got " << val << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for connection_probability: " << value << std::endl;
        }
    }
    
    // Validate neuron_count range
    else if (key == "neuron_count") {
        try {
            long long val = std::stoll(value);
            if (val < 10 || val > 100000) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": neuron_count must be between 10 and 100000, got " << val << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for neuron_count: " << value << std::endl;
        }
    }
    
    // Validate region_count range
    else if (key == "region_count") {
        try {
            long long val = std::stoll(value);
            if (val < 1 || val > 100) {
                std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                         << ": region_count must be between 1 and 100, got " << val << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Config validation error at " << filepath << ":" << lineNum 
                     << ": Invalid value for region_count: " << value << std::endl;
        }
    }
}

void Config::validateAllConfigValues() {
    // Post-validation to check for consistency between related values
    // Check if connection_probability is not too small for small networks
    auto connProb = get<double>("connection_probability");
    auto neuronCount = get<int64_t>("neuron_count");
    
    if (connProb.has_value() && neuronCount.has_value()) {
        if (neuronCount.value() < 100 && connProb.value() < 0.01) {
            std::cerr << "Config validation warning: Small network with very low connection probability may result in insufficient connectivity" << std::endl;
        }
    }
    
    // Check if simulation_time_limit is reasonable
    auto simTimeLimit = get<double>("simulation_time_limit");
    if (simTimeLimit.has_value() && simTimeLimit.value() < 0.0) {
        std::cerr << "Config validation warning: simulation_time_limit should be non-negative" << std::endl;
    }
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

} // namespace nlm
