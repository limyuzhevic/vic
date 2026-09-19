#include "ConfigMigration.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

void ConfigMigration::addMigrationStep(int fromVersion, int toVersion, const std::string& description,
                                       const std::function<void(std::map<std::string, std::any>&)>& migrationLogic) {
    migrationSteps.emplace_back(fromVersion, toVersion, description, migrationLogic);
}

std::vector<ConfigError> ConfigMigration::applyMigrations(std::map<std::string, std::any>& config) const {
    std::vector<ConfigError> errors;
    
    // Sort migration steps by version (ascending)
    std::vector<MigrationStep> sortedSteps = migrationSteps;
    std::sort(sortedSteps.begin(), sortedSteps.end(),
        [](const MigrationStep& a, const MigrationStep& b) { return a.version < b.version; });
    
    for (const auto& step : sortedSteps) {
        // Only apply migrations that target a version higher than current
        if (step.version > currentVersion) {
            try {
                step.migrationLogic(config);
                // Update current version after successful migration
                currentVersion = step.version;
            } catch (const std::exception& e) {
                errors.emplace_back(ConfigErrorType::MigrationError, "",
                    "", "",
                    "Migration from v" + std::to_string(step.version - 1) + 
                    " to v" + std::to_string(step.version) + ": " + e.what());
            }
        }
    }
    
    return errors;
}

} // namespace nlm