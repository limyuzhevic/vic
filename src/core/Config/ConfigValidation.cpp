#include "ConfigValidator.h"
#include "core/Config/Config.hpp"
#include <fstream>

namespace nlm {

// Global validator instance
std::unique_ptr<ConfigValidator> g_configValidator;

// Helper function to validate configuration
ValidationResult validateConfig(const std::shared_ptr<Config>& config) {
    if (!g_configValidator) {
        g_configValidator = std::make_unique<ConfigValidator>();
    }
    return g_configValidator->validate(config);
}

} // namespace nlm
