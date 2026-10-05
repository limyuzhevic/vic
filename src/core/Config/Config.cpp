#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Track loaded file format for debugging
    std::string loadedFileFormat;
};

// Simple JSON parser implementation (minimal, for Phase 6)
bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& result);
bool parseJSONString(const std::string& json, size_t& pos, std::string& result);
bool parseJSONNumber(const std::string& json, size_t& pos, double& result);
bool parseJSONBoolean(const std::string& json, size_t& pos, bool& result);
bool skipWhitespace(const std::string& json, size_t& pos);
bool match(const std::string& json, size_t& pos, const std::string& text);

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine file format based on extension and content
    std::string extension = getFileExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == "json" || (content.find('{') != std::string::npos && content.find('}') != std::string::npos)) {
        pImpl->loadedFileFormat = "JSON";
        success = loadFromJSON(content);
    } else if (extension == "yaml" || extension == "yml" || 
               (content.find(":") != std::string::npos && content.find("-") != std::string::npos)) {
        pImpl->loadedFileFormat = "YAML";
        success = loadFromYAML(content);
    } else {
        // Fallback to original key=value format
        pImpl->loadedFileFormat = "KEY=VALUE";
        success = loadFromSimpleFormat(content);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (format: " + pImpl->loadedFileFormat + ")");
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& json) {
    size_t pos = 0;
    if (!skipWhitespace(json, pos)) return false;
    if (!match(json, pos, "{")) return false;
    
    skipWhitespace(json, pos);
    
    while (pos < json.size()) {
        skipWhitespace(json, pos);
        
        if (match(json, pos, "}")) {
            break;
        }
        
        // Parse key
        std::string key;
        if (!parseJSONString(json, pos, key)) return false;
        
        skipWhitespace(json, pos);
        if (!match(json, pos, ":")) return false;
        
        skipWhitespace(json, pos);
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(json, pos, value)) return false;
        
        set(key, value, ConfigSource::File);
        
        skipWhitespace(json, pos);
        if (match(json, pos, ",")) {
            continue;
        } else if (match(json, pos, "}")) {
            break;
        } else {
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& yaml) {
    // Simplified YAML parser for Phase 6
    // Look for key: value pairs separated by colons
    std::regex keyValuePattern(R"(\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([^#\n]+))");
    std::smatch match;
    
    std::string line;
    std::stringstream stream(yaml);
    while (std::getline(stream, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        trim(line);
        if (line.empty()) continue;
        
        if (std::regex_search(line, match, keyValuePattern)) {
            std::string key = trim(match[1].str());
            std::string valueStr = trim(match[2].str());
            
            // Parse value based on content
            ConfigValue value;
            if (parseYAMLValue(valueStr, value)) {
                set(key, value, ConfigSource::File);
            } else {
                // Fallback to string
                set(key, valueStr, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& content) {
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
    
    // Determine best format based on existing config complexity
    std::string extension = getFileExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json" || pImpl->entries.size() > 10) {
        // Use JSON format for complex configs
        saveToJSON(file);
    } else if (extension == "yaml" || extension == "yml") {
        // Use YAML format
        saveToYAML(file);
    } else {
        // Use simple key=value format
        saveToSimpleFormat(file);
    }
    
    return true;
}

void Config::saveToJSON(std::ostream& output) const {
    output << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        output << "  \"" << entry.key << "\": ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                output << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                output << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                output << arg;
            } else {
                output << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            output << ",\n";
        } else {
            output << "\n";
        }
    }
    output << "}\n";
}

void Config::saveToYAML(std::ostream& output) const {
    output << "# NLM Configuration (YAML format)\n";
    output << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        output << entry.key << ": ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                output << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                output << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                output << arg;
            } else {
                output << arg;
            }
        }, entry.value);
        
        output << "\n";
    }
}

void Config::saveToSimpleFormat(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << getDescriptionForKey(entry.key) << "\n";
        output << entry.key << " = ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << arg << "\"";
            } else {
                output << arg;
            }
        }, entry.value);
        
        output << "\n\n";
    }
}

// Helper function implementations
std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::escapeJSONString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string Config::getDescriptionForKey(const std::string& key) {
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Probability of creating synapses between neurons"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Time step for simulation"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse elimination"},
        {"replay_interval", "Interval for memory replay"},
        {"consolidation_interval", "Interval for memory consolidation"}
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "Configuration parameter: " + key;
}

bool parseJSONString(const std::string& json, size_t& pos, std::string& result) {
    if (!match(json, pos, "\"")) return false;
    
    while (pos < json.size() && json[pos] != '\"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            // Skip escaped character
            pos += 2;
        } else {
            result += json[pos];
            pos++;
        }
    }
    
    if (pos >= json.size() || json[pos] != '\"') return false;
    pos++;
    
    return true;
}

bool parseJSONNumber(const std::string& json, size_t& pos, double& result) {
    size_t start = pos;
    bool hasDecimal = false;
    
    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
        if (json[pos] == '.') hasDecimal = true;
        pos++;
    }
    
    if (start == pos && !hasDecimal) return false;
    
    try {
        result = std::stod(json.substr(start, pos - start));
    } catch (...) {
        return false;
    }
    
    return true;
}

bool parseJSONBoolean(const std::string& json, size_t& pos, bool& result) {
    if (match(json, pos, "true")) {
        result = true;
        return true;
    } else if (match(json, pos, "false")) {
        result = false;
        return true;
    }
    return false;
}

bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& result) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) return false;
    
    char firstChar = json[pos];
    
    if (firstChar == '"') {
        std::string str;
        if (parseJSONString(json, pos, str)) {
            result = str;
            return true;
        }
    } else if (std::isdigit(firstChar) || firstChar == '-') {
        double num;
        size_t originalPos = pos;
        if (parseJSONNumber(json, pos, num)) {
            // Check if there's a decimal point in the original text
            std::string numStr = json.substr(originalPos, pos - originalPos);
            if (numStr.find('.') != std::string::npos) {
                result = num;
            } else {
                // Try to parse as integer
                try {
                    int64_t intValue = std::stoll(numStr);
                    result = intValue;
                } catch (...) {
                    result = num;
                }
            }
            return true;
        }
    } else if (firstChar == 't' || firstChar == 'f') {
        bool boolValue;
        if (parseJSONBoolean(json, pos, boolValue)) {
            result = boolValue;
            return true;
        }
    } else if (firstChar == 'n') {
        if (match(json, pos, "null")) {
            result = std::string("null");
            return true;
        }
    }
    
    return false;
}

bool parseYAMLValue(const std::string& yaml, ConfigValue& result) {
    std::string trimmed = trim(yaml);
    
    // Try to parse as boolean
    if (trimmed == "true") {
        result = true;
        return true;
    } else if (trimmed == "false") {
        result = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        if (trimmed.find('.') == std::string::npos) {
            int64_t intValue = std::stoll(trimmed);
            result = intValue;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        double doubleValue = std::stod(trimmed);
        result = doubleValue;
        return true;
    } catch (...) {}
    
    // Return as string
    result = trimmed;
    return true;
}

bool skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                json[pos] == '\r' || json[pos] == '\t')) {
        pos++;
    }
    return pos < json.size();
}

bool match(const std::string& json, size_t& pos, const std::string& text) {
    if (pos + text.size() > json.size()) return false;
    if (json.substr(pos, text.size()) != text) return false;
    pos += text.size();
    return true;
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
