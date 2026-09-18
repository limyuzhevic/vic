#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::string filename;
    bool fileLoaded = false;
    std::string validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()), filename_("") {}

Config::Config(const std::string& filename) : pImpl(std::make_unique<Impl>()), filename_(filename) {
    if (autoLoad_) {
        autoLoad();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    filename_ = filepath;
    pImpl->filename = filepath;
    pImpl->fileLoaded = true;
    
    if (ext == "json") {
        return loadFromJSONFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Try to auto-detect from content
        return loadFromFileAutoDetect(filepath);
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        // Implementation would use nlohmann::json
        // For now, just mark as loaded
        pImpl->fileLoaded = true;
        return true;
    } catch (const std::exception& e) {
        pImpl->validationErrors = "JSON parsing error: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        // Implementation would use nlohmann::yaml
        // For now, just mark as loaded
        pImpl->fileLoaded = true;
        return true;
    } catch (const std::exception& e) {
        pImpl->validationErrors = "YAML parsing error: " + std::string(e.what());
        return false;
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationErrors = "Failed to open JSON file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromJSON(content);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationErrors = "Failed to open YAML file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromYAML(content);
}

bool Config::loadFromFileAutoDetect(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationErrors = "Failed to open file: " + filepath;
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple key=value detection
    bool isKeyValue = false;
    bool isJSON = false;
    bool isYAML = false;
    
    // Check for key=value pattern
    if (firstLine.find('=') != std::string::npos) {
        std::string line = trim(firstLine);
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            if (!key.empty() && !value.empty()) {
                isKeyValue = true;
            }
        }
    }
    
    // Check for JSON pattern
    if (!firstLine.empty() && (firstLine.find('{') != std::string::npos || 
                               firstLine.find('[') != std::string::npos)) {
        isJSON = true;
    }
    
    // Check for YAML pattern
    if (!firstLine.empty() && (firstLine.find(':') != std::string::npos || 
                               firstLine.find('-') != std::string::npos)) {
        isYAML = true;
    }
    
    // Try parsing as key=value (original format)
    if (isKeyValue) {
        return loadFromFile(filepath);
    } else if (isJSON) {
        return loadFromJSONFile(filepath);
    } else if (isYAML) {
        return loadFromYAMLFile(filepath);
    }
    
    pImpl->validationErrors = "Cannot determine file format for: " + filepath;
    return false;
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

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSONFile(filepath, pretty);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAMLFile(filepath, pretty);
    } else {
        // Default to key=value format
        return saveToKeyValueFile(filepath);
    }
}

bool Config::saveToJSONFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Get JSON representation
    std::string jsonStr = saveToJSON(pretty);
    file << jsonStr;
    return true;
}

bool Config::saveToYAMLFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Get YAML representation
    std::string yamlStr = saveToYAML(pretty);
    file << yamlStr;
    return true;
}

bool Config::saveToKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description;
        if (!entry.unit.empty()) {
            file << " (" << entry.unit << ")";
        }
        file << "\n";
        if (!entry.category.empty()) {
            file << "# Category: " << entry.category << "\n";
        }
        
        file << entry.key << " = " << serializeToString(entry.value) << "\n\n";
    }
    
    return true;
}

std::string Config::saveToJSON(bool pretty) const {
    std::ostringstream oss;
    if (pretty) {
        oss << std::setw(2);
    }
    
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << serializeToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}";
    
    return oss.str();
}

std::string Config::saveToYAML(bool pretty) const {
    std::ostringstream oss;
    
    oss << "%YAML 1.2\n---\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << entry.key << ": " << serializeToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << "\n";
        }
    }
    oss << "\n...\n";
    
    return oss.str();
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

template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw ConfigValidationError(key, "Required parameter is missing");
    }
    
    if (pImpl->schema && !validateValue(val.value(), *pImpl->schema->getParameter(key))) {
        throw ConfigValidationError(key, "Parameter validation failed");
    }
    
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Validate if schema exists
        if (pImpl->schema && !validateValue(value, *pImpl->schema->getParameter(key))) {
            pImpl->validationErrors += validationErrorMessage(key, key, "Parameter validation failed\n");
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

std::vector<int> Config::getIntList(const std::string& key) const {
    auto val = get<std::vector<int>>(key);
    return val.has_value() ? val.value() : std::vector<int>();
}

std::vector<double> Config::getDoubleList(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>();
}

std::vector<std::string> Config::getStringList(const std::string& key) const {
    auto val = get<std::vector<std::string>>(key);
    return val.has_value() ? val.value() : std::vector<std::string>();
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    const ParameterDefinition* param = nullptr;
    if (pImpl->schema) {
        param = pImpl->schema->getParameter(key);
    }
    
    return param ? validateValue(it->value, *param) : true;
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

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << 
        (validate() ? "VALID" : "INVALID") << "):\n";
    oss << pImpl->validationErrors << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
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
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (!entry.unit.empty()) {
            oss << " " << entry.unit;
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = std::make_unique<ConfigSchema>(schema);
    
    // Validate existing entries
    pImpl->validationErrors.clear();
    for (const auto& entry : pImpl->entries) {
        if (pImpl->schema && !validateValue(entry.value, *pImpl->schema->getParameter(entry.key))) {
            pImpl->validationErrors += validationErrorMessage(entry.key, entry.key, "Parameter validation failed\n");
        }
    }
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    if (!pImpl->schema) {
        return true; // No validation required
    }
    
    bool isValid = true;
    for (const auto& entry : pImpl->entries) {
        const ParameterDefinition* param = pImpl->schema->getParameter(entry.key);
        if (!param) {
            pImpl->validationErrors += validationErrorMessage(entry.key, "", "Unknown parameter\n");
            isValid = false;
        } else if (!validateValue(entry.value, *param)) {
            isValid = false;
        }
    }
    
    // Check for required parameters
    for (const auto& param : pImpl->schema->parameters_) {
        if (param.second.required && !has(param.first)) {
            pImpl->validationErrors += validationErrorMessage(param.first, param.first, "Required parameter is missing\n");
            isValid = false;
        }
    }
    
    return isValid;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& otherEntry : other.pImpl->entries) {
        // Check if key already exists
        bool exists = false;
        for (auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key) {
                entry.value = otherEntry.value;
                entry.source = source;
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    return (it != pImpl->entries.end()) ? it->source : ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->description : "";
}

std::string Config::getUnit(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->unit : "";
}

std::string Config::getCategory(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->category : "";
}

bool Config::autoLoad() {
    if (!filename_.empty()) {
        return loadFromFile(filename_);
    }
    return false;
}

bool Config::reload() {
    if (!pImpl->filename.empty()) {
        clear();
        return loadFromFile(pImpl->filename);
    }
    return false;
}

bool Config::wasFileLoaded() const {
    return pImpl->fileLoaded;
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

bool Config::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    if (key.empty() || value.empty()) {
        return false;
    }
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return true;
}

std::string Config::serializeToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
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
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

bool Config::tryConvertStringToValue(const std::string& str, ConfigValue& value) {
    // Try to detect type from string
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Check for integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Check for floating point
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Check for list
    if (str.front() == '[' && str.back() == ']') {
        std::string listStr = str.substr(1, str.size() - 2);
        std::vector<int> intList;
        std::vector<double> doubleList;
        std::vector<std::string> stringList;
        
        // Try to parse as list of numbers
        bool isIntList = true;
        bool isDoubleList = true;
        
        std::string token;
        std::stringstream listStream(listStr);
        
        while (std::getline(listStream, token, ',')) {
            token = trim(token);
            if (token.empty()) continue;
            
            // Try integer
            try {
                size_t pos;
                int64_t intVal = std::stoll(token, &pos);
                if (pos == token.size()) {
                    intList.push_back(static_cast<int>(intVal));
                    continue;
                }
                isIntList = false;
            } catch (...) {}
            
            // Try double
            try {
                size_t pos;
                double doubleVal = std::stod(token, &pos);
                if (pos == token.size()) {
                    doubleList.push_back(doubleVal);
                    continue;
                }
                isDoubleList = false;
            } catch (...) {}
            
            // Try string
            if ((token.front() == '"' && token.back() == '"') ||
                (token.front() == '\'' && token.back() == '\'')) {
                stringList.push_back(token.substr(1, token.size() - 2));
                continue;
            } else {
                isIntList = false;
                isDoubleList = false;
            }
        }
        
        if (!intList.empty() && isIntList) {
            value = intList;
            return true;
        } else if (!doubleList.empty() && isDoubleList) {
            value = doubleList;
            return true;
        } else if (!stringList.empty()) {
            value = stringList;
            return true;
        }
    }
    
    // Fall back to string
    value = str;
    return true;
}

bool Config::validateValue(const ConfigValue& value, const ParameterDefinition& param) {
    bool isValid = true;
    std::string errorMsg;
    
    // Check range if specified
    if (param.range.has_value()) {
        double min = std::get<double>(param.range->first);
        double max = std::get<double>(param.range->second);
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, double>) {
                double val = (std::is_same_v<T, int> ? static_cast<double>(arg) :
                             std::is_same_v<T, int64_t> ? static_cast<double>(arg) : arg);
                
                if (val < min || val > max) {
                    isValid = false;
                    errorMsg = "Value " + std::to_string(val) + " outside range [" + 
                              std::to_string(min) + ", " + std::to_string(max) + "]";
                }
            }
        }, value);
    }
    
    // Apply custom validator if provided
    if (param.validator && !param.validator(value)) {
        isValid = false;
        errorMsg = "Custom validation failed";
    }
    
    if (!isValid && !errorMsg.empty()) {
        pImpl->validationErrors += validationErrorMessage(param.name, param.name, errorMsg + "\n");
    }
    
    return isValid;
}

std::string Config::validationErrorMessage(const std::string& key, const std::string& paramName, const std::string& reason) {
    std::string paramInfo = paramName.empty() ? "" : " (" + paramName + ")";
    return "Key \"" + key + "\"" + paramInfo + ": " + reason;
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