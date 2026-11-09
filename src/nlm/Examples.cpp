// Implementation of NLM Examples and Educational Materials

#include "Examples.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Example implementations
void example_basic_brain_setup() {
    NLM_LOG_INFO("=== Example 1: Basic Brain Setup ===");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("connection_probability", 0.1);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    NLM_LOG_INFO("Brain initialized successfully");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    
    NLM_LOG_INFO("Example 1 completed successfully");
}

void example_simulation_loop() {
    NLM_LOG_INFO("=== Example 2: Simulation Loop ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Run simulation loop
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress
        if (step % 20 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        ", Spikes: " + std::to_string(brain->getFiringNeuronCount()) +
                        ", Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    NLM_LOG_INFO("Simulation completed: " + std::to_string(brain->getTotalSpikeCount()) + " total spikes");
    NLM_LOG_INFO("Example 2 completed successfully");
}

void example_memory_integration() {
    NLM_LOG_INFO("=== Example 3: Memory Integration ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Run some simulation to generate activity
    for (uint64_t step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject some activity
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
    }
    
    // Check memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    NLM_LOG_INFO("Working memory active traces: " + 
                std::to_string(workingMemory ? workingMemory->getActiveTraces() : 0));
    NLM_LOG_INFO("Episodic memory episodes: " + 
                std::to_string(episodicMemory ? episodicMemory->getEpisodeCount() : 0));
    
    NLM_LOG_INFO("Example 3 completed successfully");
}

void example_neuromodulation() {
    NLM_LOG_INFO("=== Example 4: Neuromodulation ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (!dopamine || !curiosity || !novelty || !predictionError) {
        NLM_LOG_ERROR("Neuromodulation systems not properly initialized");
        return;
    }
    
    // Run simulation with neuromodulation
    for (uint64_t step = 0; step < 300; ++step) {
        brain->step(step, step * 0.001);
        
        // Apply sensory input
        if (step % 30 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        }
        
        // Log neuromodulation levels
        if (step % 50 == 0) {
            NLM_LOG_INFO("Dopamine: " + std::to_string(dopamine->getLevel()) +
                        ", Curiosity: " + std::to_string(curiosity->getLevel()) +
                        ", Novelty: " + std::to_string(novelty->getLevel()) +
                        ", Prediction Error: " + std::to_string(predictionError->getError()));
        }
    }
    
    NLM_LOG_INFO("Example 4 completed successfully");
}

void example_development_stages() {
    NLM_LOG_INFO("=== Example 5: Development Stages ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Check initial development stage
    NLM_LOG_INFO("Initial developmental stage: " + 
                std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    
    // Run simulation and observe development
    for (uint64_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Log development stage every 200 steps
        if (step % 200 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        ", Development Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) +
                        ", Plasticity Factor: " + std::to_string(brain->getDevelopmentSystem() ? 
                            brain->getDevelopmentSystem()->getPlasticityFactor() : 0.0));
        }
        
        // Change development stage at specific steps
        if (step == 200) {
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (step == 400) {
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else if (step == 600) {
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    NLM_LOG_INFO("Example 5 completed successfully");
}

void example_checkpointing() {
    NLM_LOG_INFO("=== Example 6: Checkpointing ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("checkpoint_dir", "./test_checkpoints");
    config->set("checkpoint_interval", 50);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Run simulation and save checkpoints
    for (uint64_t step = 0; step < 200; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            std::string checkpointPath = "./test_checkpoint_" + std::to_string(step) + ".bin";
            if (brain->save(checkpointPath)) {
                NLM_LOG_INFO("Checkpoint saved: " + checkpointPath);
            } else {
                NLM_LOG_WARNING("Failed to save checkpoint at step " + std::to_string(step));
            }
        }
    }
    
    // Test loading checkpoints
    for (uint64_t step : {50, 100, 150}) {
        std::string checkpointPath = "./test_checkpoint_" + std::to_string(step) + ".bin";
        if (std::filesystem::exists(checkpointPath)) {
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            
            if (brain2->load(checkpointPath)) {
                NLM_LOG_INFO("Checkpoint loaded successfully: " + checkpointPath);
                NLM_LOG_INFO("Loaded brain total spikes: " + std::to_string(brain2->getTotalSpikeCount()));
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint: " + checkpointPath);
            }
        }
    }
    
    // Cleanup
    std::filesystem::remove_all("./test_checkpoints");
    for (int step : {50, 100, 150}) {
        std::string checkpointPath = "./test_checkpoint_" + std::to_string(step) + ".bin";
        std::filesystem::remove(checkpointPath);
    }
    
    NLM_LOG_INFO("Example 6 completed successfully");
}

void example_complete_integration() {
    NLM_LOG_INFO("=== Example 7: Complete Integration ===");
    
    // Use Phase 6 integrated experiment
    Phase6IntegratedExperiment experiment;
    
    if (!experiment.verifyIntegration()) {
        NLM_LOG_ERROR("Integration verification failed");
        return;
    }
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 1000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    NLM_LOG_INFO("=== Integration Results ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
    NLM_LOG_INFO("Example 7 completed successfully");
}

void example_performance_comparison() {
    NLM_LOG_INFO("=== Example 8: Performance Comparison ===");
    
    std::vector<std::pair<std::string, std::shared_ptr<Config>>> configurations = {
        {"Basic", create_basic_config()},
        {"Advanced", create_advanced_config()},
        {"Research", create_research_config()},
        {"Performance", create_performance_config()}
    };
    
    for (const auto& pair : configurations) {
        NLM_LOG_INFO("Testing configuration: " + pair.first);
        
        auto brain = std::make_shared<Brain>(pair.second);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain with " + pair.first + " config");
            continue;
        }
        
        auto startTime = std::chrono::steady_clock::now();
        
        for (uint64_t step = 0; step < 500; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("  Performance: " + std::to_string(duration.count()) + "ms for 500 steps");
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    }
    
    NLM_LOG_INFO("Example 8 completed successfully");
}

void example_configuration_management() {
    NLM_LOG_INFO("=== Example 9: Configuration Management ===");
    
    // Create configuration from scratch
    auto config = std::make_shared<Config>();
    
    // Set various parameters
    config->set("simulation.neuron_count", 1000);
    config->set("simulation.region_count", 2);
    config->set("simulation.timestep", 0.0005);
    config->set("plasticity.stdp_ltp_weight", 0.02);
    config->set("plasticity.stdp_ltd_weight", 0.015);
    config->set("neuromodulation.dopamine_baseline", 0.1);
    config->set("neuromodulation.curiosity_enabled", true);
    
    // Show configuration
    NLM_LOG_INFO("Configuration summary:");
    NLM_LOG_INFO(config->summary());
    
    // Save configuration
    config->saveToFile("example_config.cfg");
    NLM_LOG_INFO("Configuration saved to example_config.cfg");
    
    // Load configuration
    auto config2 = std::make_shared<Config>();
    if (config2->loadFromFile("example_config.cfg")) {
        NLM_LOG_INFO("Configuration loaded successfully");
        NLM_LOG_INFO(config2->summary());
    }
    
    // Clean up
    std::filesystem::remove("example_config.cfg");
    
    NLM_LOG_INFO("Example 9 completed successfully");
}

void example_advanced_debugging() {
    NLM_LOG_INFO("=== Example 10: Advanced Debugging ===");
    
    // Enable advanced logging
    g_advancedLogger.enableFileLogging("advanced_debug.log");
    g_advancedLogger.setConsoleLogging(true);
    g_advancedLogger.setConsoleLogLevel(LogLevel::Debug);
    g_advancedLogger.enableColoredOutput(true);
    
    // Set source filtering
    g_advancedLogger.includeSource("Brain");
    g_advancedLogger.includeSource("Neuromodulation");
    g_advancedLogger.excludeSource("Debug");
    
    // Create and run brain with debugging
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    // Run simulation with detailed logging
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Record performance metrics
        g_advancedLogger.recordPerformanceMetric(PerformanceMetric::SpikeCount,
                                                brain->getFiringNeuronCount(),
                                                "step_" + std::to_string(step));
        
        // Log debug information
        if (step % 10 == 0) {
            g_advancedLogger.logDebug("BrainStatus",
                                    "Step: " + std::to_string(step) +
                                    ", Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                                    ", Spikes: " + std::to_string(brain->getFiringNeuronCount()),
                                    {{ "simulation_step", std::to_string(step) }});
        }
    }
    
    // Generate reports
    NLM_LOG_INFO("=== Performance Report ===");
    NLM_LOG_INFO(g_advancedLogger.generatePerformanceReport());
    
    NLM_LOG_INFO("=== Event Report ===");
    NLM_LOG_INFO(g_advancedLogger.generateEventReport("Brain"));
    
    // Get performance trends
    auto trends = g_advancedLogger.analyzePerformanceTrends();
    NLM_LOG_INFO("Performance Trends:");
    for (const auto& pair : trends) {
        NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second));
    }
    
    // Export logs
    g_advancedLogger.exportLogs("debug_output.json");
    
    // Cleanup
    std::filesystem::remove("advanced_debug.log");
    std::filesystem::remove("debug_output.json");
    
    NLM_LOG_INFO("Example 10 completed successfully");
}

// Tutorial implementations
void NLMGettingStarted::tutorial_basic_usage() {
    NLM_LOG_INFO("=== NLM Getting Started: Basic Usage ===");
    
    NLM_LOG_INFO("1. Create configuration");
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100);
    config->set("brain.region_count", 1);
    
    NLM_LOG_INFO("2. Initialize brain");
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NLM_LOG_INFO("3. Run simulation");
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    NLM_LOG_INFO("4. Check results");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    NLM_LOG_INFO("Basic usage tutorial completed");
}

void NLMGettingStarted::tutorial_configuration() {
    NLM_LOG_INFO("=== NLM Getting Started: Configuration ===");
    
    NLM_LOG_INFO("Configuration options available:");
    NLM_LOG_INFO("- simulation.neuron_count: Total number of neurons");
    NLM_LOG_INFO("- simulation.region_count: Number of brain regions");
    NLM_LOG_INFO("- simulation.timestep: Simulation timestep");
    NLM_LOG_INFO("- plasticity.*: Plasticity parameters");
    NLM_LOG_INFO("- neuromodulation.*: Neuromodulation parameters");
    NLM_LOG_INFO("- development.*: Development parameters");
    
    NLM_LOG_INFO("Configuration tutorial completed");
}

void NLMGettingStarted::tutorial_simulation() {
    NLM_LOG_INFO("=== NLM Getting Started: Simulation ===");
    
    NLM_LOG_INFO("Simulation controls:");
    NLM_LOG_INFO("- brain.step(step): Run one simulation step");
    NLM_LOG_INFO("- brain.step(step, time): Run step with timestamp");
    NLM_LOG_INFO("- brain.reset(): Reset brain state");
    NLM_LOG_INFO("- brain.getTotalSpikeCount(): Get total spikes");
    NLM_LOG_INFO("- brain.getAverageFiringRate(): Get average firing rate");
    
    NLM_LOG_INFO("Simulation tutorial completed");
}

void NLMGettingStarted::tutorial_memory() {
    NLM_LOG_INFO("=== NLM Getting Started: Memory ===");
    
    NLM_LOG_INFO("Memory systems available:");
    NLM_LOG_INFO("- Working Memory: Short-term storage of active neural states");
    NLM_LOG_INFO("- Episodic Memory: Storage of experienced episodes");
    NLM_LOG_INFO("- Associative Memory: Pattern association and recall");
    
    NLM_LOG_INFO("Memory tutorial completed");
}

void NLMIntermediate::tutorial_neuromodulation() {
    NLM_LOG_INFO("=== NLM Intermediate: Neuromodulation ===");
    
    NLM_LOG_INFO("Neuromodulation systems:");
    NLM_LOG_INFO("- Dopamine: Reward prediction error and plasticity modulation");
    NLM_LOG_INFO("- Curiosity: Exploration drive and novelty detection");
    NLM_LOG_INFO("- Novelty: Unpredictability detection");
    NLM_LOG_INFO("- Prediction Error: Learning prediction errors");
    
    NLM_LOG_INFO("... implementation would demonstrate neuromodulation usage");
    
    NLM_LOG_INFO("Neuromodulation tutorial completed");
}

void NLMIntermediate::tutorial_development() {
    NLM_LOG_INFO("=== NLM Intermediate: Development ===");
    
    NLM_LOG_INFO("Developmental stages:");
    NLM_LOG_INFO("- Initial: High plasticity, rapid learning");
    NLM_LOG_INFO("- Critical Period: Sensitive period for learning");
    NLM_LOG_INFO("- Maturation: Slower, more refined learning");
    NLM_LOG_INFO("- Adult: Stable, maintenance");
    NLM_LOG_INFO("- Aging: Declining plasticity");
    
    NLM_LOG_INFO("... implementation would demonstrate development stages");
    
    NLM_LOG_INFO("Development tutorial completed");
}

void NLMIntermediate::tutorial_advanced_features() {
    NLM_LOG_INFO("=== NLM Intermediate: Advanced Features ===");
    
    NLM_LOG_INFO("Advanced features:");
    NLM_LOG_INFO("- Checkpointing and replay");
    NLM_LOG_INFO("- Memory consolidation");
    NLM_LOG_INFO("- Structural plasticity");
    NLM_LOG_INFO("- Prediction and planning");
    NLM_LOG_INFO("- Social learning");
    
    NLM_LOG_INFO("... implementation would demonstrate advanced features");
    
    NLM_LOG_INFO("Advanced features tutorial completed");
}

void NLMAdvanced::tutorial_custom_networks() {
    NLM_LOG_INFO("=== NLM Advanced: Custom Networks ===");
    
    NLM_LOG_INFO("Custom network design:");
    NLM_LOG_INFO("- Region topology and connectivity");
    NLM_LOG_INFO("- Population-specific parameters");
    NLM_LOG_INFO("- Synaptic rule customization");
    NLM_LOG_INFO("- Neuromodulation integration");
    
    NLM_LOG_INFO("... implementation would demonstrate custom network design");
    
    NLM_LOG_INFO("Custom networks tutorial completed");
}

void NLMAdvanced::tutorial_optimization() {
    NLM_LOG_INFO("=== NLM Advanced: Optimization ===");
    
    NLM_LOG_INFO("Performance optimization strategies:");
    NLM_LOG_INFO("- Vectorization and SIMD");
    NLM_LOG_INFO("- Parallel processing");
    NLM_LOG_INFO("- Memory pooling");
    NLM_LOG_INFO("- Event-driven architecture");
    
    NLM_LOG_INFO("... implementation would demonstrate optimization techniques");
    
    NLM_LOG_INFO("Optimization tutorial completed");
}

void NLMAdvanced::tutorial_research_experiments() {
    NLM_LOG_INFO("=== NLM Advanced: Research Experiments ===");
    
    NLM_LOG_INFO("Research experiment types:");
    NLM_LOG_INFO("- Cognitive development studies");
    NLM_LOG_INFO("- Learning algorithm testing");
    NLM_LOG_INFO("- Neuromodulation effects");
    NLM_LOG_INFO("- Memory system analysis");
    
    NLM_LOG_INFO("... implementation would demonstrate research experiment design");
    
    NLM_LOG_INFO("Research experiments tutorial completed");
}

// Configuration implementations
std::shared_ptr<Config> ExampleConfigurations::create_basic_config() {
    auto config = std::make_shared<Config>();
    config->set("simulation.neuron_count", 500);
    config->set("simulation.region_count", 1);
    config->set("simulation.timestep", 0.001);
    config->set("plasticity.stdp_ltp_weight", 0.01);
    config->set("plasticity.stdp_ltd_weight", 0.012);
    return config;
}

std::shared_ptr<Config> ExampleConfigurations::create_advanced_config() {
    auto config = std::make_shared<Config>();
    config->set("simulation.neuron_count", 1000);
    config->set("simulation.region_count", 2);
    config->set("simulation.timestep", 0.0005);
    config->set("plasticity.stdp_ltp_weight", 0.02);
    config->set("plasticity.stdp_ltd_weight", 0.015);
    config->set("plasticity.synaptogenesis_rate", 0.0002);
    config->set("plasticity.pruning_rate", 0.00005);
    config->set("neuromodulation.dopamine_baseline", 0.1);
    config->set("neuromodulation.curiosity_enabled", true);
    config->set("development.enabled", true);
    config->set("development.critical_period_duration", 100.0);
    return config;
}

std::shared_ptr<Config> ExampleConfigurations::create_research_config() {
    auto config = std::make_shared<Config>();
    config->set("simulation.neuron_count", 2000);
    config->set("simulation.region_count", 4);
    config->set("simulation.timestep", 0.0001);
    config->set("plasticity.stdp_ltp_weight", 0.03);
    config->set("plasticity.stdp_ltd_weight", 0.02);
    config->set("plasticity.synaptogenesis_rate", 0.0005);
    config->set("plasticity.pruning_rate", 0.0001);
    config->set("neuromodulation.dopamine_baseline", 0.2);
    config->set("neuromodulation.curiosity_enabled", true);
    config->set("neuromodulation.novelty_enabled", true);
    config->set("development.enabled", true);
    config->set("development.critical_period_duration", 200.0);
    config->set("development.maturation_rate", 0.8);
    return config;
}

std::shared_ptr<Config> ExampleConfigurations::create_performance_config() {
    auto config = std::make_shared<Config>();
    config->set("simulation.neuron_count", 100);
    config->set("simulation.region_count", 1);
    config->set("simulation.timestep", 0.001);
    config->set("plasticity.stdp_ltp_weight", 0.008);
    config->set("plasticity.stdp_ltd_weight", 0.01);
    config->set("plasticity.synaptogenesis_rate", 0.00005);
    config->set("plasticity.pruning_rate", 0.00001);
    config->set("performance.optimization_level", "aggressive");
    config->set("performance.parallel_processing", true);
    config->set("performance.vectorization", true);
    config->set("performance.memory_pooling", true);
    return config;
}

// Output generators
std::string ExampleOutput::generate_basic_example_output() {
    std::ostringstream oss;
    oss << "=== Basic Example Output ===\n";
    oss << "Neurons: 500\n";
    oss << "Regions: 1\n";
    oss << "Total Spikes: 1250\n";
    oss << "Average Firing Rate: 2.5 Hz\n";
    oss << "Simulation Time: 1.25 seconds\n";
    return oss.str();
}

std::string ExampleOutput::generate_neuromodulation_example_output() {
    std::ostringstream oss;
    oss << "=== Neuromodulation Example Output ===\n";
    oss << "Dopamine Level: 0.8\n";
    oss << "Curiosity Level: 0.6\n";
    oss << "Novelty Level: 0.4\n";
    oss << "Prediction Error: 0.15\n";
    oss << "Plasticity Factor: 1.2\n";
    return oss.str();
}

std::string ExampleOutput::generate_development_example_output() {
    std::ostringstream oss;
    oss << "=== Development Example Output ===\n";
    oss << "Development Stage: Maturation\n";
    oss << "Plasticity Factor: 0.8\n";
    oss << "Learning Rate: 0.05\n";
    oss << "Stability Factor: 0.9\n";
    return oss.str();
}

std::string ExampleOutput::generate_memory_example_output() {
    std::ostringstream oss;
    oss << "=== Memory Example Output ===\n";
    oss << "Working Memory Traces: 15\n";
    oss << "Episodic Memory Episodes: 8\n";
    oss << "Associative Memory Patterns: 23\n";
    oss << "Memory Consolidation: 0.7\n";
    return oss.str();
}

std::string ExampleOutput::generate_performance_example_output() {
    std::ostringstream oss;
    oss << "=== Performance Example Output ===\n";
    oss << "Basic Config: 500 neurons, 1.25s\n";
    oss << "Advanced Config: 1000 neurons, 2.5s\n";
    oss << "Research Config: 2000 neurons, 5.0s\n";
    oss << "Performance Config: 100 neurons, 0.8s\n";
    oss << "Speedup Factor: 6.25x\n";
    return oss.str();
}

// Validation implementations
bool ExampleValidator::validate_example_1() {
    // Validate basic brain setup
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }
        
        return brain->getTotalNeuronCount() == 500;
    } catch (...) {
        return false;
    }
}

bool ExampleValidator::validate_example_2() {
    // Validate simulation loop
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }
        
        uint64_t initialSpikes = brain->getTotalSpikeCount();
        
        for (uint64_t step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount() > initialSpikes;
    } catch (...) {
        return false;
    }
}

// Additional validation methods would be implemented similarly

// Example database
ExampleDatabase::ExampleDatabase() {
    // Initialize with example metadata
    add_example({
        "basic_setup",
        "Sets up a basic brain with minimal configuration",
        "Beginner",
        "Setup",
        {},
        {"Create a configuration", "Initialize brain", "Run simulation"},
        5.0,
        "text",
        true
    });
    
    add_example({
        "simulation_loop",
        "Demonstrates basic simulation loop with logging",
        "Beginner",
        "Simulation",
        {"Basic setup"},
        {"Understand simulation loop", "Monitor brain state", "Log simulation data"},
        10.0,
        "text+visual",
        true
    });
    
    // Additional examples would be added
}

void ExampleDatabase::add_example(const ExampleMetadata& metadata) {
    examples.push_back(metadata);
}

ExampleMetadata* ExampleDatabase::get_example(const std::string& name) {
    for (auto& example : examples) {
        if (example.name == name) {
            return &example;
        }
    }
    return nullptr;
}

std::vector<ExampleMetadata> ExampleDatabase::get_examples_by_difficulty(const std::string& difficulty) {
    std::vector<ExampleMetadata> result;
    for (const auto& example : examples) {
        if (example.difficulty == difficulty) {
            result.push_back(example);
        }
    }
    return result;
}

std::vector<ExampleMetadata> ExampleDatabase::get_examples_by_category(const std::string& category) {
    std::vector<ExampleMetadata> result;
    for (const auto& example : examples) {
        if (example.category == category) {
            result.push_back(example);
        }
    }
    return result;
}

std::vector<ExampleMetadata> ExampleDatabase::get_all_examples() {
    return examples;
}

size_t ExampleDatabase::get_example_count() const {
    return examples.size();
}

bool ExampleDatabase::save_to_file(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // In a real implementation, this would serialize the example metadata
        // For now, just write a placeholder
        file << "NLM Examples Database" << std::endl;
        file << "Total examples: " << examples.size() << std::endl;
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool ExampleDatabase::load_from_file(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // In a real implementation, this would deserialize the example metadata
            if (line.find("Total examples: ") == 0) {
                // Parse the count (placeholder)
            }
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

// Complete example runner
bool ExampleRunner::run_example(ExampleType type) {
    switch (type) {
        case ExampleType::BasicSetup:
            example_basic_brain_setup();
            return true;
        case ExampleType::SimulationLoop:
            example_simulation_loop();
            return true;
        case ExampleType::MemoryIntegration:
            example_memory_integration();
            return true;
        case ExampleType::Neuromodulation:
            example_neuromodulation();
            return true;
        case ExampleType::DevelopmentStages:
            example_development_stages();
            return true;
        case ExampleType::Checkpointing:
            example_checkpointing();
            return true;
        case ExampleType::CompleteIntegration:
            example_complete_integration();
            return true;
        case ExampleType::PerformanceComparison:
            example_performance_comparison();
            return true;
        case ExampleType::ConfigurationManagement:
            example_configuration_management();
            return true;
        case ExampleType::AdvancedDebugging:
            example_advanced_debugging();
            return true;
        default:
            NLM_LOG_ERROR("Unknown example type: " + std::to_string(static_cast<int>(type)));
            return false;
    }
}

bool ExampleRunner::run_all_examples() {
    std::vector<ExampleType> allExamples = get_available_examples();
    bool success = true;
    
    for (const auto& example : allExamples) {
        NLM_LOG_INFO("Running example: " + get_example_name(example));
        if (!run_example(example)) {
            NLM_LOG_ERROR("Failed to run example: " + get_example_name(example));
            success = false;
        }
    }
    
    return success;
}

bool ExampleRunner::run_example_range(ExampleType start, ExampleType end) {
    std::vector<ExampleType> examples = get_available_examples();
    bool success = true;
    
    // Find the range
    auto start_it = std::find(examples.begin(), examples.end(), start);
    auto end_it = std::find(examples.begin(), examples.end(), end);
    
    if (start_it == examples.end() || end_it == examples.end()) {
        NLM_LOG_ERROR("Example type not found in available examples");
        return false;
    }
    
    for (auto it = start_it; it != end_it; ++it) {
        NLM_LOG_INFO("Running example: " + get_example_name(*it));
        if (!run_example(*it)) {
            NLM_LOG_ERROR("Failed to run example: " + get_example_name(*it));
            success = false;
        }
    }
    
    return success;
}

std::vector<ExampleRunner::ExampleType> ExampleRunner::get_available_examples() {
    return {
        ExampleType::BasicSetup,
        ExampleType::SimulationLoop,
        ExampleType::MemoryIntegration,
        ExampleType::Neuromodulation,
        ExampleType::DevelopmentStages,
        ExampleType::Checkpointing,
        ExampleType::CompleteIntegration,
        ExampleType::PerformanceComparison,
        ExampleType::ConfigurationManagement,
        ExampleType::AdvancedDebugging
    };
}

std::string ExampleRunner::get_example_name(ExampleType type) {
    const char* names[] = {
        "BasicSetup",
        "SimulationLoop",
        "MemoryIntegration",
        "Neuromodulation",
        "DevelopmentStages",
        "Checkpointing",
        "CompleteIntegration",
        "PerformanceComparison",
        "ConfigurationManagement",
        "AdvancedDebugging"
    };
    
    int index = static_cast<int>(type);
    if (index >= 0 && index < 10) {
        return names[index];
    }
    return "Unknown";
}

// Documentation generator (simplified implementations)
std::string DocumentationGenerator::generate_markdown_documentation() {
    std::ostringstream oss;
    oss << "# NLM Documentation\n\n";
    oss << "This is the main documentation for the NLM project.\n\n";
    oss << "## Examples\n\n";
    oss << "See the Examples.hpp header for detailed documentation.\n";
    return oss.str();
}

std::string DocumentationGenerator::generate_html_documentation() {
    return generate_markdown_documentation(); // Placeholder
}

std::string DocumentationGenerator::generate_latex_documentation() {
    return generate_markdown_documentation(); // Placeholder
}

std::string DocumentationGenerator::generate_python_documentation() {
    std::ostringstream oss;
    oss << "Python documentation for NLM bindings would go here.\n\n";
    oss << "For more information, see the Python bindings in the python/ directory.\n";
    return oss.str();
}

// Global instance
ExampleDatabase g_exampleDatabase;

} // namespace nlm
