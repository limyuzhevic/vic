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
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Authorization decision
enum class AuthorizationDecision {
    Allow,
    Deny,
    Conditional,
    NotApplicable,
    Error
};

// Authorization condition
struct AuthorizationCondition {
    std::string conditionId;
    std::string conditionType; // "role", "attribute", "policy", "time", "location"
    std::string conditionValue;
    std::map<std::string, std::string> conditionParameters;
    bool isRequired;
    
    AuthorizationCondition() : isRequired(false) {}
    AuthorizationCondition(const std::string& id, const std::string& type, const std::string& value)
        : conditionId(id), conditionType(type), conditionValue(value), isRequired(false) {}
};

// Authorization request
struct AuthorizationRequest {
    std::string requestId;
    std::string userId;
    std::string experimentName;
    AccessPermission requestedPermission;
    std::map<std::string, std::string> requestContext;
    std::chrono::system_clock::time_point requestTime;
    std::string clientIp;
    std::string userAgent;
    
    AuthorizationRequest() : requestTime(std::chrono::system_clock::now()) {}
    AuthorizationRequest(const std::string& requestId, const std::string& userId,
                        const std::string& experimentName, AccessPermission permission)
        : requestId(requestId), userId(userId), experimentName(experimentName),
          requestedPermission(permission), requestTime(std::chrono::system_clock::now()) {}
};

// Authorization policy
struct AuthorizationPolicy {
    std::string policyId;
    std::string name;
    std::string description;
    bool isActive;
    std::vector<AuthorizationCondition> conditions;
    std::map<std::string, std::string> policyRules;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string createdBy;
    
    AuthorizationPolicy() : isActive(true), created(std::chrono::system_clock::now()), 
                           lastModified(std::chrono::system_clock::now()) {}
    AuthorizationPolicy(const std::string& id, const std::string& name, const std::string& desc)
        : policyId(id), name(name), description(desc), isActive(true),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
};

// Conditional authorization
struct ConditionalAuthorization {
    std::string conditionId;
    std::string experimentName;
    std::string userId;
    AccessPermission permission;
    std::map<std::string, std::string> conditionValues;
    std::chrono::system_clock::time_point expiryTime;
    std::string conditionDetails;
    bool isActive;
    
    ConditionalAuthorization() : isActive(true), expiryTime(std::chrono::system_clock::now() + std::chrono::days(30)) {}
    ConditionalAuthorization(const std::string& id, const std::string& expName, 
                            const std::string& userId, AccessPermission permission)
        : conditionId(id), experimentName(expName), userId(userId), permission(permission),
          isActive(true), expiryTime(std::chrono::system_clock::now() + std::chrono::days(30)) {}
};

class ExperimentAuthorization {
public:
    ExperimentAuthorization();
    ~ExperimentAuthorization();
    
    // Disable copying, enable moving
    ExperimentAuthorization(const ExperimentAuthorization&) = delete;
    ExperimentAuthorization& operator=(const ExperimentAuthorization&) = delete;
    ExperimentAuthorization(ExperimentAuthorization&&) noexcept;
    ExperimentAuthorization& operator=(ExperimentAuthorization&&) noexcept;
    
    // Policy management
    bool addAuthorizationPolicy(const AuthorizationPolicy& policy);
    bool removeAuthorizationPolicy(const std::string& policyId);
    bool updateAuthorizationPolicy(const std::string& policyId, const AuthorizationPolicy& policy);
    AuthorizationPolicy getAuthorizationPolicy(const std::string& policyId) const;
    std::vector<AuthorizationPolicy> getAllAuthorizationPolicies() const;
    std::vector<AuthorizationPolicy> getActiveAuthorizationPolicies() const;
    
    // Conditional authorization
    bool createConditionalAuthorization(const ConditionalAuthorization& auth);
    bool removeConditionalAuthorization(const std::string& conditionId);
    bool updateConditionalAuthorization(const std::string& conditionId, const ConditionalAuthorization& auth);
    ConditionalAuthorization getConditionalAuthorization(const std::string& conditionId) const;
    std::vector<ConditionalAuthorization> getConditionalAuthorizations(const std::string& experimentName) const;
    std::vector<ConditionalAuthorization> getUserConditionalAuthorizations(const std::string& userId) const;
    
    // Authorization requests
    bool createAuthorizationRequest(const AuthorizationRequest& request);
    bool processAuthorizationRequest(const std::string& requestId);
    bool denyAuthorizationRequest(const std::string& requestId, const std::string& reason);
    AuthorizationRequest getAuthorizationRequest(const std::string& requestId) const;
    std::vector<AuthorizationRequest> getPendingAuthorizationRequests(const std::string& experimentName) const;
    
    // Authorization decisions
    bool makeAuthorizationDecision(const std::string& requestId, AuthorizationDecision decision,
                                 const std::string& performedBy);
    AuthorizationDecision getAuthorizationDecision(const std::string& requestId) const;
    std::string getAuthorizationDecisionReason(const std::string& requestId) const;
    
    // Conditional authorization evaluation
    bool evaluateConditionalAuthorization(const std::string& conditionId);
    bool updateConditionalAuthorizationStatus(const std::string& conditionId, bool active);
    bool refreshConditionalAuthorization(const std::string& conditionId);
    bool expireConditionalAuthorization(const std::string& conditionId);
    
    // Policy evaluation
    bool evaluateAuthorizationPolicy(const std::string& policyId, const std::string& experimentName,
                                   const std::string& userId) const;
    std::map<std::string, std::string> evaluatePolicyRules(const std::string& policyId, 
                                                        const std::string& experimentName,
                                                        const std::string& userId) const;
    std::vector<std::string> getPolicyViolations(const std::string& policyId, 
                                               const std::string& experimentName,
                                               const std::string& userId) const;
    
    // Authorization analytics
    std::vector<AuthorizationRequest> getAuthorizationRequestsByExperiment(const std::string& experimentName) const;
    std::vector<ConditionalAuthorization> getAuthorizationHistory(const std::string& experimentName) const;
    std::map<std::string, double> calculateAuthorizationCompliance(const std::string& experimentName) const;
    
    // Backup and recovery
    bool backupAuthorizationData(const std::string& backupPath) const;
    bool restoreAuthorizationData(const std::string& backupPath);
    bool exportAuthorizationToJSON(const std::string& filepath) const;
    bool importAuthorizationFromJSON(const std::string& filepath);
    
    // Cleanup and maintenance
    bool cleanupExpiredConditionalAuthorizations(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupProcessedAuthorizationRequests(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetAuthorization(const std::string& experimentName);
    bool clearAllAuthorizationData();
    
    // System management
    bool initializeAuthorizationSystem();
    bool shutdownAuthorizationSystem();
    bool isAuthorizationSystemInitialized() const;
    
    // Utilities
    static std::string generateAuthorizationPolicyId(const std::string& name) const;
    static std::string generateConditionalAuthorizationId(const std::string& userId) const;
    static std::string generateAuthorizationRequestId(const std::string& userId) const;
    static bool isValidAuthorizationPolicyId(const std::string& policyId) const;
    static bool isValidConditionalAuthorizationId(const std::string& conditionId) const;
    static AuthorizationDecision stringToAuthorizationDecision(const std::string& decisionStr) const;
    static std::string authorizationDecisionToString(AuthorizationDecision decision) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm