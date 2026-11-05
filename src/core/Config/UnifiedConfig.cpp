// UnifiedConfig.cpp - Implementation of the unified configuration system

#include "core/Config/UnifiedConfig.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

// Implementation of Config class methods

bool Config::validateValue(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    // Basic validation - can be extended for specific parameter types
    if (std::holds_alternative<bool>(newValue)) {
        return true; // Boolean values are always valid
    } else if (std::holds_alternative<int64_t>(newValue)) {
        int64_t val = std::get<int64_t>(newValue);
        if (key == "neuron_count" && (val < 1 || val > 100000)) return false;
        if (key == "region_count" && (val < 1 || val > 1000)) return false;
        return true;
    } else if (std::holds_alternative<double>(newValue)) {
        double val = std::get<double>(newValue);
        if (key == "simulation_timestep" && (val <= 0.0 || val > 1.0)) return false;
        if (key == "connection_probability" && (val < 0.0 || val > 1.0)) return false;
        return true;
    } else if (std::holds_alternative<float>(newValue)) {
        float val = std::get<float>(newValue);
        if (key == "random_seed" && val < 0.0f) return false;
        return true;
    }
    return true;
}

void Config::exportValue(const ConfigValue& value, std::stringstream& ss) const {
    if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        ss << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<int64_t>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
}

void Config::exportJSON(const ConfigValue& value, std::stringstream& ss) const {
    if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        ss << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<int64_t>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
}

void Config::exportYAML(const ConfigValue& value, std::stringstream& ss) const {
    if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        ss << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<int64_t>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
}

} // namespace nlm