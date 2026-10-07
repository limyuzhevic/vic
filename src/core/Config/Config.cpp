#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine parser
    std::string ext = getFileExtension(filepath);
    if (ext == "json" || ext == "JSON") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAML(filepath);
    } else {
        // Legacy simple key=value format
        return loadFromLegacy(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON recursively
        parseJSONToConfig(jsonData, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // Log error if logging is available
        return false;
    }
#else
    // nlohmann/json not available, fallback to legacy parser
    return loadFromLegacy(filepath);
#endif
}

// Helper function to get file extension
std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    
    std::string ext = filename.substr(pos + 1);
    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Parse JSON data recursively into Config
void Config::parseJSONToConfig(const nlohmann::json& jsonData, const std::string& prefix, ConfigSource source) {
#ifdef HAVE_NLOHMANN_JSON
    if (jsonData.is_object()) {
        for (const auto& item : jsonData.items()) {
            std::string key = prefix.empty() ? item.key() : prefix + "." + item.key();
            parseJSONToConfig(item.value(), key, source);
        }
    } else if (jsonData.is_array()) {
        // Handle arrays by creating multiple entries with indices
        for (size_t i = 0; i < jsonData.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            parseJSONToConfig(jsonData[i], key, source);
        }
    } else {
        // Handle primitive values
        ConfigValue configValue = jsonValueToConfigValue(jsonData);
        set(prefix, configValue, source);
    }
#endif
}

// Convert nlohmann::json value to ConfigValue
ConfigValue Config::jsonValueToConfigValue(const nlohmann::json& jsonVal) {
#ifdef HAVE_NLOHMANN_JSON
    if (jsonVal.is_string()) {
        return jsonVal.get<std::string>();
    } else if (jsonVal.is_number_integer()) {
        return jsonVal.get<int>();
    } else if (jsonVal.is_number_unsigned()) {
        return jsonVal.get<uint64_t>();
    } else if (jsonVal.is_number_float()) {
        return jsonVal.get<double>();
    } else if (jsonVal.is_boolean()) {
        return jsonVal.get<bool>();
    } else if (jsonVal.is_array()) {
        // Handle arrays - determine element type
        if (jsonVal.empty()) {
            return std::vector<int>();
        }
        
        // Check if all elements are integers
        bool allInts = true;
        for (const auto& item : jsonVal) {
            if (!item.is_number_integer()) {
                allInts = false;
                break;
            }
        }
        if (allInts) {
            return jsonVal.get<std::vector<int>>();
        }
        
        // Check if all elements are doubles
        bool allDoubles = true;
        for (const auto& item : jsonVal) {
            if (!item.is_number_float()) {
                allDoubles = false;
                break;
            }
        }
        if (allDoubles) {
            return jsonVal.get<std::vector<double>>();
        }
        
        // Check if all elements are strings
        bool allStrings = true;
        for (const auto& item : jsonVal) {
            if (!item.is_string()) {
                allStrings = false;
                break;
            }
        }
        if (allStrings) {
            return jsonVal.get<std::vector<std::string>>();
        }
        
        // Default to int vector for heterogeneous arrays
        std::vector<int> intArray;
        intArray.reserve(jsonVal.size());
        for (const auto& item : jsonVal) {
            if (item.is_number_integer()) {
                intArray.push_back(item.get<int>());
            } else if (item.is_number_float()) {
                intArray.push_back(static_cast<int>(item.get<double>()));
            } else if (item.is_string()) {
                try {
                    intArray.push_back(std::stoi(item.get<std::string>()));
                } catch (...) {
                    intArray.push_back(0);
                }
            } else {
                intArray.push_back(0);
            }
        }
        return intArray;
    } else if (jsonVal.is_null()) {
        return std::string("");
    } else {
        return 0; // Default to int
    }
#else
    return 0; // nlohmann/json not available
#endif
}

// Legacy simple key=value format parser
bool Config::loadFromLegacy(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing entries
    
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

// Legacy save to file (simple key=value format)
bool Config::saveToLegacyFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << formatConfigValue(entry.value) << "\n";
        file << "\n";
    }
    
    return true;
}

// Save to JSON format
bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAVE_NLOHMANN_JSON
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            std::vector<std::string> keyPath = splitKeyPath(entry.key);
            nlohmann::json* current = &jsonData;
            
            // Navigate/create nested structure
            for (size_t i = 0; i < keyPath.size() - 1; ++i) {
                if (!current->contains(keyPath[i]) || !current->at(keyPath[i]).is_object()) {
                    current->at(keyPath[i]) = nlohmann::json::object();
                }
                current = &((*current)[keyPath[i]]);
            }
            
            // Set the value
            (*current)[keyPath.back()] = configValueToJSON(entry.value);
        }
        
        // Write to file with pretty formatting
        file << jsonData.dump(2) << std::endl;
        return true;
    } catch (const std::exception& e) {
        // Log error if logging is available
        return false;
    }
#else
    // nlohmann/json not available, fallback to legacy format
    return saveToLegacyFile(filepath);
#endif
}

// Convert ConfigValue to nlohmann::json
nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
#ifdef HAVE_NLOHMANN_JSON
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> || 
                           std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                           std::is_same_v<T, std::vector<double>> || 
                           std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return nlohmann::json(nullptr);
        }
    }, value);
#else
    return nlohmann::json(nullptr);
#endif
}

// Split key path by dots and array brackets
std::vector<std::string> Config::splitKeyPath(const std::string& key) {
    std::vector<std::string> result;
    std::string current;
    bool inArray = false;
    
    for (char c : key) {
        if (c == '.') {
            if (!inArray && !current.empty()) {
                result.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        } else if (c == '[') {
            inArray = true;
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
            current += c;
        } else if (c == ']') {
            inArray = false;
            current += c;
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

// Load from YAML format (placeholder - requires YAML library)
bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Implement YAML support with YAML-cpp library
    // For now, fallback to legacy parser
    return loadFromLegacy(filepath);
}

// Save to YAML format (placeholder - requires YAML library)
bool Config::saveToYAML(const std::string& filepath) const {
    // TODO: Implement YAML support with YAML-cpp library
    // For now, fallback to JSON format
    return saveToJSON(filepath);
}

// Implementation of advanced command-line parsing and help system

bool Config::loadFromArgs(int argc, char** argv) {
    // Parse command line arguments
    bool showHelp = false;
    std::string configFile = "";
    bool verbose = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config option requires a filename" << std::endl;
                return false;
            }
        } else if (arg.find("--config=") == 0) {
            // Handle --config=file format
            configFile = arg.substr(9);
        } else if (arg.substr(0, 2) == "--") {
            // Handle --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (verbose) {
                    std::cout << "Setting key: " << key << " = " << value << std::endl;
                }
                set(key, value, ConfigSource::CommandLine);
            } else {
                std::cerr << "Error: Invalid option format: " << arg << std::endl;
                return false;
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            // Handle -key value format
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                if (verbose) {
                    std::cout << "Setting key: " << key << " = " << value << std::endl;
                }
                set(key, value, ConfigSource::CommandLine);
            } else {
                std::cerr << "Error: Option " << arg << " requires a value" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            return false;
        }
    }
    
    // Show help if requested
    if (showHelp) {
        printHelp("nlm"); // Default program name
    }
    
    // Load configuration file if specified
    if (!configFile.empty()) {
        if (verbose) {
            std::cout << "Loading configuration from: " << configFile << std::endl;
        }
        if (!loadFromFile(configFile)) {
            std::cerr << "Warning: Failed to load configuration from: " << configFile << std::endl;
        }
    }
    
    return true;
}

// Static help system
void Config::printHelp(const std::string& programName) {
    std::cout << "NLM (Neural Learning Machine) - Configuration Options" << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: " << programName << " [OPTIONS] [KEY=VALUE ...]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message and exit" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -c FILE, --config=FILE  Load configuration from file" << std::endl;
    std::cout << "                        Supported formats: .json, .yaml, .yml, .cfg, .txt" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Key-Value Pairs (can be specified multiple times):" << std::endl;
    std::cout << "  --key=value             Set a configuration key" << std::endl;
    std::cout << "  -key value              Alternative short format" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    std::cout << "  " << programName << " --config=config.json" << std::endl;
    std::cout << "  " << programName << " --neuron_count=500 --simulation_timestep=0.01" << std::endl;
    std::cout << "  " << programName << " -v -c=my_config.yaml -random_seed=123" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common Configuration Keys:" << std::endl;
    std::cout << "  neuron_count              Number of neurons in the brain" << std::endl;
    std::cout << "  simulation_timestep       Simulation timestep in seconds" << std::endl;
    std::cout << "  region_count              Number of brain regions" << std::endl;
    std::cout << "  connection_probability    Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  random_seed               Random seed for reproducibility" << std::endl;
    std::cout << "  plasticity_learning_rate  Learning rate for plasticity" << std::endl;
    std::cout << "  visualization_enabled     Enable/disable visualization" << std::endl;
    std::cout << "  log_level                 Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration File Formats:" << std::endl;
    std::cout << "  JSON (.json): Modern format with nested objects and arrays" << std::endl;
    std::cout << "  YAML (.yaml/.yml): Human-readable format with rich data types" << std::endl;
    std::cout << "  Legacy (.cfg/.txt): Simple key=value format (backward compatible)" << std::endl;
    std::cout << std::endl;
}

bool Config::isValidKey(const std::string& key) {
    // Validate key format (allow alphanumeric, dots, underscores, hyphens, brackets)
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!isalnum(c) && c != '.' && c != '_' && c != '-' && c != '[' && c != ']') {
            return false;
        }
    }
    
    // Check for valid key patterns
    if (key.find("..") != std::string::npos) return false; // Double dots
    if (key[0] == '.' || key[key.size() - 1] == '.') return false; // Leading/trailing dots
    
    return true;
}

bool Config::isValidValue(const std::string& value, const ConfigValue& expectedType) {
    // Basic value validation - can be extended based on expected type
    if (value.empty()) {
        return true; // Empty values are valid for some types
    }
    
    // Check for basic format issues
    if (value.find('\n') != std::string::npos || value.find('\r') != std::string::npos) {
        return false; // Newlines not allowed in simple values
    }
    
    // Can be extended to validate specific types
    return true;
}

std::string Config::valueToString(const ConfigValue& value) {
    return formatConfigValue(value);
}

std::string Config::getDataTypeName(const ConfigValue& value) {
    std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return std::string("string");
        } else if constexpr (std::is_same_v<T, int> || 
                           std::is_same_v<T, int64_t>) {
            return std::string("integer");
        } else if constexpr (std::is_same_v<T, double>) {
            return std::string("float/double");
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::string("boolean");
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                           std::is_same_v<T, std::vector<double>> || 
                           std::is_same_v<T, std::vector<std::string>>) {
            return std::string("array");
        } else {
            return std::string("unknown");
        }
    }, value);
    return "unknown";
}
