#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON using a simple parser
    // For now, we'll implement a basic JSON parser that handles:
    // - Objects with key-value pairs
    // - Strings, numbers, booleans, null
    // - Nested objects (limited depth)
    
    try {
        return parseJSON(content, ConfigSource::File);
    } catch (const std::exception&) {
        // If JSON parsing fails, fall back to simple key=value format
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
        return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

// Simple JSON parser implementation
bool Config::parseJSON(const std::string& json, ConfigSource source) {
    // Simple state machine JSON parser
    // Supports: objects with key-value pairs, strings, numbers, booleans, null
    
    size_t pos = 0;
    skipWhitespace(json, pos);
    
    if (json[pos] != '{') {
        throw std::runtime_error("JSON must start with '{'");
    }
    
    ++pos;
    skipWhitespace(json, pos);
    
    while (pos < json.size() && json[pos] != '}') {
        // Parse key
        skipWhitespace(json, pos);
        if (json[pos] != '"') {
            throw std::runtime_error("Expected quote after '{'");
        }
        
        size_t keyStart = ++pos;
        while (pos < json.size() && json[pos] != '"') {
            ++pos;
        }
        if (pos < json.size()) {
            std::string key = json.substr(keyStart, pos - keyStart);
            skipWhitespace(json, ++pos);
            
            if (pos < json.size() && json[pos] != '}') {
                if (json[pos] != '=') {
                    throw std::runtime_error("Expected '=' after key");
                }
                
                ++pos;
                skipWhitespace(json, pos);
                
                // Parse value
                ConfigValue value;
                if (json[pos] == '"') {
                    // String
                    size_t valueStart = ++pos;
                    while (pos < json.size() && json[pos] != '"') {
                        ++pos;
                    }
                    if (pos >= json.size()) {
                        throw std::runtime_error("Unterminated string in JSON");
                    }
                    std::string strValue = json.substr(valueStart, pos - valueStart);
                    set(key, strValue, source);
                    ++pos;
                } else if (json[pos] == 't' || json[pos] == 'f') {
                    // Boolean
                    std::string boolStr = "true";
                    if (json[pos] == 'f') boolStr = "false";
                    if (json.substr(pos, boolStr.size()) != boolStr) {
                        throw std::runtime_error("Invalid boolean value");
                    }
                    bool boolValue = (boolStr == "true");
                    set(key, boolValue, source);
                    pos += boolStr.size();
                } else if (json[pos] == 'n') {
                    // Null
                    if (json.substr(pos, 4) != "null") {
                        throw std::runtime_error("Expected 'null'");
                    }
                    // Skip null, don't set config entry
                    pos += 4;
                } else {
                    // Number
                    size_t numStart = pos;
                    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
                        ++pos;
                    }
                    if (pos == numStart) {
                        throw std::runtime_error("Expected number value");
                    }
                    std::string numStr = json.substr(numStart, pos - numStart);
                    
                    // Try to parse as float first, then int
                    try {
                        float floatVal = std::stof(numStr);
                        set(key, floatVal, source);
                    } catch (...) {
                        try {
                            int intVal = std::stoi(numStr);
                            set(key, intVal, source);
                        } catch (...) {
                            set(key, numStr, source);
                        }
                    }
                }
                
                skipWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ',') {
                    ++pos;
                }
            }
        }
        
        skipWhitespace(json, pos);
    }
    
    if (pos >= json.size()) {
        throw std::runtime_error("Unexpected end of JSON");
    }
    
    return true;
}

void Config::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        ++pos;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper functions for Config export/import
std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << v << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << v[i];
            }
            oss << "]";
        } else {
            oss << v;
        }
    }, value);
    return oss.str();
}

std::string Config::configValueToJSON(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << v << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                if constexpr (std::is_same_v<T, std::vector<std::string>) {
                    oss << "\"" << v[i] << "\"";
                } else {
                    oss << v[i];
                }
            }
            oss << "]";
        } else {
            oss << v;
        }
    }, value);
    return oss.str();
}

// Configuration validation
bool Config::validate(const std::string& schema) const {
    if (schema.empty()) {
        // Basic validation
        std::vector<std::string> errors;
        validate(errors);
        return errors.empty();
    }
    
    // TODO: Implement schema validation
    // For now, just do basic validation
    std::vector<std::string> errors;
    validate(errors);
    return errors.empty();
}

bool Config::validate(const Config& reference, std::string& errors) const {
    return validate(reference, errors, "");
}

bool Config::validate(const Config& reference, std::string& errors, const std::string& schema) const {
    errors.clear();
    
    // Check that all required keys exist in reference
    for (const auto& entry : reference.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == entry.key) {
                found = true;
                break;
            }
        }
        if (!found && entry.source == ConfigSource::Default) {
            errors.push_back("Missing required configuration key: \"" + entry.key + "\"");
        }
    }
    
    // Check that values are within acceptable ranges (basic validation)
    for (const auto& thisEntry : pImpl->entries) {
        // Basic range validation for common keys
        if (thisEntry.key == "neuron_count") {
            if (std::holds_alternative<int64_t>(thisEntry.value)) {
                int64_t neurons = std::get<int64_t>(thisEntry.value);
                if (neurons <= 0 || neurons > 1000000) {
                    errors.push_back("Invalid neuron_count: must be between 1 and 1,000,000");
                }
            }
        }
        
        else if (thisEntry.key == "connection_probability") {
            if (std::holds_alternative<double>(thisEntry.value)) {
                double prob = std::get<double>(thisEntry.value);
                if (prob < 0.0 || prob > 1.0) {
                    errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
                }
            }
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    validate(errors);
    return errors;
}

std::vector<std::string> Config::getValidationErrors(const Config& reference) const {
    std::vector<std::string> errors;
    validate(reference, errors);
    return errors;
}

std::vector<std::string> Config::getValidationErrors(const Config& reference, const std::string& schema) const {
    std::vector<std::string> errors;
    validate(reference, errors, schema);
    return errors;
}

// Configuration schema management
void Config::setSchema(const std::string& schema) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema = schema;
}

std::string Config::getSchema() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->schema;
}

void Config::clearSchema() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema.clear();
}

// Configuration templates
bool Config::applyTemplate(const std::string& template_name) {
    // TODO: Implement template application
    // For now, just set some common template values
    if (template_name == "demo") {
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        set("connection_probability", 0.1f, ConfigSource::Runtime);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Runtime);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Runtime);
        return true;
    } else if (template_name == "learning") {
        set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
        set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
        set("connection_probability", 0.08f, ConfigSource::Runtime);
        set("stdp_ltp_weight", 0.015f, ConfigSource::Runtime);
        set("stdp_ltd_weight", 0.013f, ConfigSource::Runtime);
        set("synaptogenesis_rate", 0.00005f, ConfigSource::Runtime);
        return true;
    } else if (template_name == "large") {
        set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Runtime);
        set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
        set("connection_probability", 0.05f, ConfigSource::Runtime);
        return true;
    }
    
    return false;
}

bool Config::saveTemplate(const std::string& template_name) const {
    // TODO: Implement template save
    return false;
}

std::vector<std::string> Config::getAvailableTemplates() const {
    return {"demo", "learning", "large"};
}

// Configuration history
void Config::enableHistory(bool enable) {
    std::lock_guard<stdCholesterol> lock(pImpl->mutex);
    pImpl->historyEnabled = enable;
    if (!enable) {
        clearHistory();
    }
}

bool Config::isHistoryEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->historyEnabled;
}

std::vector<Config> Config::getHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->history;
}

bool Config::hasHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return !pImpl->history.empty();
}

void Config::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->history.clear();
}
}
