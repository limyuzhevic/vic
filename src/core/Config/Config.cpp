#include "Config.hpp"
#include "ValidationFramework.hpp"
#include "EnhancedConfiguration.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <mutex>

namespace nlm {

// Validation Framework Implementation
ValidationErrorCollector::ValidationErrorCollector() {
    std::lock_guard<std::mutex> lock(mutex);
    errors.clear();
}

bool ValidationErrorCollector::hasErrors() const {
    std::lock_guard<std::mutex> lock(mutex);
    return !errors.empty();
}

std::vector<ValidationError> ValidationErrorCollector::getErrors() const {
    std::lock_guard<std::mutex> lock(mutex);
    return errors;
}

void ValidationErrorCollector::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    errors.clear();
}

std::string ValidationErrorCollector::getReport() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::ostringstream oss;
    oss << "Configuration Validation Report\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n";
    oss << "Total errors: " << errors.size() << "\n";

    for (const auto& error : errors) {
        oss << "\n[Severity: " << static_cast<int>(error.severity) << "] " << error.message;
        if (!error.context.empty()) {
            oss << " (Context: " << error.context << ")";
        }
        if (!error.suggestion.empty()) {
            oss << "\n   Suggestion: " << error.suggestion;
        }
        oss << "\n   Time: " << getTimestamp(error.timestamp);
    }

    return oss.str();
}

std::string ValidationErrorCollector::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return std::string(buf);
}

std::string ValidationErrorCollector::getTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return std::string(buf);
}

// Configuration Schema Implementation
void ConfigurationSchema::addParameter(const ParameterMetadata& param) {
    parameters.push_back(param);
    parameterMap[param.name] = param;
}

void ConfigurationSchema::addValidationRule(const ValidationRule& rule) {
    validationRules.push_back(rule);
}

void ConfigurationSchema::addSecurityRule(const ValidationRule& rule) {
    securityRules.push_back(rule);
}

ParameterMetadata ConfigurationSchema::getParameter(const std::string& name) const {
    auto it = parameterMap.find(name);
    if (it != parameterMap.end()) {
        return it->second;
    }
    throw ConfigException("Unknown parameter: " + name, "getParameter");
}

bool ConfigurationSchema::hasParameter(const std::string& name) const {
    return parameterMap.find(name) != parameterMap.end();
}

std::vector<std::string> ConfigurationSchema::getParametersInCategory(const std::string& category) const {
    std::vector<std::string> result;
    for (const auto& param : parameters) {
        if (std::find(param.categories.begin(), param.categories.end(), category) != param.categories.end()) {
            result.push_back(param.name);
        }
    }
    return result;
}

// Enhanced Configuration Manager Implementation
void EnhancedConfigurationManager::initializeEnhancedConfigSystem() {
    enhancedConfigSystem = new EnhancedConfigSystem(config);
}

void EnhancedConfigurationManager::cleanup() {
    if (enhancedConfigSystem) {
        delete enhancedConfigSystem;
        enhancedConfigSystem = nullptr;
    }
    globalErrorCollector.clear();
}

// Default Config Implementations
const std::vector<ConfigEntry>& Config::getEntries() const {
    return entries;
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != entries.end()) {
        return it->value;
    }
    throw ConfigException("Key not found: " + key, "getValue");
}

void Config::setValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKey(key);
    validateValue(value);

    auto it = std::find_if(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    if (it != entries.end()) {
        it->value = value;
        it->source = source;
        it->lastModified = std::chrono::system_clock::now();
    } else {
        entries.emplace_back(key, value, source);
    }
}

void Config::removeValue(const std::string& key) {
    auto it = std::remove_if(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    entries.erase(it, entries.end());
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return std::string(buf);
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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
}