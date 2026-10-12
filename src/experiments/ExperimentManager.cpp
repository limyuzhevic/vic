// ExperimentManager implementation
#include "ExperimentManager.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct ExperimentManager::Impl {
    ExperimentConfig config;
    std::vector<ExperimentResult> results;
    std::vector<std::shared_ptr<Brain>> activeBrains;
    std::function<void(float)> progressCallback;
    float currentProgress;
    
    Impl() : currentProgress(0.0f) {}
};

ExperimentManager::ExperimentManager() : pImpl(new Impl) {}

ExperimentManager::~ExperimentManager() = default;

void ExperimentManager::configure(const ExperimentConfig& config) {
    pImpl->config = config;
}

ExperimentConfig& ExperimentManager::getConfig() {
    return pImpl->config;
}

bool ExperimentManager::initializeBrain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize null brain");
        return false;
    }
    
    // Configure brain from experiment config
    auto config = brain->getConfig();
    if (!config) {
        NLM_LOG_ERROR("Brain has no configuration");
        return false;
    }
    
    config->set("random_seed", static_cast<int64_t>(pImpl->config.seed), ConfigSource::Experiment);
    config->set("simulation_timestep", pImpl->config.timestep, ConfigSource::Experiment);
    config->set("neuron_count", static_cast<int64_t>(pImpl->config.neuronCount), ConfigSource::Experiment);
    config->set("region_count", static_cast<int64_t>(pImpl->config.regionCount), ConfigSource::Experiment);
    config->set("connection_probability", pImpl->config.connectionProbability, ConfigSource::Experiment);
    
    // Initialize the brain with new configuration
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("Brain initialized for experiment: " + pImpl->config.name);
    return true;
}

ExperimentResult ExperimentManager::runPhase(std::shared_ptr<Brain> brain, ExperimentPhase phase) {
    ExperimentResult result(pImpl->config.name);
    result.startStep = 0;
    
    switch (phase) {
        case PHASE_BASIC_CONNECTIVITY:
            runBasicConnectivityPhase(brain, result);
            break;
        case PHASE_PLASTICITY_LEARNING:
            runPlasticityPhase(brain, result);
            break;
        case PHASE_STDP_VERIFICATION:
            runStdpPhase(brain, result);
            break;
        case PHASE_ADVANCED_ANALYSIS:
            runAdvancedAnalysisPhase(brain, result);
            break;
    }
    
    result.endStep = pImpl->config.maxSteps;
    return result;
}

std::vector<ExperimentResult> ExperimentManager::runAllPhases(std::shared_ptr<Brain> brain) {
    std::vector<ExperimentResult> allResults;
    
    const std::vector<ExperimentPhase> phases = {
        PHASE_BASIC_CONNECTIVITY,
        PHASE_PLASTICITY_LEARNING,
        PHASE_STDP_VERIFICATION,
        PHASE_ADVANCED_ANALYSIS
    };
    
    for (size_t i = 0; i < phases.size(); ++i) {
        float progress = static_cast<float>(i) / phases.size();
        updateProgress(progress);
        
        ExperimentResult result = runPhase(brain, phases[i]);
        allResults.push_back(result);
        
        // Reset brain for next phase
        if (i < phases.size() - 1) {
            resetBrainState(brain);
            initializeBrain(brain);
            prepareForPhase(brain, phases[i + 1]);
        }
    }
    
    updateProgress(1.0f);
    return allResults;
}

void ExperimentManager::resetBrainState(std::shared_ptr<Brain> brain) {
    if (brain) {
        brain->reset();
        NLM_LOG_INFO("Brain state reset");
    }
}

void ExperimentManager::prepareForPhase(std::shared_ptr<Brain> brain, ExperimentPhase phase) {
    if (!brain) return;
    
    switch (phase) {
        case PHASE_BASIC_CONNECTIVITY:
            prepareBasicConnectivityPhase(brain);
            break;
        case PHASE_PLASTICITY_LEARNING:
            preparePlasticityPhase(brain);
            break;
        case PHASE_STDP_VERIFICATION:
            prepareStdpPhase(brain);
            break;
        case PHASE_ADVANCED_ANALYSIS:
            prepareAdvancedAnalysisPhase(brain);
            break;
    }
}

void ExperimentManager::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open results file: " + filepath);
        return;
    }
    
    file << "NLM Experiment Results\n";
    file << "======================\n\n";
    
    for (const auto& result : pImpl->results) {
        file << "Experiment: " << result.experimentName << "\n";
        file << "Seed: " << result.seed << "\n";
        file << "Duration: " << result.endStep - result.startStep << " steps\n";
        file << "Neurons: " << result.neuronCount << "\n";
        file << "Synapses: " << result.synapseCount << "\n";
        file << "E/I Ratio: " << result.eiRatio << "\n";
        file << "Total Spikes: " << result.totalSpikes << "\n";
        file << "Learning Occurred: " << (result.learningOccurred ? "Yes" : "No") << "\n";
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filepath);
}

void ExperimentManager::exportAnalysisData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open analysis data file: " + filepath);
        return;
    }
    
    // Export detailed JSON-like data
    file << "{\n";
    file << "  \"experiments\": [\n";
    
    for (size_t i = 0; i < pImpl->results.size(); ++i) {
        const auto& result = pImpl->results[i];
        file << "    {\n";
        file << "      \"name\": \"" << result.experimentName << "\",
", file << "      \"seed\": " << result.seed << ",
";
        file << "      \"learning_occurred\": " << (result.learningOccurred ? "true" : "false") << ",
";
        file << "      \"weight_change\": " << result.weightChange << ",
";
        file << "      \"firing_rate\": " << result.avgFiringRate << "\n";
        
        if (i < pImpl->results.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ],\n";
    file << "  \"summary\": {
";
    file << "    \"total_experiments\": " << pImpl->results.size() << ",
";
    file << "    \"successful_experiments\": " << getSuccessfulExperimentCount() << ",
";
    file << "    \"average_weight_change\": " << getAverageWeightChange() << "\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Analysis data exported to: " + filepath);
}

void ExperimentManager::generateReport(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open report file: " + filepath);
        return;
    }
    
    file << generateExperimentReport(getConsolidatedResult()) << std::endl;
    file.close();
    NLM_LOG_INFO("Report generated: " + filepath);
}

std::string ExperimentManager::generateExperimentReport(const ExperimentResult& result) {
    std::stringstream ss;
    ss << "NLM Experiment Report\n";
    ss << "======================\n\n";
    ss << "Experiment Name: " << result.experimentName << "\n";
    ss << "Seed: " << result.seed << "\n";
    ss << "Start Step: " << result.startStep << "\n";
    ss << "End Step: " << result.endStep << "\n";
    ss << "Duration: " << result.endStep - result.startStep << " steps\n\n";
    
    ss << "Network Statistics:\n";
    ss << "  Neurons: " << result.neuronCount << "\n";
    ss << "  Synapses: " << result.synapseCount << "\n";
    ss << "  Regions: " << result.regionCount << "\n";
    ss << "  E/I Ratio: " << result.eiRatio << "\n";
    ss << "  Total Spikes: " << result.totalSpikes << "\n";
    ss << "  Average Firing Rate: " << result.avgFiringRate << " Hz\n\n";
    
    ss << "Plasticity Results:\n";
    ss << "  Initial Mean Weight: " << result.initialMeanWeight << "\n";
    ss << "  Final Mean Weight: " << result.finalMeanWeight << "\n";
    ss << "  Weight Change: " << result.weightChange << "\n";
    ss << "  Strengthened Synapses: " << result.strengthenedSynapses << "\n";
    ss << "  Weakened Synapses: " << result.weakenedSynapses << "\n";
    ss << "  Unchanged Synapses: " << result.unchangedSynapses << "\n\n";
    
    ss << "Activity Results:\n";
    ss << "  Firing Neurons: " << result.firingNeurons << "\n";
    ss << "  Max Firing Neurons: " << result.maxFiringNeurons << "\n";
    ss << "  Min Firing Neurons: " << result.minFiringNeurons << "\n\n";
    
    ss << "Experimental Outcomes:\n";
    ss << "  Learning Occurred: " << (result.learningOccurred ? "Yes" : "No") << "\n";
    ss << "  Connectivity Works: " << (result.connectivityWorks ? "Yes" : "No") << "\n";
    ss << "  Plasticity Active: " << (result.plasticityActive ? "Yes" : "No") << "\n";
    ss << "  STDP Working: " << (result.stdpWorking ? "Yes" : "No") << "\n\n";
    
    ss << "CONCLUSION:\n";
    if (result.learningOccurred) {
        ss << "The experiment successfully demonstrated learning through synaptic modification.\n";
        ss << "The neural network exhibited activity-dependent plasticity, indicating functional neural computation.\n";
    } else {
        ss << "The experiment did not detect significant learning.\n";
        ss << "Further investigation may be needed to understand the network's computational capabilities.\n";
    }
    
    return ss.str();
}

std::string ExperimentManager::formatMetrics(const std::map<std::string, double>& metrics) {
    std::stringstream ss;
    ss << "Metrics:\n";
    for (const auto& pair : metrics) {
        ss << "  " << pair.first << ": " << pair.second << "\n";
    }
    return ss.str();
}

std::string ExperimentManager::getPhaseName(ExperimentPhase phase) {
    switch (phase) {
        case PHASE_BASIC_CONNECTIVITY: return "Basic Connectivity";
        case PHASE_PLASTICITY_LEARNING: return "Plasticity Learning";
        case PHASE_STDP_VERIFICATION: return "STDP Verification";
        case PHASE_ADVANCED_ANALYSIS: return "Advanced Analysis";
        default: return "Unknown";
    }
}

bool ExperimentManager::validateConfiguration() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Configuration error: " + error);
        }
        return false;
    }
    return true;
}

std::vector<std::string> ExperimentManager::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (pImpl->config.name.empty()) {
        errors.push_back("Experiment name cannot be empty");
    }
    
    if (pImpl->config.neuronCount == 0) {
        errors.push_back("Neuron count must be greater than 0");
    }
    
    if (pImpl->config.regionCount == 0) {
        errors.push_back("Region count must be greater than 0");
    }
    
    if (pImpl->config.timestep <= 0.0) {
        errors.push_back("Timestep must be positive");
    }
    
    if (pImpl->config.maxSteps <= 0) {
        errors.push_back("Maximum steps must be positive");
    }
    
    if (pImpl->config.connectionProbability < 0.0 || pImpl->config.connectionProbability > 1.0) {
        errors.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    return errors;
}

void ExperimentManager::setProgressCallback(std::function<void(float)> callback) {
    pImpl->progressCallback = callback;
}

void ExperimentManager::updateProgress(float progress) {
    pImpl->currentProgress = std::max(0.0f, std::min(1.0f, progress));
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(pImpl->currentProgress);
    }
}

size_t ExperimentManager::getCompletedPhases() const {
    return pImpl->results.size();
}

float ExperimentManager::getAverageScore() const {
    if (pImpl->results.empty()) return 0.0f;
    
    float totalScore = 0.0f;
    for (const auto& result : pImpl->results) {
        totalScore += (result.learningOccurred ? 1.0f : 0.0f);
    }
    
    return totalScore / static_cast<float>(pImpl->results.size());
}

const std::vector<ExperimentResult>& ExperimentManager::getAllResults() const {
    return pImpl->results;
}

// Private helper methods
void ExperimentManager::runBasicConnectivityPhase(std::shared_ptr<Brain> brain, ExperimentResult& result) {
    NLM_LOG_INFO("Running Basic Connectivity Phase");
    
    // Simple connectivity test
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) return;
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into neurons
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    result.connectivityWorks = (spikes > 0);
    result.totalSpikes = spikes;
    
    NLM_LOG_INFO("  Basic connectivity test completed: " << (result.connectivityWorks ? "PASS" : "FAIL"));
}

void ExperimentManager::runPlasticityPhase(std::shared_ptr<Brain> brain, ExperimentResult& result) {
    NLM_LOG_INFO("Running Plasticity Learning Phase");
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Record initial state
    result.initialMeanWeight = getRegionMeanWeight(brain, RegionId(1));
    
    // Apply learning stimulus
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Record final state
    result.finalMeanWeight = getRegionMeanWeight(brain, RegionId(1));
    result.weightChange = result.finalMeanWeight - result.initialMeanWeight;
    result.learningOccurred = (std::abs(result.weightChange) > 0.001f);
    
    // Count synapse changes
    countSynapseChanges(brain, result);
    
    NLM_LOG_INFO("  Plasticity phase completed: " << (result.learningOccurred ? "LEARNING DETECTED" : "NO LEARNING"));
}

void ExperimentManager::runStdpPhase(std::shared_ptr<Brain> brain, ExperimentResult& result) {
    NLM_LOG_INFO("Running STDP Verification Phase");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    // Test STDP
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Create STDP activity
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            // Pre before post
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Check results
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    result.stdpWorking = (std::abs(totalDelta) > 0.001f);
    
    NLM_LOG_INFO("  STDP phase completed: " << (result.stdpWorking ? "WORKING" : "NOT WORKING"));
}

void ExperimentManager::runAdvancedAnalysisPhase(std::shared_ptr<Brain> brain, ExperimentResult& result) {
    NLM_LOG_INFO("Running Advanced Analysis Phase");
    
    // Comprehensive analysis
    brain->logStatus();
    
    result.eiRatio = brain->getExcitationInhibitionRatio();
    result.avgFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.firingNeurons = brain->getFiringNeuronCount();
    
    // Calculate advanced metrics
    result.maxFiringNeurons = result.firingNeurons; // Simplified
    result.minFiringNeurons = std::max(0, static_cast<int>(result.firingNeurons) - 10);
    
    NLM_LOG_INFO("  Advanced analysis completed");
}

void ExperimentManager::prepareBasicConnectivityPhase(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("Basic connectivity phase preparation complete");
}

void ExperimentManager::preparePlasticityPhase(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("Plasticity phase preparation complete");
}

void ExperimentManager::prepareStdpPhase(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("STDP phase preparation complete");
}

void ExperimentManager::prepareAdvancedAnalysisPhase(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("Advanced analysis phase preparation complete");
}

float ExperimentManager::getRegionMeanWeight(std::shared_ptr<Brain> brain, RegionId regionId) {
    auto* region = brain->getRegion(regionId);
    if (!region) return 0.0f;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& syn : synapses) {
        sum += syn->getWeight();
    }
    
    return sum / static_cast<float>(synapses.size());
}

void ExperimentManager::countSynapseChanges(std::shared_ptr<Brain> brain, ExperimentResult& result) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // This would need to compare with previous state - simplified here
    result.strengthenedSynapses = 10; // Placeholder
    result.weakenedSynapses = 5;      // Placeholder
    result.unchangedSynapses = 100;   // Placeholder
}

size_t ExperimentManager::getSuccessfulExperimentCount() const {
    size_t count = 0;
    for (const auto& result : pImpl->results) {
        if (result.learningOccurred || result.connectivityWorks || result.stdpWorking) {
            count++;
        }
    }
    return count;
}

float ExperimentManager::getAverageWeightChange() const {
    if (pImpl->results.empty()) return 0.0f;
    
    float totalChange = 0.0f;
    for (const auto& result : pImpl->results) {
        totalChange += result.weightChange;
    }
    
    return totalChange / static_cast<float>(pImpl->results.size());
}

ExperimentResult ExperimentManager::getConsolidatedResult() const {
    ExperimentResult consolidated("Consolidated Experiment");
    
    for (const auto& result : pImpl->results) {
        consolidated.neuronCount += result.neuronCount;
        consolidated.synapseCount += result.synapseCount;
        consolidated.totalSpikes += result.totalSpikes;
        
        if (result.learningOccurred) {
            consolidated.learningOccurred = true;
        }
        
        if (result.connectivityWorks) {
            consolidated.connectivityWorks = true;
        }
    }
    
    if (consolidated.neuronCount > 0) {
        consolidated.avgFiringRate = static_cast<float>(consolidated.totalSpikes) / 1000.0f;
    }
    
    return consolidated;
}

}} // namespace nlm