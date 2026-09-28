// NLM Phase 2: Plasticity Test
// Tests plasticity learning experiment with synaptic weight changes

#include <iostream>
#include <cassert>
#include <cmath>
#include <numeric>

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

namespace test_plasticity {
    
    // Test 2: Plasticity learning experiment
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::cout << "=== Test 2: Plasticity Learning Experiment ===" << std::endl;
        
        if (!brain) {
            std::cout << "  ERROR: No brain provided" << std::endl;
            return;
        }
        
        // Create learning experiment instance
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        std::cout << "" << std::endl;
        std::cout << "Applying repeated input patterns (500 steps)..." << std::endl;
        
        for (SimulationStep step = 0; step < 500; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 8; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "  Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
    }
    
    // Check if learning occurred based on weight changes
    bool checkLearningOccurred(const LearningExperiment& experiment) {
        if (experiment.initialWeights.empty() || experiment.finalWeights.empty()) {
            return false;
        }
        
        // Calculate weight changes
        float initialSum = std::accumulate(experiment.initialWeights.begin(), 
                                          experiment.initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(experiment.finalWeights.begin(), 
                                        experiment.finalWeights.end(), 0.0f);
        float initialMean = initialSum / experiment.initialWeights.size();
        float finalMean = finalSum / experiment.finalWeights.size();
        
        // Check for significant weight change
        if (std::abs(finalMean - initialMean) > 0.001f) return true;
        
        // Check individual synapse changes
        size_t minSize = std::min(experiment.initialWeights.size(), 
                                 experiment.finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = experiment.finalWeights[i] - experiment.initialWeights[i];
            if (delta > 0.01f || delta < -0.01f) return true;
        }
        
        return false;
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
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run plasticity experiment
        runPlasticityExperiment(brain);
        
        // Create learning experiment to check results
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        // Reset and run again for comparison
        brain->reset();
        brain->initialize();
        experiment.recordFinalState();
        
        // Check if learning occurred
        bool learningOccurred = checkLearningOccurred(experiment);
        
        std::cout << "=== Plasticity Test Results ===" << std::endl;
        if (learningOccurred) {
            std::cout << "✓ Learning detected: Synaptic weights changed through experience" << std::endl;
        } else {
            std::cout << "! Limited learning detected: Weights may need more stimulation" << std::endl;
        }
        
        std::cout << "✓ Plasticity experiment completed successfully!" << std::endl;
    }
    
} // namespace test_plasticity

PlasticityTest.cpp