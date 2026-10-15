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

// Web interface user session
struct WebInterfaceSession {
    std::string sessionId;
    std::string userId;
    std::string experimentName;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastActivity;
    std::string ipAddress;
    std::string userAgent;
    std::map<std::string, std::string> sessionData;
    
    WebInterfaceSession() : created(std::chrono::system_clock::now()),
                           lastActivity(std::chrono::system_clock::now()) {}
    WebInterfaceSession(const std::string& sessionId, const std::string& userId,
                       const std::string& experimentName)
        : sessionId(sessionId), userId(userId), experimentName(experimentName),
          created(std::chrono::system_clock::now()),
          lastActivity(std::chrono::system_clock::now()) {}
};

// Web interface page
struct WebInterfacePage {
    std::string pageId;
    std::string name;
    std::string title;
    std::string url;
    std::string htmlContent;
    std::map<std::string, std::string> cssStyles;
    std::map<std::string, std::string> jsScripts;
    std::map<std::string, std::string> pageData;
    bool isActive;
    
    WebInterfacePage() : isActive(true) {}
    WebInterfacePage(const std::string& pageId, const std::string& name, const std::string& title)
        : pageId(pageId), name(name), title(title), isActive(true) {}
};

// Web interface component
struct WebInterfaceComponent {
    std::string componentId;
    std::string name;
    std::string type; // "chart", "table", "form", "widget", "dashboard"
    std::string htmlTemplate;
    std::map<std::string, std::string> componentData;
    std::map<std::string, std::string> componentConfig;
    bool isVisible;
    
    WebInterfaceComponent() : isVisible(true) {}
    WebInterfaceComponent(const std::string& componentId, const std::string& name, const std::string& type)
        : componentId(componentId), name(name), type(type), isVisible(true) {}
};

class ExperimentWebInterface {
public:
    ExperimentWebInterface();
    ~ExperimentWebInterface();
    
    // Disable copying, enable moving
    ExperimentWebInterface(const ExperimentWebInterface&) = delete;
    ExperimentWebInterface& operator=(const ExperimentWebInterface&) = delete;
    ExperimentWebInterface(ExperimentWebInterface&&) noexcept;
    ExperimentWebInterface& operator=(ExperimentWebInterface&&) noexcept;
    
    // Web interface management
    bool initializeWebInterface(const std::string& webConfigPath = "");
    bool shutdownWebInterface();
    bool isWebInterfaceInitialized() const;
    
    // Session management
    bool createWebInterfaceSession(const std::string& userId, const std::string& experimentName,
                                  const std::string& ipAddress = "", const std::string& userAgent = "");
    bool terminateWebInterfaceSession(const std::string& sessionId);
    bool terminateAllUserSessions(const std::string& userId);
    WebInterfaceSession getWebInterfaceSession(const std::string& sessionId) const;
    std::vector<WebInterfaceSession> getUserSessions(const std::string& userId) const;
    std::vector<WebInterfaceSession> getExperimentSessions(const std::string& experimentName) const;
    
    // Page management
    bool addWebInterfacePage(const WebInterfacePage& page);
    bool removeWebInterfacePage(const std::string& pageId);
    bool updateWebInterfacePage(const std::string& pageId, const WebInterfacePage& page);
    WebInterfacePage getWebInterfacePage(const std::string& pageId) const;
    std::vector<WebInterfacePage> getAllWebInterfacePages() const;
    std::vector<WebInterfacePage> getActiveWebInterfacePages() const;
    
    // Component management
    bool addWebInterfaceComponent(const WebInterfaceComponent& component);
    bool removeWebInterfaceComponent(const std::string& componentId);
    bool updateWebInterfaceComponent(const std::string& componentId, const WebInterfaceComponent& component);
    WebInterfaceComponent getWebInterfaceComponent(const std::string& componentId) const;
    std::vector<WebInterfaceComponent> getAllWebInterfaceComponents() const;
    
    // Web interface serving
    std::string serveWebInterface(const std::string& pageId, const std::string& userId) const;
    std::string serveComponent(const std::string& componentId, const std::string& userId) const;
    std::string handleWebInterfaceRequest(const std::string& requestUrl, const std::string& userId) const;
    
    // Web interface analytics
    std::vector<WebInterfaceSession> getWebInterfaceSessions(const std::chrono::system_clock::time_point& fromTime,
                                                            const std::chrono::system_clock::time_point& toTime) const;
    std::map<std::string, double> calculateWebInterfacePerformance(const std::string& pageId) const;
    std::vector<std::string> getPopularWebInterfacePages() const;
    
    // Web interface backup and recovery
    bool backupWebInterfaceData(const std::string& backupPath) const;
    bool restoreWebInterfaceData(const std::string& backupPath);
    bool exportWebInterfaceToJSON(const std::string& filepath) const;
    bool importWebInterfaceFromJSON(const std::string& filepath);
    
    // Web interface cleanup
    bool cleanupWebInterfaceSessions(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetWebInterface(const std::string& pageId);
    bool clearAllWebInterfaceData();
    
    // System management
    bool startWebInterfaceServer();
    bool stopWebInterfaceServer();
    bool isWebInterfaceServerRunning() const;
    
    // Web interface utilities
    static std::string generatePageId(const std::string& name) const;
    static std::string generateComponentId(const std::string& name) const;
    static std::string generateSessionId() const;
    static bool isValidPageId(const std::string& pageId) const;
    static bool isValidComponentId(const std::string& componentId) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm