// ConfigSchema.cpp implementation
#include "ConfigSchema.hpp"
#include <algorithm>
#include <set>

namespace nlm {

// ConfigSchema implementation details
namespace {
    ConfigType stringToConfigType(const std::string& typeStr) {
        std::string lower = typeStr;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "int" || lower == "integer") return ConfigType::Integer;
        if (lower == "int64" || lower == "int64_t") return ConfigType::Integer64;
        if (lower == "float" || lower == "double") return ConfigType::Float;
        if (lower == "bool" || lower == "boolean") return ConfigType::Boolean;
        if (lower == "string") return ConfigType::String;
        if (lower == "stringlist" || lower == "string_list") return ConfigType::StringList;
        if (lower == "intlist" || lower == "int_list") return ConfigType::IntList;
        if (lower == "floatlist" || lower == "float_list") return ConfigType::FloatList;
        
        return ConfigType::None;
    }
    
    std::string configTypeToString(ConfigType type) {
        switch (type) {
            case ConfigType::Integer: return "int";
            case ConfigType::Integer64: return "int64_t";
            case ConfigType::Float: return "double";
            case ConfigType::Boolean: return "bool";
            case ConfigType::String: return "string";
            case ConfigType::StringList: return "stringlist";
            case ConfigType::IntList: return "intlist";
            case ConfigType::FloatList: return "floatlist";
            default: return "none";
        }
    }
}

} // namespace nlm