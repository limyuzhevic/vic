#pragma once

/**
 * User Configuration
 * 
 * User-specific configuration management for the NLM brain simulation project,
 * providing configuration for individual user preferences and settings.
 * 
 * Features:
 * - User-specific configuration management
 * - User preference configuration
 * - User profile configuration
 * - User session configuration
 * - User authentication configuration
 * - User access control configuration
 * - User logging configuration
 * - User interface configuration
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace system {

class ConfigurationManager;

/**
 * User role
 */
enum class UserRole {
    ADMIN,
    USER,
    GUEST,
    SERVICE_ACCOUNT,
    SYSTEM,
    CUSTOM
};

/**
 * User configuration metadata
 */
struct UserConfigMetadata {
    std::string userId;
    std::string username;
    UserRole role;
    std::string email;
    bool isActive;
    double createdAt;
    double lastLogin;
    std::map<std::string, std::string> userAttributes;
    
    UserConfigMetadata(const std::string& id, const std::string& name)
        : userId(id), username(name), role(UserRole::USER), isActive(true),
          createdAt(0.0), lastLogin(0.0) {}
};

/**
 * User configuration
 */
class UserConfiguration {
public:
    UserConfiguration();
    ~UserConfiguration();
    
    // Disable copying, enable moving
    UserConfiguration(const UserConfiguration&) = delete;
    UserConfiguration& operator=(const UserConfiguration&) = delete;
    UserConfiguration(UserConfiguration&&) noexcept = default;
    UserConfiguration& operator=(UserConfiguration&&) noexcept = default;
    
    // User management
    bool addUser(const UserConfigMetadata& user);
    bool removeUser(const std::string& userId);
    UserConfigMetadata* getUser(const std::string& userId);
    const UserConfigMetadata* getUser(const std::string& userId) const;
    std::vector<std::string> getUserIds() const;
    
    // User session management
    bool createSession(const std::string& userId, const std::string& sessionId);
    bool removeSession(const std::string& sessionId);
    std::string getCurrentSession(const std::string& userId) const;
    bool isUserAuthenticated(const std::string& userId) const;
    
    // User configuration values
    void setUserValue(const std::string& userId, const std::string& key, const std::string& value);
    std::string getUserValue(const std::string& userId, const std::string& key) const;
    bool hasUserValue(const std::string& userId, const std::string& key) const;
    
    // User preference management
    bool setUserPreference(const std::string& userId, const std::string& key, const std::string& value);
    std::string getUserPreference(const std::string& userId, const std::string& key) const;
    std::map<std::string, std::string> getAllUserPreferences(const std::string& userId) const;
    
    // User interface configuration
    bool configureUI(const std::string& userId, const std::map<std::string, std::string>& uiSettings);
    std::map<std::string, std::string> getUISettings(const std::string& userId) const;
    
    // User logging configuration
    bool configureLogging(const std::string& userId, const std::map<std::string, bool>& loggingSettings);
    std::map<std::string, bool> getLoggingSettings(const std::string& userId) const;
    
    // User authentication configuration
    bool configureAuthentication(const std::string& userId, const std::map<std::string, std::string>& authSettings);
    std::map<std::string, std::string> getAuthenticationSettings(const std::string& userId) const;
    
    // User access control configuration
    bool configureAccessControl(const std::string& userId, const std::map<std::string, bool>& accessSettings);
    std::map<std::string, bool> getAccessControlSettings(const std::string& userId) const;
    
    // User validation
    bool validateUser(const std::string& userId) const;
    std::vector<std::string> getUserErrors(const std::string& userId) const;
    
    // User loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // User merging
    bool merge(const UserConfiguration& otherConfig);
    bool applyDefaults();
    
    // User statistics
    size_t getUserCount() const { return users_.size(); }
    size_t getActiveUserCount() const;
    size_t getSessionCount() const { return sessions_.size(); }
    
    // User monitoring
    bool enableUserMonitoring(const std::string& userId, bool enable);
    bool isUserMonitoringEnabled(const std::string& userId) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    std::map<std::string, UserConfigMetadata> users_;
    std::map<std::string, std::string> userSessions_;
    std::map<std::string, std::map<std::string, std::string>> userValues_;
    std::map<std::string, std::map<std::string, std::string>> userPreferences_;
    
    friend class ConfigurationManager;
};

} // namespace system
} // namespace nlm
