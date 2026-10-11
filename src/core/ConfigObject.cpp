// Implementation of EnhancedBrain
#include "EnhancedBrain.hpp"
#include "EnhancedWorld.hpp"
#include "ConfigObject.hpp"
#include "EventSystem.hpp"
#include "DecoratorSupport.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

// ConfigObject implementation
ConfigObject::ConfigObject(const std::string& configFile) {
    if (!configFile.empty()) {
        loadFromFile(configFile);
    }
}

ConfigObject& ConfigObject::set(const std::string& key, int value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, double value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, float value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, bool value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, const std::string& value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, const std::vector<int>& value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, const std::vector<double>& value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, const std::vector<float>& value) {
    configData_[key] = value;
    return *this;
}

ConfigObject& ConfigObject::set(const std::string& key, const std::vector<std::string>& value) {
    configData_[key] = value;
    return *this;
}

template<typename T>
ConfigObject& ConfigObject::set(const std::string& key, const T& value) {
    configData_[key] = value;
    return *this;
}

int ConfigObject::getInt(const std::string& key, int defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(int)) {
            return std::any_cast<int>(it->second);
        } else if (it->second.type() == typeid(double)) {
            return static_cast<int>(std::any_cast<double>(it->second));
        } else if (it->second.type() == typeid(float)) {
            return static_cast<int>(std::any_cast<float>(it->second));
        }
    }
    return defaultValue;
}

double ConfigObject::getDouble(const std::string& key, double defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(double)) {
            return std::any_cast<double>(it->second);
        } else if (it->second.type() == typeid(float)) {
            return static_cast<double>(std::any_cast<float>(it->second));
        } else if (it->second.type() == typeid(int)) {
            return static_cast<double>(std::any_cast<int>(it->second));
        }
    }
    return defaultValue;
}

float ConfigObject::getFloat(const std::string& key, float defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(float)) {
            return std::any_cast<float>(it->second);
        } else if (it->second.type() == typeid(double)) {
            return static_cast<float>(std::any_cast<double>(it->second));
        } else if (it->second.type() == typeid(int)) {
            return static_cast<float>(std::any_cast<int>(it->second));
        }
    }
    return defaultValue;
}

bool ConfigObject::getBool(const std::string& key, bool defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(bool)) {
            return std::any_cast<bool>(it->second);
        } else if (it->second.type() == typeid(int)) {
            return static_cast<bool>(std::any_cast<int>(it->second));
        }
    }
    return defaultValue;
}

std::string ConfigObject::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(std::string)) {
            return std::any_cast<std::string>(it->second);
        }
    }
    return defaultValue;
}

std::vector<int> ConfigObject::getIntVector(const std::string& key) const {
    auto it = configData_.find(key);
    if (it != configData_.end() && it->second.type() == typeid(std::vector<int>)) {
        return std::any_cast<std::vector<int>>(it->second);
    }
    return std::vector<int>();
}

std::vector<double> ConfigObject::getDoubleVector(const std::string& key) const {
    auto it = configData_.find(key);
    if (it != configData_.end() && it->second.type() == typeid(std::vector<double>)) {
        return std::any_cast<std::vector<double>>(it->second);
    }
    return std::vector<double>();
}

std::vector<float> ConfigObject::getFloatVector(const std::string& key) const {
    auto it = configData_.find(key);
    if (it != configData_.end() && it->second.type() == typeid(std::vector<float>)) {
        return std::any_cast<std::vector<float>>(it->second);
    }
    return std::vector<float>();
}

std::vector<std::string> ConfigObject::getStringVector(const std::string& key) const {
    auto it = configData_.find(key);
    if (it != configData_.end() && it->second.type() == typeid(std::vector<std::string>)) {
        return std::any_cast<std::vector<std::string>>(it->second);
    }
    return std::vector<std::string>();
}

template<typename T>
T ConfigObject::get(const std::string& key, const T& defaultValue) const {
    auto it = configData_.find(key);
    if (it != configData_.end()) {
        if (it->second.type() == typeid(T)) {
            return std::any_cast<T>(it->second);
        }
    }
    return defaultValue;
}

bool ConfigObject::has(const std::string& key) const {
    return configData_.find(key) != configData_.end();
}

void ConfigObject::remove(const std::string& key) {
    configData_.erase(key);
}

std::vector<std::string> ConfigObject::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : configData_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ConfigObject::toJSON() const {
    // Convert config to JSON (simplified implementation)
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& pair : configData_) {
        if (!first) ss << ", ";
        first = false;
        ss << "\"" << pair.first << "\": ";
        
        if (pair.second.type() == typeid(int)) {
            ss << std::any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(double)) {
            ss << std::any_cast<double>(pair.second);
        } else if (pair.second.type() == typeid(float)) {
            ss << std::any_cast<float>(pair.second);
        } else if (pair.second.type() == typeid(bool)) {
            ss << (std::any_cast<bool>(pair.second) ? "true" : "false");
        } else if (pair.second.type() == typeid(std::string)) {
            ss << "\"" << std::any_cast<std::string>(pair.second) << "\"";
        }
    }
    ss << "}";
    return ss.str();
}

void ConfigObject::loadFromJSON(const std::string& jsonStr) {
    // Parse JSON and load into config
    // Simplified implementation
    configData_.clear();
    // In a real implementation, you would use a JSON parser like nlohmann::json
}

bool ConfigObject::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write config to file in JSON format
        file << toJSON();
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to save config to file: " << e.what() << std::endl;
        return false;
    }
}

void ConfigObject::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        loadFromJSON(content);
        
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Failed to load config from file: " << e.what() << std::endl;
    }
}

ConfigObject ConfigObject::operator[](const std::string& key) const {
    ConfigObject result;
    if (has(key)) {
        // Create a copy of the value
        result.configData_[key] = configData_.at(key);
    }
    return result;
}

size_t ConfigObject::size() const {
    return configData_.size();
}

bool ConfigObject::empty() const {
    return configData_.empty();
}

void ConfigObject::clear() {
    configData_.clear();
}

std::string ConfigObject::summary() const {
    std::stringstream ss;
    ss << "ConfigObject with " << size() << " entries:" << std::endl;
    for (const auto& pair : configData_) {
        ss << "  " << pair.first << ": ";
        
        if (pair.second.type() == typeid(int)) {
            ss << std::any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(double)) {
            ss << std::any_cast<double>(pair.second);
        } else if (pair.second.type() == typeid(float)) {
            ss << std::any_cast<float>(pair.second);
        } else if (pair.second.type() == typeid(bool)) {
            ss << (std::any_cast<bool>(pair.second) ? "true" : "false");
        } else if (pair.second.type() == typeid(std::string)) {
            ss << std::any_cast<std::string>(pair.second);
        }
        ss << std::endl;
    }
    return ss.str();
}

// EventSystem implementation
void EventSystem::emit(const std::string& eventName, const std::any& data) {
    auto it = eventListeners_.find(eventName);
    if (it != eventListeners_.end()) {
        for (const auto& callback : it->second) {
            callback(data);
        }
    }
}

void EventSystem::on(const std::string& eventName, const EventCallback& callback) {
    eventListeners_[eventName].push_back(callback);
}

void EventSystem::off(const std::string& eventName, const EventCallback& callback) {
    auto it = eventListeners_.find(eventName);
    if (it != eventListeners_.end()) {
        it->second.erase(std::remove(it->second.begin(), it->second.end(), callback), it->second.end());
    }
}

std::vector<std::string> EventSystem::getEventNames() const {
    std::vector<std::string> names;
    for (const auto& pair : eventListeners_) {
        names.push_back(pair.first);
    }
    return names;
}

// DecoratorSupport implementation
template<typename Func>
auto DecoratorSupport::logged(const std::string& name, Func&& func, const DecoratorOptions& options) {
    return [name, func, options](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
        if (options.enableLogging) {
            std::cout << "[" << options.logLevel << "] " << name << " called" << std::endl;
        }
        return func(std::forward<decltype(args)>(args)...);
    };
}

template<typename Func>
auto DecoratorSupport::timed(const std::string& name, Func&& func, const DecoratorOptions& options) {
    return [name, func, options](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func(std::forward<decltype(args)>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        if (options.enableTiming) {
            std::cout << name << " took " << duration << " ms" << std::endl;
        }
        
        return result;
    };
}

template<typename Func>
auto DecoratorSupport::metered(const std::string& name, Func&& func, const DecoratorOptions& options) {
    return [name, func, options](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
        static std::atomic<size_t> callCount{0};
        callCount++;
        
        auto result = func(std::forward<decltype(args)>(args)...);
        
        if (options.enableMetrics) {
            std::cout << options.metricsPrefix << name << " calls: " << callCount << std::endl;
        }
        
        return result;
    };
}

template<typename Func>
auto DecoratorSupport::profiled(Func&& func, const DecoratorOptions& options) {
    return [func, options](auto&&... args) -> decltype(func(std::forward<decltype(args)>(args)...)) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func(std::forward<decltype(args)>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        if (options.enableProfiling) {
            std::cout << "Function took " << duration << " ms (profiling enabled)" << std::endl;
        }
        
        return result;
    };
}

} // namespace nlm