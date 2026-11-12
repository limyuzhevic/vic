#include "examples/ExperimentRunner.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace examples {

ExperimentResult ExperimentRunner::runPhase2Demo(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "phase2_demo";
    result.timesteps = timesteps;
    
    // Record initial state
    result.meanInitialWeight = 0.0f;
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto synapses = region->getSynapses();
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = synapses.size();
        
        float sum = 0.0f;
        for (const auto& syn : synapses) {
            sum += syn->getWeight();
        }
        result.meanInitialWeight = synapses.empty() ? 0.0f : sum / synapses.size();
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < timesteps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Record final state
    result.meanFinalWeight = 0.0f;
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto synapses = region->getSynapses();
        float sum = 0.0f;
        for (const auto& syn : synapses) {
            sum += syn->getWeight();
        }
        result.meanFinalWeight = synapses.empty() ? 0.0f : sum / synapses.size();
    }
    
    result.totalSpikes = brain->getTotalSpikeCount();
    result.mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    
    // Calculate learning
    result.calculateLearning();
    
    return result;
}

ExperimentResult ExperimentRunner::runPhase3Demo(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "phase3_demo";
    result.timesteps = timesteps;
    
    // Similar structure to phase2 but with different parameters
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runPhase4Demo(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "phase4_demo";
    result.timesteps = timesteps;
    
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runPhase5Demo(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "phase5_demo";
    result.timesteps = timesteps;
    
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runPhase6Demo(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "phase6_demo";
    result.timesteps = timesteps;
    
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "learning_experiment";
    result.timesteps = timesteps;
    
    // Similar to phase2 but focused on learning
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runConnectivityTest(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "connectivity_test";
    result.timesteps = timesteps;
    
    return runPhase2Demo(brain, timesteps);
}

ExperimentResult ExperimentRunner::runStdpVerification(std::shared_ptr<Brain> brain, size_t trials) {
    ExperimentResult result;
    result.experimentName = "stdp_verification";
    result.timesteps = trials;
    
    // Simplified STDP test
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        if (!synapses.empty()) {
            Synapse* syn = synapses[0].get();
            syn->enablePlasticity(false, true, false);
            
            float beforeWeight = syn->getWeight();
            
            for (size_t trial = 0; trial < trials; ++trial) {
                brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float afterWeight = syn->getWeight();
            result.weightChange = afterWeight - beforeWeight;
            result.learningOccurred = (std::abs(result.weightChange) > 0.001f);
        }
    }
    
    result.totalSpikes = brain->getTotalSpikeCount();
    result.calculateLearning();
    return result;
}

ExperimentResult ExperimentRunner::runPlasticityStudy(std::shared_ptr<Brain> brain, size_t timesteps) {
    ExperimentResult result;
    result.experimentName = "plasticity_study";
    result.timesteps = timesteps;
    
    return runPhase2Demo(brain, timesteps);
}

void ExperimentRunner::printExperimentResults(const ExperimentResult& result) {
    std::cout << "=== " << result.experimentName << " Results ===" << std::endl;
    std::cout << "Timesteps: " << result.timesteps << std::endl;
    std::cout << "Total spikes: " << result.totalSpikes << std::endl;
    std::cout << "Neurons: " << result.neuronCount << std::endl;
    std::cout << "Synapses: " << result.synapseCount << std::endl;
    std::cout << "Initial mean weight: " << std::fixed << std::setprecision(4) << result.meanInitialWeight << std::endl;
    std::cout << "Final mean weight: " << std::fixed << std::setprecision(4) << result.meanFinalWeight << std::endl;
    std::cout << "Weight change: " << std::fixed << std::setprecision(6) << result.weightChange << std::endl;
    std::cout << "Learning occurred: " << (result.learningOccurred ? "✓" : "✗") << std::endl;
    std::cout << "Most active neurons: ";
    for (size_t i = 0; i < result.mostActiveNeurons.size() && i < 5; ++i) {
        std::cout << result.mostActiveNeurons[i].index() << (i < result.mostActiveNeurons.size() - 1 ? ", " : "");
    }
    std::cout << std::endl;
}

std::string ExperimentRunner::exportResultsToJson(const ExperimentResult& result) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"experiment_name\": \"" << result.experimentName << "\",\n";
    oss << "  \"timesteps\": " << result.timesteps << ",\n";
    oss << "  \"total_spikes\": " << result.totalSpikes << ",\n";
    oss << "  \"neuron_count\": " << result.neuronCount << ",\n";
    oss << "  \"synapse_count\": " << result.synapseCount << ",\n";
    oss << "  \"mean_initial_weight\": " << std::fixed << std::setprecision(6) << result.meanInitialWeight << ",\n";
    oss << "  \"mean_final_weight\": " << std::fixed << std::setprecision(6) << result.meanFinalWeight << ",\n";
    oss << "  \"weight_change\": " << std::fixed << std::setprecision(6) << result.weightChange << ",\n";
    oss << "  \"learning_occurred\": " << (result.learningOccurred ? "true" : "false") << ",\n";
    oss << "  \"most_active_neurons\": [";
    for (size_t i = 0; i < result.mostActiveNeurons.size(); ++i) {
        oss << result.mostActiveNeurons[i].index();
        if (i < result.mostActiveNeurons.size() - 1) oss << ", ";
    }
    oss << "]\n";
    oss << "}";
    return oss.str();
}

std::vector<float> ExperimentRunner::getInitialWeights(std::shared_ptr<Brain> brain) {
    std::vector<float> weights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            weights.push_back(syn->getWeight());
        }
    }
    return weights;
}

std::vector<float> ExperimentRunner::getFinalWeights(std::shared_ptr<Brain> brain) {
    return getInitialWeights(brain);
}

std::pair<float, float> ExperimentRunner::calculateWeightStats(const std::vector<float>& weights) {
    if (weights.empty()) {
        return {0.0f, 0.0f};
    }
    
    float sum = 0.0f;
    for (float w : weights) {
        sum += w;
    }
    
    float mean = sum / weights.size();
    float variance = 0.0f;
    for (float w : weights) {
        variance += (w - mean) * (w - mean);
    }
    variance /= weights.size();
    
    return {mean, std::sqrt(variance)};
}

}} // namespace nlm::examples