#include "Config.hpp"
#include "core/Logger/Logger.hpp"
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
    // Improved file format support (key=value, JSON, YAML)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath + ". Using default values.");
        return false;
    }
    
    // Check if file is readable and has content
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        NLM_LOG_WARNING("Configuration file is empty: " + filepath + ". Using default values.");
        file.clear();
        file.seekg(0);
        return false;
    }
    file.clear();
    file.seekg(0);
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Try to detect file format
    if (firstLine.find('{') != std::string::npos) {
        // JSON format
        if (loadFromJSON(filepath)) {
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse JSON configuration file: " + filepath + ". Using defaults.");
            return false;
        }
    } else if (firstLine.find("[") != std::string::npos) {
        // YAML format
        if (loadFromYAML(filepath)) {
            NLM_LOG_INFO("Successfully loaded configuration from YAML file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse YAML configuration file: " + filepath + ". Using defaults.");
            return false;
        }
    } else {
        // Simple key=value format
        std::string line;
        int parsedLines = 0;
        while (std::getline(file, line)) {
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
                
                try {
                    set(key, value, ConfigSource::File);
                    parsedLines++;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse config line in " + filepath + ": " + line + ". Error: " + e.what());
                }
            }
        }
        
        if (parsedLines > 0) {
            NLM_LOG_INFO("Loaded " + std::to_string(parsedLines) + " configuration parameters from: " + filepath);
            return true;
        } else if (!firstLine.empty()) {
            NLM_LOG_ERROR("Failed to parse configuration file: " + filepath + ". Invalid format. Using defaults.");
            return false;
        } else {
            NLM_LOG_WARNING("Configuration file is empty or contains no valid parameters: " + filepath + ". Using defaults.");
            return false;
        }
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
    // Validate key before attempting to set
    if (key.empty()) {
        NLM_LOG_ERROR("Attempt to set configuration with empty key. Operation cancelled.");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    try {
        if (it != pImpl->entries.end()) {
            // Update existing entry
            it->value = value;
            it->source = source;
            NLM_LOG_INFO("Updated configuration key \"" + key + "\" from source " + getSourceString(source));
        } else {
            // Add new entry
            pImpl->entries.emplace_back(key, value, source);
            NLM_LOG_INFO("Added configuration key \"" + key + "\" from source " + getSourceString(source));
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration key \"" + key + "\": " + e.what());
    }
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

// Helper function to get source string representation
std::string Config::getSourceString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

// Load configuration from JSON file
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parsing (assumes key-value pairs)
    size_t pos = 0;
    while ((pos = content.find("\"")) != std::string::npos) {
        size_t keyEnd = content.find("\"", pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) pos++;
        if (pos >= content.size() || content[pos] != ':') continue;
        pos++;
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) pos++;
        if (pos >= content.size()) break;
        
        // Find value (could be quoted string, number, or boolean)
        char quoteChar = '\0';
        if (content[pos] == '"' || content[pos] == '\'') {
            quoteChar = content[pos];
            pos++;
        }
        
        size_t valueEnd = content.find(quoteChar == '\0' ? ' ' : quoteChar, pos);
        if (valueEnd == std::string::npos) break;
        
        std::string value = content.substr(pos, valueEnd - pos);
        pos = valueEnd + 1;
        
        // Handle basic types
        if (value == "true" || value == "false") {
            set(key, value == "true", ConfigSource::File);
        } else if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
            try {
                double numValue = std::stod(value);
                set(key, numValue, ConfigSource::File);
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        } else {
            try {
                int intValue = std::stoi(value);
                set(key, intValue, ConfigSource::File);
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

// Load configuration from YAML file with error recovery
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    int parsedLines = 0;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // YAML format: key: value
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (!key.empty()) {
                try {
                    set(key, value, ConfigSource::File);
                    parsedLines++;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse YAML line in " + filepath + ": " + line + ". Error: " + e.what());
                }
            } else {
                NLM_LOG_WARNING("Empty key in YAML line in " + filepath + ": " + line);
            }
        } else {
            NLM_LOG_WARNING("Invalid YAML format in " + filepath + ": " + line);
        }
    }
    
    if (parsedLines > 0) {
        NLM_LOG_INFO("Successfully loaded " + std::to_string(parsedLines) + " parameters from YAML file: " + filepath);
        return true;
    } else if (parsedLines == 0 && !file.eof()) {
        NLM_LOG_ERROR("Failed to parse YAML configuration file: " + filepath + ". Invalid format.");
        return false;
    } else {
        NLM_LOG_WARNING("Empty YAML configuration file: " + filepath);
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv, bool showHelp) {
    if (showHelp) {
        std::cout << getCommandLineHelp();
        return true;
    }
    
    if (argc < 2) {
        std::cout << "No configuration arguments provided. Use --help for usage.\n";
        return true; // Allow default behavior
    }
    
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help and -h
        if (arg == "--help" || arg == "-h") {
            std::cout << getCommandLineHelp();
            return true;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    std::cerr << "Error setting configuration key \"" << key << "\" with value \"" << value << "\": " << e.what() << "\n";
                    std::cerr << "Use --help for proper usage.\n";
                    hasErrors = true;
                }
            } else {
                std::cerr << "Invalid argument format: " << arg << "\n";
                std::cerr << "Use --help for proper usage.\n";
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                std::cerr << "Error setting configuration key \"" << key << "\" with value \"" << value << "\": " << e.what() << "\n";
                std::cerr << "Use --help for proper usage.\n";
                hasErrors = true;
            }
        }
        else {
            // Validate standalone argument format
            std::cerr << "Invalid argument format: " << arg << "\n";
            std::cerr << "Use --help for proper usage.\n";
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        return false;
    }
    
    return true;
}

// Get command line help
std::string Config::getCommandLineHelp() {
    std::ostringstream oss;
    oss << "Usage: program [OPTIONS]\n\n";
    oss << "Configuration Options:\n";
    oss << "  --key=value      Set configuration key to value\n";
    oss << "  -k value         Set configuration key k to value\n";
    oss << "\n";
    oss << "Common Keys:\n";
    oss << "  --random_seed=N     Random seed for reproducible experiments\n";
    oss << "  --simulation_timestep=N  Simulation timestep in seconds\n";
    oss << "  --neuron_count=N    Number of neurons in the brain\n";
    oss << "  --log_level=LEVEL   Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)\n";
    oss << "  --help, -h        Show this help message\n";
    oss << "\n";
    oss << "File Options:\n";
    oss << "  --config=FILE     Load configuration from file (JSON, YAML, or key=value format)\n";
    oss << "\n";
    return oss.str();
}

// Initialize validation framework with configuration
bool Config::initializeValidationFramework(const std::string& configFile) {
    // This function would typically be called from outside the Config class
    // For now, we'll just log a message
    NLM_LOG_INFO("Initializing validation framework with config file: " + configFile);
    return true;
}

// Validate configuration using schema
std::vector<ValidationResultDetail> Config::validateWithSchema(const ConfigSchema& schema) const {
    // This function would validate the configuration using the provided schema
    // For now, we'll just return an empty vector
    return std::vector<ValidationResultDetail>{};
}

// Validate configuration using framework
std::vector<ValidationResultDetail> Config::validateWithFramework(ValidationFramework& framework) const {
    // This function would validate the configuration using the provided framework
    // For now, we'll just return an empty vector
    return std::vector<ValidationResultDetail>{};
}

// Get validation results
const std::vector<ValidationResultDetail>& Config::getValidationResults() const {
    // This function would return the validation results
    // For now, we'll return a static reference to an empty vector
    static std::vector<ValidationResultDetail> emptyResults;
    return emptyResults;
}

// Save validation results to file
bool Config::saveValidationResults(const std::string& filepath, const std::string& format) const {
    // This function would save validation results to file
    // For now, we'll just log a message
    NLM_LOG_INFO("Saving validation results to file: " + filepath + " (format: " + format + ")");
    return true;
}

// Load validation configuration from file
bool Config::loadValidationConfiguration(const std::string& filepath) {
    // This function would load validation configuration from file
    // For now, we'll just log a message
    NLM_LOG_INFO("Loading validation configuration from file: " + filepath);
    return true;
}

// Get validation framework statistics
std::string Config::getValidationStatistics() const {
    // This function would return validation framework statistics
    // For now, we'll return a simple string
    return "Validation framework not implemented yet";
}

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm