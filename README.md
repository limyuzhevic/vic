## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Configuration

NLM uses configuration files for parameter control. The default configuration is located at:

- `configs/default.cfg` - Comprehensive default settings
- `docs/API_Docs.md` - Complete configuration reference and examples

Configuration can be customized through:
1. Configuration files (`.cfg` format)
2. Command-line arguments
3. Runtime API calls

### Configuration File Format

Basic key=value format (extended support coming in Phase 2):

```cfg
# Neural Parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity Settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Development Settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation Settings
dopamine_baseline = 0.1
curiosity_threshold = 0.3
novelty_decay = 0.99

# Reward and Learning
reward_discount_factor = 0.99

# Environment
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm.log
```

### Command Line Options

Most configuration options can be overridden via command line:

```bash
./nlm_phase6_demo \
    --neuron_count=500 \
    --region_count=1 \
    --connection_probability=0.1 \
    --log_level=DEBUG \
    --config=custom.cfg
```

## Python Bindings

The NLM C++ framework includes Python bindings for high-level scripting and integration:

```python
import pynlm

# Create brain configuration
config = pynlm.Config()
config.set("neuron_count", 500)
config.set("region_count", 2)

# Initialize brain
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)
    
print(f"Final state: {brain.getTotalNeuronCount()} neurons")
```

## Documentation

### Core Documentation

- `docs/API_Docs.md` - Complete API reference with examples
- `docs/ARCHITECTURE.md` - System architecture and design
- `docs/SCIENCE.md` - Scientific background and biological inspiration
- `docs/ROADMAP.md` - Project roadmap and future directions
- `docs/EXPERIMENTS.md` - Experiment framework and protocols
- `docs/LIMITATIONS.md` - Known limitations and assumptions
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration audit report

### Quick Start Guide

#### Phase 2: Basic Neural Computation
```bash
# Run basic neural computation demo
./nlm_phase2_demo

# With custom configuration
./nlm_phase2_demo --neuron_count=200 --log_level=DEBUG
```

#### Phase 3: World Interaction
```bash
./nlm_phase3_demo --enable_world=true --world_size=20
```

#### Phase 6: Full Integration
```bash
./nlm_phase6_demo
```

## Project Structure

```
NLM/
├── configs/              # Configuration files
│   └── default.cfg       # Default configuration
├── docs/                 # Documentation
│   ├── API_Docs.md       # API documentation
│   ├── ARCHITECTURE.md   # System architecture
│   ├── SCIENCE.md        # Scientific background
│   ├── ROADMAP.md        # Project roadmap
│   ├── EXPERIMENTS.md    # Experiment framework
│   ├── LIMITATIONS.md    # Known limitations
│   └── PHASE6_FINAL_AUDIT.md # Phase 6 audit
├── src/                  # Source code
│   ├── core/             # Core utilities
│   │   ├── Config/       # Configuration system
│   │   ├── Random/       # Random number generation
│   │   ├── Logger/       # Logging system
│   │   └── ...          # Other core components
│   ├── brain/            # Neural components
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   ├── plasticity/       # Plasticity rules
│   ├── neuromodulation/  # Neuromodulators
│   ├── memory/          # Memory systems
│   ├── prediction/       # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   ├── development/      # Developmental system
│   └── visualization/    # Visualization
├── tests/                # Test suite
│   ├── test_*.cpp       # Unit tests
│   └── test_config.cpp   # Configuration tests
├── python/               # Python bindings
│   └── bindings.cpp      # Pybind11 integration
└── docs/                 # Documentation
    └── ...              # Additional documentation
```

## API Usage Examples

### Basic Neural Simulation
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create and configure brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    
    Brain brain(config);
    if (!brain.initialize()) {
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
    }
    
    return 0;
}
```

### Advanced Configuration
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void runAdvancedSimulation() {
    auto config = std::make_shared<Config>();
    
    // Neural parameters
    config->set("neuron_count", 2000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.05);
    
    // Plasticity settings
    config->set("plasticity_learning_rate", 0.02);
    config->set("stdp_ltp_weight", 0.03);
    config->set("stdp_ltd_weight", 0.025);
    
    // Development
    config->set("development_synaptogenesis_rate", 0.002);
    config->set("development_pruning_rate", 0.0002);
    
    // Neuromodulation
    config->set("dopamine_baseline", 0.15);
    config->set("curiosity_threshold", 0.25);
    
    // Reward
    config->set("reward_discount_factor", 0.95);
    
    // Initialize and run
    Brain brain(config);
    brain.initialize();
    
    // Complex simulation with learning
    for (int step = 0; step < 5000; ++step) {
        // Add sensory input, run plasticity, etc.
        brain.step(step, step * 0.001);
        brain.updatePlasticity();
        brain.develop();
        
        if (step % 1000 == 0) {
            brain.logStatus();
        }
    }
}
```

## Performance and Scaling

### Memory Requirements
- **Small Scale** (< 1K neurons): < 50 MB
- **Medium Scale** (< 10K neurons): < 500 MB  
- **Large Scale** (< 100K neurons): < 5 GB

### Computational Requirements
- **1K neurons**: ~10ms per second of simulation time
- **10K neurons**: ~100ms per second of simulation time
- **100K neurons**: ~1s per second of simulation time

### Recommended Hardware
- **Desktop**: Up to 10K neurons
- **Workstation**: Up to 100K neurons
- **Cluster**: Scale beyond 100K neurons

## Troubleshooting

### Common Issues

#### "Brain initialization failed"
- **Cause**: Invalid configuration parameters
- **Solution**: Check that `neuron_count > 0`, `region_count > 0`

#### "No neurons firing"
- **Cause**: Insufficient input or incorrect neuron parameters
- **Solution**: Check thresholds, inject current, verify neuron types

#### "Memory system not working"
- **Cause**: Memory systems not properly connected
- **Solution**: Ensure `brain->getWorkingMemory()->connect(brain)` is called

### Debugging Tools

#### Logging
```bash
# Enable verbose logging
./nlm_phase6_demo --log_level=DEBUG

# Log to file
./nlm_phase6_demo --log_to_file=true --log_filename=debug.log
```

#### Brain State Inspection
```cpp
// Get detailed brain status
brain.logStatus();

// Save/load brain state
brain.save("checkpoint.nlm");
brain.load("checkpoint.nlm");
```

## Future Enhancements

### Phase 7 Goals
1. **Advanced Plasticity**: Homeostatic and reward-modulated mechanisms
2. **Enhanced Cognition**: Abstract reasoning and planning
3. **Social Learning**: Cultural transmission and imitation
4. **Meta-Learning**: Learning how to learn
5. **Transfer Learning**: Generalization across tasks

### Research Opportunities
- **Neuromodulation**: Detailed biological modeling
- **Development**: Life-cycle learning algorithms
- **Memory**: Long-term consolidation and forgetting
- **Cognition**: Goal-directed behavior and decision making
- **Social**: Multi-agent interactions and communication

## License

MIT License - See LICENSE file for details.

## Authors

NLM Research Team - See docs for scientific background.

