# User Guide

## Introduction

This guide provides step-by-step instructions for using the NLM (熙然) neural simulation framework. It covers everything from basic setup and running your first simulation to advanced configuration and custom experiment creation.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Running Simulations](#running-simulations)
3. [Interpreting Results](#interpreting-results)
4. [Creating Custom Experiments](#creating-custom-experiments)
5. [Saving and Loading Brain States](#saving-and-loading-brain-states)

## Getting Started

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (GCC 9+, Clang 9+, MSVC 2019+)
- **Memory**: 4GB RAM minimum (8GB+ recommended)
- **Disk Space**: 500MB free space

### Installation

#### Step 1: Clone the Repository

```bash
git clone https://github.com/nlm/nlm.git
cd nlm
```

#### Step 2: Build the Project

The project uses CMake for build configuration. Run the following commands:

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build . --config Release
```

#### Step 3: Run the Basic Demo

After building, you can run the basic demo to verify the installation:

```bash
# Run basic demo
./nlm

# Or run phase 3 demo
./nlm_phase3_demo

# Or run phase 4 demo
./nlm_phase4_demo

# Or run phase 6 demo
./nlm_phase6_demo
```

#### Step 4: Verify Installation

The demo should display output showing:
- System initialization
- Brain configuration
- Simulation progress
- Results summary

If you encounter any errors, check the following:

1. **Compiler Errors**: Ensure your compiler supports C++17
2. **Missing Dependencies**: Install required dependencies (nlohmann/json, etc.)
3. **Permission Issues**: Check write permissions in the working directory

### First Simulation

Let's run your first simulation with a simple configuration:

#### Create a Simple Configuration File

Create a file called `simple_config.json`:

```json
{
    "simulation": {
        "timestep": 0.001,
        "max_steps": 100,
        "enable_progress_bar": true,
        "enable_logging": false
    },
    "brain": {
        "neuron_count": 100,
        "region_count": 1
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true
    }
}
```

#### Run with Custom Configuration

```bash
./nlm --config simple_config.json
```

The simulation will run for 100 steps and display progress. You can watch the neurons firing and synapses strengthening over time.

### Understanding the Basic Demo

The basic demo (`./nlm`) demonstrates:

1. **System Initialization**: Setting up the neural system
2. **Brain Configuration**: Creating neurons and synapses
3. **Simulation Loop**: Running the simulation for specified steps
4. **Result Output**: Displaying simulation statistics

Watch for these key outputs:
- "Neuron count: X"
- "Synapse count: Y"
- "Simulation step: Z"
- "Firing rate: A Hz"
- "Average weight: B"

## Running Simulations

### Running Different Simulation Types

NLM provides several pre-built simulation scenarios. Each serves different purposes:

#### Phase 3: Basic Neural Computation

The Phase 3 demo (`./nlm_phase3_demo`) demonstrates:
- Basic neural dynamics with LIF neurons
- Simple spike-timing-dependent plasticity (STDP)
- Hebbian learning
- Structural plasticity (synaptogenesis/pruning)

**Usage**:
```bash
./nlm_phase3_demo
```

**Configuration Options**:
```bash
./nlm_phase3_demo --help
```

#### Phase 4: Emerging Cognition

The Phase 4 demo (`./nlm_phase4_demo`) demonstrates:
- Working memory and episodic memory systems
- Prediction and planning capabilities
- Cognitive processes and concept formation
- Action selection and motor control

**Usage**:
```bash
./nlm_phase4_demo
```

#### Phase 6: Integrated Intelligence

The Phase 6 demo (`./nlm_phase6_demo`) demonstrates:
- Integration of all neural components
- Advanced cognitive functions
- Developmental processes
- Neuromodulation effects

**Usage**:
```bash
./nlm_phase6_demo
```

### Running Custom Simulations

You can create and run custom simulations by:

#### 1. Creating a Custom Brain Configuration

Create a file `custom_brain.json`:

```json
{
    "simulation": {
        "timestep": 0.0001,
        "max_steps": 500,
        "enable_progress_bar": true
    },
    "brain": {
        "neuron_count": 200,
        "region_count": 2,
        "region_0_type": "Cortical",
        "region_1_type": "Hippocampal"
    },
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural_plasticity": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    },
    "development": {
        "enable_development": true,
        "developmental_stages": 15
    },
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_novelty": true,
        "enable_curiosity": true
    }
}
```

#### 2. Creating a Custom Main Program

Create your own simulation program:

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    // Load configuration
    auto config = std::make_unique<Config>();
    config->loadFromFile("custom_brain.json");
    
    // Create brain with configuration
    auto brain = std::make_unique<Brain>(std::move(config));
    
    // Run simulation
    for (uint64_t step = 0; step < config->get<uint64_t>("simulation.max_steps").value(); ++step) {
        brain->step(step);
    }
    
    // Output results
    brain->printSummary();
    
    return 0;
}
```

#### 3. Building and Running Custom Programs

```bash
# Add your custom source file
cmake --build . --config Release
cd ..

# Run your program
./your_program
```

## Interpreting Results

### Understanding Simulation Output

When you run a simulation, you'll see various output messages. Here's what they mean:

#### Brain Statistics

- **Neuron Count**: Number of neurons in the simulation
- **Synapse Count**: Number of synaptic connections
- **Firing Rate**: Average firing rate across all neurons (Hz)
- **Average Weight**: Average synaptic weight
- **Connection Density**: Ratio of actual connections to potential connections

#### Plasticity Statistics

- **STDP Updates**: Number of STDP learning events
- **Hebbian Updates**: Number of Hebbian learning events
- **Weight Changes**: Total magnitude of weight changes
- **Plasticity Rate**: Rate of synaptic change

#### Memory Statistics

- **Working Memory Usage**: Current working memory utilization
- **Episodic Memory Count**: Number of stored episodes
- **Memory Retrieval Success**: Success rate of memory retrieval

#### Neuromodulation Statistics

- **Dopamine Level**: Current dopamine concentration
- **Novelty Detection**: Novelty detection rate
- **Curiosity Level**: Exploration motivation level

### Analyzing Output Files

#### Log Files

The system can log detailed information to files:

1. **Console Output**: Real-time simulation progress
2. **Log Files**: Detailed event logs
3. **Result Files**: Final simulation results

#### Result Files

Result files are typically saved in the `results/` directory:

- `results/brain_state.json` - Final brain state
- `results/metrics.json` - Performance metrics
- `results/experiment_data.json` - Experiment data

### Visualizing Results

#### Command-Line Visualization

Use the built-in visualization tools:

```bash
# Generate visualization data
./nlm --visualize

# View results
./nlm --view-results results/
```

#### Python Visualization

Install Python visualization tools:

```bash
pip install matplotlib numpy pandas
```

Create visualization scripts:

```python
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Load results
data = pd.read_json('results/experiment_data.json')

# Plot firing rates
plt.figure(figsize=(12, 4))
plt.subplot(1, 3, 1)
plt.plot(data['step'], data['firing_rate'])
plt.title('Firing Rate Over Time')
plt.xlabel('Simulation Step')
plt.ylabel('Firing Rate (Hz)')

# Plot synaptic weights
plt.subplot(1, 3, 2)
plt.plot(data['step'], data['average_weight'])
plt.title('Average Synaptic Weight')
plt.xlabel('Simulation Step')
plt.ylabel('Weight')

# Plot plasticity events
plt.subplot(1, 3, 3)
plt.hist(data['plasticity_events'], bins=50)
plt.title('Plasticity Events Distribution')
plt.xlabel('Event Count')
plt.ylabel('Frequency')

plt.tight_layout()
plt.savefig('results/visualization.png')
plt.show()
```

### Performance Analysis

#### Monitoring System Performance

Use the performance monitoring tools:

```bash
# Monitor system resources
./nlm --monitor

# Generate performance report
./nlm --performance-report results/performance_report.json
```

#### Performance Metrics

Key performance metrics to monitor:

- **Computation Time**: Time per simulation step
- **Memory Usage**: RAM and VRAM usage
- **Scalability**: Performance with increasing neuron count
- **Accuracy**: Simulation accuracy compared to expected results

## Creating Custom Experiments

### Experiment Framework

NLM provides a flexible experiment framework for creating custom experiments:

#### Basic Experiment Structure

```cpp
#include "src/experiments/Experiment.hpp"
#include "src/brain/Brain.hpp"

class CustomExperiment : public Experiment {
public:
    CustomExperiment(const std::string& name) : Experiment(name) {}
    
    void run(Brain& brain) override {
        // Record initial state
        record("initial_firing_rate", brain.getAverageFiringRate());
        record("initial_synapse_count", brain.getSynapseCount());
        
        // Run simulation
        for (uint64_t step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Record metrics
            if (step % 10 == 0) {
                record("firing_rate_step", brain.getAverageFiringRate(), step);
                record("weight_change", brain.getTotalWeightChange(), step);
            }
        }
        
        // Record final state
        record("final_firing_rate", brain.getAverageFiringRate());
        record("final_synapse_count", brain.getSynapseCount());
    }
};
```

#### Creating and Running Experiments

```cpp
#include "src/experiments/ExperimentRunner.hpp"

int main() {
    // Create experiment runner
    ExperimentRunner runner;
    
    // Add custom experiments
    runner.addExperiment(std::make_unique<CustomExperiment>("Basic Learning"));
    runner.addExperiment(std::make_unique<CustomExperiment>("Plasticity Study"));
    runner.addExperiment(std::make_unique<CustomExperiment>("Development Process"));
    
    // Run all experiments
    runner.runAll();
    
    // Save results
    runner.saveResults("custom_experiments.json");
    
    return 0;
}
```

### Example Experiment Types

#### Learning Experiment

Study learning processes with different plasticity rules:

```cpp
class LearningExperiment : public Experiment {
public:
    LearningExperiment(const std::string& name, const std::string& plasticityType)
        : Experiment(name), plasticityType_(plasticityType) {}
    
    void run(Brain& brain) override {
        // Enable specific plasticity
        brain.enablePlasticity(plasticityType_ == "stdp", true);
        brain.enablePlasticity(plasticityType_ == "hebbian", true);
        brain.enablePlasticity(plasticityType_ == "reward", true);
        
        // Run learning simulation
        for (uint64_t step = 0; step < 200; ++step) {
            brain.step(step);
            record("learning_step", step);
        }
    }
private:
    std::string plasticityType_;
};
```

#### Development Experiment

Study developmental processes:

```cpp
class DevelopmentExperiment : public Experiment {
public:
    DevelopmentExperiment(const std::string& name, int stages)
        : Experiment(name), developmentalStages_(stages) {}
    
    void run(Brain& brain) override {
        // Enable development
        brain.enableDevelopment(true);
        
        // Run developmental simulation
        for (int stage = 0; stage < developmentalStages_; ++stage) {
            brain.advanceDevelopmentalStage();
            
            for (uint64_t step = 0; step < 50; ++step) {
                brain.step(step);
                record("developmental_stage", stage);
                record("plasticity_rate", brain.getPlasticityRate(), step);
            }
        }
    }
private:
    int developmentalStages_;
};
```

#### Neuromodulation Experiment

Study neuromodulation effects:

```cpp
class NeuromodulationExperiment : public Experiment {
public:
    NeuromodulationExperiment(const std::string& name, float rewardStrength)
        : Experiment(name), rewardStrength_(rewardStrength) {}
    
    void run(Brain& brain) override {
        // Enable neuromodulation
        brain.enableNeuromodulation(true);
        
        // Set neuromodulation parameters
        brain.setDopamineLevel(rewardStrength_);
        brain.setNoveltyThreshold(0.1f);
        brain.setCuriosityStrength(0.5f);
        
        // Run neuromodulation simulation
        for (uint64_t step = 0; step < 300; ++step) {
            brain.step(step);
            
            // Simulate reward events
            if (step % 50 == 0) {
                brain.signalReward(1.0f);
            }
            
            record("dopamine_level", brain.getDopamineLevel(), step);
            record("curiosity_level", brain.getCuriosityLevel(), step);
        }
    }
private:
    float rewardStrength_;
};
```

### Experiment Configuration

Configure experiments through JSON:

```json
{
    "experiments": [
        {
            "name": "Basic Learning",
            "type": "learning",
            "plasticity": ["stdp", "hebbian"],
            "duration": 100,
            "recording_interval": 10
        },
        {
            "name": "Development Process",
            "type": "development",
            "stages": 15,
            "duration_per_stage": 50,
            "recording_interval": 5
        },
        {
            "name": "Neuromodulation Effects",
            "type": "neuromodulation",
            "reward_strength": 0.8,
            "novelty_threshold": 0.1,
            "duration": 300,
            "recording_interval": 20
        }
    ]
}
```

Run experiments with configuration:

```bash
./nlm --experiments experiments_config.json
```

## Saving and Loading Brain States

### Saving Brain States

Save brain states for later use or analysis:

#### Save Current Brain State

```bash
# Save brain state to file
./nlm --save-brain state1.json
```

This creates a file `state1.json` containing:
- Neuron states (positions, voltages, firing rates)
- Synapse connections (weights, delays, types)
- Plasticity history
- Development state
- Neuromodulation state

#### Save Brain State with Configuration

```bash
# Save with specific filename
./nlm --save-brain my_brain_state.json

# Save with timestamp
./nlm --save-brain brain_state_$(date +%Y%m%d_%H%M%S).json
```

### Loading Brain States

#### Load Brain State

```bash
# Load brain state
./nlm --load-brain state1.json
```

This loads the brain state from `state1.json` and continues the simulation from that point.

#### Create Initial State from File

```bash
# Create new brain from saved state
./nlm --create-from-state state1.json
```

This creates a completely new brain simulation initialized with the saved state.

#### Copying Brain States

```bash
# Copy brain state to new file
cp state1.json state2.json

# Load copy
./nlm --load-brain state2.json
```

### Brain State Format

Brain states are saved in JSON format:

```json
{
    "version": "1.0",
    "timestamp": "2024-01-15T10:30:00Z",
    "brain": {
        "neuron_count": 100,
        "synapse_count": 500,
        "neurons": [
            {
                "id": 0,
                "type": "Excitatory",
                "state": {
                    "membrane_potential": -70.5,
                    "firing_rate": 15.2,
                    "refractory_remaining": 0,
                    "last_spike_time": 45.2
                }
            }
        ],
        "synapses": [
            {
                "id": 0,
                "source": 0,
                "destination": 1,
                "weight": 0.35,
                "delay": 2,
                "type": "excitatory",
                "plasticity_flags": {
                    "hebbian_enabled": true,
                    "stdp_enabled": true
                }
            }
        ]
    },
    "plasticity": {
        "stdp_params": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "time_constant": 20.0
        },
        "history": [
            {"step": 10, "event": "LTP", "weight_change": 0.005},
            {"step": 20, "event": "LTD", "weight_change": -0.003}
        ]
    },
    "development": {
        "stage": 3,
        "plasticity_rate": 0.7,
        "critical_period_active": false
    },
    "neuromodulation": {
        "dopamine_level": 0.6,
        "novelty_level": 0.2,
        "curiosity_level": 0.4
    }
}
```

### Brain State Management

#### Listing Saved States

```bash
# List all saved brain states
./nlm --list-brain-states

# Find states from specific time periods
./nlm --list-brain-states --from "2024-01-01" --to "2024-01-15"
```

#### Backup Brain States

```bash
# Create backup directory
mkdir -p backups/$(date +%Y%m%d)

# Backup all brain states
cp *.json backups/$(date +%Y%m%d)/

# Create backup with metadata
./nlm --backup --name "weekly_backup"
```

#### Restore Brain States

```bash
# Restore specific backup
./nlm --restore-backup backups/20240115_backup.json

# Restore latest backup
./nlm --restore-backup backups/latest.json
```

### Brain State Integration

#### Resume Previous Simulations

```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <fstream>

Brain loadBrainFromFile(const std::string& filename) {
    // Load from JSON file
    std::ifstream file(filename);
    nlohmann::json j;
    file >> j;
    
    // Create brain with saved configuration
    auto config = std::make_unique<Config>();
    // Parse configuration from JSON
    
    auto brain = std::make_unique<Brain>(std::move(config));
    
    // Load neuron states
    // Load synapse states
    // Load plasticity state
    // Load development state
    // Load neuromodulation state
    
    return brain;
}

int main() {
    // Check if saved state exists
    if (std::filesystem::exists("saved_brain.json")) {
        Brain brain = loadBrainFromFile("saved_brain.json");
        std::cout << "Loaded brain from saved state" << std::endl;
    } else {
        // Create new brain
        auto config = std::make_unique<Config>();
        config->loadFromFile("default_config.json");
        Brain brain(std::move(config));
        std::cout << "Created new brain" << std::endl;
    }
    
    // Continue simulation
    for (uint64_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Save progress periodically
        if (step % 100 == 0) {
            brain.saveToFile("progress_brain.json");
        }
    }
    
    return 0;
}
```

#### Brain State Analysis

```bash
# Analyze brain state differences
./nlm --compare-states state1.json state2.json

# Generate brain state statistics
./nlm --analyze-brain-state state1.json --output statistics.json

# Create brain state visualization
./nlm --visualize-brain-state state1.json --output visualization.png
```

### Best Practices

#### When to Save/Load

1. **Save frequently**: Save brain state every 100-1000 steps
2. **Save before major changes**: Save before modifying brain configuration
3. **Save for analysis**: Save interesting brain states for later analysis
4. **Save for debugging**: Save brain state when encountering errors

#### When to Load

1. **Resume interrupted simulations**: Load from last save point
2. **Reproduce behaviors**: Load specific brain states for testing
3. **Comparative analysis**: Load multiple states for comparison
4. **Training data**: Use saved states as training data

#### File Management

1. **Use descriptive names**: Include timestamps and simulation identifiers
2. **Organize files**: Use directory structure for different types of states
3. **Keep backups**: Maintain backup versions of important states
4. **Document states**: Document the conditions and purpose of each saved state

### Troubleshooting

#### Common Issues

1. **File not found**: Ensure the file path is correct
2. **Permission denied**: Check file permissions
3. **Corrupted JSON**: Validate JSON format before loading
4. **Version mismatch**: Check if the saved state is compatible with current version

#### Error Messages

- "Error loading brain state: File not found" - Check file path
- "Error parsing brain state: Invalid JSON" - Check JSON format
- "Error loading brain state: Incompatible version" - Check version compatibility
- "Error saving brain state: Permission denied" - Check file permissions

## Conclusion

This user guide provides comprehensive instructions for using the NLM neural simulation framework. You now have the tools to:

1. **Get started quickly** with basic setup and first simulation
2. **Run various simulation types** (Phase 3, 4, 6 demos)
3. **Create custom simulations** with unique configurations
4. **Analyze and interpret results** with built-in tools
5. **Create and run custom experiments** using the experiment framework
6. **Save and load brain states** for persistence and analysis

The NLM framework is designed to be flexible and extensible, allowing you to explore a wide range of neural simulation scenarios. Use this guide as a starting point and explore the extensive API for more advanced functionality.