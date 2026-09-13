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
        return false;
    }
    
    // Try to detect format and use appropriate parser
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    // Try JSON first (simplified parsing)
    bool success = false;
    if (tryParseJSON(content)) {
        success = true;
    }
    // Fallback to key=value format
    else if (!content.empty()) {
        success = loadFromString(content);
    }
    
    return success;
}

bool Config::tryParseJSON(const std::string& content) {
    // Simple JSON parser for common config formats
    // Look for pattern: { "key": "value", "number": 42, "bool": true }
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Skip whitespace and { 
        while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == '{' || content[pos] == '[')) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        // Find key (quoted string)
        if (content[pos] == '"') {
            size_t keyStart = pos++;
            while (pos < content.size() && content[pos] != '"') {
                pos++;
            }
            if (pos >= content.size()) break;
            std::string key = content.substr(keyStart + 1, pos - keyStart - 1);
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            if (pos >= content.size() || content[pos] != ':') break;
            pos++; // Skip colon
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse value based on first character
            ConfigValue value;
            bool valueParsed = false;
            
            if (pos < content.size() && content[pos] == '"') {
                // String value
                size_t valueStart = ++pos;
                while (pos < content.size() && content[pos] != '"') {
                    pos++;
                }
                if (pos < content.size()) {
                    std::string strValue = content.substr(valueStart, pos - valueStart);
                    set(key, strValue, ConfigSource::File);
                    valueParsed = true;
                    pos++; // Skip closing quote
                }
            }
            else if (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '-')) {
                // Number value
                size_t valueStart = pos;
                while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                    pos++;
                }
                std::string numStr = content.substr(valueStart, pos - valueStart);
                try {
                    if (numStr.find('.') != std::string::npos) {
                        float fval = std::stof(numStr);
                        set(key, fval, ConfigSource::File);
                    } else {
                        int ival = std::stoi(numStr);
                        set(key, ival, ConfigSource::File);
                    }
                    valueParsed = true;
                } catch (...) {
                    // Skip if parsing fails
                }
            }
            else if (pos < content.size() && content[pos] == 't') {
                // Boolean true
                if (content.substr(pos, 4) == "true") {
                    set(key, true, ConfigSource::File);
                    valueParsed = true;
                    pos += 4;
                }
            }
            else if (pos < content.size() && content[pos] == 'f') {
                // Boolean false
                if (content.substr(pos, 5) == "false") {
                    set(key, false, ConfigSource::File);
                    valueParsed = true;
                    pos += 5;
                }
            }
            
            if (!valueParsed) {
                // Skip to next key
                while (pos < content.size() && content[pos] != '"' && content[pos] != '}') {
                    pos++;
                }
            }
        } else {
            // Not a JSON object, skip to next character
            pos++;
        }
        
        // Skip to next key or end
        while (pos < content.size() && content[pos] != '"' && content[pos] != '}') {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromString(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool hasValidEntries = false;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            hasValidEntries = true;
        }
    }
    
    return hasValidEntries;
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

// Parameter validation methods
bool Config::validateParameter(const ConfigEntry& entry) const {
    // Basic type validation - ensure entry is well-formed
    if (entry.key.empty()) {
        return false;
    }
    
    // Validate that value is not empty for string types
    if (std::holds_alternative<std::string>(entry.value)) {
        std::string val = std::get<std::string>(entry.value);
        if (val.empty()) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateParameterRange(const ConfigEntry& entry, const std::string& key) const {
    // Range validation for numeric parameters
    bool isNegative = false;
    
    if (std::holds_alternative<int>(entry.value)) {
        int val = std::get<int>(entry.value);
        if (key == "width" || key == "height" || key == "size") {
            if (val <= 0) return false;
        }
    } else if (std::holds_alternative<double>(entry.value)) {
        double val = std::get<double>(entry.value);
        if (key == "opacity") {
            if (val < 0.0 || val > 1.0) return false;
        } else if (key == "threshold") {
            if (val < 0.0) return false;
        }
    }
    
    return true;
}

bool Config::validateParameterEnum(const ConfigEntry& entry, const std::string& key, const std::vector<std::string>& validValues) const {
    // Enum validation for string parameters
    if (!validValues.empty() && std::holds_alternative<std::string>(entry.value)) {
        std::string val = std::get<std::string>(entry.value);
        bool isValid = false;
        for (const auto& valid : validValues) {
            if (toLower(val) == toLower(valid)) {
                isValid = true;
                break;
            }
        }
        if (!isValid) return false;
    }
    
    return true;
}

// Validation report generation
std::string Config::generateValidationReport() const {
    std::ostringstream oss;
    oss << "Validation Report (" << pImpl->entries.size() << " entries):\n";
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = validateParameter(entry);
        
        if (isValid && !validateParameterRange(entry, entry.key)) {
            isValid = false;
        }
        
        if (isValid) {
            std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
            if (entry.key == "format" && !validateParameterEnum(entry, entry.key, validFormats)) {
                isValid = false;
            }
        }
        
        if (isValid) {
            validCount++;
            oss << "  ✓ " << entry.key << " = ";
        } else {
            invalidCount++;
            oss << "  ✗ " << entry.key << " = [INVALID] (source: " << static_cast<int>(entry.source) << ")\n";
            continue;
        }
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nSummary: " << validCount << " valid, " << invalidCount << " invalid entries";
    return oss.str();
}

// Set methods with description support
void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    ConfigEntry newEntry(key, value, source, description);
    
    bool isValid = true;
    
    // Parameter validation
    if (!validateParameter(newEntry)) {
        isValid = false;
    }
    
    // Range validation for specific keys
    if (isValid && !validateParameterRange(newEntry, key)) {
        isValid = false;
    }
    
    // Enum validation for specific keys
    if (isValid && key == "format") {
        std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
        if (!validateParameterEnum(newEntry, key, validFormats)) {
            isValid = false;
        }
    }
    
    if (!isValid) {
        // Log validation error
        // In production, this would log to error log
        // For now, just continue without setting invalid values
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->description = description;
        } else {
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    ConfigEntry newEntry(key, value, source, description);
    
    bool isValid = true;
    
    // Parameter validation
    if (!validateParameter(newEntry)) {
        isValid = false;
    }
    
    // Range validation for specific keys
    if (isValid && !validateParameterRange(newEntry, key)) {
        isValid = false;
    }
    
    // Enum validation for specific keys
    if (isValid && key == "format") {
        std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
        if (!validateParameterEnum(newEntry, key, validFormats)) {
            isValid = false;
        }
    }
    
    if (!isValid) {
        // Log validation error
        // In production, this would log to error log
        // For now, just continue without setting invalid values
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->description = description;
        } else {
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    ConfigEntry newEntry(key, value, source, description);
    
    bool isValid = true;
    
    // Parameter validation
    if (!validateParameter(newEntry)) {
        isValid = false;
    }
    
    // Range validation for specific keys
    if (isValid && !validateParameterRange(newEntry, key)) {
        isValid = false;
    }
    
    // Enum validation for specific keys
    if (isValid && key == "format") {
        std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
        if (!validateParameterEnum(newEntry, key, validFormats)) {
            isValid = false;
        }
    }
    
    if (!isValid) {
        // Log validation error
        // In production, this would log to error log
        // For now, just continue without setting invalid values
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->description = description;
        } else {
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    ConfigEntry newEntry(key, value, source, description);
    
    bool isValid = true;
    
    // Parameter validation
    if (!validateParameter(newEntry)) {
        isValid = false;
    }
    
    // Range validation for specific keys
    if (isValid && !validateParameterRange(newEntry, key)) {
        isValid = false;
    }
    
    // Enum validation for specific keys
    if (isValid && key == "format") {
        std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
        if (!validateParameterEnum(newEntry, key, validFormats)) {
            isValid = false;
        }
    }
    
    if (!isValid) {
        // Log validation error
        // In production, this would log to error log
        // For now, just continue without setting invalid values
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            it->description = description;
        } else {
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

// Legacy set methods (without description for backward compatibility)
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry newEntry(key, value, source);
    
    bool isValid = true;
    
    // Parameter validation
    if (!validateParameter(newEntry)) {
        isValid = false;
    }
    
    // Range validation for specific keys
    if (isValid && !validateParameterRange(newEntry, key)) {
        isValid = false;
    }
    
    // Enum validation for specific keys
    if (isValid && key == "format") {
        std::vector<std::string> validFormats = {"png", "jpg", "bmp", "tiff"};
        if (!validateParameterEnum(newEntry, key, validFormats)) {
            isValid = false;
        }
    }
    
    if (!isValid) {
        // Log validation error
        // In production, this would log to error log
        // For now, just continue without setting invalid values
    } else {
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Parameter validation methods
bool Config::validateParameter(const std::string& key, const std::string& typeName) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Parameter doesn't exist, not necessarily an error
    }
    
    // Type validation based on expected type
    if (typeName == "int") {
        try {
            std::get<int>(it->value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    } else if (typeName == "int64_t") {
        try {
            std::get<int64_t>(it->value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    } else if (typeName == "double") {
        try {
            std::get<double>(it->value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    } else if (typeName == "bool") {
        try {
            std::get<bool>(it->value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    } else if (typeName == "string") {
        try {
            std::get<std::string>(it->value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    
    return false;
}

bool Config::validateParameterRange(const std::string& key, double min, double max) const {
    auto val = get<double>(key);
    if (!val) return false;
    
    double value = val.value();
    return value >= min && value <= max;
}

bool Config::validateParameterEnum(const std::string& key, 
                                   const std::vector<std::string>& allowedValues) const {
    auto val = get<std::string>(key);
    if (!val) return false;
    
    return std::find(allowedValues.begin(), allowedValues.end(), val.value()) != allowedValues.end();
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return it->description;
}

std::string Config::generateValidationReport() const {
    std::ostringstream oss;
    oss << "Configuration Parameter Validation Report" << std::endl;
    oss << "========================================= " << std::endl;
    oss << "Total parameters: " << pImpl->entries.size() << std::endl;
    oss << "Parameters by source:" << std::endl;
    
    int defaultCount = 0, fileCount = 0, cmdLineCount = 0, runtimeCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cmdLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "  Default:    " << defaultCount << std::endl;
    oss << "  From file:  " << fileCount << std::endl;
    oss << "  From cmd:   " << cmdLineCount << std::endl;
    oss << "  Runtime:    " << runtimeCount << std::endl;
    oss << std::endl;
    oss << "Detailed parameter list:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << std::left << std::setw(20) << entry.key << ": ";
        
        // Show parameter type
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << "int = " << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << "int64 = " << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << "double = " << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << "bool = " << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "string = \"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "vector<int> = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "vector<double> = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "vector<string> = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            }
        }, entry.value);
        
        oss << " (Source: " << static_cast<int>(entry.source) << ")";
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
    }
    
    return oss.str();
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