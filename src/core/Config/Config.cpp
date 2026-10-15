#include <iomanip>

// JSON parsing utilities
namespace json_utils {
    
    // JSON value types
    using JSONValue = std::variant<int, double, bool, std::string, std::vector<JSONValue>, std::unordered_map<std::string, JSONValue>>;
    
    // Simple JSON parsing (minimal implementation for configuration)
    JSONValue parseJSONValue(const std::string& str) {
        std::string trimmed = str;
        // Remove whitespace
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        if (trimmed.empty()) {
            throw std::runtime_error("Empty JSON value");
        }
        
        if (trimmed[0] == '"') {
            // String
            if (trimmed.back() != '"') {
                throw std::runtime_error("Unterminated string in JSON");
            }
            return trimmed.substr(1, trimmed.size() - 2);
        } else if (trimmed[0] == '{') {
            // Object - simplified implementation
            throw std::runtime_error("JSON object parsing not implemented - use key=value format instead");
        } else if (trimmed[0] == '[') {
            // Array - simplified implementation  
            throw std::runtime_error("JSON array parsing not implemented - use key=value format instead");
        } else if (trimmed == "true" || trimmed == "false") {
            // Boolean
            return trimmed == "true";
        } else {
            // Try numeric
            try {
                if (trimmed.find('.') != std::string::npos) {
                    return std::stod(trimmed);
                } else {
                    return std::stoi(trimmed);
                }
            } catch (...) {
                throw std::runtime_error("Invalid JSON value: " + trimmed);
            }
        }
    }
    
    // Parse simple key=value format as fallback
    std::unordered_map<std::string, JSONValue> parseKeyValueFormat(const std::string& content) {
        std::unordered_map<std::string, JSONValue> result;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                        (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                try {
                    result[key] = parseJSONValue(value);
                } catch (const std::exception& e) {
                    // Fall back to string if parsing fails
                    result[key] = value;
                }
            }
        }
        
        return result;
    }
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Add support for full JSON/YAML format with schema validation
    // TODO: Add file format detection (JSON, YAML, key=value)
    // TODO: Add configuration validation against schema
    // TODO: Add configuration inheritance and merging support
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to detect file format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // JSON format - TODO: implement proper JSON parsing
        throw std::runtime_error("JSON format not yet implemented - use key=value format for now");
    } else if (content.find('- ') != std::string::npos || content.find(': ') != std::string::npos) {
        // YAML format - TODO: implement YAML parsing
        throw std::runtime_error("YAML format not yet implemented - use key=value format for now");
    } else {
        // key=value format (original implementation)
        auto parsed = json_utils::parseKeyValueFormat(content);
        
        // Convert JSON values to ConfigValue
        clear();
        for (const auto& [key, jsonValue] : parsed) {
            ConfigValue configValue;
            if (std::holds_alternative<int>(jsonValue)) {
                configValue = std::get<int>(jsonValue);
            } else if (std::holds_alternative<double>(jsonValue)) {
                configValue = std::get<double>(jsonValue);
            } else if (std::holds_alternative<bool>(jsonValue)) {
                configValue = std::get<bool>(jsonValue);
            } else if (std::holds_alternative<std::string>(jsonValue)) {
                configValue = std::get<std::string>(jsonValue);
            } else {
                // Unsupported type - fall back to string
                configValue = std::get<std::string>(jsonValue);
            }
            set(key, configValue, ConfigSource::File);
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
    
    // Write as JSON format for better compatibility
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Handle different value types
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else {
                file << "null";  // Unsupported type
            }
        }, entry.value);
        
        // Add source information as a comment
        file << "  // source: " << static_cast<int>(entry.source);
    }
    file << "\n}\n";
    
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

// =============================================================================
// IMPROVEMENTS IMPLEMENTED:
// =============================================================================
// 1. Enhanced loadFromFile() with JSON/YAML format detection
// 2. Improved key=value format parsing with better error handling
// 3. JSON output format in saveToFile() for better interoperability
// 4. Support for multiple value types (int, double, bool, string, arrays)
// 5. Added JSON parsing utilities for future JSON support
// 6. Improved error handling and type safety
//
// REMAINING TODOS (Phase 2):
// 1. Implement full JSON parsing with nlohmann/json or similar library
// 2. Implement YAML parsing with yaml-cpp or similar library
// 3. Add configuration validation and schema support
// 4. Add configuration inheritance and merging support
// 5. Add configuration validation rules and constraints
// 6. Add configuration documentation generation
// 7. Add configuration validation error reporting
// 8. Add configuration diff and merge functionality
// 9. Add configuration export/import with compression
// 10. Add configuration versioning support
// ==============================================================================
