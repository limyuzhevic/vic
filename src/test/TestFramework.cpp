// Test Framework Implementation
#include "TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

std::shared_ptr<Brain> TestFramework::createTestBrain(const BrainConfig& config) {
    auto configPtr = std::make_shared<Config>();
    
    configPtr->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::Default);
    configPtr->set("simulation_timestep", config.timestep, ConfigSource::Default);
    configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    configPtr->set("stdp_ltp_weight", config.stdpLtpWeight, ConfigSource::Default);
    configPtr->set("stdp_ltd_weight", config.stdpLtdWeight, ConfigSource::Default);
    configPtr->set("stdp_tau", config.stdpTau, ConfigSource::Default);
    configPtr->set("synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Default);
    configPtr->set("pruning_rate", config.pruningRate, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(configPtr);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize test brain");
        return nullptr;
    }
    
    return brain;
}

void TestFramework::runPhase2Tests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== NLM Phase 2 Test Suite ===");
    
    // Test 1: Basic connectivity
    {
        NLM_LOG_INFO("Test 1: Basic neural connectivity");
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain regions found");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Connectivity test passed - " + std::to_string(spikes) + " spikes generated");
        } else {
            NLM_LOG_WARNING("Warning: No spikes generated in connectivity test");
        }
    }
    
    // Reset for next test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning
    {
        NLM_LOG_INFO("Test 2: Plasticity learning experiment");
        
        std::vector<float> initialWeights;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Enable plasticity
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning stimulus
        for (SimulationStep step = 0; step < 500; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        std::vector<float> finalWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Check for learning
        bool learningOccurred = false;
        if (!initialWeights.empty() && !finalWeights.empty()) {
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                if (std::abs(finalWeights[i] - initialWeights[i]) > 0.01f) {
                    learningOccurred = true;
                    break;
                }
            }
        }
        
        if (learningOccurred) {
            NLM_LOG_INFO("✓ Plasticity test passed - learning detected");
        } else {
            NLM_LOG_WARNING("Warning: No significant weight changes in plasticity test");
        }
    }
}

void TestFramework::runPlasticityTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Running plasticity tests...");
    // Implementation for plasticity testing
}

void TestFramework::runSTDPTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Running STDP tests...");
    // Implementation for STDP testing
}

void TestFramework::runIntegrationTests() {
    NLM_LOG_INFO("Running integration tests...");
    // Implementation for integration testing
}

} // namespace nlm