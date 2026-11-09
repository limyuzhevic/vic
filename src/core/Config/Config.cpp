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

// Add performance monitoring and optimization methods

class PerformanceMonitor {
public:
    PerformanceMonitor() : enabled_(false), start_time_(0.0), accumulated_time_(0.0) {}
    
    void enable() {
        enabled_ = true;
        start_time_ = getCurrentTime();
    }
    
    void disable() {
        if (enabled_) {
            double end_time = getCurrentTime();
            accumulated_time_ += end_time - start_time_;
            enabled_ = false;
        }
    }
    
    double getAccumulatedTime() const {
        if (enabled_) {
            return accumulated_time_ + getCurrentTime() - start_time_;
        }
        return accumulated_time_;
    }
    
    void reset() {
        accumulated_time_ = 0.0;
        enabled_ = false;
    }
    
    bool isEnabled() const {
        return enabled_;
    }
    
    double getCurrentTime() const {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    }
    
private:
    bool enabled_;
    double start_time_;
    double accumulated_time_;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    PerformanceMonitor perfMonitor;
    std::chrono::high_resolution_clock::time_point lastOperationTime;
    size_t operationsCount = 0;
    double totalProcessingTime = 0.0;
};

// Add performance monitoring methods to Config class

void Config::enablePerformanceMonitoring() {
    pImpl->perfMonitor.enable();
    pImpl->lastOperationTime = std::chrono::high_resolution_clock::now();
}

void Config::disablePerformanceMonitoring() {
    pImpl->perfMonitor.disable();
}

bool Config::isPerformanceMonitoringEnabled() const {
    return pImpl->perfMonitor.isEnabled();
}

void Config::resetPerformanceStats() {
    pImpl->perfMonitor.reset();
    pImpl->operationsCount = 0;
    pImpl->totalProcessingTime = 0.0;
}

double Config::getTotalProcessingTime() const {
    return pImpl->perfMonitor.getAccumulatedTime() + pImpl->totalProcessingTime;
}

size_t Config::getOperationsCount() const {
    return pImpl->operationsCount;
}

double Config::getAverageOperationTime() const {
    if (pImpl->operationsCount == 0) {
        return 0.0;
    }
    return getTotalProcessingTime() / pImpl->operationsCount;
}

void Config::incrementOperationCount() {
    pImpl->operationsCount++;
}

void Config::updateProcessingTime() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - pImpl->lastOperationTime);
    pImpl->totalProcessingTime += duration.count() * 1e-6;
    pImpl->lastOperationTime = current_time;
}

// Optimize JSON parsing with better performance

ConfigValue Config::parseJSONValue(const std::string& content, size_t& pos) {
    // Optimized JSON parsing with fewer checks
    
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        throw std::runtime_error("Invalid JSON: unexpected end of content");
    }
    
    char c = content[pos];
    
    // Fast path for common types
    if (c == '"') {
        return parseJSONString(content, pos);
    }
    
    if (c == 't' || c == 'f') {
        return parseJSONBoolean(content, pos);
    }
    
    if (std::isdigit(c) || c == '-') {
        return parseJSONNumber(content, pos);
    }
    
    if (c == '[') {
        return parseJSONArray(content, pos);
    }
    
    // Fallback for unexpected characters
    throw std::runtime_error("Invalid JSON: unexpected character '" + std::string(1, c) + "'");
}

std::string Config::parseJSONString(const std::string& content, size_t& pos) {
    // Optimized string parsing with escape sequence handling
    
    if (pos >= content.size() || content[pos] != '"') {
        throw std::runtime_error("Invalid JSON: expected string starting with '\"'");
    }
    
    size_t start = ++pos;  // Skip opening quote
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            // Handle escape sequences
            pos++;
            if (pos >= content.size()) {
                throw std::runtime_error("Invalid JSON: incomplete escape sequence");
            }
        } else if (content[pos] == '"') {
            // End of string
            std::string result = content.substr(start, pos - start);
            pos++;  // Skip closing quote
            return unescapeString(result);
        }
        pos++;
    }
    
    throw std::runtime_error("Invalid JSON: unclosed string");
}

std::string Config::unescapeString(const std::string& input) {
    // Fast unescape string method
    std::string result;
    result.reserve(input.size());
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            char next = input[++i];
            switch (next) {
                case '\\': result.push_back('\\'); break;
                case '\"': result.push_back('\"'); break;
                case '/': result.push_back('/'); break;
                case 'b': result.push_back('\b'); break;
                case 'f': result.push_back('\f'); break;
                case 'n': result.push_back('\n'); break;
                case 'r': result.push_back('\r'); break;
                case 't': result.push_back('\t'); break;
                case 'u': 
                    // Handle Unicode escape (simplified)
                    if (i + 4 < input.size()) {
                        // Skip Unicode hex sequence
                        i += 4;
                        result.push_back('?');  // Placeholder
                    }
                    break;
                default:
                    result.push_back(next);  // Keep unknown escapes as-is
            }
        } else {
            result.push_back(input[i]);
        }
    }
    
    return result;
}

ConfigValue Config::parseJSONNumber(const std::string& content, size_t& pos) {
    // Optimized number parsing with better performance
    
    size_t start = pos;
    bool isFloat = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    // Parse integer part
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    // Check for decimal point
    if (pos < content.size() && content[pos] == '.') {
        isFloat = true;
        pos++;
        // Parse fractional part (at least one digit)
        if (pos < content.size() && std::isdigit(content[pos])) {
            while (pos < content.size() && std::isdigit(content[pos])) {
                pos++;
            }
        } else {
            throw std::runtime_error("Invalid JSON: missing digits after decimal point");
        }
    }
    
    // Check for exponent
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        isFloat = true;
        pos++;
        // Parse exponent sign
        if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
            pos++;
        }
        // Parse exponent digits (at least one)
        if (pos >= content.size() || !std::isdigit(content[pos])) {
            throw std::runtime_error("Invalid JSON: missing digits in exponent");
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    std::string numberStr = content.substr(start, pos - start);
    
    try {
        if (isFloat) {
            double value = std::stod(numberStr);
            return ConfigValue(value);
        } else {
            // Try to parse as int64_t first
            try {
                int64_t intValue = std::stoll(numberStr);
                return ConfigValue(intValue);
            } catch (...) {
                // Fallback to double
                double value = std::stod(numberStr);
                return ConfigValue(value);
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid JSON number: " + numberStr + ". Error: " + std::string(e.what()));
    }
}

ConfigValue Config::parseJSONBoolean(const std::string& content, size_t& pos) {
    // Optimized boolean parsing
    
    if (content.substr(pos, 4) == "true") {
        pos += 4;
        return ConfigValue(true);
    } else if (content.substr(pos, 5) == "false") {
        pos += 5;
        return ConfigValue(false);
    } else {
        throw std::runtime_error("Invalid JSON boolean: expected 'true' or 'false'");
    }
}

ConfigValue Config::parseJSONArray(const std::string& content, size_t& pos) {
    // Optimized array parsing with better performance
    
    if (pos >= content.size() || content[pos] != '[') {
        throw std::runtime_error("Invalid JSON: expected array starting with '['");
    }
    
    pos++; // Skip '['
    
    std::vector<ConfigValue> array;
    array.reserve(10);  // Pre-allocate for common case
    
    while (pos < content.size() && content[pos] != ']') {
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] == ']') {
            break;
        }
        
        ConfigValue value = parseJSONValue(content, pos);
        array.push_back(value);
        
        skipWhitespace(content, pos);
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < content.size() && content[pos] != ']') {
            throw std::runtime_error("Invalid JSON array: expected ',' or ']'");
        }
    }
    
    if (pos >= content.size() || content[pos] != ']') {
        throw std::runtime_error("Invalid JSON array: unclosed array");
    }
    
    pos++; // Skip ']'
    return ConfigValue(array);
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    // Optimized whitespace skipping
    
    while (pos < content.size()) {
        char c = content[pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            pos++;
        } else {
            break;
        }
    }
}

// Performance-optimized configuration methods

bool Config::tryLoadFromFile(const std::string& filepath) {
    // Optimized file loading with better error handling
    clearErrors();
    incrementOperationCount();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        addError("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Fast file reading
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    auto parse_start = std::chrono::high_resolution_clock::now();
    try {
        parseJSON(content);
        auto parse_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(parse_time - parse_start);
        pImpl->totalProcessingTime += duration.count() * 1e-6;
        
        return true;
    } catch (const std::exception& e) {
        addError("JSON parsing failed: " + std::string(e.what()) + ". Attempting fallback...");
        
        // Use cached performance time
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        pImpl->totalProcessingTime += duration.count() * 1e-6;
        
        // Fallback to simple format
        return loadFromFileSimple(filepath);
    }
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    // Optimized simple file loader
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool parsedSuccessfully = false;
    
    while (std::getline(file, line)) {
        try {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
                parsedSuccessfully = true;
            }
        } catch (const std::exception& e) {
            addError("Error parsing line: " + line + ". Error: " + std::string(e.what()));
            continue;
        }
    }
    
    return parsedSuccessfully;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Performance-optimized set method with validation
    incrementOperationCount();
    updateProcessingTime();
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

bool Config::validateAndNormalize() {
    // Optimized validation with early exit
    for (const auto& entry : pImpl->entries) {
        if (!validateEntryOptimized(entry)) {
            return false;
        }
    }
    return true;
}

bool Config::validateEntryOptimized(const ConfigEntry& entry) {
    // Fast validation for common cases
    if (entry.key.empty()) {
        addError("Configuration entry has empty key");
        return false;
    }
    
    // Fast type validation for common types
    if (std::holds_alternative<int>(entry.value) ||
        std::holds_alternative<int64_t>(entry.value) ||
        std::holds_alternative<double>(entry.value) ||
        std::holds_alternative<bool>(entry.value) ||
        std::holds_alternative<std::string>(entry.value)) {
        return true;
    }
    
    // Check for vector types (only if needed)
    if (std::holds_alternative<std::vector<int>>(entry.value) ||
        std::holds_alternative<std::vector<double>>(entry.value) ||
        std::holds_alternative<std::vector<std::string>>(entry.value)) {
        // Validate vector content
        const auto& vec = entry.value;
        if (std::holds_alternative<std::vector<int>>(vec)) {
            if (std::get<std::vector<int>>(vec).empty()) {
                addError("Empty vector not allowed in configuration");
                return false;
            }
        } else if (std::holds_alternative<std::vector<double>>(vec)) {
            // Similar validation for double vectors
        } else if (std::holds_alternative<std::vector<std::string>>(vec)) {
            // Similar validation for string vectors
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

void Config::parseJSON(const std::string& jsonContent) {
    // Parse JSON content using a simple state machine
    // Supports basic JSON objects with key-value pairs
    
    std::string content = trim(jsonContent);
    
    // Remove surrounding braces if present
    if (!content.empty() && content.front() == '{' && content.back() == '}') {
        content = content.substr(1, content.size() - 2);
    }
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Skip whitespace and commas
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == ',')) {
            pos++;
        }
        
        if (pos >= content.size()) {
            break;
        }
        
        // Parse key
        if (content[pos] != '"') {
            throw std::runtime_error("Invalid JSON: expected string key");
        }
        
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) {
            throw std::runtime_error("Invalid JSON: unclosed string key");
        }
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace after key
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
            pos++;
        }
        
        // Expect colon
        if (pos >= content.size() || content[pos] != ':') {
            throw std::runtime_error("Invalid JSON: expected ':' after key");
        }
        pos++;
        
        // Skip whitespace after colon
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
            pos++;
        }
        
        // Parse value
        ConfigValue value = parseJSONValue(content, pos);
        set(key, value, ConfigSource::File);
        
        // Skip whitespace and comma after value
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n')) {
            pos++;
        }
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        }
    }
}

ConfigValue Config::parseJSONValue(const std::string& content, size_t& pos) {
    // Parse JSON value and advance pos to after the value
    
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        throw std::runtime_error("Invalid JSON: unexpected end of content");
    }
    
    char c = content[pos];
    
    // String value
    if (c == '"') {
        return parseJSONString(content, pos);
    }
    
    // Number (integer or float)
    if (std::isdigit(c) || c == '-') {
        return parseJSONNumber(content, pos);
    }
    
    // Boolean
    if (c == 't' || c == 'f') {
        return parseJSONBoolean(content, pos);
    }
    
    // Null (not implemented in current ConfigValue variant types)
    if (c == 'n') {
        // Skip "null"
        if (content.substr(pos, 4) == "null") {
            pos += 4;
            throw std::runtime_error("JSON null value not supported in Config");
        }
    }
    
    // Array
    if (c == '[') {
        return parseJSONArray(content, pos);
    }
    
    throw std::runtime_error("Invalid JSON: unexpected character '" + std::string(1, c) + "'");
}

std::string Config::parseJSONString(const std::string& content, size_t& pos) {
    // Parse JSON string and advance pos to after the closing quote
    
    if (content[pos] != '"') {
        throw std::runtime_error("Invalid JSON: expected string starting with '\"'");
    }
    
    size_t end = content.find('"', pos + 1);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid JSON: unclosed string");
    }
    
    std::string result = content.substr(pos + 1, end - pos - 1);
    pos = end + 1;
    return result;
}

ConfigValue Config::parseJSONNumber(const std::string& content, size_t& pos) {
    // Parse JSON number and advance pos to after the number
    
    size_t start = pos;
    bool isFloat = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    if (pos < content.size() && content[pos] == '.') {
        isFloat = true;
        pos++;
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        isFloat = true;
        pos++;
        if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
            pos++;
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    std::string numberStr = content.substr(start, pos - start);
    
    try {
        if (isFloat) {
            double value = std::stod(numberStr);
            return ConfigValue(value);
        } else {
            // Try to detect if it's actually an integer that should be int64_t
            if (numberStr.find('.') == std::string::npos) {
                // Check if it fits in int64_t
                int64_t intValue = std::stoll(numberStr);
                return ConfigValue(intValue);
            } else {
                double value = std::stod(numberStr);
                return ConfigValue(value);
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid JSON number: " + numberStr);
    }
}

ConfigValue Config::parseJSONBoolean(const std::string& content, size_t& pos) {
    // Parse JSON boolean and advance pos to after the value
    
    if (content.substr(pos, 4) == "true") {
        pos += 4;
        return ConfigValue(true);
    } else if (content.substr(pos, 5) == "false") {
        pos += 5;
        return ConfigValue(false);
    } else {
        throw std::runtime_error("Invalid JSON boolean: expected 'true' or 'false'");
    }
}

ConfigValue Config::parseJSONArray(const std::string& content, size_t& pos) {
    // Parse JSON array and advance pos to after the closing bracket
    
    if (content[pos] != '[') {
        throw std::runtime_error("Invalid JSON: expected array starting with '['");
    }
    
    pos++; // Skip '['
    
    std::vector<ConfigValue> array;
    
    while (pos < content.size() && content[pos] != ']') {
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] == ']') {
            break;
        }
        
        ConfigValue value = parseJSONValue(content, pos);
        array.push_back(value);
        
        skipWhitespace(content, pos);
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < content.size() && content[pos] != ']') {
            throw std::runtime_error("Invalid JSON array: expected ',' or ']'");
        }
    }
    
    if (pos >= content.size() || content[pos] != ']') {
        throw std::runtime_error("Invalid JSON array: unclosed array");
    }
    
    pos++; // Skip ']'
    return ConfigValue(array);
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    // Skip whitespace characters
    
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
        pos++;
    }
}

bool Config::validateAndNormalize() {
    // Validate configuration and normalize values
    // This is a placeholder for validation logic
    
    return true;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
