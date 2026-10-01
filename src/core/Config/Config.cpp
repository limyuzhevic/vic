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
    
    // Save in JSON format for better interoperability
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Serialize value based on type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

std::string Config::escapeJSONString(const std::string& str) const {
    std::string escaped;
    escaped.reserve(str.size() * 2); // Pre-allocate for efficiency
    
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control characters - use Unicode escape
                    escaped += "\\u";
                    char hex[5];
                    std::snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned int>(c));
                    escaped += hex;
                } else {
                    escaped += c;
                }
                break;
        }
    }
    
    return escaped;
}

bool Config::validate(const ConfigSchema& schema) const {
    // Validate configuration against schema
    bool isValid = true;
    
    for (const auto& entry : schema.requiredFields) {
        if (!has(entry)) {
            return false;
        }
    }
    
    for (const auto& rule : schema.validationRules) {
        if (!validateRule(rule)) {
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::validateRule(const ValidationRule& rule) const {
    // Apply a single validation rule
    if (!has(rule.key)) {
        return !rule.required; // Missing but not required is OK
    }
    
    auto value = get(rule.key);
    if (!value) {
        return false; // Shouldn't happen if has() returned true
    }
    
    // Type checking
    bool typeMatches = false;
    if (rule.expectedType == "string" && std::holds_alternative<std::string>(*value)) {
        typeMatches = true;
    } else if (rule.expectedType == "int" && std::holds_alternative<int64_t>(*value)) {
        typeMatches = true;
    } else if (rule.expectedType == "float" && std::holds_alternative<double>(*value)) {
        typeMatches = true;
    } else if (rule.expectedType == "bool" && std::holds_alternative<bool>(*value)) {
        typeMatches = true;
    }
    
    if (!typeMatches) {
        return false;
    }
    
    // Range validation
    if (rule.minValue.has_value() && rule.maxValue.has_value()) {
        double val = getAsDouble(rule.key);
        if (val < rule.minValue.value() || val > rule.maxValue.value()) {
            return false;
        }
    }
    
    // Enum validation
    if (!rule.allowedValues.empty()) {
        bool inAllowed = false;
        for (const auto& allowed : rule.allowedValues) {
            if (compareValues(*value, allowed)) {
                inAllowed = true;
                break;
            }
        }
        if (!inAllowed) {
            return false;
        }
    }
    
    return true;
}

bool Config::compareValues(const ConfigValue& a, const ConfigValue& b) const {
    // Compare two configuration values
    if (a.index() != b.index()) {
        return false; // Different types can't be equal
    }
    
    return std::visit([](auto&& aVal, auto&& bVal) {
        return aVal == bVal;
    }, a, b);
}

double Config::getAsDouble(const std::string& key) const {
    auto value = get(key);
    if (!value) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    return std::visit([](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::stod(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<double>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? 1.0 : 0.0;
        } else {
            return static_cast<double>(val);
        }
    }, *value);
}

std::string Config::toString(const std::string& key, const std::string& defaultValue) const {
    auto value = get<std::string>(key);
    return value.has_value() ? value.value() : defaultValue;
}

int64_t Config::toInt(const std::string& key, int64_t defaultValue) const {
    auto value = get<int64_t>(key);
    return value.has_value() ? value.value() : defaultValue;
}

double Config::toDouble(const std::string& key, double defaultValue) const {
    auto value = get<double>(key);
    return value.has_value() ? value.value() : defaultValue;
}

bool Config::toBool(const std::string& key, bool defaultValue) const {
    auto value = get<bool>(key);
    return value.has_value() ? value.value() : defaultValue;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Format: JSON-compatible key-value store\n";
    oss << "Source tracking: Enabled\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (source=" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

bool Config::exportToJSON(const std::string& filepath) const {
    // Export configuration to JSON file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n\"nlm_version\": \"0.1.0\",\n";
    file << "  \"config_entries\": [\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "    {\n      \"key\": \"" << escapeJSONString(entry.key) << "\",
              \"value\": ";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << ",\n      \"source\": " << static_cast<int>(entry.source);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n    }\n";
        } else {
            file << "\n    }\n";
        }
    }
    
    file << "  ]\n}\n";
    return true;
}

bool Config::importFromJSON(const std::string& content) {
    // Import configuration from JSON string
    // For now, parse as enhanced key=value format
    return loadFromKeyValue(content);
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
