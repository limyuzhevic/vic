#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nlm {

Config::Config(const std::string& filepath) {
    if (!filepath.empty()) {
        load(filepath);
    }
}

bool Config::load(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Cannot open config file: ") + filepath);
            return false;
        }
        
        deserialize(file);
        NLM_LOG_INFO(std::string("Config loaded from ") + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load config: ") + e.what());
        return false;
    }
}

bool Config::save(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Cannot save config to file: ") + filepath);
            return false;
        }
        
        serialize(file);
        NLM_LOG_INFO(std::string("Config saved to ") + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save config: ") + e.what());
        return false;
    }
}

template<typename T>
void Config::set(const std::string& key, const T& value) {
    auto it = std::find_if(parameters_.begin(), parameters_.end(),
                           [&key](const Parameter& p) { return p.key == key; });n    
    if (it != parameters_.end()) {
        it->value = value;
    } else {
        parameters_.emplace_back(key, value);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(parameters_.begin(), parameters_.end(),
                           [&key](const Parameter& p) { return p.key == key; });
    
    if (it == parameters_.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(it->value);
    } catch (const std::bad_any_cast&) {
        NLM_LOG_WARNING(std::string("Type mismatch for config key: ") + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

void Config::createParameterGroup(const std::string& groupName, 
                                 const std::string& description) {
    if (std::find(parameterGroups_.begin(), parameterGroups_.end(), groupName) == parameterGroups_.end()) {
        parameterGroups_.push_back(groupName);
        NLM_LOG_INFO(std::string("Created parameter group: ") + groupName);
    }
}

void Config::addToGroup(const std::string& groupName, const std::string& key) {
    auto it = std::find_if(parameters_.begin(), parameters_.end(),
                           [&key](const Parameter& p) { return p.key == key; });
    
    if (it != parameters_.end()) {
        it->groupName = groupName;
        NLM_LOG_INFO(std::string("Added parameter ") + key + std::string(" to group ") + groupName);
    }
}

bool Config::validate() const {
    validationErrors_.clear();
    
    for (const auto& param : parameters_) {
        validateParameter(param);
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return validationErrors_;
}

void Config::serialize(std::ostream& out) const {
    // Write number of parameters
    size_t paramCount = parameters_.size();
    out.write(reinterpret_cast<const char*>(&paramCount), sizeof(paramCount));
    
    // Write each parameter
    for (const auto& param : parameters_) {
        // Write key length and key
        size_t keyLen = param.key.length();
        out.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        out.write(param.key.c_str(), keyLen);
        
        // Write value (simplified - would use proper serialization for each type)
        // For now, write type marker and then value
        out << "{" << param.key << "}:";
        
        if (std::holds_alternative<bool>(param.value)) {
            bool val = std::get<bool>(param.value);
            out << (val ? "true" : "false");
        } else if (std::holds_alternative<int>(param.value)) {
            int val = std::get<int>(param.value);
            out << val;
        } else if (std::holds_alternative<float>(param.value)) {
            float val = std::get<float>(param.value);
            out << val;
        } else if (std::holds_alternative<double>(param.value)) {
            double val = std::get<double>(param.value);
            out << val;
        } else if (std::holds_alternative<std::string>(param.value)) {
            out << "\"" << std::get<std::string>(param.value) << "\"";
        }
        
        out << "\n";
    }
}

void Config::deserialize(std::istream& in) {
    parameters_.clear();
    parameterGroups_.clear();
    
    // Read number of parameters
    size_t paramCount;
    in.read(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));
    
    // Read each parameter (simplified parsing)
    std::string line;
    for (size_t i = 0; i < paramCount && std::getline(in, line); ++i) {
        // Parse line (format: {key}:value)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(1, colonPos - 2);
            std::string valueStr = line.substr(colonPos + 1);
            
            // Simple type detection
            if (valueStr == "true" || valueStr == "false") {
                bool val = (valueStr == "true");
                set(key, val);
            } else if (valueStr.find('.') != std::string::npos) {
                try {
                    double val = std::stod(valueStr);
                    set(key, val);
                } catch (...) {
                    set(key, valueStr);
                }
            } else {
                try {
                    int val = std::stoi(valueStr);
                    set(key, val);
                } catch (...) {
                    set(key, valueStr);
                }
            }
        }
    }
}

bool Config::hasParameter(const std::string& key) const {
    return get<std::string>(key).has_value();
}

std::vector<std::string> Config::getAllParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters_.size());
    for (const auto& param : parameters_) {
        keys.push_back(param.key);
    }
    return keys;
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = std::find_if(parameters_.begin(), parameters_.end(),
                           [&key](const Parameter& p) { return p.key == key; });n    
    if (it != parameters_.end()) {
        return it->description;
    }
    return "";
}

void Config::validateParameter(const Parameter& param) const {
    if (param.required) {
        if (!hasParameter(param.key)) {
            validationErrors_.push_back(std::string("Required parameter missing: ") + param.key);
        }
    }
    
    if (param.validator && !param.validator(param.value)) {
        validationErrors_.push_back(std::string("Invalid value for parameter: ") + param.key);
    }
}

bool Config::validateTypeMatch(const ConfigValue& value, const std::type_info& type) {
    return std::visit([&, this](const auto& val) {
        return typeid(decltype(val)) == type;
    }, value);
}

// Template specializations for common types

// bool specializations
template void Config::set<bool>(const std::string& key, const bool& value);
template std::optional<bool> Config::get<bool>(const std::string& key) const;
template bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const;

// int specializations  
template void Config::set<int>(const std::string& key, const int& value);
template std::optional<int> Config::get<int>(const std::string& key) const;
template int Config::getOr<int>(const std::string& key, const int& defaultValue) const;

// float specializations
template void Config::set<float>(const std::string& key, const float& value);
template std::optional<float> Config::get<float>(const std::string& key) const;
template float Config::getOr<float>(const std::string& key, const float& defaultValue) const;

// double specializations
template void Config::set<double>(const std::string& key, const double& value);
template std::optional<double> Config::get<double>(const std::string& key) const;
template double Config::getOr<double>(const std::string& key, const double& defaultValue) const;

// string specializations
template void Config::set<std::string>(const std::string& key, const std::string& value);
template std::optional<std::string> Config::get<std::string>(const std::string& key) const;
template std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const;
