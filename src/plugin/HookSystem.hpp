//! Hook System for NLM Neural Simulation Project
//! 
//! This module implements the HookSystem class which provides extension points
//! throughout the NLM core systems. The HookSystem allows plugins to:
//! - Add/modify/remove plasticity rules
//! - Add/modify/remove neuromodulators
//! - Extend developmental processes
//! - Add/modify memory systems
//! - Extend prediction systems
//! - Add/modify cognitive mechanisms
//! - Modify sensory processing
//! - Modify motor output
//! - Integrate with external systems
//! 
//! The HookSystem uses a callback-based architecture with event propagation,
//! allowing plugins to inject behavior at specific points in the NLM systems
//! while maintaining separation of concerns and enabling modular extensions.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <any>
#include <string>
#include <algorithm>
#include <type_traits>
#include <memory>

namespace nlm {

class HookSystem {
public:
    HookSystem();
    ~HookSystem();
    
    // Disable copying
    HookSystem(const HookSystem&) = delete;
    HookSystem& operator=(const HookSystem&) = delete;
    
    // Enable moving
    HookSystem(HookSystem&&) noexcept = default;
    HookSystem& operator=(HookSystem&&) noexcept = default;
    
    // Hook registration
    bool registerHook(PluginHook hook, const std::function<bool(const HookEventData&)>& callback,
                     const std::string& pluginName = "", int priority = 0);
    bool registerHook(const std::string& pluginName, PluginHook hook,
                     const std::function<bool(const HookEventData&)>& callback,
                     int priority = 0);
    bool unregisterHook(PluginHook hook, const std::string& pluginName = "");
    bool unregisterAllHooks(const std::string& pluginName);
    bool unregisterAllHooks(PluginHook hook);
    
    // Hook invocation
    bool invokeHook(PluginHook hook, HookEventData& data, const std::string& sourcePlugin = "");
    bool invokeHookAsync(PluginHook hook, HookEventData& data, const std::string& sourcePlugin = "");
    bool invokeHookSync(PluginHook hook, HookEventData& data, const std::string& sourcePlugin = "");
    
    // Hook information
    std::vector<std::string> getRegisteredPluginsForHook(PluginHook hook) const;
    std::vector<PluginHook> getRegisteredHooksForPlugin(const std::string& pluginName) const;
    int getHookPriority(const std::string& pluginName, PluginHook hook) const;
    bool setHookPriority(const std::string& pluginName, PluginHook hook, int priority);
    
    // Hook filtering
    bool hasHooksRegistered(PluginHook hook) const;
    bool hasHooksRegistered(const std::string& pluginName) const;
    bool hasAnyHooksRegistered() const;
    
    // Hook management
    void clearAllHooks();
    void clearHooksForPlugin(const std::string& pluginName);
    void clearHooksForHook(PluginHook hook);
    
    // Hook statistics
    struct HookInfo {
        PluginHook hook;
        std::string pluginName;
        std::function<bool(const HookEventData&)> callback;
        int priority;
        std::chrono::system_clock::time_point registeredTime;
        size_t invocationCount;
        size_t successfulInvocations;
        size_t failedInvocations;
        std::string lastError;
        
        HookInfo() : hook(PluginHook::SystemInitialize), priority(0),
            registeredTime(std::chrono::system_clock::now()),
            invocationCount(0), successfulInvocations(0), failedInvocations(0) {}
    };
    
    std::vector<HookInfo> getHookInfo(PluginHook hook) const;
    std::vector<HookInfo> getHookInfo(const std::string& pluginName) const;
    std::vector<HookInfo> getAllHookInfo() const;
    
    // Hook filtering and querying
    std::vector<HookInfo> getHooksByPriority(int priority) const;
    std::vector<HookInfo> getHooksByPlugin(const std::string& pluginName) const;
    std::vector<HookInfo> getHooksByHook(PluginHook hook) const;
    
    // Hook-specific operations
    bool addPlasticityRuleHook(const std::string& pluginName,
                              const std::function<bool(const HookEventData&)>& callback,
                              int priority = 0);
    bool removePlasticityRuleHook(const std::string& pluginName);
    bool addNeuromodulatorHook(const std::string& pluginName,
                             const std::function<bool(const HookEventData&)>& callback,
                             int priority = 0);
    bool removeNeuromodulatorHook(const std::string& pluginName);
    bool addDevelopmentHook(const std::string& pluginName,
                           const std::function<bool(const HookEventData&)>& callback,
                           int priority = 0);
    bool removeDevelopmentHook(const std::string& pluginName);
    bool addMemoryHook(const std::string& pluginName,
                      const std::function<bool(const HookEventData&)>& callback,
                      int priority = 0);
    bool removeMemoryHook(const std::string& pluginName);
    bool addPredictionHook(const std::string& pluginName,
                          const std::function<bool(const HookEventData&)>& callback,
                          int priority = 0);
    bool removePredictionHook(const std::string& pluginName);
    bool addCognitionHook(const std::string& pluginName,
                         const std::function<bool(const HookEventData&)>& callback,
                         int priority = 0);
    bool removeCognitionHook(const std::string& pluginName);
    bool addSensoryHook(const std::string& pluginName,
                      const std::function<bool(const HookEventData&)>& callback,
                      int priority = 0);
    bool removeSensoryHook(const std::string& pluginName);
    bool addMotorHook(const std::string& pluginName,
                     const std::function<bool(const HookEventData&)>& callback,
                     int priority = 0);
    bool removeMotorHook(const std::string& pluginName);
    bool addIntegrationHook(const std::string& pluginName,
                           const std::function<bool(const HookEventData&)>& callback,
                           int priority = 0);
    bool removeIntegrationHook(const std::string& pluginName);
    
    // Hook event preparation
    HookEventData prepareHookEvent(PluginHook hook, const std::string& pluginName,
                                  const std::unordered_map<std::string, std::any>& data = {});
    bool canInvokeHook(PluginHook hook, const HookEventData& data) const;
    bool shouldPreventDefault(const HookEventData& data) const;
    bool shouldStopPropagation(const HookEventData& data) const;
    
    // Hook result handling
    bool processHookResults(HookEventData& data);
    std::vector<std::any> getHookResults(const HookEventData& data) const;
    bool addHookResult(HookEventData& data, const std::any& result);
    void clearHookResults(HookEventData& data);
    
    // Hook event chaining
    bool triggerHookChain(PluginHook hook, HookEventData& data, const std::string& sourcePlugin = "");
    bool breakHookChain(const HookEventData& data);
    bool continueHookChain(const HookEventData& data);
    
    // Hook performance monitoring
    void recordHookInvocation(PluginHook hook, const std::string& pluginName, bool success, const std::string& error = "");
    double getHookInvocationTime(PluginHook hook, const std::string& pluginName) const;
    size_t getHookInvocationCount(PluginHook hook, const std::string& pluginName) const;
    size_t getSuccessfulHookInvocations(PluginHook hook, const std::string& pluginName) const;
    size_t getFailedHookInvocations(PluginHook hook, const std::string& pluginName) const;
    
    // Hook statistics
    struct HookPerformanceMetrics {
        double averageInvocationTime;
        double averageSuccessRate;
        size_t totalInvocations;
        size_t totalSuccessfulInvocations;
        size_t totalFailedInvocations;
        
        HookPerformanceMetrics() : averageInvocationTime(0.0), averageSuccessRate(0.0),
            totalInvocations(0), totalSuccessfulInvocations(0), totalFailedInvocations(0) {}
    };
    
    HookPerformanceMetrics getHookPerformanceMetrics(PluginHook hook) const;
    std::unordered_map<PluginHook, HookPerformanceMetrics> getAllHookPerformanceMetrics() const;
    
    // Hook-specific event data preparation
    static HookEventData createPlasticityRuleEvent(const std::string& pluginName,
                                                 const std::string& operation,
                                                 const std::unordered_map<std::string, std::any>& data);
    static HookEventData createNeuromodulatorEvent(const std::string& pluginName,
                                                  const std::string& operation,
                                                  const std::unordered_map<std::string, std::any>& data);
    static HookEventData createDevelopmentEvent(const std::string& pluginName,
                                               const std::string& operation,
                                               const std::unordered_map<std::string, std::any>& data);
    static HookEventData createMemoryEvent(const std::string& pluginName,
                                          const std::string& operation,
                                          const std::unordered_map<std::string, std::any>& data);
    static HookEventData createPredictionEvent(const std::string& pluginName,
                                              const std::string& operation,
                                              const std::unordered_map<std::string, std::any>& data);
    static HookEventData createCognitionEvent(const std::string& pluginName,
                                             const std::string& operation,
                                             const std::unordered_map<std::string, std::any>& data);
    static HookEventData createSensoryEvent(const std::string& pluginName,
                                          const std::string& operation,
                                          const std::unordered_map<std::string, std::any>& data);
    static HookEventData createMotorEvent(const std::string& pluginName,
                                         const std::string& operation,
                                         const std::unordered_map<std::string, std::any>& data);
    static HookEventData createIntegrationEvent(const std::string& pluginName,
                                               const std::string& operation,
                                               const std::unordered_map<std::string, std::any>& data);
    
    // Hook event validation
    static bool validateHookEvent(const HookEventData& data);
    static std::vector<std::string> getHookEventErrors(const HookEventData& data);
    static bool isHookEventValid(const HookEventData& data);
    
    // Hook system configuration
    void setMaxHookPriority(int priority);
    int getMaxHookPriority() const;
    void setHookTimeout(double timeout);
    double getHookTimeout() const;
    void setAsyncHookProcessing(bool enable);
    bool isAsyncHookProcessingEnabled() const;
    
    // Hook system lifecycle
    void pauseAllHooks();
    void resumeAllHooks();
    void shutdownAllHooks();
    bool isHookSystemActive() const;
    
    // Hook system integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    
    // Hook debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logHookRegistration(PluginHook hook, const std::string& pluginName, bool success) const;
    void logHookInvocation(PluginHook hook, const HookEventData& data, bool success, const std::string& error = "") const;
    void logHookError(const std::string& error) const;
    
    // Hook performance optimization
    void optimizeHookExecutionOrder();
    void rebalanceHookPriorities();
    void consolidateDuplicateHooks();
    
    // Hook configuration
    bool exportHookConfiguration(const std::string& filePath) const;
    bool importHookConfiguration(const std::string& filePath);
    std::string exportHookConfigurationAsJSON() const;
    bool importHookConfigurationFromJSON(const std::string& jsonData);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withHookLock(T func) {
        std::unique_lock<std::mutex> lock(hookMutex_);
        return func();
    }
    
    // Hook comparison for priority sorting
    static bool compareHooksByPriority(const HookInfo& a, const HookInfo& b) {
        return a.priority > b.priority; // Higher priority first
    }
}; // namespace nlm