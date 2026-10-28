#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Clear existing entries
    clear();
    
    bool success = false;
    
    if (ext == ".json" || ext == ".cfg") {
        success = loadFromJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAMLFile(filepath);
    } else {
        // Fallback to simple key=value format
        success = loadFromSimpleFile(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

// Enhanced JSON file loading
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    try {
        parseJSON(content);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

// Enhanced YAML file loading  
bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    try {
        parseYAML(content);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

// Simple key=value format (backward compatibility)
bool Config::loadFromSimpleFile(const std::string& filepath) {
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

// Enhanced JSON parser implementation
void Config::parseJSON(const std::string& jsonStr) {
    if (jsonStr.empty() || jsonStr == "{}") {
        return;
    }
    
    std::string content = jsonStr;
    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
    content.erase(std::remove(content.begin(), content.end(), ' '), content.end());
    
    if (content[0] == '{' && content[content.size() - 1] == '}') {
        content = content.substr(1, content.size() - 2);
        
        size_t pos = 0;
        while (pos < content.size()) {
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            parseJSONValue(content, valueStart, key);
            
            pos = content.find(',', valueStart);
            if (pos == std::string::npos) break;
            pos++;
        }
    }
}

// Enhanced YAML parser implementation
void Config::parseYAML(const std::string& yamlStr) {
    std::string content = yamlStr;
    
    size_t commentPos;
    while ((commentPos = content.find("#")) != std::string::npos) {
        size_t newLinePos = content.find('\n', commentPos);
        if (newLinePos == std::string::npos) {
            content.erase(commentPos);
            break;
        }
        content.erase(commentPos, newLinePos - commentPos);
    }
    
    size_t pos = 0;
    while (pos < content.size()) {
        size_t keyEnd = content.find(':', pos);
        if (keyEnd == std::string::npos) break;
        
        size_t keyStart = content.find_last_not_of(' ', keyEnd - 1);
        if (keyStart == std::string::npos) break;
        
        std::string key = content.substr(0, keyStart + 1);
        key = trim(key);
        
        content = content.substr(keyEnd + 1);
        
        parseYAMLValue(content, key);
        
        pos = content.find('\n', 0);
        if (pos == std::string::npos) break;
        content = content.substr(pos + 1);
    }
}

// Parse JSON value
void Config::parseJSONValue(std::string& content, size_t startPos, const std::string& key) {
    size_t pos = startPos;
    
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
        pos++;
    }
    
    if (pos >= content.size()) return;
    
    char firstChar = content[pos];
    
    if (firstChar == '"') {
        size_t endQuote = content.find('"', pos + 1);
        if (endQuote == std::string::npos) return;
        
        std::string value = content.substr(pos + 1, endQuote - pos - 1);
        set(key, value, ConfigSource::File);
        
        content = content.substr(endQuote + 1);
    } else if (firstChar == 't' || firstChar == 'f') {
        size_t commaOrBrace = content.find(',', pos);
        if (commaOrBrace == std::string::npos) {
            commaOrBrace = content.find('}', pos);
        }
        if (commaOrBrace == std::string::npos) return;
        
        std::string valueStr = content.substr(pos, commaOrBrace - pos);
        bool value = (valueStr == "true");
        set(key, value, ConfigSource::File);
        
        content = content.substr(commaOrBrace + 1);
    } else if (firstChar == 'n' || (firstChar >= '0' && firstChar <= '9')) {
        size_t commaOrBrace = content.find(',', pos);
        if (commaOrBrace == std::string::npos) {
            commaOrBrace = content.find('}', pos);
        }
        if (commaOrBrace == std::string::npos) return;
        
        std::string valueStr = content.substr(pos, commaOrBrace - pos);
        
        try {
            size_t posAfter = 0;
            double value = std::stod(valueStr, &posAfter);
            if (posAfter == valueStr.size()) {
                set(key, value, ConfigSource::File);
            } else {
                size_t posAfterInt = 0;
                int64_t intValue = std::stoll(valueStr, &posAfterInt);
                if (posAfterInt == valueStr.size()) {
                    set(key, intValue, ConfigSource::File);
                }
            }
        } catch (...) {
            set(key, valueStr, ConfigSource::File);
        }
        
        content = content.substr(commaOrBrace + 1);
    } else if (firstChar == '[') {
        parseJSONArray(content, pos, key);
    }
}

// Parse JSON array
void Config::parseJSONArray(std::string& content, size_t startPos, const std::string& key) {
    size_t pos = content.find('[', startPos);
    if (pos == std::string::npos) return;
    
    size_t endBracket = content.find(']', pos + 1);
    if (endBracket == std::string::npos) return;
    
    std::string arrayContent = content.substr(pos + 1, endBracket - pos - 1);
    
    std::vector<ConfigValue> arrayValues;
    size_t elementPos = 0;
    while (elementPos < arrayContent.size()) {
        size_t commaOrBracket = arrayContent.find(',', elementPos);
        if (commaOrBracket == std::string::npos) {
            commaOrBracket = arrayContent.find(']', elementPos);
        }
        if (commaOrBracket == std::string::npos) break;
        
        std::string elementStr = arrayContent.substr(elementPos, commaOrBracket - elementPos);
        elementStr.erase(0, elementStr.find_first_not_of(' '));
        elementStr.erase(elementStr.find_last_not_of(' ') + 1);
        
        if (!elementStr.empty()) {
            arrayValues.push_back(parseJSONPrimitive(elementStr));
        }
        
        elementPos = commaOrBracket + 1;
        while (elementPos < arrayContent.size() && (arrayContent[elementPos] == ',' || arrayContent[elementPos] == ' ')) {
            elementPos++;
        }
    }
    
    set(key, arrayValues, ConfigSource::File);
    
    content = content.substr(endBracket + 1);
}

// Parse JSON primitive value
ConfigValue Config::parseJSONPrimitive(const std::string& str) {
    if (str.empty()) {
        return std::string();
    }
    
    char firstChar = str[0];
    
    if (firstChar == '"') {
        return str.substr(1, str.size() - 2);
    } else if (firstChar == 't' || firstChar == 'f') {
        return (str == "true");
    } else if (firstChar == 'n') {
        return std::string("null");
    } else if (firstChar == '[' || firstChar == '{') {
        return str;
    } else {
        try {
            size_t posAfter = 0;
            double value = std::stod(str, &posAfter);
            if (posAfter == str.size()) {
                return value;
            }
            
            int64_t intValue = std::stoll(str, &posAfter);
            if (posAfter == str.size()) {
                return intValue;
            }
        } catch (...) {
        }
    }
    
    return str;
}

// Parse YAML value
void Config::parseYAMLValue(std::string& content, const std::string& key) {
    size_t pos = 0;
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
        pos++;
    }
    
    if (pos >= content.size()) return;
    
    size_t endLine = content.find('\n', pos);
    if (endLine == std::string::npos) {
        endLine = content.size();
    }
    
    std::string valueStr = content.substr(pos, endLine - pos);
    valueStr = trim(valueStr);
    
    if (!valueStr.empty()) {
        if (valueStr == "true" || valueStr == "false") {
            bool value = (valueStr == "true");
            set(key, value, ConfigSource::File);
        } else {
            try {
                size_t posAfter = 0;
                double value = std::stod(valueStr, &posAfter);
                if (posAfter == valueStr.size()) {
                    set(key, value, ConfigSource::File);
                } else {
                    size_t posAfterInt = 0;
                    int64_t intValue = std::stoll(valueStr, &posAfterInt);
                    if (posAfterInt == valueStr.size()) {
                        set(key, intValue, ConfigSource::File);
                    } else {
                        set(key, valueStr, ConfigSource::File);
                    }
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::File);
            }
        }
    }
    
    content = content.substr(endLine);
}

// Enhanced saveToFile with JSON output
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        file << "{" << std::endl;
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Output based on value type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << "\"" << arg[j] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
            
            file << (i < pImpl->entries.size() - 1 ? "," : "") << std::endl;
        }
        file << "}" << std::endl;
    } else if (ext == ".yaml" || ext == ".yml") {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << std::endl;
            file << entry.key << ": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << std::endl << std::endl;
        }
    } else {
        // Simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << std::endl;
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << std::endl << std::endl;
        }
    }
    
    return true;
}

// Enhanced summary with file type information
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
    }
    return oss.str();
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
