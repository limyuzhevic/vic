#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <map>
#include <set>

namespace nlm {

// JSON parsing implementation
namespace {

class JsonParser {
public:
    static std::map<std::string, nlm::ConfigValue> parseJson(const std::string& jsonStr) {
        std::map<std::string, nlm::ConfigValue> result;
        
        // Simple JSON parser for basic structures
        // This handles objects with string keys and simple value types
        size_t pos = 0;
        parseJsonObject(jsonStr, pos, result);
        
        return result;
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static void parseJsonObject(const std::string& str, size_t& pos, std::map<std::string, nlm::ConfigValue>& obj) {
        skipWhitespace(str, pos);
        if (pos >= str.size() || str[pos] != '{') {
            throw std::runtime_error("Expected '{' for JSON object");
        }
        pos++;
        
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != '}') {
            // Parse key
            skipWhitespace(str, pos);
            if (str[pos] == '"') {
                pos++;
                size_t keyStart = pos;
                while (pos < str.size() && str[pos] != '"') {
                    pos++;
                }
                std::string key = str.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                skipWhitespace(str, pos);
                if (pos >= str.size() || str[pos] != ':') {
                    throw std::runtime_error("Expected ':' after JSON key");
                }
                pos++;
                
                skipWhitespace(str, pos);
                
                // Parse value
                nlm::ConfigValue value = parseJsonValue(str, pos);
                obj[key] = value;
                
                skipWhitespace(str, pos);
                if (pos < str.size() && str[pos] == ',') {
                    pos++;
                }
            } else {
                throw std::runtime_error("Expected string key in JSON object");
            }
            
            skipWhitespace(str, pos);
        }
        
        if (pos < str.size() && str[pos] == '}') {
            pos++;
        }
    }
    
    static nlm::ConfigValue parseJsonValue(const std::string& str, size_t& pos) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        
        switch (str[pos]) {
            case '{': {
                // Object
                std::map<std::string, nlm::ConfigValue> obj;
                parseJsonObject(str, pos, obj);
                
                // For now, we don't support nested objects in config
                // Just use the first value we find or throw error
                if (obj.empty()) {
                    throw std::runtime_error("Empty object not supported in config");
                }
                auto it = obj.begin();
                return it->second;
            }
            case '"': {
                // String
                pos++;
                size_t start = pos;
                while (pos < str.size() && str[pos] != '"') {
                    pos++;
                }
                std::string result = str.substr(start, pos - start);
                pos++; // Skip closing quote
                return result;
            }
            case '[': {
                // Array
                std::vector<nlm::ConfigValue> array;
                parseJsonArray(str, pos, array);
                
                // For now, determine array type from first element
                if (array.empty()) {
                    // Empty array - return as vector<string> by default
                    return std::vector<std::string>();
                }
                
                // Check if all elements are the same type
                bool allInts = std::all_of(array.begin(), array.end(), 
                    [](const nlm::ConfigValue& v) { return std::holds_alternative<int>(v); });
                bool allDoubles = std::all_of(array.begin(), array.end(), 
                    [](const nlm::ConfigValue& v) { return std::holds_alternative<double>(v); });
                bool allStrings = std::all_of(array.begin(), array.end(), 
                    [](const nlm::ConfigValue& v) { return std::holds_alternative<std::string>(v); });
                
                if (allInts) {
                    std::vector<int> intArray;
                    intArray.reserve(array.size());
                    for (const auto& v : array) {
                        intArray.push_back(std::get<int>(v));
                    }
                    return intArray;
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(array.size());
                    for (const auto& v : array) {
                        doubleArray.push_back(std::get<double>(v));
                    }
                    return doubleArray;
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(array.size());
                    for (const auto& v : array) {
                        stringArray.push_back(std::get<std::string>(v));
                    }
                    return stringArray;
                } else {
                    // Mixed types - return as vector<string> with string conversion
                    std::vector<std::string> mixedArray;
                    mixedArray.reserve(array.size());
                    for (const auto& v : array) {
                        mixedArray.push_back(toString(v));
                    }
                    return mixedArray;
                }
            }
            default:
                if (std::isdigit(str[pos]) || str[pos] == '-') {
                    // Number
                    size_t start = pos;
                    while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == 'e' || str[pos] == 'E' || str[pos] == '+')) {
                        pos++;
                    }
                    std::string numStr = str.substr(start, pos - start);
                    try {
                        if (numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                            return std::stod(numStr);
                        } else {
                            return std::stoi(numStr);
                        }
                    } catch (...) {
                        throw std::runtime_error("Invalid number in JSON: " + numStr);
                    }
                } else if (str[pos] == 't' || str[pos] == 'f') {
                    // Boolean
                    size_t start = pos;
                    while (pos < str.size() && (str[pos] == 't' || str[pos] == 'r' || str[pos] == 'u' || str[pos] == 'e')) {
                        pos++;
                    }
                    std::string boolStr = str.substr(start, pos - start);
                    if (boolStr == "true") {
                        return true;
                    } else if (boolStr == "false") {
                        return false;
                    } else {
                        throw std::runtime_error("Invalid boolean in JSON: " + boolStr);
                    }
                } else {
                    throw std::runtime_error("Unexpected character in JSON value: " + std::string(1, str[pos]));
                }
        }
    }
    
    static void parseJsonArray(const std::string& str, size_t& pos, std::vector<nlm::ConfigValue>& array) {
        pos++; // Skip '['
        skipWhitespace(str, pos);
        
        while (pos < str.size() && str[pos] != ']') {
            nlm::ConfigValue value = parseJsonValue(str, pos);
            array.push_back(value);
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
            }
            
            skipWhitespace(str, pos);
        }
        
        if (pos < str.size() && str[pos] == ']') {
            pos++;
        }
    }
    
    static std::string toString(const nlm::ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        return "";
    }
};

} // namespace

// Helper methods for enhanced config loading
void Config::applyConfigData(const std::map<std::string, nlm::ConfigValue>& data) {
    for (const auto& pair : data) {
        set(pair.first, convertConfigValue(pair.second), ConfigSource::File);
    }
}

void Config::parseSimpleFormat(const std::string& content, const std::string& filepath) {
    std::stringstream stream(content);
    std::string line;
    
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Config: Invalid line in simple format: " + line);
        }
    }
}

nlm::ConfigValue Config::convertConfigValue(const nlm::ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return value;
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<int>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return value;
    } else if (std::holds_alternative<bool>(value)) {
        return value;
    } else if (std::holds_alternative<std::string>(value)) {
        return value;
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return value;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return value;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return value;
    }
    return "";
}

bool Config::validateConfig() const {
    // Basic validation - check for required essential keys
    const std::set<std::string> essentialKeys = {"neuron_count", "region_count", "simulation_timestep"};
    
    bool isValid = true;
    
    for (const auto& key : essentialKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Config: Missing essential key: " + key);
            isValid = false;
        }
    }
    
    // Validate value ranges
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    if (neuronCount < 1) {
        NLM_LOG_ERROR("Config: neuron_count must be >= 1, got " + std::to_string(neuronCount));
        isValid = false;
    }
    
    auto timestep = getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Config: simulation_timestep must be > 0, got " + std::to_string(timestep));
        isValid = false;
    }
    
    auto connProb = getOr<float>("connection_probability", 0.1f);
    if (connProb < 0.0f || connProb > 1.0f) {
        NLM_LOG_ERROR("Config: connection_probability must be in [0.0, 1.0], got " + std::to_string(connProb));
        isValid = false;
    }
    
    return isValid;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config: Empty file: " + filepath);
        return false;
    }
    
    try {
        // Try JSON parsing first
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            auto jsonData = JsonParser::parseJson(content);
            applyConfigData(jsonData);
            NLM_LOG_INFO("Config: Loaded JSON from file: " + filepath);
        } else {
            // Fall back to simple key=value format
            parseSimpleFormat(content, filepath);
            NLM_LOG_INFO("Config: Loaded simple format from file: " + filepath);
        }
        
        // Validate loaded configuration
        if (!validateConfig()) {
            NLM_LOG_ERROR("Config: Validation failed for file: " + filepath);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config: Failed to parse file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced command-line argument parsing
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Config: Invalid command line argument --" + key + "=" + value + ": " + std::string(e.what()));
                    hasErrors = true;
                }
            } else {
                NLM_LOG_ERROR("Config: Invalid command line argument format: " + arg);
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config: Invalid command line argument -" + key + " " + value + ": " + std::string(e.what()));
                hasErrors = true;
            }
        }
        // Handle --help flag
        else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit after showing help
        }
        // Handle --version flag
        else if (arg == "--version") {
            printVersion();
            return false; // Exit after showing version
        }
        // Handle --validate flag (validate current config)
        else if (arg == "--validate") {
            if (!validateConfig()) {
                NLM_LOG_ERROR("Config: Configuration validation failed");
                hasErrors = true;
            } else {
                NLM_LOG_INFO("Config: Configuration validation passed");
            }
        }
        // Handle unknown flags
        else if (arg.substr(0, 2) == "--") {
            NLM_LOG_ERROR("Config: Unknown option: " + arg);
            hasErrors = true;
        }
    }
    
    // Post-validation of command line arguments
    if (hasErrors) {
        return false;
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

}

// Helper methods for command-line interface
void Config::printHelp() const {
    NLM_LOG_INFO("Config Help:");
    NLM_LOG_INFO("  Configuration file formats supported:");
    NLM_LOG_INFO("    - Simple key=value format");
    NLM_LOG_INFO("    - JSON format (recommended)");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("  Command line options:")
    NLM_LOG_INFO("    --key=value     Set configuration parameter")
    NLM_LOG_INFO("    -k value        Set configuration parameter (short form)")
    NLM_LOG_INFO("    --help, -h      Show this help message")
    NLM_LOG_INFO("    --version       Show version information")
    NLM_LOG_INFO("    --validate      Validate current configuration")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("  Common configuration parameters:")
    NLM_LOG_INFO("    neuron_count       Number of neurons in the brain")
    NLM_LOG_INFO("    region_count       Number of brain regions")
    NLM_LOG_INFO("    simulation_timestep Time step for simulation")
    NLM_LOG_INFO("    connection_probability Connection probability between neurons")
    NLM_LOG_INFO("    stdp_ltp_weight    STDP long-term potentiation weight")
    NLM_LOG_INFO("    stdp_ltd_weight    STDP long-term depression weight")
    NLM_LOG_INFO("    synaptogenesis_rate Rate of new synapse formation")
    NLM_LOG_INFO("    pruning_rate       Rate of synapse elimination")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("  Configuration files can be specified with --config=filename")
}

void Config::printVersion() const {
    NLM_LOG_INFO("NLM Configuration System v0.1.0");
    NLM_LOG_INFO("Supports JSON and simple key=value configuration formats");
}

// Enhanced saveToFile method with JSON output support
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Try to save in JSON format first
    if (saveAsJson(file)) {
        NLM_LOG_INFO("Config: Saved configuration as JSON to file: " + filepath);
        return true;
    }
    
    // Fall back to simple format
    return saveAsSimple(file);
}

bool Config::saveAsJson(std::ostream& output) const {
    output << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            output << ",\n";
        }
        first = false;
        
        output << "  \"" << entry.key << "\": ";
        output << configValueToJson(entry.value);
    }
    
    output << "\n}\n";
    return true;
}

bool Config::saveAsSimple(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.description << "\n";
        output << entry.key << " = " << configValueToSimpleString(entry.value) << "\n";
    }
    return true;
}

std::string Config::configValueToJson(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        output << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) output << ", ";
            output << vec[i];
        }
        output << "]";
        return output.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
        return ss.str();
    }
    return "\"\"";
}

std::string Config::configValueToSimpleString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
        return ss.str();
    }
    return "\"\"";
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
