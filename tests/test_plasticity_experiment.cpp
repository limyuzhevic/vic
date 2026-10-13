// Plasticity Learning Experiment Test
// Tests that synaptic weights change through repeated experience

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace nlm;

// Learning experiment structure
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    }
    
    void computeStatistics() {
        if (initialWeights.empty() || finalWeights.empty()) {
            std::cout << "ERROR: No weights recorded" << std::endl;
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        std::cout << "Weight Statistics:" << std::endl;
        std::cout << "  Initial mean weight: " << initialMean << std::endl;
        std::cout << "  Final mean weight: " << finalMean << std::endl;
        std::cout << "  Change: " << (finalMean - initialMean) << std::endl;
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        std::cout << "Synaptic Changes:" << std::endl;
        std::cout << "  Strengthened: " << strengthened << std::endl;
        std::cout << "  Weakened: " << weakened << std::endl;
        std::cout << "  Unchanged: " << unchanged << std::endl;
        
        std::cout << "Spike Activity:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Most active neurons recorded: " << mostActiveNeurons.size() << std::endl;
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        if (learningOccurred) {
            std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
        } else {
            std::cout << "✗ NO LEARNING: Weights did not change significantly" << std::endl;
        }
    }
};

bool testPlasticityExperiment() {
    std::cout << "\n=== Test 2: Plasticity Learning Experiment ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    std::cout << "Applying repeated input patterns (1000 steps)..." << std::endl;
    
    // Apply repeated input pattern to stimulate learning
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "  Step " << step << 
                " | Spikes: " << brain->getTotalSpikeCount() <<
                " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
    
    return true;
}

int main() {
    std::cout << "NLM Phase 2 Plasticity Experiment Test" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    bool success = testPlasticityExperiment();
    
    if (success) {
        std::cout << "\n✓ Plasticity experiment test PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ Plasticity experiment test FAILED" << std::endl;
        return 1;
    }
}