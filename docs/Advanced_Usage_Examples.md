# Advanced Usage Examples for Neural Simulation Project

## Overview

This document provides comprehensive examples demonstrating the advanced usage of the Neural Simulation Project. These examples showcase the full range of capabilities, from basic simulations to complex research scenarios.

## Example 1: Basic Neural Simulation

This example demonstrates a basic neural simulation with standard settings.

```cpp
#include "Brain.hpp"
#include "ConfigurationManager.hpp"
#include <iostream>

int main() {
    std::cout << "=== Basic Neural Simulation Example ===" << std::endl;
    
    // Load configuration
    ConfigurationManager& config = ConfigurationManager::getInstance();
    config.loadConfiguration("configs/default.cfg");
    
    // Create brain with default configuration
    Brain brain(config.getConfig<BrainConfig>());
    
    std::cout << "Created brain with " << brain.getNeuronCount() << " neurons and " 
              << brain.getSynapseCount() << " synapses" << std::endl;
    
    // Create neurons with different initial states
    std::vector<NeuronId> neurons;
    neurons.reserve(10);
    
    for (int i = 0; i < 10; ++i) {
        NeuronState state;
        state.membranePotential = -65.0f + static_cast<float>(i) * 2.0f;
        state.threshold = -55.0f;
        state.firingState = FiringState::Resting;
        state.firingRate = 0.0f;
        state.refractoryPeriod = 2 + (i % 5);
        state.leakConductance = 0.1f;
        state.restingPotential = -70.0f;
        state.resetPotential = -65.0f;
        
        neurons.push_back(brain.createNeuron(state));
    }
    
    std::cout << "Created " << neurons.size() << " neurons" << std::endl;
    
    // Create synapses between neurons
    std::vector<SynapseId> synapses;
    synapses.reserve(20);
    
    for (size_t i = 0; i < neurons.size() - 1; ++i) {
        for (size_t j = i + 1; j < neurons.size(); ++j) {
            SynapseState state;
            state.weight = (static_cast<float>(rand()) / RAND_MAX) * 0.8f - 0.4f;
            state.delay = static_cast<uint32_t>(rand() % 10);
            state.type = SynapseType::Excitatory;
            state.plasticityFlags.hebbian = true;
            state.plasticityFlags.stdp = true;
            state.plasticityFlags.reward_modulated = false;
            
            synapses.push_back(brain.createSynapse(neurons[i], neurons[j], state));
        }
    }
    
    std::cout << "Created " << synapses.size() << " synapses" << std::endl;
    
    // Run simulation
    std::cout << "Running simulation for 100 steps..." << std::endl;
    brain.run(100);
    
    std::cout << "Simulation completed in " << brain.getStepCount() << " steps" << std::endl;
    std::cout << "Final simulation time: " << brain.getSimulationTime() << " ms" << std::endl;
    
    // Save results
    brain.saveState("results/basic_simulation.brain");
    
    std::cout << "Results saved to results/basic_simulation.brain" << std::endl;
    
    return 0;
}
```

## Example 2: Advanced Neuromodulation

This example demonstrates advanced neuromodulation with dopamine, acetylcholine, and other signals.

```cpp
#include "Brain.hpp"
#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include <memory>
#include <iostream>

class CustomDopamine : public Dopamine {
public:
    CustomDopamine() : rewardResponse_(0.0f), predictionError_(0.0f) {}
    
    void signalReward(float reward) override {
        Dopamine::signalReward(reward);
        rewardResponse_ = reward;
        
        // Apply reward prediction error
        if (rewardResponse_ > 0.5f) {
            signalRewardPredictionError(reward - expectedReward_);
        }
    }
    
    void update(TimestepDuration dt) override {
        Dopamine::update(dt);
        
        // Add noise for more realistic dynamics
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.1f;
        setLevel(std::clamp(getLevel() + noise, 0.0f, 1.0f));
    }
    
private:
    float rewardResponse_;
    float expectedReward_ = 0.0f;
};

class AdvancedNeuromodulationSimulation {
public:
    AdvancedNeuromodulationSimulation() {
        // Create advanced neuromodulators
        dopamine_ = std::make_unique<CustomDopamine>();
        acetylcholine_ = std::make_unique<Acetylcholine>();
        norepinephrine_ = std::make_unique<Norepinephrine>();
        serotonin_ = std::make_unique<Serotonin>();
        
        // Set initial levels
        dopamine_->setLevel(0.2f);
        acetylcholine_->setLevel(0.1f);
        norepinephrine_->setLevel(0.05f);
        serotonin_->setLevel(0.05f);
        
        // Configure brain with neuromodulation
        configureBrainWithNeuromodulation();
    }
    
    void runAdvancedSimulation(size_t steps) {
        std::cout << "=== Advanced Neuromodulation Simulation ===" << std::endl;
        std::cout << "Dopamine baseline: " << dopamine_->getLevel() << std::endl;
        std::cout << "Acetylcholine baseline: " << acetylcholine_->getLevel() << std::endl;
        
        for (size_t i = 0; i < steps; ++i) {
            // Simulate environment and generate rewards
            float reward = simulateEnvironment(i);
            
            // Update neuromodulators
            dopamine_->signalReward(reward);
            
            // Update brain
            brain_.step();
            
            // Log progress every 100 steps
            if (i % 100 == 0) {
                std::cout << "Step " << i << ": Dopamine level = " << dopamine_->getLevel() << std::endl;
            }
        }
        
        std::cout << "Advanced simulation completed" << std::endl;
    }
    
private:
    void configureBrainWithNeuromodulation() {
        // Configure brain with neuromodulation
        BrainConfig config;
        config.random_seed = 12345;
        config.simulation_timestep = 0.001;
        config.neuron_count = 2000;
        config.region_count = 3;
        config.connection_probability = 0.15;
        
        // Advanced plasticity settings
        config.plasticity_learning_rate = 0.02;
        config.stdp_ltp_weight = 0.015;
        config.stdp_ltd_weight = 0.018;
        config.stdp_time_constant = 18.0;
        
        // Neuromodulation settings
        config.dopamine_baseline = 0.15;
        config.acetylcholine_baseline = 0.1f;
        config.noradrenaline_baseline = 0.05f;
        config.serotonin_baseline = 0.05f;
        
        // Advanced reward settings
        config.reward_discount_factor = 0.999;
        config.reward_scaling = 1.0f;
        config.prediction_error_scaling = 0.7f;
        
        // Environment
        config.environment_name = "AdvancedWorld";
        config.environment_width = 20;
        config.environment_height = 20;
        
        brain_ = Brain(config);
        brain_.setNeuromodulator(dopamine_.get());
    }
    
    float simulateEnvironment(size_t step) {
        // Complex environment simulation
        float reward = 0.0f;
        
        // Time-based reward function
        if (step % 50 == 0) {
            reward += 1.0f; // Periodic reward
        }
        
        // Step-based reward
        reward += static_cast<float>(step) * 0.001f;
        
        // Noise for realism
        reward += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
        
        // Cap reward
        reward = std::clamp(reward, -1.0f, 2.0f);
        
        return reward;
    }
    
    std::unique_ptr<CustomDopamine> dopamine_;
    std::unique_ptr<Acetylcholine> acetylcholine_;
    std::unique_ptr<Norepinephrine> norepinephrine_;
    std::unique_ptr<Serotonin> serotonin_;
    Brain brain_;
};

int main() {
    AdvancedNeuromodulationSimulation sim;
    sim.runAdvancedSimulation(1000);
    
    return 0;
}
```

## Example 3: Performance Profiling and Optimization

This example demonstrates how to use the performance profiling tools to optimize simulations.

```cpp
#include "PerformanceProfiler.hpp"
#include "MemoryPool.hpp"
#include "Brain.hpp"
#include <iostream>
#include <fstream>

class PerformanceOptimizedSimulation {
public:
    PerformanceOptimizedSimulation() {
        // Create performance profiler
        profiler_ = std::make_unique<PerformanceProfiler>("optimized_simulation");
        profiler_->start();
        
        // Configure memory pools
        configureMemoryPools();
        
        // Load optimized configuration
        loadOptimizedConfiguration();
        
        // Create brain
        brain_ = Brain(config_.getConfig<BrainConfig>());
    }
    
    void runOptimizedSimulation(size_t steps) {
        std::cout << "=== Performance Optimized Simulation ===" << std::endl;
        
        // Profile different operations
        profiler_->profileFunction("brain_creation", [&]() {
            createNeuronsAndSynapses();
        });
        
        profiler_->profileFunction("simulation", [&]() {
            brain_.run(steps);
        });
        
        profiler_->profileFunction("neuron_initialization", [&]() {
            initializeNeurons();
        });
        
        profiler_->profileFunction("synapse_initialization", [&]() {
            initializeSynapses();
        });
        
        // Get profiling results
        auto results = profiler_->stop();
        
        // Print performance report
        printPerformanceReport(results);
        
        // Save results
        profiler_->saveResults("performance/optimized_results.json");
        profiler_->generateReport("performance/optimized_report.html");
        
        std::cout << "Optimized simulation completed" << std::endl;
    }
    
private:
    void configureMemoryPools() {
        std::cout << "Configuring memory pools..." << std::endl;
        
        // Get memory manager
        auto& memoryManager = MemoryPool::Manager::getInstance();
        
        // Configure neuron pool
        auto& neuronPool = memoryManager.getPool<Neuron>();
        neuronPool.reserve(10000);
        
        // Configure synapse pool
        auto& synapsePool = memoryManager.getPool<Synapse>();
        synapsePool.reserve(50000);
        
        std::cout << "Memory pools configured: " << neuronPool.size() << " neurons, " 
                  << synapsePool.size() << " synapses" << std::endl;
    }
    
    void loadOptimizedConfiguration() {
        std::cout << "Loading optimized configuration..." << std::endl;
        
        // Load advanced configuration
        config_ = ConfigurationManager::getInstance();
        config_.loadConfiguration("configs/optimized_config.json");
        
        std::cout << "Configuration loaded successfully" << std::endl;
    }
    
    void createNeuronsAndSynapses() {
        std::cout << "Creating neurons and synapses..." << std::endl;
        
        // Create neurons
        for (size_t i = 0; i < 1000; ++i) {
            NeuronState state;
            state.membranePotential = -70.0f + static_cast<float>(i) * 0.1f;
            state.threshold = -55.0f;
            state.firingState = FiringState::Resting;
            state.firingRate = 0.0f;
            state.refractoryPeriod = 2 + (i % 5);
            
            brain_.createNeuron(state);
        }
        
        // Create synapses
        auto neurons = getAllNeuronIds();
        for (size_t i = 0; i < neurons.size() - 1; ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                SynapseState state;
                state.weight = (static_cast<float>(rand()) / RAND_MAX) * 0.8f - 0.4f;
                state.delay = static_cast<uint32_t>(rand() % 10);
                state.type = SynapseType::Excitatory;
                state.plasticityFlags.hebbian = true;
                state.plasticityFlags.stdp = true;
                
                brain_.createSynapse(neurons[i], neurons[j], state);
            }
        }
        
        std::cout << "Created " << neurons.size() << " neurons and " 
                  << brain_.getSynapseCount() << " synapses" << std::endl;
    }
    
    void initializeNeurons() {
        std::cout << "Initializing neurons..." << std::endl;
        
        // Initialize neurons with random values
        auto neurons = getAllNeuronIds();
        for (NeuronId id : neurons) {
            Neuron* neuron = brain_.getNeuron(id);
            if (neuron) {
                neuron->initializeRandom(RandomGenerator(42));
            }
        }
        
        std::cout << "Neurons initialized" << std::endl;
    }
    
    void initializeSynapses() {
        std::cout << "Initializing synapses..." << std::endl;
        
        // Initialize synapses with random values
        auto synapses = getAllSynapseIds();
        for (SynapseId id : synapses) {
            Synapse* synapse = brain_.getSynapse(id);
            if (synapse) {
                synapse->initializeRandom(RandomGenerator(42));
            }
        }
        
        std::cout << "Synapses initialized" << std::endl;
    }
    
    std::vector<NeuronId> getAllNeuronIds() {
        std::vector<NeuronId> ids;
        for (size_t i = 0; i < brain_.getNeuronCount(); ++i) {
            ids.push_back(static_cast<NeuronId>(i));
        }
        return ids;
    }
    
    std::vector<SynapseId> getAllSynapseIds() {
        std::vector<SynapseId> ids;
        for (size_t i = 0; i < brain_.getSynapseCount(); ++i) {
            ids.push_back(static_cast<SynapseId>(i));
        }
        return ids;
    }
    
    void printPerformanceReport(const std::vector<ProfileResult>& results) {
        std::cout << "\n=== Performance Report ===" << std::endl;
        
        double totalTime = 0.0;
        size_t totalOperations = 0;
        
        for (const auto& result : results) {
            std::cout << result.function_name << ": " 
                      << result.duration_ms << " ms, " 
                      << result.memory_current / 1024 << " KB current, " 
                      << result.memory_peak / 1024 << " KB peak" << std::endl;
            
            totalTime += result.duration_ms;
            totalOperations++;
        }
        
        std::cout << std::endl;
        std::cout << "Total time: " << totalTime << " ms" << std::endl;
        std::cout << "Average time per operation: " << (totalTime / totalOperations) << " ms" << std::endl;
        
        // Print memory usage
        auto& memoryManager = MemoryPool::Manager::getInstance();
        auto stats = memoryManager.getStatistics();
        
        std::cout << std::endl;
        std::cout << "Memory Usage:" << std::endl;
        std::cout << "  Neuron count: " << stats.neuronCount << std::endl;
        std::cout << "  Synapse count: " << stats.synapseCount << std::endl;
        std::cout << "  Allocated memory: " << stats.allocatedMemory / 1024 << " KB" << std::endl;
        std::cout << "  Peak memory: " << stats.peakMemory / 1024 << " KB" << std::endl;
        std::cout << "  Utilization rate: " << stats.utilizationRate * 100 << "%" << std::endl;
    }
    
    std::unique_ptr<PerformanceProfiler> profiler_;
    ConfigurationManager config_;
    Brain brain_;
};

int main() {
    PerformanceOptimizedSimulation sim;
    sim.runOptimizedSimulation(1000);
    
    return 0;
}
```

## Example 4: Advanced Visualization

This example demonstrates advanced visualization capabilities with multiple backends and formats.

```cpp
#include "VisualizationInterface.hpp"
#include "Brain.hpp"
#include <iostream>

class AdvancedVisualizationSimulation {
public:
    AdvancedVisualizationSimulation() {
        // Create visualization interface
        viz_ = std::make_unique<VisualizationInterface>();
        
        // Configure for web-based visualization
        configureWebVisualization();
        
        // Initialize visualization
        if (!viz_->initialize()) {
            std::cerr << "Failed to initialize visualization" << std::endl;
            return;
        }
        
        // Configure brain
        configureBrain();
    }
    
    void runVisualizationSimulation(size_t steps) {
        std::cout << "=== Advanced Visualization Simulation ===" << std::endl;
        
        // Main simulation loop with visualization
        for (size_t i = 0; i < steps; ++i) {
            // Update brain
            brain_.step();
            
            // Update visualization
            viz_->update();
            
            // Render based on format
            viz_->render();
            
            // Log progress
            if (i % 100 == 0) {
                std::cout << "Step " << i << "/" << steps << std::endl;
            }
        }
        
        std::cout << "Visualization simulation completed" << std::endl;
        
        // Export final visualization
        exportVisualization();
        
        // Cleanup
        viz_->close();
    }
    
private:
    void configureWebVisualization() {
        std::cout << "Configuring web-based visualization..." << std::endl;
        
        // Configure for web output
        VisualizationInterface::VisualizationConfig config;
        config.backend = VisualizationInterface::VisualizationBackend::WEB_SOCKET;
        config.format = VisualizationInterface::VisualizationFormat::HTML;
        config.outputFile = "visualization/neural_simulation.html";
        config.updateRate = 60.0;
        config.enableColor = true;
        config.enableAnimations = true;
        config.enableInteractive = true;
        
        // Add custom renderers
        viz_->setCustomRenderer([this]() {
            render3DView();
            renderNetworkOverview();
            renderActivityHeatmap();
        });
        
        // Configure batch updates
        viz_->addBatchUpdate([this]() {
            updateNeuronVisualization();
        });
        
        viz_->addBatchUpdate([this]() {
            updateSynapseVisualization();
        });
        
        viz_->setConfig(config);
        
        std::cout << "Web visualization configured" << std::endl;
    }
    
    void configureBrain() {
        std::cout << "Configuring brain for visualization..." << std::endl;
        
        // Configure brain with visualization settings
        BrainConfig config;
        config.random_seed = 42;
        config.simulation_timestep = 0.001;
        config.neuron_count = 500;
        config.region_count = 2;
        config.connection_probability = 0.1;
        
        // Enable visualization-specific settings
        config.visualization_enabled = true;
        config.visualization_update_rate = 60.0;
        config.visualization_format = VisualizationInterface::VisualizationFormat::HTML;
        
        brain_ = Brain(config);
        
        // Create neurons and synapses
        createVisualNeurons();
        createVisualSynapses();
        
        std::cout << "Brain configured for visualization" << std::endl;
    }
    
    void createVisualNeurons() {
        std::cout << "Creating neurons for visualization..." << std::endl;
        
        // Create neurons with visual properties
        for (size_t i = 0; i < 100; ++i) {
            NeuronState state;
            state.membranePotential = -70.0f;
            state.threshold = -55.0f;
            state.firingState = FiringState::Resting;
            state.firingRate = 0.0f;
            state.refractoryPeriod = 2;
            state.leakConductance = 0.1f;
            state.restingPotential = -70.0f;
            state.resetPotential = -65.0f;
            state.adaptationVariable = 0.0f;
            state.lastSpikeTime = -1.0f;
            
            brain_.createNeuron(state);
        }
        
        std::cout << "Created " << brain_.getNeuronCount() << " neurons" << std::endl;
    }
    
    void createVisualSynapses() {
        std::cout << "Creating synapses for visualization..." << std::endl;
        
        // Get neuron IDs
        auto neurons = getAllNeuronIds();
        
        // Create synapses with visual properties
        for (size_t i = 0; i < neurons.size() - 1; ++i) {
            for (size_t j = i + 1; j < neurons.size() && j - i < 5; ++j) {
                SynapseState state;
                state.weight = 0.5f;
                state.delay = 1;
                state.type = SynapseType::Excitatory;
                state.plasticityFlags.hebbian = true;
                state.plasticityFlags.stdp = true;
                state.plasticityFlags.reward_modulated = true;
                state.eligibilityTrace = 0.0f;
                state.efficacy = 1.0f;
                
                brain_.createSynapse(neurons[i], neurons[j], state);
            }
        }
        
        std::cout << "Created " << brain_.getSynapseCount() << " synapses" << std::endl;
    }
    
    std::vector<NeuronId> getAllNeuronIds() {
        std::vector<NeuronId> ids;
        for (size_t i = 0; i < brain_.getNeuronCount(); ++i) {
            ids.push_back(static_cast<NeuronId>(i));
        }
        return ids;
    }
    
    void updateNeuronVisualization() {
        // Update neuron visualization data
        // This would involve calculating firing rates, membrane potentials, etc.
    }
    
    void updateSynapseVisualization() {
        // Update synapse visualization data
        // This would involve calculating synaptic weights, efficacies, etc.
    }
    
    void render3DView() {
        // Render 3D view of the neural network
        // This would generate 3D graphics for web output
        std::cout << "Rendering 3D view..." << std::endl;
    }
    
    void renderNetworkOverview() {
        // Render network overview
        // This would show connectivity patterns
        std::cout << "Rendering network overview..." << std::endl;
    }
    
    void renderActivityHeatmap() {
        // Render activity heatmap
        // This would show recent activity levels
        std::cout << "Rendering activity heatmap..." << std::endl;
    }
    
    void exportVisualization() {
        // Export visualization to file
        std::string outputFile = "visualization/neural_simulation_" + 
                               std::to_string(std::time(nullptr)) + ".html";
        
        viz_->exportVisualization(outputFile);
        std::cout << "Visualization exported to " << outputFile << std::endl;
    }
    
    std::unique_ptr<VisualizationInterface> viz_;
    Brain brain_;
};

int main() {
    AdvancedVisualizationSimulation sim;
    sim.runVisualizationSimulation(500);
    
    return 0;
}
```

## Example 5: Configuration-Driven Simulation

This example demonstrates configuration-driven simulation with multiple configuration files.

```cpp
#include "ConfigurationManager.hpp"
#include "Brain.hpp"
#include "VisualizationInterface.hpp"
#include <iostream>
#include <filesystem>

class ConfigurationDrivenSimulation {
public:
    ConfigurationDrivenSimulation(const std::string& configDir) {
        configDir_ = configDir;
        
        // List all configuration files
        listConfigurationFiles();
        
        // Load base configuration
        loadBaseConfiguration();
        
        // Load and validate all configurations
        loadAllConfigurations();
        
        // Create visualization
        viz_ = std::make_unique<VisualizationInterface>();
    }
    
    void runConfigurationDrivenSimulation() {
        std::cout << "=== Configuration-Driven Simulation ===" << std::endl;
        
        // Display configuration summary
        displayConfigurationSummary();
        
        // Run simulation with each configuration
        runAllConfigurations();
        
        // Generate comparison report
        generateComparisonReport();
        
        std::cout << "Configuration-driven simulation completed" << std::endl;
    }
    
private:
    void listConfigurationFiles() {
        std::cout << "Available configuration files:" << std::endl;
        
        for (const auto& entry : std::filesystem::directory_iterator(configDir_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".cfg") || filename.ends_with(".json")) {
                    std::cout << "  - " << filename << std::endl;
                }
            }
        }
    }
    
    void loadBaseConfiguration() {
        std::cout << "Loading base configuration..." << std::endl;
        
        // Load base configuration
        configManager_.loadConfiguration(configDir_ + "/base.cfg");
        
        // Set default values
        configManager_.setValue("simulation.max_steps", 10000);
        configManager_.setValue("simulation.time_limit", 1000.0);
        configManager_.setValue("visualization.enabled", true);
        configManager_.setValue("logging.level", "INFO");
        
        std::cout << "Base configuration loaded" << std::endl;
    }
    
    void loadAllConfigurations() {
        std::cout << "Loading all configurations..." << std::endl;
        
        // Load all configuration files
        for (const auto& entry : std::filesystem::directory_iterator(configDir_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".cfg") || filename.ends_with(".json")) {
                    // Skip base configuration (already loaded)
                    if (filename != "base.cfg") {
                        configManager_.loadConfiguration(entry.path().string());
                        std::cout << "Loaded: " << filename << std::endl;
                    }
                }
            }
        }
        
        std::cout << "All configurations loaded" << std::endl;
    }
    
    void displayConfigurationSummary() {
        std::cout << "\n=== Configuration Summary ===" << std::endl;
        
        // Display current configuration
        auto currentConfig = configManager_.getAllValues();
        
        for (const auto& pair : currentConfig) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    void runAllConfigurations() {
        std::cout << "\nRunning simulations with different configurations..." << std::endl;
        
        // Run simulation with current configuration
        runSingleConfiguration("current");
        
        // Run simulation with each configuration file
        for (const auto& entry : std::filesystem::directory_iterator(configDir_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".cfg") || filename.ends_with(".json")) {
                    // Load configuration
                    configManager_.loadConfiguration(entry.path().string());
                    
                    // Run simulation
                    runSingleConfiguration(filename);
                    
                    // Reload base configuration
                    configManager_.loadConfiguration(configDir_ + "/base.cfg");
                }
            }
        }
    }
    
    void runSingleConfiguration(const std::string& name) {
        std::cout << "\nRunning simulation with configuration: " << name << std::endl;
        
        // Create brain with current configuration
        BrainConfig brainConfig;
        brainConfig.random_seed = configManager_.getValue<int>("random_seed", 42);
        brainConfig.simulation_timestep = configManager_.getValue<double>("simulation_timestep", 0.001);
        brainConfig.neuron_count = configManager_.getValue<int>("neuron_count", 1000);
        brainConfig.region_count = configManager_.getValue<int>("region_count", 2);
        brainConfig.connection_probability = configManager_.getValue<double>("connection_probability", 0.1);
        
        brain_ = Brain(brainConfig);
        
        // Create neurons and synapses
        createNeuronsForConfig();
        createSynapsesForConfig();
        
        // Run simulation
        size_t maxSteps = configManager_.getValue<size_t>("max_simulation_steps", 1000);
        brain_.run(maxSteps);
        
        // Save results
        saveConfigurationResults(name);
        
        std::cout << "Completed simulation with configuration: " << name << std::endl;
    }
    
    void createNeuronsForConfig() {
        // Create neurons based on configuration
        size_t neuronCount = configManager_.getValue<size_t>("neuron_count", 1000);
        
        for (size_t i = 0; i < neuronCount; ++i) {
            NeuronState state;
            state.membranePotential = -70.0f;
            state.threshold = -55.0f;
            state.firingState = FiringState::Resting;
            state.firingRate = 0.0f;
            state.refractoryPeriod = configManager_.getValue<int>("neuron_refractory_period", 2);
            state.leakConductance = configManager_.getValue<double>("neuron_leak_conductance", 0.1f);
            state.restingPotential = -70.0f;
            state.resetPotential = -65.0f;
            
            brain_.createNeuron(state);
        }
    }
    
    void createSynapsesForConfig() {
        // Create synapses based on configuration
        size_t connectionProbability = static_cast<size_t>(
            configManager_.getValue<double>("connection_probability", 0.1) * 100);
        
        auto neurons = getAllNeuronIds();
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                if ((rand() % 100) < connectionProbability) {
                    SynapseState state;
                    state.weight = configManager_.getValue<double>("synapse_initial_weight", 0.1);
                    state.delay = configManager_.getValue<int>("synapse_initial_delay", 1);
                    state.type = SynapseType::Excitatory;
                    state.plasticityFlags.hebbian = configManager_.getValue<bool>("plasticity.hebbian", true);
                    state.plasticityFlags.stdp = configManager_.getValue<bool>("plasticity.stdp", true);
                    state.plasticityFlags.reward_modulated = configManager_.getValue<bool>("plasticity.reward_modulated", true);
                    
                    brain_.createSynapse(neurons[i], neurons[j], state);
                }
            }
        }
    }
    
    std::vector<NeuronId> getAllNeuronIds() {
        std::vector<NeuronId> ids;
        for (size_t i = 0; i < brain_.getNeuronCount(); ++i) {
            ids.push_back(static_cast<NeuronId>(i));
        }
        return ids;
    }
    
    void saveConfigurationResults(const std::string& name) {
        std::string filename = "results/configuration_" + name + "_" +
                               std::to_string(std::time(nullptr)) + ".brain";
        
        brain_.saveState(filename);
        std::cout << "Results saved to: " << filename << std::endl;
    }
    
    void generateComparisonReport() {
        std::cout << "\nGenerating comparison report..." << std::endl;
        
        // Generate HTML report comparing all configurations
        std::string reportFile = "comparison_report.html";
        std::ofstream report(reportFile);
        
        report << "<!DOCTYPE html>" << std::endl;
        report << "<html><head><title>Configuration Comparison Report</title></head>" << std::endl;
        report << "<body><h1>Configuration Comparison Report</h1>" << std::endl;
        report << "<table border='1'><tr><th>Configuration</th><th>Neurons</th><th>Synapses</th><th>Steps</th></tr>" << std::endl;
        
        // Add configuration results
        for (const auto& entry : std::filesystem::directory_iterator(configDir_)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.ends_with(".cfg") || filename.ends_with(".json")) {
                    // Load results from simulation
                    std::string resultFile = "results/configuration_" + filename + "_" +
                                          std::to_string(std::time(nullptr)) + ".brain";
                    
                    // Read and parse results (simplified)
                    report << "<tr><td>" << filename << "</td><td>" 
                           << brain_.getNeuronCount() << "</td><td>" 
                           << brain_.getSynapseCount() << "</td><td>" 
                           << brain_.getStepCount() << "</td></tr>" << std::endl;
                }
            }
        }
        
        report << "</table></body></html>" << std::endl;
        report.close();
        
        std::cout << "Comparison report saved to: " << reportFile << std::endl;
    }
    
    std::string configDir_;
    ConfigurationManager configManager_;
    std::unique_ptr<VisualizationInterface> viz_;
    Brain brain_;
};

int main() {
    // Create configuration-driven simulation
    ConfigurationDrivenSimulation sim("configs");
    
    // Run simulation
    sim.runConfigurationDrivenSimulation();
    
    return 0;
}
```

## Example 6: Advanced Error Handling and Recovery

This example demonstrates advanced error handling and recovery mechanisms.

```cpp
#include "ConfigurationValidator.hpp"
#include "ErrorRecovery.hpp"
#include "Brain.hpp"
#include <iostream>
#include <fstream>

class AdvancedErrorHandlingSimulation {
public:
    AdvancedErrorHandlingSimulation() {
        // Create error recovery system
        recovery_ = std::make_unique<ErrorRecovery>();
        
        // Add backup configurations
        recovery_->addBackupConfiguration("configs/backup1.cfg");
        recovery_->addBackupConfiguration("configs/backup2.cfg");
        
        // Add recovery strategies
        recovery_->addRecoveryStrategy("missing_file", 
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromMissingFile(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("invalid_format",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromInvalidFormat(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("invalid_value",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromInvalidValue(filepath, errorInfo);
            });
        
        // Initialize error recovery strategies
        initializeErrorRecoveryStrategies();
    }
    
    void runErrorHandlingSimulation() {
        std::cout << "=== Advanced Error Handling and Recovery Simulation ===" << std::endl;
        
        // Create configuration validator
        ConfigurationValidator validator;
        
        // Create error recovery strategies
        setupErrorRecoveryStrategies();
        
        // Load configuration with error handling
        std::string configFile = "configs/broken_config.cfg";
        std::cout << "Loading configuration: " << configFile << std::endl;
        
        // Validate configuration
        auto validationResult = validator.validateFile(configFile);
        
        if (!validationResult.isValid) {
            std::cout << "Configuration validation failed:" << std::endl;
            for (const auto& error : validationResult.errors) {
                std::cout << "  Error: " << error << std::endl;
            }
            
            // Attempt recovery
            std::cout << "Attempting recovery..." << std::endl;
            auto recoveryResult = attemptRecovery(configFile, validationResult);
            
            if (recoveryResult.success) {
                std::cout << "Recovery successful: " << recoveryResult.message << std::endl;
                
                // Continue with recovered configuration
                continueWithRecoveredConfiguration(recoveryResult.recoveredData);
            } else {
                std::cout << "Recovery failed: " << recoveryResult.message << std::endl;
                
                // Use fallback configuration
                useFallbackConfiguration();
            }
        } else {
            std::cout << "Configuration is valid" << std::endl;
            
            // Continue with valid configuration
            continueWithValidConfiguration();
        }
        
        std::cout << "Error handling simulation completed" << std::endl;
    }
    
private:
    void initializeErrorRecoveryStrategies() {
        std::cout << "Initializing error recovery strategies..." << std::endl;
        
        // Setup various recovery strategies
        setupConfigurationRecovery();
        setupFileRecovery();
        setupDataRecovery();
        
        std::cout << "Error recovery strategies initialized" << std::endl;
    }
    
    void setupErrorRecoveryStrategies() {
        // Setup recovery strategies
        recovery_->addRecoveryStrategy("corrupted_file", 
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromCorruptedFile(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("version_mismatch",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromVersionMismatch(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("permission_denied",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromPermissionDenied(filepath, errorInfo);
            });
    }
    
    ErrorRecovery::RecoveryResult recoverFromMissingFile(const std::string& filepath, 
                                                       const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from missing file..." << std::endl;
        
        // Try to find backup file
        for (const auto& backupFile : recovery_->getBackupConfigurations()) {
            std::ifstream backup(backupFile);
            if (backup.good()) {
                // Found backup file
                std::map<std::string, std::any> recoveredData;
                recoveredData["source"] = backupFile;
                recoveredData["status"] = "recovered_from_backup";
                
                return ErrorRecovery::RecoveryResult{
                    true,
                    "Recovered from backup file: " + backupFile,
                    recoveredData
                };
            }
        }
        
        return ErrorRecovery::RecoveryResult{
            false,
            "No backup file found for: " + filepath,
            {}
        };
    }
    
    ErrorRecovery::RecoveryResult recoverFromInvalidFormat(const std::string& filepath,
                                                          const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from invalid format..." << std::endl;
        
        // Try to fix common format issues
        std::ifstream input(filepath);
        if (!input.good()) {
            return ErrorRecovery::RecoveryResult{
                false,
                "Cannot read file: " + filepath,
                {}
            };
        }
        
        std::string content((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());
        
        // Fix common issues
        // Remove trailing whitespace
        size_t pos = 0;
        while ((pos = content.find_last_not_of(" \t\n\r")) != std::string::npos) {
            content.erase(pos + 1);
        }
        
        // Ensure each line ends with newline
        size_t lastNewline = content.find_last_not_of('\n');
        if (lastNewline != std::string::npos) {
            content.erase(lastNewline + 1);
        }
        
        // Try to parse fixed content
        // (In a real implementation, we would parse and fix the content)
        
        std::ofstream output(filepath);
        if (output.good()) {
            output << content;
        }
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Fixed invalid format in: " + filepath,
            {{"original_file", filepath}, {"fixed", true}}
        };
    }
    
    ErrorRecovery::RecoveryResult recoverFromInvalidValue(const std::string& filepath,
                                                         const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from invalid value..." << std::endl;
        
        // Apply default values
        std::map<std::string, std::any> recoveredData;
        recoveredData["action"] = "applied_defaults";
        
        // Define default values for common keys
        std::map<std::string, std::any> defaults = {
            {"random_seed", 42},
            {"simulation_timestep", 0.001},
            {"neuron_count", 1000},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"plasticity_learning_rate", 0.01},
            {"stdp_ltp_weight", 0.01},
            {"stdp_ltd_weight", 0.012},
            {"stdp_time_constant", 20.0},
            {"dopamine_baseline", 0.1},
            {"reward_discount_factor", 0.99},
            {"environment_name", "GridWorld"},
            {"environment_width", 10},
            {"environment_height", 10},
            {"log_level", "INFO"},
            {"log_to_file", false},
            {"log_filename", "nlm.log"},
            {"max_simulation_steps", 10000},
            {"simulation_time_limit", 0.0},
            {"visualization_enabled", false},
            {"visualization_update_rate", 30.0}
        };
        
        // Apply defaults to recovered data
        for (const auto& pair : defaults) {
            recoveredData[pair.first] = pair.second;
        }
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Applied default values for: " + filepath,
            recoveredData
        };
    }
    
    ErrorRecovery::RecoveryResult attemptRecovery(const std::string& filepath,
                                                  const ConfigurationValidator::ValidationResult& validationResult) {
        std::cout << "Attempting recovery..." << std::endl;
        
        // Classify errors for recovery
        std::map<std::string, int> errorCounts;
        for (const auto& error : validationResult.errors) {
            std::string errorType = classifyError(error);
            errorCounts[errorType]++;
        }
        
        // Attempt recovery for each error type
        for (const auto& pair : errorCounts) {
            std::string errorType = pair.first;
            
            if (recovery_->hasRecoveryStrategy(errorType)) {
                auto errorInfo = std::map<std::string, std::any>{
                    {"filepath", filepath},
                    {"validation_result", validationResult},
                    {"error_type", errorType},
                    {"error_count", pair.second}
                };
                
                auto recoveryResult = recovery_->recoverFromError(errorType, filepath, errorInfo);
                
                if (recoveryResult.success) {
                    return recoveryResult;
                }
            }
        }
        
        // If all recovery strategies failed, return fallback
        return ErrorRecovery::RecoveryResult{
            false,
            "All recovery strategies failed, using fallback",
            {{"action", "fallback"}}
        };
    }
    
    std::string classifyError(const std::string& error) {
        // Classify error for recovery
        std::string lowerError = error;
        std::transform(lowerError.begin(), lowerError.end(), lowerError.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        
        if (lowerError.find("missing file") != std::string::npos ||
            lowerError.find("not found") != std::string::npos) {
            return "missing_file";
        } else if (lowerError.find("invalid format") != std::string::npos ||
                   lowerError.find("syntax error") != std::string::npos) {
            return "invalid_format";
        } else if (lowerError.find("must be") != std::string::npos &&
                   (lowerError.find("int") != std::string::npos ||
                    lowerError.find("float") != std::string::npos ||
                    lowerError.find("number") != std::string::npos)) {
            return "invalid_value";
        } else if (lowerError.find("duplicate") != std::string::npos) {
            return "duplicate_key";
        } else if (lowerError.find("required") != std::string::npos) {
            return "missing_required";
        } else {
            return "unknown";
        }
    }
    
    void setupConfigurationRecovery() {
        // Setup configuration recovery
        std::cout << "Setting up configuration recovery..." << std::endl;
        
        // Add recovery for configuration-specific errors
        recovery_->addRecoveryStrategy("missing_random_seed",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromMissingRandomSeed(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("invalid_connection_probability",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromInvalidConnectionProbability(filepath, errorInfo);
            });
        
        std::cout << "Configuration recovery setup completed" << std::endl;
    }
    
    ErrorRecovery::RecoveryResult recoverFromMissingRandomSeed(const std::string& filepath,
                                                             const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from missing random seed..." << std::endl;
        
        // Apply default random seed
        std::map<std::string, std::any> recoveredData;
        recoveredData["random_seed"] = 42;
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Applied default random seed",
            recoveredData
        };
    }
    
    ErrorRecovery::RecoveryResult recoverFromInvalidConnectionProbability(const std::string& filepath,
                                                                         const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from invalid connection probability..." << std::endl;
        
        // Apply valid connection probability
        std::map<std::string, std::any> recoveredData;
        recoveredData["connection_probability"] = 0.1;
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Applied valid connection probability",
            recoveredData
        };
    }
    
    void setupFileRecovery() {
        // Setup file recovery
        std::cout << "Setting up file recovery..." << std::endl;
        
        // Add recovery for file-related errors
        recovery_->addRecoveryStrategy("permission_denied",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromPermissionDenied(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("disk_full",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromDiskFull(filepath, errorInfo);
            });
        
        std::cout << "File recovery setup completed" << std::endl;
    }
    
    ErrorRecovery::RecoveryResult recoverFromPermissionDenied(const std::string& filepath,
                                                             const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from permission denied..." << std::endl;
        
        // Try to write to alternative location
        std::string alternativePath = "/tmp/config_backup.cfg";
        std::ofstream alternative(alternativePath);
        if (alternative.good()) {
            alternative.close();
            
            std::map<std::string, std::any> recoveredData;
            recoveredData["alternative_path"] = alternativePath;
            recoveredData["reason"] = "permission_denied_original";
            
            return ErrorRecovery::RecoveryResult{
                true,
                "Wrote to alternative path",
                recoveredData
            };
        }
        
        return ErrorRecovery::RecoveryResult{
            false,
            "Cannot write to alternative path",
            {}
        };
    }
    
    ErrorRecovery::RecoveryResult recoverFromDiskFull(const std::string& filepath,
                                                     const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from disk full..." << std::endl;
        
        // Try to write to a different directory
        std::string alternativePath = "/tmp/nlm_config.cfg";
        std::ofstream alternative(alternativePath);
        if (alternative.good()) {
            alternative.close();
            
            std::map<std::string, std::any> recoveredData;
            recoveredData["alternative_path"] = alternativePath;
            recoveredData["reason"] = "disk_full_original";
            
            return ErrorRecovery::RecoveryResult{
                true,
                "Wrote to alternative path (disk full)",
                recoveredData
            };
        }
        
        return ErrorRecovery::RecoveryResult{
            false,
            "Cannot write to alternative path (disk full)",
            {}
        };
    }
    
    void setupDataRecovery() {
        // Setup data recovery
        std::cout << "Setting up data recovery..." << std::endl;
        
        // Add recovery for data-related errors
        recovery_->addRecoveryStrategy("data_corruption",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromDataCorruption(filepath, errorInfo);
            });
        
        recovery_->addRecoveryStrategy("incomplete_data",
            [this](const std::string& filepath, const std::map<std::string, std::any>& errorInfo) {
                return recoverFromIncompleteData(filepath, errorInfo);
            });
        
        std::cout << "Data recovery setup completed" << std::endl;
    }
    
    ErrorRecovery::RecoveryResult recoverFromDataCorruption(const std::string& filepath,
                                                           const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from data corruption..." << std::endl;
        
        // Try to restore from backup
        std::string backupPath = "configs/backup_config.cfg";
        std::ifstream backup(backupPath);
        if (backup.good()) {
            std::map<std::string, std::any> recoveredData;
            recoveredData["source"] = backupPath;
            recoveredData["reason"] = "data_corruption_restored";
            
            return ErrorRecovery::RecoveryResult{
                true,
                "Restored from backup (data corruption)",
                recoveredData
            };
        }
        
        // Try to create minimal valid configuration
        std::map<std::string, std::any> minimalConfig;
        minimalConfig["random_seed"] = 42;
        minimalConfig["simulation_timestep"] = 0.001;
        minimalConfig["neuron_count"] = 100;
        minimalConfig["region_count"] = 1;
        minimalConfig["connection_probability"] = 0.1;
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Created minimal valid configuration",
            minimalConfig
        };
    }
    
    ErrorRecovery::RecoveryResult recoverFromIncompleteData(const std::string& filepath,
                                                            const std::map<std::string, std::any>& errorInfo) {
        std::cout << "Recovering from incomplete data..." << std::endl;
        
        // Try to complete incomplete configuration
        std::map<std::string, std::any> completedConfig;
        completedConfig["random_seed"] = 42;
        completedConfig["simulation_timestep"] = 0.001;
        completedConfig["neuron_count"] = 1000;
        completedConfig["region_count"] = 2;
        completedConfig["connection_probability"] = 0.1;
        completedConfig["plasticity_learning_rate"] = 0.01;
        completedConfig["stdp_ltp_weight"] = 0.01;
        completedConfig["stdp_ltd_weight"] = 0.012;
        completedConfig["stdp_time_constant"] = 20.0;
        completedConfig["dopamine_baseline"] = 0.1;
        completedConfig["reward_discount_factor"] = 0.99;
        completedConfig["environment_name"] = "GridWorld";
        completedConfig["environment_width"] = 10;
        completedConfig["environment_height"] = 10;
        completedConfig["log_level"] = "INFO";
        completedConfig["log_to_file"] = false;
        completedConfig["log_filename"] = "nlm.log";
        completedConfig["max_simulation_steps"] = 10000;
        completedConfig["simulation_time_limit"] = 0.0;
        completedConfig["visualization_enabled"] = false;
        completedConfig["visualization_update_rate"] = 30.0;
        
        return ErrorRecovery::RecoveryResult{
            true,
            "Completed incomplete configuration",
            completedConfig
        };
    }
    
    void continueWithRecoveredConfiguration(const std::map<std::string, std::any>& recoveredData) {
        std::cout << "Continuing with recovered configuration..." << std::endl;
        
        // Apply recovered configuration
        for (const auto& pair : recoveredData) {
            std::cout << "  Applied: " << pair.first << " = " << pair.second << std::endl;
        }
        
        // Create brain with recovered configuration
        BrainConfig brainConfig;
        brainConfig.random_seed = getFromRecoveredData<int>(recoveredData, "random_seed", 42);
        brainConfig.simulation_timestep = getFromRecoveredData<double>(recoveredData, "simulation_timestep", 0.001);
        brainConfig.neuron_count = getFromRecoveredData<size_t>(recoveredData, "neuron_count", 1000);
        brainConfig.region_count = getFromRecoveredData<size_t>(recoveredData, "region_count", 2);
        brainConfig.connection_probability = getFromRecoveredData<double>(recoveredData, "connection_probability", 0.1);
        
        brain_ = Brain(brainConfig);
        
        std::cout << "Recovered configuration applied successfully" << std::endl;
    }
    
    void useFallbackConfiguration() {
        std::cout << "Using fallback configuration..." << std::endl;
        
        // Create brain with fallback configuration
        BrainConfig brainConfig;
        brainConfig.random_seed = 42;
        brainConfig.simulation_timestep = 0.001;
        brainConfig.neuron_count = 1000;
        brainConfig.region_count = 2;
        brainConfig.connection_probability = 0.1;
        
        brain_ = Brain(brainConfig);
        
        std::cout << "Fallback configuration applied" << std::endl;
    }
    
    void continueWithValidConfiguration() {
        std::cout << "Continuing with valid configuration..." << std::endl;
        
        // Create brain with valid configuration
        BrainConfig brainConfig;
        brainConfig.random_seed = 42;
        brainConfig.simulation_timestep = 0.001;
        brainConfig.neuron_count = 1000;
        brainConfig.region_count = 2;
        brainConfig.connection_probability = 0.1;
        
        brain_ = Brain(brainConfig);
        
        std::cout << "Valid configuration applied successfully" << std::endl;
    }
    
    template<typename T>
    T getFromRecoveredData(const std::map<std::string, std::any>& data, const std::string& key, T defaultValue) {
        auto it = data.find(key);
        if (it != data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    std::unique_ptr<ErrorRecovery> recovery_;
    Brain brain_;
};

int main() {
    AdvancedErrorHandlingSimulation sim;
    sim.runErrorHandlingSimulation();
    
    return 0;
}
```

## Summary

These examples demonstrate the advanced capabilities of the Neural Simulation Project, including:

1. **Basic Neural Simulation**: Simple simulation with standard settings
2. **Advanced Neuromodulation**: Complex neuromodulation with custom dopamine dynamics
3. **Performance Profiling**: Performance optimization and profiling tools
4. **Advanced Visualization**: Multi-backend visualization with 3D rendering
5. **Configuration-Driven Simulation**: Configuration-driven simulation with multiple configurations
6. **Advanced Error Handling**: Comprehensive error handling and recovery mechanisms

Each example showcases different aspects of the framework's capabilities and demonstrates best practices for using the NLM library in production scenarios.

The examples provide a comprehensive foundation for developing applications with the Neural Simulation Project, covering everything from basic usage to advanced research scenarios.
