#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <iomanip>
#include <memory>
#include <algorithm>

namespace nlm {

// JSON parser implementation (simplified)
class JsonParser {
public:
    static bool parse(const std::string& json, std::vector<ConfigEntry>& entries) {
        try {
            size_t pos = 0;
            return parseJsonObject(json, pos, entries);
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    static bool parseJsonObject(const std::string& json, size_t& pos, std::vector<ConfigEntry>& entries) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != '{') {
            return false;
        }
        
        ++pos; // Skip '{'
        skipWhitespace(json, pos);
        
        while (pos < json.size()) {
            skipWhitespace(json, pos);
            
            if (json[pos] == '}') {
                ++pos;
                return true;
            }
            
            // Parse key
            std::string key;
            if (!parseJsonString(json, pos, key)) {
                return false;
            }
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            ++pos; // Skip ':'
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            if (!parseJsonValue(json, pos, entry.value)) {
                return false;
            }
            
            entries.push_back(entry);
            
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                ++pos; // Skip ','
            } else if (pos < json.size() && json[pos] != '}') {
                return false;
            }
        }
        
        return false;
    }
    
    static bool parseJsonString(const std::string& json, size_t& pos, std::string& result) {
        if (pos >= json.size() || json[pos] != '\"') {
            return false;
        }
        
        ++pos; // Skip '\"'
        size_t start = pos;
        
        while (pos < json.size()) {
            if (json[pos] == '\"') {
                result = json.substr(start, pos - start);
                ++pos; // Skip closing '\"'
                return true;
            } else if (json[pos] == '\\') {
                // Skip escaped character
                ++pos;
                if (pos < json.size()) {
                    ++pos;
                }
            }
            ++pos;
        }
        
        return false;
    }
    
    static bool parseJsonValue(const std::string& json, size_t& pos, ConfigValue& result) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            return false;
        }
        
        switch (json[pos]) {
            case '\"': {
                std::string str;
                if (!parseJsonString(json, pos, str)) {
                    return false;
                }
                result = str;
                return true;
            }
            case 't':
            case 'f': {
                // Parse boolean
                if (json.substr(pos, 4) == "true") {
                    result = true;
                    pos += 4;
                    return true;
                } else if (json.substr(pos, 5) == "false") {
                    result = false;
                    pos += 5;
                    return true;
                }
                return false;
            }
            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': {
                // Parse number
                size_t start = pos;
                bool hasDecimal = false;
                bool hasExp = false;
                
                while (pos < json.size() && (std::isdigit(json[pos]) || 
                                             json[pos] == '.' || 
                                             json[pos] == '-' || 
                                             json[pos] == 'e' || 
                                             json[pos] == 'E')) {
                    if (json[pos] == '.') hasDecimal = true;
                    else if (json[pos] == 'e' || json[pos] == 'E') hasExp = true;
                    ++pos;
                }
                
                std::string numStr = json.substr(start, pos - start);
                if (hasDecimal || hasExp) {
                    try {
                        double val = std::stod(numStr);
                        result = val;
                        return true;
                    } catch (...) {
                        return false;
                    }
                } else {
                    try {
                        int64_t val = std::stoll(numStr);
                        result = val;
                        return true;
                    } catch (...) {
                        return false;
                    }
                }
            }
            default:
                return false;
        }
    }
    
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
    }
};

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
        // Try alternative path: configs/ subdirectory for backward compatibility
        std::string altPath = "configs/" + filepath;
        file.open(altPath);
        if (!file.is_open()) {
            // Log error but don't fail - allow missing config files
            NLM_LOG_WARNING("Config file not found: " + filepath);
            return false;
        }
    }
    
    // Try JSON format first for better structured configuration
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (!content.empty()) {
        // Check if it's JSON (starts with { or [)
        size_t firstNonSpace = content.find_first_not_of(" \t\n\r");
        if (firstNonSpace != std::string::npos && (content[firstNonSpace] == '{' || content[firstNonSpace] == '[')) {
            // Try to parse as JSON
            bool success = loadFromJson(content);
            if (success) {
                NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
                return true;
            } else {
                NLM_LOG_WARNING("Failed to parse JSON config from: " + filepath + ", trying key=value format");
            }
        } else {
            // Reset stream to beginning
            file.clear();
            file.seekg(0, std::ios::beg);
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    NLM_LOG_INFO("Loading configuration from key=value file: " + filepath);
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config file " + filepath + ", line " + std::to_string(lineNum) +
                           ": Invalid format (missing '='), skipping: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key is not empty
        if (key.empty()) {
            NLM_LOG_WARNING("Config file " + filepath + ", line " + std::to_string(lineNum) +
                           ": Empty key, skipping line");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to determine type and set value
        bool success = trySetFromString(key, value);
        if (!success) {
            NLM_LOG_WARNING("Config file " + filepath + ", line " + std::to_string(lineNum) +
                           ": Failed to parse value \"" + value + "\" for key \"" + key + "\"");
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

bool Config::loadFromJson(const std::string& jsonContent) {
    std::vector<ConfigEntry> newEntries;
    if (!JsonParser::parse(jsonContent, newEntries)) {
        return false;
    }
    
    // Merge with existing entries (JSON takes precedence)
    for (const auto& entry : newEntries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
    
    return true;
}

bool Config::trySetFromString(const std::string& key, const std::string& valueStr) {
    // Try to detect type from string
    if (valueStr.empty()) {
        set(key, std::string(), ConfigSource::File);
        return true;
    }
    
    // Check for boolean
    if (valueStr == "true") {
        set(key, true, ConfigSource::File);
        return true;
    } else if (valueStr == "false") {
        set(key, false, ConfigSource::File);
        return true;
    }
    
    // Check for integer
    bool isInteger = true;
    size_t start = 0;
    if (valueStr[0] == '-' || valueStr[0] == '+') {
        start = 1;
    }
    for (size_t i = start; i < valueStr.size(); ++i) {
        if (!std::isdigit(valueStr[i])) {
            isInteger = false;
            break;
        }
    }
    
    if (isInteger) {
        try {
            int64_t intVal = std::stoll(valueStr);
            set(key, intVal, ConfigSource::File);
            return true;
        } catch (...) {
            // Fall through to double check
        }
    }
    
    // Check for floating point
    bool isDouble = true;
    size_t decimalCount = 0;
    bool hasExponent = false;
    
    start = 0;
    if (valueStr[0] == '-' || valueStr[0] == '+') {
        start = 1;
    }
    bool foundDigit = false;
    for (size_t i = start; i < valueStr.size(); ++i) {
        if (valueStr[i] == '.') {
            if (decimalCount > 0 || hasExponent) {
                isDouble = false;
                break;
            }
            decimalCount++;
        } else if (valueStr[i] == 'e' || valueStr[i] == 'E') {
            if (hasExponent) {
                isDouble = false;
                break;
            }
            hasExponent = true;
            // Check for exponent sign
            if (i + 1 < valueStr.size() && (valueStr[i + 1] == '-' || valueStr[i + 1] == '+')) {
                ++i; // Skip sign
            }
        } else if (!std::isdigit(valueStr[i])) {
            isDouble = false;
            break;
        } else {
            foundDigit = true;
        }
    }
    
    if (isDouble && foundDigit) {
        try {
            double doubleVal = std::stod(valueStr);
            set(key, doubleVal, ConfigSource::File);
            return true;
        } catch (...) {
            // Fall through to string
        }
    }
    
    // Default to string
    set(key, valueStr, ConfigSource::File);
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated at " << std::time(nullptr) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double to avoid scientific notation
                std::stringstream ss;
                ss << std::fixed << std::setprecision(6) << arg;
                file << ss.str();
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::printConfig(const std::string& path) {
    auto tempConfig = std::make_shared<Config>();
    if (path.empty()) {
        // Create a config with default values for demonstration
        tempConfig->set("neuron_count", static_cast<int64_t>(1000));
        tempConfig->set("region_count", static_cast<int64_t>(1));
        tempConfig->set("connection_probability", 0.15f);
        tempConfig->set("stdp_ltp_weight", 0.02f);
        tempConfig->set("stdp_ltd_weight", 0.015f);
        tempConfig->set("synaptogenesis_rate", 0.0001f);
        tempConfig->set("pruning_rate", 0.00001f);
        
        // Create sample debug config
        std::cout << "=== SAMPLE DEBUG CONFIGURATION ===" << std::endl;
        std::cout << tempConfig->summary() << std::endl;
    } else {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(path)) {
            std::cout << "=== LOADED CONFIGURATION ===" << std::endl;
            std::cout << config->summary() << std::endl;
        }
    }
}

{
    pImpl->entries = other.pImpl->entries;
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!trySetFromString(key, value)) {
                    NLM_LOG_WARNING("Failed to parse command line argument: " + arg);
                }
            } else {
                NLM_LOG_WARNING("Missing value in command line argument: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!trySetFromString(key, value)) {
                NLM_LOG_WARNING("Failed to parse command line argument: -" + key + " " + value);
            }
        }
    }
    
    return true;
}

void Config::printConfig(const std::string& path) {
    auto tempConfig = std::make_shared<Config>();
    if (path.empty()) {
        // Create a config with default values for demonstration
        tempConfig->set("neuron_count", static_cast<int64_t>(1000));
        tempConfig->set("region_count", static_cast<int64_t>(1));
        tempConfig->set("connection_probability", 0.15f);
        tempConfig->set("stdp_ltp_weight", 0.02f);
        tempConfig->set("stdp_ltd_weight", 0.015f);
        tempConfig->set("synaptogenesis_rate", 0.0001f);
        tempConfig->set("pruning_rate", 0.00001f);
        
        std::cout << "=== SAMPLE DEBUG CONFIGURATION ===" << std::endl;
        std::cout << tempConfig->summary() << std::endl;
    } else {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(path)) {
            std::cout << "=== LOADED CONFIGURATION ===" << std::endl;
            std::cout << config->summary() << std::endl;
        }
    }
}
