#pragma once

#include "ConfigBuilder.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;
    std::vector<std::function<void(const std::string&, const ConfigValue&, ConfigSource)>> callbacks;
    bool validateOnSet = false;
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
    
    // Try JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Clear existing entries
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->entries.clear();
    }
    
    try {
        // Try to parse as JSON
        auto jsonData = nlohmann::json::parse(content);
        parseJson(jsonData, ConfigSource::File);
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // Fall back to YAML
        try {
            auto yamlData = YAML::Load(content);
            parseYaml(yamlData, ConfigSource::File);
            return true;
        } catch (const YAML::Exception&) {
            // Fall back to simple key=value format
            return loadFromSimpleFormat(content, ConfigSource::File);
        }
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
                setInternal(key, value, ConfigSource::CommandLine);
            } else {
                // Handle --key value format
                setArgValue(key, arg, i, argc, argv);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            setArgValue(arg.substr(1), arg, i, argc, argv);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue back to string representation
        file << entry.key << " = ";
        visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<int>>) {
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
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setInternal(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setInternal(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setInternal(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setInternal(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setInternal(key, value, source);
}

void Config::setInternal(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool isNew = true;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check for validation
        if (pImpl->validateOnSet && !validateValue(key, value, source)) {
            return;
        }
        it->value = value;
        it->source = source;
        isNew = false;
    } else {
        pImpl->entries.emplace_back(key, value, source, "");
    }
    
    // Notify callbacks
    for (const auto& callback : pImpl->callbacks) {
        callback(key, value, source);
    }
}

void Config::setArgValue(const std::string& key, const std::string& arg, int& currentIndex, int argc, char** argv) {
    // If the argument is --key=value, value is in arg
    if (key.find('=') != std::string::npos) {
        size_t pos = key.find('=');
        std::string actualKey = key.substr(0, pos);
        std::string value = key.substr(pos + 1);
        setInternal(actualKey, value, ConfigSource::CommandLine);
        return;
    }
    
    // Otherwise, next argument is the value
    if (currentIndex + 1 < argc) {
        currentIndex++;
        setInternal(key, std::string(argv[currentIndex]), ConfigSource::CommandLine);
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Basic validation logic
    if (source == ConfigSource::File && key == "log_to_file" && std::get<bool>(value) == true) {
        // Additional validation for file logging could be added here
    }
    return true;
}

void Config::parseJson(const nlohmann::json& jsonData, ConfigSource source) {
    for (const auto& [key, value] : jsonData.items()) {
        setFromJsonValue(key, value, source);
    }
}

void Config::parseYaml(const YAML::Node& yamlData, ConfigSource source) {
    for (const auto& pair : yamlData) {
        if (pair.first.IsScalar() && pair.second.IsDefined()) {
            setFromYamlValue(pair.first.Scalar(), pair.second, source);
        }
    }
}

void Config::setFromJsonValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        setInternal(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        setInternal(key, value.get<int>(), source);
    } else if (value.is_number_float()) {
        setInternal(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        setInternal(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Try to convert to appropriate vector type
        auto arr = value.get<std::vector<nlohmann::json>>();
        std::vector<int> intArray;
        bool allInt = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_number_integer(); });
        if (allInt) {
            intArray = arr.get<std::vector<int>>();
            setInternal(key, intArray, source);
        } else {
            std::vector<double> doubleArray;
            bool allDouble = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_number_float(); });
            if (allDouble) {
                doubleArray = arr.get<std::vector<double>>();
                setInternal(key, doubleArray, source);
            } else {
                std::vector<std::string> stringArray;
                bool allString = std::all_of(arr.begin(), arr.end(), [](const nlohmann::json& v) { return v.is_string(); });
                if (allString) {
                    stringArray = arr.get<std::vector<std::string>>();
                    setInternal(key, stringArray, source);
                }
            }
        }
    }
}

void Config::setFromYamlValue(const std::string& key, const YAML::Node& value, ConfigSource source) {
    if (value.IsScalar()) {
        std::string strValue = value.Scalar();
        // Try to convert to appropriate type
        if (strValue == "true" || strValue == "false") {
            setInternal(key, strValue == "true", source);
        } else {
            try {
                size_t pos;
                int intValue = std::stoi(strValue, &pos);
                if (pos == strValue.size()) {
                    setInternal(key, intValue, source);
                    return;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                double doubleValue = std::stod(strValue, &pos);
                if (pos == strValue.size()) {
                    setInternal(key, doubleValue, source);
                    return;
                }
            } catch (...) {}
            
            setInternal(key, strValue, source);
        }
    } else if (value.IsSequence()) {
        // Convert YAML sequence to vector
        std::vector<std::string> stringArray;
        for (const auto& item : value) {
            stringArray.push_back(item.Scalar());
        }
        setInternal(key, stringArray, source);
    }
}

bool Config::loadFromSimpleFormat(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to infer type
            bool converted = false;
            
            if (value == "true" || value == "false") {
                setInternal(key, value == "true", source);
                converted = true;
            } else {
                try {
                    size_t pos;
                    int intValue = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        setInternal(key, intValue, source);
                        converted = true;
                    }
                } catch (...) {}
                
                if (!converted) {
                    try {
                        size_t pos;
                        double doubleValue = std::stod(value, &pos);
                        if (pos == value.size()) {
                            setInternal(key, doubleValue, source);
                            converted = true;
                        }
                    } catch (...) {}
                }
            }
            
            if (!converted) {
                setInternal(key, value, source);
            }
        }
    }
    return true;
}

void Config::setCallbacks(std::vector<std::function<void(const std::string&, const ConfigValue&, ConfigSource)>> callbacks) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->callbacks = std::move(callbacks);
}

void Config::enableValidation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->validateOnSet = enable;
}

std::string Config::toString(const ConfigValue& value) const {
    std::ostringstream oss;
    visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
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