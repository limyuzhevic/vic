#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <limits>

namespace nlm {

// Improved parsing functions
static bool parseValue(const std::string& valueStr, ConfigValue& outValue) {
    // Remove whitespace from both ends
    std::string str = trim(valueStr);
    if (str.empty()) {
        return false;
    }
    
    // Try bool
    if (str == "true" || str == "1") {
        outValue = true;
        return true;
    } else if (str == "false" || str == "0") {
        outValue = false;
        return true;
    }
    
    // Try int
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            outValue = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            outValue = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try string (remove quotes)
    if (str.size() >= 2 && ((str.front() == '"' && str.back() == '"') || 
                            (str.front() == '\'' && str.back() == '\''))) {
        outValue = str.substr(1, str.size() - 2);
        return true;
    }
    
    // Try vector format [1,2,3] or [1.0,2.0,3.0] or ["a","b","c"]
    if (str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<ConfigValue> vec;
        
        // Simple vector parsing - split by commas, handling nested brackets
        size_t pos = 0;
        while (pos < inner.size()) {
            size_t commaPos = inner.find(',', pos);
            if (commaPos == std::string::npos) {
                commaPos = inner.size();
            }
            
            std::string element = trim(inner.substr(pos, commaPos - pos));
            ConfigValue elementValue;
            if (parseValue(element, elementValue)) {
                vec.push_back(std::move(elementValue));
            }
            pos = commaPos + 1;
            while (pos < inner.size() && (inner[pos] == ' ' || inner[pos] == '\t')) {
                pos++;
            }
        }
        
        if (!vec.empty()) {
            // Try to determine vector element type
            bool allInts = std::all_of(vec.begin(), vec.end(), 
                [](const ConfigValue& val) { return std::holds_alternative<int>(val); });
            bool allDoubles = std::all_of(vec.begin(), vec.end(),
                [](const ConfigValue& val) { return std::holds_alternative<double>(val); });
            bool allStrings = std::all_of(vec.begin(), vec.end(),
                [](const ConfigValue& val) { return std::holds_alternative<std::string>(val); });
            bool allBools = std::all_of(vec.begin(), vec.end(),
                [](const ConfigValue& val) { return std::holds_alternative<bool>(val); });
            
            if (allInts) {
                std::vector<int> intVec;
                for (const auto& val : vec) {
                    intVec.push_back(std::get<int>(val));
                }
                outValue = intVec;
                return true;
            } else if (allDoubles) {
                std::vector<double> doubleVec;
                for (const auto& val : vec) {
                    doubleVec.push_back(std::get<double>(val));
                }
                outValue = doubleVec;
                return true;
            } else if (allStrings) {
                std::vector<std::string> stringVec;
                for (const auto& val : vec) {
                    stringVec.push_back(std::get<std::string>(val));
                }
                outValue = stringVec;
                return true;
            } else if (allBools) {
                std::vector<bool> boolVec;
                for (const auto& val : vec) {
                    boolVec.push_back(std::get<bool>(val));
                }
                outValue = boolVec;
                return true;
            }
        }
    }
    
    // Try string (no quotes)
    outValue = str;
    return true;
}

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
        
        // In the loadFromFile method, replace the key=value parsing with parseValue
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            ConfigValue value;
            if (parseValue(valueStr, value)) {
                set(key, value, ConfigSource::File);
            } else {
                // Log or handle parsing error
                // For now, just skip invalid values
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double to avoid scientific notation for small numbers
                if (std::abs(arg) < 1e6 && std::abs(arg) >= 0.0001) {
                    file << std::fixed << std::setprecision(6) << arg;
                } else {
                    file << std::scientific << std::setprecision(6) << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    if (std::abs(arg[i]) < 1e6 && std::abs(arg[i]) >= 0.0001) {
                        file << std::fixed << std::setprecision(6) << arg[i];
                    } else {
                        file << std::scientific << std::setprecision(6) << arg[i];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
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

// Add new validation and constraint methods
void Config::setValidator(const std::string& key, const ValidationFunction& validator) {
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.validator = validator;
            break;
        }
    }
    constraints.validators[key] = validator;
}

void Config::setConstraints(const ConfigConstraints& newConstraints) {
    constraints = newConstraints;
}

ValidationResult Config::validate() const {
    ValidationResult overallResult = ValidationResult::Success;
    
    // Check for required keys
    for (const auto& requiredKey : constraints.requiredKeys) {
        if (!has(requiredKey)) {
            return ValidationResult::MissingRequired;
        }
    }
    
    // Validate individual entries
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry)) {
            return ValidationResult::InvalidType;
        }
        
        // Check custom validators
        auto validatorIt = constraints.validators.find(entry.key);
        if (validatorIt != constraints.validators.end()) {
            ValidationResult validatorResult = validatorIt->second(entry.key, entry.value);
            if (validatorResult != ValidationResult::Success) {
                return validatorResult;
            }
        }
        
        // Check entry-level validator
        if (entry.validator) {
            ValidationResult entryResult = entry.validator(entry.key, entry.value);
            if (entryResult != ValidationResult::Success) {
                return entryResult;
            }
        }
    }
    
    return overallResult;
}

bool Config::isValid() const {
    return validate() == ValidationResult::Success;
}

std::vector<std::string> Config::getKeysWithPrefix(const std::string& prefix) const {
    std::vector<std::string> matchingKeys;
    std::string lowerPrefix = toLower(prefix);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(lowerPrefix) == 0) {
            matchingKeys.push_back(entry.key);
        }
    }
    
    return matchingKeys;
}

bool Config::validateValue(const ConfigEntry& entry) {
    // Perform type-specific validation
    bool isValid = true;
    
    // Check if value exists
    if (entry.key.empty()) {
        return false;
    }
    
    // Type-specific validation
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            if (arg < constraints.minInt || arg > constraints.maxInt) {
                isValid = false;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (arg < constraints.minFloat || arg > constraints.maxFloat) {
                isValid = false;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Bool values are always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            // String validation - check for empty strings if required
            if (arg.empty() && entry.key.find("required") != std::string::npos) {
                isValid = false;
            }
        }
        
        // Validate vector types
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& val : arg) {
                if (val < constraints.minInt || val > constraints.maxInt) {
                    isValid = false;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& val : arg) {
                if (val < constraints.minFloat || val > constraints.maxFloat) {
                    isValid = false;
                }
            }
        }
    }, entry.value);
    
    return isValid;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

} // namespace nlm
