#include "ConfigFactory.hpp"
#include "Config.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<Config> ConfigFactory::createConfig(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("ConfigFactory not initialized, using default config");
        return Config::create();
    }
    
    // Get configuration from params or use defaults
    bool merge = params.getParam<bool>("merge", false);
    std::string source = params.getParam<std::string>("source", "default");
    
    // Handle different config creation scenarios based on source
    if (source == "file") {
        std::string filepath = params.getParam<std::string>("filepath", "");
        if (!filepath.empty()) {
            return createConfigFromFile(filepath, merge);
        }
    } else if (source == "args") {
        int argc = params.getParam<int>("argc", 0);
        char** argv = params.getParam<char**>("argv", nullptr);
        return createConfigFromArgs(argc, argv, merge);
    }
    
    // Default config creation
    return createDefaultConfig();
}

std::unique_ptr<Config> ConfigFactory::createDefaultConfig() {
    spdlog::debug("Creating default config via ConfigFactory");
    return Config::create();
}

std::unique_ptr<Config> ConfigFactory::createConfigFromDefaults(const std::map<std::string, ConfigValue>& defaults) {
    spdlog::debug("Creating config from defaults via ConfigFactory");
    
    // Create a config with default values
    auto config = Config::create();
    for (const auto& pair : defaults) {
        config->set(pair.first, pair.second);
    }
    
    return config;
}

std::unique_ptr<Config> ConfigFactory::createConfigFromFile(const std::string& filepath, bool merge) {
    spdlog::debug("Creating config from file '{}' via ConfigFactory (merge={})", filepath, merge);
    return ConfigFactory::createConfigFromFile(filepath, merge);
}

std::unique_ptr<Config> ConfigFactory::createConfigFromArgs(int argc, char** argv, bool merge) {
    spdlog::debug("Creating config from args via ConfigFactory (merge={})", merge);
    return ConfigFactory::createConfigFromArgs(argc, argv, merge);
}

void ConfigFactory::registerProfile(const ConfigProfile& profile) {
    spdlog::debug("Registering config profile '{}'", profile.name);
    // Profile registration logic
}

std::optional<ConfigProfile> ConfigFactory::getProfile(const std::string& name) {
    spdlog::debug("Getting config profile '{}'", name);
    // Profile retrieval logic
    return std::nullopt;
}

bool ConfigFactory::applyProfileToConfig(const std::string& profileName, Config& config) {
    spdlog::debug("Applying profile '{}' to config", profileName);
    // Profile application logic
    return true;
}

void ConfigFactory::registerTemplate(const std::string& name, const ConfigProfile& template_) {
    spdlog::debug("Registering config template '{}'", name);
    // Template registration logic
}

std::optional<ConfigProfile> ConfigFactory::getTemplate(const std::string& name) {
    spdlog::debug("Getting config template '{}'", name);
    // Template retrieval logic
    return std::nullopt;
}

std::unique_ptr<Config> ConfigFactory::createFromTemplate(const std::string& templateName) {
    spdlog::debug("Creating config from template '{}'", templateName);
    // Template-based creation logic
    return Config::create();
}

std::unique_ptr<Config> ConfigFactory::acquireConfigFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!configPool_.empty()) {
        auto config = std::move(configPool_.back());
        configPool_.pop_back();
        spdlog::debug("Acquired config from pool");
        return config;
    }
    return nullptr;
}

void ConfigFactory::releaseConfigToPool(std::unique_ptr<Config> config) {
    if (config) {
        std::lock_guard<std::mutex> lock(mutex_);
        configPool_.push_back(std::move(config));
        spdlog::debug("Released config to pool");
    }
}

} // namespace nlm
