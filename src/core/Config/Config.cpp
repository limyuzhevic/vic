#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <type_traits>

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
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Try to parse JSON first (if file contains { } or [ ])
        if ((line.find('{') != std::string::npos || line.find('[') != std::string::npos) {
            // TODO PHASE 2: Implement JSON parser
            // For now, return false to indicate JSON parsing not implemented
            return false;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '\"' && valueStr.back() == '\"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Try to parse value based on comments or heuristics
            ConfigValue value;
            bool parsed = false;
            
            // Check for vector format [1, 2, 3]
            if (valueStr.front() == '[' && valueStr.back() == ']') {
                std::string content = valueStr.substr(1, valueStr.size() - 2);
                std::vector<std::string> tokens;
                std::string token;
                std::istringstream iss(content);
                while (std::getline(iss, token, ',')) {
                    token = trim(token);
                    if (!token.empty()) {
                        tokens.push_back(token);
                    }
                }
                
                if (!tokens.empty()) {
                    // Try to determine vector type by first element
                    bool allInts = true;
                    bool allDoubles = true;
                    bool allStrings = true;
                    
                    for (const auto& t : tokens) {
                        // Check if can be int
                        try {
                            size_t pos;
                            int val = std::stoi(t, &pos);
                            if (pos == t.size()) {
                                // Could be int
                                allStrings = false;
                                allDoubles = false;
                            } else {
                                allInts = false;
                            }
                        } catch (...) {
                            allInts = false;
                        }
                        
                        // Check if can be double
                        try {
                            size_t pos;
                            double val = std::stod(t, &pos);
                            if (pos == t.size()) {
                                // Could be double
                                allStrings = false;
                                allInts = false;
                            } else {
                                allDoubles = false;
                            }
                        } catch (...) {
                            allDoubles = false;
                        }
                    }
                    
                    if (allInts) {
                        std::vector<int> intVec;
                        for (const auto& t : tokens) {
                            intVec.push_back(std::stoi(t));
                        }
                        value = intVec;
                        parsed = true;
                    } else if (allDoubles) {
                        std::vector<double> doubleVec;
                        for (const auto& t : tokens) {
                            doubleVec.push_back(std::stod(t));
                        }
                        value = doubleVec;
                        parsed = true;
                    } else if (allStrings) {
                        std::vector<std::string> stringVec;
                        for (const auto& t : tokens) {
                            stringVec.push_back(t);
                        }
                        value = stringVec;
                        parsed = true;
                    }
                }
            } else {
                // Try boolean
                if (valueStr == "true" || valueStr == "false") {
                    value = (valueStr == "true");
                    parsed = true;
                }
                // Try integer
                else {
                    try {
                        size_t pos;
                        // Try int
                        int intVal = std::stoi(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            value = intVal;
                            parsed = true;
                        } else {
                            // Try int64_t
                            int64_t int64Val = std::stoll(valueStr, &pos);
                            if (pos == valueStr.size()) {
                                value = int64Val;
                                parsed = true;
                            }
                        }
                    } catch (...) {}
                }
                
                // Try double
                if (!parsed) {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            value = doubleVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Fall back to string
                if (!parsed) {
                    value = valueStr;
                    parsed = true;
                }
            }
            
            if (parsed) {
                // Use ConfigValue directly constructor via set method
                set(key, value, ConfigSource::File);
            }
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
        
        // Save the actual value based on its type
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
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
    // Auto-detect type name for new entries
    std::string typeName = getTypeName(value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->typeName = typeName; // Update type name for existing entry
    } else {
        pImpl->entries.emplace_back(key, value, source, "", typeName);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Convert string to appropriate type based on value
    ConfigValue convertedValue = stringToConfigValue(value);
    set(key, convertedValue, source);
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
