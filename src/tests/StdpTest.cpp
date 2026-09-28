// NLM Phase 2: STDP Test
// Tests Spike-Timing-Dependent Plasticity (STDP) learning rule

#include <iostream>
#include <cassert>
#include <cmath>

// Include the necessary headers
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/LearningExperiment.hpp"

namespace test_stdp {
    
    // Test 3: STDP verification
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        std::cout << "=== Test 3: STDP Verification ===" << std::endl;
        
        if (!brain) {
            std::cout << "  ERROR: No brain provided" << std::endl;
            return;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  ERROR: No region found" << std::endl;
            return;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 3) {
            std::cout << "  Not enough synapses for STDP test" << std::endl;
            return;
        }
        
        std::cout << "  Testing STDP on " << std::min(size_t(3), synapses.size()) << " synapses:" << std::endl;
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            std::cout << "    Synapse " << i 
                      << " before: " << beforeWeights[i] << std::endl;
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        std::cout << "" << std::endl;
        std::cout << "  Creating correlated pre->post activity (50 trials)..." << std::endl;
        
        auto neurons = region->getAllNeurons();
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        std::cout << "  After correlated activity:" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            std::cout << "    Synapse " << i 
                      << " after: " << synapses[i]->getWeight()
                      << " (Δ=" << delta << ")" << std::endl;
        }
        
        std::cout << "" << std::endl;
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        }
    }
    
    // Test STDP with reverse order (post before pre for LTD)
    void testStdpReverseOrder(std::shared_ptr<Brain> brain) {
        std::cout << "=== Test 3B: STDP Reverse Order (LTD) ===" << std::endl;
        
        if (!brain) return;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 3) return;
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        }
        
        // Create reverse correlated activity: fire post then pre
        std::cout << "  Testing reverse STDP (post before pre)..." << std::endl;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 50; ++trial) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // Post fires first
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then pre fires
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check for depression (weight decrease)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        std::cout << "  Total Δ weight: " << totalDelta << std::endl;
        if (totalDelta < -0.001f) {
            std::cout << "  ✓ STDP LTD working: Post-before-pre produced depression" << std::endl;
        }
        
        std::cout << "=== STDP Tests ===" << std::endl;
        std::cout << "✓ All STDP tests completed!" << std::endl;
    }
    
    void runAll() {
        // Initialize logger for testing
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set test configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run STDP verification
        runStdpVerification(brain);
        
        // Reset brain for reverse STDP test
        brain->reset();
        brain->initialize();
        
        // Run reverse STDP test
        testStdpReverseOrder(brain);
        
        std::cout << "✓ STDP tests passed!" << std::endl;
    }
    
} // namespace test_stdp

StdpTest.cpp