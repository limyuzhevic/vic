#include "AdvancedVisualization.hpp"
#include "DebugConsole.hpp"
#include "NetworkVisualizer.hpp"
#include "ActivityVisualizer.hpp"
#include "MemoryVisualizer.hpp"
#include "NeuromodulationVisualizer.hpp"
#include "DevelopmentVisualizer.hpp"
#include "Profiler.hpp"
#include "ExportUtils.hpp"
#include "../core/Logger.hpp"
#include "../brain/SpikeSystem.hpp"
#include <thread>
#include <chrono>
#include <fstream>

namespace nlm {

struct AdvancedVisualization::Impl {
    bool running;
    OutputFormat outputFormat;
    std::string outputPath;
    double updateRate;
    double lastFrameTime;
    uint64_t totalFramesRendered;
    double fps;
    
    // Component managers
    std::unique_ptr<DebugConsole> debugConsole;
    std::unique_ptr<NetworkVisualizer> networkVisualizer;
    std::unique_ptr<ActivityVisualizer> activityVisualizer;
    std::unique_ptr<MemoryVisualizer> memoryVisualizer;
    std::unique_ptr<NeuromodulationVisualizer> neuromodulationVisualizer;
    std::unique_ptr<DevelopmentVisualizer> developmentVisualizer;
    std::unique_ptr<Profiler> profiler;
    std::unique_ptr<ExportUtils> exportUtils;
    
    // Plugin system
    std::vector<std::unique_ptr<PluginInterface>> plugins;
    
    // Component state
    std::map<std::string, bool> componentStates;
    
    // Synchronization
    mutable std::mutex renderMutex;
    bool initialized;
    
    Impl() : running(false), outputFormat(OutputFormat::Text), 
             updateRate(30.0), lastFrameTime(0.0), totalFramesRendered(0), 
             fps(0.0), initialized(false) {
        componentStates["network"] = true;
        componentStates["activity"] = true;
        componentStates["memory"] = true;
        componentStates["neuromodulation"] = true;
        componentStates["development"] = true;
        componentStates["profiler"] = true;
        componentStates["export"] = true;
    }
};

AdvancedVisualization::AdvancedVisualization() : pImpl(std::make_unique<Impl>()) {}
AdvancedVisualization::~AdvancedVisualization() {
    shutdown();
}

bool AdvancedVisualization::initialize(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    try {
        // Initialize core components
        pImpl->debugConsole = std::make_unique<DebugConsole>();
        pImpl->networkVisualizer = std::make_unique<NetworkVisualizer>();
        pImpl->activityVisualizer = std::make_unique<ActivityVisualizer>();
        pImpl->memoryVisualizer = std::make_unique<MemoryVisualizer>();
        pImpl->neuromodulationVisualizer = std::make_unique<NeuromodulationVisualizer>();
        pImpl->developmentVisualizer = std::make_unique<DevelopmentVisualizer>();
        pImpl->profiler = std::make_unique<Profiler>();
        pImpl->exportUtils = std::make_unique<ExportUtils>();
        
        // Initialize plugins
        for (const auto& pluginName : pImpl->getRegisteredPlugins()) {
            if (registerPlugin(pluginName)) {
                Logger::getInstance().logInfo("Registered plugin: " + pluginName);
            }
        }
        
        pImpl->initialized = true;
        pImpl->running = true;
        
        Logger::getInstance().logInfo("Advanced visualization system initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize visualization: " + std::string(e.what()));
        return false;
    }
}

void AdvancedVisualization::update(double deltaTime) {
    if (!pImpl->initialized || !pImpl->running) return;
    
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    // Update all active components
    for (const auto& [name, enabled] : pImpl->componentStates) {
        if (enabled) {
            // Update component based on name
            if (name == "network" && pImpl->networkVisualizer) {
                pImpl->networkVisualizer->update(deltaTime);
            } else if (name == "activity" && pImpl->activityVisualizer) {
                pImpl->activityVisualizer->update(deltaTime);
            } else if (name == "memory" && pImpl->memoryVisualizer) {
                pImpl->memoryVisualizer->update(deltaTime);
            } else if (name == "neuromodulation" && pImpl->neuromodulationVisualizer) {
                pImpl->neuromodulationVisualizer->update(deltaTime);
            } else if (name == "development" && pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->update(deltaTime);
            } else if (name == "profiler" && pImpl->profiler) {
                pImpl->profiler->update(deltaTime);
            } else if (name == "export" && pImpl->exportUtils) {
                pImpl->exportUtils->update(deltaTime);
            }
        }
    }
    
    // Update plugins
    for (auto& plugin : pImpl->plugins) {
        if (plugin->isActive()) {
            plugin->update(deltaTime);
        }
    }
    
    // Update FPS calculation
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    pImpl->lastFrameTime = elapsed;
    pImpl->fps = 1.0 / elapsed;
    pImpl->totalFramesRendered++;
}

void AdvancedVisualization::render() {
    if (!pImpl->initialized || !pImpl->running) return;
    
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    // Render all active components
    for (const auto& [name, enabled] : pImpl->componentStates) {
        if (enabled) {
            if (name == "network" && pImpl->networkVisualizer) {
                pImpl->networkVisualizer->render();
            } else if (name == "activity" && pImpl->activityVisualizer) {
                pImpl->activityVisualizer->render();
            } else if (name == "memory" && pImpl->memoryVisualizer) {
                pImpl->memoryVisualizer->render();
            } else if (name == "neuromodulation" && pImpl->neuromodulationVisualizer) {
                pImpl->neuromodulationVisualizer->render();
            } else if (name == "development" && pImpl->developmentVisualizer) {
                pImpl->developmentVisualizer->render();
            } else if (name == "profiler" && pImpl->profiler) {
                pImpl->profiler->render();
            } else if (name == "export" && pImpl->exportUtils) {
                pImpl->exportUtils->render();
            }
        }
    }
    
    // Render plugins
    for (auto& plugin : pImpl->plugins) {
        if (plugin->isActive()) {
            plugin->render();
        }
    }
}

void AdvancedVisualization::shutdown() {
    if (pImpl->initialized) {
        // Shutdown plugins
        for (auto& plugin : pImpl->plugins) {
            plugin->shutdown();
        }
        pImpl->plugins.clear();
        
        pImpl->running = false;
        pImpl->initialized = false;
        
        Logger::getInstance().logInfo("Advanced visualization system shutdown");
    }
}

void AdvancedVisualization::setOutputFormat(OutputFormat format) {
    pImpl->outputFormat = format;
    if (pImpl->exportUtils) {
        pImpl->exportUtils->setOutputFormat(format);
    }
}

void AdvancedVisualization::setOutputPath(const std::string& path) {
    pImpl->outputPath = path;
    if (pImpl->exportUtils) {
        pImpl->exportUtils->setOutputPath(path);
    }
}

void AdvancedVisualization::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double AdvancedVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

void AdvancedVisualization::enableComponent(const std::string& name) {
    if (pImpl->componentStates.find(name) != pImpl->componentStates.end()) {
        pImpl->componentStates[name] = true;
    }
}

void AdvancedVisualization::disableComponent(const std::string& name) {
    if (pImpl->componentStates.find(name) != pImpl->componentStates.end()) {
        pImpl->componentStates[name] = false;
    }
}

bool AdvancedVisualization::isComponentEnabled(const std::string& name) const {
    auto it = pImpl->componentStates.find(name);
    return (it != pImpl->componentStates.end()) ? it->second : false;
}

bool AdvancedVisualization::registerPlugin(const std::string& pluginName) {
    auto plugin = PluginRegistry::getInstance().createPlugin(pluginName);
    if (plugin) {
        pImpl->plugins.push_back(std::move(plugin));
        return true;
    }
    return false;
}

void AdvancedVisualization::unregisterPlugin(const std::string& pluginName) {
    pImpl->plugins.erase(
        std::remove_if(pImpl->plugins.begin(), pImpl->plugins.end(),
                      [&pluginName](const std::unique_ptr<PluginInterface>& plugin) {
                          return plugin->getName() == pluginName;
                      }),
        pImpl->plugins.end()
    );
}

std::vector<std::string> AdvancedVisualization::getActivePlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& plugin : pImpl->plugins) {
        if (plugin->isActive()) {
            pluginNames.push_back(plugin->getName());
        }
    }
    return pluginNames;
}

void AdvancedVisualization::setDebugConsole(DebugConsole* console) {
    pImpl->debugConsole.reset();
    if (console) {
        pImpl->debugConsole.reset(console);
    }
}

void AdvancedVisualization::run(const Brain& brain, double deltaTime) {
    update(deltaTime);
    render();
}

bool AdvancedVisualization::exportData(const std::string& filename, OutputFormat format) {
    if (pImpl->exportUtils) {
        return pImpl->exportUtils->exportVisualization(filename, format);
    }
    return false;
}

double AdvancedVisualization::getFPS() const {
    return pImpl->fps;
}

double AdvancedVisualization::getLastFrameTime() const {
    return pImpl->lastFrameTime;
}

uint64_t AdvancedVisualization::getTotalFramesRendered() const {
    return pImpl->totalFramesRendered;
}

bool AdvancedVisualization::isRunning() const {
    return pImpl->running;
}

std::string AdvancedVisualization::getSystemStatus() const {
    std::stringstream status;
    status << "Visualization System Status:\n";
    status << "  Running: " << (pImpl->running ? "Yes" : "No") << "\n";
    status << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << "\n";
    status << "  Output Format: " << static_cast<int>(pImpl->outputFormat) << "\n";
    status << "  Update Rate: " << pImpl->updateRate << " Hz\n";
    status << "  FPS: " << pImpl->fps << "\n";
    status << "  Total Frames: " << pImpl->totalFramesRendered << "\n";
    status << "  Active Plugins: " << pImpl->plugins.size() << "\n";
    
    int enabledCount = 0;
    for (const auto& [name, enabled] : pImpl->componentStates) {
        if (enabled) enabledCount++;
    }
    status << "  Enabled Components: " << enabledCount << "/" << pImpl->componentStates.size() << "\n";
    
    return status.str();
}

} // namespace nlm