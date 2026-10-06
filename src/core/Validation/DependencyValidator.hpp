#pragma once

#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "Config.hpp"

namespace nlm {

/**
 * @class DependencyValidator
 * @brief Validator for key dependencies and relationships
 * 
 * Handles validation of:
 * - Required key dependencies
 * - Conditional dependencies
 * - Mutually exclusive keys
 * - Optional key requirements
 */
class DependencyValidator {
public:
    DependencyValidator() = default;

    // Required dependencies
    bool hasRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) const;

    // Conditional dependencies
    bool checkConditionalDependency(const std::string& key, const Config& config,
                                   const std::string& conditionKey, 
                                   const std::string& conditionValue) const;

    // Mutual exclusivity
    bool isMutuallyExclusive(const std::string& key1, const std::string& key2,
                           const Config& config) const;

    // Conditional requirements
    bool checkConditionalRequirement(const std::string& requiredKey,
                                    const std::string& conditionKey,
                                    const std::string& conditionValue,
                                    const Config& config) const;
};

/**
 * @class ConsistencyValidator
 * @brief Validator for consistency and conflict checking
 * 
 * Handles validation of:
 * - Conflicting value combinations
 * - Logical consistency
 * - Business rule validation
 * - Value relationship validation
 */
class ConsistencyValidator {
public:
    ConsistencyValidator() = default;

    // Conflict validation
    bool checkConflictingValues(const std::string& key1, const std::string& key2,
                               const Config& config) const;

    // Logical consistency
    bool checkLogicalConsistency(const std::string& key, const ConfigValue& value,
                                const Config& config) const;

    // Business rule validation
    bool checkBusinessRule(const std::string& key, const ConfigValue& value,
                          const Config& config,
                          const std::function<bool(const Config&, const ConfigValue&)> rule) const;

    // Relationship validation
    bool checkRelationship(const std::string& key1, const std::string& key2,
                          const std::function<bool(const ConfigValue&, const ConfigValue&)> relationship,
                          const Config& config) const;
};

} // namespace nlm
