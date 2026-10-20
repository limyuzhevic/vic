//! Plugin Update Manager for NLM Neural Simulation Project
//! 
//! This module implements the PluginUpdateManager class which provides
//! update management for plugins. The PluginUpdateManager:
//! - Manages plugin version updates
//! - Handles dependency updates
//! - Provides update checking and installation
//! - Manages plugin rollback and recovery
//! - Monitors plugin update status
//! - Integrates with plugin repositories
//! - Supports automated and manual updates
//! - Provides update statistics and reporting
//! 
//! The PluginUpdateManager ensures that plugins remain up-to-date while
//! maintaining system stability and security.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <any>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <set>
#include <queue>

namespace nlm {

class PluginUpdateManager {
public:
    PluginUpdateManager();
    ~PluginUpdateManager();
    
    // Disable copying
    PluginUpdateManager(const PluginUpdateManager&) = delete;
    PluginUpdateManager& operator=(const PluginUpdateManager&) = delete;
    
    // Enable moving
    PluginUpdateManager(PluginUpdateManager&&) noexcept = default;
    PluginUpdateManager& operator=(PluginUpdateManager&&) noexcept = default;
    
    // Update checking
    bool checkForUpdates(const std::string& pluginName);
    bool checkForUpdates(const std::string& pluginName, const std::string& repository);
    bool checkForAllUpdates();
    bool checkForAllUpdates(const std::string& repository);
    
    // Update management
    bool installUpdate(const std::string& pluginName, const PluginVersion& version);
    bool installUpdate(const std::string& pluginName, const std::string& versionString);
    bool installUpdate(const std::string& pluginName, const PluginVersion& version, const std::string& repository);
    bool installUpdate(const UpdateInfo& updateInfo);
    bool rollbackUpdate(const std::string& pluginName);
    bool rollbackUpdate(const std::string& pluginName, const PluginVersion& version);
    bool cancelUpdate(const std::string& pluginName);
    bool skipUpdate(const std::string& pluginName, const PluginVersion& version);
    
    // Update status management
    bool setPluginUpdateStatus(const std::string& pluginName, const UpdateStatus& status);
    UpdateStatus getPluginUpdateStatus(const std::string& pluginName) const;
    bool updatePluginVersion(const std::string& pluginName, const PluginVersion& version);
    PluginVersion getPluginVersion(const std::string& pluginName) const;
    std::chrono::system_clock::time_point getLastUpdateTime(const std::string& pluginName) const;
    std::chrono::system_clock::time_point getNextUpdateCheckTime(const std::string& pluginName) const;
    
    // Update information
    UpdateInfo getUpdateInfo(const std::string& pluginName) const;
    std::vector<UpdateInfo> getAllUpdateInfo() const;
    std::vector<UpdateInfo> getAvailableUpdates() const;
    std::vector<UpdateInfo> getPendingUpdates() const;
    std::vector<UpdateInfo> getFailedUpdates() const;
    
    // Update dependency management
    bool addUpdateDependency(const std::string& pluginName, const std::string& dependency);
    bool removeUpdateDependency(const std::string& pluginName, const std::string& dependency);
    std::vector<std::string> getUpdateDependencies(const std::string& pluginName) const;
    std::unordered_map<std::string, std::vector<std::string>> getAllUpdateDependencies() const;
    
    // Update repository management
    bool addUpdateRepository(const UpdateRepository& repository);
    bool removeUpdateRepository(const std::string& repositoryName);
    UpdateRepository getUpdateRepository(const std::string& repositoryName) const;
    std::vector<UpdateRepository> getAllUpdateRepositories() const;
    bool setDefaultUpdateRepository(const std::string& repositoryName);
    std::string getDefaultUpdateRepository() const;
    
    // Update validation
    bool validateUpdate(const UpdateInfo& updateInfo) const;
    std::vector<std::string> getUpdateValidationErrors(const UpdateInfo& updateInfo) const;
    bool validatePluginUpdateDependencies(const std::string& pluginName, const PluginVersion& version) const;
    
    // Update installation validation
    bool validateUpdateInstallation(const std::string& pluginName, const PluginVersion& version) const;
    std::vector<std::string> getUpdateInstallationErrors(const std::string& pluginName, const PluginVersion& version) const;
    bool checkUpdateConflicts(const std::string& pluginName, const PluginVersion& version) const;
    
    // Update rollback
    bool prepareRollback(const std::string& pluginName);
    bool performRollback(const std::string& pluginName);
    bool validateRollback(const std::string& pluginName) const;
    std::vector<std::string> getRollbackErrors(const std::string& pluginName) const;
    
    // Update monitoring
    bool enableUpdateMonitoring(bool enable);
    bool isUpdateMonitoringEnabled() const;
    void recordUpdateAttempt(const std::string& pluginName, const UpdateInfo& updateInfo, bool success, const std::string& error = "");
    UpdateStats getUpdateStats(const std::string& pluginName) const;
    UpdateStats getOverallUpdateStats() const;
    
    // Update statistics
    struct UpdateStats {
        std::string pluginName;
        size_t totalUpdatesChecked;
        size_t successfulUpdates;
        size_t failedUpdates;
        size_t skippedUpdates;
        size_t rolledBackUpdates;
        double successRate;
        std::chrono::system_clock::time_point lastUpdateAttempt;
        std::chrono::system_clock::time_point firstUpdateAttempt;
        
        UpdateStats() : pluginName(""), totalUpdatesChecked(0), successfulUpdates(0),
            failedUpdates(0), skippedUpdates(0), rolledBackUpdates(0), successRate(0.0),
            lastUpdateAttempt(std::chrono::system_clock::now()),
            firstUpdateAttempt(std::chrono::system_clock::now()) {}
    };
    
    UpdateStats getPluginUpdateStats(const std::string& pluginName) const;
    std::vector<UpdateStats> getAllUpdateStats() const;
    
    // Update system integration
    void setNLMConfig(std::shared_ptr<Config> nlmConfig);
    std::shared_ptr<Config> getNLMConfig() const;
    bool syncWithNLMConfig(const std::string& updatePrefix = "");
    bool applyNLMConfigToUpdateManager(const std::string& updatePrefix = "");
    
    // Update templates
    bool registerUpdateTemplate(const std::string& templateName, const UpdateTemplate& template);
    bool unregisterUpdateTemplate(const std::string& templateName);
    std::unique_ptr<PluginUpdateManager> createFromUpdateTemplate(const std::string& templateName,
                                                                 const std::unordered_map<std::string, std::any>& parameters = {});
    std::vector<std::string> getRegisteredUpdateTemplateNames() const;
    
    // Update lifecycle
    void pauseUpdateChecking();
    void resumeUpdateChecking();
    void shutdownUpdateManager();
    bool isUpdateManagerActive() const;
    
    // Update system integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setConfigurationManager(std::shared_ptr<PluginConfiguration> configurationManager);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    void setFactory(std::shared_ptr<ComponentFactory> factory);
    
    // Update debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logUpdateCheck(const std::string& pluginName, bool success, const std::string& error = "") const;
    void logUpdateInstall(const std::string& pluginName, const PluginVersion& version, bool success, const std::string& error = "") const;
    void logUpdateRollback(const std::string& pluginName, bool success, const std::string& error = "") const;
    void logUpdateSystemError(const std::string& error) const;
    
    // Update system optimization
    void optimizeUpdateChecking();
    void consolidateUpdateRepositories();
    void cleanInvalidUpdateInfo();
    void balanceUpdateScheduling();
    
    // Update system export/import
    bool exportUpdateConfiguration(const std::string& filePath) const;
    bool importUpdateConfiguration(const std::string& filePath);
    std::string exportUpdateConfigurationAsJSON() const;
    bool importUpdateConfigurationFromJSON(const std::string& jsonData);
    
    // Update system testing
    bool testUpdateSystem() const;
    std::vector<std::string> getUpdateTestResults() const;
    bool testUpdateRepository(const std::string& repositoryName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withUpdateLock(T func) {
        std::unique_lock<std::mutex> lock(updateMutex_);
        return func();
    }
    
    // Update validation
    bool validateUpdateRepository(const UpdateRepository& repository) const;
    bool validateUpdateTemplate(const UpdateTemplate& template) const;
    
    // Update statistics
    void updateUpdateStatistics();
    double calculateSuccessRate() const;
    
    // Update cleanup
    void cleanupExpiredUpdateInfo();
    void cleanupInvalidUpdateRepositories();
    
    // Update scheduling
    void scheduleNextUpdateCheck(const std::string& pluginName);
    void processPendingUpdates();
    
    // Update optimization
    void optimizeUpdateRepositoryOrder();
    void consolidateDuplicateUpdateRepositories();
}; // namespace nlm

// Update-related data structures

struct UpdateInfo {
    std::string pluginName;
    PluginVersion currentVersion;
    PluginVersion availableVersion;
    std::string releaseNotes;
    std::chrono::system_clock::time_point releaseDate;
    std::string repositoryName;
    std::string packageName;
    std::string packagePath;
    double packageSize;
    std::string checksum;
    bool isSecurityUpdate;
    bool isBreakingChange;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflicts;
    UpdateStatus status;
    std::chrono::system_clock::time_point attemptedAt;
    std::chrono::system_clock::time_point installedAt;
    std::string errorMessage;
    
    UpdateInfo() : currentVersion(1, 0, 0), availableVersion(1, 0, 0),
        packageSize(0.0), isSecurityUpdate(false), isBreakingChange(false),
        status(UpdateStatus::NotChecked),
        attemptedAt(std::chrono::system_clock::now()),
        installedAt(std::chrono::system_clock::now()) {}
};

struct UpdateRepository {
    std::string name;
    std::string url;
    std::string type; // e.g., "github", "local", "remote", "custom"
    std::string username;
    std::string password;
    std::string token;
    bool isEnabled;
    bool isDefault;
    std::chrono::system_clock::time_point lastSync;
    std::chrono::system_clock::time_point nextSync;
    std::unordered_map<std::string, std::any> configuration;
    
    UpdateRepository() : isEnabled(true), isDefault(false),
        lastSync(std::chrono::system_clock::now()),
        nextSync(std::chrono::system_clock::now()) {}
};

struct UpdateTemplate {
    std::string name;
    std::string description;
    std::string repository;
    PluginVersion minVersion;
    PluginVersion maxVersion;
    std::unordered_map<std::string, std::any> parameters;
    bool allowPrereleases;
    bool allowBreakingChanges;
    std::vector<std::string> requiredPlugins;
    std::vector<std::string> optionalPlugins;
    
    UpdateTemplate() : minVersion(1, 0, 0), maxVersion(999, 999, 999),
        allowPrereleases(false), allowBreakingChanges(false) {}
};

struct UpdateStats {
    std::string pluginName;
    size_t totalUpdatesChecked;
    size_t successfulUpdates;
    size_t failedUpdates;
    size_t skippedUpdates;
    size_t rolledBackUpdates;
    double successRate;
    std::chrono::system_clock::time_point lastUpdateAttempt;
    std::chrono::system_clock::time_point firstUpdateAttempt;
    
    UpdateStats() : pluginName(""), totalUpdatesChecked(0), successfulUpdates(0),
        failedUpdates(0), skippedUpdates(0), rolledBackUpdates(0), successRate(0.0),
        lastUpdateAttempt(std::chrono::system_clock::now()),
        firstUpdateAttempt(std::chrono::system_clock::now()) {}
};

// Enum definitions for update-related types
enum class UpdateStatus {
    NotChecked,
    Available,
    Checking,
    Downloading,
    Installing,
    Installed,
    Failed,
    Skipped,
    RolledBack,
    UpToDate,
    Cancelled,
    Deprecated
};

enum class UpdateType {
    Standard,
    Security,
    BugFix,
    Feature,
    BreakingChange,
    Patch,
    Minor,
    Major
};

// Constants for update management
namespace UpdateConstants {
    const int DEFAULT_UPDATE_CHECK_INTERVAL_HOURS = 24;
    const int DEFAULT_UPDATE_RETRY_ATTEMPTS = 3;
    const double DEFAULT_UPDATE_RETRY_DELAY_SECONDS = 60.0;
    const size_t DEFAULT_MAX_PENDING_UPDATES = 10;
    const double DEFAULT_UPDATE_SUCCESS_RATE_THRESHOLD = 0.9;
    const std::string DEFAULT_UPDATE_REPOSITORY = "default";
    const std::string DEFAULT_UPDATE_TYPE = "standard";
    const bool DEFAULT_ALLOW_PRERELEASES = false;
    const bool DEFAULT_ALLOW_BREAKING_CHANGES = false;
}