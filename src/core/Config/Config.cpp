#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cerrno>
#include <system_error>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::string lastError; // Store last error message for debugging
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration values
    setDefaultConfiguration();
}

Config::~Config() = default;

// Initialize with default configuration values
void Config::setDefaultConfiguration() {
    try {
        // Neural network parameters
        set("brain.neuron.count", 100, ConfigSource::Default);
        set("brain.region.count", 5, ConfigSource::Default);
        set("brain.synapse.density", 0.1f, ConfigSource::Default);
        
        // Neuron parameters
        set("neuron.membrane.resting", -70.0f, ConfigSource::Default);
        set("neuron.membrane.threshold", -55.0f, ConfigSource::Default);
        set("neuron.membrane.reset", -70.0f, ConfigSource::Default);
        set("neuron.refractory.period", 5, ConfigSource::Default);
        set("neuron.leak.conductance", 10.0f, ConfigSource::Default);
        
        // Plasticity parameters
        set("plasticity.stdp.lr", 0.01f, ConfigSource::Default);
        set("plasticity.hebbian.lr", 0.005f, ConfigSource::Default);
        set("plasticity.structural.rate", 0.001f, ConfigSource::Default);
        
        // Memory system parameters
        set("memory.working.capacity", 1000, ConfigSource::Default);
        set("memory.episodic.capacity", 5000, ConfigSource::Default);
        set("memory.associative.capacity", 2000, ConfigSource::Default);
        set("memory.procedural.capacity", 1500, ConfigSource::Default);
        
        // Simulation parameters
        set("simulation.dt", 0.001, ConfigSource::Default);
        set("simulation.max.steps", 10000, ConfigSource::Default);
        set("simulation.time.limit", 1000.0, ConfigSource::Default);
        
        // Neuromodulation parameters
        set("neuromodulation.dopamine.scale", 1.0f, ConfigSource::Default);
        set("neuromodulation.curiosity.rate", 0.1f, ConfigSource::Default);
        set("neuromodulation.novelty.threshold", 0.5f, ConfigSource::Default);
        
        // Developmental parameters
        set("development.critical.period", 100, ConfigSource::Default);
        set("development.maturation.rate", 0.01f, ConfigSource::Default);
        
        // Visualization parameters
        set("visualization.enabled", true, ConfigSource::Default);
        set("visualization.frame.rate", 30, ConfigSource::Default);
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Failed to set default configuration: ") + e.what();
    }
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

void Config::clearLastError() const {
    pImpl->lastError.clear();
}

// Enhanced file loading with proper error handling
bool Config::loadFromFile(const std::string& filepath) {
    // Validate input
    if (filepath.empty()) {
        pImpl->lastError = "Empty filepath provided";
        return false;
    }
    
    // Check if file exists before attempting to open
    if (!std::filesystem::exists(filepath)) {
        pImpl->lastError = "File does not exist: " + filepath;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        pImpl->lastError = "Path is not a regular file: " + filepath;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::error_code ec = std::make_error_code(std::errc::io_error);
        pImpl->lastError = "Failed to open file \"" + filepath + "\": " + ec.message();
        return false;
    }
    
    // Get file size for validation (prevent loading extremely large files)
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    const size_t MAX_CONFIG_FILE_SIZE = 10 * 1024 * 1024; // 10 MB limit
    if (fileSize > MAX_CONFIG_FILE_SIZE) {
        pImpl->lastError = "Config file too large (" + std::to_string(fileSize) + 
                         " bytes). Maximum allowed is " + std::to_string(MAX_CONFIG_FILE_SIZE) + " bytes";
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    try {
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate line length
            if (line.length() > 1024) { // 1KB line limit
                pImpl->lastError = "Line " + std::to_string(lineNumber) + " exceeds maximum length (1024 characters)";
                return false;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key format
                if (key.empty()) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Empty key";
                    return false;
                }
                
                if (!isValidKey(key)) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Invalid key format \"" + key + "\"";
                    return false;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate value format
                if (!isValidValue(value)) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Invalid value format \"" + value + "\"";
                    return false;
                }
                
                try {
                    set(key, value, ConfigSource::File);
                } catch (const std::exception& e) {
                    pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Failed to set key \"" + 
                                     key + "\": " + std::string(e.what());
                    return false;
                }
            } else {
                pImpl->lastError = "Line " + std::to_string(lineNumber) + ": Missing '=' separator";
                return false;
            }
        }
        
        clearLastError(); // Clear any previous error messages
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Error reading config file \"" + filepath + "\" at line " + 
                         std::to_string(lineNumber) + ": " + std::string(e.what());
        return false;
    }
}

// Enhanced argument loading with proper validation
bool Config::loadFromArgs(int argc, char** argv) {
    // Validate input arguments
    if (argc <= 0) {
        pImpl->lastError = "Invalid argc value: " + std::to_string(argc);
        return false;
    }
    
    if (argv == nullptr) {
        pImpl->lastError = "nullptr argv provided";
        return false;
    }
    
    try {
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                pImpl->lastError = "nullptr argv[" + std::to_string(i) + "]";
                return false;
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key and value
                    if (key.empty()) {
                        pImpl->lastError = "Empty key in argument: \"" + arg + "\"";
                        return false;
                    }
                    
                    if (!isValidKey(key)) {
                        pImpl->lastError = "Invalid key format in argument: \"" + key + "\"";
                        return false;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    pImpl->lastError = "Missing '=' in argument: \"" + arg + "\"";
                    return false;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate key
                if (key.empty()) {
                    pImpl->lastError = "Empty key in argument: \"" + arg + "\"";
                    return false;
                }
                
                if (!isValidKey(key)) {
                    pImpl->lastError = "Invalid key format in argument: \"" + key + "\"";
                    return false;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                pImpl->lastError = "Invalid argument format: \"" + arg + "\"";
                return false;
            }
        }
        
        clearLastError();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Error processing command line arguments: " + std::string(e.what());
        return false;
    }
}

// Enhanced file saving with error handling
bool Config::saveToFile(const std::string& filepath) const {
    // Validate input
    if (filepath.empty()) {
        pImpl->lastError = "Empty filepath provided for saving";
        return false;
    }
    
    // Check if directory exists and is writable
    std::filesystem::path path(filepath);
    std::filesystem::path parentDir = path.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        pImpl->lastError = "Directory does not exist: " + parentDir.string();
        return false;
    }
    
    if (!parentDir.empty() && !std::filesystem::is_directory(parentDir)) {
        pImpl->lastError = "Path is not a directory: " + parentDir.string();
        return false;
    }
    
    // Check if we have write permissions
    if (!parentDir.empty()) {
        std::error_code ec;
        bool canWrite = std::filesystem::is_writable(parentDir, ec);
        if (ec || !canWrite) {
            pImpl->lastError = "No write permission for directory: " + parentDir.string();
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::error_code ec = std::make_error_code(std::errc::io_error);
        pImpl->lastError = "Failed to open file for writing \"" + filepath + "\": " + ec.message();
        return false;
    }
    
    // Set file to text mode and check for errors
    if (!file.good()) {
        pImpl->lastError = "File stream is not in good state for file: " + filepath;
        return false;
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            // Write comment with description
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Write key-value pair
            file << entry.key << " = ";
            
            // Handle different value types
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n";
        }
        
        // Force write to disk and check for errors
        file.flush();
        if (!file.good()) {
            pImpl->lastError = "Error flushing to file: " + filepath;
            return false;
        }
        
        clearLastError();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Error writing config file \"" + filepath + "\": " + std::string(e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input
    if (key.empty()) {
        pImpl->lastError = "Empty key provided for get()";
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        clearLastError(); // Clear any previous error messages
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        pImpl->lastError = "Type mismatch for key \"" + key + "\": expected " + 
                         getTypeName<T>() + " but got " + getVariantTypeName(it->value);
        return std::nullopt;
    } catch (const std::exception& e) {
        pImpl->lastError = "Error retrieving value for key \"" + key + "\": " + std::string(e.what());
        return std::nullopt;
    }
}

// Get value or default with validation
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set value with type validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }
    
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid key format: " + key);
    }
    
    setInternal(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid key format: " + key);
    }
    
    if (!isValidStringValue(value)) {
        throw std::invalid_argument("Invalid string value format: " + value);
    }
    
    setInternal(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setInternal(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setInternal(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setInternal(key, ConfigValue(value), source);
}

// Internal set method with validation
void Config::setInternal(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate value type
    if (!isValidConfigValue(value)) {
        throw std::invalid_argument("Invalid config value for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    clearLastError();
}

// Enhanced has method with error handling
bool Config::has(const std::string& key) const {
    if (key.empty()) {
        pImpl->lastError = "Empty key provided for has()";
        return false;
    }
    
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!result) {
        clearLastError();
    }
    
    return result;
}

// Enhanced remove method with error handling
void Config::remove(const std::string& key) {
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty for removal");
    }
    
    size_t beforeCount = pImpl->entries.size();
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (pImpl->entries.size() == beforeCount) {
        pImpl->lastError = "Key not found for removal: " + key;
        throw std::runtime_error("Key not found: " + key);
    }
    
    clearLastError();
}

// Enhanced getKeys method
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    try {
        for (const auto& entry : pImpl->entries) {
            if (!entry.key.empty()) {
                keys.push_back(entry.key);
            }
        }
        
        clearLastError();
        return keys;
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Error retrieving keys: " + std::string(e.what());
        throw;
    }
}

// Enhanced clear method
void Config::clear() {
    size_t beforeCount = pImpl->entries.size();
    
    try {
        pImpl->entries.clear();
        clearLastError();
    } catch (const std::exception& e) {
        pImpl->lastError = "Error clearing configuration: " + std::string(e.what());
        throw;
    }
}

// Enhanced summary method with better formatting
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    try {
        for (const auto& entry : pImpl->entries) {
            oss << "  Key: \"" << entry.key << "\"\n";
            oss << "    Value Type: " << getVariantTypeName(entry.value) << "\n";
            oss << "    Value: ";
            
            // Format value based on type for better readability
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "["; 
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "["; 
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << std::fixed << std::setprecision(2) << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "["; 
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << "\"" << arg[i] << "\"";
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, entry.value);
            
            oss << "\n";
            oss << "    Source: " << static_cast<int>(entry.source) << "\n";
            if (!entry.description.empty()) {
                oss << "    Description: " << entry.description << "\n";
            }
            oss << "\n";
        }
        
        return oss.str();
        
    } catch (const std::exception& e) {
        pImpl->lastError = "Error generating configuration summary: " + std::string(e.what());
        return "Error generating configuration summary: " + std::string(e.what());
    }
}

// Validation helper functions
bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 256) {
        return false;
    }
    
    // Key should contain only alphanumeric characters, underscores, and dots
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            return false;
        }
    }
    
    // Cannot start with a digit
    if (std::isdigit(key[0])) {
        return false;
    }
    
    return true;
}

bool Config::isValidStringValue(const std::string& value) {
    if (value.length() > 1024) { // 1KB limit for string values
        return false;
    }
    
    // Check for null bytes
    if (value.find('\0') != std::string::npos) {
        return false;
    }
    
    return true;
}

bool Config::isValidConfigValue(const ConfigValue& value) {
    // Check if the variant contains a valid type
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                     std::is_same_v<T, double> || std::is_same_v<T, bool> ||
                     std::is_same_v<T, std::string>) {
            return true;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            // Validate vector elements
            for (const auto& element : arg) {
                if constexpr (std::is_same_v<T, std::vector<int>>) {
                    if (element < std::numeric_limits<int>::min() || 
                        element > std::numeric_limits<int>::max()) {
                        return false;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    if (std::isnan(element) || std::isinf(element)) {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }, value);
}

std::string Config::getTypeName() const {
    return "Unknown";
}

template<typename T>
std::string Config::getTypeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else return "Unknown";
}

std::string Config::getVariantTypeName(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "Unknown";
    }, value);
}

} // namespace nlm
