// ConfigParser.cpp - Implementation of configuration parsing utilities

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<Config> ConfigParser::parseFile(const std::string& filepath) {
    auto config = std::make_unique<Config>();
    bool success = config->loadFromFile(filepath, false);
    if (!success) {
        spdlog::error("Failed to parse config file: {}", filepath);
        return nullptr;
    }
    return config;
}

std::unique_ptr<Config> ConfigParser::parseString(const std::string& content, const std::string& format) {
    auto config = std::make_unique<Config>();
    std::string detectedFormat = format;
    
    if (format == "auto") {
        detectedFormat = detectFormat(content);
    }
    
    bool success = false;
    if (detectedFormat == "json") {
        success = config->loadFromJson(content, false);
    } else if (detectedFormat == "yaml" || detectedFormat == "yml") {
        success = config->loadFromYaml(content, false);
    } else {
        success = config->loadFromSimpleFormat(content, false);
    }
    
    if (!success) {
        spdlog::error("Failed to parse config content (format: {})", detectedFormat);
        return nullptr;
    }
    
    return config;
}

std::unique_ptr<Config> ConfigParser::parseArgs(int argc, char** argv) {
    auto config = std::make_unique<Config>();
    bool success = config->loadFromArgs(argc, argv, false);
    if (!success) {
        spdlog::error("Failed to parse command line arguments");
        return nullptr;
    }
    return config;
}

std::unique_ptr<Config> ConfigParser::parseEnvironment(const std::map<std::string, std::string>& envVars) {
    auto config = std::make_unique<Config>();
    bool success = config->loadFromEnvironment(envVars);
    if (!success) {
        spdlog::error("Failed to parse environment variables");
        return nullptr;
    }
    return config;
}

std::string ConfigParser::serializeToJson(const Config& config) {
    return config.toJson();
}

std::string ConfigParser::serializeToYaml(const Config& config) {
    return config.toYaml();
}

std::string ConfigParser::serializeToSimple(const Config& config) {
    return config.toSimple();
}

std::string ConfigParser::detectFormat(const std::string& content) {
    if (isJson(content)) {
        return "json";
    } else if (isYaml(content)) {
        return "yaml";
    } else if (isSimpleFormat(content)) {
        return "simple";
    }
    return "unknown";
}

bool ConfigParser::isJson(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        return !json.empty();
    } catch (...) {
        return false;
    }
}

bool ConfigParser::isYaml(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        return !yaml.IsNull();
    } catch (...) {
        return false;
    }
}

bool ConfigParser::isSimpleFormat(const std::string& content) {
    // Simple format: key = value
    // Allow comments starting with # or /
    // Allow empty lines
    std::istringstream stream(content);
    std::string line;
    bool hasKeyValue = false;
    
    while (std::getline(stream, line)) {
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            hasKeyValue = true;
            // Check if both key and value are non-empty
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            if (!key.empty() && !value.empty()) {
                return true;
            }
        }
    }
    
    return hasKeyValue;
}

// Implementation of ConfigFactory methods
std::unique_ptr<Config> ConfigFactory::createDefaultConfig() {
    auto config = std::make_unique<Config>();
    spdlog::info("Created default config");
    return config;
}

std::unique_ptr<Config> ConfigFactory::createConfigFromDefaults(const std::map<std::string, ConfigValue>& defaults) {
    auto config = std::make_unique<Config>();
    
    for (const auto& [key, value] : defaults) {
        config->set(key, value, ConfigSource::Default, "Default value");
    }
    
    spdlog::info("Created config from {} defaults", defaults.size());
    return config;
}

std::unique_ptr<Config> ConfigFactory::createConfigFromFile(const std::string& filepath, bool merge) {
    auto config = std::make_unique<Config>();
    bool success = config->loadFromFile(filepath, merge);
    if (success) {
        spdlog::info("Created config from file: {}", filepath);
    } else {
        spdlog::error("Failed to create config from file: {}", filepath);
        config.reset();
    }
    return config;
}

std::unique_ptr<Config> ConfigFactory::createConfigFromArgs(int argc, char** argv, bool merge) {
    auto config = std::make_unique<Config>();
    bool success = config->loadFromArgs(argc, argv, merge);
    if (success) {
        spdlog::info("Created config from command line arguments");
    } else {
        spdlog::error("Failed to create config from command line arguments");
        config.reset();
    }
    return config;
}

void ConfigFactory::registerProfile(const ConfigProfile& profile) {
    // In a real implementation, this would store the profile in a global registry
    spdlog::info("Registered profile: {}", profile.name);
}

std::optional<ConfigProfile> ConfigFactory::getProfile(const std::string& name) {
    // In a real implementation, this would retrieve the profile from the registry
    spdlog::warn("Profile retrieval not implemented: {}", name);
    return std::nullopt;
}

bool ConfigFactory::applyProfileToConfig(const std::string& profileName, Config& config) {
    auto profile = getProfile(profileName);
    if (profile) {
        return config.applyProfile(*profile);
    }
    return false;
}

void ConfigFactory::registerTemplate(const std::string& name, const ConfigProfile& template_) {
    // In a real implementation, this would store the template in a global registry
    spdlog::info("Registered template: {}", name);
}

std::optional<ConfigProfile> ConfigFactory::getTemplate(const std::string& name) {
    // In a real implementation, this would retrieve the template from the registry
    spdlog::warn("Template retrieval not implemented: {}", name);
    return std::nullopt;
}

std::unique_ptr<Config> ConfigFactory::createFromTemplate(const std::string& templateName) {
    auto template_ = getTemplate(templateName);
    if (template_) {
        return createConfigFromDefaults(template_->defaults);
    }
    return nullptr;
}

} // namespace nlm