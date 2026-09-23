// ConfigSchema.cpp implementation
#pragma once

#include "ConfigSchema.hpp"

namespace nlm {

std::string ConfigSchema::getTypeFromAny(const std::any& value) {
    if (value.type() == typeid(int)) return Type::Integer;
    if (value.type() == typeid(int64_t)) return Type::Integer64;
    if (value.type() == typeid(double)) return Type::Double;
    if (value.type() == typeid(bool)) return Type::Boolean;
    if (value.type() == typeid(std::string)) return Type::String;
    if (value.type() == typeid(std::vector<int>)) return Type::IntVector;
    if (value.type() == typeid(std::vector<double>)) return Type::DoubleVector;
    if (value.type() == typeid(std::vector<std::string>)) return Type::StringVector;
    return Type::Unknown;
}

} // namespace nlm