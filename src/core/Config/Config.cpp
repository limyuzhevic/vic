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
    // Try to load as JSON first, then as simple key=value format for backward compatibility
    if (loadFromJSONFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Try simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loaded configuration from simple key=value file: " + filepath);
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        if (!parseSimpleKeyValue(line, lineNumber)) {
            NLM_LOG_WARNING("Invalid configuration format in " + filepath + ": line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
    }
    
    return true;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Try to parse as JSON using a simple approach
    // In a real implementation, you would use a proper JSON parser like nlohmann/json
    // For now, we'll implement a basic JSON parser for common patterns
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Basic JSON parsing - look for key: value patterns
    // This is a simplified implementation - for production, use proper JSON parser
    size_t pos = 0;
    while (pos < content.length()) {
        // Find key
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) break;
        
        // Find key name (could be "key" or 'key')
        size_t quoteStart = content.find_last_of('"', colonPos - 1);
        if (quoteStart == std::string::npos) quoteStart = content.find_last_of(\"'\", colonPos - 1);
        
        if (quoteStart == std::string::npos) {
            pos = colonPos + 1;
            continue;
        }
        
        size_t quoteEnd = content.find_first_of('\"\'', quoteStart + 1);
        if (quoteEnd == std::string::npos) {
            pos = colonPos + 1;
            continue;
        }
        
        std::string key = content.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        key = trim(key);
        
        // Find value (skip whitespace)
        size_t valueStart = content.find_first_not_of("\t \n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
// String value
            size_t valueEnd = content.find_first_of('\"\'', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
        } else if (content[valueStart] == 't' || content[valueStart] == 'f' || content[valueStart] == 'n') {
            // boolean or null - parse entire value
            size_t valueEnd = content.find_first_of",\n\t ", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.length();
            }
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        } else {
            // Number or other value - parse until comma or end of object
            size_t valueEnd = content.find_first_of",}\]\t \n", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.length();
            }
            valueStr = content.substr(valueStart, valueEnd - valueStart);
        }
        
        // Set the value
        set(key, valueStr, ConfigSource::File);
        
        // Move position past this entry
        pos = valueEnd;
        while (pos < content.length() && (content[pos] == ',' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r' || content[pos] == ' ')) {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML parsing
    // For now, return false to indicate YAML support is not implemented
    NLM_LOG_WARNING("YAML configuration file loading not yet implemented: " + filepath);
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced command-line argument parsing with validation
    // Build a list of valid configuration keys
    std::vector<std::string> validKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate"
    };
    
    bool hasHelp = false;
    bool hasVersion = false;
    
    // Check for help/version flags before processing other args
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            hasHelp = true;
        } else if (arg == "--version") {
            hasVersion = true;
        }
    }
    
    if (hasHelp) {
        printHelp();
        return true;
    }
    
    if (hasVersion) {
        printVersion();
        return true;
    }
    
    // Process configuration arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help/version flags
        if (arg == "--help" || arg == "-h") {
            printHelp();
            continue;
        } else if (arg == "--version") {
            printVersion();
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (!isValidKey(key)) {
                    NLM_LOG_ERROR("Unknown configuration key: " + key);
                    NLM_LOG_INFO("Use --help to see available configuration options");
                    return false;
                }
                
                // Set the value with validation
                if (!setValidated(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Failed to set configuration key: " + key);
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Invalid configuration argument format: " + arg);
                NLM_LOG_INFO("Use --help to see available configuration options");
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (!isValidKey(key)) {
                NLM_LOG_ERROR("Unknown configuration key: " + key);
                NLM_LOG_INFO("Use --help to see available configuration options");
                return false;
            }
            
            // Set the value with validation
            if (!setValidated(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Failed to set configuration key: " + key);
                return false;
            }
        } else {
            NLM_LOG_WARNING("Unknown argument: " + arg);
        }
    }
    
    return true;
}

void Config::printHelp() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM (熙然) - Neural Learning Machine Configuration Options");
    NLM_LOG_INFO("================================");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Usage:");
    NLM_LOG_INFO("  nlm [OPTIONS] [CONFIG_FILE]");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Options:");
    NLM_LOG_INFO("  --help, -h              Show this help message");
    NLM_LOG_INFO("  --version               Show version information");
    NLM_LOG_INFO("  --config=FILE           Load configuration from file");
    NLM_LOG_INFO("  --key=value             Set configuration key=value (string)");
    NLM_LOG_INFO("  -key value              Set configuration key=value (string)");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Available Configuration Keys:");
    NLM_LOG_INFO("  random_seed             Random seed (int64_t)");
    NLM_LOG_INFO("  simulation_timestep     Simulation timestep in seconds (double)");
    NLM_LOG_INFO("  neuron_count            Number of neurons (int64_t)");
    NLM_LOG_INFO("  region_count            Number of brain regions (int64_t)");
    NLM_LOG_INFO("  connection_probability  Connection probability between neurons (float)");
    NLM_LOG_INFO("  stdp_ltp_weight         STDP long-term potentiation weight (float)");
    NLM_LOG_INFO("  stdp_ltd_weight         STDP long-term depression weight (float)");
    NLM_LOG_INFO("  stdp_tau                STDP time constant (float)");
    NLM_LOG_INFO("  synaptogenesis_rate     Structural plasticity synaptogenesis rate (float)");
    NLM_LOG_INFO("  pruning_rate            Structural plasticity pruning rate (float)");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Type Conversion:");
    NLM_LOG_INFO("  String values will be automatically converted based on the key name");
    NLM_LOG_INFO("  Numeric strings will be converted to appropriate numeric types");
    NLM_LOG_INFO("  Boolean strings: 'true'/'false', '1'/'0', 'yes'/'no'");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Examples:");
    NLM_LOG_INFO("  nlm --config=myconfig.json");
    NLM_LOG_INFO("  nlm --neuron_count=1000 --simulation_timestep=0.0001");
    NLM_LOG_INFO("  nlm -random_seed 12345");
    NLM_LOG_INFO("  nlm --help");
    NLM_LOG_INFO("");
}

void Config::printVersion() const {
    NLM_LOG_INFO("NLM (熙然) - Neural Learning Machine v0.1.0");
    NLM_LOG_INFO("Phase 2: Real Neural Computation");
    NLM_LOG_INFO("An experimental artificial developmental brain.");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Copyright (c) 2026 NLM Authors");
    NLM_LOG_INFO("License: MIT");
    NLM_LOG_INFO("");
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << ": " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Convert value back to string representation
        file << entry.key << " = ";
        
        // Use the configValueToString helper
        file << configValueToString(entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
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

// Helper function implementations

bool Config::parseSimpleKeyValue(const std::string& line, size_t lineNumber) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    set(key, value, ConfigSource::File);
    return true;
}

std::string Config::escapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2); // Worst case: all chars escaped
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\/': result += "\\/"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control character
                    result += "\\u";
                    // Simple unicode escape for control chars
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned int>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string Config::escapeYAML(const std::string& str) {
    std::string result = str;
    
    // Check if YAML special characters are present
    bool needsQuoting = false;
    for (char c : str) {
        if (c == ':' || c == '{' || c == '}' || c == '[' || c == ']' || 
            c == ',' || c == '#' || c == '|' || c == '>' || c == '-' ||
            c == '?' || c == '@' || c == '`') {
            needsQuoting = true;
            break;
        }
    }
    
    if (needsQuoting) {
        result = "\"" + str + "\"";
    }
    
    return result;
}

} // namespace nlm
