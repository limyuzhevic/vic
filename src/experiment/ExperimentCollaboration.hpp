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
#include <unordered_set>
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

// Collaboration user
struct CollaborationUser {
    std::string userId;
    std::string username;
    std::string email;
    std::vector<std::string> experimentIds;
    std::vector<std::string> projectIds;
    std::chrono::system_clock::time_point lastLogin;
    std::map<std::string, std::string> userMetadata;
    
    CollaborationUser() : lastLogin(std::chrono::system_clock::now()) {}
    CollaborationUser(const std::string& id, const std::string& name, const std::string& email)
        : userId(id), username(name), email(email), lastLogin(std::chrono::system_clock::now()) {}
};

// Collaboration project
struct CollaborationProject {
    std::string projectId;
    std::string name;
    std::string description;
    std::string ownerId;
    std::vector<std::string> memberIds;
    std::vector<std::string> experimentIds;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::map<std::string, std::string> projectMetadata;
    
    CollaborationProject() : created(std::chrono::system_clock::now()), 
                            lastModified(std::chrono::system_clock::now()) {}
    CollaborationProject(const std::string& id, const std::string& name, const std::string& ownerId)
        : projectId(id), name(name), ownerId(ownerId),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
};

// Collaboration permissions
enum class CollaborationPermission {
    Read,
    Write,
    Execute,
    Admin,
    Share,
    Collaborate,
    Comment,
    Invite,
    Manage
};

// Collaboration role
struct CollaborationRole {
    std::string roleId;
    std::string name;
    std::string description;
    std::vector<CollaborationPermission> permissions;
    
    CollaborationRole() {}
    CollaborationRole(const std::string& id, const std::string& name, const std::string& desc)
        : roleId(id), name(name), description(desc) {}
};

// Collaboration invitation
struct CollaborationInvitation {
    std::string invitationId;
    std::string projectId;
    std::string senderId;
    std::string receiverId;
    CollaborationRole role;
    std::string message;
    std::chrono::system_clock::time_point sentTime;
    std::chrono::system_clock::time_point expiresTime;
    std::string status; // "pending", "accepted", "rejected", "cancelled"
    
    CollaborationInvitation() : sentTime(std::chrono::system_clock::now()), 
                               expiresTime(std::chrono::system_clock::now() + std::chrono::hours(24)),
                               status("pending") {}
    CollaborationInvitation(const std::string& id, const std::string& projectId, 
                           const std::string& senderId, const std::string& receiverId)
        : invitationId(id), projectId(projectId), senderId(senderId), receiverId(receiverId),
          sentTime(std::chrono::system_clock::now()),
          expiresTime(std::chrono::system_clock::now() + std::chrono::hours(24)),
          status("pending") {}
};

class ExperimentCollaboration {
public:
    ExperimentCollaboration();
    ~ExperimentCollaboration();
    
    // Disable copying, enable moving
    ExperimentCollaboration(const ExperimentCollaboration&) = delete;
    ExperimentCollaboration& operator=(const ExperimentCollaboration&) = delete;
    ExperimentCollaboration(ExperimentCollaboration&&) noexcept;
    ExperimentCollaboration& operator=(ExperimentCollaboration&&) noexcept;
    
    // User management
    bool addUser(const CollaborationUser& user);
    bool removeUser(const std::string& userId);
    bool updateUser(const std::string& userId, const CollaborationUser& user);
    CollaborationUser getUser(const std::string& userId) const;
    std::vector<CollaborationUser> getAllUsers() const;
    std::vector<CollaborationUser> getUsersByProject(const std::string& projectId) const;
    
    // Role management
    bool addRole(const CollaborationRole& role);
    bool removeRole(const std::string& roleId);
    bool updateRole(const std::string& roleId, const CollaborationRole& role);
    CollaborationRole getRole(const std::string& roleId) const;
    std::vector<CollaborationRole> getAllRoles() const;
    
    // Project management
    bool createProject(const CollaborationProject& project);
    bool removeProject(const std::string& projectId);
    bool updateProject(const std::string& projectId, const CollaborationProject& project);
    CollaborationProject getProject(const std::string& projectId) const;
    std::vector<CollaborationProject> getAllProjects() const;
    std::vector<CollaborationProject> getUserProjects(const std::string& userId) const;
    
    // Invitation management
    bool sendInvitation(const CollaborationInvitation& invitation);
    bool acceptInvitation(const std::string& invitationId, const std::string& userId);
    bool rejectInvitation(const std::string& invitationId);
    bool cancelInvitation(const std::string& invitationId);
    CollaborationInvitation getInvitation(const std::string& invitationId) const;
    std::vector<CollaborationInvitation> getUserInvitations(const std::string& userId) const;
    
    // Permission management
    bool grantPermission(const std::string& userId, const std::string& projectId, 
                        CollaborationPermission permission);
    bool revokePermission(const std::string& userId, const std::string& projectId, 
                         CollaborationPermission permission);
    bool hasPermission(const std::string& userId, const std::string& projectId, 
                      CollaborationPermission permission) const;
    std::vector<CollaborationPermission> getUserProjectPermissions(const std::string& userId, 
                                                                  const std::string& projectId) const;
    
    // Collaboration features
    bool addExperimentToProject(const std::string& projectId, const std::string& experimentName);
    bool removeExperimentFromProject(const std::string& projectId, const std::string& experimentName);
    std::vector<std::string> getProjectExperiments(const std::string& projectId) const;
    
    bool shareExperiment(const std::string& experimentName, const std::string& projectId, 
                        const std::string& userId);
    bool unshareExperiment(const std::string& experimentName, const std::string& projectId, 
                          const std::string& userId);
    
    bool commentOnExperiment(const std::string& experimentName, const std::string& userId, 
                           const std::string& comment);
    std::vector<std::string> getExperimentComments(const std::string& experimentName) const;
    
    // Collaboration analytics
    std::vector<CollaborationUser> getActiveUsers() const;
    std::map<std::string, size_t> getProjectMemberCounts() const;
    std::chrono::system_clock::time_point getLastActivityTime(const std::string& userId) const;
    
    // Collaboration backup and recovery
    bool backupCollaborationData(const std::string& backupPath) const;
    bool restoreCollaborationData(const std::string& backupPath);
    bool exportCollaborationToJSON(const std::string& filepath) const;
    bool importCollaborationFromJSON(const std::string& filepath);
    
    // Collaboration cleanup
    bool cleanupInactiveUsers(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupExpiredInvitations(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetProject(const std::string& projectId);
    bool clearAllCollaborationData();
    
    // Collaboration system management
    bool initializeCollaborationSystem();
    bool shutdownCollaborationSystem();
    bool isCollaborationSystemInitialized() const;
    
    // Collaboration utilities
    static std::string generateUserId() const;
    static std::string generateProjectId() const;
    static std::string generateInvitationId() const;
    static bool isValidUserId(const std::string& userId) const;
    static bool isValidProjectId(const std::string& projectId) const;
    static bool isValidInvitationId(const std::string& invitationId) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm