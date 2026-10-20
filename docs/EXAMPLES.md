# NLM (Neural Learning Machine) - Examples and Tutorials

## Overview

This document provides comprehensive examples and tutorials for the NLM (Neural Learning Machine) framework. It includes step-by-step tutorials for different skill levels, working example programs, and real-world use cases.

## Table of Contents

1. [Getting Started Examples](#getting-started-examples)
2. [Basic Usage Examples](#basic-usage-examples)
3. [Advanced Examples](#advanced-examples)
4. [Tutorial Series](#tutorial-series)
5. [Real-World Use Cases](#real-world-use-cases)
6. [Code Snippets and Examples](#code-snippets-and-examples)
7. [Best Practices](#best-practices)
8. [Troubleshooting](#troubleshooting)

## Getting Started Examples

### 1. Minimal Working Example

The simplest possible NLM program that creates a brain and runs a simulation:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create a simple configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100);
    
    // Create and initialize the brain
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run a simple simulation
    std::cout << "Starting simulation..." << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain.getTotalSpikeCount() << " spikes" << std::endl;
        }
    }
    
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}
```

### 2. Python Minimal Example

The simplest possible NLM program using Python:

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation
for step in range(100):
    brain.step(step)

print("Simulation complete!")
```

### 3. Configuration File Example

Create a configuration file for different brain sizes:

```json
// config_small.json
{
    "brain": {
        "neuron_count": 100,
        "synapse_density": 0.05
    },
    "plasticity": {
        "stdp_learning_rate": 0.001
    }
}
```

```json
// config_large.json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.1,
        "region_count": 5
    },
    "plasticity": {
        "stdp_learning_rate": 0.001,
        "synaptogenesis_rate": 0.001
    },
    "performance": {
        "parallel_threads": 4
    }
}
```

## Basic Usage Examples

### 1. Brain Simulation with Statistics

Track and analyze brain activity over time:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Create configuration with statistics collection
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create and initialize brain
    Brain brain(config);
    brain.initialize();
    
    // Simulation parameters
    int num_steps = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "Running simulation with " << config->get("brain.neuron_count") 
              << " neurons for " << num_steps << " steps" << std::endl;
    
    // Run simulation with statistics
    for (int step = 0; step < num_steps; ++step) {
        brain.step(step);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
            
            std::cout << "Step " << step << ": "
                      << "Time: " << duration.count() << "ms, "
                      << "Spikes: " << brain.getTotalSpikeCount() << ", "
                      << "Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    // Final statistics
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "\nSimulation Summary:" << std::endl;
    std::cout << "Total Steps: " << num_steps << std::endl;
    std::cout << "Total Duration: " << total_duration.count() << " ms" << std::endl;
    std::cout << "Average Steps per Second: " << (num_steps / (total_duration.count() / 1000.0)) << std::endl;
    std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

### 2. Agent-Based Simulation

Create an agent that interacts with an environment:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    
    // Create brain and agent
    Brain brain(config);
    AgentBrain agentBrain(brain);
    
    // Create and configure world
    SimpleWorld world;
    world.configure(width=100, height=100, visionWidth=8, visionHeight=8);
    world.reset();
    world.setAgentStart(50.0, 50.0);
    
    // Initialize agent
    agentBrain.initialize(world);
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    
    // Simulation loop
    std::cout << "Starting agent simulation..." << std::endl;
    for (int step = 0; step < 500; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Run brain simulation
        brain.step(step);
        
        // Generate and apply action
        auto action = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(action.get(), step * 0.1);
        
        // Apply neuromodulation
        double reward = world.getSensoryPercept().getInternal()[0];
        agentBrain.applyRewardModulation(reward, 0.0);
        
        // Print progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "Action: " << action->getType() << ", "
                      << "Curiosity: " << agentBrain.getCuriosityLevel() << ", "
                      << "Novelty: " << agentBrain.getNoveltyLevel() << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    return 0;
}
```

### 3. Memory System Example

Demonstrate different memory systems:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Access memory systems
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    auto associativeMemory = brain.getAssociativeMemory();
    
    std::cout << "Memory Systems Example" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Create sample patterns
    Pattern pattern1 = {1.0f, 0.5f, 0.8f};
    Pattern pattern2 = {0.3f, 0.7f, 0.2f};
    Pattern pattern3 = {0.9f, 0.1f, 0.4f};
    
    // Store patterns in working memory
    std::cout << "Storing patterns in working memory..." << std::endl;
    workingMemory.storePattern(pattern1, 0.9f);
    workingMemory.storePattern(pattern2, 0.7f);
    workingMemory.storePattern(pattern3, 0.8f);
    
    // Retrieve similar patterns
    std::cout << "Retrieving similar patterns..." << std::endl;
    auto similar = workingMemory.retrieveSimilarPatterns(pattern1, 0.6f);
    std::cout << "Found " << similar.size() << " similar patterns" << std::endl;
    
    // Store experiences in episodic memory
    Experience experience1;
    experience1.pattern = pattern1;
    experience1.time = 0;
    experience1.location = Location(10.0, 20.0);
    
    Experience experience2;
    experience2.pattern = pattern2;
    experience2.time = 100;
    experience2.location = Location(30.0, 40.0);
    
    episodicMemory.encodeExperience(experience1);
    episodicMemory.encodeExperience(experience2, Location(30.0, 40.0), TimeContext(50, 150));
    
    // Pattern completion in associative memory
    std::cout << "Performing pattern completion..." << std::endl;
    associativeMemory.formAssociation(pattern1, pattern2);
    Pattern completed = associativeMemory.completePattern(pattern1, 0.7f);
    
    std::cout << "Pattern completion successful!" << std::endl;
    std::cout << "Original: [" << pattern1[0] << ", " << pattern1[1] << ", " << pattern1[2] << "]" << std::endl;
    std::cout << "Completed: [" << completed[0] << ", " << completed[1] << ", " << completed[2] << "]" << std::endl;
    
    return 0;
}
```

### 4. Neuromodulation Example

Demonstrate neuromodulation effects on learning:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <iostream>

int main() {
    // Create configuration with neuromodulation
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 500);
    config->set("neuromod.dopamine.scale", 1.5);
    config->set("neuromod.curiosity.threshold", 0.2);
    config->set("neuromod.novelty.sensitivity", 0.8);
    
    // Create brain and agent
    Brain brain(config);
    AgentBrain agentBrain(brain);
    
    // Create world
    SimpleWorld world;
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8);
    world.reset();
    agentBrain.initialize(world);
    
    // Enable neuromodulation
    agentBrain.enableRewardModulation(true);
    agentBrain.enableCuriosity(true);
    agentBrain.enableNovelty(true);
    
    std::cout << "Neuromodulation Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Simulation with neuromodulation tracking
    for (int step = 0; step < 200; ++step) {
        // Update world
        world.update(0.1);
        
        // Process sensory input
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Run brain
        brain.step(step);
        
        // Track neuromodulation levels
        double curiosity = agentBrain.getCuriosityLevel();
        double novelty = agentBrain.getNoveltyLevel();
        double prediction_error = agentBrain.getPredictionError();
        
        // Apply reward
        double reward = world.getSensoryPercept().getInternal()[0];
        agentBrain.applyRewardModulation(reward, 0.0);
        
        // Print neuromodulation statistics
        if (step % 50 == 0) {
            std::cout << "Step " << step << ": "
                      << "Curiosity: " << curiosity << ", "
                      << "Novelty: " << novelty << ", "
                      << "Prediction Error: " << prediction_error << ", "
                      << "Reward: " << reward << std::endl;
        }
    }
    
    std::cout << "\nNeuromodulation Statistics:" << std::endl;
    std::cout << "Final Curiosity: " << agentBrain.getCuriosityLevel() << std::endl;
    std::cout << "Final Novelty: " << agentBrain.getNoveltyLevel() << std::endl;
    std::cout << "Final Prediction Error: " << agentBrain.getPredictionError() << std::endl;
    
    return 0;
}
```

## Advanced Examples

### 1. Development Simulation

Simulate brain development over time:

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Create configuration with development
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("development.enabled", true);
    config->set("development.stage_duration", 150.0);
    config->set("development.critical_period_start", 30.0);
    config->set("development.critical_period_end", 120.0);
    
    // Create brain and agent
    Brain brain(config);
    AgentBrain agentBrain(brain);
    
    // Create world
    SimpleWorld world;
    world.configure(width=80, height=80, visionWidth=8, visionHeight=8);
    world.reset();
    agentBrain.initialize(world);
    
    // Enable development
    agentBrain.enableDevelopment(true);
    
    std::cout << "Development Simulation Example" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Track developmental stages
    for (int step = 0; step < 500; ++step) {
        // Update world
        world.update(0.1);
        
        // Process sensory input
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Run brain
        brain.step(step);
        
        // Update development
        agentBrain.updateDevelopment(0.1);
        
        // Track developmental stage
        DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
        float stage_progress = agentBrain.getDevelopmentProgress();
        
        // Print developmental information
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "Stage: " << static_cast<int>(stage) << ", "
                      << "Progress: " << stage_progress << "%, "
                      << "Plasticity: " << brain.getPlasticityLevel() << std::endl;
        }
    }
    
    std::cout << "\nDevelopment Simulation Summary:" << std::endl;
    std::cout << "Final Developmental Stage: " << static_cast<int>(agentBrain.getDevelopmentalStage()) << std::endl;
    std::cout << "Final Development Progress: " << agentBrain.getDevelopmentProgress() << "%" << std::endl;
    std::cout << "Final Plasticity Level: " << brain.getPlasticityLevel() << std::endl;
    
    return 0;
}
```

### 2. Python Integration Example

Advanced Python integration with NLM:

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.preprocessing import StandardScaler

def advanced_simulation(num_steps=1000):
    """Advanced NLM simulation with Python integration."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("performance.enable_optimization", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    world.reset()
    agent_brain.initialize(world)
    
    # Enable subsystems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableNovelty(True)
    agent_brain.enableDevelopment(True)
    
    # Data collection
    simulation_data = {
        'step': [],
        'spike_count': [],
        'firing_rate': [],
        'curiosity': [],
        'novelty': [],
        'reward': []
    }
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent_brain.applyRewardModulation(reward, 0.0)
        
        # Record data
        simulation_data['step'].append(step)
        simulation_data['spike_count'].append(brain.getTotalSpikeCount())
        simulation_data['firing_rate'].append(brain.getAverageFiringRate())
        simulation_data['curiosity'].append(agent_brain.getCuriosityLevel())
        simulation_data['novelty'].append(agent_brain.getNoveltyLevel())
        simulation_data['reward'].append(reward)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: Curiosity={agent_brain.getCuriosityLevel():.3f}, "
                  f"Novelty={agent_brain.getNoveltyLevel():.3f}, "
                  f"Reward={reward:.3f}")
    
    return simulation_data

def analyze_simulation_data(data):
    """Analyze simulation data using Python libraries."""
    
    # Convert to DataFrame
    df = pd.DataFrame(data)
    
    # Basic statistics
    print("Simulation Data Analysis:")
    print("=======================")
    print(f"Total steps: {len(df)}")
    print(f"Average spike count: {df['spike_count'].mean():.1f}")
    print(f"Average firing rate: {df['firing_rate'].mean():.2f} Hz")
    print(f"Average curiosity: {df['curiosity'].mean():.3f}")
    print(f"Average novelty: {df['novelty'].mean():.3f}")
    print(f"Average reward: {df['reward'].mean():.3f}")
    
    # Time series plots
    fig, axes = plt.subplots(3, 2, figsize=(15, 12))
    
    # Spike count over time
    axes[0, 0].plot(df['step'], df['spike_count'])
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Spike Count')
    axes[0, 0].set_title('Spike Count Over Time')
    axes[0, 0].grid(True)
    
    # Firing rate over time
    axes[0, 1].plot(df['step'], df['firing_rate'])
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Firing Rate (Hz)')
    axes[0, 1].set_title('Firing Rate Over Time')
    axes[0, 1].grid(True)
    
    # Curiosity over time
    axes[1, 0].plot(df['step'], df['curiosity'])
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Curiosity Level')
    axes[1, 0].set_title('Curiosity Over Time')
    axes[1, 0].grid(True)
    
    # Novelty over time
    axes[1, 1].plot(df['step'], df['novelty'])
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Novelty Level')
    axes[1, 1].set_title('Novelty Over Time')
    axes[1, 1].grid(True)
    
    # Reward over time
    axes[2, 0].plot(df['step'], df['reward'])
    axes[2, 0].set_xlabel('Step')
    axes[2, 0].set_ylabel('Reward')
    axes[2, 0].set_title('Reward Over Time')
    axes[2, 0].grid(True)
    
    # Correlation matrix
    correlation_data = df[['spike_count', 'firing_rate', 'curiosity', 'novelty', 'reward']]
    correlation_matrix = correlation_data.corr()
    
    import seaborn as sns
    sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0, ax=axes[2, 1])
    axes[2, 1].set_title('Correlation Matrix')
    
    plt.tight_layout()
    plt.savefig('simulation_analysis.png')
    plt.show()
    
    # Machine learning preprocessing
    scaler = StandardScaler()
    scaled_data = scaler.fit_transform(df[['spike_count', 'firing_rate', 'curiosity', 'novelty', 'reward']])
    
    # PCA for dimensionality reduction
    from sklearn.decomposition import PCA
    pca = PCA(n_components=2)
    pca_data = pca.fit_transform(scaled_data)
    
    # Plot PCA results
    plt.figure(figsize=(10, 8))
    scatter = plt.scatter(pca_data[:, 0], pca_data[:, 1], c=df['step'], cmap='viridis', alpha=0.6)
    plt.colorbar(scatter)
    plt.xlabel('PC1')
    plt.ylabel('PC2')
    plt.title('Principal Component Analysis')
    plt.grid(True)
    plt.show()
    
    return df, scaled_data, pca_data

# Run advanced simulation and analysis
print("Running advanced NLM simulation...")
data = advanced_simulation(1000)
print("Analyzing simulation data...")
df, scaled_data, pca_data = analyze_simulation_data(data)

print("\nAdvanced simulation complete!")
```

## Tutorial Series

### Tutorial 1: Hello World

**Objective**: Create your first NLM program and understand the basic concepts.

**Prerequisites**:
- Basic C++ or Python knowledge
- Understanding of neural networks (basic concepts)

**Steps**:

1. **Install NLM**

```bash
# For C++
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# For Python
pip install pynlm
```

2. **Create a Simple Brain**

```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 100);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int i = 0; i < 10; ++i) {
        brain.step(i);
    }
    
    std::cout << "Hello, NLM World!" << std::endl;
    return 0;
}
```

3. **Run and Test**

```bash
# Compile and run
cd build
./test_hello_world
```

4. **Expected Output**:

```
Hello, NLM World!
```

### Tutorial 2: Brain Configuration

**Objective**: Learn how to configure and customize NLM brains.

**Steps**:

1. **Understand Configuration System**

NLM uses a key-value configuration system with validation.

2. **Create Custom Configuration**

```cpp
Config config;

// Set various parameters
config.set("brain.neuron_count", 500);
config.set("brain.synapse_density", 0.1);
config.set("plasticity.stdp_learning_rate", 0.001);
config.set("neuromod.dopamine_scale", 1.0);
config.set("world.width", 100);
config.set("world.height", 100);
```

3. **Save and Load Configuration**

```cpp
// Save configuration to file
config.saveToFile("my_config.json");

// Load configuration from file
Config config2;
config2.loadFromFile("my_config.json");
```

4. **Validation**

```cpp
// Validate configuration
if (!config.validate()) {
    std::cout << "Configuration validation failed:" << std::endl;
    config.printValidationErrors();
}
```

### Tutorial 3: Agent-Based Simulation

**Objective**: Learn how to create agents that interact with environments.

**Steps**:

1. **Create Agent Brain**

```cpp
// Create brain and agent
Brain brain(config);
AgentBrain agentBrain(brain);
```

2. **Create and Configure World**

```cpp
SimpleWorld world;
world.configure(width=50, height=50, visionWidth=8, visionHeight=8);
world.reset();
world.setAgentStart(25.0, 25.0);
```

3. **Initialize Agent**

```cpp
agentBrain.initialize(world);
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);
```

4. **Run Simulation Loop**

```cpp
for (int step = 0; step < 100; ++step) {
    world.update(0.1);
    
    auto percept = world.getSensoryPercept();
    agentBrain.processSensoryInput(percept);
    
    brain.step(step);
    
    auto action = agentBrain.decodeMotorCommand();
    world.applyMotorCommand(action.get(), step * 0.1);
}
```

### Tutorial 4: Memory Systems

**Objective**: Explore different memory systems in NLM.

**Steps**:

1. **Access Memory Systems**

```cpp
auto workingMemory = brain.getWorkingMemory();
auto episodicMemory = brain.getEpisodicMemory();
auto associativeMemory = brain.getAssociativeMemory();
```

2. **Working Memory Example**

```cpp
// Store and retrieve patterns
workingMemory.storePattern(pattern1, 0.9f);
Pattern retrieved = workingMemory.retrievePattern(0);
```

3. **Episodic Memory Example**

```cpp
// Encode and retrieve experiences
Experience exp;
exp.pattern = pattern1;
exp.time = 0;
exp.location = Location(10, 20);

episodicMemory.encodeExperience(exp);
Pattern completed = episodicMemory.retrieveExperience(0);
```

4. **Associative Memory Example**

```cpp
// Form associations
associativeMemory.formAssociation(pattern1, pattern2);
Pattern completed = associativeMemory.completePattern(pattern1, 0.7f);
```

### Tutorial 5: Neuromodulation

**Objective**: Learn how neuromodulation affects learning and behavior.

**Steps**:

1. **Enable Neuromodulation**

```cpp
agentBrain.enableRewardModulation(true);
agentBrain.enableCuriosity(true);
agentBrain.enableNovelty(true);
```

2. **Track Neuromodulation Levels**

```cpp
double curiosity = agentBrain.getCuriosityLevel();
double novelty = agentBrain.getNoveltyLevel();
double prediction_error = agentBrain.getPredictionError();
```

3. **Apply Reward Modulation**

```cpp
double reward = world.getSensoryPercept().getInternal()[0];
agentBrain.applyRewardModulation(reward, 0.0);
```

4. **Analyze Effects**

```cpp
// Monitor changes over time
if (curiosity > 0.5) {
    std::cout << "High curiosity - encouraging exploration" << std::endl;
}
```

## Real-World Use Cases

### 1. Cognitive Development Study

Simulate cognitive development in artificial systems:

```cpp
// Configuration for developmental simulation
{
    "development": {
        "enabled": true,
        "stage_duration": 100.0,
        "critical_period_start": 20.0,
        "critical_period_end": 80.0
    },
    "plasticity": {
        "stdp_learning_rate": 0.01,
        "hebbian_learning_rate": 0.02
    }
}
```

**Use Case**: Study how developmental stages affect learning capabilities.

### 2. Exploration vs. Exploitation

Balance exploration and exploitation using curiosity:

```cpp
// Configuration for exploration-exploitation balance
{
    "neuromod": {
        "curiosity": {
            "threshold": 0.3,
            "learning_rate": 0.05
        },
        "novelty": {
            "sensitivity": 0.7
        }
    }
}
```

**Use Case**: Study how agents balance exploring new options vs. exploiting known options.

### 3. Social Learning

Implement social learning capabilities:

```cpp
// Configuration for social learning
{
    "cognition": {
        "social_learning": {
            "enabled": true,
            "imitation_probability": 0.3
        }
    }
}
```

**Use Case**: Study how agents learn from observing others.

### 4. Decision Making

Implement decision-making capabilities:

```cpp
// Configuration for decision making
{
    "cognition": {
        "planning": {
            "horizon": 5,
            "evaluation_weight": 0.8
        },
        "attention": {
            "competition_strength": 1.0
        }
    }
}
```

**Use Case**: Study how agents make complex decisions.

### 5. Memory-Based Reasoning

Implement memory-based reasoning:

```cpp
// Configuration for memory-based reasoning
{
    "memory": {
        "working_memory": {
            "capacity": 1000,
            "decay_rate": 0.1
        },
        "episodic_memory": {
            "max_episodes": 5000,
            "consolidation_rate": 0.05
        },
        "associative_memory": {
            "max_associations": 2000,
            "learning_rate": 0.01
        }
    }
}
```

**Use Case**: Study how agents use memory for reasoning and decision making.

## Code Snippets and Examples

### 1. Quick Configuration Setup

```cpp
// Quick setup for common configurations
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);
config->set("plasticity.stdp_learning_rate", 0.001);
config->set("neuromod.dopamine_scale", 1.0);
```

### 2. Error Handling

```cpp
try {
    config.set("brain.neuron_count", 1000);
    brain.initialize();
    brain.step(0);
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

### 3. Performance Monitoring

```cpp
// Monitor simulation performance
auto start_time = std::chrono::high_resolution_clock::now();
brain.step(step);
auto end_time = std::chrono::high_resolution_clock::now();

auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
std::cout << "Step time: " << duration.count() << " ms" << std::endl;
```

### 4. Checkpoint Management

```cpp
// Create checkpoints
brain.save("checkpoint_step_100.bin");
brain.save("checkpoint_step_500.bin");

// Restore from checkpoint
brain.load("checkpoint_step_100.bin");
```

### 5. Data Collection

```cpp
// Collect simulation data
std::vector<double> spike_counts;
std::vector<double> firing_rates;

for (int step = 0; step < 1000; ++step) {
    brain.step(step);
    spike_counts.push_back(brain.getTotalSpikeCount());
    firing_rates.push_back(brain.getAverageFiringRate());
}

// Save data to file
std::ofstream file("simulation_data.csv");
file << "step,spike_count,firing_rate" << std::endl;
for (size_t i = 0; i < spike_counts.size(); ++i) {
    file << i << "," << spike_counts[i] << "," << firing_rates[i] << std::endl;
}
```

### 6. Visualization

```cpp
// Create visualization data
std::vector<std::vector<double>> neural_activity;
for (int step = 0; step < 100; ++step) {
    brain.step(step);
    neural_activity.push_back(brain.getNeuralActivityData());
}

// Export for visualization tools
import numpy as np
np.save("neural_activity.npy", neural_activity);
```

### 7. Parallel Processing

```cpp
// Enable multi-threading
config.set("performance.parallel_threads", 4);

// Use thread pool for simulation
#include <thread>
#include <vector>

std::vector<std::thread> threads;
for (int i = 0; i < 4; ++i) {
    threads.emplace_back([&, i]() {
        for (int step = i; step < 1000; step += 4) {
            brain.step(step);
        }
    });
}

for (auto& thread : threads) {
    thread.join();
}
```

## Best Practices

### 1. Configuration Management

- **Validate Configuration**: Always validate configuration before using it.
- **Use Templates**: Use configuration templates for common setups.
- **Backup Configuration**: Regularly backup important configurations.
- **Document Configuration**: Document configuration parameters and their effects.

### 2. Memory Management

- **Monitor Memory Usage**: Monitor memory usage during simulation.
- **Use Memory Pools**: Use memory pools for large-scale simulations.
- **Clean Up Resources**: Clean up resources after simulation.
- **Checkpoint Regularly**: Use checkpoints for long simulations.

### 3. Performance Optimization

- **Profile Performance**: Profile simulation performance to identify bottlenecks.
- **Optimize Configuration**: Use optimal configuration for specific use cases.
- **Use Parallel Processing**: Use parallel processing for large networks.
- **Enable Optimizations**: Enable performance optimizations when possible.

### 4. Error Handling

- **Handle Exceptions**: Handle exceptions gracefully.
- **Provide Meaningful Error Messages**: Provide meaningful error messages.
- **Use Validation**: Use validation to catch errors early.
- **Test Error Cases**: Test error cases thoroughly.

### 5. Documentation

- **Document Code**: Document all public APIs.
- **Add Comments**: Add comments for complex code.
- **Provide Examples**: Provide examples for common use cases.
- **Maintain Documentation**: Keep documentation up to date.

## Troubleshooting

### Common Issues

#### Issue 1: Brain Initialization Fails

**Symptom**: Brain initialization fails with error message.

**Possible Causes**:
- Invalid configuration
- Memory allocation failure
- Missing dependencies

**Solutions**:

1. **Check Configuration**:

```cpp
if (!config.validate()) {
    std::cout << "Configuration validation failed:" << std::endl;
    config.printValidationErrors();
}
```

2. **Check Memory Availability**:

```cpp
// Reduce brain size
config.set("brain.neuron_count", 500);
```

3. **Check Dependencies**:

```cpp
// Ensure all required libraries are linked
```

#### Issue 2: Simulation Runs Too Slowly

**Symptom**: Simulation runs too slowly.

**Possible Causes**:
- Too many neurons
- Inefficient code
- Lack of optimizations

**Solutions**:

1. **Reduce Network Size**:

```cpp
config.set("brain.neuron_count", 500);
config.set("brain.synapse_density", 0.05);
```

2. **Enable Optimizations**:

```cpp
config.set("performance.enable_optimization", true);
config.set("performance.parallel_threads", 4);
```

3. **Profile Performance**:

```bash
# Use profiling tools
valgrind --tool=callgrind ./build/tests/test_simulation
```

#### Issue 3: Memory Usage Too High

**Symptom**: Memory usage is too high.

**Possible Causes**:
- Too many neurons
- Memory leaks
- Inefficient data structures

**Solutions**:

1. **Monitor Memory Usage**:

```cpp
// Monitor memory usage
memory_usage = brain.getMemoryUsage();
std::cout << "Memory usage: " << memory_usage / 1024 / 1024 << " MB" << std::endl;
```

2. **Enable Memory Pooling**:

```cpp
brain.enableMemoryPool(512);  // 512MB pool
```

3. **Clean Up Resources**:

```cpp
// Clean up resources
brain.reset();
```

#### Issue 4: Configuration Validation Fails

**Symptom**: Configuration validation fails.

**Possible Causes**:
- Invalid parameter values
- Missing required parameters
- Invalid parameter types

**Solutions**:

1. **Check Parameter Values**:

```cpp
if (config.get("brain.neuron_count") < 100) {
    std::cout << "Neurons must be >= 100" << std::endl;
}
```

2. **Check Required Parameters**:

```cpp
if (!config.has("brain.neuron_count")) {
    std::cout << "Missing required parameter: brain.neuron_count" << std::endl;
}
```

3. **Use Valid Ranges**:

```cpp
// Use valid ranges
config.set("brain.neuron_count", 100, 10000);
```

#### Issue 5: Agent Behavior Unpredictable

**Symptom**: Agent behavior is unpredictable.

**Possible Causes**:
- Complex environment
- Unstable neural dynamics
- Poor action selection

**Solutions**:

1. **Simplify Environment**:

```cpp
world.configure(width=20, height=20, visionWidth=4, visionHeight=4);
```

2. **Stabilize Neural Dynamics**:

```cpp
config.set("brain.synapse_density", 0.05);
config.set("plasticity.stdp_learning_rate", 0.001);
```

3. **Improve Action Selection**:

```cpp
agentBrain.enableCuriosity(true);
agentBrain.enableNovelty(true);
```

## Conclusion

This document provides comprehensive examples and tutorials for the NLM (Neural Learning Machine) framework. It covers:

1. **Getting Started Examples**: Simple examples for beginners
2. **Basic Usage Examples**: Common use cases and patterns
3. **Advanced Examples**: Complex applications and integrations
4. **Tutorial Series**: Step-by-step tutorials for different skill levels
5. **Real-World Use Cases**: Practical applications in various domains
6. **Code Snippets**: Common code patterns and best practices
7. **Best Practices**: Guidelines for effective NLM usage
8. **Troubleshooting**: Common issues and their solutions

The examples and tutorials provided in this document will help you:

1. **Understand NLM Concepts**: Learn about NLM architecture and components
2. **Get Started Quickly**: Create your first NLM program
3. **Build Complex Applications**: Create advanced NLM applications
4. **Debug and Troubleshoot**: Solve common NLM issues
5. **Integrate with Other Tools**: Integrate NLM with Python and other tools

For more information and advanced usage, refer to the API reference and other documentation files in the NLM documentation set.

## References

- [NLM API Reference](API_REFERENCE.md)
- [NLM User Guide](USER_GUIDE.md)
- [NLM Configuration Documentation](CONFIGURATION.md)
- [NLM Python Bindings](PYTHON_BINDINGS.md)
- [NLM Architecture Documentation](ARCHITECTURE.md)
- [NLM Testing Documentation](TESTING.md)
- [NLM Upgrading Guide](UPGRADING_GUIDE.md)

## See Also

- NLM GitHub repository: https://github.com/yourusername/NLM
- NLM documentation: https://nlm.readthedocs.io/
- NLM discussion forum: https://github.com/yourusername/NLM/discussions
- NLM issue tracker: https://github.com/yourusername/NLM/issues

## License

MIT License - See LICENSE file for details.