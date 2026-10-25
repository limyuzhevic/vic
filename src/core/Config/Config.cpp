#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <memory>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format and load accordingly
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file to detect format
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first (if it looks like JSON)
    if (isJSONString(content)) {
        NLM_LOG_DEBUG("Loading configuration as JSON: " + filepath);
        return loadFromJSON(content);
    } else if (isYAMLString(content)) {
        NLM_LOG_DEBUG("Loading configuration as YAML: " + filepath);
        return loadFromYAML(content);
    } else {
        // Fall back to simple key=value format
        NLM_LOG_DEBUG("Loading configuration as simple key=value format: " + filepath);
        return loadFromSimpleFormat(content);
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

// Helper function to detect if string is JSON format
bool Config::isJSONString(const std::string& content) const {
    if (content.empty()) return false;
    
    // Simple JSON detection: starts with { and ends with }
    size_t firstNonWhitespace = content.find_first_not_of(" \t\r\n");
    if (firstNonWhitespace == std::string::npos) return false;
    
    return content[firstNonWhitespace] == '{' && 
           content[content.size() - 1] == '}';
}

// Helper function to detect if string is YAML format
bool Config::isYAMLString(const std::string& content) const {
    if (content.empty()) return false;
    
    // Simple YAML detection: contains : (colon) and may contain indentation
    size_t colonPos = content.find(':');
    return colonPos != std::string::npos && colonPos < content.size() - 1;
}

// Load configuration from JSON content
bool Config::loadFromJSON(const std::string& jsonContent) {
    try {
        // Simple JSON parser implementation
        // Parse key-value pairs from JSON
        std::string content = trim(jsonContent);
        if (content.empty() || content.front() != '{' || content.back() != '}') {
            return false;
        }
        
        // Remove outer braces
        content = content.substr(1, content.size() - 2);
        
        // Parse key-value pairs (simplified JSON parser)
        std::regex jsonPattern(R"(("?)\s*([^:]+)\s*:\s*("[^"]*"|[^,}\s]+))");
        auto begin = std::sregex_iterator(content.begin(), content.end(), jsonPattern);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string key = trim(match[2].str());
            std::string valueStr = trim(match[3].str());
            
            // Remove quotes from key and value if present
            if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                                   (key.front() == '\'' && key.back() == '\''))) {
                key = key.substr(1, key.size() - 2);
            }
            
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML content
bool Config::loadFromYAML(const std::string& yamlContent) {
    try {
        // Simple YAML parser implementation
        // Parse key-value pairs from YAML
        std::string content = yamlContent;
        size_t pos = 0;
        
        while (pos < content.size()) {
            // Find next key: value pair
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            // Extract key (everything before colon)
            std::string key = content.substr(pos, colonPos - pos);
            key = trim(key);
            
            // Skip colon and find value
            size_t valueStart = content.find_first_not_of(" :\t", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Extract value (everything after value start)
            std::string value = content.substr(valueStart);
            
            // Find end of this line
            size_t lineEnd = value.find('\n');
            if (lineEnd != std::string::npos) {
                value = value.substr(0, lineEnd);
            }
            
            value = trim(value);
            
            // Remove quotes from value if present
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                       (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            
            // Move to next line
            size_t nextLine = content.find('\n', valueStart);
            if (nextLine == std::string::npos) break;
            pos = nextLine + 1;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML configuration: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from simple key=value format
bool Config::loadFromSimpleFormat(const std::string& content) {
    try {
        std::string line;
        std::istringstream stream(content);
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse simple format configuration: " + std::string(e.what()));
        return false;
    }
}

// Save configuration to file in simple format (backward compatibility)
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Add header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format the value appropriately
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

// Get current timestamp for file headers
std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Enhanced loadFromArgs with better error handling and validation
bool Config::loadFromArgs(int argc, char** argv) {
    // Clear existing command line arguments to avoid duplicates
    clearCommandLineArgs();
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value
                if (validateConfigKey(key) && validateConfigValue(key, value)) {
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Invalid configuration key or value: --" + key + "=" + value);
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key and value
            if (validateConfigKey(key) && validateConfigValue(key, value)) {
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid configuration key or value: -" + key + " " + value);
                success = false;
            }
        }
        // Handle --help
        else if (arg == "--help" || arg == "-h") {
            printConfigurationHelp();
            // Don't exit, just print help
        }
        // Handle --list
        else if (arg == "--list") {
            printConfigurationList();
            // Don't exit, just print list
        }
        // Handle --validate
        else if (arg == "--validate") {
            if (!validateConfiguration()) {
                return false;
            }
        }
        else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

// Clear all command line arguments
void Config::clearCommandLineArgs() {
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ) {
        if (it->source == ConfigSource::CommandLine) {
            it = pImpl->entries.erase(it);
        } else {
            ++it;
        }
    }
}

// Validate configuration key
bool Config::validateConfigKey(const std::string& key) const {
    // Check for empty key
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    static const std::regex invalidChars("[^a-zA-Z0-9_\-\.\\]");
    if (std::regex_search(key, invalidChars)) {
        return false;
    }
    
    // Check for reserved keywords
    static const std::set<std::string> reservedKeys = {
        "help", "list", "validate", "version", "config", "configfile"
    };
    
    return reservedKeys.find(key) == reservedKeys.end();
}

// Validate configuration value based on key
bool Config::validateConfigValue(const std::string& key, const std::string& value) const {
    // Convert key to lowercase for case-insensitive comparison
    std::string lowerKey = toLower(key);
    
    // Type-specific validation
    if (lowerKey.find("seed") != std::string::npos) {
        // Check if it's a valid integer
        try {
            size_t pos;
            int64_t val = std::stoll(value, &pos);
            return pos == value.size();
        } catch (const std::exception&) {
            return false;
        }
    } else if (lowerKey.find("time") != std::string::npos) {
        // Check if it's a valid floating-point number
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            return pos == value.size() && val >= 0.0;
        } catch (const std::exception&) {
            return false;
        }
    } else if (lowerKey.find("count") != std::string::npos || 
               lowerKey.find("probability") != std::string::npos) {
        // Check if it's a valid floating-point number between 0 and 1
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            return pos == value.size() && val >= 0.0 && val <= 1.0;
        } catch (const std::exception&) {
            return false;
        }
    } else if (lowerKey.find("rate") != std::string::npos) {
        // Check if it's a valid floating-point number
        try {
            size_t pos;
            double val = std::stod(value, &pos);
            return pos == value.size() && val >= 0.0;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // For string values, just check they're not empty
    return !value.empty();
}

// Validate entire configuration
bool Config::validateConfiguration() const {
    bool isValid = true;
    
    // Check for required configuration keys
    static const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            isValid = false;
        }
    }
    
    // Check for conflicting configuration keys
    std::set<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            NLM_LOG_WARNING("Duplicate configuration key: " + entry.key);
            isValid = false;
        }
        seenKeys.insert(entry.key);
    }
    
    return isValid;
}

// Print configuration help
void Config::printConfigurationHelp() const {
    std::cout << "NLM Configuration Options:\n\n";
    
    std::cout << "  --config=<file>          Load configuration from file (JSON, YAML, or key=value format)\n";
    std::cout << "  --key=value              Set configuration value\n";
    std::cout << "  -key value               Set configuration value (alternative format)\n";
    std::cout << "  --help, -h               Show this help message\n";
    std::cout << "  --list                   List all configuration keys and values\n";
    std::cout << "  --validate               Validate current configuration\n\n";
    
    std::cout << "Common configuration options:\n";
    std::cout << "  --neuron_count=<n>       Number of neurons in the brain\n";
    std::cout << "  --region_count=<n>       Number of brain regions\n";
    std::cout << "  --simulation_timestep=<t> Simulation timestep in seconds\n";
    std::cout << "  --random_seed=<n>        Random seed for reproducible experiments\n";
    std::cout << "  --connection_probability=<p> Probability of synaptic connections\n";
    std::cout << "  --log_level=<level>      Log level (Debug, Info, Warning, Error, Critical)\n";
}

// Print current configuration list
void Config::printConfigurationList() const {
    std::cout << "Current Configuration:\n\n";
    
    for (const auto& entry : pImpl->entries) {
        std::cout << "  " << entry.key << " = ";
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                std::cout << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                std::cout << (arg ? "true" : "false");
            } else {
                std::cout << arg;
            }
        }, entry.value);
        std::cout << " (" << static_cast<int>(entry.source) << ")\n";
    }
}
