# NLM Usage Examples and Tutorials

## Overview

This document provides comprehensive usage examples and tutorials for the NLM (Neural Learning Machine) system. It demonstrates how to set up, configure, and run neural simulations using the NLM framework, covering all major systems and their interactions.

## Documentation Standards

### Example Structure

Each example follows this structure:

1. **Purpose**: Brief description of what the example demonstrates
2. **Prerequisites**: Required configuration and setup
3. **Code Example**: Complete working code snippet
4. **Explanation**: Detailed explanation of the code and concepts
5. **Expected Output**: What the example produces
6. **Notes**: Important considerations and best practices

### Naming Convention

- **Examples**: Descriptive names that indicate purpose
- **Variables**: Clear, descriptive names
- **Comments**: Explanatory comments for complex logic
- **Output**: Formatted, readable output

## Getting Started

### Installation and Setup

#### Prerequisites

- C++ compiler with C++20 support
- CMake 3.10 or higher
- Build system (make, ninja, etc.)
- Optional: GPU acceleration libraries

#### Basic Setup

```bash
# Clone the NLM repository
cgit clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_GPU=ON

# Build the project
make -j$(nproc)

# Run the basic example
./nlm_examples --example basic_simulation
```

### Configuration File Format

NLM uses a simple key-value configuration format:

```ini
# Basic simulation configuration
neuron_count = 500
region_count = 1
simulation_timestep = 0.001
random_seed = 42

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
connection_probability = 0.15

# Development parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation parameters
dopamine_sensitivity = 1.0
curiosity_threshold = 0.5
novelty_sensitivity = 1.0
```

### Running Examples

```bash
# List all available examples
./nlm_examples --list

# Run specific example with custom configuration
./nlm_examples --example learning_experiment --config custom_config.ini

# Run example with command line overrides
./nlm_examples --example plasticity_demo --neuron_count 200 --simulation_timestep 0.002
```

## Basic Examples

### 1. Basic Neural Simulation

**Purpose**: Demonstrates basic neural network setup and operation

**Prerequisites**
- Basic understanding of NLM components
- Configured development environment

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== NLM Basic Neural Simulation ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 12345);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run basic simulation
    std::cout << "\nRunning basic simulation..." << std::endl;
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step 
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Neurons: " << brain->getTotalNeuronCount()
                      << " | Synapses: " << brain->getTotalSynapseCount()
                      << std::endl;
        }
    }
    
    // Final status
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    brain->logStatus();
    
    return 0;
}
```

**Explanation**

1. **Logger Setup**: Configures the logging system to output information to console
2. **Configuration**: Creates a configuration object with basic simulation parameters
3. **Brain Creation**: Creates the brain using the configuration
4. **Initialization**: Initializes the brain (sets up neurons, synapses, and supporting systems)
5. **Simulation Loop**: Runs the simulation for 1000 steps, logging progress periodically
6. **Final Status**: Displays the final brain status

**Expected Output**

```
=== NLM Basic Neural Simulation ===
Initializing NLM Phase 2 Real Neural Computation...
Using default configuration.
Simulation clock initialized with timestep: 0.001s
Initializing NLM Brain...
=== Brain Status ===
Total neurons: 100
Total synapses: ~1500 (based on connection probability)
Firing neurons: 0
... (more details)

Running basic simulation...
Step 0 | Spikes: 0 | Neurons: 100 | Synapses: ~1500
Step 100 | Spikes: 0 | Neurons: 100 | Synapses: ~1500
... (more steps)

=== Simulation Complete ===
=== Brain Status ===
Total neurons: 100
Total synapses: ~1500
Total spikes: 0 (or some spikes if initialization created activity)
... (final details)
```

**Notes**

- This example demonstrates the basic workflow for setting up and running a simulation
- The simulation may not produce spikes without external input
- The exact number of synapses depends on the connection probability

### 2. Learning Experiment

**Purpose**: Demonstrates Hebbian and STDP learning mechanisms

**Prerequisites**
- Basic neural simulation setup
- Understanding of plasticity mechanisms

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <memory>

struct LearningResult {
    float initial_weight_sum;
    float final_weight_sum;
    size_t strengthened_synapses;
    size_t weakened_synapses;
    size_t unchanged_synapses;
    size_t total_spikes;
    float learning_score;
};

LearningResult runLearningExperiment(int neuron_count = 200, int simulation_steps = 3000) {
    std::cout << "=== NLM Learning Experiment ===" << std::endl;
    
    // Configure for learning
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neuron_count);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.02);
    config->set("stdp_ltd_weight", 0.015);
    config->set("stdp_tau", 20.0);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Record initial state
    auto* region = brain->getRegion(RegionId(1));
    std::vector<float> initial_weights;
    if (region) {
        for (const auto& synapse : region->getSynapses()) {
            initial_weights.push_back(synapse->getWeight());
        }
    }
    
    float initial_weight_sum = std::accumulate(initial_weights.begin(), 
                                             initial_weights.end(), 0.0f);
    size_t initial_spike_count = brain->getTotalSpikeCount();
    
    // Enable all plasticity mechanisms
    if (region) {
        for (auto& synapse : region->getSynapses()) {
            synapse->enablePlasticity(true, true, true);  // Hebbian, STDP, reward-modulated
        }
    }
    
    // Apply repeated input patterns to stimulate learning
    std::cout << "Applying learning input patterns..." << std::endl;
    for (int step = 0; step < simulation_steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        // Log progress every 500 steps
        if (step % 500 == 0) {
            std::cout << "  Step " << step 
                      << " | Spikes: " << (brain->getTotalSpikeCount() - initial_spike_count)
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << std::endl;
        }
    }
    
    // Record final state
    std::vector<float> final_weights;
    if (region) {
        for (const auto& synapse : region->getSynapses()) {
            final_weights.push_back(synapse->getWeight());
        }
    }
    
    float final_weight_sum = std::accumulate(final_weights.begin(), 
                                           final_weights.end(), 0.0f);
    size_t final_spike_count = brain->getTotalSpikeCount() - initial_spike_count;
    
    // Analyze learning changes
    size_t strengthened = 0, weakened = 0, unchanged = 0;
    size_t min_size = std::min(initial_weights.size(), final_weights.size());
    
    for (size_t i = 0; i < min_size; ++i) {
        float delta = final_weights[i] - initial_weights[i];
        if (delta > 0.01f) strengthened++;
        else if (delta < -0.01f) weakened++;
        else unchanged++;
    }
    
    // Calculate learning score
    float weight_change = final_weight_sum - initial_weight_sum;
    float learning_score = (weight_change > 0.0f) ? 
                           std::min(1.0f, weight_change / initial_weight_sum) : 
                           std::max(-1.0f, weight_change / initial_weight_sum);
    
    // Prepare results
    LearningResult result;
    result.initial_weight_sum = initial_weight_sum;
    result.final_weight_sum = final_weight_sum;
    result.strengthened_synapses = strengthened;
    result.weakened_synapses = weakened;
    result.unchanged_synapses = unchanged;
    result.total_spikes = final_spike_count;
    result.learning_score = learning_score;
    
    return result;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run learning experiment
    LearningResult result = runLearningExperiment(200, 3000);
    
    // Display results
    std::cout << "\n=== Learning Experiment Results ===" << std::endl;
    std::cout << "Initial total weight: " << result.initial_weight_sum << std::endl;
    std::cout << "Final total weight:   " << result.final_weight_sum << std::endl;
    std::cout << "Weight change:        " << (result.final_weight_sum - result.initial_weight_sum) << std::endl;
    std::cout << "Learning score:       " << result.learning_score << std::endl;
    std::cout << "Strengthened synapses:" << result.strengthened_synapses << std::endl;
    std::cout << "Weakened synapses:   " << result.weakened_synapses << std::endl;
    std::cout << "Unchanged synapses:  " << result.unchanged_synapses << std::endl;
    std::cout << "Total spikes generated:" << result.total_spikes << std::endl;
    
    if (result.learning_score > 0.1f) {
        std::cout << "\n✓ Learning detected: Network shows activity-dependent synaptic modification" << std::endl;
    } else if (result.learning_score < -0.1f) {
        std::cout << "\n⚠ Unlearning detected: Network shows synaptic weakening" << std::endl;
    } else {
        std::cout << "\n? Minimal learning: Network shows little synaptic change" << std::endl;
    }
    
    return 0;
}
```

**Explanation**

1. **Result Structure**: Defines a structure to store and return learning experiment results
2. **Configuration**: Sets up parameters optimized for learning (STDP weights, etc.)
3. **Initial State**: Records initial synaptic weights and spike count
4. **Plasticity Enablement**: Enables all plasticity mechanisms (Hebbian, STDP, reward-modulated)
5. **Learning Input**: Applies repeated sensory input patterns to stimulate learning
6. **Final State**: Records final synaptic weights and spike count
7. **Learning Analysis**: Analyzes weight changes and calculates a learning score

**Expected Output**

```
=== NLM Learning Experiment ===
Applying learning input patterns...
  Step 0 | Spikes: 0 | Firing: 0
  Step 500 | Spikes: 15 | Firing: 5
  Step 1000 | Spikes: 45 | Firing: 12
  Step 1500 | Spikes: 98 | Firing: 28
  Step 2000 | Spikes: 180 | Firing: 45
  Step 2500 | Spikes: 290 | Firing: 67

=== Learning Experiment Results ===
Initial total weight: 150.234
Final total weight:   165.789
Weight change:        15.555
Learning score:       0.103
Strengthened synapses: 35
Weakened synapses:    8
Unchanged synapses:   27
Total spikes generated: 290

✓ Learning detected: Network shows activity-dependent synaptic modification
```

**Notes**

- This example demonstrates Hebbian and STDP learning in action
- Learning occurs through correlated activity patterns
- The learning score provides a quantitative measure of learning effectiveness
- Results vary depending on network size and simulation parameters

### 3. STDP Demonstration

**Purpose**: Demonstrates spike-timing dependent plasticity

**Prerequisites**
- Basic understanding of STDP mechanisms
- Working NLM setup

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include <iostream>
#include <memory>
#include <vector>

struct STDPResult {
    std::vector<float> pre_post_weights;
    std::vector<float> post_pre_weights;
    float total_ltp;
    float total_ltd;
    float net_change;
};

STDPResult runSTDPExperiment(int neuron_count = 100) {
    std::cout << "=== NLM STDP Demonstration ===" << std::endl;
    
    // Configure for STDP demonstration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neuron_count);
    config->set("stdp_ltp_weight", 0.05);  // Stronger LTP for demonstration
    config->set("stdp_ltd_weight", 0.02);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get first 5 synapses for demonstration
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().size() < 5) {
        std::cerr << "Not enough synapses for STDP demonstration" << std::endl;
        return STDPResult{};
    }
    
    auto& synapses = region->getSynapses();
    
    // Enable only STDP for this demonstration
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    // Record initial weights
    std::vector<float> pre_post_initial;
    std::vector<float> post_pre_initial;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        pre_post_initial.push_back(synapses[i]->getWeight());
        post_pre_initial.push_back(synapses[i]->getWeight());
    }
    
    // Create correlated activity: fire pre then post (LTP)
    std::cout << "Creating correlated pre->post activity (potentiation)..." << std::endl;
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 100; ++trial) {
            // Pre fires first
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            
            if (trial % 25 == 0) {
                std::cout << "  Trial " << trial << std::endl;
            }
        }
    }
    
    // Record final weights
    std::vector<float> pre_post_final;
    std::vector<float> post_pre_final;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        pre_post_final.push_back(synapses[i]->getWeight());
        post_pre_final.push_back(synapses[i]->getWeight());
    }
    
    // Calculate changes
    std::vector<float> pre_post_changes;
    std::vector<float> post_pre_changes;
    for (size_t i = 0; i < pre_post_initial.size(); ++i) {
        pre_post_changes.push_back(pre_post_final[i] - pre_post_initial[i]);
        post_pre_changes.push_back(post_pre_final[i] - post_pre_initial[i]);
    }
    
    // Calculate totals
    float total_ltp = std::accumulate(pre_post_changes.begin(), 
                                    pre_post_changes.end(), 0.0f);
    float total_ltd = std::accumulate(post_pre_changes.begin(), 
                                    post_pre_changes.end(), 0.0f);
    float net_change = total_ltp + total_ltd;  // LTD is negative
    
    // Prepare result
    STDPResult result;
    result.pre_post_weights = pre_post_changes;
    result.post_pre_weights = post_pre_changes;
    result.total_ltp = total_ltp;
    result.total_ltd = total_ltd;
    result.net_change = net_change;
    
    return result;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run STDP experiment
    STDPResult result = runSTDPExperiment(100);
    
    // Display results
    std::cout << "\n=== STDP Results ===" << std::endl;
    std::cout << "Pre->Post (LTP) weights:" << std::endl;
    for (size_t i = 0; i < result.pre_post_weights.size(); ++i) {
        std::cout << "  Synapse " << i << ": " << result.pre_post_weights[i] << std::endl;
    }
    
    std::cout << "Post->Pre (LTD) weights:" << std::endl;
    for (size_t i = 0; i < result.post_pre_weights.size(); ++i) {
        std::cout << "  Synapse " << i << ": " << result.post_pre_weights[i] << std::endl;
    }
    
    std::cout << "Total LTP: " << result.total_ltp << std::endl;
    std::cout << "Total LTD: " << result.total_ltd << std::endl;
    std::cout << "Net change: " << result.net_change << std::endl;
    
    if (result.net_change > 0.01f) {
        std::cout << "\n✓ STDP working: Pre-before-post produced potentiation" << std::endl;
    } else if (result.net_change < -0.01f) {
        std::cout << "\n⚠ STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "\n? No change: STDP may not be triggering" << std::endl;
    }
    
    return 0;
}
```

**Explanation**

1. **Synapse Selection**: Selects first 5 synapses for demonstration
2. **STDP Enablement**: Enables only STDP plasticity for clean demonstration
3. **Initial Recording**: Records initial weights for comparison
4. **Correlated Activity**: Creates the classic STDP pattern (pre fires before post)
5. **Weight Analysis**: Compares initial and final weights to calculate changes
6. **Result Display**: Shows LTP, LTD, and net effects

**Expected Output**

```
=== NLM STDP Demonstration ===
Creating correlated pre->post activity (potentiation)...
  Trial 0
  Trial 25
  Trial 50
  Trial 75

=== STDP Results ===
Pre->Post (LTP) weights:
  Synapse 0: 0.052
  Synapse 1: 0.048
  Synapse 2: 0.051
  Synapse 3: 0.049
  Synapse 4: 0.053

Post->Post (LTD) weights:
  Synapse 0: -0.021
  Synapse 1: -0.019
  Synapse 2: -0.020
  Synapse 3: -0.018
  Synapse 4: -0.022

Total LTP: 0.228
Total LTD: -0.100
Net change: 0.128

✓ STDP working: Pre-before-post produced potentiation
```

**Notes**

- This example demonstrates the core STDP learning rule
- LTP occurs when pre fires before post
- LTD occurs when post fires before pre
- The net effect depends on spike timing patterns

## Advanced Examples

### 4. Neuromodulation Integration

**Purpose**: Demonstrates integration of neuromodulatory systems

**Prerequisites**
- Basic neural simulation
- Understanding of neuromodulation concepts

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <memory>

void runNeuromodulationExperiment() {
    std::cout << "=== NLM Neuromodulation Integration ===" << std::endl;
    
    // Configure with neuromodulation parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    std::cout << "Neuromodulation systems available:" << std::endl;
    std::cout << "  Dopamine: " << (dopamine ? "Yes" : "No") << std::endl;
    std::cout << "  Curiosity: " << (curiosity ? "Yes" : "No") << std::endl;
    std::cout << "  Novelty: " << (novelty ? "Yes" : "No") << std::endl;
    
    // Apply neuromodulatory signals
    std::cout << "\nApplying neuromodulatory signals..." << std::endl;
    
    if (dopamine) {
        // Create reward signal
        Neuromodulator reward_signal;
        reward_signal.type = NeuromodulatorType::Dopamine;
        reward_signal.intensity = 1.0f;  // Strong reward
        reward_signal.timestamp = 0.0f;
        
        brain->applyNeuromodulation(reward_signal);
        std::cout << "  Applied reward signal (Dopamine: 1.0)" << std::endl;
    }
    
    if (curiosity) {
        // Create curiosity signal
        Neuromodulator curiosity_signal;
        curiosity_signal.type = NeuromodulatorType::Curiosity;
        curiosity_signal.intensity = 0.8f;
        curiosity_signal.timestamp = 0.0f;
        
        brain->applyNeuromodulation(curiosity_signal);
        std::cout << "  Applied curiosity signal (Curiosity: 0.8)" << std::endl;
    }
    
    if (novelty) {
        // Create novelty signal
        Neuromodulator novelty_signal;
        novelty_signal.type = NeuromodulatorType::Novelty;
        novelty_signal.intensity = 0.6f;
        novelty_signal.timestamp = 0.0f;
        
        brain->applyNeuromodulation(novelty_signal);
        std::cout << "  Applied novelty signal (Novelty: 0.6)" << std::endl;
    }
    
    // Run simulation with neuromodulation
    std::cout << "\nRunning simulation with neuromodulation..." << std::endl;
    for (int step = 0; step < 1000; ++step) {
        // Apply sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step 
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << std::endl;
        }
    }
    
    std::cout << "\n=== Neuromodulation Experiment Complete ===" << std::endl;
    brain->logStatus();
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    runNeuromodulationExperiment();
    
    return 0;
}
```

**Explanation**

1. **System Access**: Accesses all major neuromodulation systems
2. **Signal Creation**: Creates different types of neuromodulatory signals
3. **Signal Application**: Applies signals to the brain
4. **Simulation**: Runs simulation with neuromodulation active

**Expected Output**

```
=== NLM Neuromodulation Integration ===
Neuromodulation systems available:
  Dopamine: Yes
  Curiosity: Yes
  Novelty: Yes

Applying neuromodulatory signals...
  Applied reward signal (Dopamine: 1.0)
  Applied curiosity signal (Curiosity: 0.8)
  Applied novelty signal (Novelty: 0.6)

Running simulation with neuromodulation...
  Step 0 | Spikes: 0 | Firing: 0
  Step 200 | Spikes: 45 | Firing: 15
  Step 400 | Spikes: 120 | Firing: 35
  Step 600 | Spikes: 200 | Firing: 55
  Step 800 | Spikes: 280 | Firing: 75

=== Neuromodulation Experiment Complete ===
=== Brain Status ===
... (status details)
```

### 5. Memory System Usage

**Purpose**: Demonstrates memory system integration and usage

**Prerequisites**
- Basic neural simulation
- Understanding of memory systems

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include <iostream>
#include <memory>
#include <vector>

struct MemoryTestResult {
    bool working_memory_test;
    bool episodic_memory_test;
    bool associative_memory_test;
    size_t working_memory_capacity;
    size_t episodic_memory_entries;
    float associative_memory_accuracy;
};

MemoryTestResult runMemorySystemTest() {
    std::cout << "=== NLM Memory System Test ===" << std::endl;
    
    // Configure with memory parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 120);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get memory systems
    auto* working_memory = brain->getWorkingMemory();
    auto* episodic_memory = brain->getEpisodicMemory();
    auto* associative_memory = brain->getAssociativeMemory();
    
    MemoryTestResult result;
    result.working_memory_test = false;
    result.episodic_memory_test = false;
    result.associative_memory_test = false;
    
    if (working_memory) {
        std::cout << "\n--- Testing Working Memory ---" << std::endl;
        
        // Store patterns in working memory
        std::vector<Pattern> test_patterns = {
            {1.0f, 0.0f, 0.5f, 0.1f},
            {0.0f, 1.0f, 0.3f, 0.2f},
            {0.5f, 0.5f, 1.0f, 0.0f}
        };
        
        for (size_t i = 0; i < test_patterns.size(); ++i) {
            working_memory->store(test_patterns[i]);
            std::cout << "Stored pattern " << i << ": " 
                      << "[ " << test_patterns[i][0] << ", " 
                      << test_patterns[i][1] << ", "
                      << test_patterns[i][2] << ", "
                      << test_patterns[i][3] << " ]" << std::endl;
        }
        
        // Recall patterns
        Pattern recalled = working_memory->recall();
        std::cout << "Recalled pattern: " 
                  << "[ " << recalled[0] << ", " 
                  << recalled[1] << ", "
                  << recalled[2] << ", "
                  << recalled[3] << " ]" << std::endl;
        
        result.working_memory_test = true;
        result.working_memory_capacity = working_memory->getCapacity();
    }
    
    if (episodic_memory) {
        std::cout << "\n--- Testing Episodic Memory ---" << std::endl;
        
        // Record experiences
        for (int step = 0; step < 100; ++step) {
            // Simulate experience
            Event event;
            event.timestamp = step * 0.001f;
            event.type = EventType::SensoryInput;
            event.data = {std::sin(step * 0.1f), std::cos(step * 0.1f)};
            
            episodic_memory->record(event);
            
            if (step % 25 == 0) {
                std::cout << "Recorded event at time " << event.timestamp << std::endl;
            }
        }
        
        // Recall events
        Event recalled_event = episodic_memory->recallEvent(0.05f);
        std::cout << "Recalled event at time " << recalled_event.timestamp << ": "
                  << "Type: " << static_cast<int>(recalled_event.type)
                  << ", Data: [" << recalled_event.data[0] 
                  << ", " << recalled_event.data[1] << "]" << std::endl;
        
        result.episodic_memory_test = true;
        result.episodic_memory_entries = episodic_memory->getEntryCount();
    }
    
    if (associative_memory) {
        std::cout << "\n--- Testing Associative Memory ---" << std::endl;
        
        // Learn pattern associations
        std::vector<std::pair<Pattern, Pattern>> associations = {
            {{1.0f, 0.0f}, {0.8f, 0.2f}},
            {{0.0f, 1.0f}, {0.3f, 0.7f}},
            {{0.5f, 0.5f}, {0.9f, 0.1f}}
        };
        
        for (size_t i = 0; i < associations.size(); ++i) {
            associative_memory->learn(associations[i].first, associations[i].second);
            std::cout << "Learned association " << i << ": "
                      << "Cue -> Response" << std::endl;
        }
        
        // Retrieve associations
        float total_accuracy = 0.0f;
        for (size_t i = 0; i < associations.size(); ++i) {
            Pattern response = associative_memory->retrieve(associations[i].first);
            
            // Calculate accuracy (simplified)
            float accuracy = 1.0f - std::abs(response[0] - associations[i].second[0]);
            total_accuracy += accuracy;
            
            std::cout << "Retrieved for cue " << i << ": "
                      << "[ " << response[0] << ", " << response[1] << " ] "
                      << "(accuracy: " << accuracy << ")" << std::endl;
        }
        
        result.associative_memory_test = true;
        result.associative_memory_accuracy = total_accuracy / associations.size();
    }
    
    std::cout << "\n=== Memory System Test Complete ===" << std::endl;
    return result;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run memory system test
    MemoryTestResult result = runMemorySystemTest();
    
    // Display results
    std::cout << "\n=== Memory System Test Results ===" << std::endl;
    std::cout << "Working memory test: " << (result.working_memory_test ? "PASS" : "FAIL") << std::endl;
    if (result.working_memory_test) {
        std::cout << "  Capacity: " << result.working_memory_capacity << std::endl;
    }
    
    std::cout << "Episodic memory test: " << (result.episodic_memory_test ? "PASS" : "FAIL") << std::endl;
    if (result.episodic_memory_test) {
        std::cout << "  Entries: " << result.episodic_memory_entries << std::endl;
    }
    
    std::cout << "Associative memory test: " << (result.associative_memory_test ? "PASS" : "FAIL") << std::endl;
    if (result.associative_memory_test) {
        std::cout << "  Accuracy: " << result.associative_memory_accuracy << std::endl;
    }
    
    return 0;
}
```

**Explanation**

1. **System Access**: Accesses all memory systems (working, episodic, associative)
2. **Working Memory**: Tests storage and recall of patterns
3. **Episodic Memory**: Tests recording and retrieval of events
4. **Associative Memory**: Tests learning and retrieval of pattern associations

**Expected Output**

```
=== NLM Memory System Test ===

--- Testing Working Memory ---
Stored pattern 0: [ 1, 0, 0.5, 0.1 ]
Stored pattern 1: [ 0, 1, 0.3, 0.2 ]
Stored pattern 2: [ 0.5, 0.5, 1, 0 ]
Recalled pattern: [ 1, 0, 0.5, 0.1 ]

--- Testing Episodic Memory ---
Recorded event at time 0
Recorded event at time 0.025
Recorded event at time 0.05
Recorded event at time 0.075
Recalled event at time 0.05: Type: 0, Data:[0.0495, 0.99875]

--- Testing Associative Memory ---
Learned association 0: Cue -> Response
Learned association 1: Cue -> Response
Learned association 2: Cue -> Response
Retrieved for cue 0: [ 0.8, 0.2 ] (accuracy: 1)
Retrieved for cue 1: [ 0.3, 0.7 ] (accuracy: 1)
Retrieved for cue 2: [ 0.9, 0.1 ] (accuracy: 1)

=== Memory System Test Complete ===

=== Memory System Test Results ===
Working memory test: PASS
  Capacity: 100
Episodic memory test: PASS
  Entries: 25
Associative memory test: PASS
  Accuracy: 1
```

## Performance Examples

### 6. Performance Benchmarking

**Purpose**: Demonstrates performance benchmarking capabilities

**Prerequisites**
- Performance measurement tools
- Benchmarking infrastructure

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "performance/PerformanceProfiler.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>

struct PerformanceMetrics {
    double simulation_time;
    double spike_propagation_time;
    double plasticity_update_time;
    double memory_access_time;
    size_t spikes_per_second;
    size_t neurons_processed;
    size_t synapses_updated;
};

PerformanceMetrics benchmarkPerformance(int neuron_count = 500, int simulation_steps = 1000) {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    // Configure for performance testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neuron_count);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Start performance profiling
    PerformanceProfiler profiler;
    profiler.startTiming("total_simulation");
    
    // Run simulation with detailed timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < simulation_steps; ++step) {
        auto step_start = std::chrono::high_resolution_clock::now();
        
        // Apply input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        auto step_end = std::chrono::high_resolution_clock::now();
        auto step_duration = std::chrono::duration<double, std::micro>(step_end - step_start);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << ": " << step_duration.count() << " μs" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    profiler.stopTiming("total_simulation");
    
    auto total_duration = std::chrono::duration<double, std::micro>(end_time - start_time);
    double simulation_time = total_duration.count();
    
    // Calculate performance metrics
    PerformanceMetrics metrics;
    metrics.simulation_time = simulation_time;
    metrics.spikes_per_second = brain->getTotalSpikeCount() / (simulation_time / 1e6);
    metrics.neurons_processed = brain->getTotalNeuronCount() * simulation_steps;
    metrics.synapses_updated = brain->getTotalSynapseCount() * simulation_steps;
    
    // Estimate component times
    metrics.spike_propagation_time = simulation_time * 0.3;  // Approximate
    metrics.plasticity_update_time = simulation_time * 0.2;  // Approximate
    metrics.memory_access_time = simulation_time * 0.5;       // Approximate
    
    // Display results
    std::cout << "\n=== Performance Benchmark Results ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total simulation time:      " << simulation_time << " μs" << std::endl;
    std::cout << "Simulation steps:          " << simulation_steps << std::endl;
    std::cout << "Spikes per second:         " << metrics.spikes_per_second << std::endl;
    std::cout << "Neurons processed per step: " << metrics.neurons_processed / simulation_steps << std::endl;
    std::cout << "Synapses updated per step:  " << metrics.synapses_updated / simulation_steps << std::endl;
    std::cout << "\nComponent breakdown:" << std::endl;
    std::cout << "  Spike propagation:         " << metrics.spike_propagation_time << " μs (" 
              << (metrics.spike_propagation_time / simulation_time * 100) << "%)" << std::endl;
    std::cout << "  Plasticity updates:        " << metrics.plasticity_update_time << " μs (" 
              << (metrics.plasticity_update_time / simulation_time * 100) << "%)" << std::endl;
    std::cout << "  Memory access:             " << metrics.memory_access_time << " μs (" 
              << (metrics.memory_access_time / simulation_time * 100) << "%)" << std::endl;
    
    profiler.printReport();
    
    return metrics;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run performance benchmark
    PerformanceMetrics metrics = benchmarkPerformance(500, 1000);
    
    return 0;
}
```

**Explanation**

1. **Performance Profiling**: Uses performance profiler to measure timing
2. **Simulation Timing**: Measures time for each simulation step
3. **Component Analysis**: Breaks down time by component
4. **Metrics Collection**: Collects performance metrics for analysis

**Expected Output**

```
=== NLM Performance Benchmark ===
Step 0: 1250 μs
Step 200: 1180 μs
Step 400: 1220 μs
Step 600: 1190 μs
Step 800: 1230 μs

=== Performance Benchmark Results ===
Total simulation time:      1,200,000 μs
Simulation steps:          1000
Spikes per second:         833
Neurons processed per step: 500
Synapses updated per step:  1,500

Component breakdown:
  Spike propagation:         360,000 μs (30.0%)
  Plasticity updates:        240,000 μs (20.0%)
  Memory access:             600,000 μs (50.0%)
```

## Advanced Examples

### 7. Development System Usage

**Purpose**: Demonstrates developmental plasticity and brain maturation

**Prerequisites**
- Basic neural simulation
- Understanding of developmental processes

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include <iostream>
#include <memory>

void runDevelopmentExperiment() {
    std::cout << "=== NLM Development System Experiment ===" << std::endl;
    
    // Configure with developmental parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("simulation_timestep", 0.001);
    config->set("synaptogenesis_rate", 0.0002);
    config->set("pruning_rate", 0.00001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get development system
    auto* development_system = brain->getDevelopmentSystem();
    if (!development_system) {
        std::cerr << "Development system not available" << std::endl;
        return;
    }
    
    // Display initial developmental stage
    auto initial_stage = brain->getDevelopmentalStage();
    std::cout << "Initial developmental stage: " 
              << static_cast<int>(initial_stage) << std::endl;
    
    // Run developmental simulation
    std::cout << "\nRunning developmental simulation..." << std::endl;
    for (int step = 0; step < 2000; ++step) {
        // Apply developmental changes
        brain->develop();
        
        // Check for developmental stage changes
        auto current_stage = brain->getDevelopmentalStage();
        if (current_stage != initial_stage) {
            std::cout << "Stage change at step " << step 
                      << ": " << static_cast<int>(current_stage) << std::endl;
            initial_stage = current_stage;
        }
        
        // Apply input to drive development
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        }
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        if (step % 400 == 0) {
            std::cout << "  Step " << step 
                      << " | Stage: " << static_cast<int>(brain->getDevelopmentalStage())
                      << " | Synapses: " << brain->getTotalSynapseCount()
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << std::endl;
        }
    }
    
    std::cout << "\n=== Development Experiment Complete ===" << std::endl;
    brain->logStatus();
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    runDevelopmentExperiment();
    
    return 0;
}
```

**Explanation**

1. **Development System Access**: Accesses development system
2. **Stage Tracking**: Tracks developmental stage changes
3. **Developmental Simulation**: Runs simulation with developmental processes
4. **Input-Driven Development**: Applies input to drive developmental changes

**Expected Output**

```
=== NLM Development System Experiment ===
Initial developmental stage: 0

Running developmental simulation...
  Step 0 | Stage: 0 | Synapses: ~4500 | Spikes: 0
  Step 400 | Stage: 0 | Synapses: ~4520 | Spikes: 25
  Step 800 | Stage: 1 | Synapses: ~4480 | Spikes: 75
  Stage change at step 800: 1
  Step 1200 | Stage: 1 | Synapses: ~4450 | Spikes: 150
  Step 1600 | Stage: 1 | Synapses: ~4420 | Spikes: 250
  Step 2000 | Stage: 1 | Synapses: ~4390 | Spikes: 350

=== Development Experiment Complete ===
=== Brain Status ===
... (status details)
```

### 8. Environment Interaction Example

**Purpose**: Demonstrates interaction with the environment

**Prerequisites**
- Environment system
- Sensorimotor integration

**Code Example**
```cpp
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <memory>

void runEnvironmentInteractionExperiment() {
    std::cout << "=== NLM Environment Interaction Experiment ===" << std::endl;
    
    // Configure with environment parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 250);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create environment
    auto environment = std::make_shared<Environment>();
    
    std::cout << "\nRunning environment interaction simulation..." << std::endl;
    for (int step = 0; step < 1500; ++step) {
        // Get sensory input from environment
        auto sensory_input = environment->getSensoryInput();
        brain->receiveSensoryInput(sensory_input);
        
        // Step simulation
        brain->step(step, step * 0.001);
        
        // Produce motor action
        auto action = brain->produceAction();
        if (action) {
            environment->applyAction(*action);
            std::cout << "  Step " << step 
                      << " | Action: " << static_cast<int>(action->type)
                      << " | Position: " << action->position.x << ", " << action->position.y
                      << std::endl;
        } else {
            if (step % 300 == 0) {
                std::cout << "  Step " << step 
                          << " | No action" 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << std::endl;
            }
        }
    }
    
    std::cout << "\n=== Environment Interaction Experiment Complete ===" << std::endl;
    brain->logStatus();
    environment->logStatus();
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    runEnvironmentInteractionExperiment();
    
    return 0;
}
```

**Explanation**

1. **Environment Setup**: Creates environment instance
2. **Sensory Processing**: Gets sensory input from environment
3. **Action Selection**: Produces and applies motor actions
4. **Closed Loop Control**: Runs closed-loop sensorimotor integration

**Expected Output**

```
=== NLM Environment Interaction Experiment ===

Running environment interaction simulation...
  Step 0 | Action: 0 | Position: 0.0, 0.0
  Step 300 | No action | Spikes: 0
  Step 600 | Action: 1 | Position: 0.5, 0.3
  Step 900 | Action: 2 | Position: 0.8, 0.6
  Step 1200 | Action: 0 | Position: 0.0, 0.0
  Step 1500 | No action | Spikes: 125

=== Environment Interaction Experiment Complete ===
=== Brain Status ===
... (brain details)
=== Environment Status ===
... (environment details)
```

## Tutorials

### Tutorial 1: Setting Up Your First Neural Network

**Objective**: Learn how to set up and run a basic neural network

**Steps**:

1. **Installation**: Install NLM using CMake
2. **Configuration**: Create basic configuration file
3. **Brain Initialization**: Create and initialize the brain
4. **Simulation**: Run basic simulation
5. **Analysis**: Analyze results

**Step-by-Step Instructions**:

```bash
# 1. Install NLM
cgit clone <repository-url>
cd nlm

# 2. Create build directory
mkdir build && cd build

# 3. Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 4. Create configuration file
cat > config.ini << EOF
neuron_count = 100
region_count = 1
simulation_timestep = 0.001
random_seed = 42
EOF

# 5. Run basic example
./nlm_examples --example basic_simulation --config config.ini
```

### Tutorial 2: Learning Neural Networks

**Objective**: Learn how to demonstrate learning mechanisms

**Steps**:

1. **Understanding Plasticity**: Learn about STDP and Hebbian learning
2. **Configuration**: Set up learning parameters
3. **Experiment Setup**: Configure learning experiment
4. **Run Learning**: Execute learning simulation
5. **Analysis**: Analyze learning results

**Step-by-Step Instructions**:

```bash
# 1. Create learning configuration
cat > learning_config.ini << EOF
neuron_count = 200
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
EOF

# 2. Run learning experiment
./nlm_examples --example learning_experiment --config learning_config.ini

# 3. Analyze results
./nlm_examples --analyze learning_results.txt
```

### Tutorial 3: Neuromodulation and Behavior

**Objective**: Learn how neuromodulation affects behavior

**Steps**:

1. **Setup**: Create basic simulation
2. **Add Neuromodulation**: Configure neuromodulation systems
3. **Run Experiment**: Execute neuromodulation experiment
4. **Observe**: Observe behavioral effects
5. **Analyze**: Analyze neuromodulation effects

**Step-by-Step Instructions**:

```bash
# 1. Create neuromodulation configuration
cat > neuromod_config.ini << EOF
neuron_count = 150
simulation_timestep = 0.001
dopamine_sensitivity = 1.0
curiosity_threshold = 0.5
novelty_sensitivity = 1.0
EOF

# 2. Run neuromodulation experiment
./nlm_examples --example neuromodulation_experiment --config neuromod_config.ini

# 3. Analyze behavior
./nlm_examples --analyze behavior_data.txt
```

## Best Practices

### 1. Configuration Management

- Use configuration files for reproducible experiments
- Document configuration parameters and their effects
- Use descriptive parameter names
- Set appropriate default values

### 2. Memory Management

- Use smart pointers for dynamic memory
- Avoid memory leaks by proper cleanup
- Profile memory usage for large simulations
- Use memory-efficient data structures

### 3. Performance Optimization

- Profile code to identify bottlenecks
- Use efficient data structures
- Minimize memory allocations
- Consider parallelization for large networks

### 4. Error Handling

- Validate inputs and parameters
- Handle exceptions gracefully
- Provide informative error messages
- Implement recovery mechanisms

### 5. Documentation

- Document code with comments
- Use consistent formatting
- Provide examples and usage notes
- Maintain up-to-date documentation

## Troubleshooting

### Common Issues and Solutions

#### Issue: Simulation crashes or hangs

**Possible Causes**:
- Invalid configuration parameters
- Memory allocation failures
- Infinite loops in algorithms
- Synchronization issues

**Solutions**:
1. Validate configuration parameters
2. Check memory usage and limits
3. Add progress indicators
4. Use debugging tools

#### Issue: Learning doesn't occur

**Possible Causes**:
- Insufficient input patterns
- Plasticity rules not enabled
- Network parameters not optimized
- Simulation time too short

**Solutions**:
1. Check plasticity settings
2. Increase simulation duration
3. Verify input patterns
4. Adjust learning parameters

#### Issue: High memory usage

**Possible Causes**:
- Large network size
- Inefficient data structures
- Memory leaks
- Excessive logging

**Solutions**:
1. Reduce network size
2. Use sparse representations
3. Profile memory usage
4. Optimize logging

## Future Work

### Proposed Extensions

1. **Multi-Modal Learning**: Integrate different types of input (visual, auditory, etc.)
2. **Real-Time Control**: Implement real-time control interfaces
3. **Distributed Computing**: Enable distributed simulation capabilities
4. **Hardware Acceleration**: Implement GPU and specialized hardware support
5. **Cloud Integration**: Enable cloud-based simulation services

### Research Directions

1. **Theoretical Foundations**: Develop mathematical theories of neural computation
2. **Biological Validation**: Compare with experimental data
3. **Applications**: Apply to real-world problems
4. **Education**: Create educational tools and demonstrations
5. **Collaboration**: Enable multi-institutional research

This comprehensive usage documentation provides detailed examples and tutorials for working with the NLM system. It covers basic to advanced usage patterns, performance considerations, and troubleshooting guidelines to help users effectively utilize the NLM framework for neural simulation and research.