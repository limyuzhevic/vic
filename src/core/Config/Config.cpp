#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for type conversion and validation
namespace {

template<typename T>
bool convertStringToValue(const std::string& str, T& out) {
    std::istringstream iss(str);
    iss >> out;
    return !iss.fail() && iss.eof();
}

bool convertStringToBool(const std::string& str, bool& out) {
    if (str == "true" || str == "1" || str == "yes") {
        out = true;
        return true;
    } else if (str == "false" || str == "0" || str == "no") {
        out = false;
        return true;
    }
    return false;
}

bool convertStringToArray(const std::string& str, std::vector<int>& out) {
    if (str.empty() || str.front() != '[' || str.back() != ']') {
        return false;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        try {
            out.push_back(std::stoi(item));
        } catch (...) {
            return false;
        }
    }
    return true;
}

bool convertStringToArray(const std::string& str, std::vector<double>& out) {
    if (str.empty() || str.front() != '[' || str.back() != ']') {
        return false;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        try {
            out.push_back(std::stod(item));
        } catch (...) {
            return false;
        }
    }
    return true;
}

bool convertStringToArray(const std::string& str, std::vector<std::string>& out) {
    if (str.empty() || str.front() != '[' || str.back() != ']') {
        return false;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        // Remove quotes from string elements
        if (item.size() >= 2 && 
            ((item.front() == '"' && item.back() == '"') ||
             (item.front() == '\'' && item.back() == '\''))) {
            item = item.substr(1, item.size() - 2);
        }
        out.push_back(item);
    }
    return true;
}

std::string convertValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool validateConfigEntry(const ConfigEntry& entry) {
    // Basic validation - can be extended with more sophisticated checks
    if (entry.key.empty()) {
        return false;
    }
    
    // Type-specific validation
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        // Currently all types are considered valid
        static_assert(std::is_same_v<T, int, int64_t, double, bool, std::string, 
                     std::vector<int>, std::vector<double>, std::vector<std::string>>, "Unsupported type");
    }, entry.value);
    
    return true;
}

} // namespace

// Add comprehensive error handling to Config class
namespace {

// Error logging function
void logConfigError(const std::string& operation, const std::string& key, const std::string& message) {
    // In a real implementation, this would use the NLM logging system
    // For now, we'll provide detailed error information
    std::cerr << "[Config Error] " << operation << " failed for key '" << key << "': " << message << std::endl;
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    // First try to parse as JSON format
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        rapidjson::IStreamWrapper isw(jsonFile);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        
        if (!doc.HasParseError() && doc.IsObject()) {
            // Parse as JSON object
            for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
                std::string key = it->name.GetString();
                
                // Convert RapidJSON value to ConfigValue
                ConfigValue value;
                if (it->value.IsInt()) {
                    value = it->value.GetInt();
                } else if (it->value.IsInt64()) {
                    value = it->value.GetInt64();
                } else if (it->value.IsUint64()) {
                    value = static_cast<int64_t>(it->value.GetUint64());
                } else if (it->value.IsDouble()) {
                    value = it->value.GetDouble();
                } else if (it->value.IsBool()) {
                    value = it->value.GetBool();
                } else if (it->value.IsString()) {
                    value = std::string(it->value.GetString());
                } else if (it->value.IsArray()) {
                    // Handle arrays
                    const auto& array = it->value.GetArray();
                    if (!array.Empty()) {
                        if (array[0].IsInt()) {
                            std::vector<int> intArray;
                            intArray.reserve(array.Size());
                            for (const auto& elem : array) {
                                intArray.push_back(elem.GetInt());
                            }
                            value = intArray;
                        } else if (array[0].IsDouble()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(array.Size());
                            for (const auto& elem : array) {
                                doubleArray.push_back(elem.GetDouble());
                            }
                            value = doubleArray;
                        } else if (array[0].IsString()) {
                            std::vector<std::string> stringArray;
                            stringArray.reserve(array.Size());
                            for (const auto& elem : array) {
                                stringArray.push_back(elem.GetString());
                            }
                            value = stringArray;
                        } else {
                            logConfigError("JSON Array Parsing", key, "Unsupported array element type");
                            continue;
                        }
                    }
                } else {
                    logConfigError("JSON Parsing", key, "Unsupported JSON value type");
                    continue;
                }
                
                // Set the value with appropriate description
                set(key, value, ConfigSource::File);
            }
            return true;
        } else {
            if (doc.HasParseError()) {
                logConfigError("JSON File", filepath, "JSON parsing error: " + std::to_string(doc.GetParseError()) + " at offset " + std::to_string(doc.GetErrorOffset()));
            } else {
                logConfigError("JSON File", filepath, "Invalid JSON document - expected object");
            }
        }
    }
    
    // Fallback to simple key=value format if JSON parsing failed
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logConfigError("File Open", filepath, "Cannot open file for reading");
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Convert string value to appropriate type with error handling
            ConfigValue value;
            bool conversionSuccess = false;
            
            // Try to parse as int
            if (!valueStr.empty() && std::all_of(valueStr.begin(), valueStr.end(), ::isdigit)) {
                try {
                    value = std::stoi(valueStr);
                    conversionSuccess = true;
                } catch (const std::exception& e) {
                    logConfigError("Int Conversion", key, "Failed to parse integer: " + std::string(e.what()));
                }
            }
            // Try to parse as double
            else {
                bool isDouble = false;
                size_t decimalPos = valueStr.find('.');
                if (decimalPos != std::string::npos) {
                    std::string before = valueStr.substr(0, decimalPos);
                    std::string after = valueStr.substr(decimalPos + 1);
                    isDouble = std::all_of(before.begin(), before.end(), ::isdigit) &&
                              std::all_of(after.begin(), after.end(), ::isdigit);
                }
                if (isDouble) {
                    try {
                        value = std::stod(valueStr);
                        conversionSuccess = true;
                    } catch (const std::exception& e) {
                        logConfigError("Double Conversion", key, "Failed to parse double: " + std::string(e.what()));
                    }
                }
                // Try to parse as bool
                else if (valueStr == "true") {
                    value = true;
                    conversionSuccess = true;
                } else if (valueStr == "false") {
                    value = false;
                    conversionSuccess = true;
                } else {
                    // Default to string
                    value = valueStr;
                    conversionSuccess = true;
                }
            }
            
            if (conversionSuccess) {
                set(key, value, ConfigSource::File);
            } else {
                logConfigError("Value Conversion", key, "Failed to convert value at line " + std::to_string(lineNum));
            }
        } else {
            logConfigError("Line Parsing", "", "Invalid format at line " + std::to_string(lineNum) + ": '=' not found");
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
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

// Add comprehensive error handling to Config class methods
namespace {

// Error logging function
void logConfigError(const std::string& operation, const std::string& key, const std::string& message) {
    // In a real implementation, this would use the NLM logging system
    // For now, we'll provide detailed error information
    std::cerr << "[Config Error] " << operation << " failed for key '" << key << "': " << message << std::endl;
}

// Success logging function
void logConfigSuccess(const std::string& operation, const std::string& key) {
    // In a real implementation, this would use the NLM logging system
    std::cout << "[Config Success] " << operation << " for key '" << key << "'" << std::endl;
}

} // namespace

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logConfigError("File Save", filepath, "Cannot open file for writing");
        return false;
    }
    
    bool success = true;
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation with error handling
        try {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << "\"" << arg[i] << "\"";
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            logConfigSuccess("File Save", entry.key);
        } catch (const std::exception& e) {
            logConfigError("File Save", entry.key, "Failed to serialize value: " + std::string(e.what()));
            success = false;
        }
        
        file << "\n\n";
    }
    
    if (success) {
        logConfigSuccess("File Save Operation", filepath);
    }
    
    return success;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        logConfigError("Value Get", key, "Key not found in configuration");
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        logConfigSuccess("Value Get", key);
        return value;
    } catch (const std::bad_variant_access&) {
        logConfigError("Type Mismatch", key, "Requested type " + std::string(typeid(T).name()) + " does not match stored type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        logConfigSuccess("Value GetOr", key + " (not found, using default)");
        return defaultValue;
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the key-value pair
    ConfigEntry newEntry(key, value, source);
    if (!validateConfigEntry(newEntry)) {
        logConfigError("Value Set", key, "Invalid key-value pair (empty key or unsupported type)");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        logConfigSuccess("Value Update", key);
        it->value = value;
        it->source = source;
    } else {
        logConfigSuccess("Value Set", key);
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
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (result) {
        logConfigSuccess("Key Check", key + " (found)");
    } else {
        logConfigSuccess("Key Check", key + " (not found)");
    }
    
    return result;
}

void Config::remove(const std::string& key) {
    auto count = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (count > 0) {
        pImpl->entries.erase(pImpl->entries.end() - count, pImpl->entries.end());
        logConfigSuccess("Key Remove", key);
    } else {
        logConfigSuccess("Key Remove", key + " (not found)");
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    logConfigSuccess("Keys Get", "Retrieved " + std::to_string(keys.size()) + " keys");
    return keys;
}

void Config::clear() {
    size_t count = pImpl->entries.size();
    pImpl->entries.clear();
    logConfigSuccess("Clear All", "Cleared " + std::to_string(count) + " entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "============================" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << std::endl;
        oss << "  Value: [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (Source: " << static_cast<int>(entry.source) << ")" << std::endl;
        oss << "  Description: " << entry.description << std::endl;
        oss << "  Type: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) oss << "int";
            else if constexpr (std::is_same_v<T, int64_t>) oss << "int64_t";
            else if constexpr (std::is_same_v<T, double>) oss << "double";
            else if constexpr (std::is_same_v<T, bool>) oss << "bool";
            else if constexpr (std::is_same_v<T, std::string>) oss << "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) oss << "vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) oss << "vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) oss << "vector<string>";
        }, entry.value);
        oss << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateConfigEntry(entry)) {
            return false;
        }
    }
    return true;
}

bool Config::validate(const std::string& key, const std::type_info& expectedType) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    // Check if the value type matches expected type
    if (expectedType == typeid(int)) {
        return std::holds_alternative<int>(it->value);
    } else if (expectedType == typeid(int64_t)) {
        return std::holds_alternative<int64_t>(it->value);
    } else if (expectedType == typeid(double)) {
        return std::holds_alternative<double>(it->value);
    } else if (expectedType == typeid(bool)) {
        return std::holds_alternative<bool>(it->value);
    } else if (expectedType == typeid(std::string)) {
        return std::holds_alternative<std::string>(it->value);
    } else if (expectedType == typeid(std::vector<int>)) {
        return std::holds_alternative<std::vector<int>>(it->value);
    } else if (expectedType == typeid(std::vector<double>)) {
        return std::holds_alternative<std::vector<double>>(it->value);
    } else if (expectedType == typeid(std::vector<std::string>)) {
        return std::holds_alternative<std::vector<std::string>>(it->value);
    }
    
    return false; // Unknown type
}

std::string Config::getSchema() const {
    std::ostringstream oss;
    oss << "Configuration Schema:" << std::endl;
    oss << "==================" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << std::endl;
        oss << "  Type: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) oss << "int";
            else if constexpr (std::is_same_v<T, int64_t>) oss << "int64_t";
            else if constexpr (std::is_same_v<T, double>) oss << "double";
            else if constexpr (std::is_same_v<T, bool>) oss << "bool";
            else if constexpr (std::is_same_v<T, std::string>) oss << "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) oss << "vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) oss << "vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) oss << "vector<string>";
        }, entry.value);
        oss << std::endl;
        oss << "  Source: " << static_cast<int>(entry.source) << std::endl;
        oss << "  Description: " << entry.description << std::endl;
        oss << std::endl;
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