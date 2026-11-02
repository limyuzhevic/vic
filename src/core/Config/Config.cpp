#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <unordered_set>

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
    
    // Try to parse as JSON first (if nlohmann::json is available)
    // For now, use the simple key=value format
    std::string line;
    bool hasValidEntries = false;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid config format at line " << lineNumber 
                     << ": '" << line << "' (missing '=')" << std::endl;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            std::cerr << "Warning: Empty key at line " << lineNumber << std::endl;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Handle different value types intelligently
        parseAndSetValue(key, value, ConfigSource::File);
        hasValidEntries = true;
    }
    
    return hasValidEntries;
}

void Config::parseAndSetValue(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse as different types in order of specificity
    bool parsed = false;
    
    // Try boolean first (true/false, yes/no, on/off)
    if (!parsed) {
        std::string lowerValue = toLower(value);
        if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "on" || lowerValue == "1") {
            set(key, true, source);
            parsed = true;
        } else if (lowerValue == "false" || lowerValue == "no" || lowerValue == "off" || lowerValue == "0") {
            set(key, false, source);
            parsed = true;
        }
    }
    
    // Try integer
    if (!parsed) {
        try {
            size_t pos = 0;
            long int intValue = std::stol(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, source);
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try int64_t
    if (!parsed) {
        try {
            size_t pos = 0;
            long long intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, source);
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try floating point
    if (!parsed) {
        try {
            size_t pos = 0;
            double floatValue = std::stod(value, &pos);
            if (pos == value.size()) {
                // Check if it looks like an integer (e.g., "42.0") and store as int
                if (value.find('.') != std::string::npos && value.find('.') + 1 < value.size()) {
                    // Has fractional part, store as double
                    set(key, floatValue, source);
                } else {
                    // Looks like integer, store as int
                    set(key, static_cast<int>(floatValue), source);
                }
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try vector types
    if (!parsed) {
        parseVectorValue(key, value, source);
    }
    
    // Fall back to string
    if (!parsed) {
        set(key, value, source);
    }
}

void Config::parseVectorValue(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse as JSON array first (simplified detection)
    if (value.front() == '[' && value.back() == ']') {
        // Try to parse as JSON array of basic types
        std::string content = value.substr(1, value.size() - 2);
        parseArrayContent(key, content, source);
    } else if (value.find(';') != std::string::npos || value.find(',') != std::string::npos) {
        // Parse as comma or semicolon separated list
        parseListValue(key, value, source);
    }
}

void Config::parseArrayContent(const std::string& key, const std::string& content, ConfigSource source) {
    // Simplified JSON array parsing
    std::vector<std::string> elements;
    size_t start = 0;
    bool inQuotes = false;
    char quoteChar = '\0';
    
    for (size_t i = 0; i <= content.size(); ++i) {
        if (i == content.size() || (content[i] == ',' && !inQuotes)) {
            if (start < i) {
                std::string element = content.substr(start, i - start);
                element = trim(element);
                if (!element.empty()) {
                    elements.push_back(element);
                }
            }
            start = i + 1;
        } else if (content[i] == '\"' || content[i] == '\'') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = content[i];
            } else if (quoteChar == content[i]) {
                inQuotes = false;
                quoteChar = '\0';
            }
        }
    }
    
    if (!elements.empty()) {
        // Try to infer the type of the first element to determine vector type
        std::string firstElement = elements[0];
        bool allInts = true;
        bool allFloats = true;
        
        for (const auto& element : elements) {
            try {
                std::stoll(element);
            } catch (...) {
                allInts = false;
            }
            try {
                std::stod(element);
            } catch (...) {
                allFloats = false;
            }
        }
        
        if (allInts) {
            std::vector<int64_t> intVec;
            intVec.reserve(elements.size());
            for (const auto& element : elements) {
                intVec.push_back(std::stoll(element));
            }
            set(key, intVec, source);
        } else if (allFloats) {
            std::vector<double> floatVec;
            floatVec.reserve(elements.size());
            for (const auto& element : elements) {
                floatVec.push_back(std::stod(element));
            }
            set(key, floatVec, source);
        } else {
            // Fall back to string vector
            std::vector<std::string> stringVec;
            stringVec.reserve(elements.size());
            for (const auto& element : elements) {
                // Remove quotes from string elements
                std::string cleanElement = element;
                if ((cleanElement.front() == '\"' && cleanElement.back() == '\"') ||
                    (cleanElement.front() == '\'' && cleanElement.back() == '\'')) {
                    cleanElement = cleanElement.substr(1, cleanElement.size() - 2);
                }
                stringVec.push_back(cleanElement);
            }
            set(key, stringVec, source);
        }
    }
}

void Config::parseListValue(const std::string& key, const std::string& value, ConfigSource source) {
    std::vector<std::string> elements;
    size_t start = 0;
    char delimiter = (value.find(';') != std::string::npos) ? ';' : ',';
    
    for (size_t i = 0; i <= value.size(); ++i) {
        if (i == value.size() || value[i] == delimiter) {
            if (start < i) {
                std::string element = value.substr(start, i - start);
                element = trim(element);
                if (!element.empty()) {
                    elements.push_back(element);
                }
            }
            start = i + 1;
        }
    }
    
    if (!elements.empty()) {
        set(key, elements, source);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Track which keys we've seen to detect conflicts
    std::unordered_set<std::string> seenKeys;
    std::vector<std::string> conflicts;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Parse --key=value or -key value format
        bool isLongOption = (arg.substr(0, 2) == "--");
        bool isShortOption = (arg[0] == '-' && arg.size() > 1 && arg[1] != '-');
        
        if (!isLongOption && !isShortOption) {
            // Could be a file path or other argument
            continue;
        }
        
        std::string key;
        std::string value;
        
        if (isLongOption) {
            // Handle --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                key = arg.substr(2, pos - 2);
                value = arg.substr(pos + 1);
            } else {
                // Handle --key value format (alternative)
                key = arg.substr(2);
                value = "true";  // Default value for flag
            }
        } else {
            // Handle -key value format
            key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            } else {
                value = "true";  // Default value for flag
            }
        }
        
        // Track key usage for conflict detection
        if (seenKeys.find(key) != seenKeys.end()) {
            conflicts.push_back(key);
        } else {
            seenKeys.insert(key);
        }
        
        // Parse and set the value with better error handling
        parseAndSetValue(key, value, ConfigSource::CommandLine);
    }
    
    // Report conflicts if any
    if (!conflicts.empty()) {
        std::cerr << "Warning: Potential key conflicts detected in command line arguments:" << std::endl;
        for (const auto& key : conflicts) {
            std::cerr << "  --" << key << " (appears multiple times)" << std::endl;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Format: key = value (type preserved)\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << " = " << "(type: ";
        
        // Describe the value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) file << "int";
            else if constexpr (std::is_same_v<T, int64_t>) file << "int64_t";
            else if constexpr (std::is_same_v<T, double>) file << "double";
            else if constexpr (std::is_same_v<T, bool>) file << "bool";
            else if constexpr (std::is_same_v<T, std::string>) file << "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) file << "int[]";
            else if constexpr (std::is_same_v<T, std::vector<double>>) file << "double[]";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) file << "string[]";
            else file << "unknown";
        }, entry.value);
        
        file << ")\n";
        file << entry.key << " = " << getValueString(entry.value) << "\n\n";
    }
    
    return true;
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
