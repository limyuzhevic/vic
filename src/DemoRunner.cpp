// Demo Runner for NLM
// Implements different demo modes and experiments

#include "DemoRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <memory>

using namespace nlm;

void DemoRunner::runPhase2Demo() {
    std::cout << "=== NLM Phase 2: Real Neural Computation Demo ===" << std::endl;
    std::cout << "Testing: LIF neurons, event-driven spikes, STDP, Hebbian plasticity" << std::endl << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    brain->logStatus();
    std::cout << std::endl;
    
    // Test 1: Basic connectivity
    std::cout << "Test 1: Basic Neural Connectivity" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Spikes generated: " << spikes << std::endl;
        if (spikes > 0) {
            std::cout << "  ✓ Basic connectivity working" << std::endl;
        } else {
            std::cout << "  ⚠ No spikes detected (check neuron parameters)" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Test 2: Plasticity
    std::cout << "Test 2: Plasticity Experiment" << std::endl;
    brain->reset();
    brain->initialize();
    
    auto* region2 = brain->getRegion(RegionId(1));
    if (region2) {
        for (auto& syn : region2->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (region2) {
        for (const auto& syn : region2->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Apply learning
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Record final weights
    std::vector<float> finalWeights;
    if (region2) {
        for (const auto& syn : region2->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Check for learning
    bool learningOccurred = false;
    if (!initialWeights.empty() && !finalWeights.empty()) {
        float initialMean = 0.0f, finalMean = 0.0f;
        for (float w : initialWeights) initialMean += w;
        for (float w : finalWeights) finalMean += w;
        initialMean /= initialWeights.size();
        finalMean /= finalWeights.size();
        
        if (std::abs(finalMean - initialMean) > 0.001f) {
            learningOccurred = true;
        }
    }
    
    if (learningOccurred) {
        std::cout << "  ✓ Learning detected: synaptic weights changed" << std::endl;
    } else {
        std::cout << "  ⚠ No significant weight change detected" << std::endl;
    }
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Active neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << std::endl;
    
    // Test 3: STDP
    std::cout << "Test 3: STDP Verification" << std::endl;
    brain->reset();
    brain->initialize();
    
    auto* region3 = brain->getRegion(RegionId(1));
    if (region3 && region3->getSynapses().size() >= 5) {
        auto& synapses = region3->getSynapses();
        std::vector<float> beforeWeights;
        
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Only STDP
        }
        
        // Create correlated pre->post activity
        for (int trial = 0; trial < 50; ++trial) {
            auto neurons = region3->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre fires first
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check for potentiation (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP working: LTP detected" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ⚠ STDP reversed: LTD detected" << std::endl;
        } else {
            std::cout << "  ⚠ No change: STDP may not be triggering" << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::cout << "=== Phase 2 Demo Complete ===" << std::endl;
}

void DemoRunner::runPhase4Demo() {
    std::cout << "=== NLM Phase 4: Advanced Cognition Demo ===" << std::endl;
    std::cout << "Testing: Advanced memory systems, planning, concept formation" << std::endl << std::endl;
    
    // Phase 4 demo would include advanced features
    std::cout << "Phase 4 features to be implemented:" << std::endl;
    std::cout << "  - Advanced working memory with competitive dynamics" << std::endl;
    std::cout << "  - Neural episodic memory with replay" << std::endl;
    std::cout << "  - Concept formation from patterns" << std::endl;
    std::cout << "  - Neural planning and action selection" << std::endl;
    std::cout << "  - Social learning and imitation" << std::endl;
    std::cout << "  - Continual learning without catastrophic forgetting" << std::endl;
    std::cout << std::endl;
    
    std::cout << "To be implemented in future development phases." << std::endl;
}

void DemoRunner::runPhase6Demo() {
    std::cout << "=== NLM Phase 6: Complete Integration Demo ===" << std::endl;
    std::cout << "Testing all integrated brain systems" << std::endl << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full integration experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
}

void DemoRunner::runBenchmarks() {
    std::cout << "=== NLM Performance Benchmarks ===" << std::endl;
    std::cout << "Measuring simulation performance for different network sizes" << std::endl << std::endl;
    
    // Include the benchmark.cpp logic or call it
    // For now, we'll show the structure
    std::cout << "Benchmark configurations to test:" << std::endl;
    std::cout << "  - 100 neurons, 1000 steps" << std::endl;
    std::cout << "  - 500 neurons, 1000 steps" << std::endl;
    std::cout << "  - 1000 neurons, 500 steps" << std::endl;
    std::cout << "  - 2000 neurons, 200 steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance metrics to collect:" << std::endl;
    std::cout << "  - Steps per second (throughput)" << std::endl;
    std::cout << "  - Spikes per step (activity)" << std::endl;
    std::cout << "  - Synapses per neuron (connectivity)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "To be implemented with actual benchmarking code." << std::endl;
}

void DemoRunner::runIntegrationTests() {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << "Running comprehensive integration verification" << std::endl << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (integrationOK) {
        std::cout << "✓ All integration tests passed!" << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate." << std::endl;
    } else {
        std::cerr << "✗ Integration tests failed!" << std::endl;
        std::cerr << "Check the logs for detailed error information." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Integration Tests Complete ===" << std::endl;
}

void DemoRunner::runCustomExperiment() {
    std::cout << "=== NLM Custom Experiment ===" << std::endl;
    std::cout << "Running user-specified configuration" << std::endl << std::endl;
    
    // This would load a custom configuration and run the experiment
    std::cout << "To be implemented with custom configuration loading." << std::endl;
    std::cout << "Would load configuration from: configs/default.cfg" << std::endl;
    std::cout << "Would run with custom parameters based on user input." << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Custom Experiment Complete ===" << std::endl;
}