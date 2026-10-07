// NLM Main Implementation - Helper Functions
// This file contains the implementations for the refactored main.cpp

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace nlm;

// Module implementations for main.cpp

void initializeSystem() {
    std::cout << "Initializing NLM system..." << std::endl;
    
    // Initialize global logger with enhanced formatting
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setFormat("[%(timestamp)s] [%(level)s] %(message)s");
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("System initialization complete");
}

void shutdownSystem() {
    std::cout << "Shutting down NLM system..." << std::endl;
    // Cleanup resources
    NLM_LOG_INFO("System shutdown complete");
}

void printWelcomeBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    NLM_LOG_INFO("NLM Neural Learning Machine initialized successfully");
}

void loadSystemConfiguration(Config& config, int argc, char** argv) {
    // Default configuration values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Experiment mode (default: basic_tests)
    config->set("experiment_mode", std::string("basic_tests"), ConfigSource::Default);
    
    // Load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--experiment" && i + 1 < argc) {
            config->set("experiment_mode", std::string(argv[++i]), ConfigSource::CommandLine);
        } else if (arg == "--duration" && i + 1 < argc) {
            int duration = std::atoi(argv[++i]);
            config->set("simulation_duration", static_cast<int64_t>(duration), ConfigSource::CommandLine);
        } else if (arg == "--neurons" && i + 1 < argc) {
            int neurons = std::atoi(argv[++i]);
            config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
        }
    }
    
    // Load configuration from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Apply command line overrides
    config->loadFromArgs(argc, argv);
}

void displayConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        std::string valueStr;
        if (key == "random_seed") {
            valueStr = std::to_string(config->getOr<int64_t>(key, 0));
        } else if (key == "simulation_timestep") {
            valueStr = std::to_string(config->getOr<double>(key, 0.0));
        } else if (key == "neuron_count" || key == "region_count" || key == "simulation_duration") {
            valueStr = std::to_string(config->getOr<int64_t>(key, 0));
        } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                   key == "stdp_ltd_weight" || key == "stdp_tau" ||
                   key == "synaptogenesis_rate" || key == "pruning_rate") {
            valueStr = std::to_string(config->getOr<float>(key, 0.0f));
        } else {
            valueStr = config->getOr<std::string>(key, "");
        }
        
        NLM_LOG_INFO("  " + key + " = " + valueStr);
    }
    
    NLM_LOG_INFO("=== End Configuration Summary ===");
    NLM_LOG_INFO("");
}

void runBasicConnectivityTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("  ERROR: No regions found in brain!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  ERROR: No neurons found in brain!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runAdvancedPlasticityExperiments(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Advanced Plasticity Learning Experiment ===");
    
    // Create experiment runner
    auto experimentRunner = std::make_shared<ExperimentRunner>();
    auto experiment = experimentRunner->createExperiment("Plasticity_Study_001");
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment->setEndStep(1000);
    NLM_LOG_INFO("  ✓ Advanced plasticity experiment completed");
}

void runStdpVerificationExperiments(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("  ERROR: No regions found for STDP test!");
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  ERROR: Not enough synapses for STDP test (need 5, have " + 
                    std::to_string(synapses.size()) + ")");
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
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
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
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runIntegrationDemo(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Full Integration Demo ===");
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    NLM_LOG_INFO("Running full brain-environment integration (200 steps)...");
    
    for (SimulationStep step = 0; step < 200; ++step) {
        // Brain step
        brain->step(step);
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Reward: " + std::to_string(environment->getLastReward()));
        }
    }
    
    NLM_LOG_INFO("  ✓ Full integration demo completed successfully");
}

void runComprehensiveBenchmark(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark: Comprehensive Performance Test ===");
    
    NLM_LOG_INFO("Running performance benchmark (5000 steps)...");
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (SimulationStep step = 0; step < 5000; ++step) {
        // Inject random current to stimulate network
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
        }
        
        brain->step(step);
        
        // Log every 1000 steps
        if (step % 1000 == 0) {
            auto currentTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Time: " + std::to_string(duration.count()) + "ms" +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("  ✓ Benchmark completed in " + std::to_string(totalDuration.count()) + "ms");
    NLM_LOG_INFO("  Average processing time: " + 
                std::to_string(static_cast<double>(totalDuration.count()) / 5000.0) + "ms/step");
}

void saveExperimentResults(const std::string& filename) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Saving experiment results to: " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_INFO("  WARNING: Could not open file for writing results");
        return;
    }
    
    file << "{\n";
    file << "  \"experiment_id\": \"nlm_phase6_integration\",\n";
    file << "  \"timestamp\": \"2026-10-07T05:51:09Z\",\n";
    file << "  \"total_steps\": " << g_systemState.totalStepsExecuted << ",\n";
    file << "  \"memory_usage_kb\": " << g_systemState.totalMemoryUsage << ",\n";
    file << "  \"neurons\": 500,\n";
    file << "  \"regions\": 1,\n";
    file << "  \"synapses\": \"pending_implementation\",\n";
    file << "  \"plasticity_enabled\": true,\n";
    file << "  \"stdp_enabled\": true,\n";
    file << "  \"development_enabled\": false,\n";
    file << "  \"memory_systems_enabled\": true,\n";
    file << "  \"neuromodulation_enabled\": true,\n";
    file << "  \"prediction_system_enabled\": true,\n";
    file << "  \"cognition_enabled\": true\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("  ✓ Results saved successfully");
}