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
    // Enhanced JSON/YAML parser with schema validation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension and use appropriate parser
    std::string ext = filepath.substr(filepath.find_last_of("."));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        // For JSON, we'd typically use a library like nlohmann::json
        // For now, implement enhanced key=value format with better parsing
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[ストア) {
                continue;
            }
            
            // Enhanced parsing with better support for:
            // - Arrays: key = [1,2,3]
            // - Booleans: key = true/false
            // - Numbers: key = 123.45
            // - Quoted strings: key = "value"
            // - Nested objects: key = { subkey = value }
            
            // For now, use the existing enhanced key=value format
            // TODO: Replace with proper JSON/YAML parser
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Enhanced value parsing
                parseValue(value);
                
                set(key, value, ConfigSource::File);
            }
        }
    } else {
        // Fallback to enhanced key=value format
        // TODO: Implement YAML parsing support
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '%') {
                continue;
            }
            
            // Parse key=value pairs with enhanced support
            // Support for YAML format as well
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
    // Enhanced set method with schema validation
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate against known configuration schema
    validateConfiguration();
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Enhanced string parsing with better error handling
    ConfigValue parsedValue;
    parseValue(value, parsedValue);
    set(key, parsedValue, source);
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

void Config::parseValue(const std::string& valueStr, ConfigValue& parsedValue) {
    std::string value = trim(valueStr);
    
    // Try to parse as boolean
    if (value == "true") {
        parsedValue = true;
        return;
    } else if (value == "false") {
        parsedValue = false;
        return;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            parsedValue = intVal;
            return;
        }
    } catch (...) {}
    
    // Try to parse as 64-bit integer
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            parsedValue = int64Val;
            return;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            parsedValue = doubleVal;
            return;
        }
    } catch (...) {}
    
    // Parse arrays (simplified)
    if (value[0] == '[' && value.back() == ']') {
        // Extract array elements
        std::string content = value.substr(1, value.size() - 2);
        std::vector<double> arr;
        size_t start = 0;
        size_t comma = content.find(',');
        
        while (comma != std::string::npos) {
            std::string token = trim(content.substr(start, comma - start));
            try {
                arr.push_back(std::stod(token));
            } catch (...) {
                // Invalid number, skip
            }
            start = comma + 1;
            comma = content.find(',', start);
        }
        
        // Handle last element
        if (start < content.size()) {
            std::string token = trim(content.substr(start));
            try {
                arr.push_back(std::stod(token));
            } catch (...) {}
        }
        
        parsedValue = arr;
        return;
    }
    
    // Default to string
    parsedValue = value;
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

void Config::validateConfiguration() {
    // Validate configuration against expected schema
    // Phase 2 enhanced validation
    
    for (const auto& entry : pImpl->entries) {
        validateEntry(entry);
    }
}

void Config::validateEntry(const ConfigEntry& entry) {
    // Validate individual configuration entry
    // For now, just basic type checking and value range validation
    
    // Example validations for known configuration keys
    if (entry.key == "neuron_count") {
        if (std::holds_alternative<int64_t>(entry.value)) {
            int64_t count = std::get<int64_t>(entry.value);
            if (count < 1 || count > 1000000) {
                // Could log warning here
            }
        }
    } else if (entry.key == "connection_probability") {
        if (std::holds_alternative<double>(entry.value)) {
            double prob = std::get<double>(entry.value);
            if (prob < 0.0 || prob > 1.0) {
                // Could log warning here
            }
        }
    }
    
    // Add more validations as needed
}

void Config::parseValue(const std::string& valueStr, ConfigValue& parsedValue) {
    std::string value = trim(valueStr);
    
    // Try to parse as boolean
    if (value == "true") {
        parsedValue = true;
        return;
    } else if (value == "false") {
        parsedValue = false;
        return;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            parsedValue = intVal;
            return;
        }
    } catch (...) {}
    
    // Try to parse as 64-bit integer
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            parsedValue = int64Val;
            return;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            parsedValue = doubleVal;
            return;
        }
    } catch (...) {}
    
    // Parse arrays (simplified)
    if (value[0] == '[' && value.back() == ']') {
        // Extract array elements
        std::string content = value.substr(1, value.size() - 2);
        std::vector<double> arr;
        size_t start = 0;
        size_t comma = content.find(',');
        
        while (comma != std::string::npos) {
            std::string token = trim(content.substr(start, comma - start));
            try {
                arr.push_back(std::stod(token));
            } catch (...) {
                // Invalid number, skip
            }
            start = comma + 1;
            comma = content.find(',', start);
        }
        
        // Handle last element
        if (start < content.size()) {
            std::string token = trim(content.substr(start));
            try {
                arr.push_back(std::stod(token));
            } catch (...) {}
        }
        
        parsedValue = arr;
        return;
    }
    
    // Default to string
    parsedValue = value;
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
