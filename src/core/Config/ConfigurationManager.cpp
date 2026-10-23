// Implementation of ConfigurationManager
#include "core/Config/ConfigurationManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Helper function to convert ConfigValue to string
std::string configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
    
    return ss.str();
}

} // namespace nlm