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
    // Try to parse as JSON first, fall back to simple key=value format
    bool success = false;
    
    // Try JSON parsing
    success = parseJSONFile(filepath);
    if (!success) {
        // Try YAML parsing
        success = parseYAMLFile(filepath);
    }
    
    if (!success) {
        // Fall back to simple key=value format for backward compatibility
        success = parseSimpleKeyValueFile(filepath);
    }
    
    return success;
}

bool Config::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON using a simple approach
    // In a real implementation, this would use a proper JSON library
    // For now, we'll implement a minimal JSON parser
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Skip whitespace
    size_t pos = content.find_first_not_of(" \t\n\r");
    if (pos == std::string::npos) {
        return false;  // Empty file
    }
    
    // Check if it's a JSON object (starts with {)
    if (content[pos] != '{') {
        return false;
    }
    
    // Parse JSON object
    return parseJSONObject(content, pos);
}

bool Config::parseJSONObject(const std::string& json, size_t& pos) {
    // Find matching closing brace
    int braceCount = 1;
    size_t endPos = pos + 1;
    
    while (endPos < json.size() && braceCount > 0) {
        char c = json[endPos];
        if (c == '{') {
            braceCount++;
        } else if (c == '}') {
            braceCount--;
        }
        endPos++;
    }
    
    if (braceCount > 0) {
        return false; // Unmatched braces
    }
    
    std::string objectContent = json.substr(pos + 1, endPos - pos - 2);
    pos = endPos;
    
    // Parse key-value pairs
    size_t keyPos = 0;
    while (keyPos < objectContent.size()) {
        // Find next key (string in quotes)
        size_t keyStart = objectContent.find('"', keyPos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = objectContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = objectContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        keyPos = keyEnd + 1;
        
        // Skip whitespace
        keyPos = objectContent.find_first_not_of(" \t\n\r:", keyPos);
        if (keyPos == std::string::npos) {
            break;
        }
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(objectContent, keyPos, value)) {
            break;
        }
        
        // Remove trailing comma if present
        size_t commaPos = keyPos;
        while (commaPos < objectContent.size() && (objectContent[commaPos] == ',' || 
                                                 objectContent[commaPos] == ' ' || 
                                                 objectContent[commaPos] == '\t' || 
                                                 objectContent[commaPos] == '\n' || 
                                                 objectContent[commaPos] == '\r')) {
            commaPos++;
        }
        
        set(key, value, ConfigSource::File);
        
        if (commaPos < objectContent.size() && objectContent[commaPos] == ',') {
            keyPos = commaPos + 1;
        } else {
            break;
        }
    }
    
    return true;
}

bool Config::parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value) {
    // Skip whitespace
    pos = json.find_first_not_of(" \t\n\r", pos);
    if (pos == std::string::npos) {
        return false;
    }
    
    char c = json[pos];
    
    if (c == '"') {
        // String value
        size_t endQuote = json.find('"', pos + 1);
        if (endQuote == std::string::npos) {
            return false;
        }
        std::string strValue = json.substr(pos + 1, endQuote - pos - 1);
        value = strValue;
        pos = endQuote + 1;
        return true;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
            value = true;
            pos += 4;
            return true;
        } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
            value = false;
            pos += 5;
            return true;
        }
        return false;
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number value
        size_t numEnd = pos;
        while (numEnd < json.size() && (json[numEnd] == '.' || 
                                       (json[numEnd] >= '0' && json[numEnd] <= '9') ||
                                       json[numEnd] == 'e' || json[numEnd] == 'E' ||
                                       json[numEnd] == '+' || json[numEnd] == '-')) {
            numEnd++;
        }
        
        std::string numStr = json.substr(pos, numEnd - pos);
        try {
            // Try to parse as double first
            size_t decimalPos = numStr.find('.');
            if (decimalPos != std::string::npos) {
                value = std::stod(numStr);
            } else {
                // Try int64_t if no decimal point
                try {
                    value = std::stoll(numStr);
                } catch (...) {
                    value = std::stod(numStr);
                }
            }
        } catch (...) {
            return false;
        }
        pos = numEnd;
        return true;
    } else if (c == '[') {
        // Array value
        size_t arrayEnd = pos + 1;
        int bracketCount = 1;
        
        while (arrayEnd < json.size() && bracketCount > 0) {
            if (json[arrayEnd] == '[') {
                bracketCount++;
            } else if (json[arrayEnd] == ']') {
                bracketCount--;
            }
            arrayEnd++;
        }
        
        if (bracketCount > 0) {
            return false;
        }
        
        std::string arrayContent = json.substr(pos + 1, arrayEnd - pos - 2);
        pos = arrayEnd;
        
        // Parse array elements (simple approach)
        std::vector<ConfigValue> arrayValues;
        size_t elementPos = 0;
        
        while (elementPos < arrayContent.size()) {
            ConfigValue element;
            if (!parseJSONValue(arrayContent, elementPos, element)) {
                break;
            }
            
            // Remove quotes for strings in arrays
            if (std::holds_alternative<std::string>(element)) {
                arrayValues.push_back(element);
            }
            
            // Skip whitespace and comma
            while (elementPos < arrayContent.size() && 
                   (arrayContent[elementPos] == ' ' || 
                    arrayContent[elementPos] == '\t' || 
                    arrayContent[elementPos] == '\n' || 
                    arrayContent[elementPos] == '\r' || 
                    arrayContent[elementPos] == ',')) {
                elementPos++;
            }
        }
        
        // For now, only support string arrays
        if (!arrayValues.empty()) {
            value = arrayValues;
        }
        return true;
    } else {
        return false; // Unsupported value type
    }
}

bool Config::parseYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, treat YAML as similar to JSON for basic support
    // In a real implementation, this would use a proper YAML library
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Skip whitespace
    size_t pos = content.find_first_not_of(" \t\n\r");
    if (pos == std::string::npos) {
        return false;
    }
    
    // Check if it's a YAML object (starts with --- or {)
    if ((content.size() > pos + 3 && content.substr(pos, 3) == "---") || content[pos] == '{') {
        return parseJSONObject(content, pos);
    }
    
    return false;
}

bool Config::parseSimpleKeyValueFile(const std::string& filepath) {
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

std::string Config::saveToFile(const std::string& filepath) const {
    // Try to save as JSON
    bool jsonSuccess = saveToJSONFile(filepath);
    if (!jsonSuccess) {
        // Fall back to simple key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return "Failed to open file for writing";
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << entry.key << " = " << arg << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            file << "\n";
        }
    }
    
    return "";
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
