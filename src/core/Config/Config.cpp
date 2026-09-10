#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <limits>

#ifdef HAS_NLOHMANN_JSON
    #include <nlohmann/json.hpp>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to check if a string represents an integer (including 64-bit)
static bool isIntegerString(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[pos] == '+' || str[pos] == '-') {
        pos++;
    }
    
    if (str.size() == pos) return false;
    
    bool hasDigit = false;
    for (size_t i = pos; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
        hasDigit = true;
    }
    
    return hasDigit;
}

// Helper function to check if a string represents a double
static bool isDoubleString(const std::string& str) {
    std::istringstream iss(str);
    double val;
    iss >> std::noskipws >> val;
    return iss && iss.eof() && !iss.fail();
}

// Helper function to convert string to appropriate type
static ConfigValue stringToConfigValue(const std::string& str) {
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try 64-bit integer first
    try {
        size_t pos;
        int64_t val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return val;
        }
    } catch (...) {}
    
    // Try 32-bit integer
    try {
        size_t pos;
        int val = std::stoi(str, &pos);
        if (pos == str.size()) {
            return val;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) {
            return val;
        }
    } catch (...) {}
    
    return str;
}

#ifdef HAS_NLOHMANN_JSON

nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_null()) {
        return std::string("");
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        int64_t val = jsonValue.get<int64_t>();
        if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
            return static_cast<int>(val);
        }
        return val;
    } else if (jsonValue.is_number_float()) {
        double val = jsonValue.get<double>();
        if (std::floor(val) == val) {
            int64_t intVal = static_cast<int64_t>(val);
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                return static_cast<int>(intVal);
            }
            return intVal;
        }
        return val;
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        if (jsonValue.empty()) {
            return std::vector<int>{};
        }
        
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& elem : jsonValue) {
            if (elem.is_number_integer()) {
                allDoubles = false;
                allStrings = false;
            } else if (elem.is_number_float()) {
                allInts = false;
                allStrings = false;
            } else if (elem.is_string()) {
                allInts = false;
                allDoubles = false;
            } else {
                allInts = false;
                allDoubles = false;
                allStrings = false;
                break;
            }
        }
        
        if (allInts) {
            std::vector<int> vec;
            vec.reserve(jsonValue.size());
            for (const auto& elem : jsonValue) {
                vec.push_back(elem.get<int>());
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            vec.reserve(jsonValue.size());
            for (const auto& elem : jsonValue) {
                vec.push_back(elem.get<double>());
            }
            return vec;
        } else if (allStrings) {
            std::vector<std::string> vec;
            vec.reserve(jsonValue.size());
            for (const auto& elem : jsonValue) {
                vec.push_back(elem.get<std::string>());
            }
            return vec;
        }
        
        std::vector<std::string> vec;
        vec.reserve(jsonValue.size());
        for (const auto& elem : jsonValue) {
            vec.push_back(elem.dump());
        }
        return vec;
    }
    
    return jsonValue.dump();
}

bool validateJsonConfig(const nlohmann::json& jsonConfig, const ConfigSchema& schema) {
    for (const auto& schemaEntry : schema.entries) {
        if (schemaEntry.required && !jsonConfig.contains(schemaEntry.key)) {
            return false;
        }
        
        if (jsonConfig.contains(schemaEntry.key)) {
            const auto& jsonValue = jsonConfig[schemaEntry.key];
            
            bool typeMatches = false;
            if (schemaEntry.type == "int" && jsonValue.is_number_integer()) {
                typeMatches = true;
            } else if (schemaEntry.type == "int64" && jsonValue.is_number_integer() && 
                       jsonValue.get<int64_t>() >= std::numeric_limits<int64_t>::min() && 
                       jsonValue.get<int64_t>() <= std::numeric_limits<int64_t>::max()) {
                typeMatches = true;
            } else if (schemaEntry.type == "double" && jsonValue.is_number()) {
                typeMatches = true;
            } else if (schemaEntry.type == "bool" && jsonValue.is_boolean()) {
                typeMatches = true;
            } else if (schemaEntry.type == "string" && jsonValue.is_string()) {
                typeMatches = true;
            } else if ((schemaEntry.type == "vector<int>" || schemaEntry.type == "array<int>") && jsonValue.is_array()) {
                bool allInt = true;
                for (const auto& elem : jsonValue) {
                    if (!elem.is_number_integer()) {
                        allInt = false;
                        break;
                    }
                }
                typeMatches = allInt;
            } else if ((schemaEntry.type == "vector<double>" || schemaEntry.type == "array<double>") && jsonValue.is_array()) {
                bool allDouble = true;
                for (const auto& elem : jsonValue) {
                    if (!elem.is_number()) {
                        allDouble = false;
                        break;
                    }
                }
                typeMatches = allDouble;
            } else if ((schemaEntry.type == "vector<string>" || schemaEntry.type == "array<string>") && jsonValue.is_array()) {
                bool allString = true;
                for (const auto& elem : jsonValue) {
                    if (!elem.is_string()) {
                        allString = false;
                        break;
                    }
                }
                typeMatches = allString;
            }
            
            if (!typeMatches) {
                return false;
            }
            
            if (typeMatches && !jsonValue.is_null()) {
                double val = 0.0;
                if (schemaEntry.type == "int" || schemaEntry.type == "int64") {
                    val = jsonValue.get<int64_t>();
                } else if (schemaEntry.type == "double") {
                    val = jsonValue.get<double>();
                }
                
                if (schemaEntry.minValue != 0.0 && val < schemaEntry.minValue) {
                    return false;
                }
                if (schemaEntry.maxValue != 0.0 && val > schemaEntry.maxValue) {
                    return false;
                }
                
                if (schemaEntry.type == "string" && !schemaEntry.validationPattern.empty()) {
                    static std::regex patternRegex("^" + schemaEntry.validationPattern + "$");
                    if (!std::regex_match(jsonValue.get<std::string>(), patternRegex)) {
                        return false;
                    }
                }
                
                if ((schemaEntry.type == "vector<int>" || schemaEntry.type == "array<int>" ||
                     schemaEntry.type == "vector<double>" || schemaEntry.type == "array<double>" ||
                     schemaEntry.type == "vector<string>" || schemaEntry.type == "array<string>") && 
                    jsonValue.is_array()) {
                    size_t size = jsonValue.size();
                    if (schemaEntry.minSize > 0 && size < schemaEntry.minSize) {
                        return false;
                    }
                    if (schemaEntry.maxSize > 0 && size > schemaEntry.maxSize) {
                        return false;
                    }
                }
            }
            
            if (schema.customValidators.find(schemaEntry.key) != schema.customValidators.end()) {
                ConfigValue configValue = jsonToConfigValue(jsonValue);
                if (!schema.customValidators[schemaEntry.key](configValue)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

#endif

bool Config::loadFromFile(const std::string& filepath) {
    #ifdef HAS_NLOHMANN_JSON
    std::ifstream jsonFile(filepath);
    if (jsonFile.good()) {
        try {
            nlohmann::json jsonConfig;
            jsonFile >> jsonConfig;
            
            if (jsonConfig.is_object()) {
                for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                    set(it.key(), jsonToConfigValue(it.value()), ConfigSource::File);
                }
                return true;
            }
        } catch (...) {
        }
    }
    #endif
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, stringToConfigValue(value), ConfigSource::File);
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
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                std::string doubleStr = oss.str();
                size_t decimalPos = doubleStr.find('.');
                if (decimalPos != std::string::npos) {
                    while (doubleStr.back() == '0') {
                        doubleStr.pop_back();
                    }
                    if (doubleStr.back() == '.') {
                        doubleStr.pop_back();
                    }
                }
                file << doubleStr;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::string escaped = arg;
                size_t pos;
                while ((pos = escaped.find('\n')) != std::string::npos) {
                    escaped.replace(pos, 1, "\\n");
                }
                while ((pos = escaped.find('"')) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                }
                file << "\"" << escaped << "\"";
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
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(6) << arg[i];
                    std::string doubleStr = oss.str();
                    size_t decimalPos = doubleStr.find('.');
                    if (decimalPos != std::string::npos) {
                        while (doubleStr.back() == '0') {
                            doubleStr.pop_back();
                        }
                        if (doubleStr.back() == '.') {
                            doubleStr.pop_back();
                        }
                    }
                    file << doubleStr;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    std::string escaped = arg[i];
                    size_t pos;
                    while ((pos = escaped.find('\n')) != std::string::npos) {
                        escaped.replace(pos, 1, "\\n");
                    }
                    while ((pos = escaped.find('"')) != std::string::npos) {
                        escaped.replace(pos, 1, "\\\"");
                    }
                    file << "\"" << escaped << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// ... rest of existing functions ...

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
