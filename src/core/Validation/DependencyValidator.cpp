#include "DependencyValidator.hpp"
#include <algorithm>

namespace nlm {

bool DependencyValidator::hasRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    return true;
}

bool DependencyValidator::checkConditionalDependency(const std::string& key, const Config& config,
                                                     const std::string& conditionKey, 
                                                     const std::string& conditionValue) const {
    if (!config.has(conditionKey)) {
        return false; // Condition key doesn't exist
    }

    try {
        auto conditionVal = config.get<std::string>(conditionKey);
        if (!conditionVal) {
            return false;
        }

        return *conditionVal == conditionValue;
    } catch (...) {}

    return false;
}

bool DependencyValidator::isMutuallyExclusive(const std::string& key1, const std::string& key2,
                                             const Config& config) const {
    // Check if both keys exist and have non-default values
    bool key1Exists = config.has(key1);
    bool key2Exists = config.has(key2);

    if (!key1Exists || !key2Exists) {
        return true; // At least one doesn't exist - not mutually exclusive
    }

    // For now, return true - actual mutual exclusivity logic would depend on specific requirements
    return true;
}

bool DependencyValidator::checkConditionalRequirement(const std::string& requiredKey,
                                                     const std::string& conditionKey,
                                                     const std::string& conditionValue,
                                                     const Config& config) const {
    // Check if condition is met
    if (!checkConditionalDependency("", config, conditionKey, conditionValue)) {
        return true; // Condition not met - no requirement
    }

    // Condition is met - check if required key exists
    return config.has(requiredKey);
}

} // namespace nlm
