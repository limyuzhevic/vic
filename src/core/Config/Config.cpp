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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help or -h for help
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit after showing help
        }
        
        // Handle --list-configs for listing all available options
        if (arg == "--list-configs") {
            printAvailableConfigs();
            return false; // Exit after showing config list
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Validate and set the configuration value
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    // Silently ignore invalid values in batch processing
                    // In a real application, you might want to log warnings
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                // Silently ignore invalid values in batch processing
            }
        }
    }
    return true;
}

void Config::printHelp() const {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "--help, -h                      Show this help message" << std::endl;
    std::cout << "--list-configs                  List all available configuration options" << std::endl;
    std::cout << "--config=<file>                Load configuration from file" << std::endl;
    std::cout << "--<key>=<value>                Set configuration value (e.g., --neuron_count=100)" << std::endl;
    std::cout << "-<key> <value>                 Short form for key=value pairs" << std::endl;
    std::cout << std::endl;
    std::cout << "Common configuration options:" << std::endl;
    printAvailableConfigs();
}

void Config::printAvailableConfigs() const {
    std::cout << "Available Configuration Options:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        std::cout << "  " << entry.key << " (type: " << getTypeName(entry.value) << ")" << std::endl;
    }
}

std::string Config::getTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&](auto&& v) {
        if constexpr (std::is_same_v<decltype(v), int>) {
            typeName = "int";
        } else if constexpr (std::is_same_v<decltype(v), int64_t>) {
            typeName = "int64";
        } else if constexpr (std::is_same_v<decltype(v), double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<decltype(v), bool>) {
            typeName = "bool";
        } else if constexpr (std::is_same_v<decltype(v), std::string>) {
            typeName = "string";
        } else if constexpr (std::is_same_v<decltype(v), std::vector<int>>) {
            typeName = "int[]";
        } else if constexpr (std::is_same_v<decltype(v), std::vector<double>>) {
            typeName = "double[]";
        } else if constexpr (std::is_same_v<decltype(v), std::vector<std::string>>) {
            typeName = "string[]";
        }
    }, value);
    return typeName.empty() ? "unknown" : typeName;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Determine target type based on key name or value content
    bool success = false;
    
    // Try to determine the best type based on the key name
    std::string lowerKey = toLower(key);
    
    // Check for boolean values
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
        success = true;
    }
    // Check for integer values
    else if (isNumeric(valueStr)) {
        // Try different integer types
        try {
            size_t pos;
            long long intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.length()) {
                // Check if it fits in 32-bit range
                if (intVal >= INT_MIN && intVal <= INT_MAX) {
                    set(key, static_cast<int>(intVal), source);
                    success = true;
                } else {
                    set(key, intVal, source);
                    success = true;
                }
            }
        } catch (...) {}
        
        // Try double if integer didn't work
        if (!success) {
            try {
                double doubleVal = std::stod(valueStr);
                set(key, doubleVal, source);
                success = true;
            } catch (...) {}
        }
    }
    // Handle string values
    else {
        set(key, valueStr, source);
        success = true;
    }
    
    return success;
}

bool Config::isNumeric(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
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
