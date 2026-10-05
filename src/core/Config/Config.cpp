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
        return false;
    }
    
    // Determine file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    
    if (extension == ".json") {
        return loadFromJSONFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Enhanced JSON parsing with error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parser for basic key-value pairs and arrays
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Remove whitespace for simpler parsing (can be enhanced)
        size_t pos = 0;
        while ((pos = content.find_first_of(" \t\n\r", pos)) != std::string::npos) {
            content.erase(pos, 1);
        }
        
        // Look for key-value pairs in format "key":"value" or 'key':'value'
        size_t keyPos = 0;
        while ((keyPos = content.find('"', keyPos)) != std::string::npos) {
            size_t keyEnd = content.find('"', keyPos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyPos + 1, keyEnd - keyPos - 1);
            keyPos = keyEnd + 1;
            
            // Find colon
            size_t colonPos = content.find(':', keyPos);
            if (colonPos == std::string::npos) break;
            
            keyPos = colonPos + 1;
            
            // Find value
            if (content[keyPos] == '"') {
                // String value
                size_t valueEnd = content.find('"', keyPos + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(keyPos + 1, valueEnd - keyPos - 1);
                keyPos = valueEnd + 1;
                set(key, value, ConfigSource::File);
            } else if (content[keyPos] == '[') {
                // Array value - extract simple array
                size_t arrayEnd = content.find(']', keyPos);
                if (arrayEnd == std::string::npos) break;
                std::string arrayContent = content.substr(keyPos + 1, arrayEnd - keyPos - 1);
                
                // Parse simple array elements (integers, floats, strings)
                std::vector<ConfigValue> arrayValues;
                size_t elemPos = 0;
                while (elemPos < arrayContent.size()) {
                    size_t elemEnd = arrayContent.find(',', elemPos);
                    if (elemEnd == std::string::npos) {
                        elemEnd = arrayContent.size();
                    }
                    
                    std::string element = trim(arrayContent.substr(elemPos, elemEnd - elemPos));
                    ConfigValue elemValue;
                    
                    if (element.empty()) {
                        elemPos = elemEnd + 1;
                        continue;
                    }
                    
                    // Try to determine type
                    if (element[0] == '"' || element[0] == '\'') {
                        elemValue = element.substr(1, element.size() - 2);
                    } else if (element == "true" || element == "false") {
                        elemValue = (element == "true");
                    } else {
                        // Try numeric parsing
                        try {
                            size_t decimalPos = element.find('.');
                            if (decimalPos != std::string::npos) {
                                elemValue = std::stod(element);
                            } else {
                                elemValue = std::stoll(element);
                            }
                        } catch (...) {
                            elemValue = element; // Fallback to string
                        }
                    }
                    
                    arrayValues.push_back(elemValue);
                    elemPos = elemEnd + 1;
                    if (elemPos < arrayContent.size() && arrayContent[elemPos] == ' ') {
                        elemPos++;
                    }
                }
                
                set(key, arrayValues, ConfigSource::File);
                keyPos = arrayEnd + 1;
            } else {
                // Number or boolean value
                size_t valueEnd = std::min(content.find(',', keyPos), content.find('}', keyPos));
                if (valueEnd == std::string::npos) valueEnd = content.size();
                std::string value = content.substr(keyPos, valueEnd - keyPos);
                
                ConfigValue configValue;
                if (value == "true") {
                    configValue = true;
                } else if (value == "false") {
                    configValue = false;
                } else {
                    try {
                        size_t decimalPos = value.find('.');
                        if (decimalPos != std::string::npos) {
                            configValue = std::stod(value);
                        } else {
                            configValue = std::stoll(value);
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
                
                set(key, configValue, ConfigSource::File);
                keyPos = valueEnd;
            }
            
            // Skip to next key or end
            if (keyPos < content.size() && content[keyPos] == ',') {
                keyPos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // YAML parsing support with simple implementation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string line;
        std::string currentKey;
        bool inMapping = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // Skip comments and empty lines
            if (line.empty() || line.substr(0, 2) == "# " || line.substr(0, 3) == "#\t") {
                continue;
            }
            
            // Handle indentation (simple YAML parser)
            size_t indent = line.find_first_not_of(" \t");
            bool isNested = (indent > 0);
            
            if (!isNested) {
                inMapping = false;
                currentKey.clear();
            }
            
            // Parse key: value format (YAML style)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                if (value.empty()) {
                    // Handle null values
                    set(key, std::string(""), ConfigSource::File);
                } else if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::File);
                } else if (value[0] == '\"' || value[0] == '\'') {
                    // String value
                    if (value.size() >= 2 && value.back() == value[0]) {
                        value = value.substr(1, value.size() - 2);
                    }
                    set(key, value, ConfigSource::File);
                } else {
                    // Try numeric parsing
                    try {
                        size_t decimalPos = value.find('.');
                        if (decimalPos != std::string::npos) {
                            set(key, std::stod(value), ConfigSource::File);
                        } else {
                            set(key, std::stoll(value), ConfigSource::File);
                        }
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                }
                
                currentKey = key;
                inMapping = true;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value parser for backward compatibility
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

// Forward declarations for implementation functions
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    
    if (extension == ".json") {
        return loadFromJSONFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Enhanced JSON parsing with error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parser for basic key-value pairs and arrays
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Remove whitespace for simpler parsing (can be enhanced)
        size_t pos = 0;
        while ((pos = content.find_first_of(" \t\n\r", pos)) != std::string::npos) {
            content.erase(pos, 1);
        }
        
        // Look for key-value pairs in format "key":"value" or 'key':'value'
        size_t keyPos = 0;
        while ((keyPos = content.find('"', keyPos)) != std::string::npos) {
            size_t keyEnd = content.find('"', keyPos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyPos + 1, keyEnd - keyPos - 1);
            keyPos = keyEnd + 1;
            
            // Find colon
            size_t colonPos = content.find(':', keyPos);
            if (colonPos == std::string::npos) break;
            
            keyPos = colonPos + 1;
            
            // Find value
            if (content[keyPos] == '"') {
                // String value
                size_t valueEnd = content.find('"', keyPos + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(keyPos + 1, valueEnd - keyPos - 1);
                keyPos = valueEnd + 1;
                set(key, value, ConfigSource::File);
            } else if (content[keyPos] == '[') {
                // Array value - extract simple array
                size_t arrayEnd = content.find(']', keyPos);
                if (arrayEnd == std::string::npos) break;
                std::string arrayContent = content.substr(keyPos + 1, arrayEnd - keyPos - 1);
                
                // Parse simple array elements (integers, floats, strings)
                std::vector<ConfigValue> arrayValues;
                size_t elemPos = 0;
                while (elemPos < arrayContent.size()) {
                    size_t elemEnd = arrayContent.find(',', elemPos);
                    if (elemEnd == std::string::npos) {
                        elemEnd = arrayContent.size();
                    }
                    
                    std::string element = trim(arrayContent.substr(elemPos, elemEnd - elemPos));
                    ConfigValue elemValue;
                    
                    if (element.empty()) {
                        elemPos = elemEnd + 1;
                        continue;
                    }
                    
                    // Try to determine type
                    if (element[0] == '"' || element[0] == '\'') {
                        elemValue = element.substr(1, element.size() - 2);
                    } else if (element == "true" || element == "false") {
                        elemValue = (element == "true");
                    } else {
                        // Try numeric parsing
                        try {
                            size_t decimalPos = element.find('.');
                            if (decimalPos != std::string::npos) {
                                elemValue = std::stod(element);
                            } else {
                                elemValue = std::stoll(element);
                            }
                        } catch (...) {
                            elemValue = element; // Fallback to string
                        }
                    }
                    
                    arrayValues.push_back(elemValue);
                    elemPos = elemEnd + 1;
                    if (elemPos < arrayContent.size() && arrayContent[elemPos] == ' ') {
                        elemPos++;
                    }
                }
                
                set(key, arrayValues, ConfigSource::File);
                keyPos = arrayEnd + 1;
            } else {
                // Number or boolean value
                size_t valueEnd = std::min(content.find(',', keyPos), content.find('}', keyPos));
                if (valueEnd == std::string::npos) valueEnd = content.size();
                std::string value = content.substr(keyPos, valueEnd - keyPos);
                
                ConfigValue configValue;
                if (value == "true") {
                    configValue = true;
                } else if (value == "false") {
                    configValue = false;
                } else {
                    try {
                        size_t decimalPos = value.find('.');
                        if (decimalPos != std::string::npos) {
                            configValue = std::stod(value);
                        } else {
                            configValue = std::stoll(value);
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
                
                set(key, configValue, ConfigSource::File);
                keyPos = valueEnd;
            }
            
            // Skip to next key or end
            if (keyPos < content.size() && content[keyPos] == ',') {
                keyPos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // YAML parsing support with simple implementation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string line;
        std::string currentKey;
        bool inMapping = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // Skip comments and empty lines
            if (line.empty() || line.substr(0, 2) == "# " || line.substr(0, 3) == "#\t") {
                continue;
            }
            
            // Handle indentation (simple YAML parser)
            size_t indent = line.find_first_not_of(" \t");
            bool isNested = (indent > 0);
            
            if (!isNested) {
                inMapping = false;
                currentKey.clear();
            }
            
            // Parse key: value format (YAML style)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                if (value.empty()) {
                    // Handle null values
                    set(key, std::string(""), ConfigSource::File);
                } else if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::File);
                } else if (value[0] == '\"' || value[0] == '\'') {
                    // String value
                    if (value.size() >= 2 && value.back() == value[0]) {
                        value = value.substr(1, value.size() - 2);
                    }
                    set(key, value, ConfigSource::File);
                } else {
                    // Try numeric parsing
                    try {
                        size_t decimalPos = value.find('.');
                        if (decimalPos != std::string::npos) {
                            set(key, std::stod(value), ConfigSource::File);
                        } else {
                            set(key, std::stoll(value), ConfigSource::File);
                        }
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                }
                
                currentKey = key;
                inMapping = true;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value parser for backward compatibility
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

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    
    if (extension == ".json") {
        return saveToJSONFile(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    // Save configuration in JSON format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    size_t entryCount = pImpl->entries.size();
    size_t entryIndex = 0;
    
    for (const auto& entry : pImpl->entries) {
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(arg[i]) << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (entryIndex < entryCount - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
        
        entryIndex++;
    }
    
    file << "}\n";
    return true;
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

std::string Config::escapeString(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    
    return escaped;
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
