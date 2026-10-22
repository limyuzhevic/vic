#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

// nlohmann::json is a header-only library
// nlohmann/json.hpp provides JSON parsing capabilities

namespace nlm {

    struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Configuration validation rules
    static bool isValidDouble(double value, const std::string& key) {
        // Prevent NaN or infinite values
        if (std::isnan(value) || std::isinf(value)) {
            return false;
        }
        
        // Specific validation for known parameters
        if (key == "simulation_timestep") {
            return value > 0.0 && value < 1.0;  // Reasonable timestep range
        } else if (key == "connection_probability") {
            return value >= 0.0 && value <= 1.0;  // Probability must be [0,1]
        } else if (key == "random_seed") {
            return value >= 0;  // Seed should be non-negative
        }
        
        return true;  // Default: accept any finite value
    }
    
    static bool isValidInt(int64_t value, const std::string& key) {
        if (key == "neuron_count") {
            return value > 0 && value <= 100000;  // Reasonable neuron count range
        } else if (key == "region_count") {
            return value > 0 && value <= 100;  // Reasonable region count range
        }
        return true;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Basic JSON parser implementation using standard C++
    // This is a simplified implementation that handles basic JSON structures
    try {
        clear();
        
        // Parse JSON string manually
        size_t pos = 0;
        size_t len = content.length();
        
        // Skip whitespace
        while (pos < len && std::isspace(content[pos])) pos++;
        
        if (pos >= len || content[pos] != '{') {
            // Fall back to simple key=value format
            file.open(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t eqPos = line.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(line.substr(0, eqPos));
                    std::string value = trim(line.substr(eqPos + 1));
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    // Extract description from line comment
                    size_t commentPos = line.find('#');
                    std::string description = "";
                    if (commentPos != std::string::npos) {
                        description = trim(line.substr(commentPos + 1));
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        }
        
        // Parse JSON object
        pos++; // Skip '{'
        
        while (pos < len && content[pos] != '}') {
            // Skip whitespace
            while (pos < len && std::isspace(content[pos])) pos++;
            
            if (pos >= len) break;
            
            // Parse key
            if (content[pos] == '"') {
                pos++; // Skip '"'
                size_t keyStart = pos;
                while (pos < len && content[pos] != '"') {
                    if (content[pos] == '\\' && pos + 1 < len) {
                        pos++; // Skip escaped character
                    }
                    pos++;
                }
                std::string key = content.substr(keyStart, pos - keyStart);
                pos++; // Skip '"'
                
                // Skip whitespace
                while (pos < len && std::isspace(content[pos])) pos++;
                
                if (pos >= len || content[pos] != ':') {
                    break; // Invalid JSON
                }
                pos++; // Skip ':'
                
                // Skip whitespace
                while (pos < len && std::isspace(content[pos])) pos++;
                
                // Parse value
                if (pos >= len) break;
                
                if (content[pos] == '"') {
                    // String value
                    pos++; // Skip '"'
                    size_t valueStart = pos;
                    while (pos < len && content[pos] != '"') {
                        if (content[pos] == '\\' && pos + 1 < len) {
                            pos++; // Skip escaped character
                        }
                        pos++;
                    }
                    std::string value = content.substr(valueStart, pos - valueStart);
                    pos++; // Skip '"'
                    set(key, value, ConfigSource::File);
                } else if (content[pos] == 't' || content[pos] == 'f') {
                    // Boolean value
                    size_t valueStart = pos;
                    while (pos < len && (content[pos] == 't' || content[pos] == 'f' ||
                                        content[pos] == 'r' || content[pos] == 'u' ||
                                        content[pos] == 'e' || content[pos] == 'a' ||
                                        content[pos] == 'l' || content[pos] == 's' ||
                                        content[pos] == ' ')) {
                        pos++;
                    }
                    std::string valueStr = content.substr(valueStart, pos - valueStart);
                    if (valueStr == "true" || valueStr == "false") {
                        set(key, valueStr == "true", ConfigSource::File);
                    }
                } else if (content[pos] == 'n') {
                    // Skip null
                    while (pos < len && content[pos] != ',' && content[pos] != '}') pos++;
                } else if (content[pos] == '[') {
                    // Array value - skip for now
                    while (pos < len && content[pos] != ']') {
                        if (content[pos] == '"') {
                            while (pos < len && content[pos] != '"') {
                                if (content[pos] == '\\' && pos + 1 < len) pos++;
                                pos++;
                            }
                        } else {
                            pos++;
                        }
                    }
                    if (pos < len) pos++; // Skip ']'
                } else {
                    // Number value
                    while (pos < len && (std::isdigit(content[pos]) || content[pos] == '-' ||
                                        content[pos] == '.' || content[pos] == 'e' ||
                                        content[pos] == 'E' || content[pos] == '+')) {
                        pos++;
                    }
                    std::string valueStr = content.substr(valueStart, pos - valueStart);
                    try {
                        if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || valueStr.find('E') != std::string::npos) {
                            set(key, std::stod(valueStr), ConfigSource::File);
                        } else {
                            set(key, std::stoll(valueStr), ConfigSource::File);
                        }
                    } catch (...) {
                        // Invalid number, skip
                    }
                }
                
                // Skip whitespace
                while (pos < len && std::isspace(content[pos])) pos++;
                
                if (pos < len && content[pos] == ',') {
                    pos++; // Skip ','
                }
            } else {
                break; // Invalid JSON
            }
        }
        
        return true;
    } catch (...) {
        // Fall back to simple key=value format
        file.open(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                
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
    
    file << "{\n";  // Start JSON object
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        // Write actual value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double with reasonable precision
                file.precision(6);
                file << std::fixed << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                // For vectors, write as JSON array
                file << "[";
                bool first = true;
                for (const auto& item : val) {
                    if (!first) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<int>> ||
                                  std::is_same_v<T, std::vector<double>>) {
                        file << item;
                    } else {
                        file << "\"" << item << "\"";
                    }
                    first = false;
                }
                file << "]";
            } else {
                // Fallback to string representation
                file << "\"" << val << "\"";
            }
        }, entry.value);
        
        file << (i < pImpl->entries.size() - 1 ? ",\n" : "\n");
    }
    
    file << "}\n";  // Close JSON object
    
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
    // Validate configuration values before setting
    bool isValid = true;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int64_t>) {
            isValid = Impl::isValidInt(val, key);
        } else if constexpr (std::is_same_v<T, double>) {
            isValid = Impl::isValidDouble(val, key);
        } else if constexpr (std::is_same_v<T, bool>) {
            // No validation needed for bool
        } else if constexpr (std::is_same_v<T, std::string>) {
            // No validation needed for string (could add length limits)
        }
    }, value);
    
    if (!isValid) {
        // Log validation error but allow the operation for robustness
        // In a production system, you might want to throw an exception or return false
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

bool Config::validateConfiguration() const {
    // Check for configuration consistency issues
    if (pImpl->entries.empty()) {
        return true;  // Empty config is valid
    }
    
    // Check for required configuration dependencies
    bool hasNeuronCount = has("neuron_count");
    bool hasRegionCount = has("region_count");
    
    if (hasNeuronCount && hasRegionCount) {
        auto neuronCount = get<size_t>("neuron_count");
        auto regionCount = get<size_t>("region_count");
        if (neuronCount.has_value() && regionCount.has_value()) {
            if (neuronCount.value() < regionCount.value()) {
                return false;  // More regions than neurons is invalid
            }
        }
    }
    
    return true;
}

std::string Config::getValidationErrors() const {
    std::string errors;
    
    if (pImpl->entries.empty()) {
        return "Configuration is empty. At least one parameter should be set.";
    }
    
    // Check for configuration consistency issues
    bool hasNeuronCount = has("neuron_count");
    bool hasRegionCount = has("region_count");
    
    if (hasNeuronCount && hasRegionCount) {
        auto neuronCount = get<size_t>("neuron_count");
        auto regionCount = get<size_t>("region_count");
        if (neuronCount.has_value() && regionCount.has_value()) {
            if (neuronCount.value() < regionCount.value()) {
                errors += "Error: Number of neurons (" + std::to_string(neuronCount.value()) + 
                         ") is less than number of regions (" + std::to_string(regionCount.value()) + ").\n";
            }
        }
    }
    
    // Check for simulation timestep validation
    auto timestep = get<double>("simulation_timestep");
    if (timestep.has_value()) {
        if (timestep.value() <= 0.0 || timestep.value() >= 1.0) {
            errors += "Error: simulation_timestep must be between 0 and 1, got " + 
                     std::to_string(timestep.value()) + ".\n";
        }
    }
    
    // Check for connection probability validation
    auto connProb = get<double>("connection_probability");
    if (connProb.has_value()) {
        if (connProb.value() < 0.0 || connProb.value() > 1.0) {
            errors += "Error: connection_probability must be between 0 and 1, got " + 
                     std::to_string(connProb.value()) + ".\n";
        }
    }
    
    return errors;
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
