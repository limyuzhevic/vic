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
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse JSON using nlohmann/json library (modern JSON parsing)
    // This provides robust JSON parsing with error handling
    
    // Use nlohmann/json library to parse the file
    // Note: In a real implementation, we would need to link with nlohmann/json
    // For now, using a simple JSON parser implementation
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Parse JSON manually (simple implementation)
    // This handles basic JSON structures: {"key": "value", "number": 42, "bool": true, "array": [1,2,3]}
    size_t pos = 0;
    
    // Skip whitespace and opening brace
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
        pos++;
    }
    
    if (pos >= content.size() || content[pos] != '{') {
        return false; // Not a valid JSON object
    }
    
    pos++; // Skip opening brace
    
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.size()) {
            break; // End of JSON
        }
        
        if (content[pos] == '}') {
            pos++; // Skip closing brace
            break; // End of JSON object
        }
        
        // Parse key
        if (content[pos] != '"') {
            // Invalid JSON - expected string key
            break;
        }
        
        size_t keyStart = ++pos; // Skip opening quote
        while (pos < content.size() && content[pos] != '"') {
            pos++;
        }
        
        if (pos >= content.size()) {
            break; // Malformed JSON
        }
        
        std::string key = content.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.size() || content[pos] != ':') {
            break; // Malformed JSON
        }
        
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        // Parse value based on type
        if (pos >= content.size()) {
            break; // Malformed JSON
        }
        
        if (content[pos] == '"') {
            // String value
            size_t valueStart = ++pos; // Skip opening quote
            while (pos < content.size() && content[pos] != '"') {
                // Handle escaped quotes
                if (content[pos] == '\\' && pos + 1 < content.size() && content[pos + 1] == '"') {
                    pos += 2; // Skip escaped quote
                    continue;
                }
                pos++;
            }
            
            if (pos < content.size()) {
                std::string value = content.substr(valueStart, pos - valueStart);
                set(key, value, ConfigSource::File);
                pos++; // Skip closing quote
            }
        }
        else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean value
            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            std::string boolStr = content.substr(valueStart, pos - valueStart);
            bool value = (boolStr == "true");
            set(key, value, ConfigSource::File);
        }
        else if (content[pos] == 'n' && content[pos + 1] == 'u' && content[pos + 2] == 'l' && content[pos + 3] == 'l') {
            // null value - skip it
            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            pos += 4; // Skip "null"
        }
        else if (content[pos] >= '0' && content[pos] <= '9') {
            // Number value (integer or float)
            size_t valueStart = pos;
            while (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9' || content[pos] == '.' || content[pos] == '-' || content[pos] == '+' || content[pos] == 'e' || content[pos] == 'E')) {
                pos++;
            }
            std::string numStr = content.substr(valueStart, pos - valueStart);
            
            // Try integer first
            try {
                size_t end;
                int64_t intValue = std::stoll(numStr, &end);
                if (end == numStr.size()) {
                    set(key, intValue, ConfigSource::File);
                }
                else {
                    double doubleValue = std::stod(numStr);
                    set(key, doubleValue, ConfigSource::File);
                }
            } catch (...) {
                // If conversion fails, try as double
                try {
                    double doubleValue = std::stod(numStr);
                    set(key, doubleValue, ConfigSource::File);
                } catch (...) {
                    // Skip unparsable value
                }
            }
        }
        else if (content[pos] == '[') {
            // Array value
            size_t arrayStart = pos++;
            // Parse array elements
            std::string arrayStr;
            int bracketCount = 1;
            
            while (pos < content.size() && bracketCount > 0) {
                if (content[pos] == '[') bracketCount++;
                else if (content[pos] == ']') bracketCount--;
                
                arrayStr += content[pos];
                pos++;
            }
            
            // Try to parse array
            if (!arrayStr.empty() && arrayStr.back() == ']') {
                arrayStr.pop_back(); // Remove closing bracket
                // For now, we only support simple arrays of integers, floats, and strings
                // Parse the array content
                size_t arrayPos = 0;
                while (arrayPos < arrayStr.size()) {
                    // Skip whitespace
                    while (arrayPos < arrayStr.size() && (arrayStr[arrayPos] == ' ' || arrayStr[arrayPos] == '\n' || arrayStr[arrayPos] == '\t')) {
                        arrayPos++;
                    }
                    
                    if (arrayPos >= arrayStr.size() || arrayStr[arrayPos] == ']') {
                        break; // End of array
                    }
                    
                    size_t elementStart = arrayPos;
                    
                    if (arrayStr[arrayPos] == '"') {
                        // String element
                        arrayPos++; // Skip opening quote
                        while (arrayPos < arrayStr.size() && arrayStr[arrayPos] != '"') {
                            if (arrayStr[arrayPos] == '\\' && arrayPos + 1 < arrayStr.size() && arrayStr[arrayPos + 1] == '"') {
                                arrayPos += 2;
                                continue;
                            }
                            arrayPos++;
                        }
                        std::string element = arrayStr.substr(elementStart + 1, arrayPos - elementStart - 2);
                        // Add to vector
                        auto intVecOpt = get<std::vector<int>>(""); // Get empty vector template
                        std::vector<std::string> strVec;
                        strVec.push_back(element);
                        set(key, strVec, ConfigSource::File);
                    }
                    else if (arrayStr[arrayPos] >= '0' && arrayStr[arrayPos] <= '9') {
                        // Number element
                        while (arrayPos < arrayStr.size() && (arrayStr[arrayPos] >= '0' && arrayStr[arrayPos] <= '9' || arrayStr[arrayPos] == '.' || arrayStr[arrayPos] == '-' || arrayStr[arrayPos] == '+' || arrayStr[arrayPos] == 'e' || arrayStr[arrayPos] == 'E')) {
                            arrayPos++;
                        }
                        std::string numStr = arrayStr.substr(elementStart, arrayPos - elementStart);
                        // Add to vector - for now only handle as double
                        auto intVecOpt = get<std::vector<double>>(""); // Get empty vector template
                        std::vector<double> dblVec;
                        try {
                            double val = std::stod(numStr);
                            dblVec.push_back(val);
                            set(key, dblVec, ConfigSource::File);
                        } catch (...) {
                            // Skip unparsable element
                        }
                    }
                    else if (arrayStr[arrayPos] == ',') {
                        arrayPos++; // Skip comma
                    }
                }
            }
        }
        else {
            // Skip unknown value type
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
        }
        
        // Skip comma or closing brace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos < content.size()) {
            if (content[pos] == ',') {
                pos++; // Skip comma
            }
        }
    }
    
    return true;
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
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        // Only save entries that are not Default source
        if (entry.source == ConfigSource::Default) {
            continue;
        }
        
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Serialize the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            }
            else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            }
            else if constexpr (std::is_same_v<T, double>) {
                // Use formatting to avoid scientific notation for nice numbers
                file << std::fixed << std::setprecision(6) << arg;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
            else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            }
            else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
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
