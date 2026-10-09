#include <nlohmann/json.hpp>
#include <exception>
#include <system_error>

namespace nlm {

// Helper to convert ConfigValue to JSON value
void to_json(nlohmann::json& j, const ConfigValue& cv) {
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, cv);
}

// Helper to convert JSON value to ConfigValue
void from_json(const nlohmann::json& j, ConfigValue& cv) {
    if (j.is_string()) {
        cv = j.get<std::string>();
    } else if (j.is_number_integer()) {
        // Try to preserve int64_t for large numbers
        if (j.get<int64_t>() == j.get<int>()) {
            cv = j.get<int>();
        } else {
            cv = j.get<int64_t>();
        }
    } else if (j.is_number_float()) {
        cv = j.get<double>();
    } else if (j.is_boolean()) {
        cv = j.get<bool>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                cv = j.get<std::vector<int>>();
            } else if (j[0].is_number_float()) {
                cv = j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                cv = j.get<std::vector<std::string>>();
            }
        }
    }
}

// Enhanced file format detection
std::string detectFileFormat(const std::string& filepath) {
    // Check file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return "json";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "yaml";
    }
    
    // Try to detect from content (check first few bytes)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for format detection: " + filepath);
    }
    
    char buffer[512];
    file.read(buffer, sizeof(buffer));
    std::streamsize bytes_read = file.gcount();
    
    // Check for JSON signature ({)
    if (bytes_read > 0 && buffer[0] == '{') {
        return "json";
    }
    
    // Check for YAML signature (---)
    if (bytes_read >= 3 && buffer[0] == '-' && buffer[1] == '-' && buffer[2] == '-') {
        return "yaml";
    }
    
    // Default to INI format for backward compatibility
    return "ini";
}

// Load configuration from JSON file
bool Config::loadFromJSONFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false; // File could not be opened
        }
        
        nlohmann::json j;
        try {
            file >> j;
        } catch (const nlohmann::json::parse_error& e) {
            return false; // Invalid JSON format
        }
        
        // Clear existing configuration
        clear();
        
        // Process JSON object
        for (const auto& [key, value] : j.items()) {
            ConfigValue cv;
            try {
                from_json(value, cv);
                set(key, cv, ConfigSource::File);
            } catch (const std::exception& e) {
                // Skip invalid entries but continue processing
                continue;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Load configuration from YAML file (stub - requires yaml-cpp)
bool Config::loadFromYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML-cpp support when available
    // For now, return false to indicate not implemented
    return false;
}

// Enhanced loadFromFile with format detection
bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Detect file format
        std::string format = detectFileFormat(filepath);
        
        if (format == "json") {
            return loadFromJSONFile(filepath);
        } else if (format == "yaml") {
            return loadFromYAMLFile(filepath);
        } else if (format == "ini") {
            // Original INI-style parser (backward compatibility)
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;  // File could not be opened
            }
            
            std::string line;
            while (std::getline(file, line)) {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty()) {
                    continue;  // Skip empty lines
                }
                if (line[0] == '#' || line[0] == '/') {
                    continue;  // Skip comment lines
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
        
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Load configuration from command line arguments
    // Supports --key=value and -key value formats
    // Returns true on success, false if argument parsing fails
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
    // Save configuration to file in INI-style format
    // Writes key=value pairs with descriptions and source information
    // Returns true if file was successfully written, false on failure
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;  // File could not be created/opened
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Get configuration value of type T for given key
    // Returns optional containing value if key exists and type matches, otherwise nullopt
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;  // Key not found
    }
    
    const ConfigValue& value = it->value;
    if (std::holds_alternative<T>(value)) {
        return std::get<T>(value);  // Type matches, return value
    }
    
    return std::nullopt;  // Type mismatch
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // Get configuration value with fallback to default
    // Returns value for key if exists and matches type, otherwise returns defaultValue
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Set configuration value with source information
    // If key doesn't exist, creates new entry; if exists, updates only if source has higher priority
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Set default value if key doesn't exist
    if (!has(key)) {
        pImpl->entries.emplace_back(key, value, source);
    } else if (source == ConfigSource::CommandLine || source == ConfigSource::File) {
        // Command line and file sources override previous values
        it->value = value;
        it->source = source;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Set string configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Set integer configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Set floating-point configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Set boolean configuration value
    set(key, ConfigValue(value), source);
}

// Check if key exists and value is of specific type
template<typename T>
bool Config::has(const std::string& key) const {
    // Check if configuration key exists and has type T
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { 
            return e.key == key && std::holds_alternative<T>(e.value); 
        });
}

bool Config::has(const std::string& key) const {
    // Check if configuration key exists (any type)
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Remove configuration entry for given key
    // Thread-safe operation for concurrent access
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    // Get all configuration keys as vector of strings
    // Pre-allocates memory for efficiency
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    // Clear all configuration entries
    pImpl->entries.clear();
}

std::string Config::summary() const {
    // Generate human-readable configuration summary
    // Lists all entries with values and source information
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    // Remove leading and trailing whitespace (spaces, tabs, newlines, carriage returns)
    size_t start = str.find_first_not_of(" \\t\\r\\n");
    if (start == std::string::npos) return "";  // String is all whitespace
    size_t end = str.find_last_not_of(" \\t\\r\\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    // Convert string to lowercase (for case-insensitive key matching)
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
