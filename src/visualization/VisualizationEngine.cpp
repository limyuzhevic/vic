// Implementation of the main visualization engine
#include "VisualizationEngine.hpp"
#include "NetworkVisualizer.hpp"
#include "ActivityAnalyzer.hpp"
#include "BrainStateDebugger.hpp"
#include "PerformanceProfiler.hpp"
#include "MemoryAnalyzer.hpp"
#include "DevelopmentInterface.hpp"
#include "ExportManager.hpp"
#include "PluginManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

VisualizationEngine::VisualizationEngine() 
    : updateRate_(30.0), active_(false), brain_(nullptr) {
}

VisualizationEngine::~VisualizationEngine() {
    shutdown();
}

bool VisualizationEngine::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize VisualizationEngine: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    
    // Initialize components
    networkVisualizer_ = std::make_unique<NetworkVisualizer>();
    activityAnalyzer_ = std::make_unique<ActivityAnalyzer>();
    brainStateDebugger_ = std::make_unique<BrainStateDebugger>();
    performanceProfiler_ = std::make_unique<PerformanceProfiler>();
    memoryAnalyzer_ = std::make_unique<MemoryAnalyzer>();
    developmentInterface_ = std::make_unique<DevelopmentInterface>();
    exportManager_ = std::make_unique<ExportManager>();
    pluginManager_ = std::make_unique<PluginManager>();
    
    // Initialize each component
    if (!networkVisualizer_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize NetworkVisualizer");
        return false;
    }
    
    if (!activityAnalyzer_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize ActivityAnalyzer");
        return false;
    }
    
    if (!brainStateDebugger_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize BrainStateDebugger");
        return false;
    }
    
    if (!performanceProfiler_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize PerformanceProfiler");
        return false;
    }
    
    if (!memoryAnalyzer_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize MemoryAnalyzer");
        return false;
    }
    
    if (!developmentInterface_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize DevelopmentInterface");
        return false;
    }
    
    if (!exportManager_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize ExportManager");
        return false;
    }
    
    if (!pluginManager_->initialize(brain)) {
        NLM_LOG_ERROR("Failed to initialize PluginManager");
        return false;
    }
    
    active_ = true;
    startBackgroundThreads();
    
    NLM_LOG_INFO("VisualizationEngine initialized successfully");
    return true;
}

void VisualizationEngine::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_) return;
    
    stopBackgroundThreads();
    
    // Shutdown components
    if (pluginManager_) pluginManager_->shutdown();
    if (exportManager_) exportManager_->shutdown();
    if (developmentInterface_) developmentInterface_->shutdown();
    if (memoryAnalyzer_) memoryAnalyzer_->shutdown();
    if (performanceProfiler_) performanceProfiler_->shutdown();
    if (brainStateDebugger_) brainStateDebugger_->shutdown();
    if (activityAnalyzer_) activityAnalyzer_->shutdown();
    if (networkVisualizer_) networkVisualizer_->shutdown();
    
    // Clear components
    pluginManager_.reset();
    exportManager_.reset();
    developmentInterface_.reset();
    memoryAnalyzer_.reset();
    performanceProfiler_.reset();
    brainStateDebugger_.reset();
    activityAnalyzer_.reset();
    networkVisualizer_.reset();
    
    brain_.reset();
    active_ = false;
    
    NLM_LOG_INFO("VisualizationEngine shutdown complete");
}

void VisualizationEngine::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_ || !brain_) return;
    
    currentData_ = data;
    
    // Update all components
    if (networkVisualizer_) {
        networkVisualizer_->update(data);
    }
    
    if (activityAnalyzer_) {
        activityAnalyzer_->update(data);
    }
    
    if (brainStateDebugger_) {
        brainStateDebugger_->update(data);
    }
    
    if (performanceProfiler_) {
        performanceProfiler_->update(data);
    }
    
    if (memoryAnalyzer_) {
        memoryAnalyzer_->update(data);
    }
    
    if (developmentInterface_) {
        developmentInterface_->update(data);
    }
    
    if (exportManager_) {
        exportManager_->update(data);
    }
    
    if (pluginManager_) {
        pluginManager_->update(data);
    }
}

void VisualizationEngine::render() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_) return;
    
    // Render using network visualizer as primary renderer
    if (networkVisualizer_) {
        networkVisualizer_->render();
    }
    
    // Plugins can also contribute to rendering
    if (pluginManager_) {
        pluginManager_->render();
    }
}

void VisualizationEngine::startVisualization() {
    std::lock_guard<std::mutex> lock(mutex_);
    active_ = true;
}

void VisualizationEngine::stopVisualization() {
    std::lock_guard<std::mutex> lock(mutex_);
    active_ = false;
}

NetworkVisualizer& VisualizationEngine::getNetworkVisualizer() {
    if (!networkVisualizer_) {
        throw std::runtime_error("NetworkVisualizer not initialized");
    }
    return *networkVisualizer_;
}

ActivityAnalyzer& VisualizationEngine::getActivityAnalyzer() {
    if (!activityAnalyzer_) {
        throw std::runtime_error("ActivityAnalyzer not initialized");
    }
    return *activityAnalyzer_;
}

BrainStateDebugger& VisualizationEngine::getBrainStateDebugger() {
    if (!brainStateDebugger_) {
        throw std::runtime_error("BrainStateDebugger not initialized");
    }
    return *brainStateDebugger_;
}

PerformanceProfiler& VisualizationEngine::getPerformanceProfiler() {
    if (!performanceProfiler_) {
        throw std::runtime_error("PerformanceProfiler not initialized");
    }
    return *performanceProfiler_;
}

MemoryAnalyzer& VisualizationEngine::getMemoryAnalyzer() {
    if (!memoryAnalyzer_) {
        throw std::runtime_error("MemoryAnalyzer not initialized");
    }
    return *memoryAnalyzer_;
}

DevelopmentInterface& VisualizationEngine::getDevelopmentInterface() {
    if (!developmentInterface_) {
        throw std::runtime_error("DevelopmentInterface not initialized");
    }
    return *developmentInterface_;
}

ExportManager& VisualizationEngine::getExportManager() {
    if (!exportManager_) {
        throw std::runtime_error("ExportManager not initialized");
    }
    return *exportManager_;
}

PluginManager& VisualizationEngine::getPluginManager() {
    if (!pluginManager_) {
        throw std::runtime_error("PluginManager not initialized");
    }
    return *pluginManager_;
}

void VisualizationEngine::setUpdateRate(double hz) {
    std::lock_guard<std::mutex> lock(mutex_);
    updateRate_ = hz;
}

double VisualizationEngine::getUpdateRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return updateRate_;
}

bool VisualizationEngine::isActive() const {
    return active_;
}

void VisualizationEngine::setBrain(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
}

std::shared_ptr<Brain> VisualizationEngine::getBrain() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return brain_;
}

void VisualizationEngine::startBackgroundThreads() {
    // Start update thread
    updateThread_ = std::thread([this]() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        auto nextTime = lastTime;
        
        while (active_) {
            auto now = std::chrono::high_resolution_clock::now();
            nextTime += std::chrono::duration<double, std::milli>(1000.0 / updateRate_);
            
            std::this_thread::sleep_until(nextTime);
            
            if (active_) {
                update(currentData_);
            }
        }
    });
    
    // Start render thread
    renderThread_ = std::thread([this]() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        auto nextTime = lastTime;
        
        while (active_) {
            auto now = std::chrono::high_resolution_clock::now();
            nextTime += std::chrono::duration<double, std::milli>(1000.0 / updateRate_);
            
            std::this_thread::sleep_until(nextTime);
            
            if (active_) {
                render();
            }
        }
    });
}

void VisualizationEngine::stopBackgroundThreads() {
    active_ = false;
    
    if (updateThread_.joinable()) {
        updateThread_.join();
    }
    
    if (renderThread_.joinable()) {
        renderThread_.join();
    }
}

bool VisualizationEngine::isBackgroundThreadsActive() const {
    return active_;
}

} // namespace nlm
