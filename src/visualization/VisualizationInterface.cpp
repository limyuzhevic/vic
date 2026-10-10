#include "VisualizationInterface.hpp"
#include "VisualizationEngine.hpp"
#include <iostream>

namespace nlm {

struct VisualizationInterface::Impl {
    std::shared_ptr<Brain> brain;
    std::unique_ptr<VisualizationEngine> engine;
    bool active;
    double updateRate;
    
    Impl() : brain(nullptr), engine(nullptr), active(false), updateRate(30.0) {
        engine = std::make_unique<VisualizationEngine>();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) {
        std::cout << "Error: Invalid brain pointer provided to visualization interface" << std::endl;
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->engine->setBrain(brain);
    
    // Initialize visualization engine
    if (pImpl->engine->initialize(brain)) {
        pImpl->active = true;
        std::cout << "Visualization interface initialized successfully" << std::endl;
        return true;
    } else {
        pImpl->active = false;
        std::cout << "Failed to initialize visualization interface" << std::endl;
        return false;
    }
}

void VisualizationInterface::update() {
    if (!pImpl->active || !pImpl->brain) {
        return;
    }
    
    pImpl->engine->update();
}

void VisualizationInterface::render() {
    if (!pImpl->active || !pImpl->brain) {
        return;
    }
    
    pImpl->engine->render();
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        pImpl->engine->close();
        pImpl->active = false;
        std::cout << "Visualization interface closed" << std::endl;
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active && pImpl->engine->isRunning();
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->engine) {
        // Update engine update rate if needed
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setDisplayMode(int mode) {
    if (pImpl->engine) {
        pImpl->engine->setDisplayMode(static_cast<VisualizationEngine::DisplayMode>(mode));
    }
}

int VisualizationInterface::getDisplayMode() const {
    if (pImpl->engine) {
        return static_cast<int>(pImpl->engine->getDisplayMode());
    }
    return static_cast<int>(VisualizationEngine::DisplayMode::Consolidated);
}

void VisualizationInterface::setBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        std::cout << "Warning: Attempting to set null brain pointer" << std::endl;
        return;
    }
    
    pImpl->brain = brain;
    if (pImpl->engine) {
        pImpl->engine->setBrain(brain);
    }
    
    std::cout << "Brain reference updated in visualization interface" << std::endl;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (pImpl->engine) {
        pImpl->engine->setDisplayMode(VisualizationEngine::DisplayMode::Structural);
        pImpl->engine->update();
        pImpl->engine->render();
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->engine) {
        pImpl->engine->setDisplayMode(VisualizationEngine::DisplayMode::Activity);
        pImpl->engine->update();
        pImpl->engine->render();
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Delegate to activity renderer if available
    if (pImpl->engine && pImpl->engine->getActivityRenderer()) {
        // This would require adding spike visualization capability
        // For now, we'll just log the action
        std::cout << "Spike visualization requested - delegating to activity renderer" << std::endl;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // This would use the brain structure renderer
    if (pImpl->engine && pImpl->engine->getBrainStructureRenderer()) {
        // Delegate to brain structure renderer
        std::cout << "Weight visualization requested - delegating to brain structure renderer" << std::endl;
    }
}

bool VisualizationInterface::exportData(const std::string& filename, const std::string& format) {
    if (!pImpl->engine) {
        std::cout << "Error: Visualization engine not available for export" << std::endl;
        return false;
    }
    
    std::cout << "Exporting visualization data to: " << filename << std::endl;
    
    // Create export options
    ExportRenderer::ExportOptions options;
    
    // Set format based on string input
    if (format == "csv") {
        options.format = ExportRenderer::ExportFormat::CSV;
    } else if (format == "json") {
        options.format = ExportRenderer::ExportFormat::JSON;
    } else if (format == "html") {
        options.format = ExportRenderer::ExportFormat::HTML;
    } else {
        options.format = ExportRenderer::ExportFormat::ASCII;
    }
    
    options.outputPath = filename;
    options.title = "NLM Visualization Export";
    options.description = "Neural network visualization export";
    
    // Perform export
    bool success = pImpl->engine->exportData(options);
    
    if (success) {
        std::cout << "Export completed successfully" << std::endl;
    } else {
        std::cout << "Export failed: " << pImpl->engine->getLastError() << std::endl;
    }
    
    return success;
}

void VisualizationInterface::loadConfiguration(const std::string& configFile) {
    if (pImpl->engine) {
        pImpl->engine->loadConfiguration(configFile);
        std::cout << "Configuration loaded from: " << configFile << std::endl;
    } else {
        std::cout << "Warning: Cannot load configuration - engine not available" << std::endl;
    }
}

void VisualizationInterface::saveConfiguration() const {
    if (pImpl->engine) {
        pImpl->engine->saveConfiguration();
        std::cout << "Configuration saved" << std::endl;
    } else {
        std::cout << "Warning: Cannot save configuration - engine not available" << std::endl;
    }
}

} // namespace nlm
