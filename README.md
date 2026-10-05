# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle
## Configuration Examples

### JSON Configuration File (`config.json`)

```json
{
  "neuron_count": 2000,
  "region_count": 2,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "checkpoint_dir": "./checkpoints",
  "replay_interval": 100,
  "consolidation_interval": 1000
}
```

### YAML Configuration File (`config.yaml`)

```yaml
neuron_count: 2000
region_count: 2
connection_probability: 0.15
simulation_timestep: 0.001
random_seed: 42
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
checkpoint_dir: ./checkpoints
replay_interval: 100
consolidation_interval: 1000
```

### Legacy Key=Value Configuration File (`config.cfg`)

```
# NLM Configuration File
# neuron_count = 2000
# region_count = 2
# connection_probability = 0.15
# simulation_timestep = 0.001
# random_seed = 42
# stdp_ltp_weight = 0.02
# stdp_ltd_weight = 0.015
# stdp_tau = 20.0
# synaptogenesis_rate = 0.0001
# pruning_rate = 0.00001

# Commented examples (uncomment to enable)

# neuron_count = 2000
# region_count = 2
# connection_probability = 0.15
# simulation_timestep = 0.001
# random_seed = 42
# stdp_ltp_weight = 0.02
# stdp_ltd_weight = 0.015
# stdp_tau = 20.0
# synaptogenesis_rate = 0.0001
# pruning_rate = 0.00001
# checkpoint_dir = ./checkpoints
# replay_interval = 100
# consolidation_interval = 1000
```

## Command-Line Usage Examples

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config=./myconfig.json

# Run with command-line overrides
./nlm \
  --neuron_count=5000 \
  --region_count=3 \
  --simulation_timestep=0.0005
```

### Advanced Command-Line Options

```bash
# Load from multiple sources
./nlm \
  --config=./base_config.json \
  --env=NLM_CHECKPOINT_DIR \
  --file=./override_config.json \
  --neuron_count=3000 \
  --random_seed=12345

# Use environment variables
export NLM_NEURON_COUNT=4000
export NLM_CONNECTION_PROBABILITY=0.2
./nlm --env=NLM_NEURON_COUNT --env=NLM_CONNECTION_PROBABILITY

# Configure with key descriptions
./nlm \
  --config-key=neuron_count \
  --config-description="Total number of neurons in the brain" \
  --config-key=region_count \
  --config-description="Number of neural regions to create"
```

## Advanced Configuration Features

### Environment Variables

NLM supports the following environment variables for configuration:

- `NLM_NEURON_COUNT` - Number of neurons (int)
- `NLM_REGION_COUNT` - Number of regions (int)
- `NLM_CONNECTION_PROBABILITY` - Connection probability (float)
- `NLM_SIMULATION_TIMESTEP` - Simulation timestep (float)
- `NLM_RANDOM_SEED` - Random seed (int64_t)
- `NLM_CHECKPOINT_DIR` - Checkpoint directory (string)
- `NLM_REPLAY_INTERVAL` - Replay interval (size_t)
- `NLM_CONSOLIDATION_INTERVAL` - Consolidation interval (size_t)

### Configuration Validation

The framework validates configuration parameters and provides informative error messages:

```bash
# Example of validation output
NLM_LOG_WARNING: Invalid neuron_count: 0 - using default of 1000
NLM_LOG_WARNING: Invalid region_count: 0 - using default of 1
NLM_LOG_WARNING: Invalid connection_probability: 1.5 - using default of 0.1
```

### Configuration File Formats

#### JSON Configuration

JSON configuration files support all numeric types and strings:

```json
{
  "configuration": {
    "neurons": {
      "count": 1000,
      "regions": 2,
      "type_distribution": {
        "sensory": 0.25,
        "motor": 0.25,
        "internal": 0.5
      }
    },
    "plasticity": {
      "stdp": {
        "learning_rate": 0.01,
        "time_constant": 20.0
      },
      "hebbian": {
        "learning_rate": 0.005
      }
    },
    "memory": {
      "working_capacity": 100,
      "episodic_max_episodes": 1000
    }
  },
  "metadata": {
    "version": "1.0.0",
    "author": "NLM Team",
    "description": "Advanced neural simulation configuration"
  }
}
```

#### YAML Configuration

YAML configuration files provide a more readable format:

```yaml
neurons:
  count: 1000
  regions: 2
  type_distribution:
    sensory: 0.25
    motor: 0.25
    internal: 0.5

plasticity:
  stdp:
    learning_rate: 0.01
    time_constant: 20.0
  hebbian:
    learning_rate: 0.005

memory:
  working_capacity: 100
  episodic_max_episodes: 1000

metadata:
  version: '1.0.0'
  author: NLM Team
  description: Advanced neural simulation configuration
```

## Advanced Usage Examples

### Creating Custom Brain Configurations

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create a custom configuration
auto config = std::make_shared<Config>();

// Set basic parameters
config->set("neuron_count", 5000);
config->set("region_count", 3);
config->set("connection_probability", 0.2);
config->set("simulation_timestep", 0.0005);
config->set("random_seed", 42);

// Configure plasticity rules
config->set("stdp_ltp_weight", 0.03);
config->set("stdp_ltd_weight", 0.02);
config->set("stdp_tau", 25.0);

// Configure memory systems
config->set("working_memory_capacity", 500);
config->set("episodic_max_episodes", 2000);

// Create and initialize brain
auto brain = std::make_shared<Brain>(config);
if (brain->initialize()) {
    NLM_LOG_INFO("Brain initialized successfully");
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
    }
} else {
    NLM_LOG_ERROR("Failed to initialize brain");
}
```

### Using Configuration with Command-Line Arguments

```cpp
int main(int argc, char** argv) {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load from file
    config->loadFromFile("configs/default.json");
    
    // Override with command-line arguments
    config->loadFromArgs(argc, argv);
    
    // Load from environment variables
    config->setFromEnvironmentVariable("NLM_NEURON_COUNT");
    config->setFromEnvironmentVariable("NLM_CONNECTION_PROBABILITY");
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return 1;
    }
    
    // Run simulation
    // ...
    
    return 0;
}
```

### Performance Optimization Tips

1. **Increase neuron count for more complex behavior**
```bash
./nlm --neuron_count=10000 --region_count=5
```

2. **Use checkpointing for long simulations**
```bash
./nlm --checkpoint_dir=./my_checkpoints --replay_interval=50
```

3. **Adjust plasticity parameters for specific learning behaviors**
```bash
./nlm --stdp_ltp_weight=0.05 --stdp_ltd_weight=0.03 --synaptogenesis_rate=0.0002
```

4. **Use smaller timesteps for more accurate simulation**
```bash
./nlm --simulation_timestep=0.0001 --neuron_count=2000
```

### Debugging and Monitoring

#### Enable Detailed Logging

```bash
# Enable debug logging
./nlm --config-key=log_level --config-description="Set logging level (0=Debug, 1=Info, 2=Warning, 3=Error)"
```

#### Monitor Configuration

```cpp
// Print current configuration
brain->getConfig()->printSummary();

// Validate configuration
if (brain->getConfig()->validate()) {
    NLM_LOG_INFO("Configuration validation passed");
} else {
    NLM_LOG_WARNING("Configuration validation failed");
}
```

## Extending NLM

### Adding Custom Configuration Parameters

```cpp
// Add custom configuration parameters
config->set("custom_parameter1", 123);
config->set("custom_parameter2", "string_value");
config->set("custom_parameter3", std::vector<double>{1.0, 2.0, 3.0});

// Access custom parameters
auto custom1 = config->getOr<int>("custom_parameter1", 0);
auto custom2 = config->getOr<std::string>("custom_parameter2", "default");
auto custom3 = config->getOr<std::vector<double>>("custom_parameter3", {});
```

### Adding Custom Plasticity Rules

```cpp
// Create custom plasticity rule
class CustomPlasticity {
public:
    void configure(float learningRate, float timeConstant) {
        // Configure custom plasticity
    }
    
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, Timestamp currentTime) {
        // Implement custom plasticity rule
    }
};

// Use custom plasticity in brain
auto customPlasticity = std::make_unique<CustomPlasticity>();
customPlasticity->configure(0.02f, 15.0f);
// Integrate with brain's plasticity system
```

## Troubleshooting

### Common Issues and Solutions

#### Configuration File Not Found
```bash
# Solution: Ensure the configuration file exists
ls -la ./configs/
```

#### Invalid Configuration Values
```bash
# Solution: Check for invalid values in configuration files
./nlm --config=./configs/debug.json
```

#### Brain Initialization Fails
```bash
# Solution: Check for insufficient resources or invalid configuration
# Reduce neuron count or increase available memory
./nlm --neuron_count=1000
```

#### Memory Usage Too High
```bash
# Solution: Reduce neuron count or increase system memory
./nlm --neuron_count=500
```

#### Simulation Runs Too Slow
```bash
# Solution: Use larger timesteps or reduce simulation time
./nlm --simulation_timestep=0.001
```

## Technical Reference

### Configuration Parameter Types

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| `neuron_count` | size_t | Number of neurons | 1000 |
| `region_count` | size_t | Number of regions | 1 |
| `connection_probability` | float | Connection probability (0.0-1.0) | 0.1 |
| `simulation_timestep` | double | Simulation timestep in seconds | 0.001 |
| `random_seed` | int64_t | Random seed for reproducibility | 42 |
| `stdp_ltp_weight` | float | STDP LTP weight | 0.01 |
| `stdp_ltd_weight` | float | STDP LTD weight | 0.012 |
| `stdp_tau` | float | STDP time constant | 20.0 |
| `synaptogenesis_rate` | float | Synaptogenesis rate | 0.0001 |
| `pruning_rate` | float | Pruning rate | 0.00001 |
| `checkpoint_dir` | string | Checkpoint directory | "./checkpoints" |
| `replay_interval` | size_t | Replay interval | 100 |
| `consolidation_interval` | size_t | Consolidation interval | 1000 |

### Build and Installation

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build with Python bindings
mkdir build_py
cd build_py
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j4

# Install
sudo make install
```

### Performance Metrics

The framework provides detailed performance metrics:

- Total neurons: `getTotalNeuronCount()`
- Total synapses: `getTotalSynapseCount()`
- Active neurons: `getActiveNeuronCount()`
- Firing neurons: `getFiringNeuronCount()`
- Average firing rate: `getAverageFiringRate()`
- Excitation/inhibition ratio: `getExcitationInhibitionRatio()`
- Total spikes: `getTotalSpikeCount()`
- Pending spike events: `getPendingSpikeEventCount()`

### License

MIT License - See LICENSE file for details.

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
