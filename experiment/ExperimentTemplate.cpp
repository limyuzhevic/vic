#pragma once

#include "ExperimentTemplate.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

ExperimentTemplate::ExperimentTemplate(const std::string& name) : name(name) {
    metadata.createdAt = std::chrono::system_clock::now();
}

ExperimentTemplate::~ExperimentTemplate() = default;

bool ExperimentTemplate::addParameter(const std::string& key, const std::string& defaultValue,
                                      const std::vector<std::string>& allowedValues) {
    Parameter param;
    param.key = key;
    param.defaultValue = defaultValue;
    param.allowedValues = allowedValues;
    
    parameters[key] = param;
    return true;
}

bool ExperimentTemplate::addParameter(const std::string& key, double defaultValue,
                                      double minValue, double maxValue) {
    Parameter param;
    param.key = key;
    param.defaultValue = std::to_string(defaultValue);
    param.minValue = minValue;
    param.maxValue = maxValue;
    
    parameters[key] = param;
    return true;
}

const std::map<std::string, ExperimentTemplate::Parameter>& ExperimentTemplate::getParameters() const {
    return parameters;
}

std::map<std::string, std::string> ExperimentTemplate::getDefaultParameters() const {
    std::map<std::string, std::string> defaults;
    
    for (const auto& pair : parameters) {
        defaults[pair.first] = pair.second.defaultValue;
    }
    
    return defaults;
}

std::string ExperimentTemplate::getDefaultParameter(const std::string& key) const {
    auto it = parameters.find(key);
    if (it != parameters.end()) {
        return it->second.defaultValue;
    }
    return "";
}

bool ExperimentTemplate::validateParameter(const std::string& key, const std::string& value) const {
    auto it = parameters.find(key);
    if (it == parameters.end()) {
        return false; // Parameter not defined
    }
    
    const Parameter& param = it->second;
    
    // Check if value matches allowed values
    if (!param.allowedValues.empty()) {
        if (std::find(param.allowedValues.begin(), param.allowedValues.end(), value) == param.allowedValues.end()) {
            return false;
        }
    }
    
    // Check numeric bounds if applicable
    if (param.minValue != std::numeric_limits<double>::max() && 
        std::stod(value) < param.minValue) {
        return false;
    }
    
    if (param.maxValue != std::numeric_limits<double>::min() && 
        std::stod(value) > param.maxValue) {
        return false;
    }
    
    return true;
}

ExperimentConfig ExperimentTemplate::createConfig(const std::map<std::string, std::string>& overrides) const {
    ExperimentConfig config;
    config.name = name;
    config.description = metadata.description;
    
    // Set default parameters
    for (const auto& pair : parameters) {
        config.parameters[pair.first] = pair.second.defaultValue;
    }
    
    // Apply overrides
    for (const auto& override : overrides) {
        if (parameters.find(override.first) != parameters.end()) {
            config.parameters[override.first] = override.second;
        }
    }
    
    // Set template metadata
    config.templateName = name;
    
    return config;
}

std::string ExperimentTemplate::getTemplatePath() const {
    return metadata.filePath;
}

void ExperimentTemplate::setTemplatePath(const std::string& path) {
    metadata.filePath = path;
}

std::chrono::system_clock::time_point ExperimentTemplate::getCreatedAt() const {
    return metadata.createdAt;
}

void ExperimentTemplate::setCreatedAt(const std::chrono::system_clock::time_point& time) {
    metadata.createdAt = time;
}

const std::string& ExperimentTemplate::getDescription() const {
    return metadata.description;
}

void ExperimentTemplate::setDescription(const std::string& description) {
    metadata.description = description;
}

const std::string& ExperimentTemplate::getVersion() const {
    return metadata.version;
}

void ExperimentTemplate::setVersion(const std::string& version) {
    metadata.version = version;
}

const ExperimentTemplate::Metadata& ExperimentTemplate::getMetadata() const {
    return metadata;
}

void ExperimentTemplate::setMetadata(const Metadata& metadata) {
    this->metadata = metadata;
}

std::string ExperimentTemplate::generateTemplateJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << name << "\",\n";
    ss << "  \"version\": \"" << metadata.version << "\",\n";
    ss << "  \"description\": \"" << metadata.description << "\",\n";
    ss << "  \"filePath\": \"" << metadata.filePath << "\",\n";
    ss << "  \"parameters\": [\n";
    
    bool first = true;
    for (const auto& pair : parameters) {
        if (!first) ss << ",\n";
        first = false;
        
        ss << "    {\n";
        ss << "      \"key\": \"" << pair.first << "\",\n";
        ss << "      \"defaultValue\": \"" << pair.second.defaultValue << "\",\n";
        
        if (!pair.second.allowedValues.empty()) {
            ss << "      \"allowedValues\": [\n";
            for (size_t i = 0; i < pair.second.allowedValues.size(); ++i) {
                ss << "        \"" << pair.second.allowedValues[i] << "\"";
                if (i < pair.second.allowedValues.size() - 1) ss << ",\n";
            }
            ss << "\n      ]\n";
        }
        
        if (pair.second.minValue != std::numeric_limits<double>::max()) {
            ss << "      \"minValue\": " << pair.second.minValue << ",\n";
        }
        
        if (pair.second.maxValue != std::numeric_limits<double>::min()) {
            ss << "      \"maxValue\": " << pair.second.maxValue << "\n";
        }
        
        ss << "    }";
    }
    
    ss << "\n  ],\n";
    ss << "  \"tags\": [\n";
    
    for (size_t i = 0; i < metadata.tags.size(); ++i) {
        ss << "    \"" << metadata.tags[i] << "\"";
        if (i < metadata.tags.size() - 1) ss << ",\n";
    }
    
    ss << "\n  ]\n";
    ss << "}\n";
    
    return ss.str();
}

bool ExperimentTemplate::loadFromFile(const std::string& filepath) {
    // TODO: Implement file loading
    return false;
}

bool ExperimentTemplate::saveToFile(const std::string& filepath) const {
    // TODO: Implement file saving
    return false;
}

bool ExperimentTemplate::validate() const {
    if (name.empty()) {
        return false;
    }
    
    if (metadata.description.empty()) {
        return false;
    }
    
    if (parameters.empty()) {
        return false;
    }
    
    for (const auto& pair : parameters) {
        if (pair.first.empty()) {
            return false;
        }
        
        if (pair.second.defaultValue.empty()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ExperimentTemplate::getTags() const {
    return metadata.tags;
}

void ExperimentTemplate::addTag(const std::string& tag) {
    metadata.tags.push_back(tag);
}

} // namespace nlm
