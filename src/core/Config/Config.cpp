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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Detect file format by extension
    std::string ext = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(file);
    } else {
        // Default to INI format for backward compatibility
        success = loadFromINI(file);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to parse config file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromINI(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '/') {
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
            
            // Try to infer type
            auto inferredValue = inferType(value);
            if (inferredValue.has_value()) {
                set(key, inferredValue.value(), ConfigSource::File);
            } else {
                set(key, value, ConfigSource::File);
            }
        }
    }
    return true;
}

bool Config::loadFromJSON(std::ifstream& file) {
    try {
        // Read entire file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Parse JSON using a simple parser (in real implementation, use a proper JSON library)
        // For now, implement a simplified version
        return parseJSONContent(content);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(std::ifstream& file) {
    try {
        // Read entire file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Parse YAML (in real implementation, use a proper YAML library)
        // For now, implement a simplified version
        return parseYAMLContent(content);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::parseJSONContent(const std::string& content) {
    // Simplified JSON parser implementation
    // In a real implementation, use a proper JSON library like nlohmann/json
    size_t pos = 0;
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '{') {
            // Object
            pos++;
            while (pos < content.size() && content[pos] != '}') {
                skipWhitespace(content, pos);
                if (content[pos] == '}') break;
                
                // Parse key
                pos++;
                size_t keyStart = pos;
                while (pos < content.size() && content[pos] != ':' && content[pos] != '"') {
                    pos++;
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                
                // Find colon
                while (pos < content.size() && content[pos] != ':') pos++;
                if (pos >= content.size()) break;
                pos++;
                
                skipWhitespace(content, pos);
                
                // Parse value
                auto value = parseJSONValue(content, pos);
                if (value.has_value()) {
                    set(trim(key), value.value(), ConfigSource::File);
                }
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            }
            if (pos < content.size() && content[pos] == '}') pos++;
        } else {
            break;
        }
    }
    return true;
}

bool Config::parseYAMLContent(const std::string& content) {
    // Simplified YAML parser implementation
    // In a real implementation, use a proper YAML library
    return loadFromINI(std::istringstream(content)); // Fallback to INI for now
}

std::optional<ConfigValue> Config::inferType(const std::string& value) const {
    // Try to infer type from string
    std::string trimmed = trim(value);
    
    // Check if boolean
    if (trimmed == "true" || trimmed == "false") {
        return trimmed == "true";
    }
    
    // Check if integer
    try {
        size_t pos;
        int int_val = std::stoi(trimmed, &pos);
        if (pos == trimmed.size()) return int_val;
    } catch (...) {}
    
    // Check if float
    try {
        size_t pos;
        float float_val = std::stof(trimmed, &pos);
        if (pos == trimmed.size()) return float_val;
    } catch (...) {}
    
    // Check if boolean-like
    if (trimmed == "1" || trimmed == "0") {
        return trimmed == "1";
    }
    
    return std::nullopt;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) const {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

std::optional<ConfigValue> Config::parseJSONValue(const std::string& content, size_t& pos) const {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) return std::nullopt;
    
    switch (content[pos]) {
        case '"': {
            // String
            size_t endQuote = content.find('"', pos + 1);
            if (endQuote == std::string::npos) return std::nullopt;
            std::string str = content.substr(pos + 1, endQuote - pos - 1);
            pos = endQuote + 1;
            return str;
        }
        case 't': case 'f': {
            // Boolean
            if (content.substr(pos, 4) == "true") {
                pos += 4;
                return true;
            } else if (content.substr(pos, 5) == "false") {
                pos += 5;
                return false;
            }
            break;
        }
        case '-': case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9': {
            // Number (simplified)
            size_t endNum = pos;
            while (endNum < content.size() && (std::isdigit(content[endNum]) || content[endNum] == '.')) {
                endNum++;
            }
            std::string numStr = content.substr(pos, endNum - pos);
            try {
                // Try integer first
                size_t pos_int;
                int int_val = std::stoi(numStr, &pos_int);
                if (pos_int == numStr.size()) {
                    pos = endNum;
                    return int_val;
                }
                
                // Try float
                size_t pos_float;
                float float_val = std::stof(numStr, &pos_float);
                if (pos_float == numStr.size()) {
                    pos = endNum;
                    return float_val;
                }
            } catch (...) {}
            break;
        }
        case '[': {
            // Array (simplified)
            pos++;
            std::vector<ConfigValue> array;
            while (pos < content.size() && content[pos] != ']') {
                skipWhitespace(content, pos);
                auto elem = parseJSONValue(content, pos);
                if (elem.has_value()) {
                    array.push_back(elem.value());
                }
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            }
            if (pos < content.size() && content[pos] == ']') {
                pos++;
            }
            return array;
        }
        default:
            break;
    }
    return std::nullopt;
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Detect file format by extension
    std::string ext = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = saveToJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = saveToYAML(file);
    } else {
        // Default to INI format for backward compatibility
        success = saveToINI(file);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to write config file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

bool Config::saveToINI(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << formatValue(entry.value) << "\n";
    }
    return true;
}

bool Config::saveToJSON(std::ofstream& file) const {
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": " << formatJSONValue(entry.value);
    }
    file << "\n}\n";
    return true;
}

bool Config::saveToYAML(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": " << formatYAMLValue(entry.value) << "\n";
    }
    return true;
}

std::string Config::formatValue(const ConfigValue& value) const {
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

std::string Config::formatJSONValue(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return std::to_string(arg);
        }
    }, value);
}

std::string Config::formatYAMLValue(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Escape YAML special characters
            std::string result = arg;
            // Simple escaping for YAML
            size_t pos = 0;
            while ((pos = result.find('"', pos)) != std::string::npos) {
                result.replace(pos, 1, "\\\"");
                pos += 2;
            }
            return "\"" + result + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return std::to_string(arg);
        }
    }, value);
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key format
    if (key.empty() || key.find('=') != std::string::npos) {
        NLM_LOG_ERROR("Invalid configuration key: '" + key + "'");
        return;
    }
    
    // Validate value based on key
    validateKeyValue(key, value);
    
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
    NLM_LOG_INFO("Removed configuration key: '" + key + "'");
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
    size_t old_size = pImpl->entries.size();
    pImpl->entries.clear();
    if (old_size > 0) {
        NLM_LOG_INFO("Cleared " + std::to_string(old_size) + " configuration entries");
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Group by source
    std::unordered_map<ConfigSource, std::vector<ConfigEntry>> bySource;
    for (const auto& entry : pImpl->entries) {
        bySource[entry.source].push_back(entry);
    }
    
    // Display by source
    oss << "\nConfiguration by Source:\n";
    for (const auto& pair : bySource) {
        oss << "  Source " << static_cast<int>(pair.first) << " (" << pair.second.size() << " entries):\n";
        for (const auto& entry : pair.second) {
            oss << "    " << entry.key << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << " (" << entry.description << ")\n";
        }
    }
    
    // Validate configuration
    oss << "\nValidation:\n";
    validateConfiguration();
    
    return oss.str();
}

void Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Validate specific keys have appropriate types
    if (key == "neuron_count") {
        auto* int_val = std::get_if<int>(&value);
        if (int_val && (*int_val <= 0 || *int_val > 1000000)) {
            NLM_LOG_WARNING("Configuration key 'neuron_count' should be in range [1, 1000000]. Got: " + std::to_string(*int_val));
        }
    } else if (key == "connection_probability") {
        auto* float_val = std::get_if<float>(&value);
        if (float_val && (*float_val < 0.0f || *float_val > 1.0f)) {
            NLM_LOG_ERROR("Configuration key 'connection_probability' must be in range [0.0, 1.0]. Got: " + std::to_string(*float_val));
        }
    } else if (key == "simulation_timestep") {
        auto* double_val = std::get_if<double>(&value);
        if (double_val && (*double_val <= 0.0)) {
            NLM_LOG_ERROR("Configuration key 'simulation_timestep' must be positive. Got: " + std::to_string(*double_val));
        }
    }
}

void Config::validateConfiguration() const {
    int errors = 0;
    int warnings = 0;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key == "neuron_count") {
            auto* int_val = std::get_if<int>(&entry.value);
            if (int_val) {
                if (*int_val <= 0) {
                    errors++;
                    NLM_LOG_ERROR("neuron_count must be positive. Got: " + std::to_string(*int_val));
                } else if (*int_val > 1000000) {
                    warnings++;
                    NLM_LOG_WARNING("neuron_count is very large (>1,000,000). Performance may be affected.");
                }
            }
        } else if (entry.key == "connection_probability") {
            auto* float_val = std::get_if<float>(&entry.value);
            if (float_val) {
                if (*float_val < 0.0f || *float_val > 1.0f) {
                    errors++;
                    NLM_LOG_ERROR("connection_probability must be in range [0.0, 1.0]. Got: " + std::to_string(*float_val));
                }
            }
        }
    }
    
    if (errors > 0) {
        NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errors) + " error(s)");
    } else if (warnings > 0) {
        NLM_LOG_WARNING("Configuration validation completed with " + std::to_string(warnings) + " warning(s)");
    } else {
        NLM_LOG_INFO("Configuration validation passed");
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
