#include "ConsistencyValidator.hpp"
#include <algorithm>

namespace nlm {

bool ConsistencyValidator::checkConflictingValues(const std::string& key1, const std::string& key2,
                                                  const Config& config) const {
    if (!config.has(key1) || !config.has(key2)) {
        return true; // At least one key doesn't exist - no conflict
    }

    // Get values for both keys
    try {
        auto val1 = config.get<std::string>(key1);
        auto val2 = config.get<std::string>(key2);

        if (val1 && val2) {
            // Example conflict: keys "mode" and "algorithm" cannot both be set
            if (key1 == "mode" && key2 == "algorithm") {
                return *val1 == "fast" && *val2 == "complex";
            }
        }
    } catch (...) {}

    return false; // Potential conflict
}

bool ConsistencyValidator::checkLogicalConsistency(const std::string& key, const ConfigValue& value,
                                                   const Config& config) const {
    // Example logical consistency checks
    if (key == "learning_rate") {
        try {
            if (std::holds_alternative<double>(value)) {
                double lr = std::get<double>(value);
                return lr >= 0.0 && lr <= 1.0;
            }
        } catch (...) {}
    } else if (key == "batch_size") {
        try {
            if (std::holds_alternative<int>(value)) {
                int bs = std::get<int>(value);
                return bs > 0 && bs <= 1000;
            }
        } catch (...) {}
    }

    return true; // No logical inconsistency detected
}

bool ConsistencyValidator::checkBusinessRule(const std::string& key, const ConfigValue& value,
                                             const Config& config,
                                             const std::function<bool(const Config&, const ConfigValue&)> rule) const {
    try {
        return rule(config, value);
    } catch (...) {}

    return false;
}

bool ConsistencyValidator::checkRelationship(const std::string& key1, const std::string& key2,
                                             const std::function<bool(const ConfigValue&, const ConfigValue&)> relationship,
                                             const Config& config) const {
    if (!config.has(key1) || !config.has(key2)) {
        return false; // At least one key doesn't exist
    }

    // Get values for both keys
    try {
        // We need to access the internal entries to get ConfigValue
        // This is a simplified implementation
        auto val1 = config.get<std::string>(key1);
        auto val2 = config.get<std::string>(key2);

        if (val1 && val2) {
            // Convert back to ConfigValue for relationship check
            // In a real implementation, we would need proper access
            return true; // Placeholder
        }
    } catch (...) {}

    return false;
}

} // namespace nlm
