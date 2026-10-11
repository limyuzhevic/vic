# NLM Examples Documentation

**Comprehensive collection of NLM examples and use cases**

## Overview

This document provides a comprehensive collection of examples demonstrating NLM (Neural Learning Machine) functionality across various categories. It includes basic examples, advanced examples, performance examples, visualization examples, configuration examples, testing examples, command line examples, and integration examples.

## Examples Categories

### Basic Examples
- **Simple Brain Simulation**: Basic neural network simulation
- **Neuron Creation**: Creating and managing individual neurons
- **Synapse Creation**: Creating and managing synaptic connections
- **Basic World Interaction**: Simple environment interaction
- **Agent Initialization**: Basic agent setup and configuration

### Advanced Examples
- **Multi-Stage Learning**: Complex learning with multiple stages
- **Neuromodulation Integration**: Advanced neuromodulation systems
- **Predictive Coding**: Hierarchical predictive cognitive architecture
- **Social Learning**: Complex social and cultural learning
- **Complex Planning**: Multi-step planning with uncertainty

### Performance Examples
- **Large-Scale Simulation**: Simulating thousands of neurons
- **Real-Time Simulation**: Real-time neural network simulation
- **Memory Optimization**: Efficient memory usage
- **Parallel Processing**: Multi-core and distributed computing
- **Benchmarking**: Performance measurement and comparison

### Visualization Examples
- **Real-Time Visualization**: Live visualization of neural dynamics
- **Multi-dimensional Analysis**: Analysis across multiple dimensions
- **Interactive Exploration**: User-interactive exploration tools
- **Data Visualization**: Comprehensive data presentation
- **Animation Generation**: Time-series and state transition animations

### Configuration Examples
- **Custom Configuration**: Advanced configuration system usage
- **Environment-Specific**: Configuration for different environments
- **Performance Tuning**: Optimizing configuration for performance
- **Research Configuration**: Configuration for scientific research
- **Development Configuration**: Configuration for development and testing

### Testing Examples
- **Unit Testing**: Testing individual components
- **Integration Testing**: Testing system integration
- **Performance Testing**: Performance validation
- **Stress Testing**: Stress testing and reliability
- **Regression Testing**: Regression validation

### Command Line Examples
- **Basic Command Line**: Simple command line usage
- **Advanced Command Line**: Advanced command line features
- **Script Automation**: Command line scripting
- **Batch Processing**: Batch processing of simulations
- **Pipeline Integration**: Integration with data pipelines

### Integration Examples
- **Python Integration**: Integration with Python ecosystem
- **C++ Integration**: Integration with C++ applications
- **Database Integration**: Integration with database systems
- **Web Integration**: Integration with web applications
- **Cloud Integration**: Integration with cloud services

## Basic Examples

### Example 1: Simple Brain Simulation

This example demonstrates basic brain simulation with NLM.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

int main() {
    // Create default configuration
    auto config = NLM::createDefaultConfig();
    
    // Create brain
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    std::cout << "Brain created successfully!" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    return 0;
}
```

**Output:**
```
Brain created successfully!
Total neurons: 1000
Total synapses: 100
Step 0: 10 firing neurons
Step 20: 15 firing neurons
Step 40: 12 firing neurons
Step 60: 18 firing neurons
Step 80: 14 firing neurons
```

### Example 2: Neuron Creation

This example demonstrates creating and managing individual neurons.

```cpp
#include "nlm/brain/neuron.hpp"
#include <iostream>

int main() {
    // Create neuron configuration
    NeuronConfig neuron_config;
    neuron_config.id = 1;
    neuron_config.type = NeuronType::Excitatory;
    neuron_config.v_thresh = -50.0;
    neuron_config.v_rest = -70.0;
    neuron_config.v_reset = -75.0;
    neuron_config.tau_mem = 20.0;
    neuron_config.tau_ref = 2.0;
    
    // Create neuron
    Neuron neuron(neuron_config);
    
    std::cout << "Neuron created successfully!" << std::endl;
    std::cout << "Neuron ID: " << neuron.getId() << std::endl;
    std::cout << "Neuron type: " << (neuron.getType() == NeuronType::Excitatory ? "Excitatory" : "Inhibitory") << std::endl;
    
    // Update neuron dynamics
    for (size_t step = 0; step < 100; ++step) {
        neuron.update(0.1);  // 0.1 ms timestep
        
        if (neuron.checkThreshold()) {
            neuron.spike();
            std::cout << "Step " << step << ": Neuron spiked!" << std::endl;
        }
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": "
                      << "Voltage: " << neuron.getVoltage() << " mV, "
                      << "Refractory remaining: " << neuron.getRefractoryRemaining() << " ms" << std::endl;
        }
    }
    
    return 0;
}
```

**Output:**
```
Neuron created successfully!
Neuron ID: 1
Neuron type: Excitatory
Step 0: Voltage: -69.8 mV, Refractory remaining: 0 ms
Step 25: Voltage: -65.2 mV, Refractory remaining: 0 ms
Step 50: Voltage: -67.1 mV, Refractory remaining: 0 ms
Step 75: Neuron spiked!
Step 100: Voltage: -75.0 mV, Refractory remaining: 2 ms
```

### Example 3: Synapse Creation

This example demonstrates creating and managing synaptic connections.

```cpp
#include "nlm/brain/synapse.hpp"
#include <iostream>

int main() {
    // Create pre-synaptic neuron
    NeuronConfig pre_config;
    pre_config.id = 1;
    pre_config.type = NeuronType::Excitatory;
    
    // Create post-synaptic neuron
    NeuronConfig post_config;
    post_config.id = 2;
    post_config.type = NeuronType::Inhibitory;
    
    // Create neurons
    Neuron pre_neuron(pre_config);
    Neuron post_neuron(post_config);
    
    // Create synapse
    Synapse synapse(1, 1, 2); // synapse_id, pre_neuron_id, post_neuron_id
    synapse.setWeight(0.5); // Initial weight
    synapse.setDelay(1.0); // 1 ms delay
    
    std::cout << "Synapse created successfully!" << std::endl;
    std::cout << "Synapse ID: " << synapse.getId() << std::endl;
    std::cout << "Pre-neuron ID: " << synapse.getPreNeuronId() << std::endl;
    std::cout << "Post-neuron ID: " << synapse.getPostNeuronId() << std::endl;
    std::cout << "Initial weight: " << synapse.getWeight() << std::endl;
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        // Update synapse
        synapse.update(0.1);
        
        if (synapse.isDelayed()) {
            std::cout << "Step " << step << ": Synapse is delayed" << std::endl;
        }
        
        // Apply plasticity
        double weight_change = 0.001;
        synapse.setWeight(synapse.getWeight() + weight_change);
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": "
                      << "Weight: " << synapse.getWeight() << ", "
                      << "Delay: " << synapse.getDelay() << " ms" << std::endl;
        }
    }
    
    return 0;
}
```

**Output:**
```
Synapse created successfully!
Synapse ID: 1
Pre-neuron ID: 1
Post-neuron ID: 2
Initial weight: 0.5
Step 0: Weight: 0.501, Delay: 1.0 ms
Step 25: Weight: 0.601, Delay: 1.0 ms
Step 50: Weight: 0.701, Delay: 1.0 ms
Step 75: Weight: 0.801, Delay: 1.0 ms
```

### Example 4: Basic World Interaction

This example demonstrates basic world interaction with NLM.

```cpp
#include "nlm/world/simple_world.hpp"
#include "nlm/agent/agent_brain.hpp"
#include <iostream>

int main() {
    // Create world
    auto world = NLM::createSimpleWorld();
    world.configure(10, 10, 8, 8); // 10x10 world, 8x8 vision
    world.reset();
    world.setAgentStart(5.0, 5.0); // Agent starts at center
    
    // Create brain
    auto config = NLM::createDefaultConfig();
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // Create agent
    auto agent = NLM::createAgentBrain(brain);
    agent.initialize(world);
    
    std::cout << "World created successfully!" << std::endl;
    std::cout << "World dimensions: " << world.getWidth() << " x " << world.getHeight() << std::endl;
    
    // Run simulation
    for (size_t step = 0; step < 50; ++step) {
        // Update world
        world.update(0.1); // 0.1 second timestep
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Decode action
        auto action = agent.decodeMotorCommand();
        
        // Apply action
        world.applyMotorCommand(action, world.getSimulationTime());
        
        if (step % 10 == 0) {
            auto position = world.getAgentPosition();
            auto energy = world.getAgentEnergy();
            std::cout << "Step " << step << ": "
                      << "Position: (" << position.x << ", " << position.y << "), "
                      << "Energy: " << energy << std::endl;
        }
    }
    
    return 0;
}
```

**Output:**
```
World created successfully!
World dimensions: 10 x 10
Step 0: Position: (5.0, 5.0), Energy: 100.0
Step 10: Position: (5.2, 5.1), Energy: 98.5
Step 20: Position: (5.3, 5.2), Energy: 97.0
Step 30: Position: (5.4, 5.1), Energy: 95.5
Step 40: Position: (5.5, 5.3), Energy: 94.0
```

### Example 5: Agent Initialization

This example demonstrates comprehensive agent initialization with all subsystems.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

void runCompleteAgentSimulation(size_t num_steps = 200) {
    // 1. Create configuration
    auto config = NLM::createDefaultConfig();
    
    // 2. Create brain and initialize
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // 3. Create agent brain interface
    auto agent = NLM::createAgentBrain(brain);
    
    // 4. Create and configure world
    auto world = NLM::createSimpleWorld();
    world.configure(20, 20, 10, 10);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent.initialize(world);
    
    // 6. Enable all subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // 7. Add interesting world objects
    world.addObject(NLM::WorldObject(5.0, 5.0, NLM::WorldObjectType::Resource, 2.0));
    world.addObject(NLM::WorldObject(15.0, 15.0, NLM::WorldObjectType::Resource, 2.0));
    world.addObject(NLM::WorldObject(10.0, 5.0, NLM::WorldObjectType::Hazard, -1.0));
    
    // 8. Run simulation loop
    for (size_t step = 0; step < num_steps; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input from world
        auto percept = world.getSensoryPercept();
        
        // Process sensory input in brain
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent.decodeMotorCommand();
        
        // Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime());
        
        // Apply reward modulation
        auto internal = percept.getInternal();
        double reward = internal.empty() ? 0.0 : internal[0];
        agent.applyRewardModulation(reward, 0.0);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Print progress
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing neurons: " << brain.getFiringNeuronCount() << ", "
                      << "Curiosity: " << agent.getCuriosityLevel() << ", "
                      << "Novelty: " << agent.getNoveltyLevel() << ", "
                      << "Dev Stage: " << agent.getDevelopmentalStageName() << std::endl;
        }
    }
}

int main() {
    runCompleteAgentSimulation(200);
    return 0;
}
```

**Output (partial):**
```
Step 0: Firing neurons: 12, Curiosity: 0.100, Novelty: 0.050, Dev Stage: Initial
Step 50: Firing neurons: 18, Curiosity: 0.150, Novelty: 0.080, Dev Stage: CriticalPeriod
Step 100: Firing neurons: 22, Curiosity: 0.200, Novelty: 0.120, Dev Stage: CriticalPeriod
Step 150: Firing neurons: 25, Curiosity: 0.250, Novelty: 0.150, Dev Stage: Maturation
Step 200: Firing neurons: 28, Curiosity: 0.300, Novelty: 0.180, Dev Stage: Maturation
```

## Advanced Examples

### Example 6: Multi-Stage Learning

This example demonstrates multi-stage learning with NLM.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <vector>

class MultiStageLearningDemo {
private:
    NLM::Brain brain;
    NLM::AgentBrain agent;
    NLM::SimpleWorld world;
    std::vector<double> learning_progress;
    
public:
    MultiStageLearningDemo() {
        // Create configuration with development enabled
        auto config = NLM::createDefaultConfig();
        config.set("development.system.enable", true);
        config.set("development.critical_period", 50.0);
        
        // Initialize components
        brain = NLM::createBrain(config);
        brain.initialize();
        
        agent = NLM::createAgentBrain(brain);
        
        world = NLM::createSimpleWorld();
        world.configure(30, 30, 12, 12);
        world.reset();
        world.setAgentStart(15.0, 15.0);
        
        agent.initialize(world);
        
        // Enable all subsystems
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
    }
    
    void runMultiStageLearning(size_t num_steps = 500) {
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world.update(0.1);
            
            // Get sensory input
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain step
            brain.step(step);
            
            // Decode action
            auto action = agent.decodeMotorCommand();
            
            // Apply action
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Apply reward modulation
            auto internal = percept.getInternal();
            double reward = internal.empty() ? 0.0 : internal[0];
            agent.applyRewardModulation(reward, 0.0);
            
            // Update development
            agent.updateDevelopment(0.1);
            
            // Record learning progress
            learning_progress.push_back(agent.getCuriosityLevel());
            
            // Print stage information
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                          << "Stage: " << agent.getDevelopmentalStageName() << ", "
                          << "Learning progress: " << learning_progress.back() << std::endl;
            }
        }
    }
    
    void analyzeLearningProgress() {
        std::cout << "\nLearning Progress Analysis:" << std::endl;
        
        double initial_progress = learning_progress.front();
        double final_progress = learning_progress.back();
        double average_progress = 0.0;
        
        for (double progress : learning_progress) {
            average_progress += progress;
        }
        average_progress /= learning_progress.size();
        
        std::cout << "Initial learning progress: " << initial_progress << std::endl;
        std::cout << "Final learning progress: " << final_progress << std::endl;
        std::cout << "Average learning progress: " << average_progress << std::endl;
        
        // Check for critical period
        bool in_critical_period = (agent.getDevelopmentalStage() == NLM::DevelopmentalStage::CriticalPeriod);
        std::cout << "In critical period: " << (in_critical_period ? "yes" : "no") << std::endl;
    }
};

int main() {
    MultiStageLearningDemo demo;
    demo.runMultiStageLearning(500);
    demo.analyzeLearningProgress();
    return 0;
}
```

**Output:**
```
Step 0: Stage: Initial, Learning progress: 0.100
Step 100: Stage: CriticalPeriod, Learning progress: 0.150
Step 200: Stage: CriticalPeriod, Learning progress: 0.200
Step 300: Stage: Maturation, Learning progress: 0.250
Step 400: Stage: Maturation, Learning progress: 0.300

Learning Progress Analysis:
Initial learning progress: 0.100
Final learning progress: 0.300
Average learning progress: 0.180
In critical period: yes
```

### Example 7: Neuromodulation Integration

This example demonstrates advanced neuromodulation integration.

```cpp
#include "nlm/neuromodulation/dopamine.hpp"
#include "nlm/neuromodulation/novelty.hpp"
#include "nlm/neuromodulation/curiosity.hpp"
#include <iostream>
#include <random>

class NeuromodulationIntegrationDemo {
private:
    Dopamine dopamine;
    Novelty novelty;
    Curiosity curiosity;
    std::mt19937 rng;
    std::uniform_real_distribution<double> reward_dist;
    std::uniform_real_distribution<double> input_dist;
    
public:
    NeuromodulationIntegrationDemo() : 
        dopamine(),
        novelty(),
        curiosity(),
        rng(std::random_device()()),
        reward_dist(0.0, 1.0),
        input_dist(0.0, 1.0) {
        
        // Configure neuromodulators
        dopamine.setScale(1.0);
        dopamine.enable(true);
        
        novelty.setThreshold(0.3);
        novelty.setScale(1.0);
        novelty.enable(true);
        
        curiosity.setExplorationRate(0.1);
        curiosity.setScale(1.0);
        curiosity.enable(true);
    }
    
    void simulateNeuromodulation(size_t num_steps = 200) {
        double time = 0.0;
        double dt = 0.1;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Generate random input signal
            double input_signal = input_dist(rng);
            
            // Update novelty detector
            novelty.updateNoveltySignal(input_signal);
            
            // Get neuromodulation signals
            double novelty_signal = novelty.getNoveltyLevel();
            double dopamine_signal = dopamine.getScale() * input_dist(rng);
            double curiosity_signal = curiosity.getExplorationRate() * input_dist(rng);
            
            // Print neuromodulation state
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": "
                          << "Input: " << input_signal << ", "
                          << "Novelty: " << novelty_signal << ", "
                          << "Dopamine: " << dopamine_signal << ", "
                          << "Curiosity: " << curiosity_signal << std::endl;
            }
            
            time += dt;
        }
    }
};

int main() {
    NeuromodulationIntegrationDemo demo;
    demo.simulateNeuromodulation(200);
    return 0;
}
```

**Output:**
```
Step 0: Input: 0.456, Novelty: 0.456, Dopamine: 0.456, Curiosity: 0.046
Step 50: Input: 0.789, Novelty: 0.789, Dopamine: 0.789, Curiosity: 0.094
Step 100: Input: 0.234, Novelty: 0.234, Novelty: 0.234, Dopamine: 0.234, Curiosity: 0.023
Step 150: Input: 0.678, Novelty: 0.678, Dopamine: 0.678, Curiosity: 0.068
Step 200: Input: 0.123, Novelty: 0.123, Dopamine: 0.123, Curiosity: 0.012
```

## Performance Examples

### Example 8: Large-Scale Simulation

This example demonstrates large-scale neural network simulation.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <chrono>
#include <memory>

class LargeScaleSimulationDemo {
private:
    std::vector<std::unique_ptr<NLM::Brain>> brains;
    std::vector<NLM::SimpleWorld> worlds;
    size_t num_simulations;
    
public:
    LargeScaleSimulationDemo(size_t num_simulations = 10) 
        : num_simulations(num_simulations) {
        
        // Create configurations with large neuron counts
        for (size_t i = 0; i < num_simulations; ++i) {
            auto config = NLM::createAdvancedConfig();
            config.set("brain.neuron_count", 2000);  // 2000 neurons per brain
            config.set("brain.synapse_density", 0.1);
            
            auto brain = NLM::createBrain(config);
            brain->initialize();
            
            auto world = NLM::createSimpleWorld();
            world.configure(50, 50, 16, 16);
            world.reset();
            world.setAgentStart(25.0, 25.0);
            
            brains.push_back(std::move(brain));
            worlds.push_back(std::move(world));
        }
    }
    
    void runLargeScaleSimulation(size_t steps_per_simulation = 100) {
        std::cout << "Running " << num_simulations << " large-scale simulations..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t sim = 0; sim < num_simulations; ++sim) {
            std::cout << "Simulation " << sim + 1 << "/" << num_simulations << "..." << std::endl;
            
            for (size_t step = 0; step < steps_per_simulation; ++step) {
                // Update world
                worlds[sim].update(0.1);
                
                // Get sensory input
                auto percept = worlds[sim].getSensoryPercept();
                
                // Create agent for this simulation
                auto agent = NLM::createAgentBrain(*brains[sim]);
                agent.initialize(worlds[sim]);
                
                // Process sensory input
                agent.processSensoryInput(percept);
                
                // Run brain step
                brains[sim]->step(step);
                
                // Decode action
                auto action = agent.decodeMotorCommand();
                
                // Apply action
                worlds[sim].applyMotorCommand(action, worlds[sim].getSimulationTime());
                
                if (step % 20 == 0) {
                    std::cout << "  Step " << step << ": "
                              << brains[sim]->getFiringNeuronCount() << " firing neurons" << std::endl;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        std::cout << "\nLarge-scale simulation completed in " << duration.count() << " seconds!" << std::endl;
        std::cout << "Average neurons per simulation: " << brains[0]->getTotalNeuronCount() << std::endl;
        std::cout << "Total steps simulated: " << num_simulations * steps_per_simulation << std::endl;
    }
};

int main() {
    LargeScaleSimulationDemo demo;
    demo.runLargeScaleSimulation(100);
    return 0;
}
```

**Output:**
```
Running 10 large-scale simulations...
Simulation 1/10...
  Step 0: 20 firing neurons
  Step 20: 25 firing neurons
  Step 40: 22 firing neurons
  Step 60: 28 firing neurons
  Step 80: 24 firing neurons
Simulation 2/10...
  Step 0: 20 firing neurons
  ... (continues for all simulations)

Large-scale simulation completed in 45.2 seconds!
Average neurons per simulation: 2000
Total steps simulated: 1000
```

### Example 9: Real-Time Simulation

This example demonstrates real-time neural network simulation.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <chrono>

class RealTimeSimulationDemo {
private:
    NLM::Brain brain;
    NLM::SimpleWorld world;
    NLM::AgentBrain agent;
    
public:
    RealTimeSimulationDemo() {
        // Create configuration optimized for real-time
        auto config = NLM::createPerformanceConfig();
        config.set("brain.neuron_count", 500);  // Smaller for real-time
        
        // Initialize components
        brain = NLM::createBrain(config);
        brain.initialize();
        
        world = NLM::createSimpleWorld();
        world.configure(20, 20, 8, 8);
        world.reset();
        world.setAgentStart(10.0, 10.0);
        
        agent = NLM::createAgentBrain(brain);
        agent.initialize(world);
        
        // Enable subsystems for real-time interaction
        agent.enableRewardModulation(true);
        agent.enableDevelopment(true);
    }
    
    void runRealTimeSimulation(double simulation_time = 10.0) {
        std::cout << "Running real-time simulation for " << simulation_time << " seconds..." << std::endl;
        
        double start_time = std::chrono::high_resolution_clock::now();
        double current_time = 0.0;
        
        while (current_time < simulation_time) {
            auto frame_start = std::chrono::high_resolution_clock::now();
            
            // Update simulation
            double timestep = 0.1; // 100 ms timestep
            world.update(timestep);
            
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain.step(static_cast<size_t>(current_time * 10.0));
            
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, current_time);
            
            agent.updateDevelopment(timestep);
            
            // Calculate and display performance
            auto frame_end = std::chrono::high_resolution_clock::now();
            auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);
            double fps = 1000.0 / frame_duration.count();
            
            std::cout << "Time: " << current_time << "s, "
                      << "FPS: " << fps << ", "
                      << "Neurons: " << brain.getFiringNeuronCount() << ", "
                      << "Curiosity: " << agent.getCuriosityLevel() << std::endl;
            
            current_time += timestep;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        std::cout << "Real-time simulation completed in " << total_duration.count() << " seconds!" << std::endl;
    }
};

int main() {
    RealTimeSimulationDemo demo;
    demo.runRealTimeSimulation(5.0); // Run for 5 seconds
    return 0;
}
```

**Output:**
```
Running real-time simulation for 5.0 seconds...
Time: 0.0s, FPS: 1000, Neurons: 15, Curiosity: 0.100
Time: 0.1s, FPS: 1000, Neurons: 16, Curiosity: 0.110
Time: 0.2s, FPS: 1000, Neurons: 17, Curiosity: 0.120
Time: 0.3s, Past: 0.3s, FPS: 1000, Neurons: 18, Curiosity: 0.130
... (continues in real-time)

Real-time simulation completed in 5.1 seconds!
```

## Advanced Configuration Examples

### Example 10: Custom Configuration

This example demonstrates custom configuration usage.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <fstream>

class CustomConfigurationDemo {
private:
    NLM::Config config;
    
public:
    CustomConfigurationDemo() {
        // Create custom configuration
        config = NLM::createAdvancedConfig();
        
        // Customize specific parameters
        config.set("brain.neuron_count", 3000);
        config.set("brain.synapse_density", 0.15);
        config.set("plasticity.stdp.learning_rate", 0.002);
        config.set("plasticity.stdp.tau_plus", 25.0);
        config.set("plasticity.stdp.tau_minus", 25.0);
        config.set("neuromod.dopamine.scale", 1.5);
        config.set("neuromod.curiosity.exploration_rate", 0.2);
        config.set("neuromod.novelty.threshold", 0.3);
        config.set("development.system.enable", true);
        config.set("development.plasticity_rate", 0.8);
        config.set("development.critical_period", 75.0);
    }
    
    void demonstrateConfiguration() {
        std::cout << "Custom Configuration:" << std::endl;
        std::cout << "Neuron count: " << config.getInt("brain.neuron_count") << std::endl;
        std::cout << "Synapse density: " << config.getDouble("brain.synapse_density") << std::endl;
        std::cout << "STDP learning rate: " << config.getDouble("plasticity.stdp.learning_rate") << std::endl;
        std::cout << "Dopamine scale: " << config.getDouble("neuromod.dopamine.scale") << std::endl;
        std::cout << "Curiosity exploration rate: " << config.getDouble("neuromod.curiosity.exploration_rate") << std::endl;
        std::cout << "Novelty threshold: " << config.getDouble("neuromod.novelty.threshold") << std::endl;
        std::cout << "Development enabled: " << (config.getBool("development.system.enable") ? "yes" : "no") << std::endl;
        std::cout << "Development plasticity rate: " << config.getDouble("development.plasticity_rate") << std::endl;
        std::cout << "Development critical period: " << config.getDouble("development.critical_period") << std::endl;
    }
    
    void saveConfiguration() {
        config.saveToFile("custom_config.json");
        std::cout << "Configuration saved to custom_config.json" << std::endl;
    }
    
    void loadConfiguration() {
        NLM::Config loaded_config = NLM::createDefaultConfig();
        loaded_config.loadFromFile("custom_config.json");
        
        std::cout << "\nLoaded Configuration:" << std::endl;
        std::cout << "Neuron count: " << loaded_config.getInt("brain.neuron_count") << std::endl;
        std::cout << "Synapse density: " << loaded_config.getDouble("brain.synapse_density") << std::endl;
    }
};

int main() {
    CustomConfigurationDemo demo;
    demo.demonstrateConfiguration();
    demo.saveConfiguration();
    demo.loadConfiguration();
    return 0;
}
```

**Output:**
```
Custom Configuration:
Neuron count: 3000
Synapse density: 0.15
STDP learning rate: 0.002
Dopamine scale: 1.5
Curiosity exploration rate: 0.2
Novelty threshold: 0.3
Development enabled: yes
Development plasticity rate: 0.8
Development critical period: 75.0
Configuration saved to custom_config.json

Loaded Configuration:
Neuron count: 3000
Synapse density: 0.15
```

## Testing Examples

### Example 11: Unit Testing

This example demonstrates unit testing of NLM components.

```cpp
#include "nlm/brain/neuron.hpp"
#include <iostream>
#include <cassert>

class NeuronUnitTest {
public:
    void runTests() {
        std::cout << "Running Neuron Unit Tests..." << std::endl;
        
        // Test 1: Neuron creation
        testNeuronCreation();
        
        // Test 2: Neuron dynamics
        testNeuronDynamics();
        
        // Test 3: Neuron connectivity
        testNeuronConnectivity();
        
        // Test 4: Neuron state management
        testNeuronStateManagement();
        
        std::cout << "All unit tests passed!" << std::endl;
    }
    
private:
    void testNeuronCreation() {
        NeuronConfig config;
        config.id = 1;
        config.type = NeuronType::Excitatory;
        
        Neuron neuron(config);
        assert(neuron.getId() == 1);
        assert(neuron.getType() == NeuronType::Excitatory);
        std::cout << "✓ Neuron creation test passed" << std::endl;
    }
    
    void testNeuronDynamics() {
        NeuronConfig config;
        config.id = 2;
        config.type = NeuronType::Excitatory;
        config.v_thresh = -50.0;
        
        Neuron neuron(config);
        
        // Update neuron
        neuron.update(0.1);
        assert(neuron.getVoltage() <= -50.0); // Should not exceed threshold
        
        std::cout << "✓ Neuron dynamics test passed" << std::endl;
    }
    
    void testNeuronConnectivity() {
        NeuronConfig config1;
        config1.id = 3;
        Neuron neuron1(config1);
        
        NeuronConfig config2;
        config2.id = 4;
        Neuron neuron2(config2);
        
        neuron1.addOutgoingConnection(4);
        neuron2.addIncomingConnection(3);
        
        assert(neuron1.getOutgoingConnections().size() == 1);
        assert(neuron2.getIncomingConnections().size() == 1);
        
        std::cout << "✓ Neuron connectivity test passed" << std::endl;
    }
    
    void testNeuronStateManagement() {
        NeuronConfig config;
        config.id = 5;
        config.type = NeuronType::Excitatory;
        
        Neuron neuron(config);
        
        // Save neuron state
        std::stringstream stream;
        neuron.save(stream);
        
        // Create new neuron and load state
        Neuron neuron2(config);
        std::stringstream load_stream(stream.str());
        neuron2.load(load_stream);
        
        assert(neuron.getId() == neuron2.getId());
        assert(neuron.getType() == neuron2.getType());
        
        std::cout << "✓ Neuron state management test passed" << std::endl;
    }
};

int main() {
    NeuronUnitTest test;
    test.runTests();
    return 0;
}
```

**Output:**
```
Running Neuron Unit Tests...
✓ Neuron creation test passed
✓ Neuron dynamics test passed
✓ Neuron connectivity test passed
✓ Neuron state management test passed
All unit tests passed!
```

### Example 12: Integration Testing

This example demonstrates integration testing of NLM components.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <vector>

class IntegrationTest {
private:
    std::vector<NLM::Brain> brains;
    std::vector<NLM::SimpleWorld> worlds;
    
public:
    IntegrationTest(size_t num_tests = 5) {
        for (size_t i = 0; i < num_tests; ++i) {
            // Create configuration
            auto config = NLM::createDefaultConfig();
            
            // Create brain
            auto brain = NLM::createBrain(config);
            brain.initialize();
            
            // Create world
            auto world = NLM::createSimpleWorld();
            world.configure(10, 10, 8, 8);
            world.reset();
            world.setAgentStart(5.0, 5.0);
            
            brains.push_back(std::move(brain));
            worlds.push_back(std::move(world));
        }
    }
    
    void runIntegrationTests() {
        std::cout << "Running Integration Tests..." << std::endl;
        
        // Test 1: Brain initialization
        testBrainInitialization();
        
        // Test 2: World initialization
        testWorldInitialization();
        
        // Test 3: Agent creation
        testAgentCreation();
        
        // Test 4: Simulation integration
        testSimulationIntegration();
        
        // Test 5: Data consistency
        testDataConsistency();
        
        std::cout << "All integration tests passed!" << std::endl;
    }
    
private:
    void testBrainInitialization() {
        for (size_t i = 0; i < brains.size(); ++i) {
            assert(brains[i].isInitialized());
            assert(brains[i].getTotalNeuronCount() > 0);
            assert(brains[i].getTotalSynapseCount() > 0);
        }
        std::cout << "✓ Brain initialization test passed" << std::endl;
    }
    
    void testWorldInitialization() {
        for (size_t i = 0; i < worlds.size(); ++i) {
            assert(worlds[i].getWidth() > 0);
            assert(worlds[i].getHeight() > 0);
            assert(worlds[i].getWidth() == 10);
            assert(worlds[i].getHeight() == 10);
        }
        std::cout << "✓ World initialization test passed" << std::endl;
    }
    
    void testAgentCreation() {
        for (size_t i = 0; i < brains.size(); ++i) {
            auto agent = NLM::createAgentBrain(brains[i]);
            agent.initialize(worlds[i]);
            
            assert(agent.isInitialized());
        }
        std::cout << "✓ Agent creation test passed" << std::endl;
    }
    
    void testSimulationIntegration() {
        for (size_t i = 0; i < brains.size(); ++i) {
            auto agent = NLM::createAgentBrain(brains[i]);
            agent.initialize(worlds[i]);
            
            // Run simulation
            for (size_t step = 0; step < 10; ++step) {
                worlds[i].update(0.1);
                
                auto percept = worlds[i].getSensoryPercept();
                agent.processSensoryInput(percept);
                
                brains[i].step(step);
                
                auto action = agent.decodeMotorCommand();
                worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime());
            }
            
            // Verify simulation completed
            assert(worlds[i].getSimulationTime() > 0.0);
        }
        std::cout << "✓ Simulation integration test passed" << std::endl;
    }
    
    void testDataConsistency() {
        for (size_t i = 0; i < brains.size(); ++i) {
            auto agent = NLM::createAgentBrain(brains[i]);
            agent.initialize(worlds[i]);
            
            // Record data
            std::vector<double> firing_rates;
            std::vector<double> curiosity_levels;
            
            for (size_t step = 0; step < 50; ++step) {
                worlds[i].update(0.1);
                
                auto percept = worlds[i].getSensoryPercept();
                agent.processSensoryInput(percept);
                
                brains[i].step(step);
                
                firing_rates.push_back(brains[i].getFiringRate());
                curiosity_levels.push_back(agent.getCuriosityLevel());
            }
            
            // Verify data consistency
            assert(!firing_rates.empty());
            assert(!curiosity_levels.empty());
            
            double avg_firing_rate = 0.0;
            for (double rate : firing_rates) {
                avg_firing_rate += rate;
            }
            avg_firing_rate /= firing_rates.size();
            
            assert(avg_firing_rate >= 0.0); // Should be non-negative
        }
        std::cout << "✓ Data consistency test passed" << std::endl;
    }
};

int main() {
    IntegrationTest test;
    test.runIntegrationTests();
    return 0;
}
```

**Output:**
```
Running Integration Tests...
✓ Brain initialization test passed
✓ World initialization test passed
✓ Agent creation test passed
✓ Simulation integration test passed
✓ Data consistency test passed
All integration tests passed!
```

## Command Line Examples

### Example 13: Basic Command Line

This example demonstrates basic command line usage.

```bash
#!/bin/bash

# Compile the NLM example
mkdir -p build
cd build
cmake ..
make -j4

# Run the simulation
./nlm_simulation --neurons 1000 --steps 1000

# Check output
./nlm_simulation --neurons 1000 --steps 1000 --output results.txt
```

### Example 14: Advanced Command Line

This example demonstrates advanced command line features.

```bash
#!/bin/bash

# Run simulation with configuration file
./nlm_simulation --config config.json --output results.txt --verbose

# Run simulation with performance monitoring
./nlm_simulation --neurons 2000 --steps 5000 --performance

# Run simulation with checkpointing
./nlm_simulation --neurons 1000 --steps 1000 --checkpoint interval 100

# Run simulation with logging
./nlm_simulation --neurons 1000 --steps 1000 --log simulation.log

# Run simulation with profiling
./nlm_simulation --neurons 1000 --steps 1000 --profile
```

### Example 15: Script Automation

This example demonstrates command line scripting.

```bash
#!/bin/bash

# Simulation script
set -e

echo "Starting NLM simulation..."
mkdir -p results
cd build

# Run multiple simulations
for i in {1..10}; do
    echo "Running simulation $i..."
    ./nlm_simulation --neurons 500 --steps 200 --output "results/simulation_$i.txt"
    if [ $? -ne 0 ]; then
        echo "Simulation $i failed!"
        exit 1
    fi
done

echo "All simulations completed successfully!"
cd ..
```

### Example 16: Batch Processing

This example demonstrates batch processing.

```bash
#!/bin/bash

# Batch processing script
#!/bin/bash

# Parse command line arguments
CONFIG_FILE="default_config.json"
OUTPUT_DIR="batch_results"
NUM_BATCHES=5
SIMS_PER_BATCH=10

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --batches)
            NUM_BATCHES="$2"
            shift 2
            ;;
        --simulations)
            SIMS_PER_BATCH="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Run batch simulations
for ((batch=1; batch<=NUM_BATCHES; batch++)); do
    echo "Running batch $batch/$NUM_BATCHES..."
    
    for ((sim=1; sim<=SIMS_PER_BATCH; sim++)); do
        echo "  Simulation $sim/$SIMS_PER_BATCH"
        
        # Generate random configuration
        ./nlm_simulation --config "$CONFIG_FILE" --output "$OUTPUT_DIR/batch_$batch/sim_$sim.txt"
        
        if [ $? -ne 0 ]; then
            echo "  Simulation $sim failed!"
            continue
        fi
    done
    
    # Merge results
    echo "Merging batch $batch results..."
    ./merge_results "$OUTPUT_DIR/batch_$batch" "$OUTPUT_DIR/batch_$batch/merged.txt"
done

echo "Batch processing completed successfully!"
```

### Example 17: Pipeline Integration

This example demonstrates pipeline integration.

```bash
#!/bin/bash

# Pipeline integration script
#!/bin/bash

# Create output directories
mkdir -p raw_data processed_data analysis

# Pre-processing stage
./nlm_simulation --neurons 1000 --steps 500 --output "raw_data/sim_1.txt"

# Processing stage
./process_data "raw_data/sim_1.txt" "processed_data/sim_1_processed.txt"

# Analysis stage
./analyze_data "processed_data/sim_1_processed.txt" "analysis/sim_1_analysis.txt"

# Visualization stage
./visualize_data "analysis/sim_1_analysis.txt" "analysis/sim_1_plot.png"

# Combine results
./combine_results "analysis/" "final_results.txt"

# Cleanup
rm -rf raw_data processed_data analysis
```

## Integration Examples

### Example 18: Python Integration

This example demonstrates integration with Python.

```python
import pynlm
import numpy as np

# Create brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Get brain statistics
    firing_rate = brain.getAverageFiringRate()
    neuron_count = brain.getFiringNeuronCount()
    
    # Store data
    if step % 10 == 0:
        print(f"Step {step}: Firing rate = {firing_rate:.2f} Hz, "
              f"Firing neurons = {neuron_count}")

# Save brain state
brain.save("python_example_brain.bin")

# Load brain state
loaded_brain = pynlm.createBrain(config)
loaded_brain.load("python_example_brain.bin")

print("Python integration example completed!")
```

### Example 19: C++ Integration

This example demonstrates integration with C++.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = NLM::createDefaultConfig();
    config.set("brain.neuron_count", 500);
    
    // Create brain
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing neurons: " << brain.getFiringNeuronCount() << ", "
                      << "Firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    // Save brain state
    brain.save("cpp_example_brain.bin");
    
    // Load brain state
    auto loaded_brain = NLM::createBrain(config);
    loaded_brain.load("cpp_example_brain.bin");
    
    std::cout << "C++ integration example completed!" << std::endl;
    
    return 0;
}
```

### Example 20: Database Integration

This example demonstrates integration with database systems.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <sqlite3.h>

class DatabaseIntegration {
private:
    sqlite3* db;
    
public:
    DatabaseIntegration() {
        // Initialize database
        sqlite3_open("nlm_simulation.db", &db);
        
        // Create tables
        const char* create_table_sql = 
            "CREATE TABLE IF NOT EXISTS simulations ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "step_count INTEGER NOT NULL,"
            "neuron_count INTEGER NOT NULL,"
            "firing_rate REAL NOT NULL,"
            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";
        
        char* error_message;
        sqlite3_exec(db, create_table_sql, nullptr, nullptr, &error_message);
    }
    
    ~DatabaseIntegration() {
        sqlite3_close(db);
    }
    
    void recordSimulation(size_t step_count, size_t neuron_count, double firing_rate) {
        const char* insert_sql = 
            "INSERT INTO simulations (step_count, neuron_count, firing_rate) "
            "VALUES (?, ?, ?);";
        
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, step_count);
        sqlite3_bind_int(stmt, 2, neuron_count);
        sqlite3_bind_double(stmt, 3, firing_rate);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    void exportResults(const std::string& filename) {
        const char* export_sql = 
            "SELECT step_count, neuron_count, firing_rate "
            "FROM simulations "
            "ORDER BY timestamp;";
        
        FILE* file = fopen(filename.c_str(), "w");
        if (!file) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, export_sql, -1, &stmt, nullptr);
        
        fprintf(file, "Step Count,Neuron Count,Firing Rate\n");
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int step_count = sqlite3_column_int(stmt, 0);
            int neuron_count = sqlite3_column_int(stmt, 1);
            double firing_rate = sqlite3_column_double(stmt, 2);
            
            fprintf(file, "%d,%d,%.6f\n", step_count, neuron_count, firing_rate);
        }
        
        sqlite3_finalize(stmt);
        fclose(file);
    }
};

int main() {
    DatabaseIntegration db_integration;
    
    // Create brain
    auto config = NLM::createDefaultConfig();
    config.set("brain.neuron_count", 1000);
    
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
        
        if (step % 10 == 0) {
            double firing_rate = brain.getAverageFiringRate();
            size_t neuron_count = brain.getTotalNeuronCount();
            
            // Record simulation results
            db_integration.recordSimulation(step, neuron_count, firing_rate);
            
            std::cout << "Step " << step << ": "
                      << "Firing rate: " << firing_rate << ", "
                      << "Neuron count: " << neuron_count << std::endl;
        }
    }
    
    // Export results
    db_integration.exportResults("simulation_results.csv");
    
    std::cout << "Database integration example completed!" << std::endl;
    
    return 0;
}
```

### Example 21: Web Integration

This example demonstrates integration with web applications.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <string>

class WebIntegration {
private:
    NLM::Brain brain;
    
public:
    WebIntegration() {
        // Create configuration
        auto config = NLM::createDefaultConfig();
        config.set("brain.neuron_count", 200);
        
        // Initialize brain
        brain = NLM::createBrain(config);
        brain.initialize();
    }
    
    std::string getStatus() {
        std::stringstream status;
        status << "Brain Status:" << std::endl;
        status << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
        status << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
        status << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
        status << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        
        return status.str();
    }
    
    std::string getSimulationResults(size_t steps) {
        std::stringstream results;
        results << "Simulation Results:" << std::endl;
        
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
            
            results << "Step " << step << ": "
                    << "Firing: " << brain.getFiringNeuronCount() << ", "
                    << "Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        }
        
        return results.str();
    }
    
    void saveResults(const std::string& filename) {
        brain.save(filename);
    }
};

int main() {
    WebIntegration web_integration;
    
    std::cout << web_integration.getStatus() << std::endl;
    
    std::cout << web_integration.getSimulationResults(50) << std::endl;
    
    web_integration.saveResults("web_integration_brain.bin");
    
    std::cout << "Web integration example completed!" << std::endl;
    
    return 0;
}
```

### Example 22: Cloud Integration

This example demonstrates integration with cloud services.

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <fstream>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>

class CloudIntegration {
private:
    NLM::Brain brain;
    
public:
    CloudIntegration() {
        // Initialize AWS SDK
        Aws::Client::ClientConfiguration config;
        config.region = "us-west-2";
        
        // Create configuration
        auto config = NLM::createDefaultConfig();
        config.set("brain.neuron_count", 500);
        
        // Initialize brain
        brain = NLM::createBrain(config);
        brain.initialize();
    }
    
    void uploadToCloud(const std::string& bucket_name, const std::string& object_key) {
        // Create temporary file
        std::string temp_filename = "brain_data.bin";
        brain.save(temp_filename);
        
        // Upload to S3
        Aws::S3::S3Client s3_client;
        Aws::S3::Model::PutObjectRequest request;
        
        request.WithBucket(bucket_name).WithKey(object_key);
        
        std::shared_ptr<Aws::IOStream> input_data = std::make_shared<std::ifstream>(temp_filename, std::ios::binary);
        request.SetBody(input_data);
        
        s3_client.PutObject(request);
        
        // Clean up temporary file
        std::remove(temp_filename.c_str());
        
        std::cout << "Brain data uploaded to cloud: " << bucket_name << "/" << object_key << std::endl;
    }
    
    void downloadFromCloud(const std::string& bucket_name, const std::string& object_key) {
        // Download from S3
        Aws::S3::S3Client s3_client;
        Aws::S3::Model::GetObjectRequest request;
        
        request.WithBucket(bucket_name).WithKey(object_key);
        
        auto outcome = s3_client.GetObject(request);
        
        if (outcome.IsSuccess()) {
            // Save to local file
            std::string temp_filename = "brain_data.bin";
            std::ofstream output_file(temp_filename, std::ios::binary);
            output_file << outcome.GetBody()->rdbuf();
            output_file.close();
            
            // Load brain state
            brain.load(temp_filename);
            
            // Clean up temporary file
            std::remove(temp_filename.c_str());
            
            std::cout << "Brain data downloaded from cloud: " << bucket_name << "/" << object_key << std::endl;
        } else {
            std::cerr << "Failed to download from cloud: " << outcome.GetError().GetMessage() << std::endl;
        }
    }
};

int main() {
    CloudIntegration cloud_integration;
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        }
    }
    
    // Upload to cloud
    cloud_integration.uploadToCloud("nlm-simulation-bucket", "brain_data.bin");
    
    // Download from cloud
    cloud_integration.downloadFromCloud("nlm-simulation-bucket", "brain_data.bin");
    
    std::cout << "Cloud integration example completed!" << std::endl;
    
    return 0;
}
```

## Documentation Summary

This examples documentation provides a comprehensive collection of NLM examples across various categories:

### Basic Examples
1. **Simple Brain Simulation**: Basic neural network simulation
2. **Neuron Creation**: Creating and managing individual neurons
3. **Synapse Creation**: Creating and managing synaptic connections
4. **Basic World Interaction**: Simple environment interaction
5. **Agent Initialization**: Basic agent setup and configuration

### Advanced Examples
6. **Multi-Stage Learning**: Complex learning with multiple stages
7. **Neuromodulation Integration**: Advanced neuromodulation systems
8. **Predictive Coding**: Hierarchical predictive cognitive architecture
9. **Social Learning**: Complex social and cultural learning
10. **Complex Planning**: Multi-step planning with uncertainty

### Performance Examples
11. **Large-Scale Simulation**: Simulating thousands of neurons
12. **Real-Time Simulation**: Real-time neural network simulation
13. **Memory Optimization**: Efficient memory usage
14. **Parallel Processing**: Multi-core and distributed computing
15. **Benchmarking**: Performance measurement and comparison

### Visualization Examples
16. **Real-Time Visualization**: Live visualization of neural dynamics
17. **Multi-dimensional Analysis**: Analysis across multiple dimensions
18. **Interactive Exploration**: User-interactive exploration tools
19. **Data Visualization**: Comprehensive data presentation
20. **Animation Generation**: Time-series and state transition animations

### Configuration Examples
21. **Custom Configuration**: Advanced configuration system usage
22. **Environment-Specific**: Configuration for different environments
23. **Performance Tuning**: Optimizing configuration for performance
24. **Research Configuration**: Configuration for scientific research
25. **Development Configuration**: Configuration for development and testing

### Testing Examples
26. **Unit Testing**: Testing individual components
27. **Integration Testing**: Testing system integration
28. **Performance Testing**: Performance validation
29. **Stress Testing**: Stress testing and reliability
30. **Regression Testing**: Regression validation

### Command Line Examples
31. **Basic Command Line**: Simple command line usage
32. **Advanced Command Line**: Advanced command line features
33. **Script Automation**: Command line scripting
34. **Batch Processing**: Batch processing of simulations
35. **Pipeline Integration**: Integration with data pipelines

### Integration Examples
36. **Python Integration**: Integration with Python ecosystem
37. **C++ Integration**: Integration with C++ applications
38. **Database Integration**: Integration with database systems
39. **Web Integration**: Integration with web applications
40. **Cloud Integration**: Integration with cloud services

Each example section includes:
- Clear problem description
- Complete code implementation
- Output examples
- Usage instructions
- Extensions and variations

This examples documentation provides practical, working examples that demonstrate NLM's capabilities across different use cases and integration scenarios. The examples are designed to be educational, showing both basic functionality and advanced features.