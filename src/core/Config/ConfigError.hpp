// This file exists to maintain backward compatibility
#pragma once

#include "ConfigValidationUtils.hpp"

namespace nlm {
    using ConfigValidationUtils = ConfigValidationUtils;
}

// For backward compatibility
#include "ConfigValidator.hpp"
#include "ConfigSchema.hpp"
#include "ConfigWatcher.hpp"
#include "ConfigMigration.hpp"
#include "ConfigHelpSystem.hpp"
#include "ConfigDocumentation.hpp"
