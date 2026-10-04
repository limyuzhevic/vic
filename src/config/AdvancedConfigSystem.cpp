#include "config/AdvancedConfigSystem.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// AdvancedConfigSystem implementation
std::string AdvancedConfigSystem::getConstraintString(const AdvancedConfigParameter& param) const {
    std::stringstream ss;
    
    if (param.type == "int" || param.type == "float" || param.type == "double") {
        if (param.constraints.min_value != "") {
            ss << "min: " << param.constraints.min_value;
        }
        if (param.constraints.max_value != "") {
            if (!ss.str().empty()) ss << ", ";
            ss << "max: " << param.constraints.max_value;
        }
    } else if (param.type == "string") {
        if (param.constraints.min_length > 0) {
            ss << "min len: " << param.constraints.min_length;
        }
        if (param.constraints.max_length > 0) {
            if (!ss.str().empty()) ss << ", ";
            ss << "max len: " << param.constraints.max_length;
        }
        if (!param.constraints.allowed_values.empty()) {
            if (!ss.str().empty()) ss << ", ";
            ss << "allowed: " << formatAllowedValues(param.constraints.allowed_values);
        }
    }
    
    if (param.constraints.dependency != "") {
        if (!ss.str().empty()) ss << ", ";
        ss << "depends on: " << param.constraints.dependency;
    }
    
    return ss.str();
}

std::string AdvancedConfigSystem::formatAllowedValues(const std::vector<std::string>& values) const {
    std::stringstream ss;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << values[i] << "\"";
    }
    return ss.str();
}

} // namespace nlm