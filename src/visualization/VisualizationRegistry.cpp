// ============================================================================
// Visualization Registry Implementation (Final)
// ============================================================================

#include "VisualizationRegistry.hpp"
#include <algorithm>
#include <chrono>
#include <numeric>

namespace nlm {

struct VisualizationRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<VisualizationBase>> visualizers;
    std::vector<std::string> visualizerOrder;
    std::unordered_map<std::string, bool> visualizerActive;
    uint64_t lastUpdateTime;
    uint32_t fpsCounter;
    double accumulatedTime;
    
    Impl() : lastUpdateTime(0), fpsCounter(0), accumulatedTime(0.0) {}
};

VisualizationRegistry::VisualizationRegistry() : pImpl(std::make_unique<Impl>()) {}

VisualizationRegistry::~VisualizationRegistry() = default;

bool VisualizationRegistry::registerVisualizer(std::unique_ptr<VisualizationBase> visualizer) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!visualizer) return false;
    
    const std::string& name = visualizer->getName();
    
    // Check for duplicate name
    if (pImpl->visualizers.find(name) != pImpl->visualizers.end()) {
        return false;
    }
    
    pImpl->visualizers[name] = std::move(visualizer);
    pImpl->visualizerOrder.push_back(name);
    pImpl->visualizerActive[name] = true;
    
    return true;
}

bool VisualizationRegistry::unregisterVisualizer(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->visualizers.find(name);
    if (it == pImpl->visualizers.end()) return false;
    
    pImpl->visualizers.erase(it);
    
    // Remove from order
    pImpl->visualizerOrder.erase(
        std::remove(pImpl->visualizerOrder.begin(), pImpl->visualizerOrder.end(), name),
        pImpl->visualizerOrder.end()
    );
    
    pImpl->visualizerActive.erase(name);
    
    return true;
}

VisualizationBase* VisualizationRegistry::getVisualizer(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pImpl->visualizers.find(name);
    if (it == pImpl->visualizers.end()) return nullptr;
    
    return it->second.get();
}

VisualizationBase* VisualizationRegistry::getVisualizer(size_t index) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (index >= pImpl->visualizerOrder.size()) return nullptr;
    
    const std::string& name = pImpl->visualizerOrder[index];
    auto it = pImpl->visualizers.find(name);
    
    return (it != pImpl->visualizers.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> VisualizationRegistry::getVisualizerNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->visualizerOrder;
}

size_t VisualizationRegistry::getVisualizerCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->visualizers.size();
}

void VisualizationRegistry::registerVisualizers(const std::vector<std::unique_ptr<VisualizationBase>>& visualizers) {
    for (const auto& viz : visualizers) {
        registerVisualizer(std::make_unique<VisualizationBase>(*viz));
    }
}

void VisualizationRegistry::unregisterAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->visualizers.clear();
    pImpl->visualizerOrder.clear();
    pImpl->visualizerActive.clear();
}

bool VisualizationRegistry::setGlobalConfiguration(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    globalConfig_ = config;
    return true;
}

void VisualizationRegistry::setDataStream(VisualizationDataStream* stream) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataStream_ = stream;
}

bool VisualizationRegistry::isHealthy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pImpl->visualizers.empty()) return false;
    
    for (const auto& pair : pImpl->visualizers) {
        if (pair.second && !pair.second->isActive()) {
            return false;
        }
    }
    
    return true;
}

std::string VisualizationRegistry::getHealthReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "Visualization Registry Health Report\n";
    oss << "====================================\n";
    oss << "Total visualizers: " << pImpl->visualizers.size() << "\n";
    oss << "Active visualizers: " << std::count_if(pImpl->visualizers.begin(), pImpl->visualizers.end(),
        [](const auto& pair) { return pair.second && pair.second->isActive(); }) << "\n";
    oss << "System FPS: " << systemFPS_ << "\n";
    oss << "Average frame time: " << averageFrameTime_ << "ms\n";
    
    if (!isHealthy()) {
        oss << "\nWARNING: Registry is not healthy!\n";
        for (const auto& pair : pImpl->visualizers) {
            if (pair.second && !pair.second->isActive()) {
                oss << "  - Inactive visualizer: " << pair.first << "\n";
            }
        }
    }
    
    return oss.str();
}

bool VisualizationRegistry::initializeAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    bool allInitialized = true;
    for (auto& pair : pImpl->visualizers) {
        if (pair.second) {
            if (!pair.second->initialize(globalConfig_)) {
                allInitialized = false;
            }
        }
    }
    
    return allInitialized;
}

void VisualizationRegistry::updateAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto currentTime = std::chrono::steady_clock::now();
    auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - std::chrono::steady_clock::time_point(std::chrono::microseconds(pImpl->lastUpdateTime)));
    
    pImpl->lastUpdateTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch()).count();
    
    if (timeSinceLastUpdate.count() > 0) {
        systemFPS_ = 1000000.0 / timeSinceLastUpdate.count();
    }
    
    double totalUpdateTime = 0.0;
    uint32_t updateCount = 0;
    
    for (auto& pair : pImpl->visualizers) {
        if (pair.second && pair.second->isActive()) {
            auto startTime = std::chrono::steady_clock::now();
            pair.second->update();
            auto endTime = std::chrono::steady_clock::now();
            
            auto updateDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            totalUpdateTime += updateDuration.count() / 1000000.0;
            updateCount++;
        }
    }
    
    if (updateCount > 0) {
        averageFrameTime_ = (totalUpdateTime * 1000.0) / updateCount;
    }
    
    pImpl->fpsCounter++;
}

void VisualizationRegistry::renderAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : pImpl->visualizers) {
        if (pair.second && pair.second->isActive()) {
            auto startTime = std::chrono::steady_clock::now();
            pair.second->render();
            auto endTime = std::chrono::steady_clock::now();
            
            auto renderDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            pair.second->getFrameTime();  // Access to track frame time
        }
    }
}

void VisualizationRegistry::closeAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : pImpl->visualizers) {
        if (pair.second) {
            pair.second->close();
        }
    }
}

void VisualizationRegistry::activateVisualizers(const std::vector<std::string>& names) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const std::string& name : names) {
        auto it = pImpl->visualizers.find(name);
        if (it != pImpl->visualizers.end() && it->second) {
            it->second->resume();
            pImpl->visualizerActive[name] = true;
        }
    }
}

void VisualizationRegistry::deactivateVisualizers(const std::vector<std::string>& names) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const std::string& name : names) {
        auto it = pImpl->visualizers.find(name);
        if (it != pImpl->visualizers.end() && it->second) {
            it->second->pause();
            pImpl->visualizerActive[name] = false;
        }
    }
}

std::vector<std::string> VisualizationRegistry::getActiveVisualizerNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> activeNames;
    for (const auto& pair : pImpl->visualizers) {
        if (pair.second && pImpl->visualizerActive.at(pair.first)) {
            activeNames.push_back(pair.first);
        }
    }
    
    return activeNames;
}

std::vector<VisualizationRegistry::VisualizerStats> VisualizationRegistry::getAllStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<VisualizerStats> stats;
    
    for (const auto& pair : pImpl->visualizers) {
        if (pair.second) {
            VisualizerStats stat;
            stat.name = pair.first;
            stat.isActive = pImpl->visualizerActive.at(pair.first);
            // Note: Individual visualizer stats would need to be tracked within each visualizer
            stat.updateCount = 0; // Placeholder
            stat.totalUpdateTime = 0.0;
            stat.averageUpdateTime = 0.0;
            stat.renderCount = 0;
            stat.totalRenderTime = 0.0;
            stat.averageRenderTime = 0.0;
            
            stats.push_back(stat);
        }
    }
    
    return stats;
}

} // namespace nlm
