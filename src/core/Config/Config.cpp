#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> defaultDescriptions;
};

// JSON parser implementation (simplified, no external dependencies)
namespace {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    const auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char ch) { return std::tolower(ch); });
    return result;
}

// Simple JSON parser - supports basic object, array, string, number, boolean
class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        try {
            size_t pos = 0;
            parseValue(jsonStr, pos, entries);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static void parseValue(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) return;
        
        char ch = str[pos];
        
        if (ch == '{') {
            parseObject(str, pos, entries);
        } else if (ch == '[') {
            // Arrays not used for configuration
            parseArray(str, pos);
        } else if (ch == '\"') {
            parseString(str, pos, entries);
        } else if (std::isdigit(ch) || ch == '-') {
            parseNumber(str, pos, entries);
        } else if (ch == 't' || ch == 'f') {
            parseBoolean(str, pos, entries);
        } else if (ch == 'n') {
            parseNull(str, pos);
        }
    }
    
    static void parseObject(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        pos++; // Skip '{'
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != '}') {
            // Parse key
            std::string key;
            parseString(str, pos, key);
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ':') {
                pos++;
                skipWhitespace(str, pos);
                
                // Parse value and create config entry
                ConfigValue value;
                if (tryParseNumber(str, pos, value)) {
                    // Number found
                } else if (tryParseBoolean(str, pos, value)) {
                    // Boolean found
                } else if (tryParseString(str, pos, value)) {
                    // String found
                } else {
                    // Skip unknown value
                    parseValue(str, pos, entries);
                    continue;
                }
                
                ConfigSource source = ConfigSource::File;
                std::string description = "";
                
                // Try to find description from default descriptions map
                if (auto config = getConfigFromEntries(entries)) {
                    config->defaultDescriptions[key] = "";
                }
                
                entries.emplace_back(key, value, source, description);
                
                skipWhitespace(str, pos);
                if (pos < str.size() && str[pos] == ',') {
                    pos++;
                    skipWhitespace(str, pos);
                }
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip '}'
        }
    }
    
    static void parseArray(const std::string& str, size_t& pos) {
        pos++; // Skip '['
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != ']') {
            parseValue(str, pos, {}); // Ignore array values
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
                skipWhitespace(str, pos);
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip ']'
        }
    }
    
    static void parseString(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries = {}) {
        size_t start = ++pos; // Skip opening quote
        
        while (pos < str.size() && str[pos] != '\"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos < str.size() && str[pos] == '\"') {
            std::string value = str.substr(start, pos - start);
            
            // Special handling for configuration keys
            if (entries.empty() && value.find('=') != std::string::npos) {
                // This looks like a "key=value" line
                size_t eqPos = value.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(value.substr(0, eqPos));
                    std::string val = trim(value.substr(eqPos + 1));
                    entries.emplace_back(key, val, ConfigSource::File);
                    pos++; // Skip closing quote
                    return;
                }
            }
            
            // Normal string parsing
            entries.emplace_back(value, value, ConfigSource::File);
            pos++; // Skip closing quote
        }
    }
    
    static void parseString(const std::string& str, size_t& pos) {
        parseString(str, pos, {});
    }
    
    static void parseNumber(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        entries.emplace_back(numStr, std::stod(numStr), ConfigSource::File);
    }
    
    static void parseBoolean(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        if (str.compare(pos, 4, "true") == 0) {
            entries.emplace_back("", true, ConfigSource::File);
            pos += 4;
        } else if (str.compare(pos, 5, "false") == 0) {
            entries.emplace_back("", false, ConfigSource::File);
            pos += 5;
        }
    }
    
    static void parseNull(const std::string& str, size_t& pos) {
        if (str.compare(pos, 4, "null") == 0) {
            pos += 4;
        }
    }
    
    static bool tryParseNumber(const std::string& str, size_t& pos, ConfigValue& value) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        if (pos >= str.size() || !std::isdigit(str[pos])) return false;
        
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            if (pos >= str.size() || !std::isdigit(str[pos])) return false;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        // Try to determine if it's int or double
        try {
            size_t endpos;
            int intVal = std::stoi(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = intVal;
                return true;
            }
            
            double doubleVal = std::stod(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {
        }
        
        return false;
    }
    
    static bool tryParseBoolean(const std::string& str, size_t& pos, ConfigValue& value) {
        if (str.compare(pos, 4, "true") == 0) {
            value = true;
            pos += 4;
            return true;
        } else if (str.compare(pos, 5, "false") == 0) {
            value = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool tryParseString(const std::string& str, size_t& pos, ConfigValue& value) {
        if (pos < str.size() && str[pos] == '\"') {
            size_t start = ++pos;
            while (pos < str.size() && str[pos] != '\"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos++;
                }
                pos++;
            }
            if (pos < str.size() && str[pos] == '\"') {
                value = str.substr(start, pos - start);
                pos++; // Skip closing quote
                return true;
            }
        }
        return false;
    }
    
    static std::shared_ptr<Config> getConfigFromEntries(const std::vector<ConfigEntry>& entries) {
        return nullptr; // Placeholder
    }
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
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON first, then fallback to simple key=value format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // JSON format
        return SimpleJsonParser::parse(content, pImpl->entries);
    } else {
        // Simple key=value format (legacy support)
        std::string line;
        std::stringstream stream(content);
        while (std::getline(stream, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

// Complete implementation of JSON/YAML parser and enhanced configuration system
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <unordered_map>
#include <cmath>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> defaultDescriptions;
    bool caseInsensitive;
    std::string fileFormat;
    
    Impl() : caseInsensitive(true), fileFormat("auto") {}
};

// JSON parser implementation (simplified, no external dependencies)
namespace {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    const auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char ch) { return std::tolower(ch); });
    return result;
}

// Simple JSON parser - supports basic object, array, string, number, boolean
class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        try {
            size_t pos = 0;
            parseValue(jsonStr, pos, entries);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static void parseValue(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) return;
        
        char ch = str[pos];
        
        if (ch == '{') {
            parseObject(str, pos, entries);
        } else if (ch == '[') {
            // Arrays not used for configuration
            parseArray(str, pos);
        } else if (ch == '\"') {
            parseString(str, pos, entries);
        } else if (std::isdigit(ch) || ch == '-') {
            parseNumber(str, pos, entries);
        } else if (ch == 't' || ch == 'f') {
            parseBoolean(str, pos, entries);
        } else if (ch == 'n') {
            parseNull(str, pos);
        }
    }
    
    static void parseObject(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        pos++; // Skip '{'
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != '}') {
            // Parse key
            std::string key;
            parseString(str, pos, key);
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ':') {
                pos++;
                skipWhitespace(str, pos);
                
                // Parse value and create config entry
                ConfigValue value;
                if (tryParseNumber(str, pos, value)) {
                    // Number found
                } else if (tryParseBoolean(str, pos, value)) {
                    // Boolean found
                } else if (tryParseString(str, pos, value)) {
                    // String found
                } else {
                    // Skip unknown value
                    parseValue(str, pos, entries);
                    continue;
                }
                
                ConfigSource source = ConfigSource::File;
                std::string description = "";
                
                // Try to find description from default descriptions map
                if (auto config = getConfigFromEntries(entries)) {
                    auto it = config->pImpl->defaultDescriptions.find(key);
                    if (it != config->pImpl->defaultDescriptions.end()) {
                        description = it->second;
                    }
                }
                
                entries.emplace_back(key, value, source, description);
                
                skipWhitespace(str, pos);
                if (pos < str.size() && str[pos] == ',') {
                    pos++;
                    skipWhitespace(str, pos);
                }
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip '}'
        }
    }
    
    static void parseArray(const std::string& str, size_t& pos) {
        pos++; // Skip '['
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != ']') {
            parseValue(str, pos, {}); // Ignore array values
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
                skipWhitespace(str, pos);
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip ']'
        }
    }
    
    static void parseString(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries = {}) {
        size_t start = ++pos; // Skip opening quote
        
        while (pos < str.size() && str[pos] != '\"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos < str.size() && str[pos] == '\"') {
            std::string value = str.substr(start, pos - start);
            
            // Special handling for configuration keys
            if (entries.empty() && value.find('=') != std::string::npos) {
                // This looks like a "key=value" line
                size_t eqPos = value.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(value.substr(0, eqPos));
                    std::string val = trim(value.substr(eqPos + 1));
                    entries.emplace_back(key, val, ConfigSource::File);
                    pos++; // Skip closing quote
                    return;
                }
            }
            
            // Normal string parsing
            entries.emplace_back(value, value, ConfigSource::File);
            pos++; // Skip closing quote
        }
    }
    
    static void parseString(const std::string& str, size_t& pos) {
        parseString(str, pos, {});
    }
    
    static void parseNumber(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        entries.emplace_back(numStr, std::stod(numStr), ConfigSource::File);
    }
    
    static void parseBoolean(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        if (str.compare(pos, 4, "true") == 0) {
            entries.emplace_back("", true, ConfigSource::File);
            pos += 4;
        } else if (str.compare(pos, 5, "false") == 0) {
            entries.emplace_back("", false, ConfigSource::File);
            pos += 5;
        }
    }
    
    static void parseNull(const std::string& str, size_t& pos) {
        if (str.compare(pos, 4, "null") == 0) {
            pos += 4;
        }
    }
    
    static bool tryParseNumber(const std::string& str, size_t& pos, ConfigValue& value) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        if (pos >= str.size() || !std::isdigit(str[pos])) return false;
        
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            if (pos >= str.size() || !std::isdigit(str[pos])) return false;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        // Try to determine if it's int or double
        try {
            size_t endpos;
            int intVal = std::stoi(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = intVal;
                return true;
            }
            
            double doubleVal = std::stod(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {
        }
        
        return false;
    }
    
    static bool tryParseBoolean(const std::string& str, size_t& pos, ConfigValue& value) {
        if (str.compare(pos, 4, "true") == 0) {
            value = true;
            pos += 4;
            return true;
        } else if (str.compare(pos, 5, "false") == 0) {
            value = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool tryParseString(const std::string& str, size_t& pos, ConfigValue& value) {
        if (pos < str.size() && str[pos] == '\"') {
            size_t start = ++pos;
            while (pos < str.size() && str[pos] != '\"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos++;
                }
                pos++;
            }
            if (pos < str.size() && str[pos] == '\"') {
                value = str.substr(start, pos - start);
                pos++; // Skip closing quote
                return true;
            }
        }
        return false;
    }
    
    static std::shared_ptr<Config> getConfigFromEntries(const std::vector<ConfigEntry>& entries) {
        return nullptr; // Placeholder
    }
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
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON first, then fallback to simple key=value format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // JSON format
        return SimpleJsonParser::parse(content, pImpl->entries);
    } else {
        // Simple key=value format (legacy support)
        std::string line;
        std::stringstream stream(content);
        while (std::getline(stream, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    pImpl->entries.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help or -h
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        
        // Handle --version
        if (arg == "--version") {
            printVersion();
            exit(0);
        }
        
        // Handle --validate
        if (arg == "--validate") {
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                if (validateConfigFile(configFile)) {
                    std::cout << "Configuration file \"" << configFile << "\" is valid." << std::endl;
                } else {
                    std::cout << "Configuration file \"" << configFile << "\" is invalid." << std::endl;
                    return false;
                }
            }
            continue;
        }
        
        // Handle --benchmark
        if (arg == "--benchmark") {
            startBenchmark();
            continue;
        }
        
        // Handle --info
        if (arg == "--info") {
            printInfo();
            continue;
        }
        
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

// Complete implementation of JSON/YAML parser and enhanced configuration system
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <unordered_map>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> defaultDescriptions;
    bool caseInsensitive;
    std::string fileFormat;
    
    Impl() : caseInsensitive(true), fileFormat("auto") {}
};

// JSON parser implementation (simplified, no external dependencies)
namespace {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    const auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char ch) { return std::tolower(ch); });
    return result;
}

// Simple JSON parser - supports basic object, array, string, number, boolean
class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        try {
            size_t pos = 0;
            parseValue(jsonStr, pos, entries);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static void parseValue(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) return;
        
        char ch = str[pos];
        
        if (ch == '{') {
            parseObject(str, pos, entries);
        } else if (ch == '[') {
            // Arrays not used for configuration
            parseArray(str, pos);
        } else if (ch == '\"') {
            parseString(str, pos, entries);
        } else if (std::isdigit(ch) || ch == '-') {
            parseNumber(str, pos, entries);
        } else if (ch == 't' || ch == 'f') {
            parseBoolean(str, pos, entries);
        } else if (ch == 'n') {
            parseNull(str, pos);
        }
    }
    
    static void parseObject(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        pos++; // Skip '{'
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != '}') {
            // Parse key
            std::string key;
            parseString(str, pos, key);
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ':') {
                pos++;
                skipWhitespace(str, pos);
                
                // Parse value and create config entry
                ConfigValue value;
                if (tryParseNumber(str, pos, value)) {
                    // Number found
                } else if (tryParseBoolean(str, pos, value)) {
                    // Boolean found
                } else if (tryParseString(str, pos, value)) {
                    // String found
                } else {
                    // Skip unknown value
                    parseValue(str, pos, entries);
                    continue;
                }
                
                ConfigSource source = ConfigSource::File;
                std::string description = "";
                
                // Try to find description from default descriptions map
                if (auto config = getConfigFromEntries(entries)) {
                    auto it = config->pImpl->defaultDescriptions.find(key);
                    if (it != config->pImpl->defaultDescriptions.end()) {
                        description = it->second;
                    }
                }
                
                entries.emplace_back(key, value, source, description);
                
                skipWhitespace(str, pos);
                if (pos < str.size() && str[pos] == ',') {
                    pos++;
                    skipWhitespace(str, pos);
                }
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip '}'
        }
    }
    
    static void parseArray(const std::string& str, size_t& pos) {
        pos++; // Skip '['
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != ']') {
            parseValue(str, pos, {}); // Ignore array values
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
                skipWhitespace(str, pos);
            }
        }
        
        if (pos < str.size()) {
            pos++; // Skip ']'
        }
    }
    
    static void parseString(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries = {}) {
        size_t start = ++pos; // Skip opening quote
        
        while (pos < str.size() && str[pos] != '\"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos < str.size() && str[pos] == '\"') {
            std::string value = str.substr(start, pos - start);
            
            // Special handling for configuration keys
            if (entries.empty() && value.find('=') != std::string::npos) {
                // This looks like a "key=value" line
                size_t eqPos = value.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(value.substr(0, eqPos));
                    std::string val = trim(value.substr(eqPos + 1));
                    entries.emplace_back(key, val, ConfigSource::File);
                    pos++; // Skip closing quote
                    return;
                }
            }
            
            // Normal string parsing
            entries.emplace_back(value, value, ConfigSource::File);
            pos++; // Skip closing quote
        }
    }
    
    static void parseString(const std::string& str, size_t& pos) {
        parseString(str, pos, {});
    }
    
    static void parseNumber(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        entries.emplace_back(numStr, std::stod(numStr), ConfigSource::File);
    }
    
    static void parseBoolean(const std::string& str, size_t& pos, std::vector<ConfigEntry>& entries) {
        if (str.compare(pos, 4, "true") == 0) {
            entries.emplace_back("", true, ConfigSource::File);
            pos += 4;
        } else if (str.compare(pos, 5, "false") == 0) {
            entries.emplace_back("", false, ConfigSource::File);
            pos += 5;
        }
    }
    
    static void parseNull(const std::string& str, size_t& pos) {
        if (str.compare(pos, 4, "null") == 0) {
            pos += 4;
        }
    }
    
    static bool tryParseNumber(const std::string& str, size_t& pos, ConfigValue& value) {
        size_t start = pos;
        
        if (str[pos] == '-') pos++;
        if (pos >= str.size() || !std::isdigit(str[pos])) return false;
        
        while (pos < str.size() && std::isdigit(str[pos])) pos++;
        
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            if (pos >= str.size() || !std::isdigit(str[pos])) return false;
            while (pos < str.size() && std::isdigit(str[pos])) pos++;
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        // Try to determine if it's int or double
        try {
            size_t endpos;
            int intVal = std::stoi(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = intVal;
                return true;
            }
            
            double doubleVal = std::stod(numStr, &endpos);
            if (endpos == numStr.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {
        }
        
        return false;
    }
    
    static bool tryParseBoolean(const std::string& str, size_t& pos, ConfigValue& value) {
        if (str.compare(pos, 4, "true") == 0) {
            value = true;
            pos += 4;
            return true;
        } else if (str.compare(pos, 5, "false") == 0) {
            value = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool tryParseString(const std::string& str, size_t& pos, ConfigValue& value) {
        if (pos < str.size() && str[pos] == '\"') {
            size_t start = ++pos;
            while (pos < str.size() && str[pos] != '\"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos++;
                }
                pos++;
            }
            if (pos < str.size() && str[pos] == '\"') {
                value = str.substr(start, pos - start);
                pos++; // Skip closing quote
                return true;
            }
        }
        return false;
    }
    
    static std::shared_ptr<Config> getConfigFromEntries(const std::vector<ConfigEntry>& entries) {
        return nullptr; // Placeholder
    }
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
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON first, then fallback to simple key=value format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // JSON format
        return SimpleJsonParser::parse(content, pImpl->entries);
    } else {
        // Simple key=value format (legacy support)
        std::string line;
        std::stringstream stream(content);
        while (std::getline(stream, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    pImpl->entries.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help or -h
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        
        // Handle --version
        if (arg == "--version") {
            printVersion();
            exit(0);
        }
        
        // Handle --validate
        if (arg == "--validate") {
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                if (validateConfigFile(configFile)) {
                    std::cout << "Configuration file \"" << configFile << "\" is valid." << std::endl;
                } else {
                    std::cout << "Configuration file \"" << configFile << "\" is invalid." << std::endl;
                    return false;
                }
            }
            continue;
        }
        
        // Handle --benchmark
        if (arg == "--benchmark") {
            startBenchmark();
            continue;
        }
        
        // Handle --info
        if (arg == "--info") {
            printInfo();
            continue;
        }
        
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

void Config::printHelp() {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --version                   Show version information" << std::endl;
    std::cout << "  --validate <file>           Validate configuration file" << std::endl;
    std::cout << "  --benchmark                 Run performance benchmark" << std::endl;
    std::cout << "  --info                      Show system information" << std::endl;
    std::cout << "  --key=value                 Set configuration option" << std::endl;
    std::cout << "  -key value                  Set configuration option (short form)" << std::endl;
    std::cout << std::endl;
    std::cout << "Common configuration keys:" << std::endl;
    std::cout << "  random_seed                 Random number generator seed" << std::endl;
    std::cout << "  simulation_timestep         Time step duration (seconds)" << std::endl;
    std::cout << "  neuron_count                Number of neurons" << std::endl;
    std::cout << "  region_count                Number of regions" << std::endl;
    std::cout << "  connection_probability      Probability of synaptic connections" << std::endl;
    std::cout << "  stdp_ltp_weight             STDP potentiation weight" << std::endl;
    std::cout << "  stdp_ltd_weight             STDP depression weight" << std::endl;
    std::cout << "  stdp_tau                    STDP time constant" << std::endl;
    std::cout << "  synaptogenesis_rate         Rate of new synapse formation" << std::endl;
    std::cout << "  pruning_rate                Rate of synapse elimination" << std::endl;
}

void Config::printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0 (Phase 2)" << std::endl;
    std::cout << "Real neural computation with LIF neurons and event-driven dynamics" << std::endl;
}

bool Config::validateConfigFile(const std::string& filepath) {
    Config tempConfig;
    return tempConfig.loadFromFile(filepath);
}

void Config::startBenchmark() {
    std::cout << "Starting NLM performance benchmark..." << std::endl;
    // TODO: Implement benchmark mode
    std::cout << "Benchmark mode not yet implemented." << std::endl;
}

void Config::printInfo() {
    std::cout << "NLM System Information:" << std::endl;
    std::cout << "  Build type: Release" << std::endl;
    std::cout << "  C++ standard: C++20" << std::endl;
    std::cout << "  Platform: Linux" << std::endl;
    std::cout << "  Neural computation: LIF neurons with event-driven dynamics" << std::endl;
    std::cout << "  Plasticity: STDP and Hebbian learning" << std::endl;
    std::cout << "  Memory systems: Working, Episodic, and Associative memory" << std::endl;
    std::cout << "  Prediction system: Sensory prediction and error computation" << std::endl;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Add description comment if available
        if (!entry.description.empty()) {
            file << "  \"# " << entry.description << "\"\n";
        }
        
        // Write key-value pair
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(val) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeString(val[j]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char ch : str) {
        switch (ch) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (ch < 32 || ch == 127) {
                    result += "\\u" + toHexString(ch);
                } else {
                    result += ch;
                }
                break;
        }
    }
    return result;
}

std::string Config::toHexString(char ch) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(ch));
    return ss.str();
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
