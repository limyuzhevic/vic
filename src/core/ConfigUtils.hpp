// Config utility functions

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>

namespace nlm {

// Configuration validation utilities
class ConfigValidator {
public:
    static bool validate(const Config& config);
    static std::vector<std::string> getMissingKeys(const Config& config);
    static std::vector<std::string> getInvalidValues(const Config& config);
};

// Configuration enhancement utilities
class ConfigEnhancer {
public:
    static void addDefaults(Config& config);
    static void applyDefaults(Config& config);
    static bool ensureMinValues(Config& config);
};

// Configuration file utilities
class ConfigFileUtils {
public:
    static std::vector<std::string> getAvailableConfigs();
    static std::string getConfigPath(const std::string& name);
    static bool validateConfigFile(const std::string& filepath);
};