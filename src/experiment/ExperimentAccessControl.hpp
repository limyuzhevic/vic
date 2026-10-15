#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentSecurity.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Access control models
enum class AccessControlModel {
    RoleBasedAccessControl,
    AttributeBasedAccessControl,
    PolicyBasedAccessControl,
    MandatoryAccessControl,
    DiscretionaryAccessControl,
    HybridAccessControl
};

// Authentication method
enum class AuthenticationMethod {
    Password,
    Biometric,
    Token,
    Certificate,
    MultiFactor,
    OAuth,
    SAML,
    LDAP,
    Kerberos,
    APIKey
};

// Authentication result
struct AuthenticationResult {
    std::string authenticationId;
    std::string userId;
    std::string experimentName;
    bool success;
    std::chrono::system_clock::time_point timestamp;
    std::string sessionToken;
    std::chrono::system_clock::time_point sessionExpiry;
    std::map<std::string, std::string> authenticationData;
    std::string errorMessage;
    
    AuthenticationResult() : success(false), timestamp(std::chrono::system_clock::now()),
                            sessionExpiry(std::chrono::system_clock::now()) {}
    AuthenticationResult(const std::string& authId, const std::string& userId,
                        const std::string& experimentName, bool success)
        : authenticationId(authId), userId(userId), experimentName(experimentName),
          success(success), timestamp(std::chrono::system_clock::now()),
          sessionExpiry(std::chrono::system_clock::now()) {}
};

// Authorization result
struct AuthorizationResult {
    std::string authorizationId;
    std::string userId;
    std::string experimentName;
    bool granted;
    std::chrono::system_clock::time_point timestamp;
    std::vector<AccessPermission> grantedPermissions;
    std::vector<AccessPermission> deniedPermissions;
    std::map<std::string, std::string> authorizationData;
    std::string reason;
    
    AuthorizationResult() : granted(false), timestamp(std::chrono::system_clock::now()) {}
    AuthorizationResult(const std::string& authId, const std::string& userId,
                       const std::string& experimentName, bool granted,
                       const std::string& reason)
        : authorizationId(authId), userId(userId), experimentName(experimentName),
          granted(granted), timestamp(std::chrono::system_clock::now()),
          reason(reason) {}
};

// User session
struct UserSession {
    std::string sessionId;
    std::string userId;
    std::string experimentName;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastAccess;
    std::chrono::system_clock::time_point expires;
    std::map<std::string, std::string> sessionData;
    std::string ipAddress;
    std::string userAgent;
    
    UserSession() : created(std::chrono::system_clock::now()),
                   lastAccess(std::chrono::system_clock::now()),
                   expires(std::chrono::system_clock::now() + std::chrono::hours(24)) {}
    UserSession(const std::string& sessionId, const std::string& userId,
               const std::string& experimentName)
        : sessionId(sessionId), userId(userId), experimentName(experimentName),
          created(std::chrono::system_clock::now()),
          lastAccess(std::chrono::system_clock::now()),
          expires(std::chrono::system_clock::now() + std::chrono::hours(24)) {}
};

// User profile
struct UserProfile {
    std::string userId;
    std::string username;
    std::string email;
    std::vector<std::string> experimentIds;
    std::vector<std::string> projectIds;
    std::map<std::string, std::string> profileData;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastLogin;
    bool isActive;
    bool isLocked;
    
    UserProfile() : created(std::chrono::system_clock::now()),
                   lastLogin(std::chrono::system_clock::now()),
                   isActive(true), isLocked(false) {}
    UserProfile(const std::string& userId, const std::string& username, const std::string& email)
        : userId(userId), username(username), email(email),
          created(std::chrono::system_clock::now()),
          lastLogin(std::chrono::system_clock::now()),
          isActive(true), isLocked(false) {}
};

class ExperimentAccessControl {
public:
    ExperimentAccessControl();
    ~ExperimentAccessControl();
    
    // Disable copying, enable moving
    ExperimentAccessControl(const ExperimentAccessControl&) = delete;
    ExperimentAccessControl& operator=(const ExperimentAccessControl&) = delete;
    ExperimentAccessControl(ExperimentAccessControl&&) noexcept;
    ExperimentAccessControl& operator=(ExperimentAccessControl&&) noexcept;
    
    // Authentication
    bool authenticateUser(const std::string& username, const std::string& password,
                        const std::string& experimentName = "");
    bool authenticateUser(const std::string& username, AuthenticationMethod method,
                        const std::map<std::string, std::string>& credentials,
                        const std::string& experimentName = "");
    AuthenticationResult getAuthenticationResult(const std::string& authenticationId) const;
    bool validateAuthenticationToken(const std::string& token, const std::string& experimentName = "") const;
    std::string generateAuthenticationToken(const std::string& userId, 
                                           const std::string& experimentName = "") const;
    
    // Authorization
    bool authorizeUser(const std::string& userId, const std::string& experimentName,
                      const std::vector<AccessPermission>& permissions);
    AuthorizationResult checkAuthorization(const std::string& userId, 
                                          const std::string& experimentName,
                                          AccessPermission permission) const;
    bool grantPermission(const std::string& userId, const std::string& experimentName,
                       AccessPermission permission, const std::string& grantedBy);
    bool revokePermission(const std::string& userId, const std::string& experimentName,
                         AccessPermission permission);
    std::vector<AccessPermission> getUserPermissions(const std::string& userId,
                                                    const std::string& experimentName) const;
    
    // Session management
    bool createUserSession(const std::string& userId, const std::string& experimentName,
                          const std::string& ipAddress = "", const std::string& userAgent = "");
    bool terminateUserSession(const std::string& sessionId);
    bool terminateAllUserSessions(const std::string& userId);
    UserSession getUserSession(const std::string& sessionId) const;
    std::vector<UserSession> getUserSessions(const std::string& userId) const;
    std::vector<UserSession> getExperimentSessions(const std::string& experimentName) const;
    
    // User management
    bool createUser(const UserProfile& userProfile);
    bool removeUser(const std::string& userId);
    bool updateUser(const std::string& userId, const UserProfile& userProfile);
    UserProfile getUser(const std::string& userId) const;
    std::vector<UserProfile> getAllUsers() const;
    std::vector<UserProfile> searchUsers(const std::string& query) const;
    
    // User account management
    bool activateUser(const std::string& userId);
    bool deactivateUser(const std::string& userId);
    bool lockUserAccount(const std::string& userId);
    bool unlockUserAccount(const std::string& userId);
    bool resetUserPassword(const std::string& userId, const std::string& newPassword);
    bool changeUserPassword(const std::string& userId, const std::string& oldPassword,
                           const std::string& newPassword);
    
    // Access control models
    bool setAccessControlModel(const std::string& experimentName, AccessControlModel model);
    AccessControlModel getAccessControlModel(const std::string& experimentName) const;
    bool configureAccessControlPolicy(const std::string& experimentName, 
                                     const std::map<std::string, std::string>& policy);
    std::map<std::string, std::string> getAccessControlPolicy(const std::string& experimentName) const;
    
    // Compliance and auditing
    bool logAccessAttempt(const std::string& userId, const std::string& experimentName,
                         AccessPermission permission, bool success, const std::string& reason = "");
    std::vector<std::string> getAccessViolations(const std::string& experimentName) const;
    bool generateAccessReport(const std::string& experimentName, const std::string& outputPath) const;
    
    // Backup and recovery
    bool backupAccessControlData(const std::string& backupPath) const;
    bool restoreAccessControlData(const std::string& backupPath);
    bool exportAccessControlToJSON(const std::string& filepath) const;
    bool importAccessControlFromJSON(const std::string& filepath);
    
    // Cleanup and maintenance
    bool cleanupExpiredSessions(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupInactiveUsers(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetAccessControl(const std::string& experimentName);
    bool clearAllAccessControlData();
    
    // System management
    bool initializeAccessControlSystem();
    bool shutdownAccessControlSystem();
    bool isAccessControlSystemInitialized() const;
    
    // Utilities
    static std::string generateUserId(const std::string& prefix = "user") const;
    static std::string generateSessionId() const;
    static bool isValidUserId(const std::string& userId) const;
    static bool isValidUsername(const std::string& username) const;
    static AccessControlModel stringToAccessControlModel(const std::string& modelStr) const;
    static std::string accessControlModelToString(AccessControlModel model) const;
    static AuthenticationMethod stringToAuthenticationMethod(const std::string& methodStr) const;
    static std::string authenticationMethodToString(AuthenticationMethod method) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm