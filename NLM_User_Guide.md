# NLM - Neural Learning Machine User Guide

**Version:** 0.1.0 (Phase 6: Final Integration)

**Author:** NLM Research Team

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. This guide provides comprehensive documentation for using the NLM system, including the new command-line interface (CLI) and advanced features.

## Quick Start

### Basic Usage

1. **Install NLM:**
   ```bash
   # Build the project
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the Phase 6 Integration Demo:**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Use the NLM Command Line Interface:**
   ```bash
   # Show help
   nlm-cli help
   
   # Build the project
   nlm-cli build
   
   # Run Phase 6 demo with custom settings
   nlm-cli phase6-demo --neuron_count 1000 --max_steps 5000
   
   # Save a checkpoint
   nlm-cli save-checkpoint my_checkpoint
   
   # Load a checkpoint
   nlm-cli load-checkpoint my_checkpoint
   ```

## System Architecture

### Core Components

1. **Brain:** The central neural processing unit implementing:
   - Real LIF neuron dynamics
   - Event-driven spike propagation
   - STDP and Hebbian plasticity
   - Structural plasticity
   - Integrated memory systems (working, episodic, associative)
   - Prediction system
   - Cognitive systems (planner, concept formation, attention)
   - Neuromodulation (dopamine, curiosity, novelty)
   - Development system

2. **Agent:** Interface between brain and environment
   - Sensory processing
   - Motor control
   - Reward modulation
   - Structural plasticity control

3. **World:** Simulation environment
   - Grid-based world
   - Action execution
   - Reward computation

4. **Experiment Framework:** Comprehensive testing and validation
   - Phase 6 integration experiments
   - Checkpoint testing
   - Replay system testing

### Phase 6 Integration

Phase 6 represents the final integration stage where all NLM systems work together as a coherent artificial brain:

- **Memory Systems:** Working memory, episodic memory, and associative memory are connected to neural processing
- **Neuromodulation:** Dopamine, curiosity, and novelty systems affect plasticity and neural dynamics
- **Prediction System:** Integrated with learning and memory
- **Development:** Plasticity rates affected by developmental stages
- **Checkpointing:** Save/load functionality working
- **Replay:** Episodic memory replay during rest/consolidation

## Command Line Interface (CLI)

The NLM CLI provides a comprehensive interface for controlling the NLM brain simulation.

### Available Commands

#### `nlm-cli build`
Build the NLM project using CMake and make.

**Options:**
- Automatically detects build directory
- Uses Release configuration
- Runs with 4 parallel jobs

#### `nlm-cli phase6-demo [options]`
Run the Phase 6 integration demo with custom settings.

**Options:**
- `--neuron_count N`: Number of neurons (default: 500)
- `--max_steps N`: Maximum simulation steps (default: 2000)

**Example:**
```bash
nlm-cli phase6-demo --neuron_count 1000 --max_steps 5000
```

#### `nlm-cli phase2-demo`
Run the Phase 2 basic demo demonstrating real neural computation.

#### `nlm-cli save-checkpoint <name>`
Save the current brain state to a checkpoint file.

**Arguments:**
- `<name>`: Name of the checkpoint (will be saved as `{name}.bin`)

#### `nlm-cli load-checkpoint <name>`
Load a brain state from a checkpoint file.

**Arguments:**
- `<name>`: Name of the checkpoint (will be loaded from `{name}.bin`)

#### `nlm-cli status`
Show current system status and available commands.

#### `nlm-cli help`
Show this help message.

### CLI Usage Examples

#### Building the Project
```bash
# Build from source
nlm-cli build

# The build system will:
# 1. Create a build directory
# 2. Run CMake configuration
# 3. Compile all components
# 4. Generate executables (nlm, nlm_phase6_demo, etc.)
```

#### Running Phase 6 Integration Demo
```bash
# Default settings
nlm-cli phase6-demo

# Custom settings
nlm-cli phase6-demo --neuron_count 2000 --max_steps 10000

# This will run:
# 1. Integration verification
# 2. Memory integration tests
# 3. Neuromodulation integration tests
# 4. Checkpoint save/load tests
# 5. Replay system tests
# 6. Full integration experiment with metrics collection
```

#### Checkpoint Management
```bash
# Save current brain state
nlm-cli save-checkpoint my_simulation_state

# Later, load the saved state
nlm-cli load-checkpoint my_simulation_state
```

#### System Monitoring
```bash
# View system status
nlm-cli status

# View detailed help
nlm-cli help
```

## Configuration

### Configuration Files

The NLM system uses configuration files for customization:

#### Default Configuration (`configs/default.cfg`)
```ini
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Configuration Parameters

#### Neuron Configuration
- `neuron_count`: Total number of neurons in the brain
- `region_count`: Number of neural regions (subsets of neurons)
- `connection_probability`: Probability of synaptic connections between neurons

#### Plasticity Configuration
- `plasticity_learning_rate`: Base learning rate for plasticity rules
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_time_constant`: Time constant for STDP eligibility traces

#### Development Configuration
- `development_synaptogenesis_rate`: Rate of new synapse formation
- `development_pruning_rate`: Rate of synapse elimination

#### Neuromodulation Configuration
- `dopamine_baseline`: Baseline dopamine level

#### Environment Configuration
- `environment_name`: Type of environment (GridWorld, etc.)
- `environment_width`, `environment_height`: Environment dimensions

#### Simulation Configuration
- `max_simulation_steps`: Maximum number of simulation steps
- `simulation_time_limit`: Maximum simulation time (seconds)

### Custom Configuration

Create a custom configuration file and use it with:

```bash
# Using command line
./nlm_phase6_demo --config my_config.cfg

# Or using the CLI
nlm-cli run --config my_config.cfg
```

## Advanced Features

### Expert Mode

The NLM system includes advanced features for expert users:

#### Direct API Access
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create and configure brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 2000);
config->set("random_seed", 12345);

auto brain = std::make_shared<Brain>(config);
if (brain->initialize()) {
    // Access integrated systems
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    auto* dopamine = brain->getDopamine();
    auto* prediction = brain->getPredictionSystem();
    
    // Run simulation
    brain->step(0, 0.0);
    
    // Access metrics
    float firingRate = brain->getAverageFiringRate();
    size_t totalSpikes = brain->getTotalSpikeCount();
    
    // Save checkpoint
    brain->save("my_checkpoint.bin");
}
```

#### Custom Experiments
```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"

Phase6IntegratedExperiment experiment;

// Run custom experiment with specific configuration
Phase6Config config;
config.neuronCount = 1500;
config.maxSteps = 3000;
config.enableCheckpointing = true;
config.enableReplay = true;
config.enableDevelopment = true;

auto result = experiment.run(config);

// Analyze results
if (result.memoryWorkingMemoryIntegrated) {
    std::cout << "✓ Working memory integrated" << std::endl;
}
if (result.checkpointingWorks) {
    std::cout << "✓ Checkpointing working" << std::endl;
}
```

### Performance Monitoring

The NLM system provides comprehensive performance monitoring:

#### Available Metrics

1. **Neural Metrics:**
   - Average firing rate
   - Total spike count
   - Active neuron count
   - Synaptic weight statistics

2. **Memory Metrics:**
   - Working memory traces
   - Episodic memory episodes
   - Memory consolidation status

3. **Neuromodulation Metrics:**
   - Dopamine levels
   - Curiosity levels
   - Novelty detection

4. **System Metrics:**
   - Simulation time
   - Memory usage
   - CPU utilization

#### Monitoring Commands
```bash
# View current status
nlm-cli status

# Get detailed metrics (if supported by your terminal)
./nlm_phase6_demo --metrics
```

## Troubleshooting

### Common Issues

#### Build Errors
**Problem:** CMake or make fails to build
**Solution:**
1. Check system requirements
2. Ensure CMake 3.16 or higher is installed
3. Run with verbose output: `make VERBOSE=1`
4. Check for missing dependencies

#### Runtime Errors
**Problem:** Simulation crashes or produces errors
**Solution:**
1. Check configuration parameters
2. Verify file permissions
3. Use logging to debug (enable verbose logging)
4. Run with smaller simulation parameters

#### Memory Issues
**Problem:** Out of memory errors
**Solution:**
1. Reduce neuron count in configuration
2. Disable replay or checkpointing if not needed
3. Increase system memory
4. Use checkpointing to save intermediate states

### Help Resources

#### Documentation
- This guide (NLM User Guide)
- Project documentation in `docs/` directory
- API documentation (if available)

#### Online Resources
- NLM project repository
- Research papers and publications
- Community forums

#### Support
- Check issue tracker for known problems
- Report bugs with detailed error messages
- Provide configuration files and logs when reporting issues

## License

MIT License

Copyright (c) 2024 NLM Research Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.