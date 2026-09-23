#pragma once

#include "ConfigBuilder.hpp"
#include <stdexcept>

namespace nlm {

ConfigBuilder& ConfigBuilder::add(const std::string& key, ConfigSchema schema) {
    if (schemas.find(key) != schemas.end()) {
        throw ConfigError(ConfigError::ErrorType::SchemaError, 
                         "Schema with key '" + key + "' already exists");
    }
    schemas[key] = std::move(schema);
    return *this;
}

ConfigBuilder& ConfigBuilder::setDefault(const std::string& key, const std::any& value) {
    if (schemas.find(key) == schemas.end()) {
        throw ConfigError(ConfigError::ErrorType::SchemaError,
                         "Cannot set default for unknown key '" + key + "'");
    }
    defaults[key] = value;
    return *this;
}

std::unordered_map<std::string, ConfigSchema> ConfigBuilder::build() const {
    std::unordered_map<std::string, ConfigSchema> result = schemas;

    for (const auto& pair : defaults) {
        auto it = result.find(pair.first);
        if (it != result.end()) {
            it->second.defaultValue = pair.second;
        }
    }

    return result;
}

} // namespace nlm