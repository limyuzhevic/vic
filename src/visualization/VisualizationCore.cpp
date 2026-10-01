#include "VisualizationCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <unordered_map>

namespace nlm {

struct VisualizationCore::Impl {
    bool active;
    double updateRate;
    double renderTime;
    double updateTime;
    size_t drawCallCount;
    std::unordered_map<std::string, std::unique_ptr<VisualizationPlugin>> plugins;
    std::unordered_map<std::string, bool> visualizationEnabled;
    std::unordered_map<std::string, std::shared_ptr<VisualizationData>> visualizationData;
    std::unordered_map<std::string, std::string> config;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    Impl() : active(false), updateRate(60.0), renderTime(0.0), updateTime(0.0), 
             drawCallCount(0), lastUpdateTime(std::chrono::steady_clock::now()) {}
};

VisualizationCore::VisualizationCore() : pImpl(std::make_unique<Impl>()) {}

VisualizationCore::~VisualizationCore() = default;

bool VisualizationCore::initialize() {
    // Initialize visualization system
    // This would typically initialize OpenGL, Vulkan, or other graphics API
    pImpl->active = true;
    NLM_LOG_INFO("VisualizationCore: Initialized successfully");
    return true;
}

void VisualizationCore::update(const Brain& brain, double deltaTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Update all registered plugins
    for (auto& pair : pImpl->plugins) {
        if (pImpl->visualizationEnabled[pair.first]) {
            pair.second->update(brain, deltaTime);
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->updateTime = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->lastUpdateTime = endTime;
}

void VisualizationCore::render() {
    auto startTime = std::chrono::steady_clock::now();
    
    // Render all enabled visualizations
    pImpl->drawCallCount = 0;
    for (const auto& pair : pImpl->visualizationEnabled) {
        if (pair.second) {
            // Render logic would go here
            pImpl->drawCallCount++;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->renderTime = std::chrono::duration<double>(endTime - startTime).count();
    NLM_LOG_DEBUG("VisualizationCore: Rendered " + std::to_string(pImpl->drawCallCount) + 
                  " draw calls in " + std::to_string(pImpl->renderTime) + " seconds");
}

void VisualizationCore::close() {
    pImpl->active = false;
    pImpl->plugins.clear();
    NLM_LOG_INFO("VisualizationCore: Closed successfully");
}

bool VisualizationCore::isActive() const {
    return pImpl->active;
}

void VisualizationCore::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationCore::getUpdateRate() const {
    return pImpl->updateRate;
}

bool VisualizationCore::registerPlugin(std::unique_ptr<VisualizationPlugin> plugin) {
    if (!plugin) return false;
    
    const std::string& name = plugin->getName();
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        NLM_LOG_WARNING("VisualizationCore: Plugin with name '" + name + "' already registered");
        return false;
    }
    
    pImpl->plugins[name] = std::move(plugin);
    pImpl->visualizationEnabled[name] = true;  // Enable by default
    NLM_LOG_INFO("VisualizationCore: Registered plugin '" + name + "'");
    return true;
}

bool VisualizationCore::unregisterPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("VisualizationCore: Plugin with name '" + name + "' not found");
        return false;
    }
    
    pImpl->plugins.erase(it);
    pImpl->visualizationEnabled.erase(name);
    NLM_LOG_INFO("VisualizationCore: Unregistered plugin '" + name + "'");
    return true;
}

VisualizationPlugin* VisualizationCore::getPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    return (it != pImpl->plugins.end()) ? it->second.get() : nullptr;
}

void VisualizationCore::enableVisualization(const std::string& name) {
    if (pImpl->visualizationEnabled.find(name) != pImpl->visualizationEnabled.end()) {
        pImpl->visualizationEnabled[name] = true;
        NLM_LOG_INFO("VisualizationCore: Enabled visualization '" + name + "'");
    }
}

void VisualizationCore::disableVisualization(const std::string& name) {
    if (pImpl->visualizationEnabled.find(name) != pImpl->visualizationEnabled.end()) {
        pImpl->visualizationEnabled[name] = false;
        NLM_LOG_INFO("VisualizationCore: Disabled visualization '" + name + "'");
    }
}

bool VisualizationCore::isVisualizationEnabled(const std::string& name) const {
    auto it = pImpl->visualizationEnabled.find(name);
    return (it != pImpl->visualizationEnabled.end()) && it->second;
}

std::shared_ptr<VisualizationData> VisualizationCore::getVisualizationData(const std::string& name) const {
    auto it = pImpl->visualizationData.find(name);
    return (it != pImpl->visualizationData.end()) ? it->second : nullptr;
}

bool VisualizationCore::exportData(const std::string& format, const std::string& filepath,
                                   const std::vector<std::string>& visualizationNames) const {
    NLM_LOG_INFO("VisualizationCore: Exporting data to " + filepath + " in format " + format);
    // Export implementation would go here
    return true;
}

double VisualizationCore::getRenderTime() const {
    return pImpl->renderTime;
}

double VisualizationCore::getUpdateTime() const {
    return pImpl->updateTime;
}

size_t VisualizationCore::getDrawCallCount() const {
    return pImpl->drawCallCount;
}

void VisualizationCore::setConfig(const std::string& key, const std::string& value) {
    pImpl->config[key] = value;
}

std::string VisualizationCore::getConfig(const std::string& key) const {
    auto it = pImpl->config.find(key);
    return (it != pImpl->config.end()) ? it->second : "";
}

} // namespace nlm