# NLM (Neural Learning Machine) - Advanced Usage Guide
# Phase 6: Comprehensive Usage Examples and Best Practices

This document provides comprehensive guidance for using the NLM system, from basic setup to advanced usage patterns.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Advanced Configuration Patterns](#advanced-configuration-patterns)
3. [Brain Development and Training](#brain-development-and-training)
4. [Memory System Integration](#memory-system-integration)
5. [Neuromodulation and Learning](#neuromodulation-and-learning)
6. [Cross-System Coordination](#cross-system-coordination)
7. [Performance Optimization](#performance-optimization)
8. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
9. [Python API Examples](#python-api-examples)
10. [Batch Processing and Automation](#batch-processing-and-automation)
11. [Custom Component Integration](#custom-component-integration)
12. [Best Practices and Patterns](#best-practices-and-patterns)

---

# 1. Quick Start for Beginners

## Basic Brain Setup

### Step 1: Create a Brain Configuration

```cpp
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

// Create default configuration
auto config = std::make_shared<nlm::Config>();

// Set up basic neural simulation parameters
config->set("neuron_count", 1000);
config->set("region_count", 1);
config->set("simulation_timestep", 0.001);
config->set("random_seed", 42);
config->set("connection_probability", 0.15f);

// Initialize brain
auto brain = std::make_shared<nlm::Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Failed to initialize brain!" << std::endl;
    return 1;
}
```

### Step 2: Run Simulation Loop

```cpp
// Main simulation loop
for (size_t step = 0; step < 1000; ++step) {
    double time = step * 0.001;
    brain->step(step, time);
    
    // Inject sensory input periodically
    if (step % 100 == 0) {
        auto sensoryInput = std::make_shared<nlm::Vision>(10, 10, 3);
        // ... add visual data ...
        brain->receiveSensoryInput(sensoryInput);
    }
}
```

## Python Quick Start

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure neural simulation
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i, i * 0.001)
    if i % 10 == 0:
        # Inject sensory input
        vision_data = [0.0] * 300  # 10x10x3
        sensory_input = pynlm.Vision(10, 10, 3)
        sensory_input.setData(vision_data)
        brain.receiveSensoryInput(sensory_input)

print("Simulation completed successfully!")
```

---

# 2. Advanced Configuration Patterns

## Fluent Configuration API

```cpp
// Using Pythonic dictionary interface (available in C++ via template)
nlm::Config config;

// Method chaining for fluent API
config.from_dict({
    {"neuron_count", 1000},
    {"region_count", 1},
    {"simulation_timestep", 0.001},
    {"random_seed", 42},
    {"stdp_ltp_weight", 0.02f},
    {"stdp_ltd_weight", 0.015f},
    {"connection_probability", 0.15f},
    {"enable_curiosity", true},
    {"enable_reward_modulation", true},
    {"enable_development", true}
});

// Alternative: factory methods
auto config = nlm::Config::neural_config();  // Sensible defaults for neural simulation
auto world_config = nlm::Config::world_config();  // For world simulation
auto agent_config = nlm::Config::agent_config();  // For agent behavior
```

## Configuration Validation and Schema

```cpp
// Validate configuration against schema
if (!config.validate("neural_simulation_schema.json")) {
    std::cerr << "Configuration validation failed!" << std::endl;
    return 1;
}

// Check configuration type information
std::string type = config.getType("neuron_count");
assert(type == "int");

// Get configuration statistics
auto stats = config.getStats();
std::cout << "Configuration has " << stats.totalEntries << " entries" << std::endl;
```

## Configuration Merging and Inheritance

```cpp
// Create base configuration
nlm::Config baseConfig;
baseConfig.set("neuron_count", 100);
baseConfig.set("region_count", 1);
baseConfig.set("simulation_timestep", 0.001);

// Create specialized configuration
nlm::Config specializedConfig;
specializedConfig.set("neuron_count", 500);  // Override
// ... other settings ...

// Merge configurations (specialized overrides base)
baseConfig.merge(specializedConfig);
```

---

# 3. Brain Development and Training

## Basic Training Loop

```cpp
void trainBrain(std::shared_ptr<nlm::Brain> brain, size_t steps) {
    for (size_t step = 0; step < steps; ++step) {
        double time = step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        // Generate random input based on step
        size_t neuronCount = brain->getTotalNeuronCount();
        for (size_t i = 0; i < neuronCount / 10; ++i) {
            size_t neuronIndex = (step + i) % neuronCount;
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        // Execute simulation step
        brain->step(step, time);
        
        // Log progress periodically
        if (step % 100 == 0) {
            brain->logStatus();
        }
    }
}
```

## Development-Stage Training

```cpp
void developBrain(std::shared_ptr<nlm::AgentBrain> agentBrain, double totalTime) {
    double timestep = 0.001;
    double elapsed = 0.0;
    
    while (elapsed < totalTime) {
        // Update development system
        agentBrain->updateDevelopment(timestep);
        
        // Get current developmental stage
        auto stage = agentBrain->getDevelopmentalStage();
        
        // Adjust learning rates based on developmental stage
        if (stage == nlm::DevelopmentalStage::CriticalPeriod) {
            // Higher plasticity during critical period
            agentBrain->getBrain()->getConfig()->set("stdp_ltp_weight", 0.05f);
        } else if (stage == nlm::DevelopmentalStage::Maturation) {
            // Moderate plasticity during maturation
            agentBrain->getBrain()->getConfig()->set("stdp_ltp_weight", 0.02f);
        }
        
        elapsed += timestep;
        
        // Process any available sensory input
        // ... (your sensory processing logic) ...
    }
}
```

## Learning with Neuromodulation

```cpp
void trainWithNeuromodulation(std::shared_ptr<nlm::AgentBrain> agentBrain, 
                             const std::vector<float>& rewards) {
    // Enable all learning systems
    agentBrain->enable_all_systems();
    
    for (size_t i = 0; i < rewards.size(); ++i) {
        double timestep = 0.001;
        
        // Process sensory input (simulate environment interaction)
        // ... (your sensory processing) ...
        
        // Apply reward modulation
        float predictedReward = predictReward(agentBrain);  // Your prediction function
        agentBrain->applyRewardModulation(rewards[i], predictedReward);
        
        // Update curiosity-driven exploration
        if (agentBrain->getCuriosityLevel() < 0.5) {
            agentBrain->getCuriosity()->signalNovelty(0.1f);  // Explore novel stimuli
        }
        
        // Update development
        agentBrain->updateDevelopment(timestep);
    }
}
```

---

# 4. Memory System Integration

## Working Memory Usage

```cpp
void demonstrateWorkingMemory(std::shared_ptr<nlm::Brain> brain) {
    auto workingMemory = brain->getWorkingMemory();
    
    // Store temporary information
    workingMemory->store("current_goal", "find_food");
    workingMemory->store("urgency", 0.8f);
    
    // Access and use working memory
    std::string goal = workingMemory->getAs<std::string>("current_goal", "none");
    float urgency = workingMemory->getAs<float>("urgency", 0.0f);
    
    // Working memory decays over time
    // ... (your decay logic) ...
}
```

## Episodic Memory Integration

```cpp
void demonstrateEpisodicMemory(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    auto episodicMemory = agentBrain->getBrain()->getEpisodicMemory();
    
    // Record an experience
    nlm::Experience experience;
    experience.timestamp = agentBrain->getBrain()->getTotalSpikeCount();
    experience.sensoryInput = getCurrentSensoryInput();  // Your function
    experience.action = getCurrentAction();  // Your function
    experience.reward = getCurrentReward();  // Your function
    
    episodicMemory->recordExperience(experience);
    
    // Replay important experiences
    episodicMemory->replayImportantEpisodes(10);  // Replay last 10 important episodes
    
    // Learn from replayed experiences
    episodicMemory->learnFromReplay();
}
```

## Associative Memory Integration

```cpp
void demonstrateAssociativeMemory(std::shared_ptr<nlm::Brain> brain) {
    auto associativeMemory = brain->getAssociativeMemory();
    
    // Form associations between patterns
    std::vector<float> pattern1 = {1.0f, 0.5f, 0.3f, 0.8f, 0.2f};
    std::vector<float> pattern2 = {0.9f, 0.4f, 0.7f, 0.1f, 0.6f};
    
    associativeMemory->formAssociation(pattern1, pattern2, 0.8f);
    
    // Retrieve associated patterns
    std::vector<float> retrieved = associativeMemory->retrieveAssociation(pattern1);
    
    // Use associations for prediction
    float predictedValue = associativeMemory->predictOutcome(pattern1);
}
```

---

# 5. Neuromodulation and Learning

## Dopamine-Based Learning

```cpp
void demonstrateDopamineLearning(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    auto dopamine = agentBrain->getDopamine();
    
    // Signal reward prediction errors
    float actualReward = getActualReward();  // Your function
    float predictedReward = agentBrain->getPredictionError();
    float rewardPredictionError = actualReward - predictedReward;
    
    dopamine->signalRewardPredictionError(rewardPredictionError);
    
    // Update synaptic weights based on dopamine signals
    dopamine->updatePlasticity();
}
```

## Curiosity-Driven Exploration

```cpp
void demonstrateCuriosityLearning(std::shared_ptr<nlm::AgentBrain> agentBrain, 
                                 std::shared_ptr<nlm::SimpleWorld> world) {
    auto curiosity = agentBrain->getCuriosity();
    auto novelty = agentBrain->getNovelty();
    
    // Monitor novelty in environment
    auto sensoryPercept = world->getSensoryPercept();
    float noveltyLevel = novelty->detectNovelty(sensoryPercept->getVision());
    
    // Use curiosity to drive exploration
    curiosity->signalNovelty(noveltyLevel);
    
    // High curiosity leads to more exploration
    if (curiosity->getLevel() > 0.7f) {
        // Encourage exploration behavior
        auto explorationAction = world->getExplorationAction();
        // ... execute exploration action ...
    }
}
```

## Novelty Detection

```cpp
void demonstrateNoveltyDetection(std::shared_ptr<nlm::SimpleWorld> world,
                                std::shared_ptr<nlm::AgentBrain> agentBrain) {
    auto novelty = std::make_shared<nlm::Novelty>();
    
    // Create sensory patterns from world state
    auto percept = world->getSensoryPercept();
    
    // Detect novel patterns
    float noveltyScore = novelty->calculateNovelty(percept->getVision());
    
    // Store novelty information for learning
    novelty->updateNoveltyHistory(noveltyScore);
    
    // Use novelty to guide attention and learning
    if (noveltyScore > 0.8f) {
        // Novel stimulus - allocate more attentional resources
        agentBrain->applyRewardModulation(1.0f, 0.0f);  // Strong learning signal
    }
}
```

---

# 6. Cross-System Coordination

## Coordinated Memory and Prediction

```cpp
void demonstrateMemoryPredictionIntegration(std::shared_ptr<nlm::Brain> brain) {
    auto predictionSystem = brain->getPredictionSystem();
    auto episodicMemory = brain->getEpisodicMemory();
    
    // Generate predictions based on current state
    auto prediction = predictionSystem->generatePrediction(brain->getTotalSpikeCount());
    
    // Check memory for similar past experiences
    auto similarExperiences = episodicMemory->findSimilarExperiences(prediction);
    
    // Integrate prediction with memory
    if (!similarExperiences.empty()) {
        // Use past experiences to refine prediction
        float refinedPrediction = predictionSystem->integrateWithMemory(prediction, similarExperiences);
        
        // Apply prediction error signal
        float predictionError = calculatePredictionError(refinedPrediction);
        predictionSystem->applyErrorSignal(predictionError);
    }
}
```

## Neuromodulation and Memory Integration

```cpp
void demonstrateNeuromodulationMemoryIntegration(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    auto dopamine = agentBrain->getDopamine();
    auto episodicMemory = agentBrain->getBrain()->getEpisodicMemory();
    
    // Use dopamine signals to prioritize memory consolidation
    float dopamineLevel = dopamine->getLevel();
    
    if (dopamineLevel > 0.5f) {
        // High dopamine - consolidate important memories
        episodicMemory->prioritizeConsolidation();
    } else {
        // Low dopamine - normal consolidation
        episodicMemory->normalConsolidation();
    }
    
    // Use prediction error to strengthen memory traces
    float predictionError = agentBrain->getPredictionError();
    episodicMemory->strengthenBasedOnError(predictionError);
}
```

## Development and Learning Integration

```cpp
void demonstrateDevelopmentLearningIntegration(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    auto developmentSystem = agentBrain->getBrain()->getDevelopmentSystem();
    auto workingMemory = agentBrain->getBrain()->getWorkingMemory();
    
    // Get current developmental stage
    auto stage = developmentSystem->getCurrentStage();
    
    // Adjust learning based on developmental stage
    if (stage == nlm::DevelopmentalStage::CriticalPeriod) {
        // High learning plasticity
        workingMemory->increaseLearningRate(0.5f);
        
        // Rapid memory formation
        workingMemory->speedUpConsolidation();
    } else if (stage == nlm::DevelopmentalStage::Adult) {
        // Stable learning
        workingMemory->normalizeLearningRate();
        
        // Efficient memory retrieval
        workingMemory->optimizeRetrievalSpeed();
    }
}
```

---

# 7. Performance Optimization

## Efficient Memory Usage

```cpp
void optimizeMemoryUsage(std::shared_ptr<nlm::Brain> brain) {
    // Use memory pools for better memory management
    auto& memoryPool = brain->getMemoryPool();
    
    // Allocate neurons from pool
    auto neuron = memoryPool.allocateNeuron();
    
    // Return neurons to pool when no longer needed
    memoryPool.deallocateNeuron(neuron);
}
```

## Parallel Processing Optimization

```cpp
void optimizeParallelProcessing(std::shared_ptr<nlm::Brain> brain) {
    // Enable SIMD optimization
    brain->enableSIMDProcessing(true);
    
    // Use parallel event processing
    brain->enableParallelEventProcessing(true);
    
    // Configure thread pool for neural computation
    auto& threadPool = brain->getThreadPool();
    threadPool.setThreadCount(std::thread::hardware_concurrency());
}
```

## Checkpointing for Large Simulations

```cpp
void optimizeLargeScaleSimulation(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    // Enable checkpointing for long simulations
    agentBrain->enableCheckpointing(true);
    
    // Configure checkpoint interval
    agentBrain->setCheckpointInterval(1000);  // Every 1000 steps
    
    // Set up checkpoint storage
    agentBrain->configureCheckpointStorage(
        "/path/to/checkpoints",
        nlm::CompressionLevel::Medium,
        100  // Keep 100 checkpoints
    );
    
    // Main simulation loop with checkpointing
    for (size_t step = 0; step < totalSteps; ++step) {
        // Run simulation step
        agentBrain->step(step);
        
        // Save checkpoint periodically
        if (step % checkpointInterval == 0) {
            agentBrain->saveCheckpoint("checkpoint_" + std::to_string(step));
        }
    }
}
```

---

# 8. Debugging and Troubleshooting

## Basic Debugging

```cpp
void debugBrainSimulation(std::shared_ptr<nlm::Brain> brain) {
    // Enable verbose logging
    brain->getConfig()->set("log_level", "debug");
    
    // Run simulation with detailed output
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Log detailed status
        brain->logStatus();
        
        // Check for errors
        auto status = brain->checkForErrors();
        if (!status.isHealthy()) {
            std::cerr << "Error detected: " << status.getErrorMessage() << std::endl;
            // ... handle error ...
        }
    }
}
```

## Memory Leak Detection

```cpp
void detectMemoryLeaks(std::shared_ptr<nlm::Brain> brain) {
    // Enable memory tracking
    brain->enableMemoryTracking(true);
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Check for memory leaks
    auto memoryReport = brain->getMemoryReport();
    
    if (memoryReport.hasLeaks()) {
        std::cout << "Memory leaks detected:" << std::endl;
        for (const auto& leak : memoryReport.getLeaks()) {
            std::cout << "  - " << leak.description << " (" << leak.size << " bytes)" << std::endl;
        }
    }
}
```

## Performance Profiling

```cpp
void profileBrainPerformance(std::shared_ptr<nlm::Brain> brain) {
    // Enable performance profiling
    brain->enablePerformanceProfiling(true);
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < totalSteps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Get performance statistics
    auto perfStats = brain->getPerformanceStatistics();
    
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "  Steps per second: " << (totalSteps / (duration.count() / 1000.0)) << std::endl;
    std::cout << "  Average step time: " << (duration.count() / totalSteps) << " ms" << std::endl;
}
```

---

# 9. Python API Examples

## Complete Python Example

```python
#!/usr/bin/env python3
"""
Complete example demonstrating NLM Python API usage.
"""

import pynlm
import time
import numpy as np

def main():
    print("=== NLM Python API Complete Example ===\n")
    
    # 1. Create configuration with Pythonic dictionary interface
    print("1. Creating neural configuration...")
    config = pynlm.createDefaultConfig()
    
    # Use Pythonic dictionary for configuration
    config.update({
        "neuron_count": 500,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "connection_probability": 0.15,
        "enable_curiosity": True,
        "enable_reward_modulation": True,
        "enable_development": True
    })
    
    # 2. Create brain and agent
    print("2. Creating brain and agent...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
    world.setAgentStart(50, 50)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # 3. Run simulation loop
    print("3. Running simulation...")
    for step in range(100):
        # Get current sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input (simulate environment interaction)
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command from brain activity
        motor_command = agent_brain.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_command, time.time())
        
        # Update world
        world.update(0.001)
        
        # Update agent development
        agent_brain.updateDevelopment(0.001)
        
        # Log progress
        if step % 10 == 0:
            stats = brain.get_simulation_stats()
            agent_stats = agent_brain.get_agent_stats()
            print(f"Step {step}: "
                  f"Spikes: {stats['total_spikes']}, "
                  f"Firing: {agent_stats['firing_rate']:.2f} Hz, "
                  f"Curiosity: {agent_stats['curiosity_level']:.2f}")
    
    # 4. Demonstrate advanced features
    print("4. Demonstrating advanced features...")
    
    # Show brain statistics
    brain_stats = brain.get_simulation_stats()
    print(f"\nBrain Statistics:")
    print(f"  Total neurons: {brain_stats['total_neurons']}")
    print(f"  Total synapses: {brain_stats['total_synapses']}")
    print(f"  Total spikes: {brain_stats['total_spikes']}")
    print(f"  Firing rate: {brain_stats['firing_rate']:.2f} Hz")
    
    # Show agent statistics
    agent_stats = agent_brain.get_agent_stats()
    print(f"\nAgent Statistics:")
    print(f"  Neuromodulation level: {agent_stats['neuromodulation_level']:.2f}")
    print(f"  Curiosity level: {agent_stats['curiosity_level']:.2f}")
    print(f"  Novelty level: {agent_stats['novelty_level']:.2f}")
    print(f"  Prediction error: {agent_stats['prediction_error']:.2f}")
    
    # 5. Context manager example
    print("5. Demonstrating context manager...")
    with brain:
        brain.step(200, 0.2)
        print("  Brain context manager working correctly")
    
    print("\n=== Example completed successfully! ===")

if __name__ == "__main__":
    main()
```

## Batch Processing Example

```python
#!/usr/bin/env python3
"""
Batch processing example for running multiple simulations.
"""

import pynlm
import multiprocessing
import time

def run_single_simulation(config_dict, simulation_id):
    """Run a single simulation with given configuration."""
    print(f"Starting simulation {simulation_id}")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.update(config_dict)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(50):
        brain.step(step, step * 0.001)
    
    # Get results
    stats = brain.get_simulation_stats()
    stats['simulation_id'] = simulation_id
    
    print(f"Completed simulation {simulation_id}: {stats['total_spikes']} spikes")
    return stats

def main():
    print("=== NLM Batch Processing Example ===\n")
    
    # Define different configurations
    configs = [
        {"neuron_count": 100, "log_level": "info"},
        {"neuron_count": 200, "log_level": "warning"},
        {"neuron_count": 300, "log_level": "error"}
    ]
    
    # Run simulations sequentially
    print("Running simulations sequentially...")
    start_time = time.time()
    
    results = []
    for i, config_dict in enumerate(configs):
        result = run_single_simulation(config_dict, i)
        results.append(result)
    
    sequential_time = time.time() - start_time
    print(f"Sequential time: {sequential_time:.2f} seconds")
    
    # Run simulations in parallel (if available)
    if hasattr(multiprocessing, 'Pool'):
        print("\nRunning simulations in parallel...")
        start_time = time.time()
        
        with multiprocessing.Pool(3) as pool:
            parallel_results = pool.starmap(
                run_single_simulation,
                [(config, i) for i, config in enumerate(configs)]
            )
        
        parallel_time = time.time() - start_time
        print(f"Parallel time: {parallel_time:.2f} seconds")
        print(f"Speedup: {sequential_time / parallel_time:.2f}x")
    
    # Aggregate results
    print("\n=== Results Summary ===")
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_firing_rate = sum(r['firing_rate'] for r in results) / len(results)
    
    print(f"Total spikes across all simulations: {total_spikes}")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")

if __name__ == "__main__":
    main()
```

---

# 10. Batch Processing and Automation

## Automated Experiment Runner

```cpp
class ExperimentRunner {
public:
    ExperimentRunner(std::string outputDir) : outputDir(outputDir) {}
    
    void runExperiments(const std::vector<ExperimentConfig>& configs) {
        for (size_t i = 0; i < configs.size(); ++i) {
            std::cout << "Running experiment " << i + 1 << "/" << configs.size() << std::endl;
            
            runSingleExperiment(configs[i], i);
            
            // Save results
            saveResults(i);
            
            // Wait between experiments
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
private:
    void runSingleExperiment(const ExperimentConfig& config, int experimentId) {
        // Create configuration
        auto nlmConfig = std::make_shared<nlm::Config>();
        for (const auto& setting : config.settings) {
            nlmConfig->set(setting.first, setting.second);
        }
        
        // Create and initialize brain
        auto brain = std::make_shared<nlm::Brain>(nlmConfig);
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Run experiment
        runExperiment(brain, config);
        
        // Log experiment completion
        std::cout << "  Experiment " << experimentId << " completed" << std::endl;
    }
    
    void runExperiment(std::shared_ptr<nlm::Brain> brain, const ExperimentConfig& config) {
        for (size_t step = 0; step < config.steps; ++step) {
            double time = step * config.timestep;
            
            // Inject stimuli based on experiment phase
            injectStimulation(brain, step, config);
            
            // Execute simulation step
            brain->step(step, time);
            
            // Record metrics
            recordMetrics(brain, step, config);
        }
    }
    
    void injectStimulation(std::shared_ptr<nlm::Brain> brain, size_t step, 
                          const ExperimentConfig& config) {
        switch (config.phase) {
            case ExperimentPhase::Baseline:
                // No stimulation
                break;
            case ExperimentPhase::Excitatory:
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
                break;
            case ExperimentPhase::Inhibitory:
                brain->injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -20.0f);
                break;
            case ExperimentPhase::Pattern:
                // Inject specific pattern
                injectPatternStimulus(brain, step, config);
                break;
        }
    }
    
    void saveResults(int experimentId) {
        std::string filename = outputDir + "/experiment_" + std::to_string(experimentId) + ".json";
        // Save results to file
    }
    
    std::string outputDir;
};
```

## Parameter Sweeps

```cpp
void runParameterSweep() {
    // Define parameter ranges
    std::vector<int> neuronCounts = {100, 200, 500, 1000};
    std::vector<double> timesteps = {0.001, 0.005, 0.01};
    std::vector<double> connectionProbabilities = {0.1, 0.2, 0.3};
    
    std::cout << "Running parameter sweep..." << std::endl;
    std::cout << "Neuron counts: " << neuronCounts.size() << std::endl;
    std::cout << "Timesteps: " << timesteps.size() << std::endl;
    std::cout << "Connection probabilities: " << connectionProbabilities.size() << std::endl;
    
    int totalExperiments = neuronCounts.size() * timesteps.size() * connectionProbabilities.size();
    int completed = 0;
    
    for (int neuronCount : neuronCounts) {
        for (double timestep : timesteps) {
            for (double connectionProb : connectionProbabilities) {
                // Run experiment with current parameters
                runSingleParameterSet(neuronCount, timestep, connectionProb);
                
                completed++;
                std::cout << "  Completed: " << completed << "/" << totalExperiments << std::endl;
            }
        }
    }
    
    std::cout << "Parameter sweep completed!" << std::endl;
}
```

---

# 11. Custom Component Integration

## Extending with Custom Plasticity Rules

```cpp
class CustomPlasticityRule : public nlm::PlasticityRule {
public:
    CustomPlasticityRule() : nlm::PlasticityRule("CustomRule") {}
    
    void applyPlasticity(std::shared_ptr<nlm::Synapse> synapse, 
                        const nlm::SpikeEvent& preSpike,
                        const nlm::SpikeEvent& postSpike) override {
        // Custom plasticity rule implementation
        float weight = synapse->getWeight();
        
        // Custom rule: weight change depends on spike timing difference
        float timeDiff = postSpike.timestamp - preSpike.timestamp;
        float weightChange = customWeightChangeFunction(timeDiff, weight);
        
        synapse->addToWeight(weightChange);
    }
    
private:
    float customWeightChangeFunction(float timeDiff, float currentWeight) {
        // Custom mathematical function for weight change
        return 0.01f * std::exp(-std::abs(timeDiff) / 10.0f) * (1.0f - currentWeight);
    }
};
```

## Custom Neuromodulators

```cpp
class CustomNeuromodulator : public nlm::Neuromodulator {
public:
    CustomNeuromodulator() : nlm::Neuromodulator("CustomModulator") {}
    
    void update(double dt) override {
        // Custom neuromodulator dynamics
        level_ = updateLevel(dt);
    }
    
    float getPlasticityFactor() const override {
        // Custom plasticity modulation
        return calculatePlasticityFactor();
    }
    
protected:
    float updateLevel(double dt) {
        // Custom update logic
        return level_ * (1.0f - decayRate_ * dt) + input_ * dt;
    }
    
    float calculatePlasticityFactor() const {
        // Custom factor calculation
        return 1.0f + std::sin(level_) * 0.5f;
    }
    
private:
    float level_ = 0.0f;
    float decayRate_ = 0.1f;
    float input_ = 0.0f;
};
```

---

# 12. Best Practices and Patterns

## Design Patterns for NLM

### 1. Facade Pattern
Use the AgentBrain as a facade to simplify interaction with complex brain systems:

```cpp
void userCode(std::shared_ptr<nlm::AgentBrain> agentBrain) {
    // Simple facade interface
    agentBrain->enable_all_systems();
    
    // Process sensory input
    agentBrain->processSensoryInput(percept);
    
    // Get decision
    auto action = agentBrain->decodeMotorCommand();
    
    // Apply learning
    agentBrain->applyRewardModulation(reward, predictedReward);
}
```

### 2. Builder Pattern
Use configuration builders for complex setup:

```cpp
void complexConfigurationExample() {
    // Method chaining for complex configuration
    auto config = nlm::Config()
        .set("neuron_count", 1000)
        .set("region_count", 2)
        .set("simulation_timestep", 0.001)
        .set("random_seed", 12345)
        .set("stdp_ltp_weight", 0.02f)
        .set("stdp_ltd_weight", 0.015f)
        .set("connection_probability", 0.15f)
        .set("enable_curiosity", true)
        .set("enable_reward_modulation", true)
        .set("enable_development", true);
    
    // Create brain with complex configuration
    auto brain = std::make_shared<nlm::Brain>(config);
}
```

### 3. Strategy Pattern
Implement different learning strategies:

```cpp
class LearningStrategy {
public:
    virtual ~LearningStrategy() = default;
    virtual void applyLearning(std::shared_ptr<nlm::AgentBrain> agentBrain) = 0;
};

class ReinforcementLearningStrategy : public LearningStrategy {
public:
    void applyLearning(std::shared_ptr<nlm::AgentBrain> agentBrain) override {
        // Reinforcement learning specific implementation
        auto dopamine = agentBrain->getDopamine();
        // Apply reward-based learning
    }
};

class CuriosityDrivenStrategy : public LearningStrategy {
public:
    void applyLearning(std::shared_ptr<nlm::AgentBrain> agentBrain) override {
        // Curiosity-driven exploration
        auto curiosity = agentBrain->getCuriosity();
        // Apply curiosity-based learning
    }
};
```

## Best Practices

### 1. Error Handling
Always handle errors gracefully and provide meaningful error messages:

```cpp
bool safeBrainOperation(std::shared_ptr<nlm::Brain> brain) {
    try {
        // Perform brain operation
        brain->initialize();
        brain->step(0, 0.0);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in brain operation: " << e.what() << std::endl;
        return false;
    }
}
```

### 2. Resource Management
Use context managers and RAII principles:

```cpp
void resourceSafeExample() {
    auto config = nlm::createDefaultConfig();
    
    // Use context manager for automatic cleanup
    auto brain = nlm::createBrain(config);
    
    with brain:
        brain->initialize();
        brain->step(0, 0.0);
        
        // Brain automatically reset when exiting context
    
    // Brain is now safely cleaned up
}
```

### 3. Configuration Validation
Always validate configuration before use:

```cpp
bool validateConfiguration(const nlm::Config& config) {
    // Check required settings
    if (!config.has("neuron_count")) {
        std::cerr << "Missing required setting: neuron_count" << std::endl;
        return false;
    }
    
    int neuronCount = config.getOr<int>("neuron_count", 0);
    if (neuronCount <= 0) {
        std::cerr << "Invalid neuron_count: must be positive" << std::endl;
        return false;
    }
    
    // Additional validation...
    return true;
}
```

### 4. Logging and Monitoring
Use comprehensive logging for debugging and monitoring:

```cpp
void loggedExample(std::shared_ptr<nlm::Brain> brain) {
    // Set up logging
    brain->getConfig()->set("log_level", "info");
    
    // Run simulation with logging
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 10 == 0) {
            brain->logStatus();  // Log current status
        }
    }
}
```

### 5. Performance Considerations
Optimize for performance in production:

```cpp
void performanceOptimizedExample(std::shared_ptr<nlm::Brain> brain) {
    // Enable performance optimizations
    brain->enableSIMDProcessing(true);
    brain->enableParallelEventProcessing(true);
    
    // Use memory pools for better performance
    brain->enableMemoryPools(true);
    
    // Run performance-critical simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 10000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Performance: " << (10000 / (duration.count() / 1000.0)) << " steps/second" << std::endl;
}
```

## Migration Guide

### From Single-Module to Full Integration

If you're migrating from a simple single-module NLM setup to the full Phase 6 integration, follow these steps:

1. **Update Configuration**
```cpp
// Old: Simple configuration
config->set("neuron_count", 100);

// New: Comprehensive configuration
config->set("neuron_count", 500);
config->set("region_count", 2);
config->set("simulation_timestep", 0.001);
config->set("random_seed", 42);
config->set("connection_probability", 0.15f);
config->set("enable_curiosity", true);
config->set("enable_reward_modulation", true);
config->set("enable_development", true);
```

2. **Enable All Systems**
```cpp
// Old: Basic brain operations
brain->initialize();
brain->step(step, time);

// New: Enable all integrated systems
agentBrain->enable_all_systems();
agentBrain->processSensoryInput(percept);
auto action = agentBrain->decodeMotorCommand();
agentBrain->applyRewardModulation(reward, predictedReward);
```

3. **Use New Memory Systems**
```cpp
// Old: Basic brain operations
brain->logStatus();

// New: Access all memory systems
auto workingMemory = brain->getWorkingMemory();
auto episodicMemory = brain->getEpisodicMemory();
auto associativeMemory = brain->getAssociativeMemory();
```

4. **Leverage Neuromodulation**
```cpp
// Old: Basic learning
// New: Advanced neuromodulation-based learning
agentBrain->applyRewardModulation(reward, predictedReward);
agentBrain->updateDevelopment(timestep);
```

---

# API Reference Summary

## Core Classes

### Brain
- Central neural simulation component
- Manages all neural components and systems
- Provides main interface for neural computation

### AgentBrain
- High-level interface for brain-world interaction
- Manages sensory processing and motor output
- Integrates all learning systems

### Config
- Configuration management with JSON/YAML support
- Pythonic API for easy configuration
- Schema validation and type safety

### SimpleWorld
- 2D environment simulation
- Sensory input processing
- Motor command execution

## Key Methods

### Brain Methods
- `initialize()`: Initialize brain with configuration
- `step(step, time)`: Execute simulation step
- `receiveSensoryInput(input)`: Inject sensory input
- `injectCurrent(neuron, current)`: Inject current into specific neuron
- `produceAction()`: Generate motor action
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

### AgentBrain Methods
- `initialize(world)`: Initialize with world interface
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command from neural activity
- `applyRewardModulation(reward, predictedReward)`: Apply reward-based learning
- `updateDevelopment(timestep)`: Update development system

### Config Methods
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file
- `set(key, value)`: Set configuration value
- `getOr<T>(key, default)`: Get configuration value with default

## Python API Highlights

### Configuration
```python
config = nlm.createDefaultConfig()
config.update({"neuron_count": 500, "simulation_timestep": 0.001})
brain = nlm.createBrain(config)
```

### Brain Operations
```python
brain.initialize()
for i in range(100):
    brain.step(i, i * 0.001)
    if i % 10 == 0:
        brain.receiveSensoryInput(sensory_input)
```

### Agent Interface
```python
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)
action = agent_brain.decodeMotorCommand()
agent_brain.processSensoryInput(percept)
```

## Error Handling

NLM provides comprehensive error handling:

1. **Standard Exceptions**: Use standard C++ exceptions for error reporting
2. **Error Context**: Detailed error context for debugging
3. **Graceful Degradation**: System continues operation even with partial failures
4. **Recovery Mechanisms**: Automatic recovery from non-critical errors

## Performance Considerations

1. **Memory Management**: Use memory pools for long-running simulations
2. **Parallel Processing**: Enable SIMD and parallel processing for performance
3. **Checkpointing**: Use checkpointing for large-scale simulations
4. **Configuration Optimization**: Optimize configuration for specific use cases

## Testing and Validation

1. **Unit Tests**: Comprehensive test suite for all components
2. **Integration Tests**: Verify system integration and cross-component communication
3. **Performance Tests**: Validate performance characteristics
4. **Stress Tests**: Test system behavior under extreme conditions

---

## Conclusion

This guide provides comprehensive guidance for using the NLM system, from basic setup to advanced usage patterns. Whether you're a beginner looking to get started quickly or an experienced user seeking advanced optimization techniques, this document serves as a complete reference for NLM usage.

The NLM system provides a powerful, flexible framework for implementing artificial neural systems with biological inspiration. By following the patterns and best practices outlined in this guide, you can build robust, efficient, and maintainable neural simulation applications.

For additional support and questions, refer to the NLM documentation and community resources available at the NLM project website.

---

*Document Version: 1.0*  
*Last Updated: 2026*  
*NLM Research Project*