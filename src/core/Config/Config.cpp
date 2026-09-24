#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <numeric>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    bool success = false;
    if (ext == "json" || ext == "jsonc") {
        success = loadFromString(content, "json");
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromString(content, "yaml");
    } else {
        // Try to auto-detect format
        success = loadFromString(content, "auto");
    }
    
    return success;
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

bool Config::loadFromString(const std::string& content, const std::string& format) {
    try {
        if (format == "json") {
            return parseJSON(content);
        } else if (format == "yaml" || format == "yml") {
            return parseYAML(content);
        } else {
            // Auto-detect format
            if (content.empty()) return true;
            
            char firstChar = content[0];
            if (firstChar == '{' || firstChar == '[') {
                return parseJSON(content);
            } else if (firstChar == ' ' || firstChar == '\t' || firstChar == '\n' ||
                      firstChar == '#' || firstChar == '-' || firstChar == ':') {
                return parseYAML(content);
            } else {
                // Default to simple key=value format
                return parseKeyValue(content);
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on file extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::string content;
    if (ext == "json") {
        content = toJSON();
    } else if (ext == "yaml" || ext == "yml") {
        content = toYAML();
    } else {
        // Default to text format
        content = toText();
    }
    
    file << content;
    return file.good();
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
    setWithValidation(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setWithValidation(key, ConfigValue(value), source);
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return false;
    }
    
    set(key, value, source);
    return true;
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Private helper methods

bool Config::parseJSON(const std::string& content) {
    // Simplified JSON parsing - in production, use a proper JSON library
    clear();
    
    // Remove whitespace
    std::string cleanContent;
    for (char c : content) {
        if (!std::isspace(c)) {
            cleanContent += c;
        }
    }
    
    if (cleanContent.empty()) return true;
    
    // Simple JSON object parsing (key-value pairs)
    if (cleanContent.front() != '{' || cleanContent.back() != '}') {
        return false;
    }
    
    cleanContent = cleanContent.substr(1, cleanContent.size() - 2);
    
    size_t pos = 0;
    while (pos < cleanContent.size()) {
        // Find next key
        size_t keyStart = cleanContent.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleanContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleanContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < cleanContent.size() && (std::isspace(cleanContent[pos]) || cleanContent[pos] == ':')) {
            pos++;
        }
        
        if (pos >= cleanContent.size()) break;
        
        // Parse value based on next character
        char nextChar = cleanContent[pos];
        std::string valueStr;
        
        if (nextChar == '"') {
            // String value
            size_t valStart = pos;
            size_t valEnd = cleanContent.find('"', pos + 1);
            if (valEnd == std::string::npos) break;
            valueStr = cleanContent.substr(valStart, valEnd - valStart + 1);
            pos = valEnd + 1;
        } else if (nextChar == 't' || nextChar == 'f') {
            // Boolean value
            size_t valEnd = cleanContent.find_first_of(",}", pos);
            if (valEnd == std::string::npos) break;
            valueStr = cleanContent.substr(pos, valEnd - pos);
            pos = valEnd;
        } else if (nextChar == '-' || std::isdigit(nextChar)) {
            // Number
            size_t valEnd = cleanContent.find_first_of(",}", pos);
            if (valEnd == std::string::npos) break;
            valueStr = cleanContent.substr(pos, valEnd - pos);
            pos = valEnd;
        } else {
            break;
        }
        
        // Convert string to appropriate type and add to config
        ConfigValue value = parseJSONValue(valueStr);
        set(key, value, ConfigSource::File);
        
        // Skip comma and whitespace
        while (pos < cleanContent.size() && (cleanContent[pos] == ',' || std::isspace(cleanContent[pos]))) {
            pos++;
        }
    }
    
    return true;
}

bool Config::parseYAML(const std::string& content) {
    // Simplified YAML parsing - in production, use a proper YAML library
    clear();
    
    // Remove comments and handle simple YAML key-value pairs
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line = trim(line);
        if (line.empty()) continue;
        
        // Parse key: value
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                    (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue = parseYAMLValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseKeyValue(const std::string& content) {
    // Parse simple key=value format
    clear();
    
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
            
            ConfigValue configValue = parseValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

ConfigValue Config::parseValue(const std::string& str) {
    if (str.empty()) {
        return std::string();
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Try to parse as vector
    if (str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<double> vec;
        
        size_t start = 0;
        size_t end = 0;
        while ((end = inner.find(',', start)) != std::string::npos) {
            std::string token = trim(inner.substr(start, end - start));
            try {
                vec.push_back(std::stod(token));
            } catch (...) {}
            start = end + 1;
        }
        
        if (start < inner.size()) {
            std::string token = trim(inner.substr(start));
            try {
                vec.push_back(std::stod(token));
            } catch (...) {}
        }
        
        if (!vec.empty()) {
            return vec;
        }
    }
    
    // Default to string
    return str;
}

ConfigValue Config::parseJSONValue(const std::string& str) {
    // Parse JSON-like string value
    std::string value = str;
    
    // Remove surrounding quotes
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.size() - 2);
    }
    
    // Try to parse as boolean
    if (value == "true") return true;
    if (value == "false") return false;
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) return intVal;
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) return doubleVal;
    } catch (...) {}
    
    // Return as string
    return value;
}

ConfigValue Config::parseYAMLValue(const std::string& str) {
    // Parse YAML-like string value
    std::string value = str;
    
    // Remove quotes
    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                             (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Try to parse as boolean
    if (value == "true") return true;
    if (value == "false") return false;
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) return intVal;
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) return doubleVal;
    } catch (...) {}
    
    // Return as string
    return value;
}

std::string Config::toJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJSON(entry.value);
    }
    
    return json.dump(2);
}

std::string Config::toYAML() const {
    // Simplified YAML generation
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        oss << configValueToYAML(entry.value) << "\n";
    }
    
    return oss.str();
}

std::string Config::toText() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = " << configValueToText(entry.value) << "\n";
    }
    
    return oss.str();
}

std::string Config::configValueToJSON(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json json = arg;
            return json.dump();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json json = arg;
            return json.dump();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json json = arg;
            return json.dump();
        } else {
            return "\"" + std::to_string(arg) + "\"";
        }
    }, value);
}

std::string Config::configValueToYAML(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
            return oss.str();
        } else {
            return std::to_string(arg);
        }
    }, value);
}

std::string Config::configValueToText(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value);
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

bool Config::validateKey(const std::string& key) {
    if (key.empty()) return false;
    if (key.size() > 255) return false;
    
    // Check for invalid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '.' || c == '-')) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return !arg.empty() && arg.size() <= 1024;
        } else if constexpr (std::is_same_v<T, bool>) {
            return true; // Always valid
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            return true; // Always valid
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg.size() <= 100 && std::all_of(arg.begin(), arg.end(), [](int v) { return v >= -1000000 && v <= 1000000; });
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg.size() <= 100 && std::all_of(arg.begin(), arg.end(), [](double v) { return v >= -1e6 && v <= 1e6; });
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg.size() <= 100 && std::all_of(arg.begin(), arg.end(), [](const std::string& v) { return !v.empty() && v.size() <= 100; });
        } else {
            return false;
        }
    }, value);
}

bool Config::isValidPrimitiveType(const ConfigValue& value) {
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value) ||
           std::holds_alternative<bool>(value) ||
           std::holds_alternative<std::string>(value);
}

bool Config::isValidVectorType(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

std::vector<ConfigSource> Config::getSources() const {
    std::vector<ConfigSource> sources;
    for (const auto& entry : pImpl->entries) {
        sources.push_back(entry.source);
    }
    return sources;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.getEntries()) {
        if (!has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

Config Config::getDefaultConfig() {
    Config config;
    
    // Add default brain configuration
    config.set("brain.neuron_count", 1000, ConfigSource::Default);
    config.set("brain.synapse_density", 0.1, ConfigSource::Default);
    config.set("brain.connection_probability", 0.05, ConfigSource::Default);
    config.set("brain.initial_weight_mean", 0.5, ConfigSource::Default);
    config.set("brain.initial_weight_std", 0.1, ConfigSource::Default);
    config.set("brain.v_thresh", -50.0, ConfigSource::Default);
    config.set("brain.v_rest", -70.0, ConfigSource::Default);
    config.set("brain.v_reset", -75.0, ConfigSource::Default);
    config.set("brain.tau_mem", 20.0, ConfigSource::Default);
    config.set("brain.tau_ref", 2.0, ConfigSource::Default);
    
    // Add plasticity configuration
    config.set("plasticity.stdp.enable", true, ConfigSource::Default);
    config.set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
    config.set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
    config.set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
    config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config.set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Add neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
    config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config.set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    return config;
}

} // namespace nlm
