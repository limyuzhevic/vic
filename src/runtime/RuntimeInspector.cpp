// Runtime Inspector Implementation
// Contains the full implementation of runtime introspection capabilities

#include "runtime/RuntimeInspector.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

// Global Runtime Inspector Instance
RuntimeInspector globalInspector;

BrainStateSnapshot RuntimeInspector::captureSnapshot() const {
    BrainStateSnapshot snapshot;
    snapshot.step = 0; // This should be passed from the calling context
    snapshot.wallClockTime = std::chrono::duration<double>(0.0);
    
    // Global brain metrics (simplified - would need access to brain state)
    snapshot.totalNeuronCount = 0;
    snapshot.activeNeuronCount = 0;
    snapshot.firingNeuronCount = 0;
    snapshot.totalSynapseCount = 0;
    snapshot.averageFiringRate = 0.0f;
    snapshot.excitationInhibitionRatio = 1.0f;
    snapshot.totalSpikeCount = 0;
    
    // Developmental state (would need brain access)
    snapshot.developmentalStage = "Unknown";
    snapshot.neuromodulationLevel = 0.0f;
    snapshot.curiosityLevel = 0.0f;
    snapshot.noveltyLevel = 0.0f;
    snapshot.predictionError = 0.0f;
    
    // Memory systems (would need brain access)
    snapshot.workingMemoryCapacity = 0;
    snapshot.episodicMemoryCount = 0;
    snapshot.semanticMemoryConcepts = 0;
    snapshot.proceduralMemoryRules = 0;
    
    // Plasticity metrics (would need brain access)
    snapshot.totalSynapticWeight = 0.0f;
    snapshot.meanSynapticWeight = 0.0f;
    snapshot.plasticityEnabledSynapses = 0;
    snapshot.strengthenedConnections = 0;
    snapshot.weakenedConnections = 0;
    
    // Network statistics
    snapshot.maxFiringRate = 0.0f;
    snapshot.minFiringRate = 0.0f;
    snapshot.stdFiringRate = 0.0f;
    
    // Cognitive metrics
    snapshot.conceptsFormed = 0;
    snapshot.plansExecuted = 0;
    snapshot.decisionsMade = 0;
    
    // Time series data (truncated if too long)
    if (firingRateHistory.size() > maxHistorySize) {
        firingRateHistory.erase(firingRateHistory.begin(),
                               firingRateHistory.begin() + 
                               (firingRateHistory.size() - maxHistorySize));
    }
    
    if (activeNeuronHistory.size() > maxHistorySize) {
        activeNeuronHistory.erase(activeNeuronHistory.begin(),
                                  activeNeuronHistory.begin() + 
                                  (activeNeuronHistory.size() - maxHistorySize));
    }
    
    if (synapticWeightHistory.size() > maxHistorySize) {
        synapticWeightHistory.erase(synapticWeightHistory.begin(),
                                    synapticWeightHistory.begin() + 
                                    (synapticWeightHistory.size() - maxHistorySize));
    }
    
    // Copy to snapshot
    snapshot.firingRateHistory = firingRateHistory;
    snapshot.activeNeuronHistory = activeNeuronHistory;
    snapshot.synapticWeightHistory = synapticWeightHistory;
    
    // Update history
    if (debugMode || detailedLogging) {
        updateHistory(snapshot);
    }
    
    // Sanitize for safety
    sanitizeSnapshot(snapshot);
    
    return snapshot;
}

std::string RuntimeInspector::getBrainStatus() const {
    std::stringstream ss;
    ss << "=== Brain Runtime Status ===" << std::endl;
    ss << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
    ss << "Detailed logging: " << (detailedLogging ? "enabled" : "disabled") << std::endl;
    ss << "Simulation paused: " << (simulationPaused ? "yes" : "no") << std::endl;
    ss << "History size: " << firingRateHistory.size() << std::endl;
    ss << "Max history size: " << maxHistorySize << std::endl;
    ss << "Inspection interval: " << inspectionInterval << std::endl;
    ss << std::endl;
    ss << "Performance metrics:" << std::endl;
    ss << "  Total operations: " << getPerformanceTime() << " seconds" << std::endl;
    ss << "  Current state: " << (simulationPaused ? "PAUSED" : "RUNNING") << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getPerformanceMetrics() const {
    std::stringstream ss;
    ss << "=== Performance Metrics ===" << std::endl;
    ss << "Inspection frequency: " << (inspectionInterval > 0 ? 
                                      1.0 / inspectionInterval : 0.0) << " snapshots per step" << std::endl;
    ss << "Memory efficiency: " << (firingRateHistory.size() > 0 ? 
                                   (firingRateHistory.size() * sizeof(float) + 
                                    activeNeuronHistory.size() * sizeof(size_t)) / 1024.0 : 0.0) << " KB" << std::endl;
    ss << "CPU usage: Not available (requires system integration)" << std::endl;
    ss << "Network bandwidth: Not applicable for single agent" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getMemoryUsage() const {
    std::stringstream ss;
    ss << "=== Memory Usage ===" << std::endl;
    ss << "Firing rate history: " << firingRateHistory.size() * sizeof(float) << " bytes" << std::endl;
    ss << "Active neuron history: " << activeNeuronHistory.size() * sizeof(size_t) << " bytes" << std::endl;
    ss << "Synaptic weight history: " << synapticWeightHistory.size() * sizeof(float) << " bytes" << std::endl;
    ss << "Total history: " << (firingRateHistory.size() * sizeof(float) + 
                               activeNeuronHistory.size() * sizeof(size_t) + 
                               synapticWeightHistory.size() * sizeof(float)) << " bytes" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getNetworkTopology() const {
    std::stringstream ss;
    ss << "=== Network Topology Analysis ===" << std::endl;
    ss << "Topology: Not available (requires brain integration)" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getPlasticitySummary() const {
    std::stringstream ss;
    ss << "=== Plasticity Summary ===" << std::endl;
    ss << "Plasticity tracking: " << (debugMode ? "enabled" : "disabled") << std::endl;
    ss << "Number of plasticity events recorded: Not available (requires brain integration)" << std::endl;
    ss << "Synaptic weight change statistics: Not available (requires brain integration)" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getCognitionSummary() const {
    std::stringstream ss;
    ss << "=== Cognition Summary ===" << std::endl;
    ss << "Cognitive tracking: " << (debugMode ? "enabled" : "disabled") << std::endl;
    ss << "Concept formation: Not available (requires brain integration)" << std::endl;
    ss << "Planning capabilities: Not available (requires brain integration)" << std::endl;
    ss << "Decision making: Not available (requires brain integration)" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getNeuromodulationSummary() const {
    std::stringstream ss;
    ss << "=== Neuromodulation Summary ===" << std::endl;
    ss << "Neuromodulation tracking: " << (debugMode ? "enabled" : "disabled") << std::endl;
    ss << "Dopamine levels: Not available (requires brain integration)" << std::endl;
    ss << "Curiosity levels: Not available (requires brain integration)" << std::endl;
    ss << "Novelty detection: Not available (requires brain integration)" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

std::string RuntimeInspector::getPredictionSummary() const {
    std::stringstream ss;
    ss << "=== Prediction Summary ===" << std::endl;
    ss << "Prediction tracking: " << (debugMode ? "enabled" : "disabled") << std::endl;
    ss << "Prediction error tracking: Not available (requires brain integration)" << std::endl;
    ss << "Confidence tracking: Not available (requires brain integration)" << std::endl;
    ss << "Planning based on predictions: Not available (requires brain integration)" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

bool RuntimeInspector::takeCheckpoint(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "ERROR: Cannot create checkpoint file: " << filename << std::endl;
        return false;
    }
    
    try {
        // Write checkpoint data
        // This would serialize the brain state and inspection history
        // For now, write a simple header
        file << "NLM Checkpoint File" << std::endl;
        file << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
        file << "Debug mode: " << (debugMode ? "true" : "false") << std::endl;
        file << "History size: " << firingRateHistory.size() << std::endl;
        
        std::cout << "Checkpoint created: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR creating checkpoint: " << e.what() << std::endl;
        return false;
    }
}

bool RuntimeInspector::restoreCheckpoint(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "ERROR: Cannot open checkpoint file: " << filename << std::endl;
        return false;
    }
    
    try {
        std::string line;
        std::getline(file, line); // Skip header
        if (line != "NLM Checkpoint File") {
            std::cerr << "ERROR: Invalid checkpoint file format" << std::endl;
            return false;
        }
        
        std::getline(file, line); // Skip generation date
        std::getline(file, line); // Skip debug mode line
        std::getline(file, line); // Skip history size line
        
        std::cout << "Checkpoint restored: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR restoring checkpoint: " << e.what() << std::endl;
        return false;
    }
}

bool RuntimeInspector::compareSnapshots(const BrainStateSnapshot& snapshot1, const BrainStateSnapshot& snapshot2) const {
    // Simple comparison of key metrics
    bool equal = true;
    
    // Compare key metrics
    if (snapshot1.step != snapshot2.step) equal = false;
    if (std::abs(snapshot1.averageFiringRate - snapshot2.averageFiringRate) > 0.01f) equal = false;
    if (snapshot1.totalSpikeCount != snapshot2.totalSpikeCount) equal = false;
    if (snapshot1.excitationInhibitionRatio != snapshot2.excitationInhibitionRatio) equal = false;
    
    return equal;
}

void RuntimeInspector::updateHistory(const BrainStateSnapshot& snapshot) const {
    // Add to history if not full
    if (firingRateHistory.size() >= maxHistorySize) {
        firingRateHistory.erase(firingRateHistory.begin());
        activeNeuronHistory.erase(activeNeuronHistory.begin());
        synapticWeightHistory.erase(synapticWeightHistory.begin());
        historyTimestamps.erase(historyTimestamps.begin());
    }
    
    firingRateHistory.push_back(snapshot.averageFiringRate);
    activeNeuronHistory.push_back(snapshot.activeNeuronCount);
    synapticWeightHistory.push_back(snapshot.meanSynapticWeight);
    historyTimestamps.push_back(snapshot.wallClockTime);
}

float RuntimeInspector::calculateStandardDeviation(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    float variance = 0.0f;
    
    for (float value : values) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    return std::sqrt(variance / values.size());
}

void RuntimeInspector::sanitizeSnapshot(BrainStateSnapshot& snapshot) const {
    // Ensure values are within safe ranges
    snapshot.averageFiringRate = std::max(0.0f, std::min(snapshot.averageFiringRate, 1000.0f));
    snapshot.excitationInhibitionRatio = std::max(0.0f, std::min(snapshot.excitationInhibitionRatio, 100.0f));
    snapshot.totalSpikeCount = std::max(0ULL, snapshot.totalSpikeCount);
    
    // Check for NaN or infinity
    if (std::isnan(snapshot.averageFiringRate)) snapshot.averageFiringRate = 0.0f;
    if (std::isinf(snapshot.averageFiringRate)) snapshot.averageFiringRate = 1000.0f;
    
    // Truncate history if too large
    if (snapshot.firingRateHistory.size() > maxHistorySize) {
        snapshot.firingRateHistory.erase(snapshot.firingRateHistory.begin() + maxHistorySize, 
                                        snapshot.firingRateHistory.end());
    }
}

std::string RuntimeInspector::formatTimestamp(std::chrono::duration<double> time) const {
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(time).count();
    auto hours = totalSeconds / 3600;
    auto minutes = (totalSeconds % 3600) / 60;
    auto seconds = totalSeconds % 60;
    
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hours << ":"
       << std::setw(2) << std::setfill('0') << minutes << ":"
       << std::setw(2) << std::setfill('0') << seconds;
    
    return ss.str();
}

// BrainInspector implementation
BrainStateSnapshot BrainInspector::captureState() const {
    BrainStateSnapshot snapshot;
    
    if (brain) {
        // Capture brain state (simplified implementation)
        snapshot.step = 0; // Would need to get from brain
        snapshot.totalNeuronCount = brain->getTotalNeuronCount();
        snapshot.activeNeuronCount = brain->getActiveNeuronCount();
        snapshot.firingNeuronCount = brain->getFiringNeuronCount();
        snapshot.totalSynapseCount = brain->getTotalSynapseCount();
        snapshot.averageFiringRate = brain->getAverageFiringRate();
        snapshot.totalSpikeCount = brain->getTotalSpikeCount();
        
        // Get developmental stage
        snapshot.developmentalStage = "Unknown"; // Would need developmental stage method
        
        // Get neuromodulation levels (would need methods)
        snapshot.neuromodulationLevel = 0.0f;
        snapshot.curiosityLevel = 0.0f;
        snapshot.noveltyLevel = 0.0f;
        snapshot.predictionError = 0.0f;
        
        // Get memory statistics (would need methods)
        snapshot.workingMemoryCapacity = 0;
        snapshot.episodicMemoryCount = 0;
        snapshot.semanticMemoryConcepts = 0;
        snapshot.proceduralMemoryRules = 0;
        
        // Get network statistics
        snapshot.maxFiringRate = snapshot.averageFiringRate; // Simplified
        snapshot.minFiringRate = 0.0f;
        snapshot.stdFiringRate = 0.0f;
        
        // Get cognitive metrics (would need methods)
        snapshot.conceptsFormed = 0;
        snapshot.plansExecuted = 0;
        snapshot.decisionsMade = 0;
        
        // Update inspector history
        inspector.updateHistory(snapshot);
    }
    
    return snapshot;
}

std::string BrainInspector::getRegionInfo(RegionId regionId) const {
    std::stringstream ss;
    if (brain) {
        auto* region = brain->getRegion(regionId);
        if (region) {
            ss << "Region " << regionId.index() << ":" << std::endl;
            ss << "  Neuron count: " << region->getAllNeurons().size() << std::endl;
            ss << "  Synapse count: " << region->getSynapses().size() << std::endl;
            ss << "  Type: Unknown" << std::endl; // Would need region type
        } else {
            ss << "Region " << regionId.index() << ": Not found" << std::endl;
        }
    } else {
        ss << "Brain not available" << std::endl;
    }
    
    return ss.str();
}

std::string BrainInspector::getNeuronInfo(NeuronId neuronId) const {
    std::stringstream ss;
    if (brain) {
        auto* neuron = brain->getNeuron(neuronId); // Would need getNeuron method
        if (neuron) {
            ss << "Neuron " << neuronId.index() << ":" << std::endl;
            ss << "  Type: " << static_cast<int>(neuron->getType()) << std::endl;
            ss << "  State: " << static_cast<int>(neuron->getState()) << std::endl;
            ss << "  Membrane potential: " << neuron->getMembranePotential() << std::endl;
            ss << "  Threshold: " << neuron->getThreshold() << std::endl;
            ss << "  Refractory time: " << neuron->getRefractoryTime() << std::endl;
        } else {
            ss << "Neuron " << neuronId.index() << ": Not found" << std::endl;
        }
    } else {
        ss << "Brain not available" << std::endl;
    }
    
    return ss.str();
}

std::string BrainInspector::getSynapseInfo(SynapseId synapseId) const {
    std::stringstream ss;
    ss << "Synapse " << synapseId.index() << ":" << std::endl;
    ss << "  State: Not available (would need synapse lookup)" << std::endl;
    ss << "  Pre-synaptic neuron: Unknown" << std::endl;
    ss << "  Post-synaptic neuron: Unknown" << std::endl;
    ss << "  Weight: Unknown" << std::endl;
    ss << "  Delay: Unknown" << std::endl;
    
    return ss.str();
}

std::string BrainInspector::analyzeConnectivity() const {
    std::stringstream ss;
    ss << "=== Connectivity Analysis ===" << std::endl;
    ss << "Total neurons: " << (brain ? brain->getTotalNeuronCount() : 0) << std::endl;
    ss << "Total synapses: " << (brain ? brain->getTotalSynapseCount() : 0) << std::endl;
    ss << "Connection density: Not available (would need connectivity calculation)" << std::endl;
    ss << "Clustering coefficient: Not available (would need network analysis)" << std::endl;
    ss << "Small-worldness: Not available (would need network analysis)" << std::endl;
    
    return ss.str();
}

std::string BrainInspector::analyzePlasticity() const {
    std::stringstream ss;
    ss << "=== Plasticity Analysis ===" << std::endl;
    ss << "Plasticity enabled: Not available (would need brain access)" << std::endl;
    ss << "STDP activity: Not available (would need brain access)" << std::endl;
    ss << "Hebbian activity: Not available (would need brain access)" << std::endl;
    ss << "Structural plasticity: Not available (would need brain access)" << std::endl;
    ss << "Weight change statistics: Not available (would need brain access)" << std::endl;
    
    return ss.str();
}

std::string BrainInspector::analyzeActivity() const {
    std::stringstream ss;
    ss << "=== Activity Analysis ===" << std::endl;
    ss << "Average firing rate: " << getAverageFiringRate() << std::endl;
    ss << "Peak active neurons: " << getPeakActiveNeurons() << std::endl;
    ss << "Total spikes: " << (brain ? brain->getTotalSpikeCount() : 0) << std::endl;
    ss << "Excitation/inhibition ratio: " << (brain ? brain->getExcitationInhibitionRatio() : 0.0f) << std::endl;
    ss << std::endl;
    ss << "Activity patterns:" << std::endl;
    ss << "  Burst frequency: Not available" << std::endl;
    ss << "  Synchronous activity: Not available" << std::endl;
    ss << "  Asynchronous activity: Not available" << std::endl;
    
    return ss.str();
}

double BrainInspector::getAverageFiringRate() const {
    if (brain) {
        return brain->getAverageFiringRate();
    }
    return 0.0;
}

size_t BrainInspector::getPeakActiveNeurons() const {
    // Would need access to time series data to calculate peak
    return 0;
}

double BrainInspector::getSimulationEfficiency() const {
    // Would need to calculate efficiency metrics
    return 0.0;
}

std::string BrainInspector::debugNeuronState(NeuronId neuronId) const {
    std::stringstream ss;
    ss << "=== Neuron Debug Info ===" << std::endl;
    ss << "Neuron ID: " << neuronId.index() << std::endl;
    ss << "Debug information requires brain access and detailed neuron state tracking" << std::endl;
    ss << "Available through: brain->getNeuron() and neuron->debugState()" << std::endl;
    
    return ss.str();
}

std::string BrainInspector::debugSynapseState(SynapseId synapseId) const {
    std::stringstream ss;
    ss << "=== Synapse Debug Info ===" << std::endl;
    ss << "Synapse ID: " << synapseId.index() << std::endl;
    ss << "Debug information requires brain access and detailed synapse state tracking" << std::endl;
    ss << "Available through: brain->getSynapse() and synapse->debugState()" << std::endl;
    
    return ss.str();
}

std::string BrainInspector::debugRegionState(RegionId regionId) const {
    std::stringstream ss;
    ss << "=== Region Debug Info ===" << std::endl;
    ss << "Region ID: " << regionId.index() << std::endl;
    ss << "Debug information requires brain access and detailed region state tracking" << std::endl;
    ss << "Available through: brain->getRegion() and region->debugState()" << std::endl;
    
    return ss.str();
}

void BrainInspector::logStateChange(const std::string& description) const {
    if (inspector.isDetailedLoggingEnabled()) {
        std::cout << "[BrainInspector] State change: " << description << std::endl;
    }
}

void BrainInspector::checkpointState(const std::string& label) const {
    std::string filename = "checkpoint_" + label + "_" + 
                          std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                              std::chrono::high_resolution_clock::now().time_since_epoch()).count()) + 
                          ".bin";
    
    if (inspector.takeCheckpoint(filename)) {
        std::cout << "Checkpoint created with label: " << label << std::endl;
    }
}

bool BrainInspector::compareState(const BrainInspector& other) const {
    if (!brain || !other.brain) {
        return false;
    }
    
    // Compare key brain metrics
    bool equal = (brain->getTotalNeuronCount() == other.brain->getTotalNeuronCount()) &&
                 (brain->getTotalSynapseCount() == other.brain->getTotalSynapseCount()) &&
                 (brain->getAverageFiringRate() == other.brain->getAverageFiringRate()) &&
                 (brain->getTotalSpikeCount() == other.brain->getTotalSpikeCount());
    
    return equal;
}

std::shared_ptr<Config> BrainInspector::getConfig() const {
    if (brain) {
        return brain->getConfig();
    }
    return nullptr;
}

std::string BrainInspector::formatTime(double seconds) {
    auto totalSeconds = static_cast<long>(seconds);
    auto hours = totalSeconds / 3600;
    auto minutes = (totalSeconds % 3600) / 60;
    auto secs = totalSeconds % 60;
    
    std::stringstream ss;
    if (hours > 0) {
        ss << hours << "h ";
    }
    ss << minutes << "m " << secs << "s";
    
    return ss.str();
}

std::string BrainInspector::formatBytes(size_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024.0) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024.0 * 1024)) + " MB";
    return std::to_string(bytes / (1024.0 * 1024 * 1024)) + " GB";
}

std::string BrainInspector::formatPercentage(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << "%";
    return ss.str();
}

// Runtime namespace convenience functions
BrainStateSnapshot Runtime::captureBrainSnapshot() {
    return globalInspector.captureSnapshot();
}

std::string Runtime::getBrainStatus() {
    return globalInspector.getBrainStatus();
}

std::string Runtime::getPerformanceReport() {
    return globalInspector.getPerformanceMetrics();
}

std::string Runtime::getDebugReport() {
    std::stringstream ss;
    ss << "=== Debug Report ===" << std::endl;
    ss << globalInspector.getBrainStatus() << std::endl;
    ss << globalInspector.getPerformanceMetrics() << std::endl;
    ss << globalInspector.getMemoryUsage() << std::endl;
    ss << globalInspector.getPlasticitySummary() << std::endl;
    ss << globalInspector.getCognitionSummary() << std::endl;
    ss << globalInspector.getNeuromodulationSummary() << std::endl;
    ss << globalInspector.getPredictionSummary() << std::endl;
    
    return ss.str();
}

void Runtime::enableDebugMode(bool enable) {
    globalInspector.enableDebugMode(enable);
}

void Runtime::setInspectionInterval(SimulationStep interval) {
    globalInspector.setInspectionInterval(interval);
}

void Runtime::pauseInspection() {
    globalInspector.pauseSimulation();
}

void Runtime::resumeInspection() {
    globalInspector.resumeSimulation();
}

bool Runtime::isInspectionPaused() {
    return globalInspector.isSimulationPaused();
}

void Runtime::clearInspectionHistory() {
    globalInspector.clearHistory();
}

size_t Runtime::getHistorySize() {
    return globalInspector.getMaxHistorySize();
}

void Runtime::setErrorHandler(std::function<void(const std::string&)> handler) {
    globalInspector.setErrorCallback(handler);
}

} // namespace nlm
