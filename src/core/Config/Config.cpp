#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <iomanip>

// Helper function to trim whitespace from strings
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to escape special characters in strings
static std::string escapeString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\t': escaped += "\\t"; break;
            case '\r': escaped += "\\r"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Helper function to parse JSON values
static std::optional<ConfigValue> parseJSONValue(const std::string& valueStr) {
    std::string trimmed = trim(valueStr);
    if (trimmed.empty()) {
        return std::nullopt;
    }
    
    // Parse boolean
    if (trimmed == "true" || trimmed == "false") {
        return std::stoi(trimmed == "true") != 0;
    }
    
    // Parse integers
    try {
        if (trimmed.find('.') == std::string::npos) {
            return std::stoll(trimmed);
        }
    } catch (...) {}
    
    try {
        return std::stod(trimmed);
    } catch (...) {}
    
    // Parse string (already quoted)
    if ((trimmed.front() == '"' && trimmed.back() == '"') ||
        (trimmed.front() == '\'' && trimmed.back() == '\'')) {
        return trim(trimmed.substr(1, trimmed.size() - 2));
    }
    
    // Array parsing (simplified)
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        // For now, just treat as string
        return valueStr;
    }
    
    return std::nullopt;
}

// Helper function to parse JSON object and load into config
static bool parseJSONObject(const std::string& jsonContent, std::vector<ConfigEntry>& entries) {
    // Simple JSON object parsing (key: value pairs)
    std::string content = trim(jsonContent);
    if (content.front() != '{' || content.back() != '}') {
        return false;
    }
    
    content = content.substr(1, content.size() - 2);  // Remove braces
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Find key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Handle simple values (string, number, boolean)
        if (content[valueStart] == '"' || content[valueStart] == '\'') {
            // String value
            size_t valueEnd = content.find('"', valueStart);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            auto valueOpt = parseJSONValue("\"" + valueStr + "\"");
            if (valueOpt) {
                entries.emplace_back(key, *valueOpt, ConfigSource::File);
            }
            pos = valueEnd + 1;
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.find('}', valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            auto valueOpt = parseJSONValue(valueStr);
            if (valueOpt) {
                entries.emplace_back(key, *valueOpt, ConfigSource::File);
            }
            pos = valueEnd + 1;
        } else {
            // Number value
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) valueEnd = content.find('}', valueStart);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            auto valueOpt = parseJSONValue(valueStr);
            if (valueOpt) {
                entries.emplace_back(key, *valueOpt, ConfigSource::File);
            }
            pos = valueEnd + 1;
        }
        
        // Find next comma
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == ',')) {
            pos++;
        }
    }
    
    return true;
}

namespace nlm {

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open configuration file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Clear existing entries before loading
    pImpl->entries.clear();
    
    // Try JSON parsing first (if file contains { and })
    if (!content.empty() && content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        if (parseJSONObject(content, pImpl->entries)) {
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        }
    }
    
    // Fallback to simple key=value format
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
            
            // Try to parse value as different types
            auto valueOpt = parseJSONValue(value);
            if (valueOpt) {
                set(key, *valueOpt, ConfigSource::File);
            } else {
                // Fallback to string
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    } else {
        NLM_LOG_WARNING("No valid configuration found in: " + filepath);
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true;  // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Invalid command line argument: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not save configuration to file: " + filepath);
        return false;
    }
    
    // Try to save as JSON for modern formats
    file << "{\n";
    bool firstEntry = true;
    for (const auto& entry : pImpl->entries) {
        if (!firstEntry) {
            file << ",\n";
        }
        firstEntry = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << escapeString(arg[i]) << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << "\"unknown\"";
            }
        }, entry.value);
        
        if (entry.description.empty()) {
            file << "  // " << entry.key << "\n";
        } else {
            file << "  // " << entry.description << "\n";
        }
    }
    file << "}\n";
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
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

// Template implementations for the new helper functions

std::vector<std::string> Config::getKeysSortedBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<ConfigSource> Config::getValueSources(const std::string& key) const {
    std::vector<ConfigSource> sources;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        sources.push_back(it->source);
    }
    return sources;
}

bool Config::wasSetBySource(const std::string& key, ConfigSource source) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key, source](const ConfigEntry& e) { return e.key == key && e.source == source; });
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Overwrite existing entries or add new ones
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source;
            it->description = entry.description;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

void Config::copyFrom(const Config& other) {
    pImpl->entries = other.pImpl->entries;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

const ConfigEntry& Config::getEntryAt(size_t index) const {
    return pImpl->entries.at(index);
}

const ConfigEntry& Config::getEntryAt(size_t index) const {
    return pImpl->entries.at(index);
}

// Configuration validation system
namespace {
    struct ConfigValidationRule {
        std::string key;
        std::string expectedType;
        std::string defaultValue;
        std::string description;
        std::function<bool(const ConfigValue&)> validator;
    };
    
    std::vector<ConfigValidationRule> getDefaultValidationRules() {
        std::vector<ConfigValidationRule> rules;
        
        // Common configuration validation rules
        rules.push_back({"random_seed", "int64_t", "42", "Random seed for reproducibility", 
                         [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }});
        
        rules.push_back({"simulation_timestep", "double", "0.001", "Simulation timestep in seconds",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; }});
        
        rules.push_back({"neuron_count", "int64_t", "500", "Number of neurons in the brain",
                         [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }});
        
        rules.push_back({"region_count", "int64_t", "1", "Number of regions",
                         [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }});
        
        rules.push_back({"connection_probability", "double", "0.15", "Probability of connections between neurons",
                         [](const ConfigValue& v) { 
                             return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; 
                         }});
        
        rules.push_back({"stdp_ltp_weight", "double", "0.02", "STDP LTP weight",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }});
        
        rules.push_back({"stdp_ltd_weight", "double", "0.015", "STDP LTD weight",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }});
        
        rules.push_back({"stdp_tau", "double", "20.0", "STDP time constant",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; }});
        
        rules.push_back({"synaptogenesis_rate", "double", "0.0001", "Structural plasticity synaptogenesis rate",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; }});
        
        rules.push_back({"pruning_rate", "double", "0.00001", "Structural plasticity pruning rate",
                         [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; }});
        
        return rules;
    }
}

bool Config::validate() const {
    const auto& validationRules = getDefaultValidationRules();
    
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Check for required keys with valid values
    for (const auto& rule : validationRules) {
        if (has(rule.key)) {
            auto value = get(rule.key);
            if (value.has_value()) {
                if (!rule.validator(value.value())) {
                    errors.push_back("Configuration key '" + rule.key + "' has invalid value type. Expected: " + rule.expectedType);
                }
            } else {
                warnings.push_back("Configuration key '" + rule.key + "' is missing (using default).");
            }
        }
    }
    
    // Check for unknown keys (optional)
    for (const auto& entry : pImpl->entries) {
        bool isKnown = false;
        for (const auto& rule : validationRules) {
            if (rule.key == entry.key) {
                isKnown = true;
                break;
            }
        }
        if (!isKnown) {
            warnings.push_back("Configuration key '" + entry.key + "' is not in the known schema.");
        }
    }
    
    // Log validation results
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  ERROR: " + error);
        }
    }
    
    if (!warnings.empty()) {
        NLM_LOG_WARNING("Configuration validation warnings:");
        for (const auto& warning : warnings) {
            NLM_LOG_WARNING("  WARNING: " + warning);
        }
    }
    
    if (errors.empty()) {
        NLM_LOG_INFO("Configuration validation passed.");
    }
    
    return errors.empty();
}

bool Config::isValid() const {
    return validate();
}

} // namespace nlm
