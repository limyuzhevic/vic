#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    if (parseJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fallback to simple key=value format (for backward compatibility)
    NLM_LOG_WARNING("Failed to parse as JSON, trying legacy key=value format: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments...");
    
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
                    NLM_LOG_ERROR(std::string("Error processing command line argument --") + 
                                 key + std::string("=") + value + 
                                 std::string(": ") + e.what());
                }
            } else {
                NLM_LOG_WARNING(std::string("Invalid command line argument (missing '='): ") + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error processing command line argument -") + 
                             key + std::string(" ") + value + 
                             std::string(": ") + e.what());
            }
        } else {
            NLM_LOG_WARNING(std::string("Ignoring unknown command line argument: ") + arg);
        }
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING(std::string("Invalid config line ") + std::to_string(lineNum) + 
                           std::string(": missing '=' in: ") + line);
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
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error parsing config line ") + std::to_string(lineNum) +
                         std::string(": ") + e.what() + std::string(" in: ") + line);
        }
    }
    
    return true;
}

bool Config::parseJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    // Simple JSON parsing (supports basic JSON format)
    // For production, use a proper JSON library like nlohmann/json
    
    // Remove whitespace for easier parsing
    jsonContent.erase(std::remove_if(jsonContent.begin(), jsonContent.end(), ::isspace), jsonContent.end());
    
    if (jsonContent.empty() || jsonContent[0] != '{') {
        return false;
    }
    
    // Simple object parsing: { "key": value, "key2": value2 }
    size_t pos = 1; // Skip opening {
    while (pos < jsonContent.size() && jsonContent[pos] != '}') {
        // Find next key
        size_t keyStart = jsonContent.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = jsonContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find value separator
        size_t colonPos = jsonContent.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value end (next comma or closing brace)
        size_t valueEnd = jsonContent.find_first_of(",}", colonPos + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = jsonContent.substr(colonPos + 1, valueEnd - colonPos - 1);
        
        // Parse value
        try {
            ConfigValue value = parseJSONValue(valueStr);
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("JSON parsing warning for key '") + key + 
                           std::string("': ") + e.what());
        }
        
        // Move to next key or closing brace
        pos = valueEnd + 1;
    }
    
    return true;
}

ConfigValue Config::parseJSONValue(const std::string& valueStr) {
    if (valueStr.empty()) {
        return std::string("");
    }
    
    // Handle different JSON value types
    if (valueStr == "true") {
        return true;
    } else if (valueStr == "false") {
        return false;
    } else if (valueStr == "null") {
        return std::string(""); // null becomes empty string
    } else if (valueStr[0] == '"' && valueStr[valueStr.size() - 1] == '"') {
        // String (strip quotes)
        return valueStr.substr(1, valueStr.size() - 2);
    } else {
        // Try to parse as number
        try {
            // Check for float
            size_t pos;
            float floatVal = std::stof(valueStr, &pos);
            if (pos == valueStr.size()) {
                return floatVal;
            }
        } catch (...) {}
        
        try {
            // Check for integer
            size_t pos;
            long long intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Try to parse as array
        if (valueStr[0] == '[' && valueStr[valueStr.size() - 1] == ']') {
            return parseJSONArray(valueStr);
        }
        
        // Default to string if cannot parse
        return valueStr;
    }
}

ConfigValue Config::parseJSONArray(const std::string& arrayStr) {
    // Simple array parsing: [1, 2, 3] or ["a", "b", "c"]
    std::vector<std::string> stringElements;
    
    size_t start = 1; // Skip opening [
    size_t pos = start;
    
    while (pos < arrayStr.size() && arrayStr[pos] != ']') {
        size_t elementEnd = arrayStr.find_first_of(",]", pos);
        if (elementEnd == std::string::npos) break;
        
        std::string element = trim(arrayStr.substr(pos, elementEnd - pos));
        if (!element.empty()) {
            stringElements.push_back(element);
        }
        
        pos = elementEnd + 1;
        
        // Skip comma if present
        if (pos < arrayStr.size() && arrayStr[pos] == ',') {
            pos++;
        }
    }
    
    // Convert string elements to appropriate types
    if (stringElements.empty()) {
        return std::vector<double>{}; // Empty double array
    }
    
    // Try to determine element type and convert
    bool allIntegers = true;
    bool allFloats = true;
    bool allStrings = true;
    
    for (const auto& elem : stringElements) {
        if (allIntegers) {
            try {
                size_t pos;
                std::stoll(elem, &pos);
                if (pos != elem.size()) allIntegers = false;
            } catch (...) { allIntegers = false; }
        }
        
        if (allFloats) {
            try {
                size_t pos;
                std::stof(elem, &pos);
                if (pos != elem.size()) allFloats = false;
            } catch (...) { allFloats = false; }
        }
        
        if (allStrings && ((elem.size() >= 2 && 
                            ((elem.front() == '"' && elem.back() == '"') ||
                             (elem.front() == '\'' && elem.back() == '\''))))) {
            allStrings = false; // It's a quoted string, so not just plain string
        }
    }
    
    // Create appropriate vector type
    if (allIntegers) {
        std::vector<long long> result;
        for (const auto& elem : stringElements) {
            result.push_back(std::stoll(elem));
        }
        return result;
    } else if (allFloats) {
        std::vector<double> result;
        for (const auto& elem : stringElements) {
            result.push_back(std::stof(elem));
        }
        return result;
    } else if (allStrings) {
        std::vector<std::string> result;
        for (const auto& elem : stringElements) {
            result.push_back(elem);
        }
        return result;
    } else {
        // Mixed types, return as strings
        std::vector<std::string> result;
        for (const auto& elem : stringElements) {
            // Remove quotes if present
            if (elem.size() >= 2 && ((elem.front() == '"' && elem.back() == '"') ||
                                     (elem.front() == '\'' && elem.back() == '\''))) {
                result.push_back(elem.substr(1, elem.size() - 2));
            } else {
                result.push_back(elem);
            }
        }
        return result;
    }
}

bool Config::exportToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        // Write key
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        switch (entry.value.index()) {
            case 0: // int
                file << std::get<int>(entry.value);
                break;
            case 1: // int64_t
                file << std::get<int64_t>(entry.value);
                break;
            case 2: // double
                file << std::get<double>(entry.value);
                break;
            case 3: // bool
                file << (std::get<bool>(entry.value) ? "true" : "false");
                break;
            case 4: // std::string
                file << "\"" << std::get<std::string>(entry.value) << "\"";
                break;
            case 5: // std::vector<int>
                writeJSONArray(file, std::get<std::vector<int>>(entry.value));
                break;
            case 6: // std::vector<double>
                writeJSONArray(file, std::get<std::vector<double>>(entry.value));
                break;
            case 7: // std::vector<std::string>
                writeJSONArray(file, std::get<std::vector<std::string>>(entry.value));
                break;
            default:
                file << "null";
                break;
        }
    }
    
    file << "\n}\n";
    return true;
}

template<typename T>
void Config::writeJSONArray(std::ofstream& file, const std::vector<T>& array) const {
    file << "[";
    bool first = true;
    for (const auto& item : array) {
        if (!first) {
            file << ", ";
        }
        first = false;
        
        // Use type-specific formatting
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            file << item;
        } else if constexpr (std::is_same_v<T, double>) {
            file << item;
        } else if constexpr (std::is_same_v<T, std::string>) {
            file << "\"" << item << "\"";
        }
    }
    file << "]";
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

// Type-safe value access with detailed error reporting
// Helper function to check if ConfigValue can hold the requested type
template<typename T>
bool Config::isTypeCompatible(const ConfigEntry& entry) const {
    return std::visit([](const auto& value) {
        using ValueType = std::decay_t<decltype(value)>;
        return std::is_same_v<T, ValueType>;
    }, entry.value);
}

// Type-safe value access with custom error handling
template<typename T>
std::optional<T> Config::getWithValidation(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Type checking
    if (!isTypeCompatible<T>(*it)) {
        return std::nullopt;
    }
    
    // Safe extraction
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = getWithValidation<T>(key);
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

// Helper to trim whitespace from a string
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper to convert string to lowercase
static std::string toLower(const std::string& str) {
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
