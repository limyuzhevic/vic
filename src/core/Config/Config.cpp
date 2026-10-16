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
    // Enhanced file loading with comprehensive error handling
    
    // Validate input filepath
    if (filepath.empty()) {
        std::cerr << "Config Error: Empty filepath provided" << std::endl;
        return false;
    }
    
    // Check if file exists and is readable
    std::filesystem::path fsPath(filepath);
    if (!std::filesystem::exists(fsPath)) {
        std::cerr << "Config Error: File does not exist: " << filepath << std::endl;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(fsPath)) {
        std::cerr << "Config Error: Path is not a regular file: " << filepath << std::endl;
        return false;
    }
    
    if (!std::filesystem::is_readable(fsPath)) {
        std::cerr << "Config Error: File is not readable: " << filepath << std::endl;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Config Error: Failed to open file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Config Warning: Line " << lineNumber << 
                         " does not contain '=' separator: " << line << std::endl;
            hasErrors = true;
            continue;
        }
        
        // Extract and validate key
        std::string key = trim(line.substr(0, pos));
        if (key.empty()) {
            std::cerr << "Config Warning: Line " << lineNumber << 
                         " has empty key: " << line << std::endl;
            hasErrors = true;
            continue;
        }
        
        if (!isValidKey(key)) {
            std::cerr << "Config Warning: Line " << lineNumber << 
                         " contains invalid key characters: " << key << std::endl;
            hasErrors = true;
            continue;
        }
        
        // Extract and validate value
        std::string value = trim(line.substr(pos + 1));
        if (value.empty()) {
            std::cerr << "Config Warning: Line " << lineNumber << 
                         " has empty value: " << key << " = " << value << std::endl;
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Attempt to set the value with validation
        if (!setValidatedKeyValue(key, value, ConfigSource::File)) {
            std::cerr << "Config Warning: Line " << lineNumber << 
                         " contains invalid value format for key: " << key << std::endl;
            hasErrors = true;
            continue;
        }
    }
    
    if (hasErrors) {
        std::cerr << "Config Warning: Some lines had errors during parsing" << std::endl;
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced command-line argument parsing with validation
    
    if (argc <= 1) {
        // No arguments provided - this is valid
        return true;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            std::cerr << "Config Warning: Null argument pointer at position " << i << std::endl;
            continue;
        }
        
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            std::cerr << "Config Warning: Empty argument at position " << i << std::endl;
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    std::cerr << "Config Warning: Empty key in argument at position " << i << std::endl;
                    continue;
                }
                
                if (!isValidKey(key)) {
                    std::cerr << "Config Warning: Invalid key in argument at position " << i << ": " << key << std::endl;
                    continue;
                }
                
                if (!setValidatedKeyValue(key, value, ConfigSource::CommandLine)) {
                    std::cerr << "Config Warning: Invalid value in argument at position " << i << " for key: " << key << std::endl;
                    continue;
                }
            } else {
                std::cerr << "Config Warning: Argument at position " << i << 
                             " uses '--' but has no '=' separator: " << arg << std::endl;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];  // Increment i for next iteration
            
            if (key.empty()) {
                std::cerr << "Config Warning: Empty key in argument at position " << i-1 << std::endl;
                continue;
            }
            
            if (!isValidKey(key)) {
                std::cerr << "Config Warning: Invalid key in argument at position " << i-1 << ": " << key << std::endl;
                continue;
            }
            
            if (!setValidatedKeyValue(key, value, ConfigSource::CommandLine)) {
                std::cerr << "Config Warning: Invalid value in argument at position " << i-1 << " for key: " << key << std::endl;
                continue;
            }
        } else {
            std::cerr << "Config Warning: Unrecognized argument format at position " << i << ": " << arg << std::endl;
            std::cerr << "  Expected: --key=value or -key value" << std::endl;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Enhanced file saving with comprehensive error handling
    
    // Validate input filepath
    if (filepath.empty()) {
        std::cerr << "Config Error: Empty filepath provided for saving" << std::endl;
        return false;
    }
    
    // Ensure the directory exists
    std::filesystem::path fsPath(filepath);
    std::filesystem::path dirPath = fsPath.parent_path();
    
    if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
        std::cerr << "Config Error: Directory does not exist: " << dirPath << std::endl;
        return false;
    }
    
    // Check if we can write to the directory
    if (!dirPath.empty() && !std::filesystem::is_writable(dirPath)) {
        std::cerr << "Config Error: Directory is not writable: " << dirPath << std::endl;
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Config Error: Failed to open file for writing: " << filepath << std::endl;
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the value in a type-safe manner
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << "# End of configuration file" << std::endl;
    
    if (!file.good()) {
        std::cerr << "Config Error: Failed to write to file: " << filepath << std::endl;
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Enhanced value retrieval with validation and error handling
    
    if (key.empty()) {
        std::cerr << "Config Error: Empty key provided to get()" << std::endl;
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Key not found - this is not necessarily an error, depending on context
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        
        // Validate the retrieved value
        if (!isValidValue(value, it->key)) {
            std::cerr << "Config Warning: Retrieved value for key '" << key << 
                         " does not match expected type or constraints" << std::endl;
            return std::nullopt;
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        // Type mismatch - this is an error
        std::cerr << "Config Error: Type mismatch when retrieving key '" << key << 
                     " (expected " << typeid(T).name() << ")" << std::endl;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Enhanced value setting with comprehensive validation
    
    if (!isValidKey(key)) {
        std::cerr << "Config Error: Invalid key: " << key << std::endl;
        return;
    }
    
    if (!isValidValue(value, key)) {
        std::cerr << "Config Error: Invalid value for key: " << key << std::endl;
        return;
    }
    
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

bool Config::hasErrors() const {
    return !errors_.empty();
}

size_t Config::getErrorCount() const {
    return errors_.size();
}

std::string Config::getLastError() const {
    if (errors_.empty()) {
        return "";
    }
    return errors_.back();
}

std::vector<std::string> Config::getAllErrors() const {
    return errors_;
}

void Config::clearErrors() {
    errors_.clear();
    lastErrorIndex_ = 0;
}

// Validation methods
bool Config::isValidKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    
    // Check for valid key characters (alphanumeric, underscore, dash, dot, colon)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.' && c != ':') {
            return false;
        }
    }
    
    // Check for reserved key patterns
    std::string lowerKey = toLower(key);
    if (lowerKey == "default" || lowerKey == "source" || 
        lowerKey == "value" || lowerKey == "description") {
        return false;
    }
    
    return true;
}

bool Config::isValidValue(const ConfigValue& value, const std::string& key) {
    // Use variant visitor to validate each possible type
    bool result = std::visit([key](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            // Validate int range
            return arg >= -2147483648 && arg <= 2147483647;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            // Validate int64_t range
            return arg >= -9223372036854775808 && arg <= 9223372036854775807;
        } else if constexpr (std::is_same_v<T, double>) {
            // Validate double range and special values
            if (std::isnan(arg) || std::isinf(arg)) return false;
            return arg >= -1e300 && arg <= 1e300;
        } else if constexpr (std::is_same_v<T, bool>) {
            return true;  // Boolean values are always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Validate string
            return !arg.empty() && arg.length() <= 1024;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Validate int vector
            return arg.size() <= 1000 && 
                   std::all_of(arg.begin(), arg.end(), 
                              [](int v) { return v >= -1000000 && v <= 1000000; });
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Validate double vector
            return arg.size() <= 1000 &&
                   std::all_of(arg.begin(), arg.end(),
                              [](double v) { return !std::isnan(v) && !std::isinf(v); });
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Validate string vector
            return arg.size() <= 100 &&
                   std::all_of(arg.begin(), arg.end(),
                              [](const std::string& s) { return !s.empty() && s.length() <= 256; });
        } else {
            return false; // Unknown type
        }
    }, value);
    
    return result;
}

// Enhanced setters with validation
bool Config::setValidatedKeyValue(const std::string& key, const std::string& value, ConfigSource source) {
    // Convert string to appropriate type based on key patterns
    ConfigValue configValue;
    bool success = convertStringToValue(value, configValue);
    
    if (success) {
        // Validate the converted value
        if (isValidValue(configValue, key)) {
            set(key, configValue, source);
            return true;
        } else {
            std::cerr << "Config Error: Invalid value '" << value << 
                         "' for key '" << key << "'" << std::endl;
        }
    } else {
        std::cerr << "Config Error: Cannot convert value '" << value << 
                     "' to appropriate type for key '" << key << "'" << std::endl;
    }
    
    return false;
}

bool Config::convertStringToValue(const std::string& str, ConfigValue& value) {
    // Attempt to convert string to various types
    // Try bool first (true/false, yes/no, 1/0)
    if (str == "true" || str == "yes" || str == "1") {
        value = true;
        return true;
    } else if (str == "false" || str == "no" || str == "0") {
        value = false;
        return true;
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.length()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.length()) {
            value = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.length()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try string (already validated)
    value = str;
    return true;
}

// ConfigValidator implementation
namespace {
    template<typename T>
    bool validateValue(const ConfigValue& value) {
        try {
            T val = std::get<T>(value);
            return Config::isValidValue(val, "");
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
}

// Explicit template instantiations for validation
template bool ConfigValidator::validate<int>(const ConfigValue& value);
template bool ConfigValidator::validate<int64_t>(const ConfigValue& value);
template bool ConfigValidator::validate<double>(const ConfigValue& value);
template bool ConfigValidator::validate<bool>(const ConfigValue& value);
template bool ConfigValidator::validate<std::string>(const ConfigValue& value);
template bool ConfigValidator::validate<std::vector<int>>(const ConfigValue& value);
template bool ConfigValidator::validate<std::vector<double>>(const ConfigValue& value);
template bool ConfigValidator::validate<std::vector<std::string>>(const ConfigValue& value);
