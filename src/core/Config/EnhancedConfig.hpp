// NLM (熙然) - Neural Learning Machine
// Configuration Utilities and Validation System
//
// This file provides enhanced configuration management with:
// - Proper JSON/YAML configuration file support
// - Configuration validation and type checking
// - Configuration constraints and ranges
// - Better error handling and reporting
// - Configuration templates and defaults
// - Safe value conversion and validation

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <stdexcept>
#include <sstream>
#include <nlohmann/json.hpp>

#include "core/Config/Config.hpp"

// Forward declarations
namespace nlm {
    class IConfigValidator;
    class DefaultManager;
    class EnhancedConfig;
}

// JSON support for configuration files
namespace json = nlohmann::json;

using namespace nlm;