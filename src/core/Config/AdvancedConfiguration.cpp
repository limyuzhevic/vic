#include "AdvancedConfiguration.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <sys/stat.h>
#include <dirent.h>

namespace nlm {

AdvancedConfiguration::AdvancedConfiguration() {
    // Initialize with default profiles and environment variables
    initializeDefaults();
    
    NLM_LOG_INFO("AdvancedConfiguration initialized", "AdvancedConfiguration::Constructor");
}

void AdvancedConfiguration::initializeDefaults() {
    // Load default environment variables
    auto defaultEnvVars = getDefaultEnvironmentVariables();
    for (const auto& envVar : defaultEnvVars) {
        pImpl->environmentVariables[envVar.name] = envVar;
    }
    
    // Load default profiles
    auto defaultProfiles = getDefaultProfiles();
    for (const auto& profile : defaultProfiles) {
        pImpl->profiles[profile.name] = profile;
    }
    
    // Load default configuration profiles
    auto defaultConfigProfiles = getDefaultConfigurationProfiles();
    for (const auto& configProfile : defaultConfigProfiles) {
        pImpl->configurationProfiles[configProfile.name] = configProfile;
    }
    
    // Load default performance profiles
    auto defaultPerformanceProfiles = getDefaultPerformanceProfiles();
    for (const auto& perfProfile : defaultPerformanceProfiles) {
        pImpl->performanceProfiles[perfProfile.name] = perfProfile;
    }
}

void AdvancedConfiguration::setEnvironmentVariable(const std::string& name, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    EnvironmentVariable envVar;
    envVar.name = name;
    envVar.value = value;
    
    // Find if this is a known environment variable
    auto it = pImpl->environmentVariables.find(name);
    if (it != pImpl->environmentVariables.end()) {
        envVar.description = it->second.description;
        envVar.configKey = it->second.configKey;
        envVar.required = it->second.required;
        envVar.validationPattern = it->second.validationPattern;
    }
    
    pImpl->environmentVariables[name] = envVar;
    
    NLM_LOG_INFO(std::string("Environment variable set: ") + name + " = " + value, 
                "AdvancedConfiguration::setEnvironmentVariable");
}

void AdvancedConfiguration::setEnvironmentVariable(const EnvironmentVariable& envVar) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->environmentVariables[envVar.name] = envVar;
    
    NLM_LOG_INFO(std::string("Environment variable defined and set: ") + envVar.name + " = " + envVar.value, 
                "AdvancedConfiguration::setEnvironmentVariable");
}

std::string AdvancedConfiguration::getEnvironmentVariable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->environmentVariables.find(name);
    if (it != pImpl->environmentVariables.end()) {
        return it->second.value;
    }
    
    return "";
}

bool AdvancedConfiguration::hasEnvironmentVariable(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->environmentVariables.find(name) != pImpl->environmentVariables.end();
}

void AdvancedConfiguration::removeEnvironmentVariable(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->environmentVariables.find(name);
    if (it != pImpl->environmentVariables.end()) {
        pImpl->environmentVariables.erase(it);
        
        NLM_LOG_INFO(std::string("Environment variable removed: ") + name, 
                    "AdvancedConfiguration::removeEnvironmentVariable");
    }
}

std::vector<std::string> AdvancedConfiguration::getEnvironmentVariableNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->environmentVariables) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool AdvancedConfiguration::createProfile(const Profile& profile) {
    if (!isValidProfileName(profile.name)) {
        NLM_LOG_ERROR(std::string("Invalid profile name: ") + profile.name, 
                     "AdvancedConfiguration::createProfile");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->profiles[profile.name] = profile;
    
    NLM_LOG_INFO(std::string("Profile created: ") + profile.name, 
                "AdvancedConfiguration::createProfile");
    
    return true;
}

bool AdvancedConfiguration::loadProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it == pImpl->profiles.end()) {
        NLM_LOG_ERROR(std::string("Profile not found: ") + profileName, 
                     "AdvancedConfiguration::loadProfile");
        return false;
    }
    
    // Apply profile settings
    activateProfile(normalizedName);
    
    NLM_LOG_INFO(std::string("Profile loaded: ") + profileName, 
                "AdvancedConfiguration::loadProfile");
    
    return true;
}

bool AdvancedConfiguration::saveProfile(const Profile& profile) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filename = generateProfileFilename(profile.name);
    
    if (!saveProfileToFile(profile, filename)) {
        NLM_LOG_ERROR(std::string("Failed to save profile to file: ") + filename, 
                     "AdvancedConfiguration::saveProfile");
        return false;
    }
    
    NLM_LOG_INFO(std::string("Profile saved to file: ") + filename, 
                "AdvancedConfiguration::saveProfile");
    
    return true;
}

bool AdvancedConfiguration::deleteProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it == pImpl->profiles.end()) {
        NLM_LOG_WARN(std::string("Profile not found: ") + profileName, 
                    "AdvancedConfiguration::deleteProfile");
        return false;
    }
    
    // Remove profile
    pImpl->profiles.erase(it);
    
    // Remove from active profiles if active
    auto activeIt = pImpl->activeProfiles.find(normalizedName);
    if (activeIt != pImpl->activeProfiles.end()) {
        pImpl->activeProfiles.erase(activeIt);
    }
    
    NLM_LOG_INFO(std::string("Profile deleted: ") + profileName, 
                "AdvancedConfiguration::deleteProfile");
    
    return true;
}

AdvancedConfiguration::Profile AdvancedConfiguration::getProfile(const std::string& profileName) const {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it == pImpl->profiles.end()) {
        throw std::runtime_error("Profile not found: " + profileName);
    }
    
    return it->second;
}

std::vector<AdvancedConfiguration::Profile> AdvancedConfiguration::getAllProfiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<Profile> profiles;
    for (const auto& pair : pImpl->profiles) {
        profiles.push_back(pair.second);
    }
    
    return profiles;
}

std::vector<std::string> AdvancedConfiguration::getProfileNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->profiles) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool AdvancedConfiguration::activateProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it == pImpl->profiles.end()) {
        NLM_LOG_ERROR(std::string("Cannot activate non-existent profile: ") + profileName, 
                     "AdvancedConfiguration::activateProfile");
        return false;
    }
    
    pImpl->activeProfiles.insert(normalizedName);
    
    NLM_LOG_INFO(std::string("Profile activated: ") + profileName, 
                "AdvancedConfiguration::activateProfile");
    
    return true;
}

std::string AdvancedConfiguration::getActiveProfileName() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->activeProfiles.empty()) {
        return "";
    }
    
    return *pImpl->activeProfiles.begin();
}

std::vector<AdvancedConfiguration::Profile> AdvancedConfiguration::getBuiltInProfiles() const {
    std::vector<Profile> builtInProfiles;
    
    for (const auto& pair : pImpl->profiles) {
        if (pair.second.isBuiltIn) {
            builtInProfiles.push_back(pair.second);
        }
    }
    
    return builtInProfiles;
}

std::vector<AdvancedConfiguration::ProfileType> AdvancedConfiguration::getProfileTypes() const {
    return {ProfileType::BASIC, ProfileType::HIGH_PERFORMANCE, ProfileType::RESEARCH, ProfileType::PRODUCTION, ProfileType::CUSTOM};
}

AdvancedConfiguration::Profile AdvancedConfiguration::getBuiltInProfile(ProfileType type) const {
    std::string typeName;
    
    switch (type) {
        case ProfileType::BASIC: typeName = "basic"; break;
        case ProfileType::HIGH_PERFORMANCE: typeName = "high_performance"; break;
        case ProfileType::RESEARCH: typeName = "research"; break;
        case ProfileType::PRODUCTION: typeName = "production"; break;
        case ProfileType::CUSTOM: typeName = "custom"; break;
    }
    
    return getProfile(typeName);
}

void AdvancedConfiguration::addEnvironmentVariable(const EnvironmentVariable& envVar) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->environmentVariables[envVar.name] = envVar;
    
    NLM_LOG_INFO(std::string("Environment variable added: ") + envVar.name, 
                "AdvancedConfiguration::addEnvironmentVariable");
}

AdvancedConfiguration::EnvironmentVariable AdvancedConfiguration::getEnvironmentVariableDefinition(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->environmentVariables.find(name);
    if (it != pImpl->environmentVariables.end()) {
        return it->second;
    }
    
    throw std::runtime_error("Environment variable not defined: " + name);
}

std::vector<AdvancedConfiguration::EnvironmentVariable> AdvancedConfiguration::getEnvironmentVariableDefinitions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<EnvironmentVariable> envVars;
    for (const auto& pair : pImpl->environmentVariables) {
        envVars.push_back(pair.second);
    }
    
    return envVars;
}

std::string AdvancedConfiguration::getEnvironmentVariableHelp(const std::string& name) const {
    auto it = pImpl->environmentVariables.find(name);
    if (it == pImpl->environmentVariables.end()) {
        return "Environment variable not defined: " + name;
    }
    
    const auto& envVar = it->second;
    
    std::stringstream help;
    help << "Environment Variable: " << envVar.name << std::endl;
    help << "Description: " << envVar.description << std::endl;
    help << "Config Key: " << envVar.configKey << std::endl;
    help << "Required: " << (envVar.required ? "Yes" : "No") << std::endl;
    if (!envVar.validationPattern.empty()) {
        help << "Validation Pattern: " << envVar.validationPattern << std::endl;
    }
    help << "Current Value: " << envVar.value << std::endl;
    
    return help.str();
}

std::string AdvancedConfiguration::getAllEnvironmentVariablesHelp() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream help;
    
    for (const auto& pair : pImpl->environmentVariables) {
        help << "---" << std::endl;
        help << getEnvironmentVariableHelp(pair.first);
    }
    
    return help.str();
}

bool AdvancedConfiguration::loadFromEnvironment() {
    // Update configuration from environment variables
    auto envVars = getEnvironmentVariableDefinitions();n
    return loadFromEnvironmentVariables(envVars);
}

bool AdvancedConfiguration::loadFromEnvironmentVariables(const std::vector<EnvironmentVariable>& envVars) {
    bool success = true;
    
    for (const auto& envVar : envVars) {
        if (envVar.required) {
            // Check if environment variable is set
            const char* envValue = std::getenv(envVar.name.c_str());
            if (!envValue) {
                NLM_LOG_WARN(std::string("Required environment variable not set: ") + envVar.name, 
                           "AdvancedConfiguration::loadFromEnvironmentVariables");
                success = false;
                continue;
            }
            
            setEnvironmentVariable(envVar.name, envValue);
        }
    }
    
    return success;
}

bool AdvancedConfiguration::updateFromEnvironment() {
    // Update existing configuration from environment variables
    bool success = true;
    
    auto envVars = getEnvironmentVariableDefinitions();
    for (const auto& envVar : envVars) {
        // Check if this environment variable is set in the system
        const char* envValue = std::getenv(envVar.name.c_str());
        if (envValue) {
            // Validate the value
            if (!envVar.validationPattern.empty()) {
                std::regex pattern(envVar.validationPattern);
                if (!std::regex_match(envValue, pattern)) {
                    NLM_LOG_WARN(std::string("Environment variable value validation failed for: ") + envVar.name, 
                               "AdvancedConfiguration::updateFromEnvironment");
                    success = false;
                    continue;
                }
            }
            
            setEnvironmentVariable(envVar.name, envValue);
        }
    }
    
    return success;
}

bool AdvancedConfiguration::createConfigurationProfile(const ConfigurationProfile& profile) {
    if (!isValidProfileName(profile.name)) {
        NLM_LOG_ERROR(std::string("Invalid configuration profile name: ") + profile.name, 
                     "AdvancedConfiguration::createConfigurationProfile");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->configurationProfiles[profile.name] = profile;
    
    NLM_LOG_INFO(std::string("Configuration profile created: ") + profile.name, 
                "AdvancedConfiguration::createConfigurationProfile");
    
    return true;
}

bool AdvancedConfiguration::loadConfigurationProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->configurationProfiles.find(normalizedName);
    if (it == pImpl->configurationProfiles.end()) {
        NLM_LOG_ERROR(std::string("Configuration profile not found: ") + profileName, 
                     "AdvancedConfiguration::loadConfigurationProfile");
        return false;
    }
    
    // Apply profile settings
    activateConfigurationProfile(normalizedName);
    
    NLM_LOG_INFO(std::string("Configuration profile loaded: ") + profileName, 
                "AdvancedConfiguration::loadConfigurationProfile");
    
    return true;
}

bool AdvancedConfiguration::saveConfigurationProfile(const ConfigurationProfile& profile) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filename = generateProfileFilename(profile.name);
    
    if (!saveConfigurationProfileToFile(profile, filename)) {
        NLM_LOG_ERROR(std::string("Failed to save configuration profile to file: ") + filename, 
                     "AdvancedConfiguration::saveConfigurationProfile");
        return false;
    }
    
    NLM_LOG_INFO(std::string("Configuration profile saved to file: ") + filename, 
                "AdvancedConfiguration::saveConfigurationProfile");
    
    return true;
}

bool AdvancedConfiguration::deleteConfigurationProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->configurationProfiles.find(normalizedName);
    if (it == pImpl->configurationProfiles.end()) {
        NLM_LOG_WARN(std::string("Configuration profile not found: ") + profileName, 
                    "AdvancedConfiguration::deleteConfigurationProfile");
        return false;
    }
    
    // Remove profile
    pImpl->configurationProfiles.erase(it);
    
    // Remove from active profiles if active
    auto activeIt = pImpl->activeConfigProfiles.find(normalizedName);
    if (activeIt != pImpl->activeConfigProfiles.end()) {
        pImpl->activeConfigProfiles.erase(activeIt);
    }
    
    NLM_LOG_INFO(std::string("Configuration profile deleted: ") + profileName, 
                "AdvancedConfiguration::deleteConfigurationProfile");
    
    return true;
}

AdvancedConfiguration::ConfigurationProfile AdvancedConfiguration::getConfigurationProfile(const std::string& profileName) const {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->configurationProfiles.find(normalizedName);
    if (it == pImpl->configurationProfiles.end()) {
        throw std::runtime_error("Configuration profile not found: " + profileName);
    }
    
    return it->second;
}

std::vector<AdvancedConfiguration::ConfigurationProfile> AdvancedConfiguration::getAllConfigurationProfiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ConfigurationProfile> profiles;
    for (const auto& pair : pImpl->configurationProfiles) {
        profiles.push_back(pair.second);
    }
    
    return profiles;
}

std::vector<std::string> AdvancedConfiguration::getConfigurationProfileNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->configurationProfiles) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool AdvancedConfiguration::activateConfigurationProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->configurationProfiles.find(normalizedName);
    if (it == pImpl->configurationProfiles.end()) {
        NLM_LOG_ERROR(std::string("Cannot activate non-existent configuration profile: ") + profileName, 
                     "AdvancedConfiguration::activateConfigurationProfile");
        return false;
    }
    
    pImpl->activeConfigProfiles.insert(normalizedName);
    
    NLM_LOG_INFO(std::string("Configuration profile activated: ") + profileName, 
                "AdvancedConfiguration::activateConfigurationProfile");
    
    return true;
}

std::string AdvancedConfiguration::getActiveConfigurationProfileName() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->activeConfigProfiles.empty()) {
        return "";
    }
    
    return *pImpl->activeConfigProfiles.begin();
}

bool AdvancedConfiguration::createPerformanceProfile(const PerformanceProfile& profile) {
    if (!isValidProfileName(profile.name)) {
        NLM_LOG_ERROR(std::string("Invalid performance profile name: ") + profile.name, 
                     "AdvancedConfiguration::createPerformanceProfile");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->performanceProfiles[profile.name] = profile;
    
    NLM_LOG_INFO(std::string("Performance profile created: ") + profile.name, 
                "AdvancedConfiguration::createPerformanceProfile");
    
    return true;
}

bool AdvancedConfiguration::loadPerformanceProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->performanceProfiles.find(normalizedName);
    if (it == pImpl->performanceProfiles.end()) {
        NLM_LOG_ERROR(std::string("Performance profile not found: ") + profileName, 
                     "AdvancedConfiguration::loadPerformanceProfile");
        return false;
    }
    
    // Apply profile settings
    activatePerformanceProfile(normalizedName);
    
    NLM_LOG_INFO(std::string("Performance profile loaded: ") + profileName, 
                "AdvancedConfiguration::loadPerformanceProfile");
    
    return true;
}

bool AdvancedConfiguration::savePerformanceProfile(const PerformanceProfile& profile) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filename = generateProfileFilename(profile.name);
    
    if (!savePerformanceProfileToFile(profile, filename)) {
        NLM_LOG_ERROR(std::string("Failed to save performance profile to file: ") + filename, 
                     "AdvancedConfiguration::savePerformanceProfile");
        return false;
    }
    
    NLM_LOG_INFO(std::string("Performance profile saved to file: ") + filename, 
                "AdvancedConfiguration::savePerformanceProfile");
    
    return true;
}

bool AdvancedConfiguration::deletePerformanceProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->performanceProfiles.find(normalizedName);
    if (it == pImpl->performanceProfiles.end()) {
        NLM_LOG_WARN(std::string("Performance profile not found: ") + profileName, 
                    "AdvancedConfiguration::deletePerformanceProfile");
        return false;
    }
    
    // Remove profile
    pImpl->performanceProfiles.erase(it);
    
    // Remove from active profiles if active
    auto activeIt = pImpl->activePerfProfiles.find(normalizedName);
    if (activeIt != pImpl->activePerfProfiles.end()) {
        pImpl->activePerfProfiles.erase(activeIt);
    }
    
    NLM_LOG_INFO(std::string("Performance profile deleted: ") + profileName, 
                "AdvancedConfiguration::deletePerformanceProfile");
    
    return true;
}

AdvancedConfiguration::PerformanceProfile AdvancedConfiguration::getPerformanceProfile(const std::string& profileName) const {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->performanceProfiles.find(normalizedName);
    if (it == pImpl->performanceProfiles.end()) {
        throw std::runtime_error("Performance profile not found: " + profileName);
    }
    
    return it->second;
}

std::vector<AdvancedConfiguration::PerformanceProfile> AdvancedConfiguration::getAllPerformanceProfiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceProfile> profiles;
    for (const auto& pair : pImpl->performanceProfiles) {
        profiles.push_back(pair.second);
    }
    
    return profiles;
}

std::vector<std::string> AdvancedConfiguration::getPerformanceProfileNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->performanceProfiles) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool AdvancedConfiguration::activatePerformanceProfile(const std::string& profileName) {
    std::string normalizedName = normalizeProfileName(profileName);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->performanceProfiles.find(normalizedName);
    if (it == pImpl->performanceProfiles.end()) {
        NLM_LOG_ERROR(std::string("Cannot activate non-existent performance profile: ") + profileName, 
                     "AdvancedConfiguration::activatePerformanceProfile");
        return false;
    }
    
    pImpl->activePerfProfiles.insert(normalizedName);
    
    NLM_LOG_INFO(std::string("Performance profile activated: ") + profileName, 
                "AdvancedConfiguration::activatePerformanceProfile");
    
    return true;
}

std::string AdvancedConfiguration::getActivePerformanceProfileName() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->activePerfProfiles.empty()) {
        return "";
    }
    
    return *pImpl->activePerfProfiles.begin();
}

AdvancedConfiguration::ValidationResult AdvancedConfiguration::validateProfile(const Profile& profile) const {
    ValidationResult result;
    
    // Check profile name
    if (!isValidProfileName(profile.name)) {
        result.isValid = false;
        result.errors.push_back("Invalid profile name: " + profile.name);
        result.suggestions["name"] = "Use only alphanumeric characters and underscores";
    }
    
    // Check if profile name is already used
    auto it = pImpl->profiles.find(profile.name);
    if (it != pImpl->profiles.end() && it->second.name != profile.name) {
        result.isValid = false;
        result.errors.push_back("Profile name already exists: " + profile.name);
        result.suggestions["name"] = "Choose a different profile name";
    }
    
    // Check settings
    for (const auto& setting : profile.settings) {
        if (setting.first.empty()) {
            result.isValid = false;
            result.errors.push_back("Empty setting key");
        }
        
        // Validate against known settings
        if (!isSettingValid(setting.first, setting.second)) {
            result.isValid = false;
            result.errors.push_back(std::string("Invalid setting '") + setting.first + "': " + setting.second);
            
            std::string description = getSettingDescription(setting.first);
            if (!description.empty()) {
                result.suggestions[setting.first] = "Use: " + description;
            }
        }
    }
    
    // Calculate confidence score
    if (!result.errors.empty()) {
        result.confidenceScore = 0.0;
    } else {
        result.confidenceScore = 1.0;
    }
    
    return result;
}

AdvancedConfiguration::ValidationResult AdvancedConfiguration::validateEnvironmentVariables(
    const std::vector<EnvironmentVariable>& envVars) const {
    ValidationResult result;
    
    for (const auto& envVar : envVars) {
        if (envVar.required && envVar.value.empty()) {
            result.isValid = false;
            result.errors.push_back("Required environment variable missing: " + envVar.name);
            result.suggestions[envVar.name] = "Set the environment variable or remove the requirement";
        }
        
        if (!envVar.validationPattern.empty()) {
            std::regex pattern(envVar.validationPattern);
            if (!std::regex_match(envVar.value, pattern)) {
                result.isValid = false;
                result.errors.push_back(std::string("Environment variable validation failed: ") + envVar.name);
                result.warnings.push_back(std::string("Environment variable '") + envVar.name + "' does not match pattern: " + envVar.validationPattern);
            }
        }
    }
    
    // Calculate confidence score
    if (!result.errors.empty()) {
        result.confidenceScore = 0.0;
    } else {
        result.confidenceScore = 1.0;
    }
    
    return result;
}

AdvancedConfiguration::ValidationResult AdvancedConfiguration::validateConfigurationProfile(
    const ConfigurationProfile& profile) const {
    ValidationResult result;
    
    // Check profile name
    if (!isValidProfileName(profile.name)) {
        result.isValid = false;
        result.errors.push_back("Invalid configuration profile name: " + profile.name);
        result.suggestions["name"] = "Use only alphanumeric characters and underscores";
    }
    
    // Check settings
    for (const auto& setting : profile.settings) {
        if (setting.first.empty()) {
            result.isValid = false;
            result.errors.push_back("Empty setting key");
        }
        
        // Validate against known settings
        if (!isSettingValid(setting.first, setting.second)) {
            result.isValid = false;
            result.errors.push_back(std::string("Invalid setting '") + setting.first + "': " + setting.second);
            
            std::string description = getSettingDescription(setting.first);
            if (!description.empty()) {
                result.suggestions[setting.first] = "Use: " + description;
            }
        }
    }
    
    // Calculate confidence score
    if (!result.errors.empty()) {
        result.confidenceScore = 0.0;
    } else {
        result.confidenceScore = 1.0;
    }
    
    return result;
}

std::map<std::string, std::string> AdvancedConfiguration::mergeProfiles(
    const std::vector<std::string>& profileNames) const {
    std::map<std::string, std::string> mergedSettings;
    
    for (const auto& profileName : profileNames) {
        std::string normalizedName = normalizeProfileName(profileName);
        
        auto it = pImpl->profiles.find(normalizedName);
        if (it != pImpl->profiles.end()) {
            // Add profile settings (later profiles override earlier ones)
            for (const auto& setting : it->second.settings) {
                mergedSettings[setting.first] = setting.second;
            }
        }
    }
    
    return mergedSettings;
}

std::map<std::string, std::string> AdvancedConfiguration::getInheritableSettings(const std::string& profileName) const {
    std::map<std::string, std::string> inheritableSettings;
    
    std::string normalizedName = normalizeProfileName(profileName);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it != pImpl->profiles.end()) {
        // For now, just return the profile's own settings
        // In a real implementation, this would also look at parent profiles
        inheritableSettings = it->second.settings;
    }
    
    return inheritableSettings;
}

void AdvancedConfiguration::enableDynamicConfiguration(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->dynamicConfigurationEnabled = enabled;
}

bool AdvancedConfiguration::isDynamicConfigurationEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->dynamicConfigurationEnabled;
}

void AdvancedConfiguration::setConfigurationChangeCallback(
    std::function<void(const std::string&, const std::string&, const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->configurationChangeCallback = callback;
}

bool AdvancedConfiguration::exportProfile(const std::string& profileName, const std::string& filepath) const {
    std::string normalizedName = normalizeProfileName(profileName);
    
    auto it = pImpl->profiles.find(normalizedName);
    if (it == pImpl->profiles.end()) {
        NLM_LOG_ERROR(std::string("Cannot export non-existent profile: ") + profileName, 
                     "AdvancedConfiguration::exportProfile");
        return false;
    }
    
    return saveProfileToFile(it->second, filepath);
}

bool AdvancedConfiguration::importProfile(const std::string& filepath) {
    Profile profile;
    
    if (!loadProfileFromFile(profile, filepath)) {
        NLM_LOG_ERROR(std::string("Failed to load profile from file: ") + filepath, 
                     "AdvancedConfiguration::importProfile");
        return false;
    }
    
    return createProfile(profile);
}

bool AdvancedConfiguration::exportAllProfiles(const std::string& directory) const {
    bool success = true;
    
    for (const auto& pair : pImpl->profiles) {
        std::string filepath = directory + "/" + generateProfileFilename(pair.first);
        if (!saveProfileToFile(pair.second, filepath)) {
            NLM_LOG_ERROR(std::string("Failed to export profile '") + pair.first + "'", 
                         "AdvancedConfiguration::exportAllProfiles");
            success = false;
        }
    }
    
    return success;
}

bool AdvancedConfiguration::importAllProfiles(const std::string& directory) {
    bool success = true;
    
    // In a real implementation, this would read all profile files from the directory
    // and import them
    return success;
}

void AdvancedConfiguration::clearAllProfiles() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->profiles.clear();
    pImpl->activeProfiles.clear();
    pImpl->configurationProfiles.clear();
    pImpl->activeConfigProfiles.clear();
    pImpl->performanceProfiles.clear();
    pImpl->activePerfProfiles.clear();
    
    NLM_LOG_INFO("All profiles cleared", "AdvancedConfiguration::clearAllProfiles");
}

void AdvancedConfiguration::clearEnvironmentVariables() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pImpl->environmentVariables.clear();
    
    NLM_LOG_INFO("All environment variables cleared", "AdvancedConfiguration::clearEnvironmentVariables");
}

void AdvancedConfiguration::resetToDefaults() {
    clearAllProfiles();
    clearEnvironmentVariables();
    
    // Reinitialize with defaults
    initializeDefaults();
    
    NLM_LOG_INFO("Configuration reset to defaults", "AdvancedConfiguration::resetToDefaults");
}

std::map<std::string, std::string> AdvancedConfiguration::analyzeConfigurationImpact(
    const Profile& profile) const {
    std::map<std::string, std::string> impact;
    
    for (const auto& setting : profile.settings) {
        std::string impactText = analyzeSettingImpact(setting.first, setting.second);
        impact[setting.first] = impactText;
    }
    
    return impact;
}

std::vector<std::string> AdvancedConfiguration::getRecommendedSettings(const std::string& useCase) const {
    std::vector<std::string> recommendations;
    
    if (useCase == "research") {
        recommendations = {
            "neuron_count=5000",
            "connection_probability=0.15",
            "enable_development=true",
            "log_level=DEBUG",
            "visualization_enabled=true"
        };
    } else if (useCase == "production") {
        recommendations = {
            "neuron_count=1000",
            "connection_probability=0.1",
            "enable_development=false",
            "log_level=INFO",
            "visualization_enabled=false"
        };
    } else if (useCase == "high_performance") {
        recommendations = {
            "neuron_count=2000",
            "connection_probability=0.12",
            "enable_development=true",
            "log_level=WARN",
            "visualization_enabled=false"
        };
    } else {
        recommendations = {
            "neuron_count=1000",
            "connection_probability=0.1",
            "enable_development=false",
            "log_level=INFO",
            "visualization_enabled=false"
        };
    }
    
    return recommendations;
}

std::string AdvancedConfiguration::getOptimalProfileForUseCase(const std::string& useCase) const {
    // Find the best matching profile for the use case
    std::string bestProfile;
    double bestScore = 0.0;
    
    for (const auto& pair : pImpl->profiles) {
        double score = calculateProfileScore(pair.second, useCase);
        if (score > bestScore) {
            bestScore = score;
            bestProfile = pair.first;
        }
    }
    
    return bestProfile;
}

bool AdvancedConfiguration::syncProfiles(const std::string& remotePath) {
    // In a real implementation, this would sync profiles with a remote server
    // For now, it's a placeholder
    NLM_LOG_WARN(std::string("Profile synchronization not implemented for: ") + remotePath, 
                "AdvancedConfiguration::syncProfiles");
    return false;
}

bool AdvancedConfiguration::backupProfiles(const std::string& backupPath) {
    try {
        // Create backup directory
        mkdir(backupPath.c_str(), 0777);
        
        // Export all profiles to backup directory
        return exportAllProfiles(backupPath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error creating backup: ") + e.what(), 
                     "AdvancedConfiguration::backupProfiles");
        return false;
    }
}

bool AdvancedConfiguration::restoreProfiles(const std::string& backupPath) {
    try {
        // Clear existing profiles
        clearAllProfiles();
        
        // Import profiles from backup directory
        return importAllProfiles(backupPath);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error restoring profiles: ") + e.what(), 
                     "AdvancedConfiguration::restoreProfiles");
        return false;
    }
}

size_t AdvancedConfiguration::getProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->profiles.size();
}

size_t AdvancedConfiguration::getEnvironmentVariableCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->environmentVariables.size();
}

size_t AdvancedConfiguration::getConfigurationProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->configurationProfiles.size();
}

size_t AdvancedConfiguration::getPerformanceProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->performanceProfiles.size();
}

std::string AdvancedConfiguration::getConfigurationStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream stats;
    stats << "=== Configuration Statistics ===" << std::endl;
    stats << "Profiles: " << pImpl->profiles.size() << std::endl;
    stats << "Environment Variables: " << pImpl->environmentVariables.size() << std::endl;
    stats << "Configuration Profiles: " << pImpl->configurationProfiles.size() << std::endl;
    stats << "Performance Profiles: " << pImpl->performanceProfiles.size() << std::endl;
    stats << "Active Profiles: " << pImpl->activeProfiles.size() << std::endl;
    stats << "Active Configuration Profiles: " << pImpl->activeConfigProfiles.size() << std::endl;
    stats << "Active Performance Profiles: " << pImpl->activePerfProfiles.size() << std::endl;
    
    return stats.str();
}

std::string AdvancedConfiguration::normalizeProfileName(const std::string& name) {
    std::string normalized = name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // Remove any path components
    size_t pos = normalized.find_last_of("/\\");
    if (pos != std::string::npos) {
        normalized = normalized.substr(pos + 1);
    }
    
    // Remove extension
    pos = normalized.find_last_of('.');
    if (pos != std::string::npos) {
        normalized = normalized.substr(0, pos);
    }
    
    return normalized;
}

std::string AdvancedConfiguration::normalizeEnvironmentVariableName(const std::string& name) {
    std::string normalized = name;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // Replace hyphens with underscores
    std::replace(normalized.begin(), normalized.end(), '-', '_');
    
    return normalized;
}

bool AdvancedConfiguration::isValidProfileName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool AdvancedConfiguration::isValidEnvironmentVariableName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

std::string AdvancedConfiguration::generateProfileFilename(const std::string& profileName) {
    return profileName + ".profile";
}

std::string AdvancedConfiguration::generateEnvironmentVariableFilename(const std::string& varName) {
    return varName + ".env";
}

AdvancedConfiguration::ValidationResult AdvancedConfiguration::validateConfiguration(
    const std::map<std::string, std::string>& config) const {
    ValidationResult result;
    
    for (const auto& setting : config) {
        if (!isSettingValid(setting.first, setting.second)) {
            result.isValid = false;
            result.errors.push_back(std::string("Invalid setting '") + setting.first + "': " + setting.second);
        }
    }
    
    // Calculate confidence score
    if (!result.errors.empty()) {
        result.confidenceScore = 0.0;
    } else {
        result.confidenceScore = 1.0;
    }
    
    return result;
}

std::vector<std::string> AdvancedConfiguration::getValidationErrors(
    const std::map<std::string, std::string>& config) const {
    std::vector<std::string> errors;
    
    for (const auto& setting : config) {
        if (!isSettingValid(setting.first, setting.second)) {
            errors.push_back(std::string("Invalid setting '") + setting.first + "': " + setting.second);
        }
    }
    
    return errors;
}

std::vector<std::string> AdvancedConfiguration::getValidationWarnings(
    const std::map<std::string, std::string>& config) const {
    std::vector<std::string> warnings;
    
    // Check for potentially problematic combinations
    if (config.find("neuron_count") != config.end() && config.find("connection_probability") != config.end()) {
        try {
            size_t neuronCount = std::stoull(config.at("neuron_count"));
            double connectionProb = std::stod(config.at("connection_probability"));
            
            if (neuronCount > 50000 && connectionProb > 0.5) {
                warnings.push_back("Large network with high connection probability may cause performance issues");
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
    
    // Check for development settings in production
    if (config.find("enable_development") != config.end()) {
        try {
            bool devEnabled = (config.at("enable_development") == "true" || config.at("enable_development") == "1");
            if (devEnabled) {
                warnings.push_back("Development features enabled in production configuration");
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
    
    return warnings;
}

std::map<std::string, std::string> AdvancedConfiguration::suggestMissingSettings(
    const std::map<std::string, std::string>& config) const {
    std::map<std::string, std::string> suggestions;
    
    // Check for critical settings
    std::vector<std::string> criticalSettings = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& setting : criticalSettings) {
        if (config.find(setting) == config.end()) {
            suggestions[setting] = "Required setting missing";
        }
    }
    
    return suggestions;
}

std::map<std::string, std::string> AdvancedConfiguration::suggestConflictingSettings(
    const std::map<std::string, std::string>& config) const {
    std::map<std::string, std::string> conflicts;
    
    // Check for conflicting settings
    if (config.find("neuron_count") != config.end() && config.find("max_simulation_steps") != config.end()) {
        try {
            size_t neuronCount = std::stoull(config.at("neuron_count"));
            size_t maxSteps = std::stoull(config.at("max_simulation_steps"));
            
            if (neuronCount > 10000 && maxSteps < 1000) {
                conflicts["neuron_count"] = "High neuron count with low simulation steps may cause insufficient learning";
                conflicts["max_simulation_steps"] = "Increase simulation steps for large networks";
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
    
    return conflicts;
}

std::vector<std::string> AdvancedConfiguration::getConfigurationRecommendations(
    const std::string& profileName) const {
    std::vector<std::string> recommendations;
    
    auto it = pImpl->profiles.find(normalizeProfileName(profileName));
    if (it == pImpl->profiles.end()) {
        return recommendations; // Empty if profile not found
    }
    
    const auto& profile = it->second;
    
    // Profile-specific recommendations
    if (profile.name == "research") {
        recommendations = {
            "Consider increasing neuron_count for more realistic models",
            "Enable development features for learning over time",
            "Set visualization_enabled=true for observation",
            "Use debug log level for detailed analysis"
        };
    } else if (profile.name == "production") {
        recommendations = {
            "Use moderate neuron_count for optimal performance",
            "Disable development features for stability",
            "Set visualization_enabled=false for headless operation",
            "Use info log level for operational monitoring"
        };
    } else if (profile.name == "high_performance") {
        recommendations = {
            "Optimize neuron_count for your specific use case",
            "Consider using a smaller connection_probability",
            "Disable visualization for better performance",
            "Use warn log level to reduce overhead"
        };
    }
    
    return recommendations;
}

std::vector<std::string> AdvancedConfiguration::getEnvironmentVariableRecommendations(
    const std::string& profileName) const {
    std::vector<std::string> recommendations;
    
    // Environment variable recommendations based on profile
    if (profileName == "research") {
        recommendations = {
            "NLM_RANDOM_SEED=12345  # For reproducible experiments",
            "NLM_NEURON_COUNT=5000    # Larger network for research",
            "NLM_ENABLE_DEVELOPMENT=true  # Enable learning dynamics"
        };
    } else if (profileName == "production") {
        recommendations = {
            "NLM_RANDOM_SEED=42  # Deterministic production behavior",
            "NLM_NEURON_COUNT=1000  # Efficient for production",
            "NLM_ENABLE_DEVELOPMENT=false  # Stable configuration"
        };
    }
    
    return recommendations;
}

std::vector<std::string> AdvancedConfiguration::getPerformanceRecommendations(
    const std::string& profileName) const {
    std::vector<std::string> recommendations;
    
    // Performance recommendations based on profile
    if (profileName == "high_performance") {
        recommendations = {
            "Reduce neuron_count if possible",
            "Consider using a smaller connection_probability",
            "Disable visualization to reduce overhead",
            "Use warn log level to minimize I/O"
        };
    }
    
    return recommendations;
}

void AdvancedConfiguration::initializeDefaults() {
    // Initialize with default environment variables
    auto defaultEnvVars = getDefaultEnvironmentVariables();
    for (const auto& envVar : defaultEnvVars) {
        pImpl->environmentVariables[envVar.name] = envVar;
    }
    
    // Initialize with default profiles
    auto defaultProfiles = getDefaultProfiles();
    for (const auto& profile : defaultProfiles) {
        pImpl->profiles[profile.name] = profile;
    }
    
    // Initialize with default configuration profiles
    auto defaultConfigProfiles = getDefaultConfigurationProfiles();
    for (const auto& configProfile : defaultConfigProfiles) {
        pImpl->configurationProfiles[configProfile.name] = configProfile;
    }
    
    // Initialize with default performance profiles
    auto defaultPerformanceProfiles = getDefaultPerformanceProfiles();
    for (const auto& perfProfile : defaultPerformanceProfiles) {
        pImpl->performanceProfiles[perfProfile.name] = perfProfile;
    }
    
    // Set active profiles to empty by default
    pImpl->activeProfiles.clear();
    pImpl->activeConfigProfiles.clear();
    pImpl->activePerfProfiles.clear();
    
    // Enable dynamic configuration by default
    pImpl->dynamicConfigurationEnabled = true;
}

std::vector<AdvancedConfiguration::EnvironmentVariable> AdvancedConfiguration::getDefaultEnvironmentVariables() const {
    return {
        {"NLM_RANDOM_SEED", "42", "Random seed for reproducible experiments", "random_seed", false, "^[0-9]+$"},
        {"NLM_NEURON_COUNT", "1000", "Number of neurons in the brain", "neuron_count", false, "^[0-9]+$"},
        {"NLM_CONNECTION_PROBABILITY", "0.1", "Connection probability for random connectivity", "connection_probability", false, "^([0-9]*[.])?[0-9]+$"},
        {"NLM_ENABLE_DEVELOPMENT", "false", "Whether development features are enabled", "enable_development", false, "^(true|false|1|0)$"},
        {"NLM_LOG_LEVEL", "INFO", "Log level (DEBUG, INFO, WARN, ERROR)", "log_level", false, "^(DEBUG|INFO|WARN|ERROR)$"}
    };
}

std::vector<AdvancedConfiguration::Profile> AdvancedConfiguration::getDefaultProfiles() const {
    return {
        {"basic", "Basic configuration for simple simulations", ProfileType::BASIC, true},
        {"high_performance", "Optimized for performance and speed", ProfileType::HIGH_PERFORMANCE, true},
        {"research", "Optimized for research and experimentation", ProfileType::RESEARCH, true},
        {"production", "Optimized for stable production use", ProfileType::PRODUCTION, true},
        {"custom", "Custom configuration template", ProfileType::CUSTOM, false}
    };
}

std::vector<AdvancedConfiguration::ConfigurationProfile> AdvancedConfiguration::getDefaultConfigurationProfiles() const {
    return {
        {"neural_simulation", "1.0", "NLM Team", "nlm.org", "Default neural simulation configuration", {}},
        {"agent_behavior", "1.2", "Research Team", "nlm.org", "Configuration for agent behavior studies", {}},
        {"memory_systems", "1.1", "Memory Research Group", "nlm.org", "Configuration for memory system research", {}},
        {"neuromodulation", "1.3", "Neuromodulation Lab", "nlm.org", "Configuration for neuromodulation studies", {}}
    };
}

std::vector<AdvancedConfiguration::PerformanceProfile> AdvancedConfiguration::getDefaultPerformanceProfiles() const {
    return {
        {"fast", 5.0, 50, 5, "Quick simulations for testing"},
        {"balanced", 10.0, 100, 10, "Balanced performance for general use"},
        {"thorough", 30.0, 200, 20, "Detailed simulations for research"},
        {"realtime", 1.0, 50, 5, "Near-realtime simulations"}
    };
}

bool AdvancedConfiguration::loadEnvironmentVariables(const std::vector<EnvironmentVariable>& envVars) {
    bool success = true;
    
    for (const auto& envVar : envVars) {
        if (envVar.required) {
            const char* envValue = std::getenv(envVar.name.c_str());
            if (!envValue) {
                NLM_LOG_WARN(std::string("Required environment variable not set: ") + envVar.name, 
                           "AdvancedConfiguration::loadEnvironmentVariables");
                success = false;
                continue;
            }
            
            setEnvironmentVariable(envVar.name, envValue);
        }
    }
    
    return success;
}

bool AdvancedConfiguration::validateEnvironmentVariable(const EnvironmentVariable& envVar) const {
    if (envVar.name.empty()) {
        return false;
    }
    
    if (envVar.required && envVar.value.empty()) {
        return false;
    }
    
    if (!envVar.validationPattern.empty()) {
        std::regex pattern(envVar.validationPattern);
        if (!std::regex_match(envVar.value, pattern)) {
            return false;
        }
    }
    
    return true;
}

bool AdvancedConfiguration::validateProfileName(const std::string& name) const {
    return isValidProfileName(name);
}

bool AdvancedConfiguration::validateProfileSettings(const std::map<std::string, std::string>& settings) const {
    for (const auto& setting : settings) {
        if (!isSettingValid(setting.first, setting.second)) {
            return false;
        }
    }
    
    return true;
}

bool AdvancedConfiguration::saveProfileToFile(const Profile& profile, const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Profile: " << profile.name << std::endl;
        file << "# Description: " << profile.description << std::endl;
        file << "# Type: " << static_cast<int>(profile.type) << std::endl;
        file << "# Built-in: " << (profile.isBuiltIn ? "true" : "false") << std::endl;
        file << "# Created: " << std::chrono::system_clock::to_time_t(profile.created) << std::endl;
        file << std::endl;
        
        file << "[settings]" << std::endl;
        for (const auto& setting : profile.settings) {
            file << setting.first << " = " << setting.second << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfiguration::loadProfileFromFile(Profile& profile, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line == "[settings]") {
                currentSection = "settings";
                continue;
            }
            
            if (currentSection == "settings") {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    profile.settings[key] = value;
                }
            } else {
                // Handle other sections (metadata)
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (key == "name") profile.name = value;
                    else if (key == "description") profile.description = value;
                    else if (key == "type") {
                        try { profile.type = static_cast<ProfileType>(std::stoi(value)); }
                        catch (...) {}
                    }
                    else if (key == "built_in") profile.isBuiltIn = (value == "true" || value == "1");
                    else if (key == "created") {
                        try { profile.created = std::chrono::system_clock::from_time_t(std::stoll(value)); }
                        catch (...) {}
                    }
                }
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfiguration::saveConfigurationProfileToFile(const ConfigurationProfile& profile, const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\"name\": \"" << profile.name << "\", \"version\": \"" << profile.version << "\", \"author\": \"" << profile.author << "\", \"organization\": \"" << profile.organization << "\", \"description\": \"" << profile.description << "\", \"created\": \"" << std::chrono::system_clock::to_time_t(profile.created) << "\", \"last_modified\": \"" << std::chrono::system_clock::to_time_t(profile.lastModified) << "\", \"dependencies\": [";
        
        for (size_t i = 0; i < profile.dependencies.size(); ++i) {
            if (i > 0) file << ", ";
            file << profile.dependencies[i];
        }
        
        file << "], \"settings\": {";
        
        for (size_t i = 0; i < profile.settings.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << profile.settings.begin()->first << "\": \"" << profile.settings.begin()->second << "\"";
            // Note: This is simplified; should iterate through all settings
        }
        
        file << "}}" << std::endl;
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfiguration::loadConfigurationProfileFromFile(ConfigurationProfile& profile, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // In a real implementation, this would parse JSON
        file.close();
        return false; // Placeholder
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfiguration::savePerformanceProfileToFile(const PerformanceProfile& profile, const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Performance Profile: " << profile.name << std::endl;
        file << "# Expected Update Time: " << profile.expectedUpdateTime << " ms" << std::endl;
        file << "# Expected Memory Usage: " << profile.expectedMemoryUsage << " MB" << std::endl;
        file << "# Expected CPU Usage: " << profile.expectedCpuUsage << "%" << std::endl;
        file << "# Use Case: " << profile.useCase << std::endl;
        file << std::endl;
        
        file << "[performance]" << std::endl;
        file << "expected_update_time = " << profile.expectedUpdateTime << std::endl;
        file << "expected_memory_usage = " << profile.expectedMemoryUsage << std::endl;
        file << "expected_cpu_usage = " << profile.expectedCpuUsage << std::endl;
        file << "use_case = " << profile.useCase << std::endl;
        
        file << std::endl;
        file << "[settings]" << std::endl;
        for (const auto& setting : profile.settings) {
            file << setting.first << " = " << setting.second << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool AdvancedConfiguration::loadPerformanceProfileFromFile(PerformanceProfile& profile, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line == "[performance]") {
                currentSection = "performance";
                continue;
            } else if (line == "[settings]") {
                currentSection = "settings";
                continue;
            }
            
            if (currentSection == "performance") {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (key == "expected_update_time") {
                        try { profile.expectedUpdateTime = std::stod(value); }
                        catch (...) {}
                    } else if (key == "expected_memory_usage") {
                        try { profile.expectedMemoryUsage = std::stoull(value); }
                        catch (...) {}
                    } else if (key == "expected_cpu_usage") {
                        try { profile.expectedCpuUsage = std::stod(value); }
                        catch (...) {}
                    } else if (key == "use_case") {
                        profile.useCase = value;
                    }
                }
            } else if (currentSection == "settings") {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    profile.settings[key] = value;
                }
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string AdvancedConfiguration::analyzeSettingImpact(const std::string& key, const std::string& value) const {
    std::stringstream impact;
    
    if (key == "neuron_count") {
        try {
            size_t count = std::stoull(value);
            if (count > 50000) {
                impact << "High memory usage and slow simulation";
            } else if (count > 10000) {
                impact << "Moderate memory usage and reasonable performance";
            } else {
                impact << "Low memory usage and good performance";
            }
        } catch (...) {
            impact << "Unknown impact";
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            if (prob > 0.5) {
                impact << "Dense connectivity, high connectivity but potential saturation";
            } else if (prob > 0.2) {
                impact << "Moderate connectivity, balanced network behavior";
            } else {
                impact << "Sparse connectivity, simpler network dynamics";
            }
        } catch (...) {
            impact << "Unknown impact";
        }
    } else if (key == "enable_development") {
        impact << "Enables developmental plasticity and learning over time";
    } else {
        impact << "Configuration setting";
    }
    
    return impact.str();
}

std::string AdvancedConfiguration::getSettingDescription(const std::string& key) const {
    if (key == "random_seed") {
        return "Random seed for reproducible experiments (0-2^63-1)";
    } else if (key == "neuron_count") {
        return "Number of neurons in the brain (100-100,000 recommended)";
    } else if (key == "region_count") {
        return "Number of brain regions (1-10 recommended)";
    } else if (key == "connection_probability") {
        return "Connection probability for random connectivity (0.0-1.0)";
    } else if (key == "enable_development") {
        return "Whether development features are enabled (true/false)";
    } else if (key == "log_level") {
        return "Log level (DEBUG, INFO, WARN, ERROR)";
    }
    
    return "";
}

std::string AdvancedConfiguration::getSettingType(const std::string& key) const {
    if (key == "random_seed" || key == "neuron_count" || key == "region_count" || 
        key == "max_simulation_steps") {
        return "int";
    } else if (key == "simulation_timestep" || key == "connection_probability" || 
               key == "reward_discount_factor") {
        return "double";
    } else if (key == "enable_development" || key == "visualization_enabled" || 
               key == "log_to_file") {
        return "bool";
    } else if (key == "log_level" || key == "environment_name") {
        return "string";
    }
    
    return "string";
}

bool AdvancedConfiguration::isSettingValid(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return false;
    }
    
    std::string type = getSettingType(key);
    
    if (type == "int") {
        try {
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            return pos == value.length();
        } catch (...) {
            return false;
        }
    } else if (type == "double") {
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            return pos == value.length();
        } catch (...) {
            return false;
        }
    } else if (type == "bool") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    
    return true; // string type always valid
}

std::string AdvancedConfiguration::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

double AdvancedConfiguration::calculateProfileScore(const Profile& profile, const std::string& useCase) const {
    double score = 0.0;
    
    // Check profile type matches use case
    if (useCase == "research" && profile.type == ProfileType::RESEARCH) {
        score += 1.0;
    } else if (useCase == "production" && profile.type == ProfileType::PRODUCTION) {
        score += 0.8;
    } else if (useCase == "high_performance" && profile.type == ProfileType::HIGH_PERFORMANCE) {
        score += 0.9;
    } else if (useCase == "basic" && profile.type == ProfileType::BASIC) {
        score += 0.7;
    }
    
    // Check if profile has essential settings
    std::vector<std::string> essentialSettings = {"neuron_count", "region_count", "connection_probability"};
    size_t presentEssential = 0;
    for (const auto& setting : essentialSettings) {
        if (profile.settings.find(setting) != profile.settings.end()) {
            presentEssential++;
        }
    }
    
    score += (static_cast<double>(presentEssential) / essentialSettings.size()) * 0.2;
    
    return score;
}

AdvancedConfiguration& AdvancedConfiguration::getInstance() {
    static AdvancedConfiguration instance;
    return instance;
}

void AdvancedConfiguration::resetInstance() {
    // Note: This is mainly for testing; in production, we typically want a singleton
    // that persists throughout the application lifetime.
}

} // namespace nlm
