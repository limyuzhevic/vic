#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

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
    
    // Try to parse as JSON first
    if (parseJSONFile(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
    
    return !pImpl->entries.empty();
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
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                // Format doubles with reasonable precision
                file << entry.key << " = " << std::fixed << std::setprecision(6) << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << "UNKNOWN_TYPE\n";
            }
        }, entry.value);
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

// Parse JSON configuration file
bool Config::parseJSONFile(const std::string& filepath) {
    // Enhanced JSON parser that handles common NLM configuration formats
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON
    try {
        // For now, implement a simplified JSON parser that handles:
        // {"key": "value", "number": 123, "bool": true}
        
        // Remove all whitespace for simplicity
        std::string jsonContent = content;
        jsonContent.erase(std::remove_if(jsonContent.begin(), jsonContent.end(), ::isspace), jsonContent.end());
        
        // Check if it's a JSON object
        if (jsonContent.empty() || jsonContent[0] != '{' || jsonContent.back() != '}') {
            return false;
        }
        
        // Parse key-value pairs from JSON
        std::string jsonData = jsonContent.substr(1, jsonContent.size() - 2); // Remove { and }
        size_t pos = 0;
        
        while (pos < jsonData.size()) {
            // Find next key (quoted string)
            size_t keyStart = jsonData.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonData.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonData.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon separator
            size_t colonPos = jsonData.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            size_t valueStart = colonPos + 1;
            // Skip whitespace after colon
            while (valueStart < jsonData.size() && (jsonData[valueStart] == ' ' || jsonData[valueStart] == '\t' || jsonData[valueStart] == '\n')) {
                valueStart++;
            }
            
            // Handle different JSON value types
            bool validPair = false;
            
            // Handle string values ("value")
            if (valueStart < jsonData.size() && jsonData[valueStart] == '"') {
                size_t valueEnd = jsonData.find('"', valueStart + 1);
                if (valueEnd != std::string::npos) {
                    std::string value = jsonData.substr(valueStart + 1, valueEnd - valueStart - 1);
                    set(key, value, ConfigSource::File);
                    validPair = true;
                }
            }
            // Handle numeric values (integer or float)
            else if (valueStart < jsonData.size() && (std::isdigit(jsonData[valueStart]) || jsonData[valueStart] == '-')) {
                size_t valueEnd = valueStart;
                while (valueEnd < jsonData.size() && (std::isdigit(jsonData[valueEnd]) || jsonData[valueEnd] == '.' || jsonData[valueEnd] == 'e' || jsonData[valueEnd] == 'E' || jsonData[valueEnd] == '+')) {
                    valueEnd++;
                }
                std::string valueStr = jsonData.substr(valueStart, valueEnd - valueStart);
                
                // Try to convert to number
                try {
                    size_t decimalPos = valueStr.find('.');
                    if (decimalPos != std::string::npos) {
                        // Float value
                        double value = std::stod(valueStr);
                        set(key, value, ConfigSource::File);
                    } else {
                        // Integer value
                        int64_t value = std::stoll(valueStr);
                        set(key, value, ConfigSource::File);
                    }
                    validPair = true;
                } catch (const std::exception&) {
                    // Conversion failed, skip this pair
                }
            }
            // Handle boolean values (true/false)
            else if (valueStart + 4 <= jsonData.size() && jsonData.substr(valueStart, 4) == "true") {
                set(key, true, ConfigSource::File);
                validPair = true;
            } else if (valueStart + 5 <= jsonData.size() && jsonData.substr(valueStart, 5) == "false") {
                set(key, false, ConfigSource::File);
                validPair = true;
            }
            
            if (!validPair) {
                // Find next comma or end
                size_t commaPos = jsonData.find(',', keyEnd);
                if (commaPos == std::string::npos) break;
                pos = commaPos + 1;
                continue;
            }
            
            // Find next comma or end of object
            size_t commaPos = jsonData.find(',', valueStart);
            if (commaPos == std::string::npos) break;
            pos = commaPos + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        // JSON parsing failed, fall back to simple parser
        return false;
    }
}
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
