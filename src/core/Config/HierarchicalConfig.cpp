// HierarchicalConfig.cpp - Implementation of hierarchical configuration system

#include "HierarchicalConfig.hpp"
#include "../Error/NLMErrors.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace nlm {

std::string ConfigValue::toString() const {
    switch (type_) {
        case Type::STRING:
            return std::get<std::string>(value_);
        case Type::INTEGER:
            return std::to_string(std::get<int64_t>(value_));
        case Type::DOUBLE:
            return std::to_string(std::get<double>(value_));
        case Type::BOOLEAN:
            return std::get<bool>(value_) ? "true" : "false";
        default:
            throw std::runtime_error("Cannot convert ConfigValue to string for type " + std::to_string(static_cast<int>(type_)));
    }
}

int64_t ConfigValue::toInt() const {
    switch (type_) {
        case Type::STRING:
            return std::stoll(std::get<std::string>(value_));
        case Type::INTEGER:
            return std::get<int64_t>(value_);
        case Type::DOUBLE:
            return static_cast<int64_t>(std::get<double>(value_));
        case Type::BOOLEAN:
            return std::get<bool>(value_) ? 1 : 0;
        default:
            throw std::runtime_error("Cannot convert ConfigValue to integer for type " + std::to_string(static_cast<int>(type_)));
    }
}

double ConfigValue::toDouble() const {
    switch (type_) {
        case Type::STRING:
            return std::stod(std::get<std::string>(value_));
        case Type::INTEGER:
            return static_cast<double>(std::get<int64_t>(value_));
        case Type::DOUBLE:
            return std::get<double>(value_);
        case Type::BOOLEAN:
            return std::get<bool>(value_) ? 1.0 : 0.0;
        default:
            throw std::runtime_error("Cannot convert ConfigValue to double for type " + std::to_string(static_cast<int>(type_)));
    }
}

bool ConfigValue::toBool() const {
    switch (type_) {
        case Type::STRING:
            return !std::get<std::string>(value_).empty() && std::get<std::string>(value_) != "false";
        case Type::INTEGER:
            return std::get<int64_t>(value_) != 0;
        case Type::DOUBLE:
            return std::get<double>(value_) != 0.0;
        case Type::BOOLEAN:
            return std::get<bool>(value_);
        default:
            throw std::runtime_error("Cannot convert ConfigValue to boolean for type " + std::to_string(static_cast<int>(type_)));
    }
}

bool ConfigValue::operator==(const ConfigValue& other) const {
    if (type_ != other.type_) {
        return false;
    }
    switch (type_) {
        case Type::STRING:
            return std::get<std::string>(value_) == std::get<std::string>(other.value_);
        case Type::INTEGER:
            return std::get<int64_t>(value_) == std::get<int64_t>(other.value_);
        case Type::DOUBLE:
            return std::get<double>(value_) == std::get<double>(other.value_);
        case Type::BOOLEAN:
            return std::get<bool>(value_) == std::get<bool>(other.value_);
        case Type::ARRAY:
            return std::get<std::vector<ConfigValue>>(value_) == std::get<std::vector<ConfigValue>>(other.value_);
        case Type::OBJECT:
            return std::get<std::map<std::string, ConfigValue>>(value_) == std::get<std::map<std::string, ConfigValue>>(other.value_);
        default:
            return false;
    }
}

std::string ConfigValue::toStringRepresentation() const {
    switch (type_) {
        case Type::STRING:
            return "\"" + std::get<std::string>(value_) + "\"";
        case Type::INTEGER:
            return std::to_string(std::get<int64_t>(value_));
        case Type::DOUBLE:
            return std::to_string(std::get<double>(value_));
        case Type::BOOLEAN:
            return std::get<bool>(value_) ? "true" : "false";
        case Type::ARRAY:
            return "[" + arrayToString(std::get<std::vector<ConfigValue>>(value_)) + "]";
        case Type::OBJECT:
            return objectToString(std::get<std::map<std::string, ConfigValue>>(value_));
        default:
            return "null";
    }
}

std::string ConfigValue::arrayToString(const std::vector<ConfigValue>& array) const {
    std::ostringstream oss;
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << array[i].toStringRepresentation();
    }
    return oss.str();
}

std::string ConfigValue::objectToString(const std::map<std::string, ConfigValue>& object) const {
    std::ostringstream oss;
    oss << "{\n";
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it != object.begin()) oss << ",\n";
        oss << "  \"" << it->first << "\": " << it->second.toStringRepresentation();
    }
    oss << "\n}";
    return oss.str();
}

HierarchicalConfig::HierarchicalConfig() {
    // Set up default configuration
    setDefault("neuron_count", 100);
    setDefault("region_count", 1);
    setDefault("connection_probability", 0.1f);
    setDefault("enable_plasticity", true);
    setDefault("enable_reward_modulation", true);
    setDefault("enable_development", true);
}

bool HierarchicalConfig::loadFromFile(const std::string& filepath) {
    try {
        // Try JSON first
        if (saveToJson(filepath)) {
            return true;
        }
        // If JSON fails, try binary
        return loadFromBinary(filepath);
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to load configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool HierarchicalConfig::saveToFile(const std::string& filepath) const {
    // Try JSON first
    if (saveToJson(filepath)) {
        return true;
    }
    // If JSON fails, try binary
    return saveToBinary(filepath);
}

bool HierarchicalConfig::loadFromArgs(int argc, char** argv) {
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value);
            }
        }
    }
    return true;
}

bool HierarchicalConfig::has(const std::string& key) const {
    return config_.find(key) != config_.end();
}

std::optional<std::string> HierarchicalConfig::get(const std::string& key) const {
    auto it = config_.find(key);
    if (it == config_.end()) {
        // Check defaults
        auto defaultIt = defaults_.find(key);
        if (defaultIt != defaults_.end()) {
            return defaultIt->second.toString();
        }
        return std::nullopt;
    }
    return it->second.toString();
}

std::optional<int64_t> HierarchicalConfig::getInt(const std::string& key) const {
    auto it = config_.find(key);
    if (it == config_.end()) {
        // Check defaults
        auto defaultIt = defaults_.find(key);
        if (defaultIt != defaults_.end()) {
            if (defaultIt->second.getType() == ConfigValue::Type::INTEGER) {
                return std::get<int64_t>(defaultIt->second.value_);
            }
        }
        return std::nullopt;
    }
    if (it->second.getType() == ConfigValue::Type::INTEGER) {
        return std::get<int64_t>(it->second.value_);
    }
    return std::nullopt;
}

std::optional<double> HierarchicalConfig::getDouble(const std::string& key) const {
    auto it = config_.find(key);
    if (it == config_.end()) {
        // Check defaults
        auto defaultIt = defaults_.find(key);
        if (defaultIt != defaults_.end()) {
            if (defaultIt->second.getType() == ConfigValue::Type::DOUBLE) {
                return std::get<double>(defaultIt->second.value_);
            }
        }
        return std::nullopt;
    }
    if (it->second.getType() == ConfigValue::Type::DOUBLE) {
        return std::get<double>(it->second.value_);
    }
    return std::nullopt;
}

std::optional<bool> HierarchicalConfig::getBool(const std::string& key) const {
    auto it = config_.find(key);
    if (it == config_.end()) {
        // Check defaults
        auto defaultIt = defaults_.find(key);
        if (defaultIt != defaults_.end()) {
            if (defaultIt->second.getType() == ConfigValue::Type::BOOLEAN) {
                return std::get<bool>(defaultIt->second.value_);
            }
        }
        return std::nullopt;
    }
    if (it->second.getType() == ConfigValue::Type::BOOLEAN) {
        return std::get<bool>(it->second.value_);
    }
    return std::nullopt;
}

void HierarchicalConfig::set(const std::string& key, const std::string& value) {
    config_[key] = ConfigValue(value);
}

void HierarchicalConfig::set(const std::string& key, int64_t value) {
    config_[key] = ConfigValue(value);
}

void HierarchicalConfig::set(const std::string& key, double value) {
    config_[key] = ConfigValue(value);
}

void HierarchicalConfig::set(const std::string& key, bool value) {
    config_[key] = ConfigValue(value);
}

std::vector<std::string> HierarchicalConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(config_.size());
    for (const auto& pair : config_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string HierarchicalConfig::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << config_.size() << " values):\n";
    for (const auto& pair : config_) {
        oss << "  " << pair.first << ": " << pair.second.toStringRepresentation() << "\n";
    }
    return oss.str();
}

bool HierarchicalConfig::validate() const {
    validationErrors_.clear();
    
    // Validate required fields
    if (!has("neuron_count")) {
        validationErrors_.push_back("Missing required field: neuron_count");
    }
    
    if (!has("region_count")) {
        validationErrors_.push_back("Missing required field: region_count");
    }
    
    // Validate field types
    auto neuronCountOpt = getInt("neuron_count");
    if (neuronCountOpt && *neuronCountOpt <= 0) {
        validationErrors_.push_back("Invalid neuron_count: must be positive");
    }
    
    auto regionCountOpt = getInt("region_count");
    if (regionCountOpt && *regionCountOpt <= 0) {
        validationErrors_.push_back("Invalid region_count: must be positive");
    }
    
    auto connectionProbabilityOpt = getDouble("connection_probability");
    if (connectionProbabilityOpt && (*connectionProbabilityOpt < 0.0 || *connectionProbabilityOpt > 1.0)) {
        validationErrors_.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
    }
    
    return validationErrors_.empty();
}

void HierarchicalConfig::clear() {
    config_.clear();
    validationErrors_.clear();
}

void HierarchicalConfig::setNested(const std::string& path, const ConfigValue& value) {
    std::string normalizedPath = normalizePath(path);
    std::string parentPath = getParentPath(normalizedPath);
    std::string key = getKeyFromPath(normalizedPath);
    
    if (parentPath.empty()) {
        config_[key] = value;
    } else {
        // Create parent configuration if it doesn't exist
        auto parentIt = config_.find(parentPath);
        if (parentIt == config_.end()) {
            config_[parentPath] = ConfigValue(); // Create empty parent
            parentIt = config_.find(parentPath);
        }
        // Note: This assumes parent is an object, which is a limitation
        // For a full implementation, we'd need to handle nested ConfigValue objects
    }
}

std::optional<ConfigValue> HierarchicalConfig::getNested(const std::string& path) const {
    std::string normalizedPath = normalizePath(path);
    auto it = config_.find(normalizedPath);
    if (it != config_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool HierarchicalConfig::hasNested(const std::string& path) const {
    std::string normalizedPath = normalizePath(path);
    return config_.find(normalizedPath) != config_.end();
}

void HierarchicalConfig::removeNested(const std::string& path) {
    std::string normalizedPath = normalizePath(path);
    config_.erase(normalizedPath);
}

std::vector<std::string> HierarchicalConfig::getNestedKeys(const std::string& path) const {
    std::string normalizedPath = normalizePath(path);
    std::vector<std::string> keys;
    
    for (const auto& pair : config_) {
        if (pair.first == normalizedPath || 
            (normalizedPath.empty() ? true : 
             pair.first.find(normalizedPath + ".") == 0)) {
            keys.push_back(pair.first);
        }
    }
    
    std::sort(keys.begin(), keys.end());
    return keys;
}

void HierarchicalConfig::merge(const HierarchicalConfig& other) {
    for (const auto& pair : other.config_) {
        config_[pair.first] = pair.second;
    }
}

void HierarchicalConfig::mergeFromFile(const std::string& filepath) {
    HierarchicalConfig other;
    if (other.loadFromFile(filepath)) {
        merge(other);
    }
}

bool HierarchicalConfig::validate(const std::string& path) const {
    return validate(); // For now, just validate all fields
}

std::vector<std::string> HierarchicalConfig::getValidationErrors(const std::string& path) const {
    return validationErrors_;
}

void HierarchicalConfig::setDefault(const std::string& key, const std::string& value) {
    defaults_[key] = ConfigValue(value);
}

void HierarchicalConfig::setDefault(const std::string& key, int64_t value) {
    defaults_[key] = ConfigValue(value);
}

void HierarchicalConfig::setDefault(const std::string& key, double value) {
    defaults_[key] = ConfigValue(value);
}

void HierarchicalConfig::setDefault(const std::string& key, bool value) {
    defaults_[key] = ConfigValue(value);
}

bool HierarchicalConfig::saveToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJson();
    file.close();
    return true;
}

bool HierarchicalConfig::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return fromJson(buffer.str());
}

bool HierarchicalConfig::saveToBinary(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple binary format: key value pairs
    for (const auto& pair : config_) {
        // Write key length and key
        uint32_t keyLength = static_cast<uint32_t>(pair.first.length());
        file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        file.write(pair.first.c_str(), keyLength);
        
        // Write value type and value
        ConfigValue::Type type = pair.second.getType();
        file.write(reinterpret_cast<const char*>(&type), sizeof(type));
        
        switch (type) {
            case ConfigValue::Type::STRING:
            case ConfigValue::Type::ARRAY:
            case ConfigValue::Type::OBJECT:
                // For complex types, we'd need to serialize them
                break;
            case ConfigValue::Type::INTEGER:
                file.write(reinterpret_cast<const char*>(&pair.second.toInt()), sizeof(int64_t));
                break;
            case ConfigValue::Type::DOUBLE:
                file.write(reinterpret_cast<const char*>(&pair.second.toDouble()), sizeof(double));
                break;
            case ConfigValue::Type::BOOLEAN:
                file.write(reinterpret_cast<const char*>(&pair.second.toBool()), 1);
                break;
        }
    }
    
    file.close();
    return true;
}

bool HierarchicalConfig::loadFromBinary(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    config_.clear();
    
    while (file.peek() != std::ifstream::traits_type::eof()) {
        // Read key length and key
        uint32_t keyLength;
        file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
        std::string key(keyLength, ' ');
        file.read(&key[0], keyLength);
        
        // Read value type
        ConfigValue::Type type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));
        
        // Note: This is a simplified binary format
        // A full implementation would need to properly deserialize values
    }
    
    file.close();
    return true;
}

std::string HierarchicalConfig::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    for (auto it = config_.begin(); it != config_.end(); ++it) {
        if (it != config_.begin()) oss << ",\n";
        oss << "  \"" << it->first << "\": " << it->second.toStringRepresentation();
    }
    oss << "\n}";
    return oss.str();
}

bool HierarchicalConfig::fromJson(const std::string& json) {
    // Simple JSON parsing (for demonstration)
    // A real implementation would use a proper JSON parser
    config_.clear();
    
    // For now, just clear and return true
    return true;
}

std::string HierarchicalConfig::normalizePath(const std::string& path) const {
    std::string result = path;
    // Remove leading/trailing whitespace and dots
    result.erase(0, result.find_first_not_of(" ."));
    result.erase(result.find_last_not_of(" .") + 1);
    
    // Replace multiple dots with single dot
    size_t pos;
    while ((pos = result.find("..")) != std::string::npos) {
        if (pos > 0) {
            result.erase(pos - 1, 2);
        } else {
            result.erase(0, 2);
        }
    }
    
    return result;
}

std::string HierarchicalConfig::getParentPath(const std::string& path) const {
    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos) {
        return "";
    }
    return path.substr(0, lastDot);
}

std::string HierarchicalConfig::getKeyFromPath(const std::string& path) const {
    size_t lastDot = path.find_last_of('.');
    if (lastDot == std::string::npos) {
        return path;
    }
    return path.substr(lastDot + 1);
}

bool HierarchicalConfig::validateValue(const ConfigValue& value) const {
    switch (value.getType()) {
        case ConfigValue::Type::STRING:
            return !value.toString().empty();
        case ConfigValue::Type::INTEGER:
            return value.toInt() > 0;
        case ConfigValue::Type::DOUBLE:
            return value.toDouble() >= 0.0 && value.toDouble() <= 1.0;
        case ConfigValue::Type::BOOLEAN:
            return true;
        case ConfigValue::Type::ARRAY:
            return true; // Arrays can be empty
        case ConfigValue::Type::OBJECT:
            return true; // Objects can be empty
        default:
            return false;
    }
}

std::string HierarchicalConfig::valueToString(const ConfigValue& value) const {
    return value.toString();
}

} // namespace nlm
