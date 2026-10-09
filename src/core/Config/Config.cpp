#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
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
    // Try to detect file format based on extension
    std::string lowerPath = toLower(filepath);
    bool isJson = lowerPath.substr(lowerPath.find_last_of('.') + 1) == "json";
    bool isYaml = lowerPath.substr(lowerPath.find_last_of('.') + 1) == "yaml" || 
                  lowerPath.substr(lowerPath.find_last_of('.') + 1) == "yml";
    
    if (isJson) {
        return loadFromJsonFile(filepath);
    } else if (isYaml) {
        return loadFromYamlFile(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parsing - look for key=value patterns in JSON
        // For now, we handle basic JSON-like structures
        // TODO: Integrate with proper JSON library (nlohmann/json)
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Parse JSON-like key:value pairs
        size_t pos = 0;
        while ((pos = content.find("\"", pos)) != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = content.find("\"", keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            pos = keyEnd + 1;
            
            // Skip whitespace and :
            while (pos < content.length() && (std::isspace(content[pos]) || content[pos] == ':')) pos++;
            if (pos >= content.length()) break;
            
            // Find value
            size_t valueEnd = findMatchingBracket(content, pos);
            if (valueEnd == std::string::npos) break;
            
            std::string value = content.substr(pos, valueEnd - pos);
            trim(value);
            
            // Remove surrounding quotes
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple YAML parsing - look for key: value pairs
        // For now, we handle basic YAML-like structures
        // TODO: Integrate with proper YAML library (yaml-cpp)
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '-') {
                continue;
            }
            
            // Parse key: value pairs (YAML style)
            size_t pos = line.find(':');
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
        NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

size_t Config::findMatchingBracket(const std::string& content, size_t start) {
    if (start >= content.length()) return std::string::npos;
    
    char bracket = content[start];
    if (bracket == '"') {
        size_t pos = start + 1;
        while (pos < content.length()) {
            if (content[pos] == '\\' && pos + 1 < content.length()) {
                pos += 2; // Skip escaped character
                continue;
            } else if (content[pos] == '"') {
                return pos;
            }
            pos++;
        }
    } else if (bracket == '[') {
        int depth = 1;
        size_t pos = start + 1;
        while (pos < content.length() && depth > 0) {
            if (content[pos] == '[') depth++;
            else if (content[pos] == ']') depth--;
            pos++;
        }
        return pos - 1;
    }
    
    // For simple values, just find the end of line or comma
    size_t end = content.find_first_of(",\n", start);
    if (end == std::string::npos) end = content.length();
    return end - 1;
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
    
    // Write header comment
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration System" << std::endl;
    file << std::endl;
    
    // Write each entry
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        
        // Format based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"" << std::endl;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]" << std::endl;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]" << std::endl;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]" << std::endl;
            } else {
                // For numeric types and bool
                file << entry.key << " = " << arg << std::endl;
            }
        }, entry.value);
        
        file << std::endl;
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

// Configuration validation
    bool Config::validate(const std::string& schemaFile) const {
        NLM_LOG_INFO("Validating configuration against schema: " + schemaFile);
        
        try {
            std::ifstream schemaFile(schemaFile);
            if (!schemaFile.is_open()) {
                NLM_LOG_ERROR("Schema file not found: " + schemaFile);
                return false;
            }
            
            // Parse schema (simplified - for now assume schema is in JSON/YAML format)
            // TODO: Implement proper schema validation using nlohmann/json or yaml-cpp
            
            bool isValid = true;
            for (const auto& entry : pImpl->entries) {
                if (!validateEntry(entry)) {
                    NLM_LOG_ERROR("Invalid configuration entry: " + entry.key);
                    isValid = false;
                }
            }
            
            if (isValid) {
                NLM_LOG_INFO("Configuration validation passed");
            } else {
                NLM_LOG_ERROR("Configuration validation failed");
            }
            
            return isValid;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during validation: " + std::string(e.what()));
            return false;
        }
    }
    
    // Validate a single configuration entry
    bool Config::validateEntry(const ConfigEntry& entry) const {
        // Check for required fields
        if (entry.key.empty()) {
            return false;
        }
        
        // Validate value based on type
        if constexpr (std::is_same_v<ConfigValue, std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>) {
            // Value validation depends on the specific type
            // For now, basic validation passes
            return true;
        }
        return false;
    }
    
    // Load from environment variables
    bool Config::loadFromEnvironment(const std::string& prefix) {
        NLM_LOG_INFO("Loading configuration from environment variables with prefix: " + prefix);
        
        bool success = true;
        
        // Get all environment variables
        const char* envVars = nullptr;
        #ifdef _WIN32
            envVars = getenv("");
        #else
            envVars = environ[0];
        #endif
        
        if (!envVars) {
            NLM_LOG_WARNING("No environment variables found");
            return true; // Not an error, just no environment variables
        }
        
        // Parse environment variables (simplified implementation)
        // TODO: Implement proper environment variable parsing
        
        return success;
    }
    
    // Export configuration to JSON format
    bool Config::exportToJson(const std::string& filepath) const {
        NLM_LOG_INFO("Exporting configuration to JSON: " + filepath);
        
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for JSON export: " + filepath);
                return false;
            }
            
            file << "{\n";
            
            for (size_t i = 0; i < pImpl->entries.size(); ++i) {
                const auto& entry = pImpl->entries[i];
                
                file << "  \"" << entry.key << "\": ";
                
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << "["; // Simplified - just print opening bracket
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) file << ", ";
                            file << arg[j];
                        }
                        file << "]";
                    } else {
                        file << arg; // For numeric types
                    }
                }, entry.value);
                
                if (i < pImpl->entries.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            
            file << "}\n";
            
            NLM_LOG_INFO("Configuration exported to JSON successfully");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during JSON export: " + std::string(e.what()));
            return false;
        }
    }
    
    // Export configuration to YAML format
    bool Config::exportToYaml(const std::string& filepath) const {
        NLM_LOG_INFO("Exporting configuration to YAML: " + filepath);
        
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for YAML export: " + filepath);
                return false;
            }
            
            file << "# NLM Configuration File\n";
            file << "# Generated by NLM Configuration System\n\n";
            
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                // Format based on value type
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << ": \"" << arg << "\"\n\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << ": " << (arg ? "true" : "false") << "\n\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << entry.key << ":\n";
                        for (const auto& item : arg) {
                            file << "  - " << item << "\n";
                        }
                        file << "\n";
                    } else {
                        file << entry.key << ": " << arg << "\n\n";
                    }
                }, entry.value);
            }
            
            NLM_LOG_INFO("Configuration exported to YAML successfully");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during YAML export: " + std::string(e.what()));
            return false;
        }
    }
    
    // Set default configuration values
    void Config::setDefaults() {
        NLM_LOG_INFO("Setting default configuration values");
        
        // Set common default values
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        
        // STDP parameters
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Memory system parameters
        set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
        set("episodic_memory_max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
        
        // Development parameters
        set("development_stage_interval", static_cast<int64_t>(1000), ConfigSource::Default);
        
        NLM_LOG_INFO("Default configuration values set");
    }
    
    // Import configuration from another config object
    bool Config::import(const Config& other) {
        NLM_LOG_INFO("Importing configuration from another Config object");
        
        try {
            // Clear current entries
            pImpl->entries.clear();
            
            // Copy entries from other config
            for (const auto& entry : other.pImpl->entries) {
                pImpl->entries.push_back(entry);
            }
            
            NLM_LOG_INFO("Configuration imported successfully (" + std::to_string(pImpl->entries.size()) + " entries)");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception during import: " + std::string(e.what()));
            return false;
        }
    }
    
    // Get configuration value with type conversion
    ConfigValue Config::getValue(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            return it->value;
        }
        
        // Return empty value if key not found
        return ConfigValue();
    }
    
    // Check if configuration has changed
    bool Config::hasChanged() const {
        for (const auto& entry : pImpl->entries) {
            if (entry.source == ConfigSource::Runtime) {
                return true;
            }
        }
        return false;
    }
