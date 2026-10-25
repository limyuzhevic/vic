#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Read the entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Simple JSON parsing for our format:
    // { "entries": [ { "key": "value", "source": 0, "description": "", "value": 42 } ] }
    try {
        // Find the entries array start
        size_t entriesStart = content.find("\"entries\": [");
        if (entriesStart == std::string::npos) {
            return false; // Not in expected format
        }
        
        entriesStart += 15; // Skip "\"entries\": ["
        size_t entriesEnd = content.find("]", entriesStart);
        if (entriesEnd == std::string::npos) {
            return false; // Malformed JSON
        }
        
        std::string entriesSection = content.substr(entriesStart, entriesEnd - entriesStart);
        
        // Parse each entry object in the array
        size_t pos = 0;
        while (pos < entriesSection.length()) {
            // Find start of entry object
            size_t braceStart = entriesSection.find("{", pos);
            if (braceStart == std::string::npos) break;
            
            // Find matching closing brace
            int braceCount = 1;
            size_t bracePos = braceStart + 1;
            while (bracePos < entriesSection.length() && braceCount > 0) {
                if (entriesSection[bracePos] == '{') braceCount++;
                else if (entriesSection[bracePos] == '}') braceCount--;
                bracePos++;
            }
            
            if (braceCount != 0) {
                return false; // Malformed JSON
            }
            
            size_t braceEnd = bracePos;
            std::string entryJson = entriesSection.substr(braceStart, braceEnd - braceStart);
            
            // Parse key
            size_t keyStart = entryJson.find("\"key\":\"");
            if (keyStart == std::string::npos) {
                return false; // Missing key
            }
            keyStart += 9; // Skip "\"key\":\""
            size_t keyEnd = entryJson.find("\"", keyStart);
            if (keyEnd == std::string::npos) {
                return false; // Malformed key
            }
            std::string key = entryJson.substr(keyStart, keyEnd - keyStart);
            
            // Parse source
            size_t sourceStart = entryJson.find("\"source\":");
            if (sourceStart == std::string::npos) {
                return false; // Missing source
            }
            sourceStart += 11; // Skip "\"source\":"
            size_t sourceEnd = entryJson.find(",", sourceStart);
            if (sourceEnd == std::string::npos) {
                sourceEnd = entryJson.find("}", sourceStart);
                if (sourceEnd == std::string::npos) {
                    return false; // Malformed source
                }
            }
            int sourceVal;
            try {
                sourceVal = std::stoi(entryJson.substr(sourceStart, sourceEnd - sourceStart));
            } catch (...) {
                return false; // Invalid source value
            }
            ConfigSource source = static_cast<ConfigSource>(sourceVal);
            
            // Parse description (optional)
            size_t descStart = entryJson.find("\"description\":\"");
            std::string description = "";
            if (descStart != std::string::npos) {
                descStart += 19; // Skip "\"description\":\""
                size_t descEnd = entryJson.find("\"", descStart);
                if (descEnd != std::string::npos) {
                    description = entryJson.substr(descStart, descEnd - descStart);
                }
            }
            
            // Parse value
            size_t valueStart = entryJson.find("\"value\":");
            if (valueStart == std::string::npos) {
                return false; // Missing value
            }
            valueStart += 10; // Skip "\"value\":"
            size_t valueEnd = entryJson.find("}", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = entryJson.length();
            }
            std::string valueStr = entryJson.substr(valueStart, valueEnd - valueStart);
            
            // Parse value based on type (try different types)
            ConfigValue value;
            bool parsed = false;
            
            // Try int
            try {
                size_t pos = 0;
                int intVal = std::stoi(valueStr, pos);
                if (pos == valueStr.length()) {
                    value = intVal;
                    parsed = true;
                }
            } catch (...) {}
            
            // Try int64_t if not parsed
            if (!parsed) {
                try {
                    size_t pos = 0;
                    int64_t int64Val = std::stoll(valueStr, pos);
                    if (pos == valueStr.length()) {
                        value = int64Val;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double if not parsed
            if (!parsed) {
                try {
                    size_t pos = 0;
                    double doubleVal = std::stod(valueStr, pos);
                    if (pos == valueStr.length()) {
                        value = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try bool if not parsed
            if (!parsed) {
                if (valueStr == "true") {
                    value = true;
                    parsed = true;
                } else if (valueStr == "false") {
                    value = false;
                    parsed = true;
                }
            }
            
            // Try string if not parsed (remove quotes)
            if (!parsed) {
                if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                                             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    value = valueStr.substr(1, valueStr.size() - 2);
                    parsed = true;
                }
            }
            
            // Try vector<int> if not parsed
            if (!parsed && valueStr.front() == '[' && valueStr.back() == ']') {
                std::string inner = valueStr.substr(1, valueStr.size() - 2);
                std::vector<int> intVec;
                size_t pos = 0;
                std::string token;
                while ((pos = inner.find(",")) != std::string::npos) {
                    token = inner.substr(0, pos);
                    token.erase(0, token.find_first_not_of(" \t\r\n"));
                    token.erase(token.find_last_not_of(" \t\r\n") + 1);
                    if (!token.empty()) {
                        try {
                            intVec.push_back(std::stoi(token));
                        } catch (...) {
                            intVec.clear();
                            break;
                        }
                    }
                    inner.erase(0, pos + 1);
                }
                if (!inner.empty()) {
                    inner.erase(0, inner.find_first_not_of(" \t\r\n"));
                    inner.erase(inner.find_last_not_of(" \t\r\n") + 1);
                    if (!inner.empty()) {
                        try {
                            intVec.push_back(std::stoi(inner));
                        } catch (...) {
                            intVec.clear();
                        }
                    }
                }
                if (!intVec.empty()) {
                    value = intVec;
                    parsed = true;
                }
            }
            
            // Try vector<double> if not parsed
            if (!parsed && valueStr.front() == '[' && valueStr.back() == ']') {
                std::string inner = valueStr.substr(1, valueStr.size() - 2);
                std::vector<double> doubleVec;
                size_t pos = 0;
                std::string token;
                while ((pos = inner.find(",")) != std::string::npos) {
                    token = inner.substr(0, pos);
                    token.erase(0, token.find_first_not_of(" \t\r\n"));
                    token.erase(token.find_last_not_of(" \t\r\n") + 1);
                    if (!token.empty()) {
                        try {
                            doubleVec.push_back(std::stod(token));
                        } catch (...) {
                            doubleVec.clear();
                            break;
                        }
                    }
                    inner.erase(0, pos + 1);
                }
                if (!inner.empty()) {
                    inner.erase(0, inner.find_first_not_of(" \t\r\n"));
                    inner.erase(inner.find_last_not_of(" \t\r\n") + 1);
                    if (!inner.empty()) {
                        try {
                            doubleVec.push_back(std::stod(inner));
                        } catch (...) {
                            doubleVec.clear();
                        }
                    }
                }
                if (!doubleVec.empty()) {
                    value = doubleVec;
                    parsed = true;
                }
            }
            
            // Try vector<string> if not parsed
            if (!parsed && valueStr.front() == '[' && valueStr.back() == ']') {
                std::string inner = valueStr.substr(1, valueStr.size() - 2);
                std::vector<std::string> stringVec;
                size_t pos = 0;
                std::string token;
                while ((pos = inner.find(",")) != std::string::npos) {
                    token = inner.substr(0, pos);
                    // Remove quotes from string elements
                    token.erase(0, token.find_first_not_of(" \t\r\n\"'"));
                    token.erase(token.find_last_not_of(" \t\r\n\"'") + 1);
                    if (!token.empty()) {
                        stringVec.push_back(token);
                    }
                    inner.erase(0, pos + 1);
                }
                if (!inner.empty()) {
                    inner.erase(0, inner.find_first_not_of(" \t\r\n\"'"));
                    inner.erase(inner.find_last_not_of(" \t\r\n\"'") + 1);
                    if (!inner.empty()) {
                        stringVec.push_back(inner);
                    }
                }
                if (!stringVec.empty()) {
                    value = stringVec;
                    parsed = true;
                }
            }
            
            if (!parsed) {
                return false; // Could not parse value
            }
            
            set(key, value, source);
            
            pos = braceEnd + 1;
            
            // Skip comma and whitespace
            while (pos < entriesSection.length() && (entriesSection[pos] == ',' || 
                                                   entriesSection[pos] == ' ' || entriesSection[pos] == '\n' || 
                                                   entriesSection[pos] == '\r' || entriesSection[pos] == '\t')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false; // Exception during parsing
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
    // Validate file path
    if (filepath.empty()) {
        return false;
    }
    
    // Check if we have write permissions by testing if we can create/overwrite the file
    std::ofstream testFile(filepath, std::ios::trunc);
    if (!testFile.is_open()) {
        return false;
    }
    testFile.close();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write JSON header
    file << "{\n";
    
    // Write config entries as JSON array
    file << "  \"entries\": [\n";
    
    bool firstEntry = true;
    for (const auto& entry : pImpl->entries) {
        if (!firstEntry) {
            file << ",\n";
        }
        firstEntry = false;
        
        file << "    {\n";
        file << "      \"key\": \"" << entry.key << "\"," << std::endl;
        file << "      \"source\": " << static_cast<int>(entry.source) << "," << std::endl;
        file << "      \"description\": \"" << entry.description << "\"," << std::endl;
        file << "      \"value\": ";
        
        // Write the value based on its type using JSON serialization
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(10) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << std::setprecision(10) << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << "\"" << val[i] << "\"";
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        file << std::endl << "    }";
    }
    
    file << "\n  ]\n";
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
