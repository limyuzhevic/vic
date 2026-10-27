#include "Config.hpp"
#include <iomanip>

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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

// JSON loading implementation
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing - looking for key-value pairs in JSON format
    std::string line;
    bool inObject = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        
        // Skip comments
        if (line.find("#") != std::string::npos) {
            size_t commentPos = line.find("#");
            line = line.substr(0, commentPos);
            line = trim(line);
            if (line.empty()) continue;
        }
        
        // Parse JSON key-value pairs
        if (line.find("{") != std::string::npos) {
            inObject = true;
            continue;
        }
        
        if (line.find("}") != std::string::npos) {
            inObject = false;
            continue;
        }
        
        if (inObject) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string valueStr = trim(line.substr(colonPos + 1));
                
                // Remove quotes from key
                if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                                       (key.front() == '\'' && key.back() == '\''))) {
                    key = key.substr(1, key.size() - 2);
                }
                
                ConfigValue value;
                if (parseJSONValue(line, value, key)) {
                    set(key, value, ConfigSource::File);
                }
            }
        }
    }
    
    return true;
}

// YAML loading implementation (simple key: value format)
bool Config::loadFromYAML(const std::string& filepath) {
    return loadFromPlainText(filepath);  // YAML and plain text use similar syntax
}

// Helper function to detect file format by extension
static ConfigFileFormat detectFormat(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".jsonc") return ConfigFileFormat::JSON;
    if (ext == ".yaml" || ext == ".yml") return ConfigFileFormat::YAML;
    return ConfigFileFormat::PlainText;
}

// Simple JSON parser implementation
bool parseJSONValue(const std::string& line, ConfigValue& value, std::string& outKey) {
    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) return false;
    
    std::string key = trim(line.substr(0, colonPos));
    std::string valueStr = trim(line.substr(colonPos + 1));
    
    // Remove quotes from key
    if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                           (key.front() == '\'' && key.back() == '\''))) {
        key = key.substr(1, key.size() - 2);
    }
    
    outKey = key;
    
    // Parse different JSON value types
    valueStr = trim(valueStr);
    if (valueStr.empty()) return false;
    
    // Check for boolean values
    if (valueStr == "true" || valueStr == "false") {
        value = parseJSONBool(valueStr);
        return true;
    }
    
    // Check for numbers
    if (valueStr.find('.') != std::string::npos || std::isdigit(valueStr[0])) {
        try {
            double num = std::stod(valueStr);
            value = num;
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check for integer (including negative)
    bool isNegative = (valueStr[0] == '-');
    bool isInteger = std::all_of(valueStr.begin(), valueStr.end(), 
                               [](char c) { return std::isdigit(c) || c == '-'; });
    if (isInteger) {
        try {
            int64_t num = std::stoll(valueStr);
            value = num;
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Default to string (remove trailing comma if present)
    if (!valueStr.empty() && valueStr.back() == ',') {
        valueStr = valueStr.substr(0, valueStr.size() - 1);
    }
    
    // Remove surrounding quotes
    if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                               (valueStr.front() == '\'' && valueStr.back() == '\''))) {
        valueStr = valueStr.substr(1, valueStr.size() - 2);
    }
    
    value = valueStr;
    return true;
}

// JSON parsing helper functions
double parseJSONNumber(const std::string& str) {
    return std::stod(str);
}

bool parseJSONBool(const std::string& str) {
    return str == "true";
}

std::string parseJSONString(const std::string& str) {
    std::string result = str;
    if (result.size() >= 2 && ((result.front() == '"' && result.back() == '"') ||
                               (result.front() == '\'' && result.back() == '\''))) {
        result = result.substr(1, result.size() - 2);
    }
    return result;
}

// Enhanced saveToFile with format detection and actual value saving
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect output format based on extension
    ConfigFileFormat format = detectFormat(filepath);
    
    switch (format) {
        case ConfigFileFormat::JSON:
            return saveAsJSON(filepath);
        case ConfigFileFormat::YAML:
            return saveAsYAML(filepath);
        case ConfigFileFormat::PlainText:
            return saveAsPlainText(filepath);
    }
    
    return false;
}

// Save as plain text (backward compatible)
bool Config::saveAsPlainText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Output actual value based on type
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

// Save as JSON format
bool Config::saveAsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        // Output actual value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without unnecessary trailing zeros
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                std::string result = oss.str();
                // Remove trailing zeros and potentially decimal point
                if (result.find('.') != std::string::npos) {
                    result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                    if (result.back() == '.') result.pop_back();
                }
                file << result;
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

// Save as YAML format
bool Config::saveAsYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << ": ";
        
        // Output actual value based on type
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

// Helper function to escape JSON special characters
static std::string escapeJSON(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

// Configuration validation
bool Config::validateConfiguration() const {
    const auto& schema = getDefaultSchema();
    
    for (const auto& entry : pImpl->entries) {
        const auto& schemaEntry = schema.find(entry.key);
        if (schemaEntry == schema.end()) {
            // Check if it's a valid type
            if (!validateValueType(entry.key, entry.value)) {
                return false;
            }
        } else {
            // Validate against schema
            const auto& [description, defaultValue] = schemaEntry->second;
            if (!validateValueType(entry.key, entry.value)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validateValueType(const std::string& key, const ConfigValue& value) const {
    // Basic type validation - check if key has a valid type based on its name
    // In a more complete implementation, this would use the schema
    
    // Check if value is a valid variant type
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ||
        std::holds_alternative<double>(value) || std::holds_alternative<bool>(value) ||
        std::holds_alternative<std::string>(value)) {
        return true;
    }
    
    return false;
}

// Default schema for validation
const std::map<std::string, std::pair<std::string, ConfigValue>> Config::getDefaultSchema() {
    return {
        {"random_seed", {"Random seed for reproducible experiments", 42}},
        {"simulation_timestep", {"Simulation timestep in seconds", 0.001}},
        {"neuron_count", {"Number of neurons in the brain", 1000}},
        {"region_count", {"Number of brain regions", 2}},
        {"connection_probability", {"Connection probability (0.0-1.0)", 0.1}},
        {"plasticity_learning_rate", {"Plasticity learning rate", 0.01}},
        {"stdp_ltp_weight", {"STDP LTP weight", 0.01}},
        {"stdp_ltd_weight", {"STDP LTD weight", 0.012}},
        {"stdp_time_constant", {"STDP time constant (ms)", 20.0}},
        {"development_synaptogenesis_rate", {"Synaptogenesis rate", 0.001}},
        {"development_pruning_rate", {"Pruning rate", 0.0001}},
        {"dopamine_baseline", {"Baseline dopamine level", 0.0}},
        {"reward_discount_factor", {"Reward discount factor", 0.99}},
        {"environment_name", {"Environment type", std::string("GridWorld")}},
        {"environment_width", {"Environment width", 10}},
        {"environment_height", {"Environment height", 10}},
        {"log_level", {"Log level", std::string("INFO")}},
        {"log_to_file", {"Log to file", false}},
        {"max_simulation_steps", {"Maximum simulation steps", 10000}},
        {"simulation_time_limit", {"Simulation time limit (seconds)", 0.0}},
        {"visualization_enabled", {"Enable visualization", false}},
        {"visualization_update_rate", {"Visualization update rate (fps)", 30.0}}
    };
}

// Improved error handling with detailed error messages
void Config::logConfigError(const std::string& message, const std::string& context) {
    // In a real implementation, this would use the logger system
    // For now, we'll just output to stderr with detailed context
    std::cerr << "\n=== Configuration Error ===" << std::endl;
    if (!context.empty()) {
        std::cerr << "Context: " << context << std::endl;
    }
    std::cerr << "Error: " << message << std::endl;
    std::cerr << "========================" << std::endl << std::endl;
}

// Enhanced error handling for file operations
bool Config::safeLoadFromFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logConfigError("Failed to open configuration file: " + filepath, "File I/O Error");
        return false;
    }
    
    // Check if file is empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        logConfigError("Configuration file is empty: " + filepath, "File Validation Error");
        file.close();
        return false;
    }
    file.seekg(0, std::ios::beg);
    
    // Try to parse based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json" || ext == ".jsonc") {
        success = loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        success = loadFromPlainText(filepath);
    }
    
    return success;
}

// Enhanced error handling for validation
std::string Config::validateAndReport(const std::string& filepath) const {
    std::string errors;
    
    // Check if configuration is empty
    if (pImpl->entries.empty()) {
        errors += "Configuration is empty.\n";
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        std::string error = validateEntry(entry);
        if (!error.empty()) {
            errors += "Key \"" + entry.key + "\": " + error + "\n";
        }
    }
    
    // Check for duplicate keys
    std::vector<std::string> keys = getKeys();
    std::sort(keys.begin(), keys.end());
    auto last = std::unique(keys.begin(), keys.end());
    if (last != keys.end()) {
        errors += "Configuration contains duplicate keys.\n";
    }
    
    return errors;
}

std::string Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation - check if key is valid
    if (entry.key.empty()) {
        return "Key is empty";
    }
    
    // Check for invalid characters in key
    for (char c : entry.key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return "Key contains invalid characters: '" + std::string(1, c) + "'";
        }
    }
    
    // Check value type consistency based on key name
    // This is a simple heuristic - real implementation would use schema
    std::string lowerKey = toLower(entry.key);
    
    if (lowerKey.find("count") != std::string::npos || 
        lowerKey.find("number") != std::string::npos) {
        // Should be integer type
        if (!std::holds_alternative<int>(entry.value) && 
            !std::holds_alternative<int64_t>(entry.value)) {
            return "Expected integer type for key ending with 'count' or 'number'";
        }
    }
    
    if (lowerKey.find("rate") != std::string::npos || 
        lowerKey.find("probability") != std::string::npos) {
        // Should be numeric type
        if (!std::holds_alternative<double>(entry.value)) {
            return "Expected numeric type for key containing 'rate' or 'probability'";
        }
    }
    
    if (lowerKey.find("enabled") != std::string::npos || 
        lowerKey.find("flag") != std::string::npos) {
        // Should be boolean type
        if (!std::holds_alternative<bool>(entry.value)) {
            return "Expected boolean type for key containing 'enabled' or 'flag'";
        }
    }
    
    return "";
}

// Enhanced save with error handling and backup
bool Config::saveToFileWithBackup(const std::string& filepath) const {
    // Create backup if file exists
    std::ifstream src(filepath);
    if (src.good()) {
        std::string backupPath = filepath + ".backup." + std::to_string(std::time(nullptr));
        std::ofstream backup(backupPath);
        if (backup.good()) {
            backup << src.rdbuf();
            src.close();
            backup.close();
            logConfigError("Created backup: " + backupPath, "File Backup");
        }
    }
    
    // Save new configuration
    bool success = saveToFile(filepath);
    
    if (success) {
        logConfigError("Configuration saved successfully to: " + filepath, "File Save");
    } else {
        logConfigError("Failed to save configuration to: " + filepath, "File Save Error");
    }
    
    return success;
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
