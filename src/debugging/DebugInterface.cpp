#include "DebugTools.hpp"

namespace nlm {

struct DebugInterface::Impl {
    VisualizationInterface* visualizationInterface;
    const Brain* debuggedBrain;
    bool debuggingSessionActive;
    std::vector<std::unordered_map<std::string, std::string>> executionHistory;
    std::unordered_map<int64_t, std::unordered_map<std::string, std::string>> stepStates;
    std::unordered_set<int64_t> breakpoints;
    std::vector<std::string> watchVariables;
    std::function<void(int64_t, double)> stepCallback;
    std::function<void(const std::string&, const std::string&)> errorCallback;
    std::function<void(int64_t)> breakpointCallback;
    double lastStepTime;
    
    Impl() : visualizationInterface(nullptr), debuggedBrain(nullptr), debuggingSessionActive(false),
             lastStepTime(0.0) {}
};

DebugInterface::DebugInterface() : pImpl(std::make_unique<Impl>()) {}

DebugInterface::~DebugInterface() {
    if (pImpl->debuggingSessionActive) {
        stopDebugSession();
    }
}

DebugInterface::DebugInterface(DebugInterface&&) noexcept = default;
DebugInterface& DebugInterface::operator=(DebugInterface&&) noexcept = default;

bool DebugInterface::initialize(VisualizationInterface& visualizationInterface) {
    pImpl->visualizationInterface = &visualizationInterface;
    return true;
}

bool DebugInterface::startDebugSession(const Brain& brain) {
    if (pImpl->debuggingSessionActive) {
        return false;
    }
    
    pImpl->debuggedBrain = &brain;
    pImpl->debuggingSessionActive = true;
    
    // Initialize visualization
    if (pImpl->visualizationInterface) {
        pImpl->visualizationInterface->initialize();
        pImpl->visualizationInterface->visualizeNetwork(brain);
    }
    
    return true;
}

bool DebugInterface::stopDebugSession() {
    if (!pImpl->debuggingSessionActive) {
        return false;
    }
    
    pImpl->debuggingSessionActive = false;
    pImpl->debuggedBrain = nullptr;
    
    // Cleanup visualization
    if (pImpl->visualizationInterface) {
        pImpl->visualizationInterface->close();
    }
    
    return true;
}

bool DebugInterface::executeStep(int64_t stepNumber, double time) {
    if (!pImpl->debuggingSessionActive || !pImpl->debuggedBrain) {
        return false;
    }
    
    try {
        // Execute the step
        pImpl->debuggedBrain->step(stepNumber, time);
        
        // Record state
        auto state = getExecutionState();
        pImpl->stepStates[stepNumber] = state;
        pImpl->executionHistory.push_back(state);
        
        // Check for breakpoints
        if (pImpl->breakpoints.find(stepNumber) != pImpl->breakpoints.end()) {
            if (pImpl->breakpointCallback) {
                pImpl->breakpointCallback(stepNumber);
            }
        }
        
        // Call step callback
        if (pImpl->stepCallback) {
            pImpl->stepCallback(stepNumber, time);
        }
        
        pImpl->lastStepTime = time;
        
        return true;
    } catch (const std::exception& e) {
        if (pImpl->errorCallback) {
            pImpl->errorCallback("STEP_EXECUTION", e.what());
        }
        return false;
    }
}

void DebugInterface::setBreakpoint(int64_t stepNumber) {
    pImpl->breakpoints.insert(stepNumber);
}

void DebugInterface::removeBreakpoint(int64_t stepNumber) {
    pImpl->breakpoints.erase(stepNumber);
}

std::unordered_map<std::string, std::string> DebugInterface::getExecutionState() const {
    std::unordered_map<std::string, std::string> state;
    
    if (!pImpl->debuggedBrain) {
        return state;
    }
    
    state["step"] = std::to_string(pImpl->stepStates.size() > 0 ? pImpl->stepStates.rbegin()->first : 0);
    state["total_neurons"] = std::to_string(pImpl->debuggedBrain->getTotalNeuronCount());
    state["total_spikes"] = std::to_string(pImpl->debuggedBrain->getTotalSpikeCount());
    state["firing_rate"] = std::to_string(pImpl->debuggedBrain->getFiringNeuronCount() /
                                          std::max(1.0, static_cast<double>(pImpl->debuggedBrain->getTotalNeuronCount())));
    state["time"] = std::to_string(pImpl->lastStepTime);
    
    return state;
}

std::vector<std::unordered_map<std::string, std::string>> DebugInterface::getExecutionHistory(size_t count) const {
    std::vector<std::unordered_map<std::string, std::string>> history;
    
    size_t startIndex = std::max(0, (int64_t)(pImpl->executionHistory.size() - count));
    for (size_t i = startIndex; i < pImpl->executionHistory.size(); ++i) {
        history.push_back(pImpl->executionHistory[i]);
    }
    
    return history;
}

std::vector<std::string> DebugInterface::getCallStack() const {
    return std::vector<std::string>(); // Placeholder - would require stack walking
}

std::string DebugInterface::inspectVariable(const std::string& variablePath) const {
    return "Variable not found: " + variablePath; // Placeholder
}

void DebugInterface::setVariableWatch(const std::string& variablePath) {
    pImpl->watchVariables.push_back(variablePath);
}

std::vector<std::string> DebugInterface::getWatchList() const {
    return pImpl->watchVariables;
}

std::vector<std::unordered_map<std::string, std::string>> DebugInterface::getErrorVisualization() const {
    return std::vector<std::unordered_map<std::string, std::string>>(); // Placeholder
}

bool DebugInterface::exportDebugSession(const std::string& format, const std::string& filepath) {
    // Implementation would export debug session in specified format
    return false;
}

void DebugInterface::setStepCallback(std::function<void(int64_t, double)> callback) {
    pImpl->stepCallback = callback;
}

void DebugInterface::setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pImpl->errorCallback = callback;
}

void DebugInterface::setBreakpointCallback(std::function<void(int64_t)> callback) {
    pImpl->breakpointCallback = callback;
}

} // namespace nlm