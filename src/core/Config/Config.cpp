#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <cctype>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Cache for faster lookups
    mutable std::unordered_map<std::string, size_t> keyIndexMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Helper function to detect file format
static bool isJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first few lines to detect format
    std::string line;
    std::getline(file, line);
    file.clear();
    file.seekg(0);
    
    // Skip comments and empty lines
    while (file && (line.empty() || line[0] == '#' || line[0] == '/')) {
        if (!std::getline(file, line)) break;
    }
    
    // If we can't read a line or first non-comment line doesn't start with '{', assume key=value
    if (!file || line.empty() || line[0] != '{') {
        return false;
    }
    
    return true;
}

// Parse simple key=value pairs with enhanced error handling
bool Config::parseLine(const std::string& line, std::string& key, std::string& value) {
    // Skip whitespace
    size_t start = line.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    
    // Skip comments
    if (line[start] == '/' && start + 1 < line.size() && line[start + 1] == '/') {
        return false;
    }
    
    // Find end of line
    size_t end = line.find_last_not_of(" \t\r\n");
    if (end == std::string::npos) return false;
    
    // Extract line without trailing comments
    std::string cleanLine = line.substr(start, end - start + 1);
    
    // Check for empty line
    if (cleanLine.empty()) return false;
    
    // Find separator
    size_t separatorPos = cleanLine.find('=');
    if (separatorPos == std::string::npos) {
        return false;
    }
    
    // Extract key and value
    key = trim(cleanLine.substr(0, separatorPos));
    value = trim(cleanLine.substr(separatorPos + 1));
    
    // Remove quotes from value
    if (value.size() >= 2 && 
        ((value.front() == '\"' && value.back() == '\"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

// Validate configuration key name
static bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

// Validate configuration value based on key
bool Config::isValidValue(const std::string& key, const std::string& value) {
    // Convert key to lowercase for case-insensitive comparison
    std::string lowerKey = toLower(key);
    
    // Check key-specific validation rules
    if (lowerKey == "random_seed") {
        try {
            long seed = std::stol(value);
            return seed >= 0 && seed <= 2147483647; // 32-bit signed integer range
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return validateRange(timestep, 1e-6, 1.0); // Reasonable timestep range
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "neuron_count") {
        try {
            long count = std::stol(value);
            return validateNeuronCount(static_cast<int>(count));
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "region_count") {
        try {
            long count = std::stol(value);
            return count >= 1 && count <= 1000; // Reasonable region count
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "connection_probability") {
        try {
            double prob = std::stod(value);
            return validateConnectionProbability(static_cast<float>(prob));
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "plasticity_learning_rate") {
        try {
            double rate = std::stod(value);
            return validateRange(rate, 0.0, 1.0); // Learning rate should be 0-1
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "log_level") {
        return validateLogLevel(value);
    }
    
    else if (lowerKey == "environment_width" || lowerKey == "environment_height") {
        try {
            long dim = std::stol(value);
            return dim >= 1 && dim <= 1000; // Reasonable world dimensions
        } catch (...) {
            return false;
        }
    }
    
    else if (lowerKey == "max_simulation_steps") {
        try {
            long steps = std::stol(value);
            return steps >= 0 && steps <= 1000000; // Reasonable step limit
        } catch (...) {
            return false;
        }
    }
    
    // For numeric values, try to parse
    else {
        try {
            // Try integer first
            long val = std::stol(value);
            return true;
        } catch (...) {
            try {
                // Try double
                double val = std::stod(value);
                return true;
            } catch (...) {
                // Try boolean
                return value == "true" || value == "false" || 
                       value == "1" || value == "0";
            }
        }
    }
}

// Validate double value within range
bool Config::validateRange(double value, double min, double max) {
    return value >= min && value <= max;
}

// Validate log level
bool Config::validateLogLevel(const std::string& level) {
    std::string lowerLevel = toLower(level);
    return lowerLevel == "debug" || lowerLevel == "info" || 
           lowerLevel == "warn" || lowerLevel == "error";
}

// Validate neuron count
bool Config::validateNeuronCount(int count) {
    return count >= 1 && count <= 100000; // Reasonable neuron count range
}

// Validate connection probability
bool Config::validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

// Validate simulation timestep
bool Config::validateSimulationTimestep(double timestep) {
    return timestep > 1e-6 && timestep <= 1.0; // Reasonable timestep range
}

// Enhanced set method with validation
bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        return false;
    }
    
    // For string values, additional validation
    std::string strValue;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            strValue = arg;
        }
    }, value);
    
    if (!strValue.empty() && !isValidValue(key, strValue)) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::setValidated(const std::string& key, double value, ConfigSource source) {
    if (!isValidKey(key)) {
        return false;
    }
    
    std::string strValue = std::to_string(value);
    if (!isValidValue(key, strValue)) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::setValidated(const std::string& key, int value, ConfigSource source) {
    if (!isValidKey(key)) {
        return false;
    }
    
    std::string strValue = std::to_string(value);
    if (!isValidValue(key, strValue)) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::setValidated(const std::string& key, bool value, ConfigSource source) {
    if (!isValidKey(key)) {
        return false;
    }
    
    std::string strValue = value ? "true" : "false";
    if (!isValidValue(key, strValue)) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format
    if (isJSONFile(filepath)) {
        // For now, use enhanced key=value parser that handles more formats
        // This can be expanded to full JSON support later
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with enhanced error handling
            std::string key, value;
            if (parseLine(line, key, value)) {
                // Validate key
                if (isValidKey(key)) {
                    set(key, value, ConfigSource::File);
                } else {
                    // Log warning but continue processing
                    // For now, we'll just skip invalid keys
                }
            }
        }
    } else {
        // Original key=value format parsing with validation
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with enhanced error handling
            std::string key, value;
            if (parseLine(line, key, value)) {
                // Validate key
                if (isValidKey(key)) {
                    set(key, value, ConfigSource::File);
                } else {
                    // Log warning but continue processing
                    // For now, we'll just skip invalid keys
                }
            }
        }
    }
    
    return true;
}

// Enhanced command line argument parsing
bool Config::loadFromArgs(int argc, char** argv) {
    // Parse help flags first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Help flag
        if (arg == "--help" || arg == "-h" || arg == "--h" || arg == "-help") {
            printHelp();
            return false; // Special exit for help
        }
        
        // Version flag
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false; // Special exit for version
        }
        
        // List config keys flag
        if (arg == "--config-list" || arg == "--list-config") {
            listConfigKeys();
            return false; // Special exit for list
        }
    }
    
    // Process configuration arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format (long option with explicit value)
        if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format (long option with separate value)
        else if (arg.substr(0, 2) == "--" && i + 1 < argc && 
                argv[i + 1][0] != '-' && arg.find(' ') == std::string::npos) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key (boolean flag)
        else if (arg.substr(0, 2) == "--" && (i + 1 >= argc || 
                argv[i + 1][0] == '-')) {
            std::string key = arg.substr(2);
            // For boolean flags, set to "true" or "1"
            set(key, "true", ConfigSource::CommandLine);
            i++; // Skip next argument if it's a value (handle --key true)
        }
        // Handle -key value format (short option with separate value)
        else if (arg[0] == '-' && arg.size() > 1 && 
                (arg.find(' ') == std::string::npos || i + 1 < argc)) {
            
            std::string key = arg.substr(1);
            
            // Check if next argument is a value (not another option)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Short option flag
                set(key, "true", ConfigSource::CommandLine);
            }
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

// Print help information about available configuration options
void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./nlm [options] [--config file.cfg]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration File Options:" << std::endl;
    std::cout << "  --config=file.cfg    Load configuration from file" << std::endl;
    std::cout << "  --config file.cfg    Alternative format for file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common Configuration Parameters:" << std::endl;
    std::cout << "  --random_seed=N        Random seed for reproducible experiments" << std::endl;
    std::cout << "  --simulation_timestep=N  Simulation timestep in seconds" << std::endl;
    std::cout << "  --neuron_count=N       Number of neurons in the brain" << std::endl;
    std::cout << "  --region_count=N        Number of brain regions" << std::endl;
    std::cout << "  --connection_probability=N  Connection probability (0-1)" << std::endl;
    std::cout << "  --plasticity_learning_rate=N  Plasticity learning rate" << std::endl;
    std::cout << "  --log_level=LEVEL      Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --log_to_file[=FILE]    Enable logging to file (optional filename)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Parameters:" << std::endl;
    std::cout << "  --dopamine_baseline=N    Baseline dopamine level" << std::endl;
    std::cout << "  --curiosity_rate=N        Curiosity drive strength" << std::endl;
    std::cout << "  --novelty_threshold=N      Novelty detection threshold" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Plasticity Parameters:" << std::endl;
    std::cout << "  --stdp_ltp_weight=N       STDP LTP weight" << std::endl;
    std::cout << "  --stdp_ltd_weight=N       STDP LTD weight" << std::endl;
    std::cout << "  --stdp_tau=N              STDP time constant" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Development Parameters:" << std::endl;
    std::cout << "  --synaptogenesis_rate=N   Synaptogenesis rate" << std::endl;
    std::cout << "  --pruning_rate=N          Pruning rate" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Boolean Flags:" << std::endl;
    std::cout << "  --visualization_enabled    Enable visualization" << std::endl;
    std::cout << "  --plasticity_enabled       Enable plasticity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Short Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -c, --config            Config file (same as --config)" << std::endl;
    std::cout << "  -l, --log-level LEVEL   Set log level" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --neuron_count=100 --simulation_timestep=0.01" << std::endl;
    std::cout << "  ./nlm --config=my_config.json --visualization_enabled" << std::endl;
    std::cout << "  ./nlm --help" << std::endl;
    std::cout << std::endl;
}

// Print version information
void Config::printVersion() const {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 2: Real Neural Computation)" << std::endl;
    std::cout << "Build: Phase 6 Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "This is an experimental artificial developmental brain." << std::endl;
    std::cout << "Implementing real spiking neural computation with event-driven dynamics." << std::endl;
}

// List all available configuration keys
void Config::listConfigKeys() const {
    std::cout << "=== Available Configuration Keys ===" << std::endl;
    std::cout << std::endl;
    
    auto keys = getKeys();
    if (keys.empty()) {
        std::cout << "No configuration keys found (using defaults)." << std::endl;
    } else {
        for (const auto& key : keys) {
            std::cout << "  --" << key << "=" << getOr<std::string>(key, "<value>") << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Type '--help' for detailed documentation." << std::endl;
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

// Enhanced command line argument parsing
// Supports:
// --key=value
// --key value
// --key (boolean flag)
// -k value (short form)
// Multiple values with --key1 value1 --key2 value2
// Help and validation

bool Config::loadFromArgs(int argc, char** argv) {
    // Parse help flags first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Help flag
        if (arg == "--help" || arg == "-h" || arg == "--h" || arg == "-help") {
            printHelp();
            return false; // Special exit for help
        }
        
        // Version flag
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false; // Special exit for version
        }
        
        // List config keys flag
        if (arg == "--config-list" || arg == "--list-config") {
            listConfigKeys();
            return false; // Special exit for list
        }
    }
    
    // Process configuration arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format (long option with explicit value)
        if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format (long option with separate value)
        else if (arg.substr(0, 2) == "--" && i + 1 < argc && 
                argv[i + 1][0] != '-' && arg.find(' ') == std::string::npos) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key (boolean flag)
        else if (arg.substr(0, 2) == "--" && (i + 1 >= argc || 
                argv[i + 1][0] == '-')) {
            std::string key = arg.substr(2);
            // For boolean flags, set to "true" or "1"
            set(key, "true", ConfigSource::CommandLine);
            i++; // Skip next argument if it's a value (handle --key true)
        }
        // Handle -key value format (short option with separate value)
        else if (arg[0] == '-' && arg.size() > 1 && 
                (arg.find(' ') == std::string::npos || i + 1 < argc)) {
            
            std::string key = arg.substr(1);
            
            // Check if next argument is a value (not another option)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Short option flag
                set(key, "true", ConfigSource::CommandLine);
            }
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

// Print help information about available configuration options
void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./nlm [options] [--config file.cfg]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration File Options:" << std::endl;
    std::cout << "  --config=file.cfg    Load configuration from file" << std::endl;
    std::cout << "  --config file.cfg    Alternative format for file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common Configuration Parameters:" << std::endl;
    std::cout << "  --random_seed=N        Random seed for reproducible experiments" << std::endl;
    std::cout << "  --simulation_timestep=N  Simulation timestep in seconds" << std::endl;
    std::cout << "  --neuron_count=N       Number of neurons in the brain" << std::endl;
    std::cout << "  --region_count=N        Number of brain regions" << std::endl;
    std::cout << "  --connection_probability=N  Connection probability (0-1)" << std::endl;
    std::cout << "  --plasticity_learning_rate=N  Plasticity learning rate" << std::endl;
    std::cout << "  --log_level=LEVEL      Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --log_to_file[=FILE]    Enable logging to file (optional filename)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Parameters:" << std::endl;
    std::cout << "  --dopamine_baseline=N    Baseline dopamine level" << std::endl;
    std::cout << "  --curiosity_rate=N        Curiosity drive strength" << std::endl;
    std::cout << "  --novelty_threshold=N      Novelty detection threshold" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Plasticity Parameters:" << std::endl;
    std::cout << "  --stdp_ltp_weight=N       STDP LTP weight" << std::endl;
    std::cout << "  --stdp_ltd_weight=N       STDP LTD weight" << std::endl;
    std::cout << "  --stdp_tau=N              STDP time constant" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Development Parameters:" << std::endl;
    std::cout << "  --synaptogenesis_rate=N   Synaptogenesis rate" << std::endl;
    std::cout << "  --pruning_rate=N          Pruning rate" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Boolean Flags:" << std::endl;
    std::cout << "  --visualization_enabled    Enable visualization" << std::endl;
    std::cout << "  --plasticity_enabled       Enable plasticity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Short Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -c, --config            Config file (same as --config)" << std::endl;
    std::cout << "  -l, --log-level LEVEL   Set log level" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --neuron_count=100 --simulation_timestep=0.01" << std::endl;
    std::cout << "  ./nlm --config=my_config.json --visualization_enabled" << std::endl;
    std::cout << "  ./nlm --help" << std::endl;
    std::cout << std::endl;
}

// Print version information
void Config::printVersion() const {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 2: Real Neural Computation)" << std::endl;
    std::cout << "Build: Phase 6 Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "This is an experimental artificial developmental brain." << std::endl;
    std::cout << "Implementing real spiking neural computation with event-driven dynamics." << std::endl;
}

// List all available configuration keys
void Config::listConfigKeys() const {
    std::cout << "=== Available Configuration Keys ===" << std::endl;
    std::cout << std::endl;
    
    auto keys = getKeys();
    if (keys.empty()) {
        std::cout << "No configuration keys found (using defaults)." << std::endl;
    } else {
        for (const auto& key : keys) {
            std::cout << "  --" << key << "=" << getOr<std::string>(key, "<value>") << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Type '--help' for detailed documentation." << std::endl;
}
