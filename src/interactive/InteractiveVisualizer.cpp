#include "InteractiveVisualizationTools.hpp"

namespace nlm {

struct InteractiveVisualizer::Impl {
    VisualizationEngine* visualizationEngine;
    const Brain* interactiveBrain;
    bool interactiveSessionActive;
    std::vector<std::unordered_map<std::string, std::string>> interactiveHistory;
    std::function<void(const std::string&, const std::unordered_map<std::string, std::string>&)> interactiveCallback;
    double lastUpdateTime;
    
    Impl() : visualizationEngine(nullptr), interactiveBrain(nullptr), interactiveSessionActive(false),
             lastUpdateTime(0.0) {}
};

InteractiveVisualizer::InteractiveVisualizer() : pImpl(std::make_unique<Impl>()) {}

InteractiveVisualizer::~InteractiveVisualizer() {
    if (pImpl->interactiveSessionActive) {
        stopInteractiveSession();
    }
}

InteractiveVisualizer::InteractiveVisualizer(InteractiveVisualizer&&) noexcept = default;
InteractiveVisualizer& InteractiveVisualizer::operator=(InteractiveVisualizer&&) noexcept = default;

bool InteractiveVisualizer::initialize(VisualizationEngine& visualizationEngine) {
    pImpl->visualizationEngine = &visualizationEngine;
    return true;
}

bool InteractiveVisualizer::startInteractiveSession(const Brain& brain) {
    if (pImpl->interactiveSessionActive) {
        return false;
    }
    
    pImpl->interactiveBrain = &brain;
    pImpl->interactiveSessionActive = true;
    
    // Initialize visualization
    if (pImpl->visualizationEngine) {
        auto& interface = pImpl->visualizationEngine->getVisualizationInterface();
        interface.initialize();
        interface.visualizeNetwork(brain);
        interface.visualizeActivity(brain);
    }
    
    return true;
}

bool InteractiveVisualizer::stopInteractiveSession() {
    if (!pImpl->interactiveSessionActive) {
        return false;
    }
    
    pImpl->interactiveSessionActive = false;
    pImpl->interactiveBrain = nullptr;
    
    // Cleanup visualization
    if (pImpl->visualizationEngine) {
        auto& interface = pImpl->visualizationEngine->getVisualizationInterface();
        interface.close();
    }
    
    return true;
}

bool InteractiveVisualizer::showControlPanel() {
    // Implementation would show control panel
    return true;
}

bool InteractiveVisualizer::showVisualizationControls() {
    // Implementation would show visualization controls
    return true;
}

bool InteractiveVisualizer::showAnalysisControls() {
    // Implementation would show analysis controls
    return true;
}

bool InteractiveVisualizer::showDebuggingControls() {
    // Implementation would show debugging controls
    return true;
}

bool InteractiveVisualizer::showExperimentControls() {
    // Implementation would show experiment controls
    return true;
}

void InteractiveVisualizer::setVisualizationPreset(const std::string& presetName) {
    // Implementation would set visualization preset
}

void InteractiveVisualizer::loadExperiment(const std::string& experimentName) {
    // Implementation would load experiment configuration
}

void InteractiveVisualizer::saveExperiment(const std::string& experimentName) {
    // Implementation would save experiment configuration
}

void InteractiveVisualizer::runExperiment(const std::string& experimentName) {
    // Implementation would run experiment
}

std::unordered_map<std::string, std::string> InteractiveVisualizer::getExperimentState() const {
    std::unordered_map<std::string, std::string> state;
    
    if (!pImpl->interactiveBrain) {
        return state;
    }
    
    state["total_neurons"] = std::to_string(pImpl->interactiveBrain->getTotalNeuronCount());
    state["total_spikes"] = std::to_string(pImpl->interactiveBrain->getTotalSpikeCount());
    state["firing_rate"] = std::to_string(pImpl->interactiveBrain->getFiringNeuronCount() /
                                          std::max(1.0, static_cast<double>(pImpl->interactiveBrain->getTotalNeuronCount())));
    
    return state;
}

void InteractiveVisualizer::setInteractiveCallback(std::function<void(const std::string&, const std::unordered_map<std::string, std::string>&)> callback) {
    pImpl->interactiveCallback = callback;
}

std::vector<std::unordered_map<std::string, std::string>> InteractiveVisualizer::getInteractiveHistory(size_t count) const {
    std::vector<std::unordered_map<std::string, std::string>> history;
    
    size_t startIndex = std::max(0, (int64_t)(pImpl->interactiveHistory.size() - count));
    for (size_t i = startIndex; i < pImpl->interactiveHistory.size(); ++i) {
        history.push_back(pImpl->interactiveHistory[i]);
    }
    
    return history;
}

bool InteractiveVisualizer::exportInteractiveSession(const std::string& format, const std::string& filepath) {
    // Implementation would export interactive session in specified format
    return false;
}

bool InteractiveVisualizer::isInteractiveSessionActive() const {
    return pImpl->interactiveSessionActive;
}

} // namespace nlm