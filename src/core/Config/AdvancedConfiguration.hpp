// Configuration for advanced options (environment variables, profiles)
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include "../core/Config/ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class AdvancedConfiguration {
public:
    enum class ProfileType {
        BASIC,
        HIGH_PERFORMANCE,
        RESEARCH,
        PRODUCTION,
        CUSTOM
    };

    struct Profile {
        std::string name;
        std::string description;
        ProfileType type;
        std::string configFile;
        std::map<std::string, std::string> settings;
        bool isBuiltIn;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastModified;
        
        Profile() : type(ProfileType::BASIC), isBuiltIn(false), created(std::chrono::system_clock::now()) {}
        Profile(const std::string& n, const std::string& desc, ProfileType t, bool builtIn = false)
            : name(n), description(desc), type(t), isBuiltIn(builtIn), 
              created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
    };

    struct EnvironmentVariable {
        std::string name;
        std::string value;
        std::string description;
        std::string configKey;
        bool required;
        std::string validationPattern;
        
        EnvironmentVariable() : required(false) {}
        EnvironmentVariable(const std::string& n, const std::string& v, const std::string& desc,
                          const std::string& key, bool req = false)
            : name(n), value(v), description(desc), configKey(key), required(req) {}
    };

    struct ConfigurationProfile {
        std::string name;
        std::string version;
        std::string author;
        std::string organization;
        std::string description;
        std::map<std::string, std::string> settings;
        std::vector<std::string> dependencies;
        std::chrono::system_clock::time_point created;
        std::chrono::system_clock::time_point lastModified;
        
        ConfigurationProfile() : created(std::chrono::system_clock::now()) {}
    };

    struct PerformanceProfile {
        std::string name;
        double expectedUpdateTime; // milliseconds
        size_t expectedMemoryUsage; // MB
        double expectedCpuUsage; // percentage
        std::string useCase;
        std::map<std::string, std::string> settings;
        
        PerformanceProfile() : expectedUpdateTime(10.0), expectedMemoryUsage(100), expectedCpuUsage(10.0) {}
    };

    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> suggestions;
        double confidenceScore; // 0-1
        
        ValidationResult() : isValid(true), confidenceScore(1.0) {}
    };

    AdvancedConfiguration();
    ~AdvancedConfiguration() = default;
    
    // Singleton pattern
    static AdvancedConfiguration& getInstance();
    static void resetInstance();
    
    // Environment variable management
    void setEnvironmentVariable(const std::string& name, const std::string& value);
    void setEnvironmentVariable(const EnvironmentVariable& envVar);
    std::string getEnvironmentVariable(const std::string& name) const;
    bool hasEnvironmentVariable(const std::string& name) const;
    void removeEnvironmentVariable(const std::string& name);
    std::vector<std::string> getEnvironmentVariableNames() const;
    
    // Profile management
    bool createProfile(const Profile& profile);
    bool loadProfile(const std::string& profileName);
    bool saveProfile(const Profile& profile);
    bool deleteProfile(const std::string& profileName);
    Profile getProfile(const std::string& profileName) const;
    std::vector<Profile> getAllProfiles() const;
    std::vector<std::string> getProfileNames() const;
    bool activateProfile(const std::string& profileName);
    std::string getActiveProfileName() const;
    
    // Built-in profiles
    std::vector<Profile> getBuiltInProfiles() const;
    std::vector<ProfileType> getProfileTypes() const;
    Profile getBuiltInProfile(ProfileType type) const;
    
    // Environment variable definitions
    void addEnvironmentVariable(const EnvironmentVariable& envVar);
    EnvironmentVariable getEnvironmentVariableDefinition(const std::string& name) const;
    std::vector<EnvironmentVariable> getEnvironmentVariableDefinitions() const;
    std::string getEnvironmentVariableHelp(const std::string& name) const;
    std::string getAllEnvironmentVariablesHelp() const;
    
    // Configuration loading from environment
    bool loadFromEnvironment();
    bool loadFromEnvironmentVariables(const std::vector<EnvironmentVariable>& envVars);
    bool updateFromEnvironment();
    
    // Configuration profiles
    bool createConfigurationProfile(const ConfigurationProfile& profile);
    bool loadConfigurationProfile(const std::string& profileName);
    bool saveConfigurationProfile(const ConfigurationProfile& profile);
    bool deleteConfigurationProfile(const std::string& profileName);
    ConfigurationProfile getConfigurationProfile(const std::string& profileName) const;
    std::vector<ConfigurationProfile> getAllConfigurationProfiles() const;
    std::vector<std::string> getConfigurationProfileNames() const;
    bool activateConfigurationProfile(const std::string& profileName);
    std::string getActiveConfigurationProfileName() const;
    
    // Performance profiles
    bool createPerformanceProfile(const PerformanceProfile& profile);
    bool loadPerformanceProfile(const std::string& profileName);
    bool savePerformanceProfile(const PerformanceProfile& profile);
    bool deletePerformanceProfile(const std::string& profileName);
    PerformanceProfile getPerformanceProfile(const std::string& profileName) const;
    std::vector<PerformanceProfile> getAllPerformanceProfiles() const;
    std::vector<std::string> getPerformanceProfileNames() const;
    bool activatePerformanceProfile(const std::string& profileName);
    std::string getActivePerformanceProfileName() const;
    
    // Profile validation
    ValidationResult validateProfile(const Profile& profile) const;
    ValidationResult validateEnvironmentVariables(const std::vector<EnvironmentVariable>& envVars) const;
    ValidationResult validateConfigurationProfile(const ConfigurationProfile& profile) const;
    
    // Configuration merging and inheritance
    std::map<std::string, std::string> mergeProfiles(const std::vector<std::string>& profileNames) const;
    std::map<std::string, std::string> getInheritableSettings(const std::string& profileName) const;
    
    // Dynamic configuration
    void enableDynamicConfiguration(bool enabled = true);
    bool isDynamicConfigurationEnabled() const;
    void setConfigurationChangeCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback);
    
    // Configuration export/import
    bool exportProfile(const std::string& profileName, const std::string& filepath) const;
    bool importProfile(const std::string& filepath);
    bool exportAllProfiles(const std::string& directory) const;
    bool importAllProfiles(const std::string& directory);
    
    // Configuration management
    void clearAllProfiles();
    void clearEnvironmentVariables();
    void resetToDefaults();
    
    // Configuration analysis
    std::map<std::string, std::string> analyzeConfigurationImpact(const Profile& profile) const;
    std::vector<std::string> getRecommendedSettings(const std::string& useCase) const;
    std::string getOptimalProfileForUseCase(const std::string& useCase) const;
    
    // Configuration synchronization
    bool syncProfiles(const std::string& remotePath);
    bool backupProfiles(const std::string& backupPath);
    bool restoreProfiles(const std::string& backupPath);
    
    // Configuration statistics
    size_t getProfileCount() const;
    size_t getEnvironmentVariableCount() const;
    size_t getConfigurationProfileCount() const;
    size_t getPerformanceProfileCount() const;
    std::string getConfigurationStatistics() const;
    
    // Utility functions
    static std::string normalizeProfileName(const std::string& name);
    static std::string normalizeEnvironmentVariableName(const std::string& name);
    static bool isValidProfileName(const std::string& name);
    static bool isValidEnvironmentVariableName(const std::string& name);
    static std::string generateProfileFilename(const std::string& profileName);
    static std::string generateEnvironmentVariableFilename(const std::string& varName);
    
    // Configuration validation
    ValidationResult validateConfiguration(const std::map<std::string, std::string>& config) const;
    std::vector<std::string> getValidationErrors(const std::map<std::string, std::string>& config) const;
    std::vector<std::string> getValidationWarnings(const std::map<std::string, std::string>& config) const;
    
    // Configuration completion
    std::map<std::string, std::string> suggestMissingSettings(const std::map<std::string, std::string>& config) const;
    std::map<std::string, std::string> suggestConflictingSettings(const std::map<std::string, std::string>& config) const;
    
    // Configuration recommendations
    std::vector<std::string> getConfigurationRecommendations(const std::string& profileName) const;
    std::vector<std::string> getEnvironmentVariableRecommendations(const std::string& profileName) const;
    std::vector<std::string> getPerformanceRecommendations(const std::string& profileName) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    // Default environment variables
    std::vector<EnvironmentVariable> getDefaultEnvironmentVariables() const;
    
    // Default profiles
    std::vector<Profile> getDefaultProfiles() const;
    
    // Default configuration profiles
    std::vector<ConfigurationProfile> getDefaultConfigurationProfiles() const;
    
    // Default performance profiles
    std::vector<PerformanceProfile> getDefaultPerformanceProfiles() const;
    
    // Environment variable loading
    bool loadEnvironmentVariables(const std::vector<EnvironmentVariable>& envVars);
    bool validateEnvironmentVariable(const EnvironmentVariable& envVar) const;
    
    // Profile validation
    bool validateProfileName(const std::string& name) const;
    bool validateProfileSettings(const std::map<std::string, std::string>& settings) const;
    
    // File operations
    bool saveProfileToFile(const Profile& profile, const std::string& filepath) const;
    bool loadProfileFromFile(Profile& profile, const std::string& filepath);
    bool saveConfigurationProfileToFile(const ConfigurationProfile& profile, const std::string& filepath) const;
    bool loadConfigurationProfileFromFile(ConfigurationProfile& profile, const std::string& filepath);
    bool savePerformanceProfileToFile(const PerformanceProfile& profile, const std::string& filepath) const;
    bool loadPerformanceProfileFromFile(PerformanceProfile& profile, const std::string& filepath);
    
    // Profile merging
    std::map<std::string, std::string> mergeProfileSettings(const std::vector<Profile>& profiles) const;
    std::map<std::string, std::string> applyInheritance(const std::map<std::string, std::string>& baseSettings,
                                                        const Profile& childProfile) const;
    
    // Configuration analysis
    std::map<std::string, std::string> analyzeProfileImpact(const Profile& profile) const;
    std::vector<std::string> getUseCaseRecommendations(const std::string& useCase) const;
    
    // Configuration statistics
    std::string formatProfileStatistics(size_t totalProfiles, size_t envVarCount, 
                                       size_t configProfileCount, size_t perfProfileCount) const;
    
    // Configuration validation helpers
    bool isSettingValid(const std::string& key, const std::string& value) const;
    std::string getSettingDescription(const std::string& key) const;
    std::string getSettingType(const std::string& key) const;
};

// Convenience functions for advanced configuration
inline AdvancedConfiguration& AdvancedConfig() { return AdvancedConfiguration::getInstance(); }
inline void initializeAdvancedConfiguration() {
    AdvancedConfiguration::getInstance();
}

// Advanced configuration utilities
namespace AdvancedConfigUtils {
    std::vector<std::string> getAvailableUseCases();
    std::string getRecommendedProfileForSystem(const std::string& systemInfo);
    std::map<std::string, std::string> getSystemConfigurationRecommendations();
    std::string generateConfigurationProfileFromTemplate(const std::string& templateName,
                                                         const std::map<std::string, std::string>& customizations);
    bool validateConfigurationFile(const std::string& filepath);
    std::string getConfigurationCompatibilityReport(const std::string& filepath);
}

// Advanced profile management
namespace ProfileManager {
    bool createProfileFromConfig(const std::string& profileName, 
                                const ConfigurationManager& config,
                                const std::string& description = "");
    bool exportProfileToJSON(const std::string& profileName, 
                           const ConfigurationManager& config,
                           const std::string& filepath);
    bool importProfileFromJSON(const std::string& profileName,
                             ConfigurationManager& config,
                             const std::string& filepath);
    std::vector<std::string> getRecommendedProfiles(const std::string& useCase);
    bool isProfileCompatible(const std::string& profileName,
                           const ConfigurationManager& config);
}

// Advanced environment variable management
namespace EnvVarManager {
    bool setSystemEnvironmentVariable(const std::string& name, const std::string& value);
    std::string getSystemEnvironmentVariable(const std::string& name);
    bool isSystemEnvironmentVariableSet(const std::string& name);
    bool unsetSystemEnvironmentVariable(const std::string& name);
    
    bool validateEnvironmentVariableValue(const std::string& name, 
                                         const std::string& value,
                                         const std::string& validationPattern);
    std::string getEnvironmentVariableDocumentation(const std::string& name);
    std::string getAllEnvironmentVariablesDocumentation();
}

// Advanced performance optimization
namespace PerformanceOptimizer {
    struct PerformanceRecommendation {
        std::string setting;
        std::string currentValue;
        std::string recommendedValue;
        double expectedImprovement; // percentage
        std::string reason;
    };
    
    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase);
    
    double estimatePerformanceImpact(const std::map<std::string, std::string>& changes,
                                     const std::string& useCase);
    
    std::map<std::string, std::string> optimizeForPerformance(
        const ConfigurationManager& config,
        const std::string& useCase,
        double targetUpdateTime,
        double targetMemoryUsage);
}

} // namespace nlm

// Include implementation files
#include "AdvancedConfiguration.cpp"
#include "AdvancedConfigUtils.cpp"
#include "ProfileManager.cpp"
#include "EnvVarManager.cpp"
#include "PerformanceOptimizer.cpp"
