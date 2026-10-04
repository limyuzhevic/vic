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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file extension for format detection
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFile(filepath);
    }
    
    if (!success) {
        NLM_LOG_WARNING("Failed to parse config file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // For now, fall back to simple parser
    // TODO: Implement proper JSON parser with error handling
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, fall back to simple parser
    // TODO: Implement proper YAML parser with error handling
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
            success = false;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to set the value with proper type inference
        if (!setValueWithInference(key, value, ConfigSource::File)) {
            NLM_LOG_WARNING("Failed to parse config key \"" + key + "\" at line " + 
                          std::to_string(lineNum) + ": " + value);
            success = false;
        }
    }
    
    // Validate required keys after loading
    validateRequiredKeys();
    
    return success;
}

bool Config::setValueWithInference(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try different types in order of preference
    if (trySetInt(key, valueStr, source) ||
        trySetDouble(key, valueStr, source) ||
        trySetBool(key, valueStr, source) ||
        trySetString(key, valueStr, source)) {
        return true;
    }
    
    // Try vector types
    if (trySetIntVector(key, valueStr, source) ||
        trySetDoubleVector(key, valueStr, source) ||
        trySetStringVector(key, valueStr, source)) {
        return true;
    }
    
    NLM_LOG_ERROR("Cannot parse value for key \"" + key + "\": \"" + valueStr + "\"");
    return false;
}

bool Config::trySetInt(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        size_t pos;
        int64_t value = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, value, source);
            return true;
        }
    } catch (const std::exception&) {}
    return false;
}

bool Config::trySetDouble(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        size_t pos;
        double value = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, value, source);
            return true;
        }
    } catch (const std::exception&) {}
    return false;
}

bool Config::trySetBool(const std::string& key, const std::string& valueStr, ConfigSource source) {
    std::string lower = toLower(valueStr);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        set(key, true, source);
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        set(key, false, source);
        return true;
    }
    return false;
}

bool Config::trySetString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    set(key, valueStr, source);
    return true;
}

bool Config::trySetIntVector(const std::string& key, const std::string& valueStr, ConfigSource source) {
    if (valueStr.empty() || valueStr.front() != '[' || valueStr.back() != ']') {
        return false;
    }
    
    std::string content = valueStr.substr(1, valueStr.size() - 2);
    std::vector<int> values;
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) {
            end = content.size();
        }
        
        std::string token = trim(content.substr(pos, end - pos));
        try {
            values.push_back(std::stoi(token));
        } catch (const std::exception&) {
            return false;
        }
        
        pos = end + 1;
        while (pos < content.size() && std::isspace(content[pos])) {
            ++pos;
        }
    }
    
    set(key, values, source);
    return true;
}

bool Config::trySetDoubleVector(const std::string& key, const std::string& valueStr, ConfigSource source) {
    if (valueStr.empty() || valueStr.front() != '[' || valueStr.back() != ']') {
        return false;
    }
    
    std::string content = valueStr.substr(1, valueStr.size() - 2);
    std::vector<double> values;
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) {
            end = content.size();
        }
        
        std::string token = trim(content.substr(pos, end - pos));
        try {
            values.push_back(std::stod(token));
        } catch (const std::exception&) {
            return false;
        }
        
        pos = end + 1;
        while (pos < content.size() && std::isspace(content[pos])) {
            ++pos;
        }
    }
    
    set(key, values, source);
    return true;
}

bool Config::trySetStringVector(const std::string& key, const std::string& valueStr, ConfigSource source) {
    if (valueStr.empty() || valueStr.front() != '[' || valueStr.back() != ']') {
        return false;
    }
    
    std::string content = valueStr.substr(1, valueStr.size() - 2);
    std::vector<std::string> values;
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) {
            end = content.size();
        }
        
        std::string token = trim(content.substr(pos, end - pos));
        // Remove quotes from string tokens
        if (token.size() >= 2 && 
            ((token.front() == '"' && token.back() == '"') ||
             (token.front() == '\'' && token.back() == '\''))) {
            token = token.substr(1, token.size() - 2);
        }
        
        values.push_back(token);
        
        pos = end + 1;
        while (pos < content.size() && std::isspace(content[pos])) {
            ++pos;
        }
    }
    
    set(key, values, source);
    return true;
}

void Config::validateRequiredKeys() {
    for (const auto& entry : pImpl->entries) {
        if (entry.isRequired && entry.key != "random_seed") {  // Allow random_seed to be optional
            // Check if key has a meaningful value
            bool hasValue = false;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (!std::is_same_v<T, std::string> || !arg.empty()) {
                    if constexpr (!std::is_same_v<T, std::vector<int>> || !arg.empty()) {
                        if constexpr (!std::is_same_v<T, std::vector<double>> || !arg.empty()) {
                            if constexpr (!std::is_same_v<T, std::vector<std::string>> || !arg.empty()) {
                                hasValue = true;
                            }
                        }
                    }
                }
            }, entry.value);
            
            if (!hasValue) {
                NLM_LOG_WARNING("Required config key \"" + entry.key + "\" has empty or invalid value");
            }
        }
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
