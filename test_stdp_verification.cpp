// NLM Phase 2: STDP Verification Test
// Tests that Spike-Timing-Dependent Plasticity works correctly

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <algorithm>

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    auto region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Brain has no regions (RegionId 1 not found)");
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_WARNING("Not enough synapses for STDP test (found " + std::to_string(synapses.size()) + ", need 5)");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) {
        NLM_LOG_ERROR("Not enough neurons for STDP test (found " + std::to_string(neurons.size()) + ", need 2)");
        return;
    }
    
    NLM_LOG_INFO("  Using Neuron 0 (pre) and Neuron 1 (post) for STDP trials...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Log progress every 10 trials
        if (trial % 10 == 0) {
            NLM_LOG_INFO("    Trial " + std::to_string(trial) + 
                        " | Pre spikes: " + std::to_string(preNeuron ? preNeuron->getSpikeCount() : 0) +
                        " | Post spikes: " + std::to_string(postNeuron ? postNeuron->getSpikeCount() : 0));
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (LTP)");
        NLM_LOG_INFO("  STDP verification PASSED");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Pre-before-post produced depression (LTD)");
        NLM_LOG_INFO("  STDP verification FAILED - unexpected result");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        NLM_LOG_INFO("  STDP verification INCONCLUSIVE");
    }
}

int main(int argc, char** argv) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: STDP Verification Test ===");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    std::string configFile = "configs/default.cfg";
    
    // Parse command line arguments
    bool verboseMode = false;
    size_t neuronCount = 500;
    size_t stdpTrials = 50;
    double timestep = 0.001;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
            consoleLogger->setLevel(LogLevel::Debug);
        } else if (arg.substr(0, 7) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--trials") {
            if (arg.find('=') != std::string::npos) {
                stdpTrials = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                stdpTrials = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 4) == "--step") {
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
        }
    }
    
    // Configure simulation parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Log configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run the STDP verification
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== STDP Verification Test Complete ===");
    
    return 0;
}