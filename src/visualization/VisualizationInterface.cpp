#include "VisualizationInterface.hpp"
#include "../core/Logger.hpp"
#include <algorithm>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    // Integration with AdvancedVisualization system
    AdvancedVisualization* advancedVisualization;
    DebugConsole* debugConsole;
    
    Impl() : active(false), updateRate(30.0), 
             advancedVisualization(nullptr), debugConsole(nullptr) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}
VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(const std::string& configFile) {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->initialize(configFile);
    }
    
    pImpl->active = true; // Basic active state for backward compatibility
    Logger::getInstance().logInfo("Basic visualization interface initialized");
    return true;
}

void VisualizationInterface::update(double deltaTime) {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->update(deltaTime);
    }
}

void VisualizationInterface::render() {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->render();
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->shutdown();
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->setUpdateRate(hz);
    }
}

double VisualizationInterface::getUpdateRate() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getUpdateRate();
    }
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (pImpl->advancedVisualization) {
        // Pass brain to advanced visualization for processing
        // Note: This requires the AdvancedVisualization to have access to brain data
        // In a real implementation, this would be through proper API
    }
    // Backward compatibility: no-op
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->advancedVisualization) {
        // Pass brain to advanced visualization for processing
    }
    // Backward compatibility: no-op
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->advancedVisualization) {
        // Pass spike data to advanced visualization
    }
    // Backward compatibility: no-op
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (pImpl->advancedVisualization) {
        // Pass brain to advanced visualization for weight visualization
    }
    // Backward compatibility: no-op
}

void VisualizationInterface::setAdvancedVisualization(AdvancedVisualization* advancedVis) {
    pImpl->advancedVisualization = advancedVis;
    if (advancedVis && pImpl->debugConsole) {
        advancedVis->setDebugConsole(pImpl->debugConsole);
    }
}

void VisualizationInterface::run(const Brain& brain, double deltaTime) {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->run(brain, deltaTime);
    }
}

void VisualizationInterface::enableComponent(const std::string& name) {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->enableComponent(name);
    }
}

void VisualizationInterface::disableComponent(const std::string& name) {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->disableComponent(name);
    }
}

bool VisualizationInterface::isComponentEnabled(const std::string& name) const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->isComponentEnabled(name);
    }
    return false;
}

bool VisualizationInterface::exportData(const std::string& filename, OutputFormat format) {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->exportData(filename, format);
    }
    return false;
}

double VisualizationInterface::getFPS() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getFPS();
    }
    return 0.0;
}

double VisualizationInterface::getLastFrameTime() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getLastFrameTime();
    }
    return 0.0;
}

uint64_t VisualizationInterface::getTotalFramesRendered() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getTotalFramesRendered();
    }
    return 0;
}

std::string VisualizationInterface::getSystemStatus() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getSystemStatus();
    }
    return "Basic visualization interface (no AdvancedVisualization attached)";
}

void VisualizationInterface::setDebugConsole(DebugConsole* console) {
    pImpl->debugConsole = console;
    if (pImpl->advancedVisualization && console) {
        pImpl->advancedVisualization->setDebugConsole(console);
    }
}

bool VisualizationInterface::registerPlugin(const std::string& pluginName) {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->registerPlugin(pluginName);
    }
    return false;
}

void VisualizationInterface::unregisterPlugin(const std::string& pluginName) {
    if (pImpl->advancedVisualization) {
        pImpl->advancedVisualization->unregisterPlugin(pluginName);
    }
}

std::vector<std::string> VisualizationInterface::getActivePlugins() const {
    if (pImpl->advancedVisualization) {
        return pImpl->advancedVisualization->getActivePlugins();
    }
    return std::vector<std::string>();
}

} // namespace nlm