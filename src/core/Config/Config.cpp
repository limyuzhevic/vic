#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        auto json = nlohmann::json::parse(file);
        file.close(); // Parse consumes the file, close it
        return parseJson(json);
    } catch (const nlohmann::json::parse_error& e) {
        // Try fallback to key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
        
        return loadFromKeyValueFile(file);
    }
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

bool Config::validate() const {
    // Perform basic validation
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_WARNING("Config entry has empty key");
        }
        
        // Add specific validation rules for known keys
        if (entry.key == "random_seed" || entry.key == "neuron_count" || 
            entry.key == "region_count") {
            auto val = get<int64_t>(entry.key);
            if (val && *val < 0) {
                NLM_LOG_ERROR("Invalid value for " + entry.key + ": must be non-negative");
                return false;
            }
        } else if (entry.key == "connection_probability") {
            auto val = get<float>(entry.key);
            if (val && (*val < 0.0f || *val > 1.0f)) {
                NLM_LOG_ERROR("Invalid value for " + entry.key + ": must be between 0.0 and 1.0");
                return false;
            }
        }
    }
    
    return true;
}

std::vector<ConfigValidationError> Config::validate(const ConfigSchema& schema) const {
    return schema.validate(*this);
}

void Config::applySchemaDefaults() {
    // Clear current config
    clear();
    
    // Apply schema defaults
    for (const auto& entry : currentSchema.defaults) {
        // Convert value based on its type
        if (std::holds_alternative<int>(entry.value)) {
            set(entry.key, std::get<int>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            set(entry.key, std::get<int64_t>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<double>(entry.value)) {
            set(entry.key, std::get<double>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<float>(entry.value)) {
            set(entry.key, std::get<float>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<bool>(entry.value)) {
            set(entry.key, std::get<bool>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            set(entry.key, std::get<std::string>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            set(entry.key, std::get<std::vector<int>>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            set(entry.key, std::get<std::vector<double>>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            set(entry.key, std::get<std::vector<std::string>>(entry.value), entry.source, entry.description);
        }
    }
    
    NLM_LOG_INFO("Applied schema defaults from: " + currentSchema.name);
}

bool Config::loadFromKeyValueFile(std::ifstream& file) {
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

bool Config::parseJson(const nlohmann::json& json) {
    clear(); // Start with empty config
    
    // Process JSON object recursively
    processJsonObject(json);
    return true;
}

void Config::processJsonObject(const nlohmann::json& json, const std::string& prefix = "") {
    if (json.is_object()) {
        for (const auto& item : json.items()) {
            std::string key = prefix + item.key();
            processJsonValue(item.value(), key);
        }
    }
}

void Config::processJsonValue(const nlohmann::json& value, const std::string& key) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), ConfigSource::File);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), ConfigSource::File);
    } else if (value.is_number_unsigned()) {
        set(key, value.get<uint64_t>(), ConfigSource::File);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), ConfigSource::File);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), ConfigSource::File);
    } else if (value.is_array()) {
        processJsonArray(value, key);
    } else if (value.is_null()) {
        set(key, std::string(""), ConfigSource::File);
    } else {
        // For other types, convert to string
        set(key, value.dump(), ConfigSource::File);
    }
}

void Config::processJsonArray(const nlohmann::json& array, const std::string& key) {
    // Try to infer array type
    bool allInts = true;
    bool allFloats = true;
    bool allStrings = true;
    
    for (const auto& item : array) {
        if (!item.is_number_integer()) allInts = false;
        if (!item.is_number_float()) allFloats = false;
        if (!item.is_string()) allStrings = false;
    }
    
    if (allInts) {
        std::vector<int64_t> intArray;
        for (const auto& item : array) {
            intArray.push_back(item.get<int64_t>());
        }
        set(key, intArray, ConfigSource::File);
    } else if (allFloats) {
        std::vector<double> floatArray;
        for (const auto& item : array) {
            floatArray.push_back(item.get<double>());
        }
        set(key, floatArray, ConfigSource::File);
    } else if (allStrings) {
        std::vector<std::string> stringArray;
        for (const auto& item : array) {
            stringArray.push_back(item.get<std::string>());
        }
        set(key, stringArray, ConfigSource::File);
    } else {
        // Mixed or unknown type - store as string representation
        set(key, array.dump(), ConfigSource::File);
    }
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
        NLM_LOG_ERROR("Failed to save config to file: " + filepath);
        return false;
    }
    
    // Create JSON representation
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJson(entry.value);
    }
    
    try {
        file << std::setw(2) << json << std::endl;
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to write config to file: ") + e.what());
        return false;
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) const {
    return std::visit([](const auto& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return v;
        } else {
            return "";
        }
    }, value);
}

bool Config::validate() const {
    // Perform basic validation
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_WARNING("Config entry has empty key");
        }
        
        // Add specific validation rules for known keys
        if (entry.key == "random_seed" || entry.key == "neuron_count" || 
            entry.key == "region_count") {
            auto val = get<int64_t>(entry.key);
            if (val && *val < 0) {
                NLM_LOG_ERROR("Invalid value for " + entry.key + ": must be non-negative");
                return false;
            }
        } else if (entry.key == "connection_probability") {
            auto val = get<float>(entry.key);
            if (val && (*val < 0.0f || *val > 1.0f)) {
                NLM_LOG_ERROR("Invalid value for " + entry.key + ": must be between 0.0 and 1.0");
                return false;
            }
        }
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

void Config::applySchemaDefaults() {
    // Clear current config
    clear();
    
    // Apply schema defaults
    for (const auto& entry : currentSchema.defaults) {
        // Convert value based on its type
        if (std::holds_alternative<int>(entry.value)) {
            set(entry.key, std::get<int>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            set(entry.key, std::get<int64_t>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<double>(entry.value)) {
            set(entry.key, std::get<double>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<float>(entry.value)) {
            set(entry.key, std::get<float>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<bool>(entry.value)) {
            set(entry.key, std::get<bool>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            set(entry.key, std::get<std::string>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            set(entry.key, std::get<std::vector<int>>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            set(entry.key, std::get<std::vector<double>>(entry.value), entry.source, entry.description);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            set(entry.key, std::get<std::vector<std::string>>(entry.value), entry.source, entry.description);
        }
    }
    
    NLM_LOG_INFO("Applied schema defaults from: " + currentSchema.name);
}

// Function to get validation errors from a schema and config
std::vector<ConfigValidationError> validateConfig(const ConfigSchema& schema, const Config& config) {
    return schema.validate(config);
}
