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
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    // Try to detect file format by extension
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == "json") {
        success = loadFromJSON(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        success = loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(filepath);
    }
    
    return success;
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

// Load configuration from JSON format
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // For now, use a simple JSON parser approach
        // In a real implementation, you'd use a proper JSON library
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        // Simple JSON parsing - in practice use a proper JSON library
        // This is a simplified implementation
        size_t pos = 0;
        while (pos < content.size()) {
            // Find next key (simplified)
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value
            size_t valueStart = content.find(':', keyEnd);
            if (valueStart == std::string::npos) break;
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size() - 1;
            }
            
            std::string valueStr = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            valueStr = trim(valueStr);
            
            // Remove surrounding quotes
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on type detection
            set(key, stringToValue(valueStr), ConfigSource::File);
            
            pos = valueEnd + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML format
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // For now, treat YAML as similar to JSON (simplified)
        // In a real implementation, you'd use a proper YAML library like yaml-cpp
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        
        // Simple YAML parsing - in practice use a proper YAML library
        size_t pos = 0;
        while (pos < content.size()) {
            // Find key (YAML can be simpler than JSON)
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            // Find key (everything before colon)
            size_t keyEnd = content.rfind('\n', colonPos);
            if (keyEnd == std::string::npos) keyEnd = colonPos;
            else keyEnd = content.rfind('\t', keyEnd);
            if (keyEnd == std::string::npos) keyEnd = colonPos;
            
            std::string key = content.substr(0, keyEnd);
            key = trim(key);
            
            // Find value (everything after colon)
            size_t valueStart = colonPos + 1;
            if (valueStart < content.size() && content[valueStart] == ' ') {
                valueStart++;
            }
            
            size_t nextLine = content.find('\n', valueStart);
            std::string valueStr = content.substr(valueStart, nextLine - valueStart);
            valueStr = trim(valueStr);
            
            // Handle YAML lists
            if (valueStr.find('[') == 0) {
                // Simple list handling - just store as string for now
                // In a real implementation, you'd properly parse YAML lists
                set(key, valueStr, ConfigSource::File);
            } else {
                // Parse value based on type detection
                set(key, stringToValue(valueStr), ConfigSource::File);
            }
            
            pos = nextLine + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from simple key=value format (legacy support)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
            
            set(key, stringToValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

// Save configuration to JSON format
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Format value based on type
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
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

// Save configuration to YAML format
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration\n";
    file << "# Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": ";
        
        // Format value based on type
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
        file << "\n";
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

// Validate configuration and return list of errors
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Check required keys
    if (!has("neuron_count")) {
        errors.push_back("Missing required key: neuron_count");
    }
    
    if (!has("region_count")) {
        errors.push_back("Missing required key: region_count");
    }
    
    // Get and validate numeric values
    auto neuronCount = get<size_t>("neuron_count");
    if (neuronCount.has_value()) {
        if (*neuronCount < 10) {
            errors.push_back("neuron_count should be at least 10");
        } else if (*neuronCount > 10000) {
            errors.push_back("neuron_count should be less than or equal to 10000");
        }
    }
    
    auto regionCount = get<size_t>("region_count");
    if (regionCount.has_value()) {
        if (*regionCount < 1) {
            errors.push_back("region_count should be at least 1");
        }
    }
    
    auto connectionProb = get<float>("connection_probability");
    if (connectionProb.has_value()) {
        if (*connectionProb < 0.0f || *connectionProb > 1.0f) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    auto timestep = get<double>("simulation_timestep");
    if (timestep.has_value()) {
        if (*timestep <= 0.0 || *timestep > 0.1) {
            errors.push_back("simulation_timestep should be between 0.0 and 0.1 seconds");
        }
    }
    
    return errors;
}

// Save configuration to JSON format
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Format value based on type
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
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

// Save configuration to YAML format
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration\n";
    file << "# Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": ";
        
        // Format value based on type
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
        file << "\n";
    }
    
    return true;
}

// File format detection
std::string Config::detectFileFormat(const std::string& filepath) {
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    if (extension == "json") {
        return "json";
    } else if (extension == "yaml" || extension == "yml") {
        return "yaml";
    } else {
        return "simple";
    }
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to check if a string is a number
bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (isdigit(str[i])) continue;
        if (str[i] == '.' && !hasDecimal) {
            hasDecimal = true;
            continue;
        }
        return false;
    }
    
    return true;
}

// Helper function to convert string to appropriate type
ConfigValue Config::stringToValue(const std::string& str) {
    if (str.empty()) {
        return std::string("");
    }
    
    // Try to parse as integer
    if (isNumber(str)) {
        try {
            if (str.find('.') != std::string::npos) {
                // Has decimal point - try double
                size_t pos;
                double val = std::stod(str, &pos);
                if (pos == str.size()) {
                    return val;
                }
            } else {
                // Try int64_t
                size_t pos;
                int64_t val = std::stoll(str, &pos);
                if (pos == str.size()) {
                    return val;
                }
            }
        } catch (...) {
            // Fall through to string
        }
    }
    
    // Try to parse as boolean
    std::string lowerStr = toLower(str);
    if (lowerStr == "true" || lowerStr == "false") {
        return lowerStr == "true";
    }
    
    // Fall back to string
    return str;
}
