// NeuralExperiment implementation - common neural experiment base class
#include "ExperimentInfrastructure.hpp"

namespace nlm {

NeuralExperiment::NeuralExperiment() : name_("Neural Experiment") {
    // Initialize logger with default file
    logger_ = ExperimentLogger("experiment.log");
}

NeuralExperiment::~NeuralExperiment() {
    logger_.flush();
}

NeuralExperiment::~NeuralExperiment() {
    logger_.flush();
}

void NeuralExperiment::logProgress(const std::string& message) {
    logger_.log("PROGRESS: " + message);
}

void NeuralExperiment::logError(const std::string& message) {
    logger_.log("ERROR: " + message);
}

void NeuralExperiment::logWarning(const std::string& message) {
    logger_.log("WARNING: " + message);
}

bool NeuralExperiment::validateBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        logError("Brain pointer is null");
        return false;
    }
    
    if (!brain->isInitialized()) {
        logError("Brain is not initialized");
        return false;
    }
    
    return true;
}

void NeuralExperiment::recordNetworkMetrics(std::shared_ptr<Brain> brain, SimulationStep step) {
    if (!validateBrain(brain)) {
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        logWarning("No region found for metrics recording");
        return;
    }
    
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalFiring = brain->getFiringNeuronCount();
    float avgFiringRate = brain->getAverageFiringRate();
    
    metrics_.totalSynapses = totalSynapses;
    metrics_.totalSpikes = brain->getTotalSpikeCount();
    metrics_.averageFiringRate = avgFiringRate;
    metrics_.steps = static_cast<int>(step);
    
    logger_.logSynapticStats(totalSynapses, metrics_.averageWeight, metrics_.maxWeight);
}

} // namespace nlm
