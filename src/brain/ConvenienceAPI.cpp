#include "ConvenienceAPI.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <iomanip>

namespace nlm {

// Quick convenience functions for common NLM operations

// Create a default configured brain with sensible settings for quick testing
std::shared_ptr<Brain> createQuickBrain(int neuronCount, const std::string& configFile) {
    auto config = std::make_shared<Config>();
    
    // Set default values for quick experiments
    config->setValidatedInt("neuron_count", neuronCount, ConfigSource::Default, 100, 10000);
    config->setValidatedInt("region_count", 1, ConfigSource::Default, 1, 10);
    config->setValidatedDouble("connection_probability", 0.1f, ConfigSource::Default, 0.0, 1.0);
    config->setValidatedInt("random_seed", 42, ConfigSource::Default, 0, std::numeric_limits<int64_t>::max());
    config->setValidatedDouble("simulation_timestep", 0.001f, ConfigSource::Default, 0.0, 0.1);
    
    config->setValidatedDouble("stdp_ltp_weight", 0.01f, ConfigSource::Default, 0.0, 1.0);
    config->setValidatedDouble("stdp_ltd_weight", 0.012f, ConfigSource::Default, 0.0, 1.0);
    config->setValidatedDouble("stdp_tau", 20.0f, ConfigSource::Default, 0.0, 100.0);
    
    config->setValidatedDouble("synaptogenesis_rate", 0.0001f, ConfigSource::Default, 0.0, 0.001);
    config->setValidatedDouble("pruning_rate", 0.00001f, ConfigSource::Default, 0.0, 0.001);
    
    // Load from file if provided
    if (!configFile.empty()) {
        config->loadFromFile(configFile);
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        return brain;
    }
    
    return nullptr;
}

// Run a basic connectivity test on a brain
bool testBrainConnectivity(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    std::cout << "=== Basic Connectivity Test ===" << std::endl;
    
    // Inject current into sensory neurons
    for (int i = 0; i < 10 && i < brain->getTotalNeuronCount(); ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
    }
    
    // Run a few steps and check for spikes
    int spikeCount = brain->getTotalSpikeCount();
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    int finalSpikeCount = brain->getTotalSpikeCount();
    int newSpikes = finalSpikeCount - spikeCount;
    
    std::cout << "Spikes generated: " << newSpikes << std::endl;
    
    if (newSpikes > 0) {
        std::cout << "✓ Brain connectivity test passed" << std::endl;
        return true;
    } else {
        std::cout << "! Brain connectivity test showed no activity" << std::endl;
        return false;
    }
}

// Run a basic plasticity experiment
float testBrainPlasticity(std::shared_ptr<Brain> brain, int steps) {
    if (!brain) return -1.0f;
    
    std::cout << "=== Plasticity Test ===" << std::endl;
    
    // Record initial state
    float initialExcitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    size_t initialSynapseCount = brain->getTotalSynapseCount();
    
    // Enable plasticity
    auto* spikeSystem = brain->getSpikeSystem();
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    
    if (spikeSystem && stdp && hebbian) {
        // Enable plasticity on all synapses
        for (auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Inject varied input to drive plasticity
        for (int step = 0; step < steps; ++step) {
            // Create input pattern
            for (int i = 0; i < 20 && i < brain->getTotalNeuronCount(); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f * (0.5f + 0.5f * sin(static_cast<float>(step) / 100.0f)));
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress
            if (step % 200 == 0) {
                std::cout << "Step " << step << ": Firing rate = " 
                         << brain->getAverageFiringRate() << ", "
                         << "EI ratio = " << brain->getExcitationInhibitionRatio() << std::endl;
            }
        }
        
        // Record final state
        float finalExcitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        
        // Calculate change
        float ratioChange = std::abs(finalExcitationInhibitionRatio - initialExcitationInhibitionRatio);
        float synapseChange = static_cast<float>(finalSynapseCount - initialSynapseCount);
        
        std::cout << "Plasticity test completed:" << std::endl;
        std::cout << "  Initial EI ratio: " << initialExcitationInhibitionRatio << std::endl;
        std::cout << "  Final EI ratio: " << finalExcitationInhibitionRatio << std::endl;
        std::cout << "  Ratio change: " << ratioChange << std::endl;
        std::cout << "  Synapse count change: " << synapseChange << std::endl;
        
        if (ratioChange > 0.1f || synapseChange > 10) {
            std::cout << "✓ Plasticity test showed significant change" << std::endl;
            return ratioChange + std::abs(synapseChange * 0.1f);
        } else {
            std::cout << "! Plasticity test showed minimal change" << std::endl;
            return 0.0f;
        }
    }
    
    std::cout << "! Could not run plasticity test - missing systems" << std::endl;
    return -1.0f;
}

// Run a simple STDP verification test
bool testSTDP(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    std::cout << "=== STDP Verification Test ===" << std::endl;
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "! No regions found" << std::endl;
        return false;
    }
    
    // Get synapses for testing
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "! Not enough synapses for STDP test" << std::endl;
        return false;
    }
    
    // Enable only STDP plasticity
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        initialWeights.push_back(synapses[i]->getWeight());
        std::cout << "Synapse " << i << " initial weight: " << initialWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        std::cout << "Creating correlated pre->post activity for LTP..." << std::endl;
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Check weight changes
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - initialWeights[i];
            totalDelta += delta;
            std::cout << "Synapse " << i << " final weight: " << synapses[i]->getWeight() 
                     << " (Δ=" << delta << ")" << std::endl;
        }
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            std::cout << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
            return true;
        } else if (totalDelta < -0.001f) {
            std::cout << "! STDP reversed: Check parameters" << std::endl;
            return false;
        } else {
            std::cout << "! No change: STDP may not be triggering" << std::endl;
            return false;
        }
    }
    
    return false;
}

// Run a simple Hebbian verification test
bool testHebbian(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    std::cout << "=== Hebbian Verification Test ===" << std::endl;
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "! No regions found" << std::endl;
        return false;
    }
    
    // Enable only Hebbian plasticity
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, false, false);
    }
    
    // Get some neurons with concurrent activity
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 4) {
        std::cout << "! Not enough neurons for Hebbian test" << std::endl;
        return false;
    }
    
    // Record initial weights between neuron pairs
    std::vector<std::pair<float, float>> initialPairs;
    for (size_t i = 0; i < 3 && i < neurons.size() - 1; ++i) {
        auto* syn = region->getSynapseFromTo(neurons[i]->getId(), neurons[i + 1]->getId());
        if (syn) {
            initialPairs.push_back(std::make_pair(syn->getWeight(), i));
        }
    }
    
    // Create simultaneous activity (both pre and post fire together)
    std::cout << "Creating simultaneous pre->post activity for Hebbian learning..." << std::endl;
    
    for (int trial = 0; trial < 30; ++trial) {
        // Fire both pre and post neurons simultaneously
        if (initialPairs.size() > 0) {
            neurons[initialPairs[0].second]->injectCurrent(40.0f);
            neurons[initialPairs[0].second + 1]->injectCurrent(40.0f);
        }
        
        brain->step(trial, trial * 0.001);
    }
    
    // Check weight changes
    std::cout << "Weight changes after concurrent activity:" << std::endl;
    bool strengthened = false;
    
    for (size_t i = 0; i < initialPairs.size(); ++i) {
        auto* syn = region->getSynapseFromTo(neurons[initialPairs[i].second]->getId(), 
                                            neurons[initialPairs[i].second + 1]->getId());
        if (syn) {
            float delta = syn->getWeight() - initialPairs[i].first;
            std::cout << "  Synapse " << initialPairs[i].second << ": weight changed from " 
                     << initialPairs[i].first << " to " << syn->getWeight() 
                     << " (Δ=" << delta << ")" << std::endl;
            
            if (delta > 0.01f) strengthened = true;
        }
    }
    
    if (strengthened) {
        std::cout << "✓ Hebbian learning working: Concurrent activity strengthened connections" << std::endl;
        return true;
    } else {
        std::cout << "! Hebbian learning may not be triggering with current parameters" << std::endl;
        return false;
    }
}

// Print brain status in a readable format
void printBrainStatus(std::shared_ptr<Brain> brain) {
    if (!brain) {
        std::cout << "Brain is null!" << std::endl;
        return;
    }
    
    std::cout << "=== NLM Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << "\nQuick Statistics:" << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
}

// Save brain state with timestamp
std::string saveBrainWithTimestamp(std::shared_ptr<Brain> brain, const std::string& basePath) {
    if (!brain) {
        std::cout << "Cannot save null brain!" << std::endl;
        return std::string("");
    }
    
    // Create timestamped filename
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", t);
    
    std::string filename = std::string(buffer) + "_brain.bin";
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(basePath);
    
    std::string fullPath = basePath + "/" + filename;
    
    if (brain->save(fullPath)) {
        std::cout << "Brain saved successfully to: " << fullPath << std::endl;
        return fullPath;
    } else {
        std::cout << "Failed to save brain to: " << fullPath << std::endl;
        return std::string("");
    }
}

} // namespace nlm
