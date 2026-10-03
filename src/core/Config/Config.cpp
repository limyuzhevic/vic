#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first, then fall back to key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Check if it looks like JSON or YAML
    bool isJson = false;
    bool isYaml = false;
    
    // Simple detection based on content
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        isJson = true;
    } else if (content.find(':') != std::string::npos && 
               (content.find('- ') != std::string::npos || content.find('* ') != std::string::npos)) {
        isYaml = true;
    }
    
    if (isJson) {
        return loadFromJson(content);
    } else if (isYaml) {
        return loadFromYaml(content);
    } else {
        return loadFromKeyValue(content);
    }
}

bool Config::loadFromKeyValue(const std::string& content) {
    std::istringstream stream(content);
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
        }
    }
    return true;
}

bool Config::loadFromJson(const std::string& jsonContent) {
    // Simple JSON parser for key-value pairs
    // This handles basic objects like {"key": "value", "number": 42}
    try {
        size_t start = 0;
        while (start < jsonContent.size()) {
            // Find next key
            start = jsonContent.find('"', start);
            if (start == std::string::npos) break;
            
            size_t keyEnd = jsonContent.find('"', start + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(start + 1, keyEnd - start - 1);
            
            // Find value
            size_t valueStart = jsonContent.find(':', keyEnd);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = findMatchingBracket(jsonContent, valueStart + 1, '{', '}');
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = jsonContent.substr(valueStart + 1, valueEnd - valueStart - 1);
            valueStr = trim(valueStr);
            
            // Parse value based on content
            ConfigValue value;
            if (valueStr.front() == '"' && valueStr.back() == '"') {
                // String value
                value = trim(valueStr.substr(1, valueStr.size() - 2));
            } else if (std::isdigit(valueStr.front()) || (valueStr.front() == '-' && std::isdigit(valueStr[1]))) {
                // Number value (float or int)
                try {
                    size_t decimalPos = valueStr.find('.');
                    if (decimalPos != std::string::npos) {
                        value = std::stod(valueStr);
                    } else {
                        value = std::stoll(valueStr);
                    }
                } catch (...) {
                    value = valueStr;
                }
            } else if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
            } else {
                value = valueStr;
            }
            
            set(key, value, ConfigSource::File);
            start = valueEnd + 1;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlContent) {
    // Simple YAML parser for key-value pairs
    // This handles basic YAML like key: value
    try {
        std::istringstream stream(yamlContent);
        std::string line;
        while (std::getline(stream, line)) {
            // Trim whitespace
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '%') {
                continue;
            }
            
            // Parse key: value format
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Parse boolean values
                if (value == "true") {
                    set(key, true, ConfigSource::File);
                } else if (value == "false") {
                    set(key, false, ConfigSource::File);
                } else if (std::isdigit(value.front()) || (value.front() == '-' && std::isdigit(value[1]))) {
                    // Number value
                    try {
                        size_t decimalPos = value.find('.');
                        if (decimalPos != std::string::npos) {
                            set(key, std::stod(value), ConfigSource::File);
                        } else {
                            set(key, std::stoll(value), ConfigSource::File);
                        }
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                } else {
                    set(key, value, ConfigSource::File);
                }
            }
        }
        return true;
    } catch (...) {
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

size_t Config::findMatchingBracket(const std::string& content, size_t start, char open, char close) const {
    if (start >= content.size() || content[start] != open) {
        return std::string::npos;
    }
    
    int depth = 1;
    size_t pos = start + 1;
    
    while (pos < content.size() && depth > 0) {
        if (content[pos] == open) {
            depth++;
        } else if (content[pos] == close) {
            depth--;
        }
        pos++;
    }
    
    return (depth == 0) ? pos - 1 : std::string::npos;
}

std::string Config::toString(const ConfigValue& value) const {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    return "";
}

// Add validation method
bool Config::validate() const {
    // Basic validation rules
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key.empty(); });
    if (it != pImpl->entries.end()) {
        return false;
    }
    
    // Validate type consistency if needed
    return true;
}

// Add export functionality
std::string Config::exportToJson() const {
    std::ostringstream json;
    json << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        json << "  \"" << entry.key << "\": ";
        
        if (std::holds_alternative<std::string>(entry.value)) {
            json << "\"" << toString(entry.value) << "\"";
        } else if (std::holds_alternative<int>(entry.value)) {
            json << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            json << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            json << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            json << (std::get<bool>(entry.value) ? "true" : "false");
        }
        
        if (i < pImpl->entries.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    
    json << "}\n";
    return json.str();
}

std::string Config::exportToYaml() const {
    std::ostringstream yaml;
    yaml << "# Exported NLM Configuration\n\n";
    
    for (const auto& entry : pImpl->entries) {
        yaml << entry.key << ": ";
        
        if (std::holds_alternative<std::string>(entry.value)) {
            yaml << "\"" << toString(entry.value) << "\"";
        } else if (std::holds_alternative<int>(entry.value)) {
            yaml << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            yaml << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            yaml << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            yaml << (std::get<bool>(entry.value) ? "true" : "false");
        }
        
        yaml << "\n\n";
    }
    
    return yaml.str();
}

// Add advanced configuration capabilities

// Add method to validate configuration with more comprehensive rules
bool Config::validateAdvanced() const {
    // Comprehensive validation rules for advanced users
    bool isValid = true;
    
    // Check for required keys
    std::set<std::string> requiredKeys = {"random_seed", "simulation_timestep"};
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            isValid = false;
        }
    }
    
    // Validate numeric ranges
    if (has("neuron_count")) {
        auto val = get<int64_t>("neuron_count");
        if (val.has_value() && val.value() < 10) {
            NLM_LOG_WARNING("neuron_count seems too small: " + std::to_string(val.value()));
        }
    }
    
    if (has("connection_probability")) {
        auto val = get<float>("connection_probability");
        if (val.has_value() && (val.value() < 0.0f || val.value() > 1.0f)) {
            NLM_LOG_WARNING("connection_probability should be in [0, 1]: " + std::to_string(val.value()));
        }
    }
    
    // Check for deprecated keys
    std::set<std::string> deprecatedKeys = {"plasticity_learning_rate"};
    for (const auto& key : deprecatedKeys) {
        if (has(key)) {
            NLM_LOG_WARNING("Deprecated configuration key: " + key);
        }
    }
    
    return isValid;
}

// Add method for configuration profiling
std::string Config::profile() const {
    std::ostringstream profile;
    profile << "=== NLM Configuration Profile ===\n";
    profile << "Total entries: " << pImpl->entries.size() << "\n";
    profile << "Sources:" << std::endl;
    
    int fileCount = 0, commandLineCount = 0, defaultCount = 0, otherCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::Plugin: otherCount++; break;
        }
    }
    
    profile << "  File: " << fileCount << "\n";
    profile << "  Command Line: " << commandLineCount << "\n";
    profile << "  Default: " << defaultCount << "\n";
    profile << "  Plugin: " << otherCount << "\n\n";
    
    // Profile by type
    profile << "Entries by type:" << std::endl;
    int stringCount = 0, intCount = 0, int64Count = 0, doubleCount = 0, boolCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::string>(entry.value)) stringCount++;
        else if (std::holds_alternative<int>(entry.value)) intCount++;
        else if (std::holds_alternative<int64_t>(entry.value)) int64Count++;
        else if (std::holds_alternative<double>(entry.value)) doubleCount++;
        else if (std::holds_alternative<bool>(entry.value)) boolCount++;
    }
    
    profile << "  String: " << stringCount << "\n";
    profile << "  Integer: " << intCount << "\n";
    profile << "  Int64: " << int64Count << "\n";
    profile << "  Double: " << doubleCount << "\n";
    profile << "  Boolean: " << boolCount << "\n\n";
    
    // Identify potentially conflicting settings
    profile << "Potential conflicts:" << std::endl;
    if (has("stdp_ltp_weight") && has("stdp_ltd_weight")) {
        auto ltp = get<float>("stdp_ltp_weight");
        auto ltd = get<float>("stdp_ltd_weight");
        if (ltp.has_value() && ltd.has_value() && ltp.value() >= ltd.value()) {
            profile << "  Warning: STDP LTP weight (" << ltp.value() 
                   << ") should be less than LTD weight (" << ltd.value() << ")\n";
        }
    }
    
    return profile.str();
}

// Add batch configuration operations
void Config::batchSet(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& [key, value] : values) {
        set(key, value, source);
    }
}

void Config::batchSet(const std::map<std::string, std::string>& values, ConfigSource source) {
    for (const auto& [key, value] : values) {
        set(key, value, source);
    }
}

// Add configuration comparison
bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            return false;
        }
        
        if (entry.value != otherIt->value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::diff(const Config& other) const {
    std::ostringstream diff;
    diff << "=== Configuration Differences ===\n";
    
    // Find keys in this config but not in other
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            diff << "+ " << entry.key << " = " << toString(entry.value) << "\n";
        } else if (entry.value != otherIt->value) {
            diff << "* " << entry.key << " changed from " 
                 << toString(otherIt->value) << " to " << toString(entry.value) << "\n";
        }
    }
    
    // Find keys in other but not in this
    for (const auto& entry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (thisIt == pImpl->entries.end()) {
            diff << "- " << entry.key << " = " << toString(entry.value) << "\n";
        }
    }
    
    return diff.str();
}
