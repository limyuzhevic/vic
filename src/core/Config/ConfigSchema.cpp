#include "ConfigSchema.hpp"
#include "ConfigError.hpp"

namespace nlm {

bool ParameterInfo::validate(const ConfigValue& value) const {
    // Implementation will go here
    return true;
}

std::string ParameterInfo::typeToString() const {
    switch (type) {
        case ParamType::Int: return "int";
        case ParamType::Int64: return "int64_t";
        case ParamType::Double: return "double";
        case ParamType::Bool: return "bool";
        case ParamType::String: return "string";
        case ParamType::StringVector: return "string_vector";
        case ParamType::IntVector: return "int_vector";
        case ParamType::DoubleVector: return "double_vector";
        default: return "unknown";
    }
}

ParamType ParameterInfo::stringToType(const std::string& typeStr) {
    if (typeStr == "int") return ParamType::Int;
    if (typeStr == "int64_t") return ParamType::Int64;
    if (typeStr == "double") return ParamType::Double;
    if (typeStr == "bool") return ParamType::Bool;
    if (typeStr == "string") return ParamType::String;
    if (typeStr == "string_vector") return ParamType::StringVector;
    if (typeStr == "int_vector") return ParamType::IntVector;
    if (typeStr == "double_vector") return ParamType::DoubleVector;
    return ParamType::String;
}

void Schema::addParameter(const ParameterInfo& param) {
    parameters[param.key] = param;
}

bool Schema::hasParameter(const std::string& key) const {
    return parameters.find(key) != parameters.end();
}

const ParameterInfo* Schema::getParameter(const std::string& key) const {
    auto it = parameters.find(key);
    return (it != parameters.end()) ? &it->second : nullptr;
}

const ParameterInfo& Schema::getParameterRequired(const std::string& key) const {
    const ParameterInfo* param = getParameter(key);
    if (!param) {
        throw ConfigUnknownKeyError(key, "Parameter is not defined in the schema");
    }
    return *param;
}

std::vector<std::string> Schema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters.size());
    for (const auto& pair : parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<ParameterInfo> Schema::getAllParameters() const {
    std::vector<ParameterInfo> params;
    params.reserve(parameters.size());
    for (const auto& pair : parameters) {
        params.push_back(pair.second);
    }
    return params;
}

void Schema::printSchema(std::ostream& os) const {
    os << "Configuration Schema:" << std::endl;
    os << "========================" << std::endl;
    for (const auto& pair : parameters) {
        const auto& param = pair.second;
        os << "Key: " << param.key << std::endl;
        os << "  Type: " << param.typeToString() << std::endl;
        os << "  Description: " << param.description << std::endl;
        os << "  Required: " << (param.required ? "yes" : "no") << std::endl;
        os << "  Default: ";
        std::visit([&os](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                os << "\"" << arg << "\"";
            } else {
                os << arg;
            }
        }, param.defaultValue);
        os << std::endl;
        os << std::endl;
    }
}

bool Schema::validate(const std::map<std::string, ConfigValue>& configData) const {
    // Implementation will go here
    return true;
}

void Schema::setParameterWasSet(const std::string& key) {
    auto it = parameters.find(key);
    if (it != parameters.end()) {
        it->second.wasSet = true;
    }
}

bool Schema::wasParameterSet(const std::string& key) const {
    auto it = parameters.find(key);
    if (it != parameters.end()) {
        return it->second.wasSet;
    }
    return false;
}

void Schema::clear() {
    parameters.clear();
}

} // namespace nlm