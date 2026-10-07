#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

// Forward declaration for JSON parser
class SimpleJSONParser;

// Configuration validation interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getValidationError(const std::string& key, const ConfigValue& value) const = 0;
};

// Default configuration schema
class DefaultConfigValidator : public ConfigValidator {
public:
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (key == "random_seed") {
            return std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) >= 0;
        } else if (key == "simulation_timestep") {
            if (std::holds_alternative<double>(value)) {
                double timestep = std::get<double>(value);
                return timestep > 0.0 && timestep < 1.0;  // Reasonable range for timesteps
            }
            return false;
        } else if (key == "neuron_count") {
            return std::holds_alternative<size_t>(value) && std::get<size_t>(value) > 0;
        } else if (key == "region_count") {
            return std::holds_alternative<size_t>(value) && std::get<size_t>(value) > 0;
        } else if (key == "connection_probability") {
            if (std::holds_alternative<float>(value)) {
                float prob = std::get<float>(value);
                return prob >= 0.0f && prob <= 1.0f;
            }
            return false;
        } else if (key == "stdp_ltp_weight") {
            if (std::holds_alternative<float>(value)) {
                float weight = std::get<float>(value);
                return weight >= 0.0f && weight <= 10.0f;
            }
            return false;
        } else if (key == "stdp_ltd_weight") {
            if (std::holds_alternative<float>(value)) {
                float weight = std::get<float>(value);
                return weight >= 0.0f && weight <= 10.0f;
            }
            return false;
        } else if (key == "stdp_tau") {
            if (std::holds_alternative<float>(value)) {
                float tau = std::get<float>(value);
                return tau > 0.0f && tau < 1000.0f;
            }
            return false;
        }
        return true; // No validation for unknown keys
    }
    
    std::string getValidationError(const std::string& key, const ConfigValue& value) const override {
        if (key == "random_seed") {
            return "random_seed must be a non-negative integer";
        } else if (key == "simulation_timestep") {
            return "simulation_timestep must be a positive double less than 1.0";
        } else if (key == "neuron_count") {
            return "neuron_count must be a positive size_t";
        } else if (key == "region_count") {
            return "region_count must be a positive size_t";
        } else if (key == "connection_probability") {
            return "connection_probability must be a float between 0.0 and 1.0";
        } else if (key == "stdp_ltp_weight") {
            return "stdp_ltp_weight must be a float between 0.0 and 10.0";
        } else if (key == "stdp_ltd_weight") {
            return "stdp_ltd_weight must be a float between 0.0 and 10.0";
        } else if (key == "stdp_tau") {
            return "stdp_tau must be a positive float less than 1000.0";
        }
        return "Invalid value for key: " + key;
    }
};

// Simple JSON parser implementation
class SimpleJSONParser {
public:
    static bool parseFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return parseJSON(content, entries);
    }
    
    static bool parseJSON(const std::string& json, std::vector<ConfigEntry>& entries) {
        // Remove whitespace and comments
        std::string cleanJSON = removeWhitespaceAndComments(json);
        
        // Simple JSON parsing for object notation
        if (cleanJSON.empty() || cleanJSON[0] != '{') {
            return false; // Not a JSON object
        }
        
        size_t pos = 1; // Skip opening brace
        while (pos < cleanJSON.size() && cleanJSON[pos] != '}') {
            // Parse key
            size_t keyStart = cleanJSON.find('"', pos);
            if (keyStart == std::string::npos) {
                return false;
            }
            size_t keyEnd = cleanJSON.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) {
                return false;
            }
            
            std::string key = cleanJSON.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < cleanJSON.size() && std::isspace(cleanJSON[pos])) {
                pos++;
            }
            
            // Expect colon
            if (pos >= cleanJSON.size() || cleanJSON[pos] != ':') {
                return false;
            }
            pos++;
            
            // Skip whitespace
            while (pos < cleanJSON.size() && std::isspace(cleanJSON[pos])) {
                pos++;
            }
            
            // Parse value
            ConfigValue value;
            bool parseSuccess = parseJSONValue(cleanJSON, pos, value);
            if (!parseSuccess) {
                return false;
            }
            
            // Add entry
            entries.emplace_back(key, value, ConfigSource::File, "Parsed from JSON file");
            
            // Skip whitespace
            while (pos < cleanJSON.size() && std::isspace(cleanJSON[pos])) {
                pos++;
            }
            
            // Expect comma or closing brace
            if (pos < cleanJSON.size() && cleanJSON[pos] == ',') {
                pos++;
                // Skip whitespace after comma
                while (pos < cleanJSON.size() && std::isspace(cleanJSON[pos])) {
                    pos++;
                }
            } else if (pos < cleanJSON.size() && cleanJSON[pos] == '}') {
                break;
            } else {
                return false;
            }
        }
        
        return true;
    }
    
private:
    static std::string removeWhitespaceAndComments(const std::string& json) {
        std::string result;
        bool inString = false;
        char quoteChar = '\0';
        
        for (size_t i = 0; i < json.size(); ++i) {
            char c = json[i];
            
            if (!inString) {
                if (c == '/' && i + 1 < json.size() && json[i + 1] == '/') {
                    // Line comment - skip until end of line
                    while (i < json.size() && c != '\n') {
                        i++;
                        if (i < json.size()) c = json[i];
                    }
                    if (i < json.size()) result.push_back('\n');
                    continue;
                } else if (c == '/' && i + 1 < json.size() && json[i + 1] == '*') {
                    // Block comment - skip until end
                    i += 2;
                    while (i + 1 < json.size() && !(json[i] == '*' && json[i + 1] == '/')) {
                        i++;
                    }
                    if (i < json.size()) i++; // Skip closing */
                    continue;
                } else if (std::isspace(c)) {
                    continue; // Skip whitespace
                }
            }
            
            if (c == '"' || c == '\'') {
                if (!inString) {
                    inString = true;
                    quoteChar = c;
                } else if (quoteChar == c) {
                    inString = false;
                    quoteChar = '\0';
                }
            }
            
            result.push_back(c);
        }
        
        return result;
    }
    
    static bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            return false;
        }
        
        char c = json[pos];
        
        if (c == '"' || c == '\'') {
            // String value
            size_t end = findStringEnd(json, pos);
            if (end == std::string::npos) {
                return false;
            }
            std::string str = json.substr(pos + 1, end - pos - 1);
            value = str;
            pos = end + 1;
            return true;
        } else if (std::isdigit(c) || c == '-') {
            // Number (int or float)
            size_t end = pos;
            while (end < json.size() && (std::isdigit(json[end]) || json[end] == '.' || json[end] == 'e' || json[end] == 'E' || json[end] == '+' || json[end] == '-')) {
                end++;
            }
            std::string numStr = json.substr(pos, end - pos);
            
            // Try to parse as int64_t first, then double
            try {
                size_t parsedPos;
                int64_t intVal = std::stoll(numStr, &parsedPos);
                if (parsedPos == numStr.size()) {
                    value = intVal;
                    pos = end;
                    return true;
                }
            } catch (...) {}
            
            try {
                size_t parsedPos;
                double doubleVal = std::stod(numStr, &parsedPos);
                if (parsedPos == numStr.size()) {
                    value = doubleVal;
                    pos = end;
                    return true;
                }
            } catch (...) {}
            
            return false;
        } else if (c == 't' || c == 'f') {
            // Boolean value
            if (json.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
                return true;
            } else if (json.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
                return true;
            }
            return false;
        } else if (c == 'n') {
            // Null value (skip)
            if (json.substr(pos, 4) == "null") {
                pos += 4;
                return true;
            }
            return false;
        } else if (c == '[') {
            // Array - not currently supported
            return false;
        }
        
        return false;
    }
    
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    static size_t findStringEnd(const std::string& json, size_t start) {
        char quoteChar = json[start];
        size_t pos = start + 1;
        
        while (pos < json.size()) {
            if (json[pos] == quoteChar) {
                // Check if escaped
                if (pos > 0 && json[pos - 1] == '\\') {
                    // Count consecutive backslashes
                    size_t backslashCount = 1;
                    size_t checkPos = pos - 1;
                    while (checkPos > 0 && json[checkPos] == '\\') {
                        backslashCount++;
                        checkPos--;
                    }
                    // If odd number of backslashes, this is an escaped quote
                    if (backslashCount % 2 == 1) {
                        pos++;
                        continue;
                    }
                }
                return pos;
            }
            pos++;
        }
        
        return std::string::npos;
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigValidator> validator;
    std::string configFilePath;
    std::time_t lastModified;
    
    Impl() : validator(std::make_unique<DefaultConfigValidator>()), 
             lastModified(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<size_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<size_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    set("replay_interval", static_cast<size_t>(100), ConfigSource::Default);
    set("consolidation_interval", static_cast<size_t>(1000), ConfigSource::Default);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->configFilePath = filepath;
    
    // Try to load as JSON first
    std::vector<ConfigEntry> newEntries;
    bool jsonSuccess = SimpleJSONParser::parseFile(filepath, newEntries);
    
    if (jsonSuccess) {
        // Merge with existing entries
        for (const auto& entry : newEntries) {
            // Check if key already exists
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it != pImpl->entries.end()) {
                // Update existing entry
                it->value = entry.value;
                it->source = ConfigSource::File;
            } else {
                // Add new entry
                pImpl->entries.push_back(entry);
            }
        }
        
        // Update file modification time
        if (std::filesystem::exists(filepath)) {
            pImpl->lastModified = std::filesystem::last_write_time(filepath).time_since_epoch().count();
        }
        
        return true;
    }
    
    // Fallback to simple key=value format
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
    
    // Update file modification time
    if (std::filesystem::exists(filepath)) {
        pImpl->lastModified = std::filesystem::last_write_time(filepath).time_since_epoch().count();
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value)) {
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value)) {
                success = false;
            }
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write in JSON format
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << " // " << static_cast<int>(entry.source) << \\n";
        first = false;
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
    // Validate the value if validator exists
    if (pImpl->validator && !pImpl->validator->validate(key, value)) {
        throw std::runtime_error("Invalid value for key " + key + ": " + 
                                pImpl->validator->getValidationError(key, value));
    }
    
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

std::vector<std::pair<std::string, ConfigValue>> Config::getAll() const {
    std::vector<std::pair<std::string, ConfigValue>> allEntries;
    allEntries.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        allEntries.emplace_back(entry.key, entry.value);
    }
    return allEntries;
}

bool Config::isFileLoaded() const {
    return !pImpl->configFilePath.empty();
}

std::string Config::getFilePath() const {
    return pImpl->configFilePath;
}

std::time_t Config::getFileModificationTime() const {
    return pImpl->lastModified;
}

void Config::setValidator(std::unique_ptr<ConfigValidator> validator) {
    pImpl->validator = std::move(validator);
}

bool Config::isFileOutOfDate() const {
    if (pImpl->configFilePath.empty()) {
        return false;
    }
    
    if (!std::filesystem::exists(pImpl->configFilePath)) {
        return false; // File doesn't exist
    }
    
    auto currentModTime = std::filesystem::last_write_time(pImpl->configFilePath).time_since_epoch().count();
    return currentModTime > pImpl->lastModified;
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    
    if (pImpl->configFilePath.empty()) {
        oss << "No configuration file loaded\n";
    } else {
        oss << "Configuration file: " << pImpl->configFilePath << "\n";
        if (pImpl->lastModified > 0) {
            auto time = std::localtime(&pImpl->lastModified);
            oss << "Last modified: " << std::put_time(time, "%Y-%m-%d %H:%M:%S") << "\n";
        }
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

bool Config::setFromString(const std::string& key, const std::string& valueStr) {
    try {
        // Try to parse based on key name or content
        if (key.find("count") != std::string::npos || key.find("seed") != std::string::npos) {
            // Likely integer
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, ConfigSource::CommandLine);
                return true;
            }
        } else if (key.find("time") != std::string::npos || key.find("rate") != std::string::npos || 
                   key.find("weight") != std::string::npos || key.find("tau") != std::string::npos) {
            // Likely float
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, ConfigSource::CommandLine);
                return true;
            }
        } else if (key.find("bool") != std::string::npos || valueStr == "true" || valueStr == "false") {
            // Boolean
            bool boolVal = (valueStr == "true");
            set(key, boolVal, ConfigSource::CommandLine);
            return true;
        } else {
            // Try string first
            set(key, valueStr, ConfigSource::CommandLine);
            return true;
        }
    } catch (const std::exception&) {
        // Fallback to string
        set(key, valueStr, ConfigSource::CommandLine);
        return true;
    }
    
    return false;
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
