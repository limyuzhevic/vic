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
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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

bool Config::loadFromFile(const std::string& filepath, ConfigFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError(ConfigFileError::ErrorType::FileNotFound, filepath, "Failed to open file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true;
    }
    
    std::map<std::string, ConfigValue> parsedData;
    bool success = false;
    
    try {
        switch (format) {
            case ConfigFormat::JSON:
                success = parseJson(content, parsedData);
                break;
            case ConfigFormat::YAML:
                success = parseYaml(content, parsedData);
                break;
            case ConfigFormat::Simple:
                success = parseSimpleFormat(content, parsedData);
                break;
        }
        
        if (!success) {
            throw ConfigFileError(ConfigFileError::ErrorType::ParseError, filepath, "Failed to parse file");
        }
        
        clear();
        for (const auto& pair : parsedData) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        
        return true;
    } catch (const ConfigFileError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigFileError(ConfigFileError::ErrorType::ParseError, filepath, "Parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError(ConfigFileError::ErrorType::WriteError, filepath, "Failed to open file for writing: " + filepath);
    }
    
    std::map<std::string, ConfigValue> data;
    for (const auto& entry : pImpl->entries) {
        data[entry.key] = entry.value;
    }
    
    std::string content;
    switch (format) {
        case ConfigFormat::JSON:
            content = serializeToJson(data);
            break;
        case ConfigFormat::YAML:
            content = serializeToYaml(data);
            break;
        case ConfigFormat::Simple:
            content = serializeToSimple(data);
            break;
    }
    
    file << content;
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

// JSON/YAML implementations
ConfigFormat Config::detectFormat(const std::string& filepath) {
    return getFormatFromExtension(filepath);
}

ConfigFormat Config::getFormatFromExtension(const std::string& filepath) {
    if (filepath.empty()) {
        return ConfigFormat::Simple;
    }
    
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return ConfigFormat::JSON;
    } else if (ext == "yaml" || ext == "yml") {
        return ConfigFormat::YAML;
    }
    
    return ConfigFormat::Simple;
}

bool Config::isSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool hasSimpleFormat = false;
    bool hasComplexFormat = false;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) {
            hasSimpleFormat = true;
        } else {
            if (line.front() == '{' || line.front() == '[') {
                hasComplexFormat = true;
            }
        }
    }
    
    return hasSimpleFormat && !hasComplexFormat;
}

bool Config::isValidJson(const std::string& content) {
    return !content.empty() && (content.find('{') != std::string::npos || content.find('[') != std::string::npos);
}

bool Config::isValidYaml(const std::string& content) {
    return !content.empty() && (content.find(':') != std::string::npos || content.find('-') != std::string::npos);
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    if (str.empty()) {
        return std::string("");
    }
    
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                return static_cast<int>(intVal);
            }
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            if (std::abs(doubleVal - std::round(doubleVal)) < 1e-10) {
                if (doubleVal >= static_cast<double>(std::numeric_limits<int64_t>::min()) && 
                    doubleVal <= static_cast<double>(std::numeric_limits<int64_t>::max())) {
                    return static_cast<int64_t>(std::round(doubleVal));
                }
            }
            return doubleVal;
        }
    } catch (...) {}
    
    if (str.front() == '[') {
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        bool isIntArray = true;
        bool isDoubleArray = true;
        std::string content = str.substr(1, str.size() - 2);
        
        size_t pos = 0;
        while (pos < content.size()) {
            size_t endPos = content.find(',', pos);
            if (endPos == std::string::npos) {
                endPos = content.size();
            }
            
            std::string element = trim(content.substr(pos, endPos - pos));
            if (element.empty()) {
                pos = endPos + 1;
                continue;
            }
            
            try {
                size_t epos;
                int64_t val = std::stoll(element, &epos);
                if (epos == element.size()) {
                    intArray.push_back(static_cast<int>(val));
                } else {
                    isIntArray = false;
                    doubleArray.push_back(std::stod(element));
                    isDoubleArray = false;
                }
            } catch (...) {
                isIntArray = false;
                isDoubleArray = false;
                stringArray.push_back(element);
            }
            
            pos = endPos + 1;
        }
        
        if (isIntArray) return intArray;
        if (isDoubleArray) return doubleArray;
        return stringArray;
    }
    
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::abs(arg - std::round(arg)) < 1e-10) {
                oss << static_cast<int64_t>(std::round(arg));
            } else {
                oss << std::setprecision(6) << std::fixed << arg;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << std::fixed << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

bool Config::parseJson(const std::string& content, std::map<std::string, ConfigValue>& out) {
    std::string data = content;
    
    bool inString = false;
    char quoteChar = '"';
    std::string cleaned;
    
    for (size_t i = 0; i < data.size(); ++i) {
        char c = data[i];
        if (c == '"' && (i == 0 || data[i-1] != '\\')) {
            inString = !inString;
            quoteChar = c;
        }
        
        if (!inString && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
            continue;
        }
        
        cleaned += c;
    }
    
    if (cleaned.front() != '{' || cleaned.back() != '}') {
        return false;
    }
    
    std::string objContent = cleaned.substr(1, cleaned.size() - 2);
    size_t pos = 0;
    
    while (pos < objContent.size()) {
        size_t keyPos = objContent.find('"', pos);
        if (keyPos == std::string::npos) break;
        
        size_t keyEnd = objContent.find('"', keyPos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = objContent.substr(keyPos + 1, keyEnd - keyPos - 1);
        
        size_t colonPos = objContent.find(':', keyEnd + 1);
        if (colonPos == std::string::npos) break;
        
        size_t valuePos = colonPos + 1;
        while (valuePos < objContent.size() && (objContent[valuePos] == ' ' || objContent[valuePos] == '\n')) {
            valuePos++;
        }
        
        char valueChar = objContent[valuePos];
        size_t valueEnd;
        
        if (valueChar == '"') {
            valueEnd = objContent.find('"', valuePos + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = objContent.substr(valuePos + 1, valueEnd - valuePos - 1);
            out[key] = valueStr;
            pos = valueEnd + 2;
        } else if (valueChar == 't' || valueChar == 'f') {
            if (objContent.substr(valuePos, 4) == "true") {
                out[key] = true;
                pos = valuePos + 4;
            } else if (objContent.substr(valuePos, 5) == "false") {
                out[key] = false;
                pos = valuePos + 5;
            } else {
                break;
            }
        } else if (valueChar == '-' || (valueChar >= '0' && valueChar <= '9')) {
            size_t numEnd = valuePos;
            while (numEnd < objContent.size() && (objContent[numEnd] == '-' || objContent[numEnd] == '+' || 
                                                 objContent[numEnd] == '.' || 
                                                 (objContent[numEnd] >= '0' && objContent[numEnd] <= '9') ||
                                                 objContent[numEnd] == 'e' || objContent[numEnd] == 'E')) {
                numEnd++;
            }
            
            std::string numStr = objContent.substr(valuePos, numEnd - valuePos);
            try {
                size_t npos;
                int64_t intVal = std::stoll(numStr, &npos);
                if (npos == numStr.size()) {
                    out[key] = intVal;
                } else {
                    double doubleVal = std::stod(numStr, &npos);
                    if (npos == numStr.size()) {
                        out[key] = doubleVal;
                    }
                }
            } catch (...) {}
            
            pos = numEnd;
        } else if (valueChar == '[') {
            int bracketCount = 1;
            size_t arrayEnd = valuePos + 1;
            while (arrayEnd < objContent.size() && bracketCount > 0) {
                if (objContent[arrayEnd] == '[') bracketCount++;
                else if (objContent[arrayEnd] == ']') bracketCount--;
                arrayEnd++;
            }
            
            std::string arrayStr = objContent.substr(valuePos, arrayEnd - valuePos);
            out[key] = stringToConfigValue(arrayStr);
            pos = arrayEnd;
        } else {
            break;
        }
        
        while (pos < objContent.size() && (objContent[pos] == ',' || objContent[pos] == ' ' || objContent[pos] == '\n')) {
            pos++;
        }
    }
    
    return true;
}

bool Config::parseYaml(const std::string& content, std::map<std::string, ConfigValue>& out) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        if (key.empty()) {
            continue;
        }
        
        if (!value.empty() && ((value.front() == '"' && value.back() == '"') ||
                              (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        out[key] = stringToConfigValue(value);
    }
    
    return true;
}

bool Config::parseSimpleFormat(const std::string& content, std::map<std::string, ConfigValue>& out) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                continue;
            }
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            out[key] = stringToConfigValue(value);
        }
    }
    
    return true;
}

std::string Config::serializeToJson(const std::map<std::string, ConfigValue>& data) {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << pair.first << "\": ";
        oss << json::configValueToJson(pair.second);
    }
    
    oss << "\n}";
    return oss.str();
}

std::string Config::serializeToYaml(const std::map<std::string, ConfigValue>& data) {
    std::ostringstream oss;
    
    for (const auto& pair : data) {
        oss << pair.first << ": ";
        oss << yaml::configValueToYaml(pair.second, false) << "\n";
    }
    
    return oss.str();
}

std::string Config::serializeToSimple(const std::map<std::string, ConfigValue>& data) {
    std::ostringstream oss;
    
    for (const auto& pair : data) {
        oss << "# " << "configuration value\n";
        oss << pair.first << " = " << configValueToString(pair.second) << "\n\n";
    }
    
    return oss.str();
}

void Config::setSchema(const ConfigSchema& schema) {
    schema_ = schema;
}

std::vector<ConfigValidationError> Config::validate() const {
    std::vector<ConfigValidationError> errors;
    
    std::unordered_set<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            errors.emplace_back(ConfigValidationError::ErrorType::DuplicateKey, entry.key, 
                              "Duplicate key: " + entry.key);
        }
        seenKeys.insert(entry.key);
    }
    
    if (!schema_.typeMap.empty()) {
        std::map<std::string, ConfigValue> currentValues;
        for (const auto& entry : pImpl->entries) {
            currentValues[entry.key] = entry.value;
        }
        
        if (!schema_.validateRequired(currentValues)) {
            for (const auto& requiredKey : schema_.requiredKeys) {
                errors.emplace_back(ConfigValidationError::ErrorType::MissingRequiredField, requiredKey,
                                  "Missing required field: " + requiredKey);
            }
        }
        
        for (const auto& entry : pImpl->entries) {
            if (!schema_.validate(entry.key, entry.value)) {
                errors.emplace_back(ConfigValidationError::ErrorType::InvalidType, entry.key,
                                  "Invalid type for key: " + entry.key);
            }
        }
    }
    
    return errors;
}

bool Config::loadFromJson(const std::string& filepath) {
    return loadFromFile(filepath, ConfigFormat::JSON);
}

bool Config::saveToJson(const std::string& filepath) const {
    return saveToFile(filepath, ConfigFormat::JSON);
}

bool Config::loadFromYaml(const std::string& filepath) {
    return loadFromFile(filepath, ConfigFormat::YAML);
}

bool Config::saveToYaml(const std::string& filepath) const {
    return saveToFile(filepath, ConfigFormat::YAML);
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