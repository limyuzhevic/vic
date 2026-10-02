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
        return false;
    }
    
    // Try JSON parsing first (modern approach)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fallback to simple key=value format
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

bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser for common cases
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Simple JSON object detection { ... }
        if (content.find('{') == std::string::npos) {
            return false;
        }
        
        // Parse JSON using manual string parsing (for simplicity)
        size_t pos = 1; // Skip {
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size()) break;
            
            // Check for closing }
            if (content[pos] == '}') {
                break;
            }
            
            // Parse key
            size_t keyStart = pos;
            while (pos < content.size() && content[pos] != ':' && content[pos] != '"') pos++;
            if (pos >= content.size()) break;
            
            std::string key = trim(content.substr(keyStart, pos - keyStart));
            
            // Skip past :
            pos++;
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse value based on first character
            if (pos < content.size()) {
                char firstChar = content[pos];
                if (firstChar == '"') {
                    // String value
                    size_t valueEnd = content.find('"', pos + 1);
                    if (valueEnd != std::string::npos) {
                        std::string value = content.substr(pos + 1, valueEnd - pos - 1);
                        set(key, value, ConfigSource::File);
                        pos = valueEnd + 1;
                    } else {
                        break;
                    }
                } else if (firstChar == '[') {
                    // Array value
                    size_t arrayEnd = content.find(']', pos);
                    if (arrayEnd != std::string::npos) {
                        std::string arrayContent = content.substr(pos + 1, arrayEnd - pos - 1);
                        set(key, arrayContent, ConfigSource::File);
                        pos = arrayEnd + 1;
                    } else {
                        break;
                    }
                } else {
                    // Number or boolean
                    size_t valueEnd = content.find_first_of(",}", pos);
                    if (valueEnd != std::string::npos) {
                        std::string valueStr = trim(content.substr(pos, valueEnd - pos));
                        // Try to convert to number
                        try {
                            if (valueStr.find('.') != std::string::npos || 
                                valueStr.find('e') != std::string::npos ||
                                valueStr.find('E') != std::string::npos) {
                                double value = std::stod(valueStr);
                                set(key, value, ConfigSource::File);
                            } else if (valueStr == "true" || valueStr == "false") {
                                bool value = (valueStr == "true");
                                set(key, value, ConfigSource::File);
                            } else {
                                int64_t value = std::stoll(valueStr);
                                set(key, value, ConfigSource::File);
                            }
                        } catch (...) {
                            // If parsing fails, skip
                        }
                        pos = valueEnd;
                    }
                }
            }
            
            // Skip whitespace and comma
            while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == ',')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (!validateKey(key)) {
                    NLM_LOG_ERROR("Invalid configuration key format: '" + key + "'");
                    hasErrors = true;
                    continue;
                }
                
                // Set the value with source tracking
                set(key, value, ConfigSource::CommandLine);
                
                // Log successful configuration
                NLM_LOG_INFO("[CLI] Set " + key + " = " + (value.size() > 20 ? 
                    value.substr(0, 20) + "..." : value));
            }
        }
        // Handle -key value format with validation
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (!validateKey(key)) {
                NLM_LOG_ERROR("Invalid configuration key format: '" + key + "'");
                hasErrors = true;
                continue;
            }
            
            // Set the value with source tracking
            set(key, value, ConfigSource::CommandLine);
            
            // Log successful configuration
            NLM_LOG_INFO("[CLI] Set " + key + " = " + (value.size() > 20 ? 
                value.substr(0, 20) + "..." : value));
        }
        // Handle --help flag
        else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit gracefully after showing help
        }
        // Handle --validate flag
        else if (arg == "--validate") {
            if (validateAll()) {
                NLM_LOG_INFO("[CLI] Configuration validation PASSED");
            } else {
                NLM_LOG_ERROR("[CLI] Configuration validation FAILED");
                hasErrors = true;
            }
        }
        // Handle unknown arguments
        else if (arg.substr(0, 2) == "--" || arg[0] == '-') {
            NLM_LOG_ERROR("[CLI] Unknown configuration argument: '" + arg + "'");
            hasErrors = true;
        }
    }
    
    // Log summary of CLI configuration
    if (!hasErrors) {
        std::vector<std::string> cliKeys;
        for (const auto& entry : pImpl->entries) {
            if (entry.source == ConfigSource::CommandLine) {
                cliKeys.push_back(entry.key);
            }
        }
        
        if (!cliKeys.empty()) {
            NLM_LOG_INFO("[CLI] Successfully parsed " + std::to_string(cliKeys.size()) + 
                        " configuration arguments");
        }
    }
    
    return !hasErrors;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateAll() const {
    NLM_LOG_INFO("[CLI] Validating all configuration values...");
    
    bool overallValid = true;
    
    // Validate specific known configuration keys with type checking
    struct ValidationRule {
        std::string key;
        std::string expectedType;
        std::function<bool(const ConfigValue&)> validator;
    };
    
    std::vector<ValidationRule> rules = {
        {"neuron_count", "int64_t", [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }},
        {"region_count", "int64_t", [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }},
        {"connection_probability", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"random_seed", "int64_t", [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }},
        {"simulation_timestep", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"stdp_ltp_weight", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"stdp_ltd_weight", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"stdp_tau", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"synaptogenesis_rate", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
        {"pruning_rate", "double", [](const ConfigValue& v) { return std::holds_alternative<double>(v); }},
    };
    
    for (const auto& rule : rules) {
        auto value = getFromVariant(rule.key);
        if (value.has_value()) {
            if (!rule.validator(value.value())) {
                NLM_LOG_ERROR("[CLI] Validation failed for key '" + rule.key + 
                            "': expected " + rule.expectedType + " but got different type");
                overallValid = false;
            }
        }
    }
    
    // Validate range constraints for numeric values
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    if (neuronCount < 1 || neuronCount > 1000000) {
        NLM_LOG_ERROR("[CLI] Validation failed for neuron_count: " + std::to_string(neuronCount) + 
                    " must be between 1 and 1,000,000");
        overallValid = false;
    }
    
    auto regionCount = getOr<int64_t>("region_count", 1);
    if (regionCount < 1 || regionCount > 100) {
        NLM_LOG_ERROR("[CLI] Validation failed for region_count: " + std::to_string(regionCount) + 
                    " must be between 1 and 100");
        overallValid = false;
    }
    
    auto connectionProb = getOr<double>("connection_probability", 0.0);
    if (connectionProb < 0.0 || connectionProb > 1.0) {
        NLM_LOG_ERROR("[CLI] Validation failed for connection_probability: " + std::to_string(connectionProb) + 
                    " must be between 0.0 and 1.0");
        overallValid = false;
    }
    
    return overallValid;
}

std::optional<ConfigValue> Config::getFromVariant(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

void Config::printHelp() const {
    NLM_LOG_INFO("=== NLM Configuration Help ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Configuration can be set via command line arguments or configuration files.")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Command line options:")
    NLM_LOG_INFO("  --key=value          Set configuration key to value")
    NLM_LOG_INFO("  -key value           Alternative syntax for setting values")
    NLM_LOG_INFO("  --help, -h           Show this help message")
    NLM_LOG_INFO("  --validate           Validate all configuration values")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Common configuration keys:")
    NLM_LOG_INFO("  neuron_count          Number of neurons (default: 1000)")
    NLM_LOG_INFO("  region_count          Number of brain regions (default: 1)")
    NLM_LOG_INFO("  connection_probability Connection probability between neurons (default: 0.1)")
    NLM_LOG_INFO("  random_seed           Random seed for reproducible simulations (default: 42)")
    NLM_LOG_INFO("  simulation_timestep   Simulation time step in seconds (default: 0.001)")
    NLM_LOG_INFO("  stdp_ltp_weight       STDP LTP weight (default: 0.01)")
    NLM_LOG_INFO("  stdp_ltd_weight       STDP LTD weight (default: 0.012)")
    NLM_LOG_INFO("  stdp_tau              STDP time constant (default: 20.0)")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Configuration files should be in key=value format, with optional comments.")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Error logging is enabled. Use --validate to check your configuration.")
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
