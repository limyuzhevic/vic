// nlm/plugins/integration_hooks.cpp - Implementation of integration hook system
#include "integration_hooks.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace plugins {

struct IntegrationHookManager::Impl {
    std::vector<std::unique_ptr<IntegrationPlugin>> plugins;
    std::unordered_map<std::string, size_t> pluginNameToIndex;
    std::string pluginDirectory;
    
    // Global hooks (registered directly to manager)
    std::vector<BrainInitializedHook> globalBrainInitializedHooks;
    std::vector<WorldResetHook> globalWorldResetHooks;
    std::vector<SensoryInputHook> globalSensoryInputHooks;
    std::vector<MotorOutputHook> globalMotorOutputHooks;
    std::vector<RewardModulationHook> globalRewardModulationHooks;
    std::vector<DevelopmentHook> globalDevelopmentHooks;
    std::vector<PlasticityHook> globalPlasticityHooks;
    std::vector<MemoryHook> globalMemoryHooks;
    std::vector<CheckpointHook> globalCheckpointHooks;
    std::vector<PredictionHook> globalPredictionHooks;
    std::vector<ConceptHook> globalConceptHooks;
    std::vector<SocialHook> globalSocialHooks;
    std::vector<PerformanceHook> globalPerformanceHooks;
    
    std::unordered_map<std::string, bool> pluginEnabled;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> hookExecutionTimes;
    size_t totalHookExecutions = 0;
    double totalExecutionTime = 0.0;
};

IntegrationHookManager::IntegrationHookManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("IntegrationHookManager initialized");
}

IntegrationHookManager::~IntegrationHookManager() {
    shutdownAllPlugins();
    NLM_LOG_INFO("IntegrationHookManager shutdown");
}

bool IntegrationHookManager::registerPlugin(std::unique_ptr<IntegrationPlugin> plugin) {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot register null plugin");
        return false;
    }
    
    std::string name = plugin->getName();
    if (pluginNameToIndex.find(name) != pluginNameToIndex.end()) {
        NLM_LOG_WARNING("Plugin already registered: " + name);
        return false;
    }
    
    size_t index = pImpl->plugins.size();
    pImpl->plugins.push_back(std::move(plugin));
    pImpl->pluginNameToIndex[name] = index;
    pImpl->pluginEnabled[name] = true; // Default to enabled
    
    NLM_LOG_INFO("Plugin registered: " + name);
    return true;
}

bool IntegrationHookManager::unregisterPlugin(const std::string& name) {
    auto it = pluginNameToIndex.find(name);
    if (it == pluginNameToIndex.end()) {
        NLM_LOG_WARNING("Plugin not found for unregistration: " + name);
        return false;
    }
    
    size_t index = it->second;
    
    // Shutdown the plugin if it's enabled
    if (pImpl->pluginEnabled[name]) {
        pImpl->plugins[index]->shutdown();
    }
    
    // Remove from all maps and vectors
    pImpl->plugins.erase(pImpl->plugins.begin() + index);
    pImpl->pluginNameToIndex.clear();
    pImpl->pluginEnabled.clear();
    
    // Rebuild maps
    for (size_t i = 0; i < pImpl->plugins.size(); ++i) {
        pImpl->pluginNameToIndex[pImpl->plugins[i]->getName()] = i;
        pImpl->pluginEnabled[pImpl->plugins[i]->getName()] = pImpl->plugins[i]->isEnabled();
    }
    
    NLM_LOG_INFO("Plugin unregistered: " + name);
    return true;
}

bool IntegrationHookManager::enablePlugin(const std::string& name) {
    auto it = pluginNameToIndex.find(name);
    if (it == pluginNameToIndex.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    IntegrationPlugin* plugin = pImpl->plugins[it->second].get();
    if (!plugin->isEnabled()) {
        if (!plugin->initialize()) {
            NLM_LOG_ERROR("Failed to enable plugin: " + name);
            return false;
        }
        plugin->setEnabled(true);
        pImpl->pluginEnabled[name] = true;
    }
    
    NLM_LOG_INFO("Plugin enabled: " + name);
    return true;
}

bool IntegrationHookManager::disablePlugin(const std::string& name) {
    auto it = pluginNameToIndex.find(name);
    if (it == pluginNameToIndex.end()) {
        NLM_LOG_ERROR("Plugin not found: " + name);
        return false;
    }
    
    IntegrationPlugin* plugin = pImpl->plugins[it->second].get();
    if (plugin->isEnabled()) {
        if (!plugin->shutdown()) {
            NLM_LOG_ERROR("Failed to disable plugin: " + name);
            return false;
        }
        plugin->setEnabled(false);
        pImpl->pluginEnabled[name] = false;
    }
    
    NLM_LOG_INFO("Plugin disabled: " + name);
    return true;
}

bool IntegrationHookManager::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->pluginNameToIndex.find(name);
    if (it == pImpl->pluginNameToIndex.end()) {
        return false;
    }
    
    return pImpl->pluginEnabled[name];
}

std::vector<std::string> IntegrationHookManager::getEnabledPluginNames() const {
    std::vector<std::string> enabledNames;
    for (const auto& pair : pImpl->pluginNameToIndex) {
        if (pImpl->pluginEnabled[pair.first]) {
            enabledNames.push_back(pair.first);
        }
    }
    return enabledNames;
}

std::vector<std::string> IntegrationHookManager::getPluginNames() const {
    std::vector<std::string> names;
    for (const auto& plugin : pImpl->plugins) {
        names.push_back(plugin->getName());
    }
    return names;
}

IntegrationPlugin* IntegrationHookManager::getPlugin(const std::string& name) {
    auto it = pImpl->pluginNameToIndex.find(name);
    if (it == pImpl->pluginNameToIndex.end()) {
        return nullptr;
    }
    return pImpl->plugins[it->second].get();
}

void IntegrationHookManager::registerBrainInitializedHook(const BrainInitializedHook& hook, const std::string& source) {
    pImpl->globalBrainInitializedHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global brain initialized hook registered from: " + source);
    }
}

void IntegrationHookManager::registerWorldResetHook(const WorldResetHook& hook, const std::string& source) {
    pImpl->globalWorldResetHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global world reset hook registered from: " + source);
    }
}

void IntegrationHookManager::registerSensoryInputHook(const SensoryInputHook& hook, const std::string& source) {
    pImpl->globalSensoryInputHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global sensory input hook registered from: " + source);
    }
}

void IntegrationHookManager::registerMotorOutputHook(const MotorOutputHook& hook, const std::string& source) {
    pImpl->globalMotorOutputHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global motor output hook registered from: " + source);
    }
}

void IntegrationHookManager::registerRewardModulationHook(const RewardModulationHook& hook, const std::string& source) {
    pImpl->globalRewardModulationHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global reward modulation hook registered from: " + source);
    }
}

void IntegrationHookManager::registerDevelopmentHook(const DevelopmentHook& hook, const std::string& source) {
    pImpl->globalDevelopmentHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global development hook registered from: " + source);
    }
}

void IntegrationHookManager::registerPlasticityHook(const PlasticityHook& hook, const std::string& source) {
    pImpl->globalPlasticityHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global plasticity hook registered from: " + source);
    }
}

void IntegrationHookManager::registerMemoryHook(const MemoryHook& hook, const std::string& source) {
    pImpl->globalMemoryHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global memory hook registered from: " + source);
    }
}

void IntegrationHookManager::registerCheckpointHook(const CheckpointHook& hook, const std::string& source) {
    pImpl->globalCheckpointHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global checkpoint hook registered from: " + source);
    }
}

void IntegrationHookManager::registerPredictionHook(const PredictionHook& hook, const std::string& source) {
    pImpl->globalPredictionHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global prediction hook registered from: " + source);
    }
}

void IntegrationHookManager::registerConceptHook(const ConceptHook& hook, const std::string& source) {
    pImpl->globalConceptHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global concept hook registered from: " + source);
    }
}

void IntegrationHookManager::registerSocialHook(const SocialHook& hook, const std::string& source) {
    pImpl->globalSocialHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global social hook registered from: " + source);
    }
}

void IntegrationHookManager::registerPerformanceHook(const PerformanceHook& hook, const std::string& source) {
    pImpl->globalPerformanceHooks.push_back(hook);
    if (!source.empty()) {
        NLM_LOG_DEBUG("Global performance hook registered from: " + source);
    }
}

void IntegrationHookManager::executeAllBrainInitializedHooks(Brain* brain) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Execute plugin hooks
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeBrainInitializedHook(brain);
        }
    }
    
    // Execute global hooks
    for (const auto& hook : pImpl->globalBrainInitializedHooks) {
        hook(brain);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("brain_initialized", duration, 1);
}

void IntegrationHookManager::executeAllWorldResetHooks(World* world) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeWorldResetHook(world);
        }
    }
    
    for (const auto& hook : pImpl->globalWorldResetHooks) {
        hook(world);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("world_reset", duration, 1);
}

void IntegrationHookManager::executeAllSensoryInputHooks(Brain* brain, const SensoryPercept& percept) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeSensoryInputHook(brain, percept);
        }
    }
    
    for (const auto& hook : pImpl->globalSensoryInputHooks) {
        hook(brain, percept);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("sensory_input", duration, 1);
}

void IntegrationHookManager::executeAllMotorOutputHooks(Brain* brain, Action* action, ActionResult& result) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeMotorOutputHook(brain, action, result);
        }
    }
    
    for (const auto& hook : pImpl->globalMotorOutputHooks) {
        hook(brain, action, result);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("motor_output", duration, 1);
}

void IntegrationHookManager::executeAllRewardModulationHooks(Brain* brain, float reward, float curiosity) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeRewardModulationHook(brain, reward, curiosity);
        }
    }
    
    for (const auto& hook : pImpl->globalRewardModulationHooks) {
        hook(brain, reward, curiosity);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("reward_modulation", duration, 1);
}

void IntegrationHookManager::executeAllDevelopmentHooks(Brain* brain, float deltaTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeDevelopmentHook(brain, deltaTime);
        }
    }
    
    for (const auto& hook : pImpl->globalDevelopmentHooks) {
        hook(brain, deltaTime);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("development", duration, 1);
}

void IntegrationHookManager::executeAllPlasticityHooks(Brain* brain, float deltaTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executePlasticityHook(brain, deltaTime);
        }
    }
    
    for (const auto& hook : pImpl->globalPlasticityHooks) {
        hook(brain, deltaTime);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("plasticity", duration, 1);
}

void IntegrationHookManager::executeAllMemoryHooks(Brain* brain, const std::string& type, const std::string& action) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeMemoryHook(brain, type, action);
        }
    }
    
    for (const auto& hook : pImpl->globalMemoryHooks) {
        hook(brain, type, action);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("memory", duration, 1);
}

bool IntegrationHookManager::executeAllCheckpointHooks(Brain* brain, const std::string& filepath, bool saving) {
    auto startTime = std::chrono::steady_clock::now();
    
    bool allSuccessful = true;
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            bool success = plugin->executeCheckpointHook(brain, filepath, saving);
            if (!success) {
                allSuccessful = false;
            }
        }
    }
    
    for (const auto& hook : pImpl->globalCheckpointHooks) {
        bool success = hook(brain, filepath, saving);
        if (!success) {
            allSuccessful = false;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("checkpoint", duration, allSuccessful ? 1 : 0);
    
    return allSuccessful;
}

void IntegrationHookManager::executeAllPredictionHooks(Brain* brain, float prediction, float error) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executePredictionHook(brain, prediction, error);
        }
    }
    
    for (const auto& hook : pImpl->globalPredictionHooks) {
        hook(brain, prediction, error);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("prediction", duration, 1);
}

void IntegrationHookManager::executeAllConceptHooks(Brain* brain, size_t conceptId, const std::string& conceptType) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeConceptHook(brain, conceptId, conceptType);
        }
    }
    
    for (const auto& hook : pImpl->globalConceptHooks) {
        hook(brain, conceptId, conceptType);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("concept", duration, 1);
}

void IntegrationHookManager::executeAllSocialHooks(Brain* brain, const std::string& otherAgentId, const Action& action) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executeSocialHook(brain, otherAgentId, action);
        }
    }
    
    for (const auto& hook : pImpl->globalSocialHooks) {
        hook(brain, otherAgentId, action);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("social", duration, 1);
}

void IntegrationHookManager::executeAllPerformanceHooks(Brain* brain, const std::string& operation, float duration, float memoryUsage) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled()) {
            plugin->executePerformanceHook(brain, operation, duration, memoryUsage);
        }
    }
    
    for (const auto& hook : pImpl->globalPerformanceHooks) {
        hook(brain, operation, duration, memoryUsage);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto execDuration = std::chrono::duration<double>(endTime - startTime).count();
    
    updateHookStatistics("performance", execDuration, 1);
}

bool IntegrationHookManager::initializeAllPlugins() {
    bool allInitialized = true;
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled() && !plugin->initialize()) {
            allInitialized = false;
            NLM_LOG_ERROR("Plugin initialization failed: " + plugin->getName());
        }
    }
    
    if (allInitialized) {
        NLM_LOG_INFO("All plugins initialized successfully");
    } else {
        NLM_LOG_ERROR("Some plugins failed to initialize");
    }
    
    return allInitialized;
}

bool IntegrationHookManager::shutdownAllPlugins() {
    bool allShutdown = true;
    
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isEnabled() && !plugin->shutdown()) {
            allShutdown = false;
            NLM_LOG_ERROR("Plugin shutdown failed: " + plugin->getName());
        }
    }
    
    if (allShutdown) {
        NLM_LOG_INFO("All plugins shutdown successfully");
    } else {
        NLM_LOG_ERROR("Some plugins failed to shutdown");
    }
    
    return allShutdown;
}

void IntegrationHookManager::setPluginDirectory(const std::string& directory) {
    pImpl->pluginDirectory = directory;
    NLM_LOG_INFO("Plugin directory set to: " + directory);
}

std::string IntegrationHookManager::getPluginDirectory() const {
    return pImpl->pluginDirectory;
}

void IntegrationHookManager::loadPluginsFromDirectory() {
    if (pImpl->pluginDirectory.empty()) {
        NLM_LOG_WARNING("No plugin directory set, skipping plugin loading");
        return;
    }
    
    if (!std::filesystem::exists(pImpl->pluginDirectory)) {
        NLM_LOG_WARNING("Plugin directory does not exist: " + pImpl->pluginDirectory);
        return;
    }
    
    NLM_LOG_INFO("Loading plugins from directory: " + pImpl->pluginDirectory);
    
    for (const auto& entry : std::filesystem::directory_iterator(pImpl->pluginDirectory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".so" ||
                filename.size() > 3 && filename.substr(filename.size() - 3) == ".dll" ||
                filename.size() > 4 && filename.substr(filename.size() - 5) == ".dylib") {
                
                std::string pluginName = filename.substr(0, filename.find_last_of('.'));
                NLM_LOG_INFO("Found plugin library: " + pluginName);
                
                // TODO: Dynamically load plugin library
                // For now, we'll just log that we found it
                NLM_LOG_INFO("Dynamic plugin loading not yet implemented for: " + pluginName);
            }
        }
    }
}

void IntegrationHookManager::updateHookStatistics(const std::string& hookType, double duration, int success) {
    pImpl->totalHookExecutions++;
    pImpl->totalExecutionTime += duration;
    
    if (duration > 0.1) {
        NLM_LOG_WARNING("Slow hook execution: " + hookType + " took " + std::to_string(duration) + " seconds");
    }
}

size_t IntegrationHookManager::getTotalHookCount() const {
    return pImpl->totalHookExecutions;
}

size_t IntegrationHookManager::getEnabledHookCount() const {
    return getEnabledPluginNames().size();
}

float IntegrationHookManager::getAverageExecutionTime() const {
    if (pImpl->totalHookExecutions == 0) return 0.0f;
    return static_cast<float>(pImpl->totalExecutionTime / pImpl->totalHookExecutions);
}

void IntegrationHookManager::printPluginStatus() const {
    NLM_LOG_INFO("=== Integration Hook Manager Status ===");
    NLM_LOG_INFO("Total plugins: " + std::to_string(pImpl->plugins.size()));
    NLM_LOG_INFO("Enabled plugins: " + std::to_string(getEnabledPluginNames().size()));
    NLM_LOG_INFO("Total hook executions: " + std::to_string(pImpl->totalHookExecutions));
    NLM_LOG_INFO("Average execution time: " + std::to_string(getAverageExecutionTime()) + " seconds");
    NLM_LOG_INFO("Plugin directory: " + (pImpl->pluginDirectory.empty() ? "not set" : pImpl->pluginDirectory));
    
    NLM_LOG_INFO("\nEnabled plugins:");
    for (const auto& name : getEnabledPluginNames()) {
        NLM_LOG_INFO("  - " + name);
    }
    
    NLM_LOG_INFO("=============================================");
}

// SimplePlugin implementation
SimplePlugin::SimplePlugin(const std::string& name, const std::string& version, const std::string& description)
    : name_(name), version_(version), description_(description) {
}

void SimplePlugin::registerBrainInitializedHook(const BrainInitializedHook& hook) {
    brainInitializedHooks_.push_back(hook);
}

void SimplePlugin::registerWorldResetHook(const WorldResetHook& hook) {
    worldResetHooks_.push_back(hook);
}

void SimplePlugin::registerSensoryInputHook(const SensoryInputHook& hook) {
    sensoryInputHooks_.push_back(hook);
}

void SimplePlugin::registerMotorOutputHook(const MotorOutputHook& hook) {
    motorOutputHooks_.push_back(hook);
}

void SimplePlugin::registerRewardModulationHook(const RewardModulationHook& hook) {
    rewardModulationHooks_.push_back(hook);
}

void SimplePlugin::registerDevelopmentHook(const DevelopmentHook& hook) {
    developmentHooks_.push_back(hook);
}

void SimplePlugin::registerPlasticityHook(const PlasticityHook& hook) {
    plasticityHooks_.push_back(hook);
}

void SimplePlugin::registerMemoryHook(const MemoryHook& hook) {
    memoryHooks_.push_back(hook);
}

void SimplePlugin::registerCheckpointHook(const CheckpointHook& hook) {
    checkpointHooks_.push_back(hook);
}

void SimplePlugin::registerPredictionHook(const PredictionHook& hook) {
    predictionHooks_.push_back(hook);
}

void SimplePlugin::registerConceptHook(const ConceptHook& hook) {
    conceptHooks_.push_back(hook);
}

void SimplePlugin::registerSocialHook(const SocialHook& hook) {
    socialHooks_.push_back(hook);
}

void SimplePlugin::registerPerformanceHook(const PerformanceHook& hook) {
    performanceHooks_.push_back(hook);
}

void SimplePlugin::executeBrainInitializedHook(Brain* brain) {
    for (const auto& hook : brainInitializedHooks_) {
        try {
            hook(brain);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in brain initialized hook: " + std::string(e.what()));
        }
    }
    onBrainInitialized(brain);
}

void SimplePlugin::executeWorldResetHook(World* world) {
    for (const auto& hook : worldResetHooks_) {
        try {
            hook(world);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in world reset hook: " + std::string(e.what()));
        }
    }
    onWorldReset(world);
}

void SimplePlugin::executeSensoryInputHook(Brain* brain, const SensoryPercept& percept) {
    for (const auto& hook : sensoryInputHooks_) {
        try {
            hook(brain, percept);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in sensory input hook: " + std::string(e.what()));
        }
    }
    onSensoryInput(brain, percept);
}

void SimplePlugin::executeMotorOutputHook(Brain* brain, Action* action, ActionResult& result) {
    for (const auto& hook : motorOutputHooks_) {
        try {
            hook(brain, action, result);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in motor output hook: " + std::string(e.what()));
        }
    }
    onMotorOutput(brain, action, result);
}

void SimplePlugin::executeRewardModulationHook(Brain* brain, float reward, float curiosity) {
    for (const auto& hook : rewardModulationHooks_) {
        try {
            hook(brain, reward, curiosity);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in reward modulation hook: " + std::string(e.what()));
        }
    }
    onRewardModulation(brain, reward, curiosity);
}

void SimplePlugin::executeDevelopmentHook(Brain* brain, float deltaTime) {
    for (const auto& hook : developmentHooks_) {
        try {
            hook(brain, deltaTime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in development hook: " + std::string(e.what()));
        }
    }
    onDevelopment(brain, deltaTime);
}

void SimplePlugin::executePlasticityHook(Brain* brain, float deltaTime) {
    for (const auto& hook : plasticityHooks_) {
        try {
            hook(brain, deltaTime);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in plasticity hook: " + std::string(e.what()));
        }
    }
    onPlasticity(brain, deltaTime);
}

void SimplePlugin::executeMemoryHook(Brain* brain, const std::string& type, const std::string& action) {
    for (const auto& hook : memoryHooks_) {
        try {
            hook(brain, type, action);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in memory hook: " + std::string(e.what()));
        }
    }
    onMemory(brain, type, action);
}

bool SimplePlugin::executeCheckpointHook(Brain* brain, const std::string& filepath, bool saving) {
    for (const auto& hook : checkpointHooks_) {
        try {
            if (!hook(brain, filepath, saving)) {
                return false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in checkpoint hook: " + std::string(e.what()));
            return false;
        }
    }
    return onCheckpoint(brain, filepath, saving);
}

void SimplePlugin::executePredictionHook(Brain* brain, float prediction, float error) {
    for (const auto& hook : predictionHooks_) {
        try {
            hook(brain, prediction, error);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in prediction hook: " + std::string(e.what()));
        }
    }
    onPrediction(brain, prediction, error);
}

void SimplePlugin::executeConceptHook(Brain* brain, size_t conceptId, const std::string& conceptType) {
    for (const auto& hook : conceptHooks_) {
        try {
            hook(brain, conceptId, conceptType);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in concept hook: " + std::string(e.what()));
        }
    }
    onConcept(brain, conceptId, conceptType);
}

void SimplePlugin::executeSocialHook(Brain* brain, const std::string& otherAgentId, const Action& action) {
    for (const auto& hook : socialHooks_) {
        try {
            hook(brain, otherAgentId, action);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in social hook: " + std::string(e.what()));
        }
    }
    onSocial(brain, otherAgentId, action);
}

void SimplePlugin::executePerformanceHook(Brain* brain, const std::string& operation, float duration, float memoryUsage) {
    for (const auto& hook : performanceHooks_) {
        try {
            hook(brain, operation, duration, memoryUsage);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in performance hook: " + std::string(e.what()));
        }
    }
    onPerformance(brain, operation, duration, memoryUsage);
}

// Plugin utils implementation
IntegrationHookManager& plugin_utils::getHookManager() {
    static IntegrationHookManager manager;
    return manager;
}

bool plugin_utils::loadPlugin(const std::string& pluginPath) {
    IntegrationHookManager& manager = getHookManager();
    
    // TODO: Implement dynamic plugin loading
    NLM_LOG_INFO("Dynamic plugin loading from " + pluginPath + " not yet implemented");
    
    return false;
}

bool plugin_utils::unloadPlugin(const std::string& name) {
    IntegrationHookManager& manager = getHookManager();
    return manager.unregisterPlugin(name);
}

bool plugin_utils::loadPluginsFromDirectory(const std::string& directory) {
    IntegrationHookManager& manager = getHookManager();
    manager.setPluginDirectory(directory);
    manager.loadPluginsFromDirectory();
    return true;
}

template<typename HookType>
void plugin_utils::registerHook(const HookType& hook, const std::string& hookName, const std::string& pluginName) {
    IntegrationHookManager& manager = getHookManager();
    
    if (hookName == "brain_initialized") {
        manager.registerBrainInitializedHook(hook, pluginName);
    } else if (hookName == "world_reset") {
        manager.registerWorldResetHook(hook, pluginName);
    } else if (hookName == "sensory_input") {
        manager.registerSensoryInputHook(hook, pluginName);
    } else if (hookName == "motor_output") {
        manager.registerMotorOutputHook(hook, pluginName);
    } else if (hookName == "reward_modulation") {
        manager.registerRewardModulationHook(hook, pluginName);
    } else if (hookName == "development") {
        manager.registerDevelopmentHook(hook, pluginName);
    } else if (hookName == "plasticity") {
        manager.registerPlasticityHook(hook, pluginName);
    } else if (hookName == "memory") {
        manager.registerMemoryHook(hook, pluginName);
    } else if (hookName == "checkpoint") {
        manager.registerCheckpointHook(hook, pluginName);
    } else if (hookName == "prediction") {
        manager.registerPredictionHook(hook, pluginName);
    } else if (hookName == "concept") {
        manager.registerConceptHook(hook, pluginName);
    } else if (hookName == "social") {
        manager.registerSocialHook(hook, pluginName);
    } else if (hookName == "performance") {
        manager.registerPerformanceHook(hook, pluginName);
    }
}

std::vector<std::string> plugin_utils::getAvailablePlugins() {
    IntegrationHookManager& manager = getHookManager();
    return manager.getPluginNames();
}

bool plugin_utils::isPluginLoaded(const std::string& name) {
    IntegrationHookManager& manager = getHookManager();
    return manager.getPlugin(name) != nullptr;
}

bool plugin_utils::isPluginEnabled(const std::string& name) {
    IntegrationHookManager& manager = getHookManager();
    return manager.isPluginEnabled(name);
}

void plugin_utils::executeBrainInitializedHook(Brain* brain) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllBrainInitializedHooks(brain);
}

void plugin_utils::executeWorldResetHook(World* world) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllWorldResetHooks(world);
}

void plugin_utils::executeSensoryInputHook(Brain* brain, const SensoryPercept& percept) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllSensoryInputHooks(brain, percept);
}

void plugin_utils::executeMotorOutputHook(Brain* brain, Action* action, ActionResult& result) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllMotorOutputHooks(brain, action, result);
}

void plugin_utils::executeRewardModulationHook(Brain* brain, float reward, float curiosity) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllRewardModulationHooks(brain, reward, curiosity);
}

void plugin_utils::executeDevelopmentHook(Brain* brain, float deltaTime) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllDevelopmentHooks(brain, deltaTime);
}

void plugin_utils::executePlasticityHook(Brain* brain, float deltaTime) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllPlasticityHooks(brain, deltaTime);
}

void plugin_utils::executeMemoryHook(Brain* brain, const std::string& type, const std::string& action) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllMemoryHooks(brain, type, action);
}

bool plugin_utils::executeCheckpointHook(Brain* brain, const std::string& filepath, bool saving) {
    IntegrationHookManager& manager = getHookManager();
    return manager.executeAllCheckpointHooks(brain, filepath, saving);
}

void plugin_utils::executePredictionHook(Brain* brain, float prediction, float error) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllPredictionHooks(brain, prediction, error);
}

void plugin_utils::executeConceptHook(Brain* brain, size_t conceptId, const std::string& conceptType) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllConceptHooks(brain, conceptId, conceptType);
}

void plugin_utils::executeSocialHook(Brain* brain, const std::string& otherAgentId, const Action& action) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllSocialHooks(brain, otherAgentId, action);
}

void plugin_utils::executePerformanceHook(Brain* brain, const std::string& operation, float duration, float memoryUsage) {
    IntegrationHookManager& manager = getHookManager();
    manager.executeAllPerformanceHooks(brain, operation, duration, memoryUsage);
}

} // namespace plugins
} // namespace nlm
