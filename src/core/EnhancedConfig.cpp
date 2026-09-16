#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <memory>

namespace nlm {

struct EnhancedConfig::Impl {
    std::unordered_map<std::string, ConfigValue> entries;
    std::vector<ValidationRule> validationRules;
    bool isValid = false;
    std::string lastError;
    std::chrono::system_clock::time_point lastModified;
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;
EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        if (filepath.empty()) {
            throwError(ErrorCode::CONFIG_VALUE_INVALID, 
                      "Configuration file path cannot be empty", __FILE__, __LINE__, __func__);
        }
        
        std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path)) {
            throwError(ErrorCode::CONFIG_FILE_NOT_FOUND,
                      "Configuration file not found: " + filepath, __FILE__, __LINE__, __func__);
        }
        
        if (!std::filesystem::is_regular_file(path)) {
            throwError(ErrorCode::CONFIG_FILE_INVALID_FORMAT,
                      "Path is not a regular file: " + filepath, __FILE__, __LINE__, __func__);
        }
        
        if (!parseFile(filepath)) {
            throwError(ErrorCode::CONFIG_FILE_INVALID_FORMAT,
                      "Failed to parse configuration file: " + filepath, __FILE__, __LINE__, __func__);
        }
        
        if (!validate()) {
            throwError(ErrorCode::CONFIG_VALIDATION_FAILED,
                      "Configuration validation failed", __FILE__, __LINE__, __func__);
        }
        
        applyDefaults();
        pImpl->lastModified = std::chrono::system_clock::now();
        pImpl->isValid = true;
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (duration.count() > 100) {
            Logger::getGlobal().warning("Configuration loading took " + std::to_string(duration.count()) + "ms");
        }
        
        return true;
        
    } catch (const NLMError& e) {
        logException(e);
        pImpl->lastError = e.what();
        pImpl->isValid = false;
        return false;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Unexpected error loading configuration: " + std::string(e.what()));
        pImpl->lastError = "Unexpected error: " + std::string(e.what());
        pImpl->isValid = false;
        return false;
    }
}

bool EnhancedConfig::loadWithFallback(const std::vector<std::string>& filepaths) {
    std::string lastError;
    
    for (const auto& filepath : filepaths) {
        if (loadFromFile(filepath)) {
            Logger::getGlobal().info("Successfully loaded configuration from: " + filepath);
            return true;
        }
        lastError = "Failed to load " + filepath + ": " + pImpl->lastError;
    }
    
    Logger::getGlobal().error("All fallback configurations failed. Last error: " + lastError);
    pImpl->lastError = lastError;
    pImpl->isValid = false;
    return false;
}

bool EnhancedConfig::validate() const {
    bool allValid = true;
    
    for (const auto& rule : pImpl->validationRules) {
        if (!validateRule(rule)) {
            Logger::getGlobal().error("Configuration validation failed for key: " + rule.key + 
                                     " - " + rule.errorMessage);
            allValid = false;
        }
    }
    
    return allValid;
}

void EnhancedConfig::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

std::string EnhancedConfig::getSummary() const {
    std::ostringstream oss;
    oss << "=== Configuration Summary ===\n";
    oss << "Total entries: " << getKeys().size() << "\n";
    oss << "Valid: " << (isValid() ? "Yes" : "No") << "\n";
    oss << "Last error: " << (getLastError().empty() ? "None" : getLastError()) << "\n";
    oss << "Last modified: " << std::chrono::system_clock::to_time_t(pImpl->lastModified) << "\n";
    oss << "\nValues:\n";
    
    for (const auto& key : getKeys()) {
        auto value = get(key);
        oss << "  " << key << " = ";
        std::visit([&oss](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << val;
            }
        }, value.value());
        oss << "\n";
    }
    
    oss << "\nValidation Rules: " << pImpl->validationRules.size() << " rules\n";
    return oss.str();
}

bool EnhancedConfig::validateRule(const ValidationRule& rule) const {
    if (rule.required && !has(rule.key)) {
        return false;
    }
    
    if (has(rule.key)) {
        auto value = get(rule.key);
        if (!value.has_value()) {
            return false;
        }
        
        // Type validation
        if (rule.type == "int") {
            if (!std::holds_alternative<int>(value.value())) {
                return false;
            }
            // Range validation
            if (!rule.condition.empty()) {
                size_t minPos = rule.condition.find("min=");
                size_t maxPos = rule.condition.find(",max=");
                if (minPos != std::string::npos && maxPos != std::string::npos) {
                    int minVal = std::stoi(rule.condition.substr(minPos + 4, maxPos - minPos - 5));
                    int maxVal = std::stoi(rule.condition.substr(maxPos + 5));
                    int actualVal = std::get<int>(value.value());
                    if (actualVal < minVal || actualVal > maxVal) {
                        return false;
                    }
                }
            }
        } else if (rule.type == "double") {
            if (!std::holds_alternative<double>(value.value())) {
                return false;
            }
        } else if (rule.type == "string") {
            if (!std::holds_alternative<std::string>(value.value())) {
                return false;
            }
        } else if (rule.type == "bool") {
            if (!std::holds_alternative<bool>(value.value())) {
                return false;
            }
        }
    }
    
    return true;
}

void EnhancedConfig::initializeDefaults() {
    set("simulation_timestep", 0.001);
    set("neuron_count", 500);
    set("region_count", 1);
    set("connection_probability", 0.15f);
    set("random_seed", static_cast<int64_t>(42));
    set("stdp_ltp_weight", 0.02f);
    set("stdp_ltd_weight", 0.015f);
    set("stdp_tau", 20.0f);
    set("synaptogenesis_rate", 0.0001f);
    set("pruning_rate", 0.00001f);
}

void EnhancedConfig::applyDefaults() {
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001);
    }
    if (!has("neuron_count")) {
        set("neuron_count", 500);
    }
    if (!has("region_count")) {
        set("region_count", 1);
    }
    if (!has("connection_probability")) {
        set("connection_probability", 0.15f);
    }
}

bool EnhancedConfig::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
            
            setFromString(key, value);
        } else {
            Logger::getGlobal().warning("Skipping invalid line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    return true;
}

void EnhancedConfig::setFromString(const std::string& key, const std::string& value) {
    if (value.find('.') != std::string::npos) {
        try {
            double doubleVal = std::stod(value);
            set(key, doubleVal);
        } catch (const std::exception&) {
            set(key, value);
        }
    } else if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal);
    } else {
        try {
            int intVal = std::stoi(value);
            set(key, intVal);
        } catch (const std::exception&) {
            set(key, value);
        }
    }
}

std::string EnhancedConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
