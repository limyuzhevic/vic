#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

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
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    clear();
    
    try {
        // Simple JSON parsing for basic key-value pairs
        // Remove whitespace and comments for easier parsing
        std::string cleaned;
        for (size_t i = 0; i < content.size(); ++i) {
            if (content[i] == '/' && i + 1 < content.size() && content[i + 1] == '/') {
                // Skip line comment
                while (i < content.size() && content[i] != '\n') i++;
                if (i < content.size()) cleaned += '\n';
            } else if (content[i] == '/' && i + 1 < content.size() && content[i + 1] == '*') {
                // Skip block comment
                while (i < content.size() - 1 && !(content[i] == '*' && content[i + 1] == '/')) {
                    i++;
                    if (i < content.size()) cleaned += content[i];
                }
                if (i < content.size() - 1) {
                    cleaned += content[i++]; // Add '*'
                    cleaned += content[i];   // Add '/'
                }
            } else if (content[i] != '\r') { // Skip carriage returns
                cleaned += content[i];
            }
        }
        
        content = cleaned;
        
        // Parse JSON-like format { "key": value, "key2": value }
        size_t braceStart = content.find('{');
        size_t braceEnd = content.rfind('}');
        if (braceStart == std::string::npos || braceEnd == std::string::npos) {
            return false; // No valid JSON object found
        }
        
        content = content.substr(braceStart + 1, braceEnd - braceStart - 2);
        
        // Parse key-value pairs
        size_t pos = 0;
        while (pos < content.size()) {
            // Find next quote
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon after key
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value (could be string, number, boolean, or array)
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Extract value based on type
            ConfigValue value;
            
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string strValue = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, strValue, ConfigSource::File);
                pos = valueEnd + 1;
            } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                // Boolean value
                size_t valueEnd = content.find_first_of(", }", valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.size();
                std::string boolStr = content.substr(valueStart, valueEnd - valueStart);
                bool boolValue = (boolStr == "true");
                set(key, boolValue, ConfigSource::File);
                pos = valueEnd;
            } else if (content[valueStart] == '[') {
                // Array value - simplified parsing
                size_t arrayEnd = content.find(']', valueStart);
                if (arrayEnd == std::string::npos) break;
                std::string arrayContent = content.substr(valueStart + 1, arrayEnd - valueStart - 1);
                
                // Try to parse as int array
                bool isIntArray = true;
                size_t testPos = 0;
                while (testPos < arrayContent.size()) {
                    size_t numEnd = arrayContent.find_first_of(", ", testPos);
                    if (numEnd == std::string::npos) numEnd = arrayContent.size();
                    std::string numStr = arrayContent.substr(testPos, numEnd - testPos);
                    
                    // Check if it's a valid integer
                    try {
                        size_t endPos;
                        int64_t num = std::stoll(numStr, &endPos);
                        if (endPos != numStr.size()) isIntArray = false;
                    } catch (...) {
                        isIntArray = false;
                    }
                    
                    testPos = numEnd + 1;
                }
                
                if (isIntArray) {
                    std::vector<int64_t> intArray;
                    size_t arrayPos = 0;
                    while (arrayPos < arrayContent.size()) {
                        size_t numEnd = arrayContent.find_first_of(", ", arrayPos);
                        if (numEnd == std::string::npos) numEnd = arrayContent.size();
                        std::string numStr = arrayContent.substr(arrayPos, numEnd - arrayPos);
                        
                        try {
                            int64_t num = std::stoll(numStr);
                            intArray.push_back(num);
                        } catch (...) {
                            break;
                        }
                        
                        arrayPos = numEnd + 1;
                    }
                    set(key, intArray, ConfigSource::File);
                } else {
                    // Try to parse as double array
                    bool isDoubleArray = true;
                    size_t testPos = 0;
                    while (testPos < arrayContent.size()) {
                        size_t numEnd = arrayContent.find_first_of(", ", testPos);
                        if (numEnd == std::string::npos) numEnd = arrayContent.size();
                        std::string numStr = arrayContent.substr(testPos, numEnd - testPos);
                        
                        try {
                            size_t endPos;
                            double num = std::stod(numStr, &endPos);
                            if (endPos != numStr.size()) isDoubleArray = false;
                        } catch (...) {
                            isDoubleArray = false;
                        }
                        
                        testPos = numEnd + 1;
                    }
                    
                    if (isDoubleArray) {
                        std::vector<double> doubleArray;
                        size_t arrayPos = 0;
                        while (arrayPos < arrayContent.size()) {
                            size_t numEnd = arrayContent.find_first_of(", ", arrayPos);
                            if (numEnd == std::string::npos) numEnd = arrayContent.size();
                            std::string numStr = arrayContent.substr(arrayPos, numEnd - arrayPos);
                            
                            try {
                                double num = std::stod(numStr);
                                doubleArray.push_back(num);
                            } catch (...) {
                                break;
                            }
                            
                            arrayPos = numEnd + 1;
                        }
                        set(key, doubleArray, ConfigSource::File);
                    } else {
                        // Try to parse as string array
                        bool isStringArray = true;
                        size_t testPos = 0;
                        while (testPos < arrayContent.size()) {
                            size_t quoteStart = arrayContent.find('"', testPos);
                            if (quoteStart == std::string::npos) break;
                            size_t quoteEnd = arrayContent.find('"', quoteStart + 1);
                            if (quoteEnd == std::string::npos) break;
                            testPos = quoteEnd + 1;
                        }
                        
                        if (isStringArray) {
                            std::vector<std::string> stringArray;
                            size_t arrayPos = 0;
                            while (arrayPos < arrayContent.size()) {
                                size_t quoteStart = arrayContent.find('"', arrayPos);
                                if (quoteStart == std::string::npos) break;
                                size_t quoteEnd = arrayContent.find('"', quoteStart + 1);
                                if (quoteEnd == std::string::npos) break;
                                
                                std::string strValue = arrayContent.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                                stringArray.push_back(strValue);
                                
                                arrayPos = quoteEnd + 1;
                                while (arrayPos < arrayContent.size() && 
                                       (arrayContent[arrayPos] == ' ' || arrayContent[arrayPos] == ',')) {
                                    arrayPos++;
                                }
                            }
                            set(key, stringArray, ConfigSource::File);
                        } else {
                            // Leave as unparsed string (fallback)
                            set(key, arrayContent, ConfigSource::File);
                        }
                    }
                }
                
                pos = arrayEnd + 1;
            } else {
                // Number value
                size_t valueEnd = content.find_first_of(", }", valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.size();
                std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                
                // Try to parse as integer first, then as double
                try {
                    size_t endPos;
                    int64_t intVal = std::stoll(valueStr, &endPos);
                    if (endPos == valueStr.size()) {
                        set(key, intVal, ConfigSource::File);
                        pos = valueEnd;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    size_t endPos;
                    double doubleVal = std::stod(valueStr, &endPos);
                    if (endPos == valueStr.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        pos = valueEnd;
                        continue;
                    }
                } catch (...) {}
                
                // If both fail, treat as string
                set(key, valueStr, ConfigSource::File);
                pos = valueEnd;
            }
            
            // Skip to next comma or end
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            if (pos < content.size() && content[pos] == ',') {
                pos++;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
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
    
    // Write JSON header
    file << "{\n";
    
    bool firstEntry = true;
    for (const auto& entry : pImpl->entries) {
        if (!firstEntry) {
            file << ",\n";
        }
        firstEntry = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Convert value to JSON based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "  // " << entry.description << "\n";
    }
    
    file << "}\n";
    
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
