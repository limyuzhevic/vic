#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <locale>

namespace nlm {

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
        NLM_LOG_WARN("Could not open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool fileHasContent = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Skip comments (both # and //)
        if (line.rfind("//", 0) == 0 || line.rfind("#", 0) == 0) {
            continue;
        }
        
        fileHasContent = true;
        
        try {
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + 
                            ": missing '=' in: " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Attempt to auto-detect type and parse appropriately
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                set(key, boolVal, ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARN("Failed to parse double value for key " + key + ": " + value);
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Check for integer
                try {
                    bool isInt = true;
                    size_t pos = 0;
                    int64_t intVal = std::stoll(value, &pos);
                    if (pos == value.size() && (value[0] == '-' || std::isdigit(value[0]))) {
                        set(key, intVal, ConfigSource::File);
                        continue;
                    }
                    isInt = false;
                } catch (const std::exception& e) {
                    NLM_LOG_WARN("Failed to parse integer value for key " + key + ": " + value);
                }
                
                // Try parsing as string
                set(key, value, ConfigSource::File);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARN("Error parsing config line " + std::to_string(lineNumber) + ": " + e.what());
            continue;
        }
    }
    
    if (!fileHasContent && lineNumber > 0) {
        NLM_LOG_WARN("Config file " + filepath + " is empty or contains only comments");
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
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
        NLM_LOG_WARN("Could not open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize the config value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
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
    // No longer needed - removed unused variable
    if (value == "true" || value == "True" || value == "1") {
        set(key, true, source);
        return;
    }
    if (value == "false" || value == "False" || value == "0") {
        set(key, false, source);
        return;
    }
    
    // Try double parsing
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (const std::exception& e) {}
    
    // Try int64_t parsing
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (const std::exception& e) {}
    
    // Try int parsing (to catch values that fit in int but not int64_t range)
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (const std::exception& e) {}
    
    // Try vector parsing
    if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
        std::vector<std::string> stringElements;
        std::string elements = value.substr(1, value.size() - 2);
        size_t pos = 0;
        std::string token;
        
        while ((pos = elements.find(',')) != std::string::npos) {
            token = trim(elements.substr(0, pos));
            if (!token.empty()) {
                // Remove quotes if present
                if (token.size() >= 2 && ((token.front() == '"' && token.back() == '"') ||
                                         (token.front() == '\'' && token.back() == '\''))) {
                    token = token.substr(1, token.size() - 2);
                }
                stringElements.push_back(token);
            }
            elements.erase(0, pos + 1);
        }
        
        // Add last element
        token = trim(elements);
        if (!token.empty()) {
            if (token.size() >= 2 && ((token.front() == '"' && token.back() == '"') ||
                                     (token.front() == '\'' && token.back() == '\''))) {
                token = token.substr(1, token.size() - 2);
            }
            stringElements.push_back(token);
        }
        
        if (!stringElements.empty()) {
            set(key, stringElements, source);
            return;
        }
    }
    
    // Default to string
    set(key, value, source);
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
