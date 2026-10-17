// Improved Config.cpp implementation with better type safety and validation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <regex>
#include <stdexcept>
#include <chrono>
#include <random>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    
    // Configuration statistics
    std::map<ConfigSource, size_t> sourceCounts;
    size_t totalEntries;
    
    Impl() : totalEntries(0) {
        sourceCounts[ConfigSource::Default] = 0;
        sourceCounts[ConfigSource::File] = 0;
        sourceCounts[ConfigSource::CommandLine] = 0;
        sourceCounts[ConfigSource::Runtime] = 0;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for JSON parsing and string conversion
namespace {
    
    // Simple JSON value parsing
    std::string parseJsonValue(const std::string& jsonStr) {
        if (jsonStr.empty()) return "";
        
        if (jsonStr.front() == '"' && jsonStr.back() == '"') {
            return jsonStr.substr(1, jsonStr.size() - 2);
        }
        
        try {
            size_t pos;
            int intValue = std::stoi(jsonStr);
            return std::to_string(intValue);
        } catch (...) {
            try {
                double doubleValue = std::stod(jsonStr);
                // Format to remove trailing zeros
                std::ostringstream oss;
                oss << std::fixed << doubleValue;
                std::string result = oss.str();
                // Remove trailing zeros and potential decimal point
                size_t decimalPos = result.find('.');
                if (decimalPos != std::string::npos) {
                    size_t endpos = result.find_last_not_of('0');
                    if (endpos != std::string::npos && endpos != decimalPos) {
                        result.erase(endpos + 1);
                    }
                    if (result.back() == '.') {
                        result.pop_back();
                    }
                }
                return result;
            } catch (...) {
                return jsonStr; // Return as-is if parsing fails
            }
        }
    }
    
    std::string jsonToString(const std::string& jsonStr) {
        return parseJsonValue(jsonStr);
    }
    
    int jsonToInt(const std::string& jsonStr) {
        return parseJsonValue(jsonStr).empty() ? 0 : std::stoi(parseJsonValue(jsonStr));
    }
    
    double jsonToDouble(const std::string& jsonStr) {
        return parseJsonValue(jsonStr).empty() ? 0.0 : std::stod(parseJsonValue(jsonStr));
    }
    
    bool jsonToBool(const std::string& jsonStr) {
        std::string normalized = toLower(parseJsonValue(jsonStr));
        return normalized == "true" || normalized == "1" || normalized == "yes" || normalized == "on";
    }
    
    std::vector<int> jsonToIntArray(const std::string& jsonStr) {
        // Simple array parsing - expect [1,2,3] format
        if (jsonStr.empty() || jsonStr.front() != '[' || jsonStr.back() != ']') {
            return {};
        }
        
        std::string content = jsonStr.substr(1, jsonStr.size() - 2);
        std::vector<int> result;
        
        size_t pos = 0;
        while (pos < content.size()) {
            size_t commaPos = content.find(',', pos);
            std::string element;
            if (commaPos == std::string::npos) {
                element = content.substr(pos);
                pos = content.size();
            } else {
                element = content.substr(pos, commaPos - pos);
                pos = commaPos + 1;
            }
            
            element = trim(element);
            try {
                result.push_back(std::stoi(element));
            } catch (...) {
                // Skip invalid elements
            }
        }
        
        return result;
    }
    
    std::vector<double> jsonToDoubleArray(const std::string& jsonStr) {
        // Simple array parsing - expect [1.5,2.7,3.0] format
        if (jsonStr.empty() || jsonStr.front() != '[' || jsonStr.back() != ']') {
            return {};
        }
        
        std::string content = jsonStr.substr(1, jsonStr.size() - 2);
        std::vector<double> result;
        
        size_t pos = 0;
        while (pos < content.size()) {
            size_t commaPos = content.find(',', pos);
            std::string element;
            if (commaPos == std::string::npos) {
                element = content.substr(pos);
                pos = content.size();
            } else {
                element = content.substr(pos, commaPos - pos);
                pos = commaPos + 1;
            }
            
            element = trim(element);
            try {
                result.push_back(std::stod(element));
            } catch (...) {
                // Skip invalid elements
            }
        }
        
        return result;
    }
    
    std::vector<std::string> jsonToStringArray(const std::string& jsonStr) {
        // Simple array parsing - expect ["a","b","c"] format
        if (jsonStr.empty() || jsonStr.front() != '[' || jsonStr.back() != ']') {
            return {};
        }
        
        std::string content = jsonStr.substr(1, jsonStr.size() - 2);
        std::vector<std::string> result;
        
        size_t pos = 0;
        bool inQuotes = false;
        std::string currentElement;
        
        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];
            
            if (c == '"' && (i == 0 || content[i-1] != '\\')) {
                inQuotes = !inQuotes;
                if (!inQuotes && !currentElement.empty()) {
                    result.push_back(currentElement);
                    currentElement.clear();
                }
            } else if (c == ',' && !inQuotes) {
                if (!currentElement.empty()) {
                    result.push_back(currentElement);
                    currentElement.clear();
                }
            } else if (!inQuotes) {
                currentElement += c;
            }
        }
        
        if (!currentElement.empty()) {
            result.push_back(currentElement);
        }
        
        return result;
    }
    
    std::string boolToJson(bool value) {
        return value ? "true" : "false";
    }
    
    std::string intToJson(int value) {
        return std::to_string(value);
    }
    
    std::string doubleToJson(double value) {
        std::ostringstream oss;
        oss << std::fixed << value;
        std::string result = oss.str();
        // Remove trailing zeros
        size_t decimalPos = result.find('.');
        if (decimalPos != std::string::npos) {
            size_t endpos = result.find_last_not_of('0');
            if (endpos != std::string::npos && endpos != decimalPos) {
                result.erase(endpos + 1);
            }
            if (result.back() == '.') {
                result.pop_back();
            }
        }
        return result;
    }
    
    std::string stringToJson(const std::string& value) {
        std::string result = value;
        // Escape special characters
        size_t pos = 0;
        while ((pos = result.find('"', pos)) != std::string::npos) {
            result.replace(pos, 1, "\\\"");
            pos += 2;
        }
        while ((pos = result.find('\\', pos)) != std::string::npos) {
            result.replace(pos, 1, "\\\\");
            pos += 2;
        }
        return '"' + result + '"';
    }
    
    std::string vectorToJson(const std::vector<int>& vec) {
        std::ostringstream oss;
        oss << '[';
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ',';
            oss << vec[i];
        }
        oss << ']';
        return oss.str();
    }
    
    std::string vectorToJson(const std::vector<double>& vec) {
        std::ostringstream oss;
        oss << '[';
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ',';
            oss << intToJson(static_cast<int>(vec[i])); // Use integer representation for simplicity
        }
        oss << ']';
        return oss.str();
    }
    
    std::string vectorToJson(const std::vector<std::string>& vec) {
        std::ostringstream oss;
        oss << '[';
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ',';
            oss << stringToJson(vec[i]);
        }
        oss << ']';
        return oss.str();
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return loadFromJson(content);
    }
    
    // Fall back to simple key=value format
    file.open(filepath);
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
            
            safeSet(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear(); // Clear existing config for clean argument parsing
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                safeSet(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            safeSet(key, value, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::loadFromJson(const std::string& jsonString) {
    clear();
    
    try {
        // Simple JSON parsing - expect format like {"key1": "value1", "key2": 123, "key3": true}
        if (jsonString.empty() || jsonString.front() != '{' || jsonString.back() != '}') {
            return false;
        }
        
        std::string content = jsonString.substr(1, jsonString.size() - 2);
        size_t pos = 0;
        
        while (pos < content.size()) {
            // Find next key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon
            size_t colonPos = content.find(':', keyEnd + 1);
            if (colonPos == std::string::npos) break;
            
            // Find value start
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Find value end (next comma or closing brace)
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.find('}', valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.size();
            }
            
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            
            // Parse value based on type
            std::string trimmedValue = trim(valueStr);
            ConfigValue value;
            
            if (trimmedValue == "true" || trimmedValue == "false") {
                value = jsonToBool(trimmedValue);
            } else if (trimmedValue.front() == '[' && trimmedValue.back() == ']') {
                // Check if it's a number array or string array
                if (trimmedValue.find('"') != std::string::npos) {
                    value = jsonToStringArray(trimmedValue);
                } else if (trimmedValue.find('.') != std::string::npos) {
                    value = jsonToDoubleArray(trimmedValue);
                } else {
                    value = jsonToIntArray(trimmedValue);
                }
            } else if (trimmedValue.find('.') != std::string::npos) {
                try {
                    double doubleVal = jsonToDouble(trimmedValue);
                    value = doubleVal;
                } catch (...) {
                    value = jsonToString(trimmedValue);
                }
            } else {
                try {
                    int intVal = jsonToInt(trimmedValue);
                    value = intVal;
                } catch (...) {
                    value = jsonToString(trimmedValue);
                }
            }
            
            safeSet(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    clear();
    
    // This would require access to environment variables
    // For now, just return true to indicate no error
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    
    return true;
}

std::string Config::saveToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": ";
        
        switch (entry.value.index()) {
            case 0: // int
            case 1: // int64_t
                oss << intToJson(std::get<int>(entry.value));
                break;
            case 2: // double
                oss << intToJson(static_cast<int>(std::get<double>(entry.value))); // Simplified for now
                break;
            case 3: // bool
                oss << boolToJson(std::get<bool>(entry.value));
                break;
            case 4: // string
                oss << stringToJson(std::get<std::string>(entry.value));
                break;
            case 5: // vector<int>
                oss << vectorToJson(std::get<std::vector<int>>(entry.value));
                break;
            case 6: // vector<double>
                oss << vectorToJson(std::get<std::vector<double>>(entry.value));
                break;
            case 7: // vector<string>
                oss << vectorToJson(std::get<std::vector<std::string>>(entry.value));
                break;
        }
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
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

int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto validation = validate(value);
    if (!validation.valid) {
        return false;
    }
    
    // Update source counts
    if (has(key)) {
        pImpl->sourceCounts[getEntry(key).source]--;
    }
    
    // Remove existing entry
    remove(key);
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source);
    pImpl->sourceCounts[source]++;
    pImpl->totalEntries++;
    
    return true;
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    safeSet(key, value, source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    safeSet(key, value, source);
}

bool Config::safeSet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value
    auto validation = validate(value);
    if (!validation.valid) {
        return false;
    }
    
    // Update source counts
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->sourceCounts[it->source]--;
    }
    
    // Remove existing entry or add new one
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->sourceCounts[source]++;
    pImpl->totalEntries = pImpl->entries.size();
    
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->sourceCounts[it->source]--;
        pImpl->entries.erase(it);
    }
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
    pImpl->sourceCounts.clear();
    pImpl->totalEntries = 0;
    
    // Reset source counts
    pImpl->sourceCounts[ConfigSource::Default] = 0;
    pImpl->sourceCounts[ConfigSource::File] = 0;
    pImpl->sourceCounts[ConfigSource::CommandLine] = 0;
    pImpl->sourceCounts[ConfigSource::Runtime] = 0;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << configValueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    
    return oss.str();
}

bool Config::validateSchema(const std::vector<ConfigSchema>& schema) const {
    clear(); // Clear existing config
    
    for (const auto& schemaEntry : schema) {
        addSchema(schemaEntry);
    }
    
    ValidationResult result = validate();
    return result.valid;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    std::vector<std::string> warnings;
    
    // Check required fields
    for (const auto& schema : pImpl->schemas) {
        if (schema.second.required && !has(schema.first)) {
            result.valid = false;
            result.errorMessage += "Missing required field: " + schema.first + "\n";
        }
    }
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryValidation = validateAgainstSchema(entry);
        if (!entryValidation.valid) {
            result.valid = false;
            result.errorMessage += "Field \"" + entry.key + "\": " + entryValidation.errorMessage + "\n";
        }
        
        // Collect warnings
        warnings.insert(warnings.end(), entryValidation.warnings.begin(), entryValidation.warnings.end());
    }
    
    result.warnings = warnings;
    return result;
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

void Config::removeSchema(const std::string& key) {
    pImpl->schemas.erase(key);
}

std::vector<std::string> Config::getSchemaKeys() const {
    std::vector<std::string> keys;
    for (const auto& schema : pImpl->schemas) {
        keys.push_back(schema.first);
    }
    return keys;
}

std::string Config::getSchemaDescription(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    return it != pImpl->schemas.end() ? it->second.description : "";
}

bool Config::applyPreset(const std::string& presetName) {
    // This would apply a named preset configuration
    // For now, just return false
    return false;
}

std::vector<std::string> Config::getAvailablePresets() const {
    // Return list of available presets
    return {"default", "performance", "development", "production"};
}

bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        safeSet(entry.key, entry.value, entry.source);
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::map<ConfigSource, size_t> Config::getSourceCounts() const {
    return pImpl->sourceCounts;
}

std::vector<std::string> Config::findKeysByPattern(const std::string& pattern) const {
    std::vector<std::string> results;
    std::regex regex(pattern, std::regex::icase);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, regex)) {
            results.push_back(entry.key);
        }
    }
    
    return results;
}

std::vector<ConfigEntry> Config::filterBySource(ConfigSource source) const {
    std::vector<ConfigEntry> results;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            results.push_back(entry);
        }
    }
    return results;
}

std::vector<ConfigEntry> Config::filterByType(const std::string& typeName) const {
    std::vector<ConfigEntry> results;
    
    auto getTypeName = [this](const ConfigEntry& entry) {
        return this->getTypeName(entry.value);
    };
    
    for (const auto& entry : pImpl->entries) {
        if (getTypeName(entry) == typeName) {
            results.push_back(entry);
        }
    }
    
    return results;
}

bool Config::pythonSafeSet(const std::string& key, const pybind11::object& value) {
    // Convert Python object to ConfigValue
    try {
        if (pybind11::isinstance<pybind11::int_>(value)) {
            set(key, value.cast<int>());
        } else if (pybind11::isinstance<pybind11::float_>(value)) {
            set(key, value.cast<double>());
        } else if (pybind11::isinstance<pybind11::bool_>(value)) {
            set(key, value.cast<bool>());
        } else if (pybind11::isinstance<pybind11::str>(value)) {
            set(key, value.cast<std::string>());
        } else if (pybind11::isinstance<pybind11::list>(value)) {
            pybind11::list pyList = value.cast<pybind11::list>();
            std::vector<int> intList;
            std::vector<double> doubleList;
            std::vector<std::string> stringList;
            
            bool allInts = true;
            bool allFloats = true;
            bool allStrings = true;
            
            for (size_t i = 0; i < pyList.size(); ++i) {
                pybind11::object item = pyList[i];
                if (pybind11::isinstance<pybind11::int_>(item)) {
                    if (allStrings || allFloats) allStrings = allFloats = false;
                    intList.push_back(item.cast<int>());
                } else if (pybind11::isinstance<pybind11::float_>(item)) {
                    if (allInts || allStrings) allInts = allStrings = false;
                    doubleList.push_back(item.cast<double>());
                } else if (pybind11::isinstance<pybind11::str>(item)) {
                    if (allInts || allFloats) allInts = allFloats = false;
                    stringList.push_back(item.cast<std::string>());
                } else {
                    allInts = allFloats = allStrings = false;
                    break;
                }
            }
            
            if (allInts) {
                set(key, intList);
            } else if (allFloats) {
                set(key, doubleList);
            } else if (allStrings) {
                set(key, stringList);
            } else {
                return false; // Unsupported type
            }
        } else {
            return false; // Unsupported type
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

pybind11::object Config::pythonGet(const std::string& key) const {
    auto val = getByType(key); // Need to implement this
    if (!val.has_value()) {
        return pybind11::none();
    }
    
    // Convert ConfigValue to Python object
    // This would need to handle all variant types
    return pybind11::none(); // Placeholder
}

// Validation helper implementations
ValidationResult Config::validate(const ConfigValue& value) const {
    switch (value.index()) {
        case 0: // int
        case 1: // int64_t
            return validateInt(value);
        case 2: // double
            return validateDouble(value);
        case 3: // bool
            return validateBool(value);
        case 4: // string
            return validateString(value);
        case 5: // vector<int>
            return validateIntList(value);
        case 6: // vector<double>
            return validateDoubleList(value);
        case 7: // vector<string>
            return validateStringList(value);
        default:
            return ValidationResult(false, "Unsupported configuration value type");
    }
}

ValidationResult Config::validateInt(const ConfigValue& value, int minValue, int maxValue) {
    int val = std::get<int>(value);
    if (val < minValue || val > maxValue) {
        return ValidationResult(false, "Value " + std::to_string(val) + " out of range [" + 
                                 std::to_string(minValue) + ", " + std::to_string(maxValue) + "]");
    }
    return ValidationResult(true);
}

ValidationResult Config::validateDouble(const ConfigValue& value, double minValue, double maxValue) {
    double val = std::get<double>(value);
    if (val < minValue || val > maxValue) {
        return ValidationResult(false, "Value " + std::to_string(val) + " out of range [" + 
                                 std::to_string(minValue) + ", " + std::to_string(maxValue) + "]");
    }
    return ValidationResult(true);
}

ValidationResult Config::validateBool(const ConfigValue& value) {
    // Bool validation always passes
    return ValidationResult(true);
}

ValidationResult Config::validateString(const ConfigValue& value, size_t minLength, size_t maxLength) {
    std::string val = std::get<std::string>(value);
    if (val.size() < minLength || val.size() > maxLength) {
        return ValidationResult(false, "String length " + std::to_string(val.size()) + 
                                 " out of range [" + std::to_string(minLength) + ", " + 
                                 std::to_string(maxLength) + "]");
    }
    return ValidationResult(true);
}

ValidationResult Config::validateIntList(const ConfigValue& value, size_t maxSize) {
    auto val = std::get<std::vector<int>>(value);
    if (val.size() > maxSize) {
        return ValidationResult(false, "List size " + std::to_string(val.size()) + 
                                 " exceeds maximum " + std::to_string(maxSize));
    }
    return ValidationResult(true);
}

ValidationResult Config::validateDoubleList(const ConfigValue& value, size_t maxSize) {
    auto val = std::get<std::vector<double>>(value);
    if (val.size() > maxSize) {
        return ValidationResult(false, "List size " + std::to_string(val.size()) + 
                                 " exceeds maximum " + std::to_string(maxSize));
    }
    return ValidationResult(true);
}

ValidationResult Config::validateStringList(const ConfigValue& value, size_t maxSize) {
    auto val = std::get<std::vector<std::string>>(value);
    if (val.size() > maxSize) {
        return ValidationResult(false, "List size " + std::to_string(val.size()) + 
                                 " exceeds maximum " + std::to_string(maxSize));
    }
    
    // Check individual string lengths
    for (const auto& str : val) {
        if (str.size() > 1000) { // Reasonable max length
            return ValidationResult(false, "String \"" + str + "\" exceeds maximum length of 1000");
        }
    }
    
    return ValidationResult(true);
}

ValidationResult Config::validateAgainstSchema(const ConfigEntry& entry) const {
    auto it = pImpl->schemas.find(entry.key);
    if (it == pImpl->schemas.end()) {
        return ValidationResult(true); // No schema defined, validation passes
    }
    
    const ConfigSchema& schema = it->second;
    ValidationResult result = validate(entry.value);
    
    if (!result.valid) {
        return result;
    }
    
    // Check type compatibility
    std::string actualType = getTypeName(entry.value);
    if (schema.type != "any" && schema.type != actualType) {
        return ValidationResult(false, "Type mismatch: expected " + schema.type + ", got " + actualType);
    }
    
    // Check constraints
    if (schema.validator) {
        ValidationResult constraintResult = schema.validator(entry.value);
        if (!constraintResult.valid) {
            return constraintResult;
        }
    }
    
    return ValidationResult(true);
}

std::string Config::getTypeName(const ConfigValue& value) const {
    switch (value.index()) {
        case 0: // int
        case 1: // int64_t
            return "int";
        case 2: // double
            return "float";
        case 3: // bool
            return "bool";
        case 4: // string
            return "string";
        case 5: // vector<int>
            return "list<int>";
        case 6: // vector<double>
            return "list<float>";
        case 7: // vector<string>
            return "list<string>";
        default:
            return "unknown";
    }
}

std::string Config::configValueToString(const ConfigValue& value) const {
    switch (value.index()) {
        case 0: // int
            return std::to_string(std::get<int>(value));
        case 1: // int64_t
            return std::to_string(std::get<int64_t>(value));
        case 2: // double
            return intToJson(static_cast<int>(std::get<double>(value))); // Simplified
        case 3: // bool
            return boolToJson(std::get<bool>(value));
        case 4: // string
            return stringToJson(std::get<std::string>(value));
        case 5: // vector<int>
            return vectorToJson(std::get<std::vector<int>>(value));
        case 6: // vector<double>
            return vectorToJson(std::get<std::vector<double>>(value));
        case 7: // vector<string>
            return vectorToJson(std::get<std::vector<std::string>>(value));
        default:
            return "unknown";
    }
}

// Template instantiations
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
