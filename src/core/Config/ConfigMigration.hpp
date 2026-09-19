#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <memory>
#include "ConfigSchema.hpp"

namespace nlm {

// Configuration migration support
struct MigrationStep {
    int version;
    std::string description;
    std::function<void(std::map<std::string, std::any>&)> migrationLogic;
    
    MigrationStep(int ver, const std::string& desc, const std::function<void(std::map<std::string, std::any>&)>& logic)
        : version(ver), description(desc), migrationLogic(logic) {}
};

class ConfigMigration {
public:
    ConfigMigration() = default;
    
    void addMigrationStep(int fromVersion, int toVersion, const std::string& description,
                          const std::function<void(std::map<std::string, std::any>&)>& migrationLogic);
    
    std::vector<ConfigError> applyMigrations(std::map<std::string, std::any>& config) const;
    
    void setCurrentVersion(int version) { currentVersion = version; }
    int getCurrentVersion() const { return currentVersion; }
    
    // Get all migration steps
    const std::vector<MigrationStep>& getMigrationSteps() const { return migrationSteps; }
    
private:
    std::vector<MigrationStep> migrationSteps;
    int currentVersion = 1;
};

} // namespace nlm