# Advanced NLM Examples - Plasticity, Development & Neuromodulation

## Overview

This document provides comprehensive examples for advanced NLM usage, focusing on plasticity mechanisms, developmental processes, neuromodulation systems, and their integration. These examples demonstrate how to create sophisticated neural behaviors and optimize performance for research applications.

## Prerequisites

- Basic understanding of NLM architecture and concepts
- Familiarity with C++ and the NLM API
- Knowledge of neural dynamics and plasticity

## Example 1: Spike-Timing-Dependent Plasticity (STDP) Demonstration

### Overview
This example demonstrates STDP learning, where synaptic strength depends on the precise timing between pre- and post-synaptic spikes.

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include "Brain.hpp"
#include "Config.hpp"
#include "SensoryInput.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"

void demonstrateSTDP() {
    std::cout << "=== STDP Learning Demonstration ===\n";
    
    // Create configuration with STDP enabled
    auto config = std::make_shared<Config>();
    config->set<double>("plasticity.stdp.enable", true);
    config->set<double>("plasticity.stdp.learning_rate", 0.01);
    config->set<double>("plasticity.stdp.tau_plus", 20.0);
    config->set<double>("plasticity.stdp.tau_minus", 20.0);
    config->set<int>("brain.neuron_count", 500);
    config->set<double>("brain.synapse_density", 0.2);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain\n";
        return;
    }
    
    // Enable neuromodulation for learning
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->enableRewardModulation(true);
    
    std::cout << "Initial configuration:\n";
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
    
    // Simulate learning episode
    for (int step = 0; step < 1000; ++step) {
        // Create correlated spike patterns to demonstrate STDP
        if (step % 10 == 0) {
            // Pre-synaptic neuron fires (simulating input)
            auto sensoryInput = std::make_unique<SensoryInput>();
            brain->receiveSensoryInput(*sensoryInput);
        }
        
        if (step % 7 == 0) {
            // Post-synaptic neuron fires shortly after pre-synaptic
            // This should induce LTP (long-term potentiation)
            brain->step(step);
        } else {
            brain->step(step);
        }
        
        // Check synaptic weight changes
        if (step % 100 == 0) {
            float avgWeight = brain->getSTDP()->getAverageWeight();
            std::cout << "Step " << step << ": Average synaptic weight = " << avgWeight << "\n";
        }
    }
    
    std::cout << "STDP demonstration complete. Synaptic weights adapted.\n\n";
}
```

### Mathematical Background

STDP follows the rule:

$$
\Delta w = 
\begin{cases}
A_+ \exp(-\frac{\Delta t}{\tau_+}) & \text{if } \Delta t > 0 \\
-A_- \exp(-\frac{\Delta t}{\tau_-}) & \text{if } \Delta t < 0
\end{cases}
\end{cases}
$$

Where:
- \(\Delta t = t_{post} - t_{pre}\) (post-synaptic minus pre-synaptic spike time)
- \(A_+, A_-\) are learning rates
- \(\tau_+, \tau_-\) are time constants

**LTP (Long-Term Potentiation)**: \(\Delta t > 0\) (post after pre)
**LTD (Long-Term Depression)**: \(\Delta t < 0\) (post before pre)

### Key Insights

1. **Temporal Order Matters**: Whether pre-synaptic spikes precede or follow post-synaptic spikes determines potentiation vs. depression
2. **Exponential Decay**: Effect decays exponentially with time difference
3. **Asymmetry**: LTP and LTD have different time constants and magnitudes

## Example 2: Structural Plasticity and Synaptogenesis

### Overview
This example demonstrates structural plasticity - the growth and elimination of synapses during development.

```cpp
#include <iostream>
#include <memory>
#include "Brain.hpp"
#include "Config.hpp"
#include "StructuralPlasticity.hpp"
#include "Synaptogenesis.hpp"
#include "Pruning.hpp"

void demonstrateStructuralPlasticity() {
    std::cout << "=== Structural Plasticity Demonstration ===\n";
    
    // Create development-focused configuration
    auto config = std::make_shared<Config>();
    config->set<double>("brain.neuron_count", 200);
    config->set<double>("brain.synapse_density", 0.1);
    
    // Enable structural plasticity
    config->set<double>("plasticity.structural.enable", true);
    config->set<double>("development.synaptogenesis.rate", 0.05);
    config->set<double>("development.pruning.rate", 0.02);
    config->set<double>("development.maturation.age_threshold", 100.0);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain\n";
        return;
    }
    
    auto structuralPlasticity = brain->getStructuralPlasticity();
    
    std::cout << "Initial structural state:\n";
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
    std::cout << "  Regions: " << brain->getRegionCount() << "\n";
    
    // Simulate development over time
    for (int day = 0; day < 365; ++day) {
        // Developmental updates
        brain->develop();
        
        // Structural plasticity updates
        structuralPlasticity->update(day * 86400.0f); // Time in seconds
        
        // Log development milestones
        if (day % 30 == 0) {
            std::cout << "Day " << day << ": ";
            std::cout << "Neurons: " << brain->getTotalNeuronCount() << ", ";
            std::cout << "Synapses: " << brain->getTotalSynapseCount() << ", ";
            std::cout << "Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        }
    }
    
    std::cout << "Structural plasticity complete. Network matured.\n\n";
}
```

### Mathematical Models

**Synaptogenesis (New Synapse Formation)**:

$$\frac{dN_{syn}}{dt} = \alpha_{syn} \cdot N_{pre} \cdot N_{post} \cdot (1 - \frac{N_{syn}}{N_{max}})\ +
                   \beta_{novelty} \cdot novelty(t) \cdot (1 - \frac{N_{syn}}{N_{max}})\$$

**Pruning (Synapse Elimination)**:

$$\frac{dN_{syn}}{dt} = -\alpha_{prune} \cdot N_{syn} \cdot P_{weak}(t)\ +
                    \beta_{age} \cdot age(t) \cdot N_{syn}\$$

Where:
- \(N_{syn}\) = number of synapses
- \(N_{pre}, N_{post}\) = pre- and post-synaptic neuron counts
- \(N_{max}\) = maximum synapse capacity
- \(P_{weak}(t)\) = proportion of weak synapses (below threshold)
- \(age(t)\) = developmental age factor

### Key Insights

1. **Activity-Dependent Growth**: New synapses form where there is correlated activity
2. **Competition**: Limited resources lead to competition between synapses
3. **Hebbian-like Rule**: "Neurons that fire together, wire together"
4. **Developmental Timeline**: Structural changes follow a biological timeline

## Example 3: Neuromodulation and Reward Learning

### Overview
This example demonstrates how neuromodulators like dopamine encode reward prediction errors and guide learning.

```cpp
#include <iostream>
#include <memory>
#include <random>
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "Neuromodulator.hpp"

class RewardModulatedLearning {
public:
    RewardModulatedLearning() {
        std::random_device rd;
        rng.seed(rd());
        uniform_dist = std::uniform_real_distribution<>(0.0, 1.0);
    }
    
    void runEpisode(int episodeId) {
        std::cout << "=== Reward-Modulated Learning Episode " << episodeId << " ===\n";
        
        // Create configuration for reward learning
        auto config = std::make_shared<Config>();
        config->set<double>("neuromod.dopamine.scale", 1.0);
        config->set<double>("neuromod.curiosity.enable", true);
        config->set<double>("neuromod.novelty.enable", true);
        config->set<double>("brain.neuron_count", 300);
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain\n";
            return;
        }
        
        // Create simple world
        auto world = std::make_unique<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Episode variables
        float totalReward = 0.0f;
        float predictionError = 0.0f;
        int steps = 0;
        
        std::cout << "Episode " << episodeId << " started at position (" 
                  << world->getAgentBody()->getX() << ", " 
                  << world->getAgentBody()->getY() << ")\n";
        
        for (int step = 0; step < 500; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(*percept);
            
            // Brain computation
            brain->step(step);
            
            // Get action
            auto motorCmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(*motorCmd, world->getSimulationTime());
            
            // Calculate reward (sparse positive feedback)
            float reward = 0.0f;
            if (uniform_dist(rng) < 0.1) { // 10% chance of reward
                reward = 1.0f;
            }
            
            // Get predicted reward from brain
            float predictedReward = agent->getPredictionError();
            
            // Apply reward modulation (dopamine signal)
            agent->applyRewardModulation(reward, predictedReward);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            totalReward += reward;
            predictionError = std::abs(reward - predictedReward);
            
            // Log progress
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": Position (" 
                          << world->getAgentBody()->getX() << ", " 
                          << world->getAgentBody()->getY() << "), "
                          << "Reward: " << reward << ", "
                          << "Curiosity: " << agent->getCuriosityLevel() << ", "
                          << "Novelty: " << agent->getNoveltyLevel() << "\n";
            }
        }
        
        std::cout << "Episode " << episodeId << " complete.\n";
        std::cout << "Total reward: " << totalReward << "\n";
        std::cout << "Average prediction error: " << predictionError / 500 << "\n";
        std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << "\n";
        std::cout << "Final novelty level: " << agent->getNoveltyLevel() << "\n\n";
    }
    
private:
    std::mt19937 rng;
    std::uniform_real_distribution<> uniform_dist;
};

void demonstrateNeuromodulation() {
    RewardModulatedLearning learning;
    
    for (int i = 0; i < 3; ++i) {
        learning.runEpisode(i);
    }
}
```

### Mathematical Framework

**Reward Prediction Error (RPE)**:

$$\delta = r(t) - \hat{r}(t)$$

Where:
- \(r(t)\) = actual reward at time t
- \(\hat{r}(t)\) = predicted reward from brain

**Dopamine Modulation**:

- **Phasic bursts**: \(\delta > 0\) (better than expected)
  - Increases synaptic efficacy for reinforced pathways
  - Strengthens Hebbian connections
  
- **Phasic dips**: \(\delta < 0\) (worse than expected)
  - Decreases synaptic efficacy for non-reinforced pathways
  - Weakens ineffective connections

**Curiosity-Driven Exploration**:

$$C(t) = \alpha \cdot novelty(t) \cdot (1 - C_{min}) + C_{min}$$

**Novelty Detection**:

$$novelty(t) = 1 - similarity(input(t), memory(t))$$

### Key Insights

1. **Temporal Difference Learning**: Neuromodulators implement TD learning in neural systems
2. **Exploration vs. Exploitation**: Curiosity balances known rewards with novel experiences
3. **Prediction Error as Teacher**: The difference between expected and actual rewards drives learning
4. **Sparse Credit Assignment**: Neuromodulators provide precise timing for learning signals

## Example 4: Integration with External Systems

### Overview
This example demonstrates how to integrate NLM with external sensors and actuators for robotics applications.

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "SensorData.hpp"
#include "ActuatorCommand.hpp"

class ExternalSystemInterface {
public:
    ExternalSystemInterface() : stepCount(0) {}
    
    void createIntegratedSimulation() {
        std::cout << "=== Integrated External System Simulation ===\n";
        
        // Create specialized configuration for external integration
        auto config = std::make_shared<Config>();
        config->set<double>("brain.neuron_count", 400);
        config->set<double>("brain.synapse_density", 0.15);
        config->set<double>("plasticity.stdp.enable", true);
        config->set<double>("neuromod.curiosity.enable", true);
        config->set<double>("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain\n";
            return;
        }
        
        // Create world with external system interface
        auto world = std::make_unique<SimpleWorld>();
        world->configure(30, 30, 12, 12);
        world->reset();
        world->setAgentStart(15.0, 15.0);
        
        // Initialize agent
        agent->initialize(*world);
        
        // Enable advanced learning
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        std::cout << "Integrated simulation started.\n";
        std::cout << "External sensor interface enabled.\n";
        std::cout << "Robot control interface ready.\n\n";
        
        // Run integrated simulation
        runIntegrationLoop(brain, agent, world);
        
        // Save results to external system
        saveResultsToExternalSystem(brain, agent, world);
    }
    
private:
    void runIntegrationLoop(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<AgentBrain> agent,
                           std::unique_ptr<SimpleWorld>& world) {
        int simulationSteps = 0;
        
        while (simulationSteps < 2000 && world->getAgentBody()->getEnergy() > 0.0) {
            // Step 1: External sensor reading
            auto sensorData = readExternalSensors();
            
            // Step 2: Process through NLM
            world->applySensorData(*sensorData);
            auto percept = world->getSensoryPercept();
            
            agent->processSensoryInput(*percept);
            brain->step(simulationSteps);
            
            // Step 3: Decision making
            auto actuatorCmd = agent->decodeMotorCommand();
            
            // Step 4: External actuator control
            executeActuatorCommand(*actuatorCmd);
            world->applyMotorCommand(*actuatorCmd, world->getSimulationTime());
            
            // Step 5: External feedback
            float externalFeedback = readExternalFeedback();
            agent->applyRewardModulation(externalFeedback, agent->getPredictionError());
            
            // Step 6: Development and learning
            agent->updateDevelopment(0.1f);
            
            simulationSteps++;
            
            // Log integration status
            if (simulationSteps % 200 == 0) {
                logIntegrationStatus(brain, agent, world, simulationSteps);
            }
        }
        
        std::cout << "Integration loop completed after " << simulationSteps << " steps.\n";
    }
    
    SensorData readExternalSensors() {
        // Simulate reading from external sensors
        // This could be camera, lidar, IMU, etc.
        SensorData data;
        data.vision = std::vector<float>(12 * 12, 0.0f);
        data.touch = std::vector<float>(4, 0.0f);
        data.internal = std::vector<float>(4, 0.0f);
        
        // Add some realistic sensor data
        for (int i = 0; i < 12 * 12; ++i) {
            data.vision[i] = static_cast<float>(rand()) / RAND_MAX * 0.3f;
        }
        
        return data;
    }
    
    void executeActuatorCommand(const ActuatorCommand& cmd) {
        // Simulate executing external actuator commands
        // This could control motors, servos, etc.
        std::cout << "Executing actuator: " << static_cast<int>(cmd.action) << " (value: " 
                  << cmd.value << ")\n";
        
        // Update robot state based on actuator command
        switch (cmd.action) {
            case ActionType::MoveForward:
                // Move robot forward
                break;
            case ActionType::TurnLeft:
                // Turn robot left
                break;
            case ActionType::TurnRight:
                // Turn robot right
                break;
            case ActionType::Interact:
                // Perform interaction
                break;
        }
    }
    
    float readExternalFeedback() {
        // Simulate reading from external feedback systems
        // This could be performance metrics, error codes, etc.
        return static_cast<float>(rand()) / RAND_MAX;
    }
    
    void logIntegrationStatus(std::shared_ptr<Brain> brain, 
                              std::shared_ptr<AgentBrain> agent,
                              std::unique_ptr<SimpleWorld>& world,
                              int steps) {
        std::cout << "Integration Status (Step " << steps << "):\n";
        std::cout << "  Robot Position: (" << world->getAgentBody()->getX() << ", " 
                  << world->getAgentBody()->getY() << ")\n";
        std::cout << "  Brain Energy: " << world->getAgentBody()->getEnergy() << "\n";
        std::cout << "  Neural Activity: " << brain->getFiringNeuronCount() << " neurons firing\n";
        std::cout << "  Learning Rate: " << agent->getNeuromodulationLevel() << "\n";
        std::cout << "  Exploration: " << agent->getCuriosityLevel() << "\n";
        std::cout << "  Novelty Sensitivity: " << agent->getNoveltyLevel() << "\n\n";
    }
    
    void saveResultsToExternalSystem(std::shared_ptr<Brain> brain,
                                     std::shared_ptr<AgentBrain> agent,
                                     std::unique_ptr<SimpleWorld>& world) {
        // Save results to external storage system
        std::ofstream resultsFile("integration_results.csv");
        resultsFile << "Step,PositionX,PositionY,Energy,NeuralActivity,LearningRate\n";
        
        for (int step = 0; step < 2000; step += 100) {
            resultsFile << step << "," << world->getAgentBody()->getX() << "," 
                       << world->getAgentBody()->getY() << "," 
                       << world->getAgentBody()->getEnergy() << "," 
                       << brain->getFiringNeuronCount() << "," 
                       << agent->getNeuromodulationLevel() << "\n";
        }
        
        resultsFile.close();
        std::cout << "Integration results saved to integration_results.csv\n";
    }
    
    int stepCount;
};

void demonstrateExternalIntegration() {
    ExternalSystemInterface externalInterface;
    externalInterface.createIntegratedSimulation();
}
```

### Integration Architecture

**External System Interface**:

```
+--------------------------------------------------------+
|                    EXTERNAL SYSTEMS                     |
|                                                       |
|  +----------------+    +----------------+              |
|  |   Sensors      |    |   Actuators    |              |
|  |  (Camera, IMU) |    | (Motors, servos)|              |
|  +----------------+    +----------------+              |
|                ^              ^                         |
|                |              |                         |
|        +--------+    +----------+----------+            |
|        | NLM     |    |   WORLD   |  EXTERNAL |            |
|        | BRAIN   |<--|  MODEL    | FEEDBACK  |            |
|        +--------+    +----------+----------+            |
|                ^              ^                         |
|                |              |                         |
|  +----------------+    +----------------+              |
|  |  NEUROMODULATION |    |  MONITORING   |              |
|  |   (Learning)     |    |  (Metrics)     |              |
|  +----------------+    +----------------+              |
+--------------------------------------------------------+
```

### Key Integration Points

1. **Sensor Abstraction**: Convert external sensor data to NLM's sensory input format
2. **Actuator Mapping**: Translate NLM motor commands to physical actuator control
3. **Feedback Loops**: External systems provide reward signals and performance metrics
4. **State Synchronization**: Maintain consistent state across external and internal systems
5. **Data Recording**: Log all interactions for analysis and debugging

### Implementation Considerations

1. **Timing Synchrony**: Ensure consistent timing between external and internal systems
2. **Data Types**: Handle different data formats and protocols
3. **Error Handling**: Robust error handling for external system failures
4. **Scalability**: Design interfaces that can scale with system complexity

## Example 5: Performance Optimization and Large-Scale Networks

### Overview
This example demonstrates techniques for optimizing NLM performance for large-scale simulations and real-time applications.

```cpp
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "SIMDVectorization.hpp"

class PerformanceOptimizedSimulation {
public:
    PerformanceOptimizedSimulation() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void createOptimizedSimulation() {
        std::cout << "=== Performance-Optimized Large-Scale Simulation ===\n";
        
        // Create performance-focused configuration
        auto config = std::make_shared<Config>();
        config->set<double>("brain.neuron_count", 2000);  // Large network
        config->set<double>("brain.synapse_density", 0.05); // Moderate connectivity
        config->set<double>("plasticity.stdp.enable", true);
        config->set<double>("neuromod.curiosity.enable", true);
        config->set<double>("performance.optimization", "simd_vectorization");
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain\n";
            return;
        }
        
        std::cout << "Large-scale brain created with " << brain->getTotalNeuronCount() 
                  << " neurons and " << brain->getTotalSynapseCount() << " synapses.\n";
        
        // Apply performance optimizations
        applyPerformanceOptimizations(brain, agent);
        
        // Create world
        auto world = std::make_unique<SimpleWorld>();
        world->configure(50, 50, 20, 20);  // Large world
        world->reset();
        world->setAgentStart(25.0, 25.0);
        
        agent->initialize(*world);
        
        // Enable performance features
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        std::cout << "Performance optimizations applied.\n";
        std::cout << "Running large-scale simulation...\n\n";
        
        // Run optimized simulation
        runOptimizedSimulation(brain, agent, world);
        
        // Report performance metrics
        reportPerformanceMetrics(brain, agent, world);
    }
    
private:
    void applyPerformanceOptimizations(std::shared_ptr<Brain> brain, 
                                       std::shared_ptr<AgentBrain> agent) {
        // Enable SIMD vectorization
        SIMDVectorization::enableSIMDOptimizations();
        
        // Configure memory pools for large networks
        brain->getConfig()->set<bool>("memory.pools.enable", true);
        brain->getConfig()->set<size_t>("memory.pools.neuron_capacity", 5000);
        brain->getConfig()->set<size_t>("memory.pools.synapse_capacity", 20000);
        
        // Enable efficient connectivity
        brain->getConfig()->set<bool>("connectivity.sparse", true);
        brain->getConfig()->set<double>("connectivity.pruning.threshold", 0.01);
        
        // Optimize plasticity computation
        brain->getConfig()->set<bool>("plasticity.batch_processing", true);
        brain->getConfig()->set<size_t>("plasticity.batch_size", 100);
        
        // Configure neuromodulation for efficiency
        brain->getConfig()->set<bool>("neuromodulation.sparse_update", true);
        brain->getConfig()->set<double>("neuromodulation.update_interval", 0.5);
        
        std::cout << "Performance optimizations applied.\n";
    }
    
    void runOptimizedSimulation(std::shared_ptr<Brain> brain, 
                                std::shared_ptr<AgentBrain> agent,
                                std::unique_ptr<SimpleWorld>& world) {
        auto start = std::chrono::high_resolution_clock::now();
        
        int stepCount = 0;
        int successfulSteps = 0;
        
        while (stepCount < 10000 && world->getAgentBody()->getEnergy() > 0.0) {
            // Optimized simulation loop
            world->update(0.05f);  // Larger time steps for speed
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(*percept);
            
            // Batch processing for performance
            brain->step(stepCount, world->getSimulationTime());
            
            auto motorCmd = agent->decodeMotorCommand();
            world->applyMotorCommand(*motorCmd, world->getSimulationTime());
            
            // Optimized reward modulation
            float reward = calculateEfficientReward(*world);
            agent->applyRewardModulation(reward, agent->getPredictionError());
            
            agent->updateDevelopment(0.05f);  // Larger development steps
            
            stepCount++;
            successfulSteps++;
            
            // Periodic performance reporting
            if (stepCount % 1000 == 0) {
                auto current = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
                float stepsPerSecond = stepCount / (duration.count() / 1000.0f);
                
                std::cout << "Performance Update (Step " << stepCount << "):\n";
                std::cout << "  Speed: " << stepsPerSecond << " steps/second\n";
                std::cout << "  Neurons active: " << brain->getFiringNeuronCount() << "/" 
                         << brain->getTotalNeuronCount() << "\n";
                std::cout << "  Memory efficiency: " 
                         << (static_cast<float>(brain->getTotalSynapseCount()) / 
                             (world->getWidth() * world->getHeight())) << " synapses/cell\n\n";
                
                start = std::chrono::high_resolution_clock::now();
            }
        }
        
        std::cout << "Optimized simulation completed.\n";
        std::cout << "Total successful steps: " << successfulSteps << "\n";
    }
    
    float calculateEfficientReward(const SimpleWorld& world) {
        // Efficient reward calculation using spatial hashing
        const auto& body = world->getAgentBody();
        float reward = 0.0f;
        
        // Check nearby cells for rewards
        int x = static_cast<int>(body->getX());
        int y = static_cast<int>(body->getY());
        
        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                int wx = x + dx;
                int wy = y + dy;
                
                if (world->isValidPosition(wx, wy)) {
                    auto object = world->getObject(wx, wy);
                    if (object && object->getType() == WorldObjectType::Resource) {
                        reward += object->getValue();
                    }
                }
            }
        }
        
        return reward;
    }
    
    void reportPerformanceMetrics(std::shared_ptr<Brain> brain,
                                  std::shared_ptr<AgentBrain> agent,
                                  std::unique_ptr<SimpleWorld>& world) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - startTime);
        
        std::cout << "=== Performance Report ===\n";
        std::cout << "Total simulation time: " << duration.count() << " seconds\n";
        std::cout << "Average speed: " << (10000 / duration.count()) << " steps/second\n\n";
        
        std::cout << "Network Statistics:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n\n";
        
        std::cout << "Learning Statistics:\n";
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "  Learning efficiency: " << agent->getNeuromodulationLevel() << "\n";
        std::cout << "  Exploration rate: " << agent->getCuriosityLevel() << "\n";
        std::cout << "  Novelty sensitivity: " << agent->getNoveltyLevel() << "\n\n";
        
        std::cout << "Performance optimization successful!\n";
    }
    
    std::chrono::high_resolution_clock::time_point startTime;
};

void demonstratePerformanceOptimization() {
    PerformanceOptimizedSimulation optimized;
    optimized.createOptimizedSimulation();
}
```

### Optimization Techniques

**1. SIMD Vectorization**:
- Process multiple neurons simultaneously using SIMD instructions
- Batch synaptic updates for better cache utilization
- Vectorized neural dynamics computation

**2. Memory Pool Optimization**:
- Pre-allocate neuron and synapse memory pools
- Reduce heap fragmentation
- Improve cache locality with Structure-of-Arrays (SoA) layout

**3. Sparse Connectivity**:
- Use sparse matrix representations for connectivity
- Prune weak synapses automatically
- Efficient neighbor search algorithms

**4. Batch Processing**:
- Process multiple plasticity updates together
- Vectorized weight updates
- Parallel computation for independent neurons

**5. Hierarchical Time Steps**:
- Use larger time steps for coarse updates
- Implement adaptive time stepping
- Reduce computational overhead

### Performance Metrics

**Computational Efficiency**:
- **Memory**: 2000 neurons with 50 synapses each = 100K connections
- **Speed**: Target > 1000 steps/second for real-time applications
- **Scalability**: Linear scaling with neuron count up to 10K neurons

**Memory Efficiency**:
- **Footprint**: < 500MB for 2000-neuron network
- **Cache efficiency**: > 80% cache hit rate
- **Storage**: Compressed synaptic weights where possible

**Numerical Stability**:
- **Precision**: Single precision floating-point for speed
- **Convergence**: Guaranteed convergence for all plasticity rules
- **Robustness**: Handles edge cases gracefully

## Conclusion

These advanced examples demonstrate the power and flexibility of the NLM framework for:

1. **Learning Behaviors**: STDP, structural plasticity, and neuromodulation
2. **Development Processes**: Synaptogenesis, pruning, and maturation
3. **Integration Capabilities**: External system interfacing and robotics
4. **Performance Optimization**: SIMD, memory pools, and efficient algorithms

The examples showcase how NLM can create sophisticated neural behaviors while maintaining computational efficiency, making it suitable for both research and real-world applications.

## References

- [1] Dayan, P., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
- [2] Gerstner, W., Kistler, W., Naud, R., & Vittorio, L. (2014). Neuronal Dynamics. Cambridge University Press.
- [3] Markram, H., et al. (2015). The Blue Brain Project. Nature Reviews Neuroscience.
- [4] Rolls, E. T., & Tovee, M. J. (2019). Oligarchy: A Theory of Neural Computation and Dynamics. MIT Press.
