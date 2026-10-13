# NLM (熙然) - Quick Start Guide

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain system. It creates a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Features

- **Phase 6 Integration**: Complete integration of all neural systems
- **Developmental Learning**: Neural systems that develop and adapt over time
- **Multiple Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, and novelty signals
- **Prediction and Planning**: Forward models and neural planning
- **Persistence**: Checkpoint save/load and experience replay
- **Performance Optimized**: SIMD, multithreading, memory pools

## Quick Installation

### Prerequisites
- C++17 compiler (GCC 9+, Clang 10+, MSVC 19.2+)
- CMake 3.16 or higher
- Make or Ninja build system

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Build with tests (optional)
make test
```

### Run Demos

#### Phase 6 Integration Demo
```bash
# Run the main simulation
./nlm

# Or run the Phase 6 integration demo
./nlm_phase6_demo
```

#### Experimental Demos
```bash
# Run Phase 3 demo (basic neural simulation)
./nlm_phase3_demo

# Run Phase 4 demo (cognitive functions)
./nlm_phase4_demo
```

## Configuration

### Default Configuration

NLM comes with several pre-configured setups:

#### Default (Minimal)
```bash
# For quick testing with basic functionality
neuron_count = 100
region_count = 1
```

#### Research (Full Setup)
```bash
# Recommended for research and development
neuron_count = 5000
region_count = 4
connection_probability = 0.15
enable_development = true
enable_checkpointing = true
enable_replay = true
```

### Configuration File Formats

#### JSON Configuration
Save as `config.json`:
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "enable_development": true,
    "enable_checkpointing": true,
    "enable_replay": true
}
```

#### YAML Configuration
Save as `config.yaml`:
```yaml
neuron_count: 1000
region_count: 1
connection_probability: 0.1
simulation_timestep: 0.001
random_seed: 42
enable_development: true
enable_checkpointing: true
enable_replay: true
```

### Command Line Configuration

Override configuration with command line arguments:

```bash
# Basic override
./nlm --neuron_count=2000 --enable_development=true

# Multiple overrides
./nlm --neuron_count=2000 --region_count=2 --random_seed=12345

# With configuration file
./nlm --config=config.json --enable_replay=true
```

## Running Simulations

### Basic Simulation
```bash
# Run a simple simulation with default settings
./nlm
```

### Custom Simulation
```bash
# Run with custom configuration
./nlm --config=my_config.json --enable_development=true
```

### Long-running Simulation
```bash
# Run a longer simulation (e.g., 10000 steps)
./nlm --neuron_count=500 --region_count=2 --max_steps=10000
```

## Monitoring and Debugging

### Console Output
NLM provides detailed logging to the console:

```
=== NLM Brain Status (Phase 6 - Integrated) ===
Regions: 1
Total neurons: 1000
Total synapses: 9500
Active neurons: 45
Firing neurons (this step): 12
Total spikes: 1234
Pending spike events: 23
Average firing rate: 12.34
E/I ratio: 1.23
Working memory traces: 8
Episodic memory episodes: 5
Dopamine level: 0.45
Developmental stage: 2
```

### Checkpoint Management

#### Save Checkpoint
```bash
# During simulation, NLM automatically saves checkpoints
# Checkpoints are saved to ./checkpoints/ directory
# You can also manually save checkpoints:
./nlm --save_checkpoint=./manual_checkpoint.bin
```

#### Load Checkpoint
```bash
# Load a saved checkpoint
./nlm --load_checkpoint=./manual_checkpoint.bin
```

### Profiling and Performance

#### Performance Profiling
```bash
# Profile with VTune (Intel)
./nlm --profile=vtune

# Profile with GNU profilers
gprof ./nlm simulation.gprof

# Profile memory usage
valgrind --tool=massif ./nlm
```

#### Performance Metrics
NLM reports key performance metrics:

- **Firing Rate**: Average neuronal firing rate (Hz)
- **Memory Usage**: Peak memory consumption
- **CPU Utilization**: Processing efficiency
- **Checkpoint Speed**: Save/load performance

## Advanced Features

### Custom Neural Architectures

#### Create Custom Neuron Types
```cpp
// In your custom code
class CustomNeuron : public Neuron {
public:
    void customDynamics(Timestamp time, TimestepDuration dt) override {
        // Your custom dynamics
        stepLIF(time, dt);
        // Add custom behavior
        applyCustomModulation(time, dt);
    }
};
```

#### Register Custom Neurons
```cpp
// In Brain initialization or configuration
brain->registerNeuronType("custom", &CustomNeuron::create);
```

### Custom Memory Systems

#### Implement Custom Memory
```cpp
class CustomMemorySystem : public IMemorySystem {
public:
    void initialize(Brain* brain) override {
        // Your initialization
    }
    
    void store(const std::vector<float>& pattern) override {
        // Your storage logic
    }
    
    std::vector<float> retrieve() const override {
        // Your retrieval logic
        return std::vector<float>();
    }
};
```

### Extending the Brain

#### Add New Neuromodulators
```cpp
class CustomNeuromodulator : public Neuromodulator {
public:
    const char* getName() const override { return "Custom"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    float getPlasticityFactor() const override { return plasticityFactor_; }
    void update(TimestepDuration dt) override {
        // Your neuromodulation logic
    }
};
```

## API Reference

### Core Classes

#### Brain
Main neural simulation interface:

```cpp
Brain brain(config);                    // Constructor
brain.initialize();                     // Initialize
brain.step(step);                       // Simulation step
brain.receiveSensoryInput(input);       // Receive input
brain.produceAction();                   // Get motor output
brain.save("checkpoint.bin");           // Save state
brain.load("checkpoint.bin");           // Load state
```

#### Config
Configuration management:

```cpp
Config config;                          // Create config
config.loadFromFile("config.json");      // Load from file
config.set("neuron_count", 1000);        // Set values
config.set("enable_development", true);   // Set boolean
config.set("random_seed", 42ull);        // Set unsigned int
config.getOr("neuron_count", 100);       // Get with default
config.has("neuron_count");              // Check existence
```

#### Logger
Logging system:

```cpp
Logger& logger = Logger::getGlobal();     // Get global logger
logger.info("Simulation started");         // Log info
logger.error("Something went wrong");     // Log error
NLM_LOG_INFO("Simple logging");           // Macro version
```

### Memory Systems

#### Working Memory
```cpp
auto* workingMemory = brain->getWorkingMemory();
workingMemory->setCapacity(1000);         // Set capacity
workingMemory->setDecayRate(0.01f);       // Set decay rate
workingMemory->store(pattern, 1.0f);      // Store pattern
auto retrieved = workingMemory->retrieve(); // Retrieve pattern
```

#### Episodic Memory
```cpp
auto* episodicMemory = brain->getEpisodicMemory();
auto* episode = episodicMemory->getEpisode(0); // Get episode
auto similar = episodicMemory->retrieveSimilar(pattern, 5); // Find similar
```

### Performance Features

#### Memory Pools
```cpp
// Automatic memory pool usage
// Enabled by default in Brain initialization
// Reduces memory fragmentation and allocation overhead
```

#### SIMD Vectorization
```cpp
// Automatic SIMD usage for neuron updates
// Enabled when compiler supports AVX/NEON instructions
// No explicit configuration needed
```

#### Sparse Connectivity
```cpp
// Efficient handling of sparse networks
// Automatically optimizes for connection_probability < 0.1
// Reduces memory usage significantly
```

## Research and Collaboration

### Documentation
- **Main Documentation**: `Enhanced_Documentation.md`
- **API Reference**: Generated from source code comments
- **Examples**: Various demo applications

### Research Code
- **Experiments**: `src/experiments/` directory
- **Benchmarks**: `src/benchmarks/` directory
- **Visualization**: `src/visualization/` directory

### Contributing
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

## Troubleshooting

### Common Issues

#### "Segmentation fault" or "Bus error"
- **Cause**: Memory corruption or invalid pointers
- **Solution**:
  ```bash
  # Run with memory debugger
  valgrind --tool=memcheck ./nlm
  ```

#### "Simulation too slow"
- **Cause**: Performance bottlenecks
- **Solution**:
  ```bash
  # Reduce neuron count for testing
  ./nlm --neuron_count=100
  
  # Check performance with benchmarks
  ./nlm_benchmark --neuron_count=1000
  ```

#### "Configuration file not found"
- **Cause**: Invalid file path
- **Solution**:
  ```bash
  # Use absolute path or ensure file exists
  ./nlm --config=$(pwd)/config.json
  ```

### Getting Help

#### Online Resources
- **Documentation**: `Enhanced_Documentation.md`
- **GitHub Issues**: [GitHub Repository Issues](https://github.com/nlm-project/nlm/issues)
- **Discussion Forum**: [NLM Research Forum](https://discuss.nlm.org)

#### Contact
- **Email**: research@nlm.org
- **Slack**: [Join NLM Slack](https://slack.nlm.org)
- **Twitter**: [@NLM_Research](https://twitter.com/NLM_Research)

## Examples

### Simple Brain Simulation
```cpp
#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = Config::createMinimal();
    config.set("neuron_count", 500);
    
    // Create brain
    Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Get action
        auto action = brain.produceAction();
        
        // Log status every 100 steps
        if (step % 100 == 0) {
            brain.logStatus();
        }
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}
```

### Memory Integration Example
```cpp
#include "Brain.hpp"
#include <iostream>

void demonstrateMemoryIntegration(Brain& brain) {
    // Check if memory systems are available
    if (auto* workingMemory = brain.getWorkingMemory()) {
        std::cout << "Working memory capacity: " << workingMemory->getCapacity() << std::endl;
        workingMemory->setCapacity(1000);
    }
    
    if (auto* episodicMemory = brain.getEpisodicMemory()) {
        std::cout << "Max episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        episodicMemory->enableReplay(true);
    }
    
    // Run simulation with memory usage
    for (size_t step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Store some experience in episodic memory
        if (auto* episodicMemory = brain.getEpisodicMemory() && step % 100 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f;
            episodicMemory->storeEpisode(episode);
        }
        
        // Check memory usage
        if (auto* workingMemory = brain.getWorkingMemory()) {
            size_t traces = workingMemory->getActiveTraces();
            if (traces > 0) {
                std::cout << "Step " << step << ": Working memory active traces: " << traces << std::endl;
            }
        }
    }
}
```

### Development Example
```cpp
#include "Brain.hpp"
#include "Config.hpp"

void demonstrateDevelopment(Brain& brain) {
    std::cout << "Initial developmental stage: " 
              << static_cast<int>(brain.getDevelopmentSystem()->getDevelopmentalStage()) << std::endl;
    
    // Enable development
    brain.getDevelopmentSystem()->enableDevelopment(true);
    
    // Run development simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Check developmental stage changes
        DevelopmentalStage stage = brain.getDevelopmentStage();
        if (stage != DevelopmentalStage::Initial) {
            std::cout << "Step " << step << ": Developmental stage changed to " 
                      << static_cast<int>(stage) << std::endl;
        }
        
        // Every 200 steps, force development
        if (step % 200 == 0) {
            brain.develop();
        }
    }
    
    std::cout << "Final developmental stage: " 
              << static_cast<int>(brain.getDevelopmentStage()) << std::endl;
}
```

## Conclusion

NLM (熙然) provides a comprehensive framework for creating artificial developmental brains. With its Phase 6 integration, comprehensive memory systems, and performance optimizations, NLM offers a powerful platform for neural simulation research and development.

For more information, visit the NLM website or join the research community. Together, we can create the next generation of artificial neural systems that learn, develop, and adapt like biological brains.
